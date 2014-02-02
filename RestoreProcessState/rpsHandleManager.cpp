#include "rpsPCH.h"
#include "rpsInternal.h"

namespace {

class rpsHandleManager : public rpsIModule
{
public:
    static rpsHandleManager* getInstance();

    rpsHandleManager();
    ~rpsHandleManager();
    virtual const char*     getModuleName() const;
    virtual rpsHookInfo*    getHooks() const;
    virtual void initialize();
    virtual void serialize(rpsArchive &ar);

    rpsHandleInfo* findHandleInfoByRpsHandle(HANDLE rps_handle);
    HANDLE createHandle(rpsIModule *owner, HANDLE win_handle);
    bool releaseHandle(HANDLE rps_handle);
    HANDLE findWinHandle(HANDLE rps_handle);
    rpsHandleInfo* getHandleInfoByRpsHandle(HANDLE rps_handle);
    rpsHandleInfo* getHandleInfoByWinHandle(HANDLE rps_handle);

private:
    typedef std::map<HANDLE, rpsHandleInfo, std::less<HANDLE>, rps_allocator<std::pair<HANDLE, rpsHandleInfo> > > HandleInfoTable;

    rpsMutex m_mutex;
    HandleInfoTable m_rps_to_win;
    size_t m_handle_seed;
};
typedef rpsHandleManager rpsCurrentModule;


CloseHandleT            vaCloseHandle;
WaitForSingleObjectT    vaWaitForSingleObject;

rpsHookAPI BOOL WINAPI rpsCloseHandle(HANDLE hObject)
{
    BOOL ret = vaCloseHandle(rpsToWinHandleC(hObject, vaCloseHandle));
    if(ret) {
        rpsCurrentModule::getInstance()->releaseHandle(hObject);
    }
    return ret;
}

rpsHookAPI DWORD WINAPI rpsWaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds)
{
    return vaWaitForSingleObject(rpsToWinHandleC(hHandle, vaWaitForSingleObject), dwMilliseconds);
}

rpsHookInfo g_hookinfo[] = {
    rpsDefineHookInfo("kernel32.dll", CloseHandle        ),
    rpsDefineHookInfo("kernel32.dll", WaitForSingleObject),

    rpsHookInfo(nullptr, nullptr, 0, nullptr, nullptr),
};



inline rpsArchive& operator&(rpsArchive &ar, rpsHandleInfo &v)
{
    ar.io(&v, sizeof(v));
    return ar;
}

const char* rpsHandleManager::getModuleName() const { return "rpsHandleManager"; }
rpsHookInfo* rpsHandleManager::getHooks() const { return g_hookinfo; }

rpsHandleManager* rpsHandleManager::getInstance()
{
    static rpsHandleManager *s_inst = new rpsHandleManager();
    return s_inst;
}

rpsHandleManager::rpsHandleManager()
    : m_handle_seed(0)
{
}

rpsHandleManager::~rpsHandleManager()
{
}

void rpsHandleManager::initialize()
{

}
void rpsHandleManager::serialize(rpsArchive &ar)
{
    ar & m_rps_to_win & m_handle_seed;
}

HANDLE rpsHandleManager::createHandle(rpsIModule *owner, HANDLE win_handle)
{
    if(win_handle==nullptr || win_handle==INVALID_HANDLE_VALUE) { return win_handle; }

    HANDLE ret = (void*)(++m_handle_seed + ('R'<<24));
    rpsHandleInfo info = {ret, win_handle, owner};
    m_rps_to_win[ret] = info;
    return ret;
}

bool rpsHandleManager::releaseHandle(HANDLE rps_handle)
{
    auto it = m_rps_to_win.find(rps_handle);
    if(it!=m_rps_to_win.end()) {
        m_rps_to_win.erase(it);
        return true;
    }
    return false;
}

rpsHandleInfo* rpsHandleManager::getHandleInfoByRpsHandle( HANDLE rps_handle )
{
    if(!rpsIsRpsHandle(rps_handle)) { return nullptr; }

    auto it = m_rps_to_win.find(rps_handle);
    if(it!=m_rps_to_win.end()) {
        return &it->second;
    }
    return nullptr;
}

rpsHandleInfo* rpsHandleManager::getHandleInfoByWinHandle( HANDLE win_handle )
{
    if(rpsIsRpsHandle(win_handle)) { return nullptr; }

    rpsHandleInfo *ret = nullptr;
    rpsEach(m_rps_to_win, [&](HandleInfoTable::value_type &v){
        if(v.second.win_handle==win_handle) {
            ret = &v.second;
        }
    });
    return ret;
}

} // namespace


rpsAPI HANDLE rpsCreateHandle(rpsIModule *owner, HANDLE win_handle)
{
    return rpsHandleManager::getInstance()->createHandle(owner, win_handle);
}

rpsAPI bool rpsReleaseHandle(HANDLE rps_handle)
{
    return rpsHandleManager::getInstance()->releaseHandle(rps_handle);
}

rpsAPI HANDLE rpsToWinHandle(HANDLE rps_handle)
{
    if(!rpsIsRpsHandle(rps_handle)) { return rps_handle; }
    if(rpsHandleInfo *info = rpsHandleManager::getInstance()->getHandleInfoByRpsHandle(rps_handle)) {
        return info->win_handle;
    }
    return nullptr;
}
rpsAPI HANDLE rpsToRpsHandle(HANDLE win_handle)
{
    if(rpsIsRpsHandle(win_handle)) { return win_handle; }
    if(rpsHandleInfo *info = rpsHandleManager::getInstance()->getHandleInfoByWinHandle(win_handle)) {
        return info->rps_handle;
    }
    return nullptr;
}

rpsAPI rpsHandleInfo* rpsGetHandleInfo(HANDLE rps_handle)
{
    return rpsHandleManager::getInstance()->getHandleInfoByRpsHandle(rps_handle);
}

rpsDLLExport rpsIModule* rpsCreateHandleManager() { return rpsHandleManager::getInstance(); }
