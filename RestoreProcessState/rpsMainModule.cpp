#include "rpsPCH.h"
#include "rpsInternal.h"
#include "rpsCommunicator.h"
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")



rpsAPI bool rpsIsSerializableModule(const char *path)
{
    return !rpsIsIgnoredModule(path);
}

rpsAPI bool rpsIsSerializableModule(HMODULE mod)
{
    static const HMODULE main_module = GetModuleHandleA(nullptr);
    if (mod == main_module) {
        return true;
    }

    char path[MAX_PATH + 1];
    GetModuleFileNameA(mod, path, sizeof(path));
    for (int i = 0; path[i] != '\0'; ++i) {
        path[i] = tolower(path[i]);
    }
    return rpsIsSerializableModule(path);
}

template<size_t N>
inline bool match(const char *str, const char *(&patterns)[N])
{
    for (int i = 0; i < N; ++i) {
        if (strstr(str, patterns[i])) {
            return true;
        }
    }
    return false;
}

rpsAPI bool rpsIsIgnoredModule(const char *path)
{
    rps_string ipath = path;
    for (size_t i = 0; i < ipath.size(); ++i) {
        ipath[i] = tolower(ipath[i]);
    }

    const char *ignorelist[] = {
        "rps32.dll",
        "rps64.dll",

        "zlib1.dll",
        "libpng15.dll",
        "libvorbisfile.dll",
        "libogg.dll",
        "libvorbis.dll",
        "openal32.dll",


        "\\windows\\",
    };
    const char *whitelist[] = {
        "msvcp",
        "msvcr",

        "ntdll.dll",
        "kernel32.dll",
        "kernelbase.dll",
    };

    if (match(ipath.c_str(), whitelist)) {
        return false;
    }
    if (match(ipath.c_str(), ignorelist)) {
        return true;
    }
    return false;
}

rpsAPI bool rpsIsIgnoredModule(const wchar_t *path)
{
    std::wstring tmp = path;

    size_t len = wcstombs(nullptr, tmp.c_str(), 0);
    if (len == size_t(-1)) { return false; }

    std::string dst;
    dst.resize(len);
    wcstombs(&dst[0], tmp.c_str(), len);
    return rpsIsIgnoredModule(dst.c_str());
}




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
    if (!rpsIsIgnoredModule(lpFileName)) {
        rpsMainModule::getInstance()->setHooks(ret);
    }
    return ret;
}

rpsHookAPI HMODULE WINAPI rpsLoadLibraryW(LPWSTR lpFileName)
{
    HMODULE ret = vaLoadLibraryW(lpFileName);
    if (!rpsIsIgnoredModule(lpFileName)) {
        rpsMainModule::getInstance()->setHooks(ret);
    }
    return ret;
}

rpsHookAPI HMODULE WINAPI rpsLoadLibraryExA(LPCSTR lpFileName, HANDLE hFile, DWORD dwFlags)
{
    HMODULE ret = vaLoadLibraryExA(lpFileName, hFile, dwFlags);
    if (!rpsIsIgnoredModule(lpFileName)) {
        rpsMainModule::getInstance()->setHooks(ret);
    }
    return ret;
}

rpsHookAPI HMODULE WINAPI rpsLoadLibraryExW(LPWSTR lpFileName, HANDLE hFile, DWORD dwFlags)
{
    HMODULE ret = vaLoadLibraryExW(lpFileName, hFile, dwFlags);
    if (!rpsIsIgnoredModule(lpFileName)) {
        rpsMainModule::getInstance()->setHooks(ret);
    }
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


    if (rpsIsIgnoredModule(path)) { return; }

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


static volatile bool s_ready_to_serialize;

__declspec(noinline) void rpsMainModule::callbackFromHost()
{
    while (!m_requests.empty()) {
        s_ready_to_serialize = true;
    }
    s_ready_to_serialize = false;
}

void rpsMainModule::mainloop()
{
    const int interval = 3000;
    bool enable_record = false;
    uint32_t time_prev = timeGetTime();
    uint32_t time_prog = 0;
    int last = -1;
    int pos = -1;
    char buf[256];

    rpsPrint(
        "ctrl + space: record on/off (off by default)\n"
        "ctrl + left/right: load prev/next state\n\n");

    for (;;) {
        {
            rpsMutex::ScopedLock lock(m_mtx_requests);
            if (!m_requests.empty() && s_ready_to_serialize) {
                rpsEach(m_requests, [&](SerializeRequest &req){
                    serializeImpl(req.path, req.mode);
                });
                m_requests.clear();
            }
        }

        //for(int i=0; i<10; ++i) {
        //    if(::GetAsyncKeyState('1'+i)) {
        //        char filename[64];
        //        sprintf(filename, "%c.state", '1'+i);
        //        if(::GetAsyncKeyState(VK_MENU)) {
        //            rpsMainModule::SerializeRequest req(filename, rpsArchive::Writer);
        //            pushRequest(req);
        //        }
        //        else if(::GetAsyncKeyState(VK_CONTROL)) {
        //            rpsMainModule::SerializeRequest req(filename, rpsArchive::Reader);
        //            pushRequest(req);
        //        }
        //        ::Sleep(200);
        //    }
        //}
        //::Sleep(50);

    

        if (::GetAsyncKeyState(VK_CONTROL) && ::GetAsyncKeyState(VK_SPACE)) {
            enable_record = !enable_record;
            if (enable_record) {
                rpsPrint("record start\n");
            }
            else {
                rpsPrint("record stop\n");
            }
        }
        if (enable_record) {
            uint32_t time_now = timeGetTime();
            time_prog += time_now - time_prev;
            time_prev = time_now;
            if (time_prog > interval) {
                time_prog = 0;
                sprintf(buf, "%d.state", ++pos);
                last = std::max<int>(last, pos);
                rpsMainModule::SerializeRequest req(buf, rpsArchive::Writer);
                pushRequest(req);
                rpsPrint("saved %d.state\n", pos);
            }
        }
        {
            bool needs_load = false;
            int pos_prev = pos;
            if (::GetAsyncKeyState(VK_CONTROL) && ::GetAsyncKeyState(VK_LEFT)) {
                if (time_prog<1000) {
                    --pos;
                }
                needs_load = true;
            }
            if (::GetAsyncKeyState(VK_CONTROL) && ::GetAsyncKeyState(VK_RIGHT)) {
                ++pos;
                needs_load = true;
            }
            if (needs_load && pos >= 0 && pos <= last) {
                sprintf(buf, "%d.state", pos);
                rpsMainModule::SerializeRequest req(buf, rpsArchive::Reader);
                pushRequest(req);
                ::Sleep(300);
                time_prog = 0;
                rpsPrint("loaded %d.state\n", pos);
            }
            else {
                pos = pos_prev;
            }
        }
        ::Sleep(100);
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
