﻿#ifndef rpsInlines_h
#define rpsInlines_h


inline rps_wstring rpsL(const char *mbs)
{
    if(mbs==nullptr) { return rps_wstring(); }
    rps_wstring ret;
    size_t mblen = strlen(mbs);
    size_t len = mbstowcs(nullptr, mbs, mblen);
    ret.resize(len);
    mbstowcs(&ret[0], mbs, mblen);
    return ret;
}


template<class T>
inline void rpsForceWrite(T &dst, const T &src)
{
    DWORD old_flag;
    ::VirtualProtect(&dst, sizeof(T), PAGE_EXECUTE_READWRITE, &old_flag);
    dst = src;
    ::VirtualProtect(&dst, sizeof(T), old_flag, &old_flag);
}

template<class C, class F>
inline void rpsEach(C &cont, const F &f)
{
    std::for_each(cont.begin(), cont.end(), f);
}

template<class C, class F>
inline void rpsREach(C &cont, const F &f)
{
    std::for_each(cont.rbegin(), cont.rend(), f);
}


// F: [](DWORD thread_id) -> void
template<class F>
inline void rpsEnumerateThreads(const F &proc)
{
    DWORD pid = ::GetCurrentProcessId();
    HANDLE ss = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if(ss!=INVALID_HANDLE_VALUE) {
        THREADENTRY32 te;
        te.dwSize = sizeof(te);
        for(BOOL ok=::Thread32First(ss, &te); ok; ok=::Thread32Next(ss, &te)) {
            if(te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID)+sizeof(te.th32OwnerProcessID) &&
                te.th32OwnerProcessID==pid)
            {
                proc(te.th32ThreadID);
            }
            te.dwSize = sizeof(te);
        }
        ::CloseHandle(ss);
    }
}

// F: []() -> void
template<class F>
void rpsExecExclusive(const F &proc)
{
    std::vector<HANDLE, rps_allocator<HANDLE> > threads;
    rpsEnumerateThreads([&](DWORD tid){
        if(tid==::GetCurrentThreadId()) { return; }
        if(HANDLE thandle=::OpenThread(THREAD_ALL_ACCESS, FALSE, tid)) {
            ::SuspendThread(thandle);
            threads.push_back(thandle);
        }
    });
    proc();
    rpsEach(threads, [](HANDLE thandle){
        ::ResumeThread(thandle);
        ::CloseHandle(thandle);
    });
}


// F: [](HMODULE mod) -> void
template<class F>
inline void rpsEnumerateModules(const F &f)
{
    std::vector<HMODULE, rps_allocator<HANDLE> > modules;
    DWORD num_modules;
    ::EnumProcessModules(::GetCurrentProcess(), nullptr, 0, &num_modules);
    modules.resize(num_modules/sizeof(HMODULE));
    ::EnumProcessModules(::GetCurrentProcess(), &modules[0], num_modules, &num_modules);
    for(size_t i=0; i<modules.size(); ++i) {
        f(modules[i]);
    }
}

struct rpsModuleInfo
{
    void *base;
    size_t size;
    char filename[MAX_MODULE_NAME32+1];
    char path[MAX_PATH];
};
// F: [](rpsModuleInfo &minfo) -> void
template<class F>
inline void rpsEnumerateModulesDetailed(const F &f)
{
    HANDLE snap = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, ::GetCurrentProcessId());
    MODULEENTRY32 mod;
    mod.dwSize = sizeof(MODULEENTRY32);
    for(BOOL ok=::Module32First(snap, &mod); ok; ok=::Module32Next(snap, &mod)) {
        rpsModuleInfo mi;
        mi.base = mod.modBaseAddr;
        mi.size = mod.modBaseSize;
        memcpy(mi.path, mod.szExePath, sizeof(mi.path));
        memcpy(mi.filename, mod.szModule, sizeof(mi.filename));
        f(mi);
    }
    ::CloseHandle(snap);
}



// F: [](const char*, void *&) -> void
template<class F>
inline void rpsEnumerateDLLImports(HMODULE module, const char *dllname, const F &f)
{
    if(module==nullptr) { return; }

    size_t ImageBase = (size_t)module;
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)ImageBase;
    if(pDosHeader->e_magic!=IMAGE_DOS_SIGNATURE) { return; }

    PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)(ImageBase + pDosHeader->e_lfanew);
    DWORD RVAImports = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    if(RVAImports==0) { return; }

    IMAGE_IMPORT_DESCRIPTOR *pImportDesc = (IMAGE_IMPORT_DESCRIPTOR*)(ImageBase + RVAImports);
    while(pImportDesc->Name!=0) {
        const char *pDLLName = (const char*)(ImageBase+pImportDesc->Name);
        if(dllname==nullptr || _stricmp(pDLLName, dllname)==0) {
            IMAGE_THUNK_DATA* pThunkOrig = (IMAGE_THUNK_DATA*)(ImageBase + pImportDesc->OriginalFirstThunk);
            IMAGE_THUNK_DATA* pThunk = (IMAGE_THUNK_DATA*)(ImageBase + pImportDesc->FirstThunk);
            while(pThunkOrig->u1.AddressOfData!=0) {
                if((pThunkOrig->u1.Ordinal & 0x80000000) > 0) {
                    DWORD Ordinal = pThunkOrig->u1.Ordinal & 0xffff;
                    // nameless function
                }
                else {
                    IMAGE_IMPORT_BY_NAME* pIBN = (IMAGE_IMPORT_BY_NAME*)(ImageBase + pThunkOrig->u1.AddressOfData);
                    f((char*)pIBN->Name, *(void**)pThunk);
                }
                ++pThunkOrig;
                ++pThunk;
            }
        }
        ++pImportDesc;
    }
}


inline rpsArchive& operator&(rpsArchive &ar,     char &v) { ar.io(&v, sizeof(v)); return ar; }
inline rpsArchive& operator&(rpsArchive &ar,   int8_t &v) { ar.io(&v, sizeof(v)); return ar; }
inline rpsArchive& operator&(rpsArchive &ar,  uint8_t &v) { ar.io(&v, sizeof(v)); return ar; }
inline rpsArchive& operator&(rpsArchive &ar,  int16_t &v) { ar.io(&v, sizeof(v)); return ar; }
inline rpsArchive& operator&(rpsArchive &ar, uint16_t &v) { ar.io(&v, sizeof(v)); return ar; }
inline rpsArchive& operator&(rpsArchive &ar,  int32_t &v) { ar.io(&v, sizeof(v)); return ar; }
inline rpsArchive& operator&(rpsArchive &ar, uint32_t &v) { ar.io(&v, sizeof(v)); return ar; }
inline rpsArchive& operator&(rpsArchive &ar,  int64_t &v) { ar.io(&v, sizeof(v)); return ar; }
inline rpsArchive& operator&(rpsArchive &ar, uint64_t &v) { ar.io(&v, sizeof(v)); return ar; }
inline rpsArchive& operator&(rpsArchive &ar,    float &v) { ar.io(&v, sizeof(v)); return ar; }
inline rpsArchive& operator&(rpsArchive &ar,   double &v) { ar.io(&v, sizeof(v)); return ar; }
inline rpsArchive& operator&(rpsArchive &ar,          long &v) { ar.io(&v, sizeof(v)); return ar; }
inline rpsArchive& operator&(rpsArchive &ar, unsigned long &v) { ar.io(&v, sizeof(v)); return ar; }
inline rpsArchive& operator&(rpsArchive &ar, HANDLE &v) { return ar & (size_t&)v; }


template<class CharT, template<class> class Allocator >
inline rpsArchive& operator&(rpsArchive &ar, std::basic_string<CharT, std::char_traits<CharT>, Allocator<CharT> > &v)
{
    if(ar.isWriter()) {
        size_t size = v.size();
        ar & size;
        if(size>0) { ar.io(&v[0], size*sizeof(CharT)); }
    }
    else if(ar.isReader()) {
        size_t size;
        ar & size;
        v.resize(size);
        if(size>0) { ar.io(&v[0], size*sizeof(CharT)); }
    }
    return ar;
}

template<class T, template<class> class Allocator >
inline rpsArchive& operator&(rpsArchive &ar, std::vector<T, Allocator<T> > &v)
{
    if(ar.isReader()) {
        size_t size;
        ar & size;
        v.resize(size);
        rpsEach(v, [&](T &e){ ar & e; });
    }
    else if(ar.isWriter()) {
        size_t size = v.size();
        ar & size;
        rpsEach(v, [&](T &e){ ar & e; });
    }
    return ar;
}

template<class K, class V>
inline rpsArchive& operator&(rpsArchive &ar, std::pair<K,V> &v)
{
    typedef std::remove_const<K>::type _K;
    return ar & (_K&)v.first & v.second;
}

template<class K, class V, template<class> class Allocator >
inline rpsArchive& operator&(rpsArchive &ar, std::map<K, V, std::less<K>, Allocator<std::pair<K,V> > > &v)
{
    if(ar.isReader()) {
        v.clear();
        size_t size;
        ar & size;
        for(size_t i=0; i<size; ++i) {
            std::pair<K,V> tmp;
            ar & tmp;
            v.insert(tmp);
        }
    }
    else if(ar.isWriter()) {
        size_t size = v.size();
        ar & size;
        rpsEach(v, [&](std::pair<const K,V> &e){ ar & e; });
    }
    return ar;
}

inline HANDLE rpsToWinHandleC(HANDLE rps_handle, void *p)
{
    return rpsIsInsideRpsModule(p) ? rps_handle : rpsToWinHandle(rps_handle);
}

inline HANDLE rpsToRpsHandleC(HANDLE win_handle, void *p)
{
    return rpsIsInsideRpsModule(p) ? win_handle : rpsToRpsHandle(win_handle);
}


namespace {

template<class RecordT>
struct rpsTHandleRecords
{
public:
    typedef RecordT Record;
    typedef std::map<HANDLE, Record, std::less<HANDLE>, rps_allocator<std::pair<HANDLE, Record> > > Records;
    typedef typename Records::value_type Pair;
    typedef typename Records::iterator iterator;

    iterator begin() { return m_records.begin(); }
    iterator end()   { return m_records.end(); }

    void serialize(rpsArchive &ar); // defined by user

    void addRecord(HANDLE rps_handle, const Record &record)
    {
        if(!rpsIsRpsHandle(rps_handle)) { return; }

        rpsMutex::ScopedLock lock(m_mutex);
        m_records[rps_handle] = record;
    }

    bool eraseRecord(HANDLE rps_handle)
    {
        if(!rpsIsRpsHandle(rps_handle)) { return nullptr; }

        rpsMutex::ScopedLock lock(m_mutex);
        auto it = m_records.find(rps_handle);
        if(it!=m_records.end()) {
            m_records.erase(it);
            return true;
        }
        return false;
    }

    Record* findRecord(HANDLE rps_handle)
    {
        if(!rpsIsRpsHandle(rps_handle)) { return nullptr; }

        rpsMutex::ScopedLock lock(m_mutex);
        auto it = m_records.find(rps_handle);
        if(it!=m_records.end()) {
            return &it->second;
        }
        return nullptr;
    }

private:
    rpsMutex m_mutex;
    Records m_records;
};

template<class T>
inline rpsArchive & operator&(rpsArchive &ar, rpsTHandleRecords<T> &v)
{
    v.serialize(ar);
    return ar;
}
} // namespace

#define rpsDefineHookInfo(Module, FuncName) rpsHookInfo(Module, #FuncName, 0, rps##FuncName, &(void*&)va##FuncName )
#define rpsDefineHookInfo2(Module, FuncName, Flags) rpsHookInfo(Module, #FuncName, 0, rps##FuncName, &(void*&)va##FuncName, Flags )

#endif // rpsInlines_h
