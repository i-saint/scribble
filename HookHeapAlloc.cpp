#include <windows.h>

// write protect がかかったメモリ領域を強引に書き換える
template<class T>
inline void ForceWrite(T &dst, const T &src)
{
    DWORD old_flag;
    VirtualProtect(&dst, sizeof(T), PAGE_EXECUTE_READWRITE, &old_flag);
    dst = src;
    VirtualProtect(&dst, sizeof(T), old_flag, &old_flag);
}

void HookHeapAlloc(HMODULE module)
{
    if(module==0) { return; }

    size_t ImageBase = (size_t)module;
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)ImageBase;
    PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)(ImageBase + pDosHeader->e_lfanew);

    size_t RVAImports = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    IMAGE_IMPORT_DESCRIPTOR *pImportDesc = (IMAGE_IMPORT_DESCRIPTOR*)(ImageBase + RVAImports);
    while(pImportDesc->Name != 0) {
        if(stricmp((const char*)(ImageBase+pImportDesc->Name), "kernel32.dll")==0) {
            const char *dllname = (const char*)(ImageBase+pImportDesc->Name);
            IMAGE_IMPORT_BY_NAME **func_names = (IMAGE_IMPORT_BY_NAME**)(ImageBase+pImportDesc->Characteristics);
            void **import_table = (void**)(ImageBase+pImportDesc->FirstThunk);
            for(size_t i=0; ; ++i) {
                if((size_t)func_names[i] == 0) { break;}
                const char *funcname = (const char*)(ImageBase+(size_t)func_names[i]->Name);
                if(strcmp(funcname, "HeapAlloc")==0) {
                    //ForceWrite<void*>(import_table[i], HeapAlloc_Hooked);
                }
                else if(strcmp(funcname, "HeapFree")==0) {
                    //ForceWrite<void*>(import_table[i], HeapFree_Hooked);
                }
            }
        }
        ++pImportDesc;
    }
}
