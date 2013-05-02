#pragma comment(lib, "psapi.lib")
#include <windows.h>
#include <psapi.h>
#include <cstdio>
#include <functional>
#include <regex>

typedef LPVOID (WINAPI *HeapAllocT)( HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes );
typedef BOOL (WINAPI *HeapFreeT)( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem );

// 乗っ取り前の HeapAlloc/Free
HeapAllocT HeapAlloc_Orig = NULL;
HeapFreeT HeapFree_Orig = NULL;
HANDLE g_console_out;

void my_puts(const char *text)
{
    DWORD written;
    ::WriteConsoleA(g_console_out, text, strlen(text), &written, NULL);
}


// 乗っ取り後の HeapAlloc/Free
LPVOID WINAPI HeapAlloc_Hooked( HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes )
{
    my_puts("HeapAlloc_Hooked()\n");
    return HeapAlloc_Orig(hHeap, dwFlags, dwBytes);
}

BOOL WINAPI HeapFree_Hooked( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem )
{
    my_puts("HeapFree_Hooked()\n");
    return HeapFree_Orig(hHeap, dwFlags, lpMem);
}


template<class T> inline void ForceWrite(T &dst, const T &src)
{
    DWORD old_flag;
    ::VirtualProtect(&dst, sizeof(T), PAGE_EXECUTE_READWRITE, &old_flag);
    dst = src;
    ::VirtualProtect(&dst, sizeof(T), old_flag, &old_flag);
}

inline bool IsValidMemory(void *p)
{
    if(p==NULL) { return false; }
    MEMORY_BASIC_INFORMATION meminfo;
    return ::VirtualQuery(p, &meminfo, sizeof(meminfo))!=0 && meminfo.State!=MEM_FREE;
}

inline void EnumerateDLLImports(HMODULE module, const char *dllfilter,
    const std::function<void (const char*, void *&func)> &f1,
    const std::function<void (DWORD ordinal, void *&func)> &f2 )
{
    if(!IsValidMemory(module)) { return; }

    size_t ImageBase = (size_t)module;
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)ImageBase;
    if(pDosHeader->e_magic!=IMAGE_DOS_SIGNATURE) { return; }

    PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)(ImageBase + pDosHeader->e_lfanew);
    DWORD RVAImports = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    if(RVAImports==0) { return; }

    std::regex reg(dllfilter, std::regex::ECMAScript|std::regex::icase);
    IMAGE_IMPORT_DESCRIPTOR *pImportDesc = (IMAGE_IMPORT_DESCRIPTOR*)(ImageBase + RVAImports);
    while(pImportDesc->Name!=0) {
        const char *pDLLName = (const char*)(ImageBase+pImportDesc->Name);
        if(std::regex_match(pDLLName, reg)) {
            IMAGE_THUNK_DATA* pThunkOrig = (IMAGE_THUNK_DATA*)(ImageBase + pImportDesc->OriginalFirstThunk);
            IMAGE_THUNK_DATA* pThunk = (IMAGE_THUNK_DATA*)(ImageBase + pImportDesc->FirstThunk);
            while(pThunkOrig->u1.AddressOfData!=0) {
                if((pThunkOrig->u1.Ordinal & 0x80000000) > 0) {
                    DWORD Ordinal = pThunkOrig->u1.Ordinal & 0xffff;
                    f2(Ordinal, *(void**)pThunk);
                }
                else {
                    IMAGE_IMPORT_BY_NAME* pIBN = (IMAGE_IMPORT_BY_NAME*)(ImageBase + pThunkOrig->u1.AddressOfData);
                    f1((char*)pIBN->Name, *(void**)pThunk);
                }
                ++pThunkOrig;
                ++pThunk;
            }
        }
        ++pImportDesc;
    }
    return;
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

extern "C" void __declspec(dllexport) TestInjection()
{
    g_console_out = ::GetStdHandle(STD_OUTPUT_HANDLE);
    for(size_t i=0; i<_countof(g_crtdllnames); ++i) {
        EnumerateDLLImports(::GetModuleHandleA(g_crtdllnames[i]), "kernel32.dll",
            [](const char *funcname, void *&func){
                if(strcmp(funcname, "HeapAlloc")==0) {
                    (void*&)HeapAlloc_Orig = func;
                    ForceWrite<void*>(func, HeapAlloc_Hooked);
                }
                else if(strcmp(funcname, "HeapFree")==0) {
                    (void*&)HeapFree_Orig = func;
                    ForceWrite<void*>(func, HeapFree_Hooked);
                }
            },
            [](DWORD ordinal, void *&func){}
            );
    }
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    if(fdwReason==DLL_PROCESS_ATTACH) {
        TestInjection();
    }
    return TRUE;
}
