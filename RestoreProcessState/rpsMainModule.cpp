#include "rpsPCH.h"
#include "rpsInternal.h"
#include "rpsCommunicator.h"


extern rpsIModule* rpsCreateMemoryModule();
extern rpsIModule* rpsCreateThreadModule();
extern rpsIModule* rpsCreateFileModule();
extern rpsIModule* rpsCreateTimeModule();

static rpsModuleCreator g_mcreators[] = {
    rpsCreateMemoryModule,
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


rpsMainModule::rpsMainModule()
{
    g_inst = this;

    DWORD tid;
    ::CreateThread(nullptr, 0, rpsMainThread, this, 0, &tid);

    rpsModuleCreator *ctab = rpsGetModuleCreators();
    for(size_t mi=0; ; ++mi) {
        if(!ctab[mi]) { break; }
        rpsIModule *mod = ctab[mi]();
        mod->initialize();
        m_modules[mod->getModuleName()] = mod;

        rpsHookInfo *hooks = mod->getHooks();
        for(size_t hi=0; ; ++hi) {
            if(!hooks[hi].dllname) { break; }
            m_hooks[hooks[hi].dllname][hooks[hi].funcname].push_back(&hooks[hi]);
        }
    }

    HMODULE main_module = ::GetModuleHandleA(nullptr);
    rpsEnumerateModules([&](HMODULE mod){
        rpsEach(m_hooks, [&](DLLHookTable::value_type &hp){
            rpsEnumerateDLLImports(mod, hp.first.c_str(), [&](const char *name, void *&func){
                FuncHookTable &htab = hp.second;
                auto it = htab.find(rps_string(name));
                if(it!=htab.end()) {
                    Hooks &hooks = it->second;
                    rpsREach(hooks, [&](rpsHookInfo *hinfo){
                        void *orig = func;
                        // 単純化のためメインモジュールに限定
                        if(mod==main_module) {
                            rpsForceWrite<void*>(func, hinfo->hookfunc);
                        }
                        if(hinfo->origfunc) {
                            *hinfo->origfunc = orig;
                        }
                    });
                }
            });
        });
    });
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
                serializeImpl(req.path.c_str(), req.mode);
            });
            m_requests.clear();
        }
        ::Sleep(1);
    }
}

void rpsMainModule::sendMessage( rpsMessage &m )
{
    auto it = m_modules.find(m.modulename);
    if(it!=m_modules.end()) {
        it->second->handleMessage(m);
    }
}


rpsAPI void rpsInitialize()
{
    if(g_inst) { return; }
    rpsInitializeFoundation();
    rpsExecExclusive([](){
        rpsMainModule::initialize();
    });
}

rpsAPI void rpsSaveState(const char *path_to_outfile)
{
    rpsMainModule::SerializeRequest req = {rps_string(path_to_outfile), rpsArchive::Writer};
    rpsMainModule::getInstance()->pushRequest(req);
    rpsMainModule::getInstance()->processRequest();
}

rpsAPI void rpsLoadState(const char *path_to_infile)
{
    rpsMainModule::SerializeRequest req = {rps_string(path_to_infile), rpsArchive::Reader};
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
        rpsInitialize();
    }
    else if(fdwReason==DLL_PROCESS_DETACH) {
    }
    return TRUE;
}
