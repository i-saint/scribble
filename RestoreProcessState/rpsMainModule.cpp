#include "rpsPCH.h"
#include "rpsInternal.h"
#include "rpsCommunicator.h"


extern rpsIModule* rpsCreateMemoryModule();
extern rpsIModule* rpsCreateHandleManager();
extern rpsIModule* rpsCreateSyncModule();
extern rpsIModule* rpsCreateThreadModule();
extern rpsIModule* rpsCreateFileModule();
extern rpsIModule* rpsCreateTimeModule();

static rpsModuleCreator g_mcreators[] = {
    rpsCreateMemoryModule,
    rpsCreateHandleManager,
    rpsCreateSyncModule,
    rpsCreateThreadModule,
    rpsCreateFileModule,
    rpsCreateTimeModule,

    nullptr,
};
rpsDLLExport rpsModuleCreator*  rpsGetModuleCreators()      { return g_mcreators; }



DWORD __stdcall rpsMainThread(LPVOID lpThreadParameter)
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


// todo: 外部リスト化
static bool ShouldHook(HMODULE mod)
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

    const char *whitelist[] = {
        "wuvorbis.dll",
        "x3daudio1_7.dll",
    };
    for(int i=0; i<_countof(whitelist); ++i) {
        if(strstr(path, whitelist[i])) { return true; }
    }

    return false;
}

rpsMainModule::rpsMainModule()
{
    rpsLogInfo("rpsMainModule::rpsMainModule()");
    g_inst = this;

    DWORD tid;
    ::CreateThread(nullptr, 0, rpsMainThread, this, 0, &tid);

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
    // override import table
    rpsEnumerateModules([&](HMODULE mod){
        if(!ShouldHook(mod)) { return; }
        rpsEach(m_hooks, [&](DLLHookTable::value_type &hp){
            rpsEnumerateDLLImports(mod, hp.first.c_str(), [&](const char *name, void *&func){
                FuncHookTable &htab = hp.second;
                auto it = htab.find(rps_string(name));
                if(it!=htab.end()) {
                    rpsForceWrite<void*>(func, it->second[0]->hookfunc);
                }
            });
        });
    });
    // override export table
    rpsEach(m_hooks, [&](DLLHookTable::value_type &hp){
        if(HMODULE mod = ::LoadLibraryA(hp.first.c_str())) {
            FuncHookTable &htab = hp.second;
            rpsEach(htab, [&](FuncHookTable::value_type &hp2){
                rpsHookInfo *hinfo = hp2.second[0];
                // 単純化のためメインモジュールに限定
                //rpsOverrideDLLExport(mod, hinfo->funcname, hinfo->hookfunc, nullptr);
            });
        }
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

void rpsMainModule::processRequest()
{
    while(!m_requests.empty()) {
        ::Sleep(1);
    }
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
        ::Sleep(1);
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
    rpsMainModule::getInstance()->processRequest();
}

rpsAPI void rpsLoadState(const char *path_to_infile)
{
    rpsMainModule::SerializeRequest req (path_to_infile, rpsArchive::Reader);
    rpsMainModule::getInstance()->pushRequest(req);
    rpsMainModule::getInstance()->processRequest();
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
