#include <cstdio>
#include <windows.h>
#include "module.h"
#pragma comment(lib, "before.lib")


Hoge::Hoge()
    : m_data(0)
{
}

Hoge::~Hoge()
{
}



// write protect ‚ª‚©‚©‚Á‚½ƒƒ‚ƒŠ—Ìˆæ‚ğ‹­ˆø‚É‘‚«Š·‚¦‚é
template<class T>
inline void ForceWrite(T &dst, const T &src)
{
    DWORD old_flag;
    VirtualProtect(&dst, sizeof(T), PAGE_EXECUTE_READWRITE, &old_flag);
    dst = src;
    VirtualProtect(&dst, sizeof(T), old_flag, &old_flag);
}

// dllname: ‘å•¶š¬•¶š‹æ•Ê‚µ‚Ü‚¹‚ñ
// F: functorBˆø”‚Í (const char *funcname, void *&imp_func)
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
        if(stricmp((const char*)(ImageBase+pImportDesc->Name), dllname)==0) {
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


int main()
{
    Hoge hoge;
    hoge.doSomething();

    {
        const char mangled_func_name[] =
#ifdef _WIN64
        "?doSomething@Hoge@@QEAAXXZ";
#else
        "?doSomething@Hoge@@QAEXXZ";
#endif
        HMODULE module_main = ::GetModuleHandle("main.exe");
        HMODULE module_after = ::LoadLibrary("after.dll");
        void *replacement = ::GetProcAddress(module_after, mangled_func_name);

        EachImportFunction(module_main, "before.dll", [&](const char *funcname, void *&imp_func){
            if(strcmp(funcname, mangled_func_name)==0) {
                ForceWrite<void*>(imp_func, replacement);
            }
        });
    }

    hoge.doSomething();
}
/*
Hoge::doSomething() before: 0
Hoge::doSomething() after: 0
*/
