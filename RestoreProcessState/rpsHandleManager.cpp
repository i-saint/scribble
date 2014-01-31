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
    rpsHandleInfo* getHandleInfo(HANDLE rps_handle);

private:
    typedef std::map<HANDLE, rpsHandleInfo, std::less<HANDLE>, rps_allocator<std::pair<HANDLE, rpsHandleInfo> > > rpsHandleInfoTable;

    rpsMutex m_mutex;
    rpsHandleInfoTable m_table;
    size_t m_handle_seed;
};
typedef rpsHandleManager rpsCurrentModule;


CloseHandleT            vaCloseHandle;
WaitForSingleObjectT    vaWaitForSingleObject;

BOOL WINAPI rpsCloseHandle(HANDLE hObject)
{
    BOOL ret = vaCloseHandle(rpsTranslateHandleC(hObject, vaCloseHandle));
    if(ret) {
        rpsCurrentModule::getInstance()->releaseHandle(hObject);
    }
    return ret;
}

DWORD WINAPI rpsWaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds)
{
    return vaWaitForSingleObject(rpsTranslateHandle(hHandle), dwMilliseconds);
}

rpsHookInfo g_hookinfo[] = {
    rpsHookInfo("kernel32.dll", "CloseHandle",         0, rpsCloseHandle,         &(void*&)vaCloseHandle),
    rpsHookInfo("kernel32.dll", "WaitForSingleObject", 0, rpsWaitForSingleObject, &(void*&)vaWaitForSingleObject),

    rpsHookInfo(nullptr, nullptr, 0, nullptr, nullptr),
};



inline rpsArchive& operator&(rpsArchive &ar, HANDLE &v)
{
    return ar & (size_t&)v;
}

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
    ar & m_table & m_handle_seed;
}

HANDLE rpsHandleManager::createHandle(rpsIModule *owner, HANDLE win_handle)
{
    if(win_handle==nullptr || win_handle==INVALID_HANDLE_VALUE) { return win_handle; }

    HANDLE ret = (void*)(++m_handle_seed + ('R'<<24));
    rpsHandleInfo info = {ret, win_handle, owner};
    m_table[ret] = info;
    return ret;
}

bool rpsHandleManager::releaseHandle(HANDLE rps_handle)
{
    auto it = m_table.find(rps_handle);
    if(it!=m_table.end()) {
        m_table.erase(it);
        return true;
    }
    return false;
}

HANDLE rpsHandleManager::findWinHandle( HANDLE rps_handle )
{
    if(!rpsIsRpsHandle(rps_handle)) { return rps_handle; }

    auto it = m_table.find(rps_handle);
    if(it!=m_table.end()) {
        return it->second.win_handle;
    }
    return rps_handle;
}

rpsHandleInfo* rpsHandleManager::getHandleInfo( HANDLE rps_handle )
{
    if(!rpsIsRpsHandle(rps_handle)) { return nullptr; }

    auto it = m_table.find(rps_handle);
    if(it!=m_table.end()) {
        return &it->second;
    }
    return nullptr;
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

rpsAPI HANDLE rpsTranslateHandle(HANDLE rps_handle)
{
    return rpsHandleManager::getInstance()->findWinHandle(rps_handle);
}

rpsAPI rpsHandleInfo* rpsGetHandleInfo(HANDLE rps_handle)
{
    return rpsHandleManager::getInstance()->getHandleInfo(rps_handle);
}

rpsDLLExport rpsIModule* rpsCreateHandleManager() { return rpsHandleManager::getInstance(); }
