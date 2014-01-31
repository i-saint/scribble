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


struct rpsHookInfo
{
    const char *dllname;
    const char *funcname;
    uint32_t funcordinal;
    void *hookfunc;
    void **origfunc;

    rpsHookInfo(const char *dll=nullptr, const char *fname=nullptr, uint32_t ford=0, void *hook=nullptr, void **orig=nullptr)
        : dllname(dll)
        , funcname(fname)
        , funcordinal(ford)
        , hookfunc(hook)
        , origfunc(orig)
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

typedef std::basic_string<   char, std::char_traits<   char>, rps_allocator<   char> > rps_string;
typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, rps_allocator<wchar_t> > rps_wstring;
class rpsCommunicator;

inline bool rpsIsRpsHandle(HANDLE h) { return (DWORD)h>>24=='R'; }

struct rpsHandleInfo
{
    HANDLE rps_handle;
    HANDLE win_handle;
    rpsIModule *owner;
};

rpsAPI HANDLE rpsCreateHandle(rpsIModule *owner, HANDLE win_handle);
rpsAPI bool rpsReleaseHandle(HANDLE rps_handle);
rpsAPI HANDLE rpsTranslateHandle(HANDLE rps_handle); // return win handle
rpsAPI rpsHandleInfo* rpsGetHandleInfo(HANDLE rps_handle);


class rpsMainModule
{
public:
    struct SerializeRequest
    {
        rps_string path;
        rpsArchive::Mode mode;
    };
    typedef std::map<rps_string, rpsIModule*, std::less<rps_string>, rps_allocator<std::pair<rps_string, rpsIModule*> > > Modules;
    typedef std::vector<rpsHookInfo*, rps_allocator<rpsHookInfo*> > Hooks;
    typedef std::map<rps_string, Hooks, std::less<rps_string>, rps_allocator<std::pair<rps_string, Hooks> > > FuncHookTable;
    typedef std::map<rps_string, FuncHookTable, std::less<rps_string>, rps_allocator<std::pair<rps_string, FuncHookTable> > > DLLHookTable;
    typedef std::vector<SerializeRequest, rps_allocator<SerializeRequest> > Requests;

    static void initialize();
    static rpsMainModule* getInstance();

    void sendMessage(rpsMessage &m);
    void pushRequest(SerializeRequest &req);
    void processRequest();

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
        for(auto i=m_modules.begin(); i!=m_modules.end(); ++i) {
            f(i->second);
        }
    }

    Modules m_modules;
    DLLHookTable m_hooks;
    Requests m_requests;
    rpsMutex m_mtx_requests;
    rpsCommunicator *m_communicator;
    DWORD m_mainthread;
};

#include "rpsInlines.h"
#include "rpsFuncTypes.h"

#endif // rpsInternal_h
