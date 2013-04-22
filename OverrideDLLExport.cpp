#include <windows.h>
#include <cstdio>


// write protect がかかっているメモリ領域を強引に書き換える
template<class T>
inline void ForceWrite(T &dst, const T &src)
{
    DWORD old_flag;
    ::VirtualProtect(&dst, sizeof(T), PAGE_EXECUTE_READWRITE, &old_flag);
    dst = src;
    ::VirtualProtect(&dst, sizeof(T), old_flag, &old_flag);
}

// dll が export している関数 (への RVA) を書き換える
// これにより、GetProcAddress() が返す関数をすり替える
// !! x64 では相対アドレスが DWORD に収まらないケースがあり、その場合クラッシュを招きます。GetProcAddress() そのものを乗っ取る方がたぶん安全です !!
inline void OverrideDLLExportByName(HMODULE module, const char *funcname, void *replacement)
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
        if(strcmp(pName, funcname)==0) {
            ForceWrite<DWORD>(RVAFunctions[RVANameOrdinals[i]], (DWORD)replacement - ImageBase);
            return;
        }
    }
}

// ordinal 指定版
inline void OverrideDLLExportByOrdinal(HMODULE module, DWORD func_ordinal, void *replacement)
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
        if(RVANameOrdinals[i]==func_ordinal) {
            ForceWrite<DWORD>(RVAFunctions[RVANameOrdinals[i]], (DWORD)replacement - ImageBase);
        }
    }
}



typedef LPVOID (WINAPI *HeapAllocT)(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes);

LPVOID WINAPI fake_HeapAlloc(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes)
{
    printf("fake_HeapAlloc\n");
    return NULL;
}

int main(int argc, char *argv[])
{
    HMODULE kernel32 = ::GetModuleHandleA("kernel32.dll");
    HeapAllocT pHeapAlloc;

    pHeapAlloc = (HeapAllocT)::GetProcAddress(kernel32, "HeapAlloc");
    printf("HeapAlloc: 0x%p, fake_HeapAlloc: 0x%p, pHeapAlloc: 0x%p\n", &HeapAlloc, &fake_HeapAlloc, pHeapAlloc);

    OverrideDLLExportByName(kernel32, "HeapAlloc", fake_HeapAlloc);

    pHeapAlloc = (HeapAllocT)::GetProcAddress(kernel32, "HeapAlloc");
    printf("HeapAlloc: 0x%p, fake_HeapAlloc: 0x%p, pHeapAlloc: 0x%p\n", &HeapAlloc, &fake_HeapAlloc, pHeapAlloc);

    pHeapAlloc(::GetProcessHeap(), 0, 10);
}

// $ cl OverrideDLLExport.cpp && ./OverrideDLLExport
// HeapAlloc: 0x7779E046, fake_HeapAlloc: 0x01101000, pHeapAlloc: 0x7779E046
// HeapAlloc: 0x7779E046, fake_HeapAlloc: 0x01101000, pHeapAlloc: 0x01101000
// fake_HeapAlloc
