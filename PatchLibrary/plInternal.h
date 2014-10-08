#ifndef plInternal_h
#define plInternal_h

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <ws2tcpip.h>
#include <winsock2.h>

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <ctime>
#include <cassert>
#include <cstdint>

#define rpsPrintError(...) printf(__VA_ARGS__)
#define plDefaultPort 21357

typedef std::string plString;

class plTrampolineAllocator
{
public:
    static const size_t page_size = 1024*64;
    static const size_t block_size = 32;

    plTrampolineAllocator();
    ~plTrampolineAllocator();
    void* allocate(void *location);
    bool deallocate(void *v);

private:
    class Page;
    typedef std::vector<Page*> page_cont;
    page_cont m_pages;

    Page* createPage(void *location);
    Page* findOwnerPage(void *location);
    Page* findCandidatePage(void *location);
};

template<class Container, class F> inline void plEach(Container &cont, const F &f);
template<class Container, class F> inline auto plFind(Container &cont, const F &f) -> decltype(cont.begin());
template<class F> inline void plEnumerateDLLExports(HMODULE module, const F &f);
template<class F> inline void plEnumerateDLLImports(HMODULE module, const char *dllname, const F &f);
template<class F> inline void plEnumerateDLLImportsInEveryModule(const char *dllname, const F &f);
template<class F> inline bool plMapFile(const char *path, void *&o_data, size_t &o_size, const F &alloc);

void*  plAllocateForward(size_t size, void *location);
void*  plAllocateBackward(size_t size, void *location);
void   plDeallocate(void *location, size_t size);
bool   plCopyFile(const char *srcpath, const char *dstpath);
bool   plWriteFile(const char *path, const void *data, size_t size);
bool   plDeleteFile(const char *path);
bool   plFileExists( const char *path );
size_t plSeparateDirFile(const char *path, std::string *dir, std::string *file);
size_t plSeparateFileExt(const char *filename, std::string *file, std::string *ext);
BYTE*  plAddJumpInstruction(BYTE* from, BYTE* to);


struct CV_INFO_PDB70
{
    DWORD  CvSignature;
    GUID Signature;
    DWORD Age;
    BYTE PdbFileName[1];
};
struct PDBStream70
{
    DWORD impv;
    DWORD sig;
    DWORD age;
    GUID sig70;
};
CV_INFO_PDB70* plGetPDBInfoFromModule(void *pModule, bool fill_gap);
PDBStream70* plGetPDBSignature(void *mapped_pdb_file);


template<class Container, class F>
inline void plEach(Container &cont, const F &f)
{
    std::for_each(cont.begin(), cont.end(), f);
}

template<class Container, class F>
inline auto plFind(Container &cont, const F &f) -> decltype(cont.begin())
{
    return std::find_if(cont.begin(), cont.end(), f);
}

// F: functor(const char *name, void *sym)
template<class F>
inline void plEnumerateDLLExports(HMODULE module, const F &f)
{
    if(module==NULL) { return; }

    size_t ImageBase = (size_t)module;
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)ImageBase;
    if(pDosHeader->e_magic!=IMAGE_DOS_SIGNATURE) { return; }

    PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)(ImageBase + pDosHeader->e_lfanew);
    DWORD RVAExports = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    if(RVAExports==0) { return; }

    IMAGE_EXPORT_DIRECTORY *pExportDirectory = (IMAGE_EXPORT_DIRECTORY *)(ImageBase + RVAExports);
    DWORD *RVANames = (DWORD*)(ImageBase+pExportDirectory->AddressOfNames);
    WORD *RVANameOrdinals = (WORD*)(ImageBase+pExportDirectory->AddressOfNameOrdinals);
    DWORD *RVAFunctions = (DWORD*)(ImageBase+pExportDirectory->AddressOfFunctions);
    for(DWORD i=0; i<pExportDirectory->NumberOfFunctions; ++i) {
        char *pName = (char*)(ImageBase+RVANames[i]);
        void *pFunc = (void*)(ImageBase+RVAFunctions[RVANameOrdinals[i]]);
        f(pName, pFunc);
    }
}

// dllname: 特定の dll からの import のみを巡回したい場合指定。大文字小文字区別しません。 NULL の場合全ての dll からの import を巡回。
// F: functor (const char *funcname, void *&funcptr)
template<class F>
inline void plEnumerateDLLImports(HMODULE module, const char *dllname, const F &f)
{
    if(module==NULL) { return; }

    size_t ImageBase = (size_t)module;
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)ImageBase;
    if(pDosHeader->e_magic!=IMAGE_DOS_SIGNATURE) { return; }

    PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)(ImageBase + pDosHeader->e_lfanew);
    DWORD RVAImports = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    if(RVAImports==0) { return; }

    IMAGE_IMPORT_DESCRIPTOR *pImportDesc = (IMAGE_IMPORT_DESCRIPTOR*)(ImageBase + RVAImports);
    while(pImportDesc->Name!=0) {
        const char *pDLLName = (const char*)(ImageBase+pImportDesc->Name);
        if(dllname==NULL || _stricmp(pDLLName, dllname)==0) {
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
    return;
}

template<class F>
inline void plEnumerateDLLImportsInEveryModule(const char *dllname, const F &f)
{
    std::vector<HMODULE> modules;
    DWORD num_modules;
    ::EnumProcessModules(::GetCurrentProcess(), NULL, 0, &num_modules);
    modules.resize(num_modules/sizeof(HMODULE));
    ::EnumProcessModules(::GetCurrentProcess(), &modules[0], num_modules, &num_modules);
    for(size_t i=0; i<modules.size(); ++i) {
        plEnumerateDLLImports<F>(modules[i], dllname, f);
    }
}

// F: [](size_t size) -> void* : alloc func
template<class F>
inline bool plMapFile(const char *path, void *&o_data, size_t &o_size, const F &alloc)
{
    o_data = NULL;
    o_size = 0;
    if(FILE *f=fopen(path, "rb")) {
        fseek(f, 0, SEEK_END);
        o_size = ftell(f);
        if(o_size > 0) {
            o_data = alloc(o_size);
            fseek(f, 0, SEEK_SET);
            fread(o_data, 1, o_size, f);
        }
        fclose(f);
        return true;
    }
    return false;
}

#endif // plInternal_h
