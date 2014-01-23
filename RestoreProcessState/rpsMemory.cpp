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

struct rpsStaticDataInfo
{
    HMODULE handle;
    void *base;
    rps_string data;
};
inline rpsArchive& operator&(rpsArchive &ar, rpsStaticDataInfo &v)
{
    ar & (size_t&)v.handle & (size_t&)v.base & v.data;
    return ar;
}


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

    // static 変数群の serialize
    // これらは module 内の書き込み可能領域にある
    std::vector<rpsStaticDataInfo, rps_allocator<rpsStaticDataInfo> > sinfo;
    if(ar.isWriter()) {
        HMODULE mod = ::GetModuleHandleA(nullptr);
        rpsEnumerateModules([&](HMODULE mod){
            char *pos = (char*)mod;
            for(;;) {
                MEMORY_BASIC_INFORMATION mi;
                if(::VirtualQuery(pos, &mi, sizeof(mi))) {
                    if( (mi.Protect & PAGE_READWRITE)!=0 ||
                        (mi.Protect & PAGE_EXECUTE_READWRITE)!=0 )
                    {
                        rpsStaticDataInfo tmp = {mod, mi.BaseAddress, rps_string((char*)mi.BaseAddress, mi.RegionSize)};
                        sinfo.push_back(tmp);
                    }
                    else if(mi.Protect==0 || (mi.Protect & PAGE_NOACCESS)!=0) {
                        break;
                    }
                    pos = (char*)mi.BaseAddress + mi.RegionSize;
                }
                else {
                    break;
                }
            }
        });
        ar & sinfo;
    }
    else if(ar.isReader()) {
        ar & sinfo;
        rpsEach(sinfo, [](rpsStaticDataInfo &sdi){
            char path[MAX_PATH+1];
            if(::GetModuleFileNameA(sdi.handle, path, sizeof(path))) {
                memcpy(sdi.base, &sdi.data[0], sdi.data.size());
            }
        });
    }
}

void* rpsMemory::alloc(size_t size)
{
    // 現状増える一方
    // todo: dlmalloc かなんか使う
    const size_t minimum_alignment = 8;
    void *ret = m_mem + m_pos;
    m_pos += size;
    m_pos = m_pos+minimum_alignment-1 & ~minimum_alignment;
    return ret;
}

void rpsMemory::free(void *addr)
{
    // todo
}

} // namespace

rpsDLLExport rpsIModule* rpsCreateMemory() { return rpsMemory::getInstance(); }
