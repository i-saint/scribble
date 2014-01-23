#include "rpsInternal.h"

//// todo
//#define USE_DL_PREFIX
//extern "C" {
//#include "malloc.h"
//};

namespace {

class rpsMemory : public rpsIModule
{
public:
    static rpsMemory* getInstance();

    rpsMemory();
    ~rpsMemory();
    virtual const char*     getModuleName() const;
    virtual size_t          getNumHooks() const;
    virtual rpsHookInfo*    getHooks() const;
    virtual void serialize(rpsArchive &ar);

    void* alloc(size_t size);
    void free(void *addr);

private:
    char *m_mem;
    size_t m_size;
    size_t m_pos;
};


HeapAllocT      vaHeapAlloc;
HeapReAllocT    vaHeapReAlloc;
HeapFreeT       vaHeapFree;
HeapValidateT   vaHeapValidate;

LPVOID WINAPI rpsHeapAlloc( HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes )
{
    return rpsMemory::getInstance()->alloc(dwBytes);
}

LPVOID WINAPI rpsHeapReAlloc( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes )
{
    return rpsMemory::getInstance()->alloc(dwBytes);
}

BOOL WINAPI rpsHeapFree( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem )
{
    rpsMemory::getInstance()->free(lpMem);
    return TRUE;
}

BOOL WINAPI rpsHeapValidate( HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem )
{
    return TRUE;
}

rpsHookInfo g_hookinfo[] = {
    rpsHookInfo("kernel32.dll", "HeapAlloc",   0, rpsHeapAlloc,   &(void*&)vaHeapAlloc),
    rpsHookInfo("kernel32.dll", "HeapReAlloc", 0, rpsHeapReAlloc, &(void*&)vaHeapReAlloc),
    rpsHookInfo("kernel32.dll", "HeapFree",    0, rpsHeapFree,    &(void*&)vaHeapFree),
    rpsHookInfo("kernel32.dll", "HeapValidate",0, rpsHeapValidate,&(void*&)vaHeapValidate),
};


const char*     rpsMemory::getModuleName() const    { return "rpsMemory"; }
size_t          rpsMemory::getNumHooks() const      { return _countof(g_hookinfo); }
rpsHookInfo*    rpsMemory::getHooks() const         { return g_hookinfo; }

rpsMemory* rpsMemory::getInstance()
{
    static rpsMemory *s_inst = new rpsMemory();
    return s_inst;
}

rpsMemory::rpsMemory()
    : m_mem(nullptr)
    , m_size(0)
    , m_pos(0)
{
    // 適当
    m_size = 0x10000000;
    void *addr = 
#if defined(_M_IX86)
        (void*)0x60000000;
#elif defined(_M_X64)
        (void*)0x60000000;
#endif 
    m_mem = (char*)::VirtualAlloc(addr, m_size, MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);
}

rpsMemory::~rpsMemory()
{
    // 意図的に開放しない
}

void rpsMemory::serialize(rpsArchive &ar)
{
    ar & m_size & m_pos;
    ar.io(m_mem, m_pos);
}

void* rpsMemory::alloc(size_t size)
{
    // todo: dlmalloc かなんか使う
    const size_t minimum_alignment = 8;
    void *ret = m_mem + m_pos;
    m_pos += size;
    m_pos = m_pos+minimum_alignment-1 & ~minimum_alignment;
    return ret;
}

void rpsMemory::free(void *addr)
{
}

} // namespace

rpsDLLExport rpsIModule* rpsCreateMemory() { return rpsMemory::getInstance(); }
