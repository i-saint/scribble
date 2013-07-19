#include <windows.h>
#include <cstdio>

// write protect がかかったメモリ領域を強引に書き換える
template<class T>
inline void ForceWrite(T &dst, const T &src)
{
    DWORD old_flag;
    VirtualProtect(&dst, sizeof(T), PAGE_EXECUTE_READWRITE, &old_flag);
    dst = src;
    VirtualProtect(&dst, sizeof(T), old_flag, &old_flag);
}

void* OverrideDLLImport(HMODULE module, const char *target_module, const char *target_funcname, void *replacement)
{
    if(module==0) { return nullptr; }

    size_t ImageBase = (size_t)module;
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)ImageBase;
    PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)(ImageBase + pDosHeader->e_lfanew);

    size_t RVAImports = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    IMAGE_IMPORT_DESCRIPTOR *pImportDesc = (IMAGE_IMPORT_DESCRIPTOR*)(ImageBase + RVAImports);
    while(pImportDesc->Name != 0) {
        if(stricmp((const char*)(ImageBase+pImportDesc->Name), target_module)==0) {
            const char *dllname = (const char*)(ImageBase+pImportDesc->Name);
            IMAGE_IMPORT_BY_NAME **func_names = (IMAGE_IMPORT_BY_NAME**)(ImageBase+pImportDesc->Characteristics);
            void **import_table = (void**)(ImageBase+pImportDesc->FirstThunk);
            for(size_t i=0; ; ++i) {
                if((size_t)func_names[i] == 0) { break;}
                const char *funcname = (const char*)(ImageBase+(size_t)func_names[i]->Name);
                if(strcmp(funcname, target_funcname)==0) {
                    void *before = import_table[i];
                    ForceWrite<void*>(import_table[i], replacement);
                    return before;
                }
            }
        }
        ++pImportDesc;
    }
    return nullptr;
}


void* fake_malloc(size_t s)
{
    printf("fake_malloc(%d)\n", s);
    return nullptr;
}

int main(int argc, char *argv[])
{
    OverrideDLLImport(::GetModuleHandleA(nullptr), "msvcr100.dll", "malloc", &fake_malloc);
    malloc(0x10);
}

// $ cl OverrideDLLImport.cpp /MD && ./OverrideDLLImport
// fake_malloc(16)
