#include <windows.h>
#include <cstdio>

// F: functor(const char *funcname, void *funcptr)
template<class F>
void EnumerateDLLExports(HMODULE module, const F &f)
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
    for(int i=0; i<pExportDirectory->NumberOfFunctions; ++i) {
        char *pName = (char*)(ImageBase+RVANames[i]);
        void *pFunc = (void*)(ImageBase+RVAFunctions[RVANameOrdinals[i]]);
        f(pName, pFunc);
    }
}

// dllname: 特定の dll からの import のみを巡回したい場合指定。大文字小文字区別しません。 NULL の場合全ての dll からの import を巡回。
// F: functor (const char *funcname, void *&funcptr)
template<class F>
void EnumerateDLLImports(HMODULE module, const char *dllname, const F &f)
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
        if(dllname==NULL || stricmp(pDLLName, dllname)==0) {
            IMAGE_IMPORT_BY_NAME **ppSymbolNames = (IMAGE_IMPORT_BY_NAME**)(ImageBase+pImportDesc->Characteristics);
            void **ppFuncs = (void**)(ImageBase+pImportDesc->FirstThunk);
            for(int i=0; ; ++i) {
                if(ppSymbolNames[i]==NULL) { break; }
                char *pName = (char*)(ImageBase+(size_t)ppSymbolNames[i]->Name);
                f(pName, ppFuncs[i]);
            }
        }
        ++pImportDesc;
    }
    return;
}


int main(int argc, char *argv[])
{
    HMODULE kernel32 = ::GetModuleHandleA("kernel32.dll");

    printf("exports:\n");
    EnumerateDLLExports(kernel32, [](const char *funcname, void *funcptr){
        printf("%s: 0x%p\n", funcname, funcptr);
    });

    printf("\n");

    printf("imports:\n");
    EnumerateDLLImports(kernel32, NULL, [](const char *funcname, void *funcptr){
        printf("%s: 0x%p\n", funcname, funcptr);
    });
}

// $ cl EnumerateDLLExports.cpp && ./EnumerateDLLExports
// AcquireSRWLockExclusive: 0x771E98F5
// AcquireSRWLockShared: 0x771E9916
// ...
