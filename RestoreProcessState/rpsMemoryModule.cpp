#include "rpsPCH.h"
#include "rpsInternal.h"

//// todo
//#define USE_DL_PREFIX
//extern "C" {
//#include "malloc.h"
//};

namespace {

class rpsMemoryModule : public rpsIModule
{
public:
    static rpsMemoryModule* getInstance();

    rpsMemoryModule();
    ~rpsMemoryModule();
    virtual const char*     getModuleName() const;
    virtual rpsHookInfo*    getHooks() const;
    virtual void serialize(rpsArchive &ar);

    void* alloc(size_t size);
    void free(void *addr);
    size_t getSize(const void *addr) const;

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
HeapSizeT       vaHeapSize;

LPVOID WINAPI rpsHeapAlloc( HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes )
{
    return rpsMemoryModule::getInstance()->alloc(dwBytes);
}

LPVOID WINAPI rpsHeapReAlloc( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes )
{
    return rpsMemoryModule::getInstance()->alloc(dwBytes);
}

BOOL WINAPI rpsHeapFree( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem )
{
    rpsMemoryModule::getInstance()->free(lpMem);
    return TRUE;
}

BOOL WINAPI rpsHeapValidate( HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem )
{
    return TRUE;
}

BOOL WINAPI rpsHeapSize( HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem )
{
    return rpsMemoryModule::getInstance()->getSize(lpMem);
}

rpsHookInfo g_hookinfo[] = {
    rpsHookInfo("kernel32.dll", "HeapAlloc",   0, rpsHeapAlloc,   &(void*&)vaHeapAlloc),
    rpsHookInfo("kernel32.dll", "HeapReAlloc", 0, rpsHeapReAlloc, &(void*&)vaHeapReAlloc),
    rpsHookInfo("kernel32.dll", "HeapFree",    0, rpsHeapFree,    &(void*&)vaHeapFree),
    rpsHookInfo("kernel32.dll", "HeapValidate",0, rpsHeapValidate,&(void*&)vaHeapValidate),
    rpsHookInfo("kernel32.dll", "HeapSize",    0, rpsHeapSize,    &(void*&)vaHeapSize),

    rpsHookInfo(nullptr, nullptr, 0, nullptr, nullptr),
};


const char*     rpsMemoryModule::getModuleName() const    { return "rpsMemoryModule"; }
rpsHookInfo*    rpsMemoryModule::getHooks() const         { return g_hookinfo; }

rpsMemoryModule* rpsMemoryModule::getInstance()
{
    static rpsMemoryModule *s_inst = new rpsMemoryModule();
    return s_inst;
}

rpsMemoryModule::rpsMemoryModule()
    : m_mem(nullptr)
    , m_size(0)
    , m_pos(0)
{
    // 適当
#if defined(_M_IX86)
    m_size = 0x10000000;
#elif defined(_M_X64)
    m_size = 0x40000000;
#endif 
	void *addr = (void*)0x60000000;
    m_mem = (char*)::VirtualAlloc(addr, m_size, MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);
}

rpsMemoryModule::~rpsMemoryModule()
{
    // 意図的に開放しない
}

void rpsMemoryModule::serialize(rpsArchive &ar)
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

void* rpsMemoryModule::alloc(size_t size)
{
    // 現状増える一方
    // todo: dlmalloc かなんか使う
    const size_t alignment_mask = 8-1;
    void *ret = m_mem + m_pos;
    m_pos += size;
    m_pos = m_pos+alignment_mask & ~alignment_mask;
    return ret;
}

void rpsMemoryModule::free(void *addr)
{
    // todo
}

size_t rpsMemoryModule::getSize( const void *addr ) const
{
    return m_size;
}

} // namespace

rpsDLLExport rpsIModule* rpsCreateMemoryModule() { return rpsMemoryModule::getInstance(); }
