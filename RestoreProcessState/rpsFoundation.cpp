#include "rpsPCH.h"
#include "rpsInternal.h"

namespace {

CloseHandleT                vaCloseHandle;
HeapAllocT                  vaHeapAlloc;
HeapFreeT                   vaHeapFree;
InitializeCriticalSectionT  vaInitializeCriticalSection;
DeleteCriticalSectionT      vaDeleteCriticalSection;
EnterCriticalSectionT       vaEnterCriticalSection;
LeaveCriticalSectionT       vaLeaveCriticalSection;
TryEnterCriticalSectionT    vaTryEnterCriticalSection;

CreateFileAT                vaCreateFileA;
WriteFileT                  vaWriteFile;
ReadFileT                   vaReadFile;

} // namespace

void rpsInitializeFoundation()
{
    if(HMODULE mod = ::LoadLibraryA("kernel32.dll")) {
        (void*&)vaCloseHandle = ::GetProcAddress(mod, "CloseHandle");
        (void*&)vaHeapAlloc = ::GetProcAddress(mod, "HeapAlloc");
        (void*&)vaHeapFree = ::GetProcAddress(mod, "HeapFree");
        (void*&)vaInitializeCriticalSection = ::GetProcAddress(mod, "InitializeCriticalSection");
        (void*&)vaDeleteCriticalSection = ::GetProcAddress(mod, "DeleteCriticalSection");
        (void*&)vaEnterCriticalSection = ::GetProcAddress(mod, "EnterCriticalSection");
        (void*&)vaLeaveCriticalSection = ::GetProcAddress(mod, "LeaveCriticalSection");
        (void*&)vaTryEnterCriticalSection = ::GetProcAddress(mod, "TryEnterCriticalSection");
        (void*&)vaCreateFileA = ::GetProcAddress(mod, "CreateFileA");
        (void*&)vaWriteFile = ::GetProcAddress(mod, "WriteFile");
        (void*&)vaReadFile = ::GetProcAddress(mod, "ReadFile");
    }
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

