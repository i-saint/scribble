#include <windows.h>
#include <psapi.h>
#include <dbghelp.h>
#include <tlhelp32.h>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <thread>
#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "psapi.lib")

// 指定の関数の先頭に現在のスレッドを suspend するコードをねじ込む
bool SetThreadTrap(HANDLE process, void *target); // target: hotpatch 可能な関数のアドレス
bool SetThreadTrap(HANDLE process, const char *sym_name); // sym_name: hotpatch 可能な関数のシンボル名

// SetThreadTrap() でねじ込んだ suspend コードを解除
bool UnsetThreadTrap(HANDLE process, void *target);
bool UnsetThreadTrap(HANDLE process, const char *sym_name);


void resumer(DWORD tid)
{
    ::Sleep(100);

    printf("resume_thread: wake\n");
    if(HANDLE thread = ::OpenThread(THREAD_ALL_ACCESS, FALSE, tid)) {
        ::ResumeThread(thread);
        ::CloseHandle(thread);
    }

    ::Sleep(100);
    printf("resume_thread: end\n");
}

int main(int argc, char* argv[])
{
    SetThreadTrap(::GetCurrentProcess(), "LoadLibraryA"); // LoadLibraryA に suspend を仕込む

    {
        // メインスレッドを起こすスレッドを実行開始
        DWORD tid = ::GetCurrentThreadId();
        std::thread resume_thread([=](){ resumer(tid); });

        HANDLE kernel32 = LoadLibraryA("kernel32.dll"); // LoadLibraryA() を呼ぶ (suspend に入る)
        printf("kernel32: 0x%08p\n", kernel32); // resume_thread がメインスレッドを起こした後ここに来る

        resume_thread.join();
    }

    UnsetThreadTrap(::GetCurrentProcess(), "LoadLibraryA"); // suspend コード除去

    {
        HANDLE kernel32 = LoadLibraryA("kernel32.dll"); // 今度は止まらない
        printf("kernel32: 0x%08p\n", kernel32);
    }
}

/*
$ cl SetThreadTrap.cpp /EHsc && ./SetThreadTrap

resume_thread: wake
kernel32: 0x77490000
resume_thread: end
kernel32: 0x77490000
*/




// F: [](HMODULE mod)->void
template<class F>
inline void EnumerateModules(HANDLE process, const F &f)
{
    std::vector<HMODULE> modules;
    DWORD num_modules = 0;
    ::EnumProcessModules(process, nullptr, 0, &num_modules);
    modules.resize(num_modules/sizeof(HMODULE));
    ::EnumProcessModules(process, &modules[0], num_modules, &num_modules);
    for(size_t i=0; i<modules.size(); ++i) {
        f(modules[i]);
    }
}

// F: [](DWORD thread_id)->void
template<class F>
inline void EnumerateThreads(DWORD pid, const F &f)
{
    HANDLE ss = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if(ss!=INVALID_HANDLE_VALUE) {
        THREADENTRY32 te;
        te.dwSize = sizeof(te);
        if(::Thread32First(ss, &te)) {
            do {
                if(te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID)+sizeof(te.th32OwnerProcessID) &&
                    te.th32OwnerProcessID==pid)
                {
                    f(te.th32ThreadID);
                }
                te.dwSize = sizeof(te);
            } while(::Thread32Next(ss, &te));
        }
        ::CloseHandle(ss);
    }
}


/*
// trap の元ソース

__forceinline void TrapBody()
{
    typedef HMODULE (__stdcall *tOpenThread)(DWORD, BOOL, DWORD);
    typedef DWORD (__stdcall *tSuspendThread)(HANDLE);
    tOpenThread pOpenThread = (tOpenThread)0x80000000;          // 0x80000000
    tSuspendThread pSuspendThread = (tSuspendThread)0x80000001; // 0x80000001 注入時に正しいアドレスに書き換える
    DWORD tid = __readfsdword(0x24);
    HANDLE tread = pOpenThread(THREAD_ALL_ACCESS, FALSE, tid);
    pSuspendThread(tread);
    *(BYTE*)0x90000000 = 0x80;                                  // 0x90000000, 0x80
    *(BYTE*)0x90000001 = 0x81;                                  // 0x90000001, 0x81 注入時に正しいアドレスに書き換える
}
__declspec(naked) void Trap()
{
    __asm {
        push eax
        push ecx
        push edx
        push ebx
        push ebp
        push esi
        push edi
    }
    TrapBody();
    __asm {
        pop edi
        pop esi
        pop ebp
        pop ebx
        pop edx
        pop ecx
        pop eax
        ret
    }
}
*/

static void* FollowJmp(HANDLE process, void *target)
{
    BYTE t[6];
    ::ReadProcessMemory(process, target, t, 6, nullptr);
    if(t[0]==0xFF && t[1]==0x25) {
        ::ReadProcessMemory(process, (void*&)(t[2]), &target, 4, nullptr);
    }
    return target;
}

static bool SetThreadTrapImpl(HANDLE process, void *_target)
{
#ifdef _M_IX64

    // todo:

#else  // _M_IX64

    BYTE *target = (BYTE*)FollowJmp(process, _target);

    DWORD old = 0;
    BYTE *trapcode = nullptr;
    ::VirtualProtectEx(process, target-5, 7, PAGE_EXECUTE_READWRITE, &old);
    for(int i=0; !trapcode; ++i) {
        trapcode = (BYTE*)::VirtualAllocEx(process, target-(1024*64*(i+1)), 128, MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    }
    if(!trapcode) { return false; }

    {
        HMODULE kernel32 = ::LoadLibraryA("kernel32.dll");
        size_t gap = 0;
        EnumerateModules(process, [&](HMODULE mod){
            char path[MAX_PATH];
            ::GetModuleFileNameExA(process, mod, path, MAX_PATH);
            if(strstr(path, "kernel32.dll")) {
                gap = (size_t)mod - (size_t)kernel32;
            }
        });
        size_t gOpenThread = (size_t)::GetProcAddress(kernel32, "OpenThread") + gap;
        size_t gSuspendThread = (size_t)::GetProcAddress(kernel32, "SuspendThread") + gap;
        BYTE head[2];
        ::ReadProcessMemory(process, target, head, 2, nullptr);

        BYTE code[] = "\x50\x51\x52\x53\x55\x56\x57\x64\xA1\x24\x00\x00\x00\x50\x6A\x00\x68\xFF\xFF\x1F\x00\xB8\x00\x00\x00\x80\xFF\xD0\x50\xB8\x01\x00\x00\x80\xFF\xD0\x66\xC7\x05\x00\x00\x00\x90\x80\x81\x5F\x5E\x5D\x5B\x5A\x59\x58\xC3";
        (size_t&)code[0x16] = gOpenThread;
        (size_t&)code[0x1e] = gSuspendThread;
        (size_t&)code[0x27] = (size_t)target;
        code[0x2b] = head[0];
        code[0x2c] = head[1];
        ::WriteProcessMemory(process, trapcode, code, sizeof(code), nullptr);
        ::FlushInstructionCache(process, trapcode, sizeof(code));
    }
    {

        BYTE patch[7];
        patch[0] = 0xE8; // call
        (DWORD&)patch[1] = (ptrdiff_t)trapcode-(ptrdiff_t)target;
        patch[5] = 0xEB; // short jmp -7
        patch[6] = 0xF9; // 
        ::WriteProcessMemory(process, target-5, patch, _countof(patch), nullptr);
        ::FlushInstructionCache(process, target-5, 7);
    }
    return true;

#endif // _M_IX64
}

static bool UnsetThreadTrapImpl(HANDLE process, void *_target)
{
#ifdef _M_IX64

    // todo:

#else  // _M_IX64

    BYTE *target = (BYTE*)FollowJmp(process, _target);
    BYTE sjmp[7];
    ::ReadProcessMemory(process, target-5, sjmp, 7, nullptr);
    if(sjmp[5]!=0xEB && sjmp[6]!=0xF9) {
        return false;
    }

    BYTE *trapcode = target + (size_t&)sjmp[1];
    BYTE t[2];
    ::ReadProcessMemory(process, trapcode+0x2b, t, 2, nullptr);
    ::WriteProcessMemory(process, target, t, 2, nullptr);

    // trapcode の中に制御がいる可能性があるため、開放はしない
    return true;

#endif // _M_IX64
}

template<class F>
static inline void SuspendThreadBlock(HANDLE process, const F &f)
{
    std::vector<HANDLE> threads;
    DWORD pid = ::GetProcessId(process);
    EnumerateThreads(pid, [&](DWORD tid){
        if(tid==GetCurrentThreadId()) { return; }
        if(HANDLE thread=::OpenThread(THREAD_ALL_ACCESS, FALSE, tid)) {
            ::SuspendThread(thread);
            threads.push_back(thread);
        }
    });
    f();
    std::for_each(threads.begin(), threads.end(), [](HANDLE thread){
        ::ResumeThread(thread);
        ::CloseHandle(thread);
    });
}

bool SetThreadTrap(HANDLE process, void *target)
{
    bool ret = false;
    SuspendThreadBlock(process, [&](){
        ret = SetThreadTrapImpl(process, target);
    });
    return ret;
}

bool SetThreadTrap(HANDLE process, const char *sym_name)
{
    bool ret = false;
    ::SymInitialize(process, nullptr, TRUE);
    SuspendThreadBlock(process, [&](){
        char buf[sizeof(SYMBOL_INFO)+MAX_SYM_NAME];
        PSYMBOL_INFO sinfo = (PSYMBOL_INFO)buf;
        sinfo->SizeOfStruct = sizeof(SYMBOL_INFO);
        sinfo->MaxNameLen = MAX_SYM_NAME;
        if(::SymFromName(process, sym_name, sinfo)) {
            ret = SetThreadTrapImpl(process, (void*)sinfo->Address);
        }
    });
    return ret;
}


bool UnsetThreadTrap(HANDLE process, void *target)
{
    bool ret = false;
    SuspendThreadBlock(process, [&](){
        ret = UnsetThreadTrapImpl(process, target);
    });
    return ret;
}

bool UnsetThreadTrap(HANDLE process, const char *sym_name)
{
    bool ret = false;
    SuspendThreadBlock(process, [&](){
        char buf[sizeof(SYMBOL_INFO)+MAX_SYM_NAME];
        PSYMBOL_INFO sinfo = (PSYMBOL_INFO)buf;
        sinfo->SizeOfStruct = sizeof(SYMBOL_INFO);
        sinfo->MaxNameLen = MAX_SYM_NAME;
        if(::SymFromName(process, sym_name, sinfo)) {
            ret = UnsetThreadTrapImpl(process, (void*)sinfo->Address);
        }
    });
    return ret;
}

