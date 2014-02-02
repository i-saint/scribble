#include "rpsPCH.h"
#include "rpsInternal.h"

namespace {

class rpsTimeModule : public rpsIModule
{
public:
    static rpsTimeModule* getInstance();

    rpsTimeModule();
    ~rpsTimeModule();
    virtual const char*     getModuleName() const;
    virtual rpsHookInfo*    getHooks() const;
    virtual void initialize();
    virtual void serialize(rpsArchive &ar);
    virtual void handleMessage(rpsMessage &m);

    void setTimeScale(float v) { m_timescale=v; }
    BOOL rpsQueryPerformanceCounterImpl(LARGE_INTEGER *lpPerformanceCount);
    DWORD rpstimeGetTimeImpl();

private:
    float m_timescale;
    LARGE_INTEGER m_qpc_last;
    LARGE_INTEGER m_qpc_current;
    DWORD m_tgt_last;
    DWORD m_tgt_current;
    rpsMutex m_mutex;
};


QueryPerformanceCounterT    vaQueryPerformanceCounter;
timeGetTimeT                vatimeGetTime;

rpsHookAPI BOOL WINAPI rpsQueryPerformanceCounter( LARGE_INTEGER *lpPerformanceCount )
{
    return rpsTimeModule::getInstance()->rpsQueryPerformanceCounterImpl(lpPerformanceCount);
}

rpsHookAPI DWORD rpstimeGetTime( void )
{
    return rpsTimeModule::getInstance()->rpstimeGetTimeImpl();
}


rpsHookInfo g_hookinfo[] = {
    rpsDefineHookInfo("kernel32.dll", QueryPerformanceCounter   ),
    rpsDefineHookInfo("winmm.dll",    timeGetTime               ),

    rpsHookInfo(nullptr, nullptr, 0, nullptr, nullptr),
};

const char*     rpsTimeModule::getModuleName() const    { return "rpsTimeModule"; }
rpsHookInfo*    rpsTimeModule::getHooks() const         { return g_hookinfo; }

rpsTimeModule* rpsTimeModule::getInstance()
{
    static rpsTimeModule *s_inst = new rpsTimeModule();
    return s_inst;
}

rpsTimeModule::rpsTimeModule()
    : m_timescale(1.0f)
{
    m_qpc_last.QuadPart = 0;
    m_qpc_current.QuadPart = 0;
    m_tgt_last = 0;
    m_tgt_current = 0;
}

rpsTimeModule::~rpsTimeModule()
{
}

void rpsTimeModule::initialize()
{
}

void rpsTimeModule::serialize(rpsArchive &ar)
{
    ar & m_timescale & m_qpc_last.QuadPart & m_qpc_current.QuadPart & m_tgt_last & m_tgt_current;
    if(ar.isReader()) {
        if(vaQueryPerformanceCounter) {
            vaQueryPerformanceCounter(&m_qpc_last);
        }
        if(vatimeGetTime) {
            m_tgt_last = vatimeGetTime();
        }
    }
}

void rpsTimeModule::handleMessage( rpsMessage &m )
{
    if(strcmp(m.command, "setTimeScale")==0) {
        setTimeScale(m.value.cast<float>());
        return;
    }
}

}

BOOL rpsTimeModule::rpsQueryPerformanceCounterImpl( LARGE_INTEGER *lpPerformanceCount )
{
    rpsMutex::ScopedLock lock(m_mutex);

    LARGE_INTEGER r;
    BOOL b = vaQueryPerformanceCounter(&r);
    if(b) {
        int64_t gap = r.QuadPart - m_qpc_last.QuadPart;
        m_qpc_current.QuadPart += int64_t(gap * m_timescale);
        m_qpc_last = r;
        *lpPerformanceCount = m_qpc_current;
    }
    return b;
}

DWORD rpsTimeModule::rpstimeGetTimeImpl()
{
    rpsMutex::ScopedLock lock(m_mutex);

    DWORD ret = vatimeGetTime();
    DWORD gap = ret - m_tgt_last;
    m_tgt_current += DWORD(gap * m_timescale);
    m_tgt_last = ret;
    return m_tgt_current;
}

rpsDLLExport rpsIModule* rpsCreateTimeModule() { return rpsTimeModule::getInstance(); }
