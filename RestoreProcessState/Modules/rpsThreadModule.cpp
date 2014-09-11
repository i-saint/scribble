#include "rpsPCH.h"
#include "rpsInternal.h"

namespace {

struct rpsThreadRecord
{
    HANDLE rps_handle;
    HANDLE win_handle;
    DWORD tid;
    CONTEXT contxt;
    void *stack_base;
    size_t stack_size;

    bool create(DWORD tid);
    bool create(HANDLE thandle);
    bool update();
};
inline rpsArchive& operator&(rpsArchive &ar, CONTEXT &v) { ar.io(&v, sizeof(v)); return ar; }
inline rpsArchive& operator&(rpsArchive &ar, rpsThreadRecord &v)
{
    ar & v.rps_handle & v.win_handle & v.tid & v.contxt & (size_t&)v.stack_base & v.stack_size;
    if(ar.isWriter()) {
        ar.io(v.stack_base, v.stack_size);
    }
    else if(ar.isReader()) {
        if(HANDLE thandle=::OpenThread(THREAD_ALL_ACCESS, FALSE, v.tid)) {
            ar.io(v.stack_base, v.stack_size);
            BOOL r = ::SetThreadContext(thandle, &v.contxt);
            ::CloseHandle(thandle);
        }
        else {
            ar.skip(v.stack_size);
        }
    }
    return ar;
}
typedef rpsTHandleRecords<rpsThreadRecord> rpsThreadRecords;


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

    rpsThreadRecords* getThreadRecords() { return &m_records; }

private:
    rpsThreadRecords m_records;
};
typedef rpsThreadModule rpsCurrentModule;
inline rpsCurrentModule* rpsGetCurrentModule() { return rpsCurrentModule::getInstance(); }
inline rpsThreadRecords* rpsGetThreadRecords() { return rpsGetCurrentModule()->getThreadRecords(); }


CreateThreadT vaCreateThread;
CloseHandleT  vaCloseHandle;
GetThreadIdT vaGetThreadId;

rpsHookAPI HANDLE WINAPI rpsCreateThread(
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    SIZE_T dwStackSize,
    LPTHREAD_START_ROUTINE lpStartAddress,
    LPVOID lpParameter,
    DWORD dwCreationFlags,
    LPDWORD lpThreadId
    )
{
    HANDLE ret = vaCreateThread(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId);
    rpsThreadRecord rec;
    rec.create(ret);
    return ret;
}

rpsHookAPI BOOL WINAPI rpsCloseHandle(HANDLE hObject)
{
    if(rpsThreadRecord *rec=rpsGetThreadRecords()->findRecord(hObject)) {
        HANDLE win_handle = rpsToWinHandleC(hObject, vaCloseHandle);
        BOOL ret = vaCloseHandle(win_handle);
        rpsLogInfo("rpsCloseHandle(%p): %u\n", win_handle, ret);
        if(ret) {
            rpsGetThreadRecords()->eraseRecord(hObject);
        }
        return ret;
    }
    else {
        return vaCloseHandle(rpsToWinHandleC(hObject, vaCloseHandle));
    }
}

DWORD  WINAPI rpsGetThreadId(HANDLE Thread)
{
    return vaGetThreadId(Thread);
}


rpsHookInfo g_hookinfo[] = {
    rpsDefineHookInfo("kernel32.dll", CreateThread),
    rpsDefineHookInfo("kernel32.dll", CloseHandle),
    rpsDefineHookInfo("kernel32.dll", GetThreadId),

    rpsHookInfo(nullptr, nullptr, 0, nullptr, nullptr),
};


bool create(DWORD tid);
bool create(HANDLE thandle);
bool update();

bool rpsThreadRecord::create(DWORD _tid)
{
    if(HANDLE thandle=::OpenThread(THREAD_ALL_ACCESS, FALSE, _tid)) {
        return create(thandle);
    }
    return false;
}

bool rpsThreadRecord::create(HANDLE thandle)
{
    contxt.ContextFlags = CONTEXT_ALL; 
    tid = vaGetThreadId(thandle);
    win_handle = thandle;
    rps_handle = rpsCreateHandle(rpsGetCurrentModule(), win_handle);
    ::GetThreadContext(thandle, &contxt);

    MEMORY_BASIC_INFORMATION mi;
    if(
#if defined(_M_IX86)
        ::VirtualQuery((void*)contxt.Esp, &mi, sizeof(mi))
#elif defined(_M_X64)
        ::VirtualQuery((void*)contxt.Rsp, &mi, sizeof(mi))
#endif
        )
    {
        stack_base = mi.BaseAddress;
        stack_size = mi.RegionSize;
    }
    ::CloseHandle(thandle);
    return true;

}

bool rpsThreadRecord::update()
{
    if(HANDLE thandle=::OpenThread(THREAD_ALL_ACCESS, FALSE, tid)) {
        contxt.ContextFlags = CONTEXT_ALL; 
        ::GetThreadContext(thandle, &contxt);
        ::CloseHandle(thandle);
        return true;
    }
    return false;
}

const char*     rpsThreadModule::getModuleName() const   { return "rpsThreadModule"; }
rpsHookInfo*    rpsThreadModule::getHooks() const        { return g_hookinfo; }

rpsThreadModule* rpsThreadModule::getInstance()
{
    static rpsThreadModule *s_inst = new rpsThreadModule();
    return s_inst;
}

rpsThreadModule::rpsThreadModule()
{
}

rpsThreadModule::~rpsThreadModule()
{
}

void rpsThreadModule::initialize()
{
    {
        rpsThreadRecord tmp;
        tmp.create(rpsGetMainThreadID());
        m_records.addRecord(tmp.rps_handle, tmp);
    }
}

void rpsThreadModule::serialize(rpsArchive &ar)
{
    ar & m_records;
}

void rpsThreadRecords::serialize(rpsArchive &ar)
{
    // todo: thread 再生成
    if(ar.isWriter()) {
        rpsEach(m_records, [&](rpsThreadRecords::Pair &v){
            v.second.update();
        });
    }
    ar & m_records;
}

} // namespace

rpsDLLExport rpsIModule* rpsCreateThreadModule() { return rpsThreadModule::getInstance(); }
