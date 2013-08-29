#include <windows.h>
#include <cstdio>

struct CV_INFO_PDB70
{
    DWORD  CvSignature;
    GUID Signature;
    DWORD Age;
    BYTE PdbFileName[1];
};

// fill_gap: .dll ファイルをそのままメモリに移した場合はこれを true にする必要があります。
// LoadLibrary() で正しくロードしたものは section の再配置が行われ、元ファイルとはデータの配置にズレが生じます。
// fill_gap==true の場合このズレを補正します。
CV_INFO_PDB70* GetPDBInfoFromModule(void *pModule, bool fill_gap=false)
{
    if(!pModule) { return nullptr; }

    PBYTE pData = (PUCHAR)pModule;
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pData;
    PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)(pData + pDosHeader->e_lfanew);
    if(pDosHeader->e_magic==IMAGE_DOS_SIGNATURE && pNtHeaders->Signature==IMAGE_NT_SIGNATURE) {
        ULONG DebugRVA = pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress;
        if(DebugRVA==0) { return nullptr; }

        PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION(pNtHeaders);
        for(size_t i=0; i<pNtHeaders->FileHeader.NumberOfSections; ++i) {
            PIMAGE_SECTION_HEADER s = pSectionHeader+i;
            if(DebugRVA >= s->VirtualAddress && DebugRVA < s->VirtualAddress+s->SizeOfRawData) {
                pSectionHeader = s;
                break;
            }
        }
        if(fill_gap) {
            DWORD gap = pSectionHeader->VirtualAddress - pSectionHeader->PointerToRawData;
            pData -= gap;
        }

        PIMAGE_DEBUG_DIRECTORY pDebug;
        pDebug = (PIMAGE_DEBUG_DIRECTORY)(pData + DebugRVA);
        if(DebugRVA!=0 && DebugRVA < pNtHeaders->OptionalHeader.SizeOfImage && pDebug->Type==IMAGE_DEBUG_TYPE_CODEVIEW) {
            CV_INFO_PDB70 *pCVI = (CV_INFO_PDB70*)(pData + pDebug->AddressOfRawData);
            if(pCVI->CvSignature=='SDSR') {
                return pCVI;
            }
        }
    }
    return nullptr;
}

// F: [](size_t size) -> void* : malloc() etc.
template<class F>
inline bool MapFile(const char *path, void *&o_data, size_t &o_size, const F &alloc)
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

int main(int argc, char *argv[])
{
    // dll ファイルをそのままメモリにマップして pdb パス抽出
    {
        CV_INFO_PDB70 *pdb = nullptr;
        void *module = nullptr;
        size_t module_size = 0;
        if(MapFile("GetPDBInfoFromDLL.exe", module, module_size, malloc)) {
            pdb = GetPDBInfoFromModule(module, true);
        }
        printf("%s\n", pdb->PdbFileName);
        free(module);
    }

    // LoadLibrary() でちゃんとロードしたモジュールから pdb パス抽出
    {
        HMODULE module = ::LoadLibraryA("GetPDBInfoFromDLL.exe");
        CV_INFO_PDB70 *pdb = GetPDBInfoFromModule(module);
        printf("%s\n", pdb->PdbFileName);
        ::FreeLibrary(module);
    }
}

/*
$ cl /Zi GetPDBInfoFromDLL.cpp && ./GetPDBInfoFromDLL
D:\src\scribble\GetPDBInfoFromDLL.pdb
D:\src\scribble\GetPDBInfoFromDLL.pdb
*/
