#pragma comment(lib, "psapi.lib")
#include <windows.h>
#include <psapi.h>
#include <cstdio>


typedef LPVOID (WINAPI *HeapAllocT)( HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes );
typedef BOOL (WINAPI *HeapFreeT)( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem );

// 乗っ取り前の HeapAlloc/Free
HeapAllocT HeapAlloc_Orig = NULL;
HeapFreeT HeapFree_Orig = NULL;

// 乗っ取り後の HeapAlloc/Free
LPVOID WINAPI HeapAlloc_Hooked( HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes );
BOOL WINAPI HeapFree_Hooked( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem );

LPVOID WINAPI HeapAlloc_Hooked( HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes )
{
	printf("HeapAlloc_Hooked()\n");
    return HeapAlloc_Orig(hHeap, dwFlags, dwBytes);
}

BOOL WINAPI HeapFree_Hooked( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem )
{
	printf("HeapFree_Hooked()\n");
    return HeapFree_Orig(hHeap, dwFlags, lpMem);
}

template<class T> inline void ForceWrite(T &dst, const T &src)
{
    DWORD old_flag;
    VirtualProtect(&dst, sizeof(T), PAGE_EXECUTE_READWRITE, &old_flag);
    dst = src;
    VirtualProtect(&dst, sizeof(T), old_flag, &old_flag);
}

const char *g_crtdllnames[] = {
    "msvcr110.dll",
    "msvcr110d.dll",
    "msvcr100.dll",
    "msvcr100d.dll",
    "msvcr90.dll",
    "msvcr90d.dll",
    "msvcr80.dll",
    "msvcr80d.dll",
    "msvcrt.dll",
};

// dllname: 大文字小文字区別しません
// F: functor。引数は (const char *funcname, void *&imp_func)
template<class F>
bool EachImportFunction(HMODULE module, const char *dllname, const F &f)
{
    if(module==0) { return false; }

    size_t ImageBase = (size_t)module;
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)ImageBase;
    if(pDosHeader->e_magic!=IMAGE_DOS_SIGNATURE) { return false; }
    PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)(ImageBase + pDosHeader->e_lfanew);

    size_t RVAImports = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    if(RVAImports==0) { return false; }

    IMAGE_IMPORT_DESCRIPTOR *pImportDesc = (IMAGE_IMPORT_DESCRIPTOR*)(ImageBase + RVAImports);
    while(pImportDesc->Name != 0) {
        const char *module_name = (const char*)(ImageBase+pImportDesc->Name);
        if(stricmp(module_name, dllname)==0) {
            IMAGE_IMPORT_BY_NAME **func_names = (IMAGE_IMPORT_BY_NAME**)(ImageBase+pImportDesc->Characteristics);
            void **import_table = (void**)(ImageBase+pImportDesc->FirstThunk);
            for(size_t i=0; ; ++i) {
                if((size_t)func_names[i] == 0) { break;}
                const char *funcname = (const char*)(ImageBase+(size_t)func_names[i]->Name);
                f(funcname, import_table[i]);
            }
        }
        ++pImportDesc;
    }
    return true;
}


extern "C" void __declspec(dllexport) TestInjection()
{
    printf("TestInjection()\n");
    fflush(stdout);

    for(size_t i=0; i<_countof(g_crtdllnames); ++i) {
        EachImportFunction(::GetModuleHandleA(g_crtdllnames[i]), "kernel32.dll", [](const char *funcname, void *&imp_func){
            if(strcmp(funcname, "HeapAlloc")==0) {
                (void*&)HeapAlloc_Orig = imp_func;
                ForceWrite<void*>(imp_func, HeapAlloc_Hooked);
            }
            else if(strcmp(funcname, "HeapFree")==0) {
                (void*&)HeapFree_Orig = imp_func;
                ForceWrite<void*>(imp_func, HeapFree_Hooked);
            }
        });
    }
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    if(fdwReason==DLL_PROCESS_ATTACH) {
        TestInjection();
        ExitThread(0);
    }
}
