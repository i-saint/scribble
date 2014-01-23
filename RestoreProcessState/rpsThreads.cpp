#include "rpsPCH.h"
#include "rpsInternal.h"

namespace {

class rpsThreads : public rpsIModule
{
public:
    static rpsThreads* getInstance();

    rpsThreads();
    ~rpsThreads();
    virtual const char*     getModuleName() const;
    virtual size_t          getNumHooks() const;
    virtual rpsHookInfo*    getHooks() const;
    virtual void serialize(rpsArchive &ar);

    HANDLE translate(HANDLE h);

private:
};

struct rpsThreadInformation
{
    DWORD tid;
    CONTEXT contxt;
    void *stack_base;
    rps_string stack_data;
};
inline rpsArchive& operator&(rpsArchive &ar, CONTEXT &v) { ar.io(&v, sizeof(v)); return ar; }
inline rpsArchive& operator&(rpsArchive &ar, rpsThreadInformation &v)
{
    ar & v.tid & v.contxt & (size_t&)v.stack_base & v.stack_data;
    return ar;
}



CreateThreadT vaCreateThread;

LPVOID WINAPI rpsCreateThread(
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    SIZE_T dwStackSize,
    LPTHREAD_START_ROUTINE lpStartAddress,
    LPVOID lpParameter,
    DWORD dwCreationFlags,
    LPDWORD lpThreadId
    )
{
    LPVOID ret = vaCreateThread(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId);
    return ret;
}

static rpsHookInfo g_hookinfo[] = {
    rpsHookInfo("kernel32.dll", "CreateThread",   0, rpsCreateThread, &(void*&)vaCreateThread),
};


const char*     rpsThreads::getModuleName() const   { return "rpsThreads"; }
size_t          rpsThreads::getNumHooks() const     { return _countof(g_hookinfo); }
rpsHookInfo*    rpsThreads::getHooks() const        { return g_hookinfo; }

rpsThreads* rpsThreads::getInstance()
{
    static rpsThreads *s_inst = new rpsThreads();
    return s_inst;
}

rpsThreads::rpsThreads()
{
}

rpsThreads::~rpsThreads()
{
}

void rpsThreads::serialize(rpsArchive &ar)
{
    std::vector<rpsThreadInformation, rps_allocator<rpsThreadInformation> > tinfo;
    if(ar.isWriter()) {
        rpsEnumerateThreads(::GetCurrentProcessId(), [&](DWORD tid){
            if(tid==::GetCurrentThreadId()) { return; }
            if(HANDLE thread=::OpenThread(THREAD_ALL_ACCESS, FALSE, tid)) {
                rpsThreadInformation tmp;
                tmp.contxt.ContextFlags = CONTEXT_ALL; 
                tmp.tid = tid;
                ::GetThreadContext(thread, &tmp.contxt);

                MEMORY_BASIC_INFORMATION mi;
#if defined(_M_IX86)
                ::VirtualQuery((void*)tmp.contxt.Esp, &mi, sizeof(mi));
#elif defined(_M_X64)
                ::VirtualQuery((void*)tmp.contxt.Rsp, &mi, sizeof(mi));
#endif
                tmp.stack_base = mi.BaseAddress;
                tmp.stack_data = rps_string((char*)tmp.stack_base, mi.RegionSize);

                tinfo.push_back(tmp);
                ::CloseHandle(thread);
            }
        });
        ar & tinfo;
    }
    else if(ar.isReader()) {
        ar & tinfo;
        rpsEach(tinfo, [](rpsThreadInformation &tinfo){
            if(HANDLE thread=::OpenThread(THREAD_ALL_ACCESS, FALSE, tinfo.tid)) {
                memcpy(tinfo.stack_base, &tinfo.stack_data[0], tinfo.stack_data.size());
                BOOL r = ::SetThreadContext(thread, &tinfo.contxt);
                ::CloseHandle(thread);
            }
        });
    }
}

HANDLE rpsThreads::translate(HANDLE h)
{
    // todo
    return h;
}

} // namespace

rpsDLLExport rpsIModule* rpsCreateThreads() { return rpsThreads::getInstance(); }
