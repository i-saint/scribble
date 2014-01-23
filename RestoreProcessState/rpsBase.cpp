#include "rpsInternal.h"


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
    , m_mode(Unknown)
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
    m_mode = mode;
    m_file = fopen(path_to_file, m_mode==Reader ? "rb" : "wb");
    return m_file!=nullptr;
}

void rpsArchive::close()
{
    if(m_file) {
        fclose(m_file);
    }
}


LPTHREAD_START_ROUTINE;

DWORD __stdcall rpsMainThread(LPVOID lpThreadParameter)
{
    ((rpsProcessState*)lpThreadParameter)->mainloop();
    return 0;
}


void rpsProcessState::initialize()
{
    rpsInitializeMalloc();
    getInstance();
}

rpsProcessState* rpsProcessState::getInstance()
{
    static rpsProcessState *s_inst = new rpsProcessState();
    return s_inst;
}

void rpsProcessState::serialize(const char *path, rpsArchive::Mode mode)
{
    SerializeRequest req = {rps_string(path), mode};
    getInstance()->m_requests.push_back(req);
    while(!getInstance()->m_requests.empty()) {
        ::Sleep(1);
    }
}


rpsProcessState::rpsProcessState()
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

rpsProcessState::~rpsProcessState()
{
}

void rpsProcessState::serializeImpl(rpsArchive &ar)
{
    eachModules([&](rpsIModule *mod){
        mod->serialize(ar);
    });
}

void rpsProcessState::serializeImpl(const char *path, rpsArchive::Mode mode)
{
    rpsArchive ar;
    if(ar.open(path, mode)) {
        serializeImpl(ar);
    }
}

void rpsProcessState::mainloop()
{
    for(;;) {
        if(!m_requests.empty()) {
            rpsEach(m_requests, [&](SerializeRequest &req){
                serializeImpl(req.path.c_str(), req.mode);
            });
            m_requests.clear();
        }
        ::Sleep(1);
    }
}


void rpsInitialize()
{
    rpsProcessState::initialize();
}

void rpsSaveState(const char *path_to_outfile)
{
    rpsProcessState::serialize(path_to_outfile, rpsArchive::Writer);
}

void rpsLoadState(const char *path_to_infile)
{
    rpsProcessState::serialize(path_to_infile, rpsArchive::Reader);
}

//
//void rpsRestore()
//{
//	CONTEXT ctx;
//}

