#include "rpsPCH.h"
#include "rpsInternal.h"

namespace {

struct rpsEventRecord
{
    HANDLE rps_handle;
    HANDLE win_handle;
    DWORD flags;
    DWORD access;
    rps_wstring name;
};
rpsArchive& operator&(rpsArchive &ar, rpsEventRecord &v)
{
    return ar & v.rps_handle & v.win_handle & v.flags & v.access & v.name;
}
typedef rpsTHandleRecords<rpsEventRecord> rpsEventRecords;


struct rpsMutexRecord
{
    HANDLE rps_handle;
    HANDLE win_handle;
    DWORD flags;
    DWORD access;
    rps_wstring name;
};
rpsArchive& operator&(rpsArchive &ar, rpsMutexRecord &v)
{
    return ar & v.rps_handle & v.win_handle & v.flags & v.access & v.name;
}
typedef rpsTHandleRecords<rpsMutexRecord> rpsMutexRecords;

struct rpsSemaphoreRecord
{
    HANDLE rps_handle;
    HANDLE win_handle;
    LONG lInitialCount;
    LONG lMaximumCount;
    DWORD flags;
    DWORD access;
    rps_wstring name;
};
rpsArchive& operator&(rpsArchive &ar, rpsSemaphoreRecord &v)
{
    return ar & v.rps_handle & v.win_handle & v.lInitialCount & v.lMaximumCount & v.flags & v.access & v.name;
}
typedef rpsTHandleRecords<rpsSemaphoreRecord> rpsSemaphoreRecords;


class rpsSyncModule : public rpsIModule
{
public:
    static rpsSyncModule* getInstance();

    rpsSyncModule();
    ~rpsSyncModule();
    virtual const char*     getModuleName() const;
    virtual rpsHookInfo*    getHooks() const;
    virtual void initialize();
    virtual void serialize(rpsArchive &ar);

    rpsEventRecords* getEventRecords() { return &m_event_recs; }
    rpsMutexRecords* getMutexRecords() { return &m_mutex_recs; }
    rpsSemaphoreRecords* getSemaphoreRecords() { return &m_semaphore_recs; }

private:
    rpsEventRecords m_event_recs;
    rpsMutexRecords m_mutex_recs;
    rpsSemaphoreRecords m_semaphore_recs;
};
typedef rpsSyncModule rpsCurrentModule;
inline rpsCurrentModule* rpsGetCurrentModule()       { return rpsCurrentModule::getInstance(); }
inline rpsEventRecords* rpsGetEventRecords()         { return rpsGetCurrentModule()->getEventRecords(); }
inline rpsMutexRecords* rpsGetMutexRecords()         { return rpsGetCurrentModule()->getMutexRecords(); }
inline rpsSemaphoreRecords* rpsGetSemaphoreRecords() { return rpsGetCurrentModule()->getSemaphoreRecords(); }


InitializeCriticalSectionT             vaInitializeCriticalSection;
DeleteCriticalSectionT                 vaDeleteCriticalSection;
EnterCriticalSectionT                  vaEnterCriticalSection;
InitializeCriticalSectionAndSpinCountT vaInitializeCriticalSectionAndSpinCount;
LeaveCriticalSectionT                  vaLeaveCriticalSection;
SetCriticalSectionSpinCountT           vaSetCriticalSectionSpinCount;
TryEnterCriticalSectionT               vaTryEnterCriticalSection;

CreateEventAT   vaCreateEventA;
CreateEventWT   vaCreateEventW;
CreateEventExAT vaCreateEventExA;
CreateEventExWT vaCreateEventExW;
OpenEventAT     vaOpenEventA;
OpenEventWT     vaOpenEventW;
PulseEventT     vaPulseEvent;
ResetEventT     vaResetEvent;
SetEventT       vaSetEvent;

CreateMutexAT   vaCreateMutexA;
CreateMutexWT   vaCreateMutexW;
CreateMutexExAT vaCreateMutexExA;
CreateMutexExWT vaCreateMutexExW;
OpenMutexAT     vaOpenMutexA;
OpenMutexWT     vaOpenMutexW;
ReleaseMutexT   vaReleaseMutex;

CreateSemaphoreAT   vaCreateSemaphoreA;
CreateSemaphoreWT   vaCreateSemaphoreW;
CreateSemaphoreExAT vaCreateSemaphoreExA;
CreateSemaphoreExWT vaCreateSemaphoreExW;
OpenSemaphoreAT     vaOpenSemaphoreA;
OpenSemaphoreWT     vaOpenSemaphoreW;
ReleaseSemaphoreT   vaReleaseSemaphore;

CloseHandleT    vaCloseHandle;


rpsHookAPI void WINAPI rpsInitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
    vaInitializeCriticalSection(lpCriticalSection);
}
rpsHookAPI void WINAPI rpsDeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
    vaDeleteCriticalSection(lpCriticalSection);
}
rpsHookAPI void WINAPI rpsEnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
    vaEnterCriticalSection(lpCriticalSection);
}
rpsHookAPI BOOL WINAPI rpsInitializeCriticalSectionAndSpinCount(LPCRITICAL_SECTION lpCriticalSection, DWORD dwSpinCount)
{
    return vaInitializeCriticalSectionAndSpinCount(lpCriticalSection, dwSpinCount);
}
rpsHookAPI void WINAPI rpsLeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
    vaLeaveCriticalSection(lpCriticalSection);
}
rpsHookAPI DWORD WINAPI rpsSetCriticalSectionSpinCount(LPCRITICAL_SECTION lpCriticalSection, DWORD dwSpinCount)
{
    return vaSetCriticalSectionSpinCount(lpCriticalSection, dwSpinCount);
}
rpsHookAPI BOOL WINAPI rpsTryEnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
    return vaTryEnterCriticalSection(lpCriticalSection);
}


rpsHookAPI HANDLE WINAPI rpsCreateEventExW(LPSECURITY_ATTRIBUTES lpEventAttributes, LPCWSTR lpName, DWORD dwFlags, DWORD dwDesiredAccess)
{
    HANDLE win_handle = vaCreateEventExW(lpEventAttributes, lpName, dwFlags, dwDesiredAccess);
    HANDLE rps_handle = rpsCreateHandle(rpsCurrentModule::getInstance(), win_handle);
    rpsEventRecord record = {rps_handle, win_handle, dwFlags, dwDesiredAccess, lpName};
    rpsGetEventRecords()->addRecord(rps_handle, record);
    rpsLogInfo("rpsCreateEventExW() win:%p rps:%p", win_handle, rps_handle);
    return rps_handle;
}
rpsHookAPI HANDLE WINAPI rpsCreateEventExA(LPSECURITY_ATTRIBUTES lpEventAttributes, LPCSTR lpName, DWORD dwFlags, DWORD dwDesiredAccess)
{
    HANDLE win_handle = vaCreateEventExA(lpEventAttributes, lpName, dwFlags, dwDesiredAccess);
    HANDLE rps_handle = rpsCreateHandle(rpsCurrentModule::getInstance(), win_handle);
    rpsEventRecord record = {rps_handle, win_handle, dwFlags, dwDesiredAccess, rpsL(lpName)};
    rpsGetEventRecords()->addRecord(rps_handle, record);
    rpsLogInfo("rpsCreateEventExA() win:%p rps:%p", win_handle, rps_handle);
    return rps_handle;
}
rpsHookAPI HANDLE WINAPI rpsCreateEventW(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCWSTR lpName)
{
    DWORD flags = 0;
    DWORD access = EVENT_ALL_ACCESS;
    if(bManualReset)  flags |= CREATE_EVENT_MANUAL_RESET;
    if(bInitialState) flags |= CREATE_EVENT_INITIAL_SET;
    return rpsCreateEventExW(lpEventAttributes, lpName, flags, access);
}
rpsHookAPI HANDLE WINAPI rpsCreateEventA(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCSTR lpName)
{
    DWORD flags = 0;
    DWORD access = EVENT_ALL_ACCESS;
    if(bManualReset)  flags |= CREATE_EVENT_MANUAL_RESET;
    if(bInitialState) flags |= CREATE_EVENT_INITIAL_SET;
    return rpsCreateEventExA(lpEventAttributes, lpName, flags, access);
}
rpsHookAPI HANDLE WINAPI rpsOpenEventW(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCWSTR lpName)
{
    HANDLE win_handle = vaOpenEventW(dwDesiredAccess, bInheritHandle, lpName);
    return win_handle;
}
rpsHookAPI HANDLE WINAPI rpsOpenEventA(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCSTR lpName)
{
    HANDLE win_handle = vaOpenEventA(dwDesiredAccess, bInheritHandle, lpName);
    return win_handle;
}
rpsHookAPI BOOL   WINAPI rpsPulseEvent(HANDLE hEvent)
{
    return vaPulseEvent(rpsToWinHandleC(hEvent, vaPulseEvent));
}
rpsHookAPI BOOL   WINAPI rpsResetEvent(HANDLE hEvent)
{
    return vaResetEvent(rpsToWinHandleC(hEvent, vaResetEvent));
}
rpsHookAPI BOOL   WINAPI rpsSetEvent(HANDLE hEvent)
{
    return vaSetEvent(rpsToWinHandleC(hEvent, vaSetEvent));
}


rpsHookAPI HANDLE WINAPI rpsCreateMutexExW(LPSECURITY_ATTRIBUTES lpMutexAttributes, LPCWSTR lpName, DWORD dwFlags, DWORD dwDesiredAccess)
{
    HANDLE win_handle = vaCreateMutexExW(lpMutexAttributes, lpName, dwFlags, dwDesiredAccess);
    HANDLE rps_handle = rpsCreateHandle(rpsCurrentModule::getInstance(), win_handle);
    rpsEventRecord record = {rps_handle, win_handle, dwFlags, dwDesiredAccess, lpName};
    rpsGetEventRecords()->addRecord(rps_handle, record);
    rpsLogInfo("rpsCreateMutexExW() win:%p rps:%p", win_handle, rps_handle);
    return rps_handle;
}
rpsHookAPI HANDLE WINAPI rpsCreateMutexExA(LPSECURITY_ATTRIBUTES lpMutexAttributes, LPCSTR lpName, DWORD dwFlags, DWORD dwDesiredAccess)
{
    HANDLE win_handle = vaCreateMutexExA(lpMutexAttributes, lpName, dwFlags, dwDesiredAccess);
    HANDLE rps_handle = rpsCreateHandle(rpsCurrentModule::getInstance(), win_handle);
    rpsEventRecord record = {rps_handle, win_handle, dwFlags, dwDesiredAccess, rpsL(lpName)};
    rpsGetEventRecords()->addRecord(rps_handle, record);
    rpsLogInfo("rpsCreateMutexExA() win:%p rps:%p", win_handle, rps_handle);
    return rps_handle;
}
rpsHookAPI HANDLE WINAPI rpsCreateMutexW(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCWSTR lpName)
{
    DWORD flags = 0;
    DWORD access = MUTEX_ALL_ACCESS;
    if(bInitialOwner) { flags|=CREATE_MUTEX_INITIAL_OWNER; }
    return rpsCreateMutexExW(lpMutexAttributes, lpName, flags, access);
}
rpsHookAPI HANDLE WINAPI rpsCreateMutexA(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCSTR lpName)
{
    DWORD flags = 0;
    DWORD access = MUTEX_ALL_ACCESS;
    if(bInitialOwner) { flags|=CREATE_MUTEX_INITIAL_OWNER; }
    return rpsCreateMutexExA(lpMutexAttributes, lpName, flags, access);
}
rpsHookAPI HANDLE WINAPI rpsOpenMutexW(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCWSTR lpName)
{
    HANDLE win_handle = vaOpenMutexW(dwDesiredAccess, bInheritHandle, lpName);
    return win_handle;
}
rpsHookAPI HANDLE WINAPI rpsOpenMutexA(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCSTR lpName)
{
    HANDLE win_handle = vaOpenMutexA(dwDesiredAccess, bInheritHandle, lpName);
    return win_handle;
}
rpsHookAPI BOOL   WINAPI rpsReleaseMutex(HANDLE hMutex)
{
    return vaReleaseMutex(rpsToWinHandleC(hMutex, vaReleaseMutex));
}


rpsHookAPI HANDLE WINAPI rpsCreateSemaphoreExW(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes, LONG lInitialCount, LONG lMaximumCount, LPCWSTR lpName, DWORD dwFlags, DWORD dwDesiredAccess)
{
    HANDLE win_handle = vaCreateSemaphoreExW(lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName, dwFlags, dwDesiredAccess);
    HANDLE rps_handle = rpsCreateHandle(rpsCurrentModule::getInstance(), win_handle);
    rpsSemaphoreRecord record = {rps_handle, win_handle, lInitialCount, lMaximumCount, dwFlags, dwDesiredAccess, lpName};
    rpsGetSemaphoreRecords()->addRecord(rps_handle, record);
    rpsLogInfo("rpsCreateSemaphoreExW() win:%p rps:%p", win_handle, rps_handle);
    return rps_handle;
}
rpsHookAPI HANDLE WINAPI rpsCreateSemaphoreExA(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes, LONG lInitialCount, LONG lMaximumCount, LPCSTR lpName, DWORD dwFlags, DWORD dwDesiredAccess)
{
    HANDLE win_handle = vaCreateSemaphoreExA(lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName, dwFlags, dwDesiredAccess);
    HANDLE rps_handle = rpsCreateHandle(rpsCurrentModule::getInstance(), win_handle);
    rpsSemaphoreRecord record = {rps_handle, win_handle, lInitialCount, lMaximumCount, dwFlags, dwDesiredAccess, rpsL(lpName)};
    rpsGetSemaphoreRecords()->addRecord(rps_handle, record);
    rpsLogInfo("rpsCreateSemaphoreExA() win:%p rps:%p", win_handle, rps_handle);
    return rps_handle;
}
rpsHookAPI HANDLE WINAPI rpsCreateSemaphoreW(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes, LONG lInitialCount, LONG lMaximumCount, LPCWSTR lpName)
{
    return rpsCreateSemaphoreExW(lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName, 0, SEMAPHORE_ALL_ACCESS);
}
rpsHookAPI HANDLE WINAPI rpsCreateSemaphoreA(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes, LONG lInitialCount, LONG lMaximumCount, LPCSTR lpName)
{
    return rpsCreateSemaphoreExA(lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName, 0, SEMAPHORE_ALL_ACCESS);
}
rpsHookAPI HANDLE WINAPI rpsOpenSemaphoreW(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCWSTR lpName)
{
    HANDLE win_handle = vaOpenSemaphoreW(dwDesiredAccess, bInheritHandle, lpName);
    return win_handle;
}
rpsHookAPI HANDLE WINAPI rpsOpenSemaphoreA(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCSTR lpName)
{
    HANDLE win_handle = vaOpenSemaphoreA(dwDesiredAccess, bInheritHandle, lpName);
    return win_handle;
}
rpsHookAPI BOOL   WINAPI rpsReleaseSemaphore(HANDLE hSemaphore, LONG lReleaseCount, LPLONG lpPreviousCount)
{
    return vaReleaseSemaphore(rpsToWinHandleC(hSemaphore, vaReleaseSemaphore), lReleaseCount, lpPreviousCount);
}


rpsHookAPI BOOL WINAPI rpsCloseHandle(HANDLE hObject)
{
    if(rpsEventRecord *rec=rpsGetEventRecords()->findRecord(hObject)) {
        BOOL ret = vaCloseHandle(rpsToWinHandleC(hObject, vaCloseHandle));
        if(ret) {
            rpsGetEventRecords()->eraseRecord(hObject);
        }
        return ret;
    }
    else if(rpsMutexRecord *rec=rpsGetMutexRecords()->findRecord(hObject)) {
        BOOL ret = vaCloseHandle(rpsToWinHandleC(hObject, vaCloseHandle));
        if(ret) {
            rpsGetMutexRecords()->eraseRecord(hObject);
        }
        return ret;
    }
    else if(rpsSemaphoreRecord *rec=rpsGetSemaphoreRecords()->findRecord(hObject)) {
        BOOL ret = vaCloseHandle(rpsToWinHandleC(hObject, vaCloseHandle));
        if(ret) {
            rpsGetSemaphoreRecords()->eraseRecord(hObject);
        }
        return ret;
    }
    else {
        return vaCloseHandle(rpsToWinHandleC(hObject, vaCloseHandle));
    }
}

rpsHookInfo g_hookinfo[] = {
    rpsDefineHookInfo("kernel32.dll", InitializeCriticalSection),
    rpsDefineHookInfo("kernel32.dll", DeleteCriticalSection),
    rpsDefineHookInfo("kernel32.dll", EnterCriticalSection),
    rpsDefineHookInfo("kernel32.dll", InitializeCriticalSectionAndSpinCount),
    rpsDefineHookInfo("kernel32.dll", LeaveCriticalSection),
    rpsDefineHookInfo("kernel32.dll", SetCriticalSectionSpinCount),
    rpsDefineHookInfo("kernel32.dll", TryEnterCriticalSection),

    rpsDefineHookInfo("kernel32.dll", CreateEventA),
    rpsDefineHookInfo("kernel32.dll", CreateEventW),
    rpsDefineHookInfo("kernel32.dll", CreateEventExA),
    rpsDefineHookInfo("kernel32.dll", CreateEventExW),
    rpsDefineHookInfo("kernel32.dll", OpenEventA),
    rpsDefineHookInfo("kernel32.dll", OpenEventW),
    rpsDefineHookInfo("kernel32.dll", PulseEvent),
    rpsDefineHookInfo("kernel32.dll", ResetEvent),
    rpsDefineHookInfo("kernel32.dll", SetEvent),

    rpsDefineHookInfo("kernel32.dll", CreateMutexA),
    rpsDefineHookInfo("kernel32.dll", CreateMutexW),
    rpsDefineHookInfo("kernel32.dll", CreateMutexExA),
    rpsDefineHookInfo("kernel32.dll", CreateMutexExW),
    rpsDefineHookInfo("kernel32.dll", OpenMutexA),
    rpsDefineHookInfo("kernel32.dll", OpenMutexW),
    rpsDefineHookInfo("kernel32.dll", ReleaseMutex),

    rpsDefineHookInfo("kernel32.dll", CreateSemaphoreExW),
    rpsDefineHookInfo("kernel32.dll", CreateSemaphoreExA),
    rpsDefineHookInfo("kernel32.dll", CreateSemaphoreW),
    rpsDefineHookInfo("kernel32.dll", CreateSemaphoreA),
    rpsDefineHookInfo("kernel32.dll", OpenSemaphoreW),
    rpsDefineHookInfo("kernel32.dll", OpenSemaphoreA),
    rpsDefineHookInfo("kernel32.dll", ReleaseSemaphore),

    rpsDefineHookInfo("kernel32.dll", CloseHandle),

    rpsHookInfo(nullptr, nullptr, 0, nullptr, nullptr),
};


const char*     rpsSyncModule::getModuleName() const { return "rpsSyncModule"; }
rpsHookInfo*    rpsSyncModule::getHooks() const      { return g_hookinfo; }

rpsSyncModule* rpsSyncModule::getInstance()
{
    static rpsSyncModule *s_inst = new rpsSyncModule();
    return s_inst;
}

rpsSyncModule::rpsSyncModule()
{
}

rpsSyncModule::~rpsSyncModule()
{
}

void rpsSyncModule::initialize()
{
}

void rpsSyncModule::serialize(rpsArchive &ar)
{
    ar & m_event_recs & m_mutex_recs;
}



void rpsEventRecords::serialize(rpsArchive &ar)
{
    if(ar.isReader()) {
        rpsEach(m_records, [](Pair &rec){
            vaCloseHandle(&rec.second.win_handle);
        });
    }
    ar & m_records;
    if(ar.isReader()) {
        rpsEach(m_records, [](rpsEventRecords::Pair &rec){
            rpsEventRecord &v = rec.second;
            v.win_handle = vaCreateEventExW(nullptr, v.name.c_str(), v.flags, v.access);
            rpsGetHandleInfo(v.rps_handle)->win_handle = v.win_handle;
        });
    }
}

void rpsMutexRecords::serialize(rpsArchive &ar)
{
    if(ar.isReader()) {
        rpsEach(m_records, [](Pair &rec){
            vaCloseHandle(&rec.second.win_handle);
        });
    }
    ar & m_records;
    if(ar.isReader()) {
        rpsEach(m_records, [](rpsMutexRecords::Pair &rec){
            rpsMutexRecord &v = rec.second;
            v.win_handle = vaCreateMutexExW(nullptr, v.name.c_str(), v.flags, v.access);
            rpsGetHandleInfo(v.rps_handle)->win_handle = v.win_handle;
        });
    }
}

void rpsSemaphoreRecords::serialize(rpsArchive &ar)
{
    if(ar.isReader()) {
        rpsEach(m_records, [](Pair &rec){
            vaCloseHandle(&rec.second.win_handle);
        });
    }
    ar & m_records;
    if(ar.isReader()) {
        rpsEach(m_records, [](rpsSemaphoreRecords::Pair &rec){
            rpsSemaphoreRecord &v = rec.second;
            v.win_handle = vaCreateSemaphoreExW(nullptr, v.lInitialCount, v.lMaximumCount, v.name.c_str(), v.flags, v.access);
            rpsGetHandleInfo(v.rps_handle)->win_handle = v.win_handle;
        });
    }
}

} // namespace

rpsDLLExport rpsIModule* rpsCreateSyncModule() { return rpsSyncModule::getInstance(); }
