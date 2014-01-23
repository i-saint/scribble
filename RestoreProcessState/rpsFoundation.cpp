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

