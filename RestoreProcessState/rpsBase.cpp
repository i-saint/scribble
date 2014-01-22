#include "rps.h"
#include "rpsInlines.h"

extern void rpsInitializeMalloc();
extern rpsIModule* rpsCreateMemory();
extern rpsIModule* rpsCreateThreads();
extern rpsIModule* rpsCreateFiles();

static rpsModuleCreator g_mcreators[] = {
    rpsCreateMemory,
    rpsCreateThreads,
    rpsCreateFiles,
};

rpsDLLExport size_t				rpsGetNumModuleCreators()	{ return _countof(g_mcreators); }
rpsDLLExport rpsModuleCreator*	rpsGetModuleCreators()		{ return g_mcreators; }



rpsArchive::rpsArchive()
    : m_file(nullptr)
    , m_is_reader(false)
{
}

rpsArchive::~rpsArchive()
{
    close();
}

void rpsArchive::read(void *dst, size_t size)
{
    fread(dst, size, 1, m_file);
}

void rpsArchive::write(const void *data, size_t size)
{
    fwrite(data, size, 1, m_file);
}

void rpsArchive::io(void *dst, size_t size)
{
    if(isReader()) {
        read(dst, size);
    }
    else {
        write(dst, size);
    }
}

bool rpsArchive::open(const char *path_to_file, Mode mode)
{
    close();
    m_file = fopen(path_to_file, mode==Reader ? "rb" : "wb");
    return m_file!=nullptr;
}

void rpsArchive::close()
{
    if(m_file) {
        fclose(m_file);
    }
}

void rpsProcessState::initialize()
{
    rpsInitializeMalloc();
    getInstance();
}

rpsProcessState* rpsProcessState::getInstance()
{
    static rpsProcessState *g_procstate = nullptr;
    if(!g_procstate) {
        g_procstate = new rpsProcessState();
    }
    return g_procstate;
}

rpsProcessState::rpsProcessState()
{
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
        rpsEach(m_hooks, [&](DllHookTable::value_type &hp){
            rpsEnumerateDLLImports(mod, hp.first.c_str(), [&](const char *name, void *&func){
                HookTable &htab = hp.second;
                auto it = htab.find(rps_string(name));
                if(it!=htab.end()) {
                    Hooks &hooks = it->second;
                    rpsEach(hooks, [&](rpsHookInfo *hinfo){
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
}

rpsProcessState::~rpsProcessState()
{
}

void rpsProcessState::serialize(rpsArchive &ar)
{
    eachModules([&](rpsIModule *mod){
        mod->serialize(ar);
    });
}

void rpsProcessState::serialize(const char *path, rpsArchive::Mode mode)
{
    rpsArchive ar;
    if(ar.open(path, mode)) {
        serialize(ar);
    }
}

void rpsProcessState::setHooks()
{

}




//void rpsInitialize()
//{
//	rpsEnumerateModules([](HMODULE mod){
//		rpsEnumerateDLLImports(mod, "kernel32.dll", [&](const char *name, void *&func){
//			if     (strcmp(name, "HeapAlloc")	==0) { rpsForceWrite<void*>(func, rpsHeapAlloc); }
//			else if(strcmp(name, "HeapReAlloc")	==0) { rpsForceWrite<void*>(func, rpsHeapReAlloc); }
//			else if(strcmp(name, "HeapFree")	==0) { rpsForceWrite<void*>(func, rpsHeapFree); }
//		});
//	});
//}
//
//
//void rpsRestore()
//{
//	CONTEXT ctx;
//}

