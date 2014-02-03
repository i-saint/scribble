#include "rpsPCH.h"
#include "rpsInternal.h"

namespace {

HeapAllocT                  vaHeapAlloc;
HeapFreeT                   vaHeapFree;
CreateThreadT               vaCreateThread;
InitializeCriticalSectionT  vaInitializeCriticalSection;
DeleteCriticalSectionT      vaDeleteCriticalSection;
EnterCriticalSectionT       vaEnterCriticalSection;
LeaveCriticalSectionT       vaLeaveCriticalSection;
TryEnterCriticalSectionT    vaTryEnterCriticalSection;

CreateFileAT                vaCreateFileA;
WriteFileT                  vaWriteFile;
ReadFileT                   vaReadFile;

OpenThreadT                 vaOpenThread;
GetThreadTimesT             vaGetThreadTimes;

CloseHandleT                vaCloseHandle;
} // namespace

void rpsInitializeFoundation()
{
#define GetProc(FuncName) (void*&)va##FuncName = ::GetProcAddress(mod, #FuncName)

    if(HMODULE mod = ::LoadLibraryA("kernel32.dll")) {
        GetProc(CloseHandle);
        GetProc(HeapAlloc);
        GetProc(HeapFree);
        GetProc(CreateThread);
        GetProc(InitializeCriticalSection);
        GetProc(DeleteCriticalSection);
        GetProc(EnterCriticalSection);
        GetProc(LeaveCriticalSection);
        GetProc(TryEnterCriticalSection);

        GetProc(CreateFileA);
        GetProc(WriteFile);
        GetProc(ReadFile);

        GetProc(OpenThread);
        GetProc(GetThreadTimes);
        GetProc(CloseHandle);
    }

#undef GetProc
}

template<size_t N>
inline int rpsVSprintf(char (&buf)[N], const char *format, va_list vl)
{
    return _vsnprintf(buf, N, format, vl);
}
template<size_t N>
inline int rpsVWSprintf(wchar_t (&buf)[N], const wchar_t *format, va_list vl)
{
    return _vsnwprintf(buf, N, format, vl);
}
void rpsPrintV(const char* fmt, va_list vl)
{
    char buf[4096];
    rpsVSprintf(buf, fmt, vl);
    ::OutputDebugStringA(buf);
}
void rpsPrintVW(const wchar_t* fmt, va_list vl)
{
    wchar_t buf[4096];
    rpsVWSprintf(buf, fmt, vl);
    ::OutputDebugStringW(buf);
}
void rpsPrint(const char* fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    rpsPrintV(fmt, vl);
    va_end(vl);
}
void rpsPrint(const wchar_t *fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    rpsPrintVW(fmt, vl);
    va_end(vl);
}


void* rpsMalloc(size_t s)
{
    return vaHeapAlloc((HANDLE)_get_heap_handle(), 0, s);
}

void  rpsFree(void *p)
{
    vaHeapFree((HANDLE)_get_heap_handle(), 0, p);
}

bool rpsIsValidMemory(void *p)
{
    if(p==nullptr) { return false; }
    MEMORY_BASIC_INFORMATION meminfo;
    return ::VirtualQuery(p, &meminfo, sizeof(meminfo))!=0 && meminfo.State!=MEM_FREE;
}

bool rpsIsInsideRpsModule( void *p )
{
    static rpsModuleInfo s_minfo = {0};
    if(!s_minfo.base) {
        rpsEnumerateModulesDetailed([&](rpsModuleInfo &mi){
            size_t addr = (size_t)&rpsIsInsideRpsModule;
            size_t mod_base = (size_t)mi.base;
            size_t mod_size = (size_t)mi.size;
            if(addr>=mod_base && addr<mod_base+mod_size) {
                s_minfo = mi;
            }
        });
    }
    size_t addr = (size_t)p;
    size_t mod_base = (size_t)s_minfo.base;
    size_t mod_size = (size_t)s_minfo.size;
    return addr>=mod_base && addr<mod_base+mod_size;
}

BYTE* rpsAddJumpInstruction(BYTE* from, const BYTE* to)
{
    // 距離が 32bit に収まる範囲であれば、0xe9 RVA
    // そうでない場合、0xff 0x25 [メモリアドレス] + 対象アドレス
    // の形式で jmp する必要がある。
    BYTE* jump_from = from + 5;
    size_t distance = jump_from > to ? jump_from - to : to - jump_from;
    if (distance <= 0x7fff0000) {
        from[0] = 0xe9;
        from += 1;
        *((DWORD*)from) = (DWORD)(to - jump_from);
        from += 4;
    }
    else {
        from[0] = 0xff;
        from[1] = 0x25;
        from += 2;
#ifdef _M_IX86
        *((DWORD*)from) = (DWORD)(from + 4);
#elif defined(_M_X64)
        *((DWORD*)from) = (DWORD)0;
#endif
        from += 4;
        *((DWORD_PTR*)from) = (DWORD_PTR)(to);
        from += 8;
    }
    return from;
}

void* rpsOverrideDLLExport(HMODULE module, const char *funcname, void *hook_, void *trampoline_space)
{
    if(!rpsIsValidMemory(module)) { return nullptr; }

    HANDLE proc = ::GetCurrentProcess();

    size_t ImageBase = (size_t)module;
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)ImageBase;
    if(pDosHeader->e_magic!=IMAGE_DOS_SIGNATURE) { return nullptr; }

    PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)(ImageBase + pDosHeader->e_lfanew);
    DWORD RVAExports = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    if(RVAExports==0) { return nullptr; }

    IMAGE_EXPORT_DIRECTORY *pExportDirectory = (IMAGE_EXPORT_DIRECTORY *)(ImageBase + RVAExports);
    DWORD *RVANames = (DWORD*)(ImageBase+pExportDirectory->AddressOfNames);
    WORD *RVANameOrdinals = (WORD*)(ImageBase+pExportDirectory->AddressOfNameOrdinals);
    DWORD *RVAFunctions = (DWORD*)(ImageBase+pExportDirectory->AddressOfFunctions);
    for(DWORD i=0; i<pExportDirectory->NumberOfFunctions; ++i) {
        char *pName = (char*)(ImageBase+RVANames[i]);
        if(strcmp(pName, funcname)==0) {
            BYTE *hook = (BYTE*)hook_;
            BYTE *target = (BYTE*)(ImageBase+RVAFunctions[RVANameOrdinals[i]]);
            if(trampoline_space) {
                BYTE *trampoline = (BYTE*)trampoline_space;
                rpsAddJumpInstruction(trampoline, hook);
                ::FlushInstructionCache(proc, trampoline, 32);
                rpsForceWrite<DWORD>(RVAFunctions[RVANameOrdinals[i]], (DWORD)(trampoline - ImageBase));
            }
            else {
                rpsForceWrite<DWORD>(RVAFunctions[RVANameOrdinals[i]], (DWORD)(hook - ImageBase));
            }
            return target;
        }
    }
    return nullptr;
}


DWORD __stdcall rpsRunThread_(LPVOID proc_)
{
    typedef std::function<void ()> functor;
    auto *proc = (functor*)proc_;
    (*proc)();
    proc->~functor();
    rpsFree(proc);
    return 0;
}

void rpsRunThread(const std::function<void ()> &proc)
{
    typedef std::function<void ()> functor;
    functor *fp = new (rpsMalloc(sizeof(functor))) functor(proc);
    vaCreateThread(nullptr, 0, &rpsRunThread_, fp, 0, nullptr);
}

DWORD rpsGetMainThreadID()
{
    static DWORD ret = 0;
    uint64_t oldest;
    if(ret==0) {
        rpsEnumerateThreads([&](DWORD tid){
            if(HANDLE thandle=vaOpenThread(THREAD_ALL_ACCESS, FALSE, tid)) {
                FILETIME ctime, etime, ktime, utime;
                vaGetThreadTimes(thandle, &ctime, &etime, &ktime, &utime);
                if(ret==0 || (uint64_t&)ctime < oldest) {
                    ret = tid;
                    oldest = (uint64_t&)ctime;
                }
                vaCloseHandle(thandle);
            }
        });
    }
    return ret;
}


bool rpsFileToString( const char *path, rps_string &str )
{
    HANDLE fin = vaCreateFileA(path, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if(!fin) { return false; }

    char buf[1024];
    DWORD read = 0;
    for(; read!=0; vaReadFile(fin, buf, sizeof(buf), &read, nullptr)) {
        str.insert(str.end(), buf, buf+read);
    }
    vaCloseHandle(fin);
    return true;
}



rpsArchive::rpsArchive()
    : m_file(nullptr)
    , m_mode(Writer)
{
}

rpsArchive::~rpsArchive()
{
    close();
}

void rpsArchive::read(void *dst, size_t size)
{
    DWORD read;
    vaReadFile(m_file, dst, (DWORD)size, &read, nullptr);
}

void rpsArchive::write(const void *data, size_t size)
{
    DWORD written;
    vaWriteFile(m_file, data, (DWORD)size, &written, nullptr);
}

void rpsArchive::skip(size_t size)
{
    void *tmp = rpsMalloc(size);
    io(tmp, size);
    rpsFree(tmp);
}

void rpsArchive::io(void *dst, size_t size)
{
    if(isReader()) {
        read(dst, size);
    }
    else {
        write(dst, size);
    }
}

bool rpsArchive::open(const char *path_to_file, Mode mode)
{
    close();
    m_mode = mode;

    DWORD accrss = m_mode==Reader ? GENERIC_READ : GENERIC_WRITE;
    DWORD disposition = m_mode==Reader ? OPEN_EXISTING : CREATE_ALWAYS;
    m_file = vaCreateFileA(path_to_file, accrss, 0, nullptr, disposition, FILE_ATTRIBUTE_NORMAL, nullptr);
    return m_file!=nullptr;
}

void rpsArchive::close()
{
    if(m_file) {
        vaCloseHandle(m_file);
    }
}


rpsMutex::rpsMutex()      { vaInitializeCriticalSection(&m_lockobj); }
rpsMutex::~rpsMutex()     { vaDeleteCriticalSection(&m_lockobj); }
void rpsMutex::lock()     { vaEnterCriticalSection(&m_lockobj); }
bool rpsMutex::tryLock()  { return vaTryEnterCriticalSection(&m_lockobj)==TRUE; }
void rpsMutex::unlock()   { vaLeaveCriticalSection(&m_lockobj); }

