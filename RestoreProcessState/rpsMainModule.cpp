#include "rpsPCH.h"
#include "rpsInternal.h"
#include "rpsCommunicator.h"


extern rpsIModule* rpsCreateMemoryModule();
extern rpsIModule* rpsCreateHandleManager();
extern rpsIModule* rpsCreateSyncModule();
extern rpsIModule* rpsCreateThreadModule();
extern rpsIModule* rpsCreateFileModule();
extern rpsIModule* rpsCreateTimeModule();
extern rpsIModule* rpsCreateD3D9Module();


static rpsModuleCreator g_mcreators[] = {
    rpsCreateMemoryModule,
    rpsCreateHandleManager,
    rpsCreateSyncModule,
    rpsCreateThreadModule,
    rpsCreateFileModule,
    rpsCreateTimeModule,
    rpsCreateD3D9Module,

    nullptr,
};
rpsDLLExport rpsModuleCreator*  rpsGetModuleCreators()      { return g_mcreators; }


namespace {

LoadLibraryAT   vaLoadLibraryA;
LoadLibraryWT   vaLoadLibraryW;
LoadLibraryExAT vaLoadLibraryExA;
LoadLibraryExWT vaLoadLibraryExW;

rpsHookAPI HMODULE WINAPI rpsLoadLibraryA(LPCSTR lpFileName)
{
    HMODULE ret = vaLoadLibraryA(lpFileName);
    rpsMainModule::getInstance()->setHooks(ret);
    return ret;
}

rpsHookAPI HMODULE WINAPI rpsLoadLibraryW(LPWSTR lpFileName)
{
    HMODULE ret = vaLoadLibraryW(lpFileName);
    rpsMainModule::getInstance()->setHooks(ret);
    return ret;
}

rpsHookAPI HMODULE WINAPI rpsLoadLibraryExA(LPCSTR lpFileName, HANDLE hFile, DWORD dwFlags)
{
    HMODULE ret = vaLoadLibraryExA(lpFileName, hFile, dwFlags);
    rpsMainModule::getInstance()->setHooks(ret);
    return ret;
}

rpsHookAPI HMODULE WINAPI rpsLoadLibraryExW(LPWSTR lpFileName, HANDLE hFile, DWORD dwFlags)
{
    HMODULE ret = vaLoadLibraryExW(lpFileName, hFile, dwFlags);
    rpsMainModule::getInstance()->setHooks(ret);
    return ret;
}

rpsHookInfo g_loadlibraryhooks[] = {
    rpsDefineHookInfo2("kernel32.dll", LoadLibraryA, rpsE_IEATOverride),
    rpsDefineHookInfo2("kernel32.dll", LoadLibraryW, rpsE_IEATOverride),
    rpsDefineHookInfo2("kernel32.dll", LoadLibraryExA, rpsE_IEATOverride),
    rpsDefineHookInfo2("kernel32.dll", LoadLibraryExW, rpsE_IEATOverride),
    rpsHookInfo(nullptr, nullptr, 0, nullptr, nullptr),
};

} // namespace



static DWORD __stdcall rpsMainThread(LPVOID lpThreadParameter)
{
    ((rpsMainModule*)lpThreadParameter)->mainloop();
    return 0;
}

void rpsMainModule::initialize()
{
    rpsInitializeFoundation();
    rpsInitializeNetwork();
    getInstance();
}

static rpsMainModule *g_inst;

rpsMainModule* rpsMainModule::getInstance()
{
    if(!g_inst) { new rpsMainModule(); }
    return g_inst;
}


rpsAPI bool rpsIsSerializableModule(HMODULE mod)
{
    static const HMODULE main_module = GetModuleHandleA(nullptr);
    if(mod==main_module) {
        return true;
    }

    char path[MAX_PATH+1];
    GetModuleFileNameA(mod, path, sizeof(path));
    for(int i=0; path[i]!='\0'; ++i) {
        path[i] = tolower(path[i]);
    }
    return rpsIsSerializableModule(path);
}

rpsAPI bool rpsIsSerializableModule(const char *path)
{
    // todo: 外部リスト化
    const char *whitelist[] = {
        "msvcr120.dll",
        "msvcr120d.dll",
        "msvcr110.dll",
        "msvcr110d.dll",
        "msvcr100.dll",
        "msvcr100d.dll",
        "msvcr90.dll",
        "msvcr90d.dll",
        "msvcr80.dll",
        "msvcr80d.dll",
        //"msvcrt.dll",
        "wuvorbis.dll",
        "x3daudio1_7.dll",
    };
    for(int i=0; i<_countof(whitelist); ++i) {
        if(strstr(path, whitelist[i])) { return true; }
    }
    return false;
}


rpsMainModule::rpsMainModule()
    : m_tid(0)
{
    rpsLogInfo("rpsMainModule::rpsMainModule()");
    g_inst = this;

    ::CreateThread(nullptr, 0, rpsMainThread, this, 0, &m_tid);

    rpsLogInfo("rpsMainModule: creating modules");
    rpsModuleCreator *ctab = rpsGetModuleCreators();
    for(size_t mi=0; ; ++mi) {
        if(!ctab[mi]) { break; }
        rpsIModule *mod = ctab[mi]();
        m_modules.push_back(mod);
        m_module_map[mod->getModuleName()] = mod;

        if(rpsHookInfo *hinfo = mod->getHooks()) {
            for(size_t hi=0; ; ++hi) {
                if(!hinfo[hi].dllname) { break; }
                Hooks &hooks = m_hooks[ hinfo[hi].dllname ][ hinfo[hi].funcname ];
                hooks.push_back(&hinfo[hi]);
            }
        }
    }
    if(rpsHookInfo *hinfo = g_loadlibraryhooks) {
        for(size_t hi=0; ; ++hi) {
            if(!hinfo[hi].dllname) { break; }
            Hooks &hooks = m_hooks[ hinfo[hi].dllname ][ hinfo[hi].funcname ];
            hooks.push_back(&hinfo[hi]);
        }
    }

    rpsLogInfo("rpsMainModule: setting hook functions");
    // gather original functions
    rpsEach(m_hooks, [&](DLLHookTable::value_type &hp){
        if(HMODULE mod=::GetModuleHandleA(hp.first.c_str())) {
            FuncHookTable &htab = hp.second;
            rpsEach(htab, [&](FuncHookTable::value_type &fp){
                if(void *proc=::GetProcAddress(mod, fp.first.c_str())) {
                    rpsREach(fp.second, [&](rpsHookInfo *hook){
                        *hook->origfunc = proc;
                        proc = hook->hookfunc;
                    });
                }
            });
        }
    });
    // override import & export address tables
    rpsEnumerateModules([&](HMODULE mod){
        setHooks(mod);
    });

    rpsLogInfo("rpsMainModule: initializing modules");
    rpsEach(m_modules, [&](rpsIModule *mod){
        mod->initialize();
    });

    rpsLogInfo("rpsMainModule: running communicator");
    m_communicator = new rpsCommunicator();
    m_communicator->run(rpsDefaultPort);
}

rpsMainModule::~rpsMainModule()
{
}

void rpsMainModule::pushRequest( SerializeRequest &req )
{
    rpsMutex::ScopedLock lock(m_mtx_requests);
    m_requests.push_back(req);
}

void rpsMainModule::waitForCompleteRequests()
{
    while(!m_requests.empty()) {
        ::Sleep(1);
    }
}

void rpsMainModule::setHooks( HMODULE mod )
{
    char path[MAX_PATH+1];
    char *filename = nullptr;
    GetModuleFileNameA(mod, path, sizeof(path));
    for(int i=0; path[i]!='\0'; ++i) {
        path[i] = tolower(path[i]);
        if(path[i]=='\\') {
            filename = path+i+1;
        }
    }
    if(strcmp(filename, "rps32.dll")==0 || strcmp(filename, "rps64.dll")==0) { return; }

    rpsEach(m_hooks, [&](DLLHookTable::value_type &dllname_hooks){
        rpsEnumerateDLLImports(mod, dllname_hooks.first.c_str(), [&](const char *name, void *&func){
            FuncHookTable &htab = dllname_hooks.second;
            auto it = htab.find(rps_string(name));
            if(it!=htab.end() && (it->second[0]->flags&rpsE_IATOverride)!=0) {
                rpsForceWrite<void*>(func, it->second[0]->hookfunc);
            }
        });
        if(dllname_hooks.first==filename) {
            FuncHookTable &htab = dllname_hooks.second;
            rpsEach(htab, [&](FuncHookTable::value_type &funcname_hooks){
                if((funcname_hooks.second[0]->flags&rpsE_EATOverride)!=0) {
                    void *proc = rpsOverrideDLLExport(mod, funcname_hooks.first.c_str(), funcname_hooks.second[0]->hookfunc, nullptr);
                    if(*funcname_hooks.second[0]->origfunc==nullptr) {
                        rpsREach(funcname_hooks.second, [&](rpsHookInfo *hook){
                            *hook->origfunc = proc;
                            proc = hook->hookfunc;
                        });
                    }
                }
            });
        }
    });
}

void rpsMainModule::serializeImpl(rpsArchive &ar)
{
    rpsExecExclusive([&](){
        eachModules([&](rpsIModule *mod){
            mod->serialize(ar);
        });
    });
}

void rpsMainModule::serializeImpl(const char *path, rpsArchive::Mode mode)
{
    rpsArchive ar;
    if(ar.open(path, mode)) {
        serializeImpl(ar);
    }
    if(ar.isWriter()) {
        rpsLogInfo("rps: save completed %s", path);
    }
    else {
        rpsLogInfo("rps: load completed %s", path);
    }
}

void rpsMainModule::mainloop()
{
    for(;;) {
        if(!m_requests.empty()) {
            rpsMutex::ScopedLock lock(m_mtx_requests);
            rpsEach(m_requests, [&](SerializeRequest &req){
                serializeImpl(req.path, req.mode);
            });
            m_requests.clear();
        }

        for(int i=0; i<10; ++i) {
            if(::GetAsyncKeyState('1'+i)) {
                char filename[64];
                sprintf(filename, "rpsstate%c.bin", '1'+i);
                if(::GetAsyncKeyState(VK_MENU)) {
                    rpsMainModule::SerializeRequest req(filename, rpsArchive::Writer);
                    pushRequest(req);
                }
                else if(::GetAsyncKeyState(VK_CONTROL)) {
                    rpsMainModule::SerializeRequest req(filename, rpsArchive::Reader);
                    pushRequest(req);
                }
                ::Sleep(200);
            }
        }
        ::Sleep(50);
    }
}

void rpsMainModule::sendMessage( rpsMessage &m )
{
    auto it = m_module_map.find(m.modulename);
    if(it!=m_module_map.end()) {
        it->second->handleMessage(m);
    }
}


rpsAPI void rpsInitialize()
{
    if(g_inst) { return; }
    rpsLogInfo("rpsInitialize()");
    rpsInitializeFoundation();
    rpsExecExclusive([](){
        rpsMainModule::initialize();
    });
}

rpsAPI void rpsSaveState(const char *path_to_outfile)
{
    rpsMainModule::SerializeRequest req(path_to_outfile, rpsArchive::Writer);
    rpsMainModule::getInstance()->pushRequest(req);
    rpsMainModule::getInstance()->waitForCompleteRequests();
}

rpsAPI void rpsLoadState(const char *path_to_infile)
{
    rpsMainModule::SerializeRequest req (path_to_infile, rpsArchive::Reader);
    rpsMainModule::getInstance()->pushRequest(req);
    rpsMainModule::getInstance()->waitForCompleteRequests();
}

rpsAPI void rpsSendMessage(rpsMessage &mes)
{
    rpsMainModule::getInstance()->sendMessage(mes);
}


BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    if(fdwReason==DLL_PROCESS_ATTACH) {
        ::Sleep(8000);
        rpsInitialize();
    }
    else if(fdwReason==DLL_PROCESS_DETACH) {
    }
    return TRUE;
}
