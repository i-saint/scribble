#include "rpsPCH.h"
#include "rpsInternal.h"

extern "C" {
#include "malloc.c"
};

namespace {

struct rpsModuleMemoryInfo
{
    void *base;
    size_t size;
};

struct rpsMemoryPageInfo
{
    void *base;
    size_t size;
    DWORD type;
    DWORD protect;
};


class rpsMemoryModule : public rpsIModule
{
public:
    static rpsMemoryModule* getInstance();

    rpsMemoryModule();
    ~rpsMemoryModule();
    virtual const char*     getModuleName() const;
    virtual rpsHookInfo*    getHooks() const;
    virtual void initialize();
    virtual void serialize(rpsArchive &ar);
    virtual void handleMessage(rpsMessage &m);

    void* getHeapBlock() const { return m_mem; }
    void setMemorySize(size_t size) { m_size=size; }

    LPVOID  rpsHeapAllocImpl(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes);
    LPVOID  rpsHeapReAllocImpl(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes);
    BOOL    rpsHeapFreeImpl(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem);
    BOOL    rpsHeapValidateImpl(HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem);
    SIZE_T  rpsHeapSizeImpl(HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem);
    LPVOID  rpsVirtualAllocImpl(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);
    BOOL    rpsVirtualFreeImpl(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType);

private:
    typedef std::map<size_t, rpsMemoryPageInfo, std::less<size_t>, rps_allocator< std::pair<size_t, rpsMemoryPageInfo> > > MemoryPages;

    rpsMutex m_mutex;
    char *m_mem;
    size_t m_size;
    size_t m_pos;
    mspace m_msp;
    MemoryPages m_pages;
};

inline bool rpsIsValidMemory(void *p)
{
    MEMORY_BASIC_INFORMATION mi;
    if(::VirtualQuery(p, &mi, sizeof(mi))) {
        if( mi.State==MEM_COMMIT )
        {
            return true;
        }
    }
    return false;
}

inline bool rpsIsWritableMemory(void *p)
{
    MEMORY_BASIC_INFORMATION mi;
    if(::VirtualQuery(p, &mi, sizeof(mi))) {
        if( mi.State==MEM_COMMIT && 
            ((mi.Protect & PAGE_READWRITE)!=0 || (mi.Protect & PAGE_EXECUTE_READWRITE)!=0) )
        {
            return true;
        }
    }
    return false;
}



HeapAllocT      vaHeapAlloc;
HeapReAllocT    vaHeapReAlloc;
HeapFreeT       vaHeapFree;
HeapValidateT   vaHeapValidate;
HeapSizeT       vaHeapSize;
VirtualAllocT   vaVirtualAlloc;
VirtualFreeT    vaVirtualFree;
VirtualAllocExT vaVirtualAllocEx;
VirtualFreeExT  vaVirtualFreeEx;


rpsHookAPI LPVOID WINAPI rpsHeapAlloc( HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes )
{
    return rpsMemoryModule::getInstance()->rpsHeapAllocImpl(hHeap, dwFlags, dwBytes);
}

rpsHookAPI LPVOID WINAPI rpsHeapReAlloc( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes )
{
    return rpsMemoryModule::getInstance()->rpsHeapReAllocImpl(hHeap, dwFlags, lpMem, dwBytes);
}

rpsHookAPI BOOL WINAPI rpsHeapFree( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem )
{
    return rpsMemoryModule::getInstance()->rpsHeapFreeImpl(hHeap, dwFlags, lpMem);
}

rpsHookAPI BOOL WINAPI rpsHeapValidate( HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem )
{
    return rpsMemoryModule::getInstance()->rpsHeapValidateImpl(hHeap, dwFlags, lpMem);
}

rpsHookAPI SIZE_T WINAPI rpsHeapSize( HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem )
{
    return rpsMemoryModule::getInstance()->rpsHeapSizeImpl(hHeap, dwFlags, lpMem);
}


rpsHookAPI LPVOID WINAPI rpsVirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect)
{
    return rpsMemoryModule::getInstance()->rpsVirtualAllocImpl(lpAddress, dwSize, flAllocationType, flProtect);
}

rpsHookAPI BOOL WINAPI rpsVirtualFree(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType)
{
    return rpsMemoryModule::getInstance()->rpsVirtualFreeImpl(lpAddress, dwSize, dwFreeType);
}

rpsHookAPI LPVOID WINAPI rpsVirtualAllocEx(HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect)
{
    LPVOID ret = vaVirtualAllocEx(hProcess, lpAddress, dwSize, flAllocationType, flProtect);
    return ret;
}

rpsHookAPI BOOL WINAPI rpsVirtualFreeEx(HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType)
{
    BOOL ret = vaVirtualFreeEx(hProcess, lpAddress, dwSize, dwFreeType);
    return ret;
}

rpsHookInfo g_hookinfo[] = {
    rpsDefineHookInfo("kernel32.dll", HeapAlloc     ),
    rpsDefineHookInfo("kernel32.dll", HeapReAlloc   ),
    rpsDefineHookInfo("kernel32.dll", HeapFree      ),
    rpsDefineHookInfo("kernel32.dll", HeapValidate  ),
    rpsDefineHookInfo("kernel32.dll", HeapSize      ),

    rpsDefineHookInfo("kernel32.dll", VirtualAlloc  ),
    rpsDefineHookInfo("kernel32.dll", VirtualFree   ),
    rpsDefineHookInfo("kernel32.dll", VirtualAllocEx),
    rpsDefineHookInfo("kernel32.dll", VirtualFreeEx ),

    rpsHookInfo(nullptr, nullptr, 0, nullptr, nullptr),
};


inline rpsArchive& operator&(rpsArchive &ar, rpsModuleMemoryInfo &v)
{
    ar & (size_t&)v.base & v.size;
    if(ar.isWriter()) {
        ar.io(v.base, v.size);
    }
    else if(ar.isReader()) {
        if(rpsIsWritableMemory(v.base)) {
            ar.io(v.base, v.size);
        }
        else {
            ar.skip(v.size);
        }
    }
    return ar;
}

inline rpsArchive& operator&(rpsArchive &ar, rpsMemoryPageInfo &v)
{
    ar & (size_t&)v.base & v.size & v.type & v.protect;
    if(ar.isWriter()) {
        ar.io(v.base, v.size);
    }
    else if(ar.isReader()) {
        DWORD prot;
        void *p = vaVirtualAlloc(v.base, v.size, v.type, PAGE_EXECUTE_READWRITE);
        ar.io(v.base, v.size);
        ::VirtualProtect(v.base, v.size, v.protect, &prot);
    }
    return ar;
}


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
    m_size = 0x100000000;
#endif 
}

rpsMemoryModule::~rpsMemoryModule()
{
    // 意図的に開放しない
}

void rpsMemoryModule::initialize()
{
    void *addr = (void*)nullptr;
    for(; !m_mem; m_size/=2) {
        m_mem = (char*)::VirtualAlloc(addr, m_size, MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    }
    m_msp = create_mspace_with_base(m_mem, m_size, 1);
    m_pos = 1024;
}

void rpsMemoryModule::serialize(rpsArchive &ar)
{
    ar & m_size & m_pos;
    ar.io(m_mem, m_pos);

    // static 変数群の serialize
    // これらは module 内の書き込み可能領域にある
    std::vector<rpsModuleMemoryInfo, rps_allocator<rpsModuleMemoryInfo> > sinfo;
    if(ar.isWriter()) {
        HMODULE mod = ::GetModuleHandleA(nullptr);
        rpsEnumerateModulesDetailed([&](rpsModuleInfo &modinfo){
            // 単純化のためメインモジュールに限定
            if(modinfo.base!=mod) { return; }

            char *pos = (char*)modinfo.base;
            char *end = pos + modinfo.size;
            for(; pos<end; ) {
                MEMORY_BASIC_INFORMATION mi;
                if(::VirtualQuery(pos, &mi, sizeof(mi))) {
                    if( (mi.Protect & PAGE_READWRITE)!=0 ||
                        (mi.Protect & PAGE_EXECUTE_READWRITE)!=0 )
                    {
                        rpsModuleMemoryInfo tmp = {mi.BaseAddress, mi.RegionSize};
                        sinfo.push_back(tmp);
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
    }

    if(ar.isReader()) {
        rpsEach(m_pages, [](MemoryPages::value_type &v){
            vaVirtualFree(v.second.base, 0, MEM_RELEASE);
        });
    }
    ar & m_pages;
}

void rpsMemoryModule::handleMessage( rpsMessage &m )
{
    if(strcmp(m.command, "setMemorySize")==0) {
        setMemorySize(m.value.cast<size_t>());
        return;
    }
}

LPVOID rpsMemoryModule::rpsHeapAllocImpl(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes)
{
    rpsMutex::ScopedLock lock(m_mutex);
    void *ret = mspace_malloc(m_msp, dwBytes);
    m_pos = std::max<size_t>((size_t)ret-(size_t)m_mem+dwBytes, m_pos);
    return ret;
}

LPVOID rpsMemoryModule::rpsHeapReAllocImpl(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes)
{
    rpsMutex::ScopedLock lock(m_mutex);
    void *ret = mspace_realloc(m_msp, lpMem, dwBytes);
    m_pos = std::max<size_t>((size_t)ret-(size_t)m_mem+dwBytes, m_pos);
    return ret;
}

BOOL rpsMemoryModule::rpsHeapFreeImpl(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem)
{
    rpsMutex::ScopedLock lock(m_mutex);
    mspace_free(m_msp, lpMem);
    return TRUE;
}

BOOL rpsMemoryModule::rpsHeapValidateImpl(HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem)
{
    return TRUE;
}

SIZE_T rpsMemoryModule::rpsHeapSizeImpl(HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem)
{
    return m_size;
}

LPVOID rpsMemoryModule::rpsVirtualAllocImpl(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect)
{
    rpsMutex::ScopedLock lock(m_mutex);
    LPVOID ret = vaVirtualAlloc(lpAddress, dwSize, flAllocationType, flProtect);
    rpsMemoryPageInfo pinfo = {ret, dwSize, flAllocationType, flProtect};
    m_pages[(size_t)ret] = pinfo;
    return ret;
}

BOOL rpsMemoryModule::rpsVirtualFreeImpl(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType)
{
    rpsMutex::ScopedLock lock(m_mutex);
    BOOL ret = vaVirtualFree(lpAddress, dwSize, dwFreeType);
    if(ret) {
        auto it = m_pages.find((size_t)lpAddress);
        if(it!=m_pages.end()) {
            m_pages.erase(it);
        }
    }
    return ret;
}

} // namespace

rpsAPI void* rpsGetHeapBlock() { return rpsMemoryModule::getInstance()->getHeapBlock(); }

rpsDLLExport rpsIModule* rpsCreateMemoryModule() { return rpsMemoryModule::getInstance(); }
