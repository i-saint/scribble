#include "rpsPCH.h"
#include "rpsInternal.h"

namespace {

struct rpsCriticalSectionRecord
{
    HANDLE rps_handle;
    CRITICAL_SECTION cs;
    DWORD lock;
};
typedef rpsCriticalSectionRecord rpsCSRecord;
typedef rpsTHandleRecords<rpsCSRecord> rpsCSRecords;

struct rpsEventRecord
{
    HANDLE rps_handle;
    HANDLE win_handle;
    DWORD lock;
};
typedef rpsTHandleRecords<rpsEventRecord> rpsEventRecords;

struct rpsMutexRecord
{
    HANDLE rps_handle;
    HANDLE win_handle;
    DWORD lock;
};
typedef rpsTHandleRecords<rpsMutexRecord> rpsMutexRecords;


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

    rpsCSRecords* getCSRecords() { return &m_cs_recs; }
    rpsEventRecords* getEventRecords() { return &m_event_recs; }
    rpsMutexRecords* getMutexRecords() { return &m_mutex_recs; }

private:
    rpsCSRecords m_cs_recs;
    rpsEventRecords m_event_recs;
    rpsMutexRecords m_mutex_recs;
};
typedef rpsSyncModule rpsCurrentModule;
inline rpsCurrentModule* rpsGetCurrentModule() { return rpsCurrentModule::getInstance(); }
inline rpsCSRecords* rpsGetCSRecords() { return rpsGetCurrentModule()->getCSRecords(); }
inline rpsEventRecords* rpsGetEventRecords() { return rpsGetCurrentModule()->getEventRecords(); }
inline rpsMutexRecords* rpsGetMutexRecords() { return rpsGetCurrentModule()->getMutexRecords(); }


InitializeCriticalSectionT             vaInitializeCriticalSection;
DeleteCriticalSectionT                 vaDeleteCriticalSection;
EnterCriticalSectionT                  vaEnterCriticalSection;
InitializeCriticalSectionAndSpinCountT vaInitializeCriticalSectionAndSpinCount;
LeaveCriticalSectionT                  vaLeaveCriticalSection;
SetCriticalSectionSpinCountT           vaSetCriticalSectionSpinCount;
TryEnterCriticalSectionT               vaTryEnterCriticalSection;

CloseHandleT    vaCloseHandle;
CreateEventAT   vaCreateEventA;
CreateEventWT   vaCreateEventW;
CreateEventExAT vaCreateEventExA;
CreateEventExWT vaCreateEventExW;
OpenEventAT     vaOpenEventA;
OpenEventWT     vaOpenEventW;
PulseEventT     vaPulseEvent;
ResetEventT     vaResetEvent;
SetEventT       vaSetEvent;


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


rpsHookAPI HANDLE WINAPI rpsCreateEventA(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCSTR lpName)
{
    HANDLE win_handle = vaCreateEventA(lpEventAttributes, bManualReset, bInitialState, lpName);
    return win_handle;
}
rpsHookAPI HANDLE WINAPI rpsCreateEventW(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCWSTR lpName)
{
    HANDLE win_handle = vaCreateEventW(lpEventAttributes, bManualReset, bInitialState, lpName);
    return win_handle;
}
rpsHookAPI HANDLE WINAPI rpsCreateEventExA(LPSECURITY_ATTRIBUTES lpEventAttributes, LPCSTR lpName, DWORD dwFlags, DWORD dwDesiredAccess)
{
    HANDLE win_handle = vaCreateEventExA(lpEventAttributes, lpName, dwFlags, dwDesiredAccess);
    return win_handle;
}
rpsHookAPI HANDLE WINAPI rpsCreateEventExW(LPSECURITY_ATTRIBUTES lpEventAttributes, LPCWSTR lpName, DWORD dwFlags, DWORD dwDesiredAccess)
{
    HANDLE win_handle = vaCreateEventExW(lpEventAttributes, lpName, dwFlags, dwDesiredAccess);
    return win_handle;
}
rpsHookAPI HANDLE WINAPI rpsOpenEventA(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCSTR lpName)
{
    HANDLE win_handle = vaOpenEventA(dwDesiredAccess, bInheritHandle, lpName);
    return win_handle;
}
rpsHookAPI HANDLE WINAPI rpsOpenEventW(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCWSTR lpName)
{
    HANDLE win_handle = vaOpenEventW(dwDesiredAccess, bInheritHandle, lpName);
    return win_handle;
}
rpsHookAPI BOOL   WINAPI rpsPulseEvent(HANDLE hEvent)
{
    return vaPulseEvent(hEvent);
}
rpsHookAPI BOOL   WINAPI rpsResetEvent(HANDLE hEvent)
{
    return vaResetEvent(hEvent);
}
rpsHookAPI BOOL   WINAPI rpsSetEvent(HANDLE hEvent)
{
    return vaSetEvent(hEvent);
}


rpsHookAPI BOOL WINAPI rpsCloseHandle(HANDLE hObject)
{
    if(rpsEventRecord *rec=rpsGetEventRecords()->findRecord(hObject)) {
        BOOL ret = vaCloseHandle(rpsTranslateHandleC(hObject, vaCloseHandle));
        if(ret) {
            rpsGetEventRecords()->eraseRecord(hObject);
        }
        return ret;
    }
    else if(rpsMutexRecord *rec=rpsGetMutexRecords()->findRecord(hObject)) {
        BOOL ret = vaCloseHandle(rpsTranslateHandleC(hObject, vaCloseHandle));
        if(ret) {
            rpsGetMutexRecords()->eraseRecord(hObject);
        }
        return ret;
    }
    else {
        return vaCloseHandle(rpsTranslateHandleC(hObject, vaCloseHandle));
    }
}

rpsHookInfo g_hookinfo[] = {
    rpsHookInfo("kernel32.dll", "InitializeCriticalSection",             0, rpsInitializeCriticalSection,             &(void*&)vaInitializeCriticalSection            ),
    rpsHookInfo("kernel32.dll", "DeleteCriticalSection",                 0, rpsDeleteCriticalSection,                 &(void*&)vaDeleteCriticalSection                ),
    rpsHookInfo("kernel32.dll", "EnterCriticalSection",                  0, rpsEnterCriticalSection,                  &(void*&)vaEnterCriticalSection                 ),
    rpsHookInfo("kernel32.dll", "InitializeCriticalSectionAndSpinCount", 0, rpsInitializeCriticalSectionAndSpinCount, &(void*&)vaInitializeCriticalSectionAndSpinCount),
    rpsHookInfo("kernel32.dll", "LeaveCriticalSection",                  0, rpsLeaveCriticalSection,                  &(void*&)vaLeaveCriticalSection                 ),
    rpsHookInfo("kernel32.dll", "SetCriticalSectionSpinCount",           0, rpsSetCriticalSectionSpinCount,           &(void*&)vaSetCriticalSectionSpinCount          ),
    rpsHookInfo("kernel32.dll", "TryEnterCriticalSection",               0, rpsTryEnterCriticalSection,               &(void*&)vaTryEnterCriticalSection              ),

    rpsHookInfo("kernel32.dll", "CreateEventA",   0, rpsCreateEventA,   &(void*&)vaCreateEventA  ),
    rpsHookInfo("kernel32.dll", "CreateEventW",   0, rpsCreateEventW,   &(void*&)vaCreateEventW  ),
    rpsHookInfo("kernel32.dll", "CreateEventExA", 0, rpsCreateEventExA, &(void*&)vaCreateEventExA),
    rpsHookInfo("kernel32.dll", "CreateEventExW", 0, rpsCreateEventExW, &(void*&)vaCreateEventExW),
    rpsHookInfo("kernel32.dll", "OpenEventA",     0, rpsOpenEventA,     &(void*&)vaOpenEventA    ),
    rpsHookInfo("kernel32.dll", "OpenEventW",     0, rpsOpenEventW,     &(void*&)vaOpenEventW    ),
    rpsHookInfo("kernel32.dll", "PulseEvent",     0, rpsPulseEvent,     &(void*&)vaPulseEvent    ),
    rpsHookInfo("kernel32.dll", "ResetEvent",     0, rpsResetEvent,     &(void*&)vaResetEvent    ),
    rpsHookInfo("kernel32.dll", "SetEvent",       0, rpsSetEvent,       &(void*&)vaSetEvent      ),

    rpsHookInfo("kernel32.dll", "CloseHandle",    0, rpsCloseHandle,    &(void*&)vaCloseHandle   ),

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
    ar & m_cs_recs & m_event_recs & m_mutex_recs;
}



rpsArchive& operator&(rpsArchive &ar, rpsCSRecord &v)
{
    return ar;
}

rpsArchive& operator&(rpsArchive &ar, rpsEventRecord &v)
{
    return ar;
}

rpsArchive& operator&(rpsArchive &ar, rpsMutexRecord &v)
{
    return ar;
}

void rpsCSRecords::serialize(rpsArchive &ar)
{
    if(ar.isReader()) {
        rpsEach(m_records, [](Pair &rec){
            vaDeleteCriticalSection(&rec.second.cs);
        });
    }
    ar & m_records;
}

void rpsEventRecords::serialize(rpsArchive &ar)
{
    if(ar.isReader()) {
        rpsEach(m_records, [](Pair &rec){
            vaCloseHandle(&rec.second.win_handle);
        });
    }
    ar & m_records;
}

void rpsMutexRecords::serialize(rpsArchive &ar)
{
    if(ar.isReader()) {
        rpsEach(m_records, [](Pair &rec){
            vaCloseHandle(&rec.second.win_handle);
        });
    }
    ar & m_records;
}

} // namespace

rpsDLLExport rpsIModule* rpsCreateSyncModule() { return rpsSyncModule::getInstance(); }
