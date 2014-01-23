#include "rpsPCH.h"
#include "rpsInternal.h"
#include "rpsNetwork.h"


extern rpsIModule* rpsCreateMemory();
extern rpsIModule* rpsCreateThreads();
extern rpsIModule* rpsCreateFiles();

static rpsModuleCreator g_mcreators[] = {
    rpsCreateMemory,
    rpsCreateThreads,
    rpsCreateFiles,
};
rpsDLLExport size_t             rpsGetNumModuleCreators()   { return _countof(g_mcreators); }
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

rpsMainModule* rpsMainModule::getInstance()
{
    static rpsMainModule *s_inst = new rpsMainModule();
    return s_inst;
}

void rpsMainModule::serialize(const char *path, rpsArchive::Mode mode)
{
    SerializeRequest req = {rps_string(path), mode};
    getInstance()->processRequest(req);
}


rpsMainModule::rpsMainModule()
{
    DWORD tid;
    ::CreateThread(nullptr, 0, rpsMainThread, this, 0, &tid);

    rpsModuleCreator *ctab = rpsGetModuleCreators();
    size_t n = rpsGetNumModuleCreators();
    for(size_t mi=0; mi<n; ++mi) {
        rpsIModule *mod = ctab[mi]();
        m_modules[mod->getModuleName()] = mod;

        rpsHookInfo *hooks = mod->getHooks();
        size_t num_hooks = mod->getNumHooks();
        for(size_t hi=0; hi<num_hooks; ++hi) {
            m_hooks[hooks[hi].dllname][hooks[hi].funcname].push_back(&hooks[hi]);
        }
    }

    rpsEnumerateModules([&](HMODULE mod){
        rpsEach(m_hooks, [&](DLLHookTable::value_type &hp){
            rpsEnumerateDLLImports(mod, hp.first.c_str(), [&](const char *name, void *&func){
                FuncHookTable &htab = hp.second;
                auto it = htab.find(rps_string(name));
                if(it!=htab.end()) {
                    Hooks &hooks = it->second;
                    rpsREach(hooks, [&](rpsHookInfo *hinfo){
                        void *orig = func;
                        rpsForceWrite<void*>(func, hinfo->hookfunc);
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
                rpsOverrideDLLExport(mod, hinfo->funcname, hinfo->hookfunc, nullptr);
            });
        }
    });
}

rpsMainModule::~rpsMainModule()
{
}

void rpsMainModule::processRequest(SerializeRequest &req)
{
    {
        rpsMutex::ScopedLock lock(m_mtx_requests);
        m_requests.push_back(req);
    }
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


rpsAPI void rpsInitialize()
{
    rpsInitializeFoundation();
    rpsExecExclusive([](){
        rpsMainModule::initialize();
    });
}

rpsAPI void rpsSaveState(const char *path_to_outfile)
{
    rpsMainModule::serialize(path_to_outfile, rpsArchive::Writer);
}

rpsAPI void rpsLoadState(const char *path_to_infile)
{
    rpsMainModule::serialize(path_to_infile, rpsArchive::Reader);
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
