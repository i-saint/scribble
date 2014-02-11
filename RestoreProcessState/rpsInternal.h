#ifndef rpsInternal_h
#define rpsInternal_h

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <windows.h>
#include <ws2tcpip.h>
#include <winsock2.h>
#include <dbghelp.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <d3d9.h>
#include <d3d11.h>
#ifdef max
#   undef max
#   undef min
#endif // max

#include "rps.h"
#include "rpsFoundation.h"
#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "ws2_32.lib")

// todo
#define rpsPrintError(...) printf(__VA_ARGS__)
#define rpsPrintInfo(...) printf(__VA_ARGS__)

#define rpsHookAPI __declspec(noinline)

enum {
    rpsE_IATOverride = 0x01,
    rpsE_EATOverride = 0x02,
    rpsE_IEATOverride = rpsE_IATOverride|rpsE_EATOverride,
};
struct rpsHookInfo
{
    const char *dllname;
    const char *funcname;
    void *hookfunc;
    void **origfunc;
    uint32_t funcordinal;
    uint32_t flags;

    rpsHookInfo(const char *dll=nullptr, const char *fname=nullptr, uint32_t ford=0, void *hook=nullptr, void **orig=nullptr, uint32_t f=rpsE_IATOverride)
        : dllname(dll)
        , funcname(fname)
        , hookfunc(hook)
        , origfunc(orig)
        , funcordinal(ford)
        , flags(f)
    {
    }
};


class rpsIModule
{
public:
    virtual ~rpsIModule() {}
    virtual void            release() { delete this; }
    virtual const char*     getModuleName() const=0;
    virtual rpsHookInfo*    getHooks() const=0;
    virtual void            initialize()=0;
    virtual void            serialize(rpsArchive &ar)=0;
    virtual void            handleMessage(rpsMessage &mes) {}

    void* operator new(size_t s) { return rpsMalloc(s); }
    void  operator delete(void *p) { return rpsFree(p); }
};
typedef rpsIModule* (*rpsModuleCreator)();

class rpsCommunicator;

inline bool rpsIsRpsHandle(HANDLE h) { return (DWORD)h>>24=='R'; }

struct rpsHandleInfo
{
    HANDLE rps_handle;
    HANDLE win_handle;
    rpsIModule *owner;
};

struct rpsConfigData
{
    typedef std::vector<rps_string, rps_allocator<rps_string> > StringList;
    StringList serializable_dlls;
    StringList disabled_modules;
    uint16_t tcp_port;

    rpsConfigData();
};

rpsAPI rpsConfigData& rpsGetConfig();
rpsAPI void* rpsGetHeapBlock();
rpsAPI HANDLE rpsCreateHandle(rpsIModule *owner, HANDLE win_handle);
rpsAPI bool rpsReleaseHandle(HANDLE rps_handle);
rpsAPI HANDLE rpsToWinHandle(HANDLE rps_handle);
rpsAPI HANDLE rpsToRpsHandle(HANDLE win_handle);
rpsAPI rpsHandleInfo* rpsGetHandleInfo(HANDLE rps_handle);
rpsAPI bool rpsIsSerializableModule(HMODULE mod);
rpsAPI bool rpsIsSerializableModule(const char *filename_or_path);


class rpsMainModule
{
public:
    struct SerializeRequest
    {
        char path[256];
        rpsArchive::Mode mode;

        SerializeRequest(const char *p, rpsArchive::Mode m)
        {
            strncpy(path, p, _countof(path));
            mode = m;
        }
    };
    typedef std::vector<rpsIModule*, rps_allocator<rpsIModule*> > Modules;
    typedef std::map<rps_string, rpsIModule*, std::less<rps_string>, rps_allocator<std::pair<rps_string, rpsIModule*> > > ModuleMap;
    typedef std::vector<rpsHookInfo*, rps_allocator<rpsHookInfo*> > Hooks;
    typedef std::map<rps_string, Hooks, std::less<rps_string>, rps_allocator<std::pair<rps_string, Hooks> > > FuncHookTable;
    typedef std::map<rps_string, FuncHookTable, std::less<rps_string>, rps_allocator<std::pair<rps_string, FuncHookTable> > > DLLHookTable;
    typedef std::vector<SerializeRequest, rps_allocator<SerializeRequest> > Requests;

    static void initialize();
    static rpsMainModule* getInstance();

    void sendMessage(rpsMessage &m);
    void pushRequest(SerializeRequest &req);
    void waitForCompleteRequests();
    void setHooks(HMODULE mod);

    void* operator new(size_t s) { return rpsMalloc(s); }
    void  operator delete(void *p) { return rpsFree(p); }

private:
    friend DWORD __stdcall rpsMainThread(LPVOID lpThreadParameter);

    rpsMainModule();
    ~rpsMainModule();
    void serializeImpl(rpsArchive &ar);
    void serializeImpl(const char *path, rpsArchive::Mode mode);
    void mainloop();

    // F: [](rpsIModule*) -> void
    template<class F>
    void eachModules(const F &f)
    {
        rpsEach(m_modules, f);
    }

    Modules m_modules;
    ModuleMap m_module_map;
    DLLHookTable m_hooks;
    Requests m_requests;
    rpsMutex m_mtx_requests;
    rpsCommunicator *m_communicator;
    DWORD m_tid;
};

#include "rpsInlines.h"
#include "rpsFuncTypes.h"

#endif // rpsInternal_h
