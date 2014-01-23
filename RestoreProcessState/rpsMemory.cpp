#include "rps.h"
#include "rpsInlines.h"

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

typedef LPVOID (WINAPI *HeapAllocT)( HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes );
typedef LPVOID (WINAPI *HeapReAllocT)( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes );
typedef BOOL (WINAPI *HeapFreeT)( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem );
typedef BOOL (WINAPI *HeapValidateT)( HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem );

HeapAllocT      origHeapAlloc   = nullptr;
HeapReAllocT    origHeapReAlloc = nullptr;
HeapFreeT       origHeapFree    = nullptr;
HeapValidateT   origHeapValidate= nullptr;

void rpsInitializeMalloc()
{
    if(!origHeapAlloc) {
        if(HMODULE mod = ::LoadLibraryA("kernel32.dll")) {
            (void*&)origHeapAlloc = ::GetProcAddress(mod, "HeapAlloc");
            (void*&)origHeapFree = ::GetProcAddress(mod, "HeapFree");
        }
    }
}

void* rpsMalloc(size_t s)
{
    return origHeapAlloc((HANDLE)_get_heap_handle(), 0, s);
}

void  rpsFree(void *p)
{
    origHeapFree((HANDLE)_get_heap_handle(), 0, p);
}

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

static rpsHookInfo g_rps_hooks[] = {
    rpsHookInfo("kernel32.dll", "HeapAlloc",   0, rpsHeapAlloc,   &(void*&)origHeapAlloc),
    rpsHookInfo("kernel32.dll", "HeapReAlloc", 0, rpsHeapReAlloc, &(void*&)origHeapReAlloc),
    rpsHookInfo("kernel32.dll", "HeapFree",    0, rpsHeapFree,    &(void*&)origHeapFree),
    rpsHookInfo("kernel32.dll", "HeapValidate",0, rpsHeapValidate,&(void*&)origHeapValidate),
};


const char*     rpsMemory::getModuleName() const    { return "rpsMemory"; }
size_t          rpsMemory::getNumHooks() const      { return _countof(g_rps_hooks); }
rpsHookInfo*    rpsMemory::getHooks() const         { return g_rps_hooks; }

rpsMemory* rpsMemory::getInstance()
{
    static rpsMemory *s_inst = nullptr;
    if(!s_inst) { s_inst = new rpsMemory(); }
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

rpsIModule* rpsCreateMemory() { return rpsMemory::getInstance(); }
