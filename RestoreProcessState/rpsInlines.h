#ifndef rpsInlines_h
#define rpsInlines_h

bool rpsIsValidMemory(void *p);
BYTE* rpsAddJumpInstruction(BYTE* from, const BYTE* to);
void* rpsOverrideDLLExport(HMODULE module, const char *funcname, void *hook_, void *trampoline_space);


// F: [](DWORD thread_id) -> void
template<class F>
inline void rpsEnumerateThreads(DWORD pid, const F &proc)
{
    HANDLE ss = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if(ss!=INVALID_HANDLE_VALUE) {
        THREADENTRY32 te;
        te.dwSize = sizeof(te);
        if(::Thread32First(ss, &te)) {
            do {
                if(te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID)+sizeof(te.th32OwnerProcessID) &&
                    te.th32OwnerProcessID==pid)
                {
                    proc(te.th32ThreadID);
                }
                te.dwSize = sizeof(te);
            } while(::Thread32Next(ss, &te));
        }
        ::CloseHandle(ss);
    }
}

// F: []() -> void
template<class F>
void rpsExecExclusive(const F &proc)
{
    std::vector<HANDLE, rps_allocator<HANDLE> > threads;
    rpsEnumerateThreads(::GetCurrentProcessId(), [&](DWORD tid){
        if(tid==::GetCurrentThreadId()) { return; }
        if(HANDLE thread=::OpenThread(THREAD_ALL_ACCESS, FALSE, tid)) {
            ::SuspendThread(thread);
            threads.push_back(thread);
        }
    });
    proc();
    std::for_each(threads.begin(), threads.end(), [](HANDLE thread){
        ::ResumeThread(thread);
        ::CloseHandle(thread);
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

template<class CharT, template<class> class Allocator >
inline rpsArchive& operator&(rpsArchive &ar, std::basic_string<CharT, std::char_traits<CharT>, Allocator<CharT> > &v)
{
    if(ar.isWriter()) {
        size_t size = v.size();
        ar & size;
        if(size>0) { ar.io(&v[0], size); }
    }
    else if(ar.isReader()) {
        size_t size;
        ar & size;
        v.resize(size);
        if(size>0) { ar.io(&v[0], size); }
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

#endif // rpsInlines_h
