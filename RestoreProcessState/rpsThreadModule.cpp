#include "rpsPCH.h"
#include "rpsInternal.h"

namespace {

class rpsThreadModule : public rpsIModule
{
public:
    static rpsThreadModule* getInstance();

    rpsThreadModule();
    ~rpsThreadModule();
    virtual const char*     getModuleName() const;
    virtual rpsHookInfo*    getHooks() const;
    virtual void initialize();
    virtual void serialize(rpsArchive &ar);
    virtual void handleMessage(rpsMessage &m);

    void addExcludeThread(DWORD tid);
    bool isExcludeThread(DWORD tid) const;
    HANDLE translate(HANDLE h);

private:
    typedef std::vector<DWORD, rps_allocator<DWORD> > ThreadIDs;
    DWORD m_main_thread_id;
    ThreadIDs m_exclude_threads;
    rpsMutex m_mutex;
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

    rpsHookInfo(nullptr, nullptr, 0, nullptr, nullptr),
};


const char*     rpsThreadModule::getModuleName() const   { return "rpsThreadModule"; }
rpsHookInfo*    rpsThreadModule::getHooks() const        { return g_hookinfo; }

rpsThreadModule* rpsThreadModule::getInstance()
{
    static rpsThreadModule *s_inst = new rpsThreadModule();
    return s_inst;
}

rpsThreadModule::rpsThreadModule()
    : m_main_thread_id(0)
{
    m_main_thread_id = ::GetCurrentThreadId();
}

rpsThreadModule::~rpsThreadModule()
{
}

void rpsThreadModule::initialize()
{
}

void rpsThreadModule::serialize(rpsArchive &ar)
{
    std::vector<rpsThreadInformation, rps_allocator<rpsThreadInformation> > tinfo;
    if(ar.isWriter()) {
        //rpsEnumerateThreads(::GetCurrentProcessId(), [&](DWORD tid){
        //    if(isExcludeThread(tid)) { return; }

            // 単純化のためメインスレッドに限定
            DWORD tid = m_main_thread_id;

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
        //});
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

void rpsThreadModule::handleMessage( rpsMessage &m )
{
    if(strcmp(m.command, "addExcludeThread")==0) {
        addExcludeThread(m.value.cast<DWORD>());
        return;
    }
}

void rpsThreadModule::addExcludeThread(DWORD tid)
{
    rpsMutex::ScopedLock lock(m_mutex);
    m_exclude_threads.push_back(tid);
    std::sort(m_exclude_threads.begin(), m_exclude_threads.end());
}

bool rpsThreadModule::isExcludeThread(DWORD tid) const
{
    if(tid==::GetCurrentThreadId()) { return true; }
    auto it = std::lower_bound(m_exclude_threads.begin(), m_exclude_threads.end(), tid);
    return it!=m_exclude_threads.end() && *it==tid;
}

HANDLE rpsThreadModule::translate(HANDLE h)
{
    rpsMutex::ScopedLock lock(m_mutex);
    // todo
    return h;
}

} // namespace

rpsDLLExport rpsIModule* rpsCreateThreadModule() { return rpsThreadModule::getInstance(); }
