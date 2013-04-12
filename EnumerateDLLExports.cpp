#include <windows.h>
#include <cstdio>

// F: functor(const char *funcname, void *funcptr)
template<class F>
void EnumerateDLLExports(HMODULE module, const F &f)
{
    if(module==NULL) { return; }

    size_t ImageBase = (size_t)module;
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)ImageBase;
    PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)(ImageBase + pDosHeader->e_lfanew);
    IMAGE_SECTION_HEADER *pFirstSectionHeader = (IMAGE_SECTION_HEADER*)((PBYTE)&pNTHeader->OptionalHeader + pNTHeader->FileHeader.SizeOfOptionalHeader);
    DWORD RVAExports = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    IMAGE_EXPORT_DIRECTORY *pExportDirectory = (IMAGE_EXPORT_DIRECTORY *)(ImageBase + RVAExports);

    DWORD *RVANames = (DWORD*)(ImageBase+pExportDirectory->AddressOfNames);
    WORD *RVANameOrdinals = (WORD*)(ImageBase+pExportDirectory->AddressOfNameOrdinals);
    DWORD *RVAFunctions = (DWORD*)(ImageBase+pExportDirectory->AddressOfFunctions);
    for(int i=0; i<pExportDirectory->NumberOfFunctions; ++i) {
        char *name = (char*)(ImageBase+RVANames[i]);
        void *func = (void*)(ImageBase+RVAFunctions[RVANameOrdinals[i]]);
        f(name, func);
    }
}

int main(int argc, char *argv[])
{
    HMODULE kernel32 = ::GetModuleHandleA("kernel32.dll");
    EnumerateDLLExports(kernel32, [](const char *funcname, void *funcptr){
        printf("%s: 0x%p\n", funcname, funcptr);
    });
}

// $ cl EnumerateDLLExports.cpp && ./EnumerateDLLExports
// AcquireSRWLockExclusive: 0x771E98F5
// AcquireSRWLockShared: 0x771E9916
// ...
