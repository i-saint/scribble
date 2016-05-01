// MessageBox("hello!")
extern "C" {
    #pragma section(".code", read,execute)
    __declspec(allocate(".code"))
#ifdef _M_IX86
    char main[] = "\x55\x8B\xEC\x83\xEC\x34\x53\x56\x57\xC7\x45\xD8\x75\x73\x65\x72\xC7\x45\xDC\x33\x32\x2E\x64\xC7\x45\xE0\x6C\x6C\x00\x00\xC7\x45\xCC\x4D\x65\x73\x73\xC7\x45\xD0\x61\x67\x65\x42\xC7\x45\xD4\x6F\x78\x41\x00\xC7\x45\xE4\x68\x65\x6C\x6C\xC7\x45\xE8\x6F\x21\x00\x00\x33\xC9\x64\x8B\x35\x30\x00\x00\x00\x8B\x76\x0C\x8B\x76\x1C\x8B\x46\x08\x8B\x7E\x20\x8B\x36\x38\x4F\x18\x75\xF3\x89\x45\xF8\x8B\x55\xF8\x8B\x42\x3C\x8B\x44\x10\x78\x83\x65\xFC\x00\x03\xC2\x8B\x70\x20\x8B\x78\x24\x8B\x58\x1C\x8B\x40\x14\x03\xF2\x03\xFA\x03\xDA\x89\x45\xF4\x85\xC0\x74\x36\x8B\x45\xFC\x8B\x0C\x86\x0F\xB7\x04\x47\x8B\x04\x83\x03\xC2\x81\x7C\x11\x08\x61\x72\x79\x41\x75\x05\x89\x45\xF0\xEB\x0D\x81\x7C\x11\x04\x72\x6F\x63\x41\x75\x03\x89\x45\xEC\xFF\x45\xFC\x8B\x45\xFC\x3B\x45\xF4\x72\xCA\x8D\x45\xD8\x50\xFF\x55\xF0\x8D\x4D\xCC\x51\x50\xFF\x55\xEC\x6A\x00\x8D\x4D\xE4\x51\x51\x6A\x00\xFF\xD0\x5F\x5E\x5B\xC9\xC3";
#elif _M_AMD64
    char main[] = "\x48\x89\x5C\x24\x10\x48\x89\x74\x24\x18\x48\x89\x7C\x24\x20\x55\x48\x8B\xEC\x48\x83\xEC\x40\xC7\x45\xE0\x75\x73\x65\x72\x45\x33\xD2\xC7\x45\xE4\x33\x32\x2E\x64\xC7\x45\xE8\x6C\x6C\x00\x00\xC7\x45\xF0\x4D\x65\x73\x73\xC7\x45\xF4\x61\x67\x65\x42\xC7\x45\xF8\x6F\x78\x41\x00\xC7\x45\x10\x68\x65\x6C\x6C\xC7\x45\x14\x6F\x21\x00\x00\x65\x48\x8B\x04\x25\x60\x00\x00\x00\x48\x8B\x48\x18\x48\x8B\x51\x20\x48\x8B\x42\x50\x66\x44\x39\x50\x18\x74\x11\x48\x8B\x12\x48\x8B\x12\x48\x8B\x42\x50\x66\x44\x39\x50\x18\x75\xF2\x4C\x8B\x42\x20\x48\x8B\x5D\x10\x49\x63\x40\x3C\x42\x8B\x8C\x00\x88\x00\x00\x00\x49\x03\xC8\x8B\x51\x20\x44\x8B\x49\x24\x49\x03\xD0\x8B\x71\x1C\x4D\x03\xC8\x49\x03\xF0\x44\x39\x51\x14\x4C\x8B\x55\x10\x76\x3B\x8B\x79\x14\x41\x0F\xB7\x01\x44\x8B\x1A\x8B\x0C\x86\x49\x03\xC8\x43\x81\x7C\x03\x08\x61\x72\x79\x41\x75\x05\x4C\x8B\xD1\xEB\x0D\x43\x81\x7C\x03\x04\x72\x6F\x63\x41\x48\x0F\x44\xD9\x48\x83\xC2\x04\x49\x83\xC1\x02\x48\x83\xEF\x01\x75\xC8\x48\x8D\x4D\xE0\x41\xFF\xD2\x48\x8D\x55\xF0\x48\x8B\xC8\xFF\xD3\x45\x33\xC9\x4C\x8D\x45\x10\x48\x8D\x55\x10\x33\xC9\xFF\xD0\x48\x8B\x5C\x24\x58\x48\x8B\x74\x24\x60\x48\x8B\x7C\x24\x68\x48\x83\xC4\x40\x5D\xC3";
#endif
}


#include <windows.h>
#include <winternl.h>
#include <intrin.h>

// 元コード。
// 最終的に shellcode にするため、外部 symbol を一切参照しない書き方にする必要がある。
// 文字列は string literal だと symbol が分離してしまうので、mov address, immediate 形式の命令を用いてスタックに生成する。
// char ではなく int なのは単純にバイナリを縮めるため。
// 
// cl shellcode.cpp /GS- /Ox /Os /c

void shellcode()
{
    int strLoadLibraryA[4]; // "LoadLibraryA"
    strLoadLibraryA[0] = 0x64616F4C;
    strLoadLibraryA[1] = 0x7262694C;
    strLoadLibraryA[2] = 0x41797261;

    int strGetProcAddress[4]; // "GetProcAddress"
    strGetProcAddress[0] = 0x50746547;
    strGetProcAddress[1] = 0x41636F72;
    strGetProcAddress[2] = 0x65726464;
    strGetProcAddress[3] = 0x00007373;

    int strUser32[3]; // "user32.dll"
    strUser32[0] = 0x72657375;
    strUser32[1] = 0x642E3233;
    strUser32[2] = 0x00006C6C;

    int strMessageboxA[3]; // "MessageBoxA"
    strMessageboxA[0] = 0x7373654D;
    strMessageboxA[1] = 0x42656761;
    strMessageboxA[2] = 0x0041786F;

    int strHello[2]; // "hello!"
    strHello[0] = 0x6C6C6568;
    strHello[1] = 0x0000216F;


    HMODULE kernel32;
    {
        // find HMODULE of kernel32.dll
        // thanks: http://www.ic0de.org/archive/index.php/t-10217.html
#ifdef _M_IX86
        PPEB pPeb = (PPEB)__readfsdword(0x30);
#elif _M_AMD64
        PPEB pPeb = (PPEB)__readgsqword(0x60);
#endif
        PLDR_DATA_TABLE_ENTRY pLdrDataTableEntry = (PLDR_DATA_TABLE_ENTRY)pPeb->Ldr->InMemoryOrderModuleList.Flink;
        PLIST_ENTRY pListEntry = pPeb->Ldr->InMemoryOrderModuleList.Flink;
        while (pLdrDataTableEntry->FullDllName.Buffer[12] != 0) {
            pListEntry = pListEntry->Flink;
            pLdrDataTableEntry = (PLDR_DATA_TABLE_ENTRY)(pListEntry->Flink);
        }
        kernel32 = (HMODULE)pLdrDataTableEntry->Reserved2[0];
    }

    typedef HMODULE (__stdcall *LoadLibraryAT)(const char *);
    typedef void* (__stdcall *GetProcAddressT)(HMODULE, const char*);
    LoadLibraryAT pLoadLibraryA;
    GetProcAddressT pGetProcAddress;
    {
        // kernel32.dll の exports を巡回して LoadLibraryA と GetProcAddress を get
        size_t ImageBase = (size_t)kernel32;
        PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)ImageBase;
        PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)(ImageBase + pDosHeader->e_lfanew);
        DWORD RVAExports = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
        IMAGE_EXPORT_DIRECTORY *pExportDirectory = (IMAGE_EXPORT_DIRECTORY *)(ImageBase + RVAExports);

        DWORD *RVANames = (DWORD*)(ImageBase+pExportDirectory->AddressOfNames);
        WORD *RVANameOrdinals = (WORD*)(ImageBase+pExportDirectory->AddressOfNameOrdinals);
        DWORD *RVAFunctions = (DWORD*)(ImageBase+pExportDirectory->AddressOfFunctions);
        for(int i=0; i<pExportDirectory->NumberOfFunctions; ++i) {
            int *name = (int*)(ImageBase+RVANames[i]);
            void *func = (void*)(ImageBase+RVAFunctions[RVANameOrdinals[i]]);
            if     (name[2]==strLoadLibraryA[2])  { (void*&)pLoadLibraryA=func; }   // 
            else if(name[1]==strGetProcAddress[1]){ (void*&)pGetProcAddress=func; } // できるだけ短い比較パターン
        }
    }

    typedef int (__stdcall *MessageBoxAT)(HWND , LPCTSTR , LPCTSTR , UINT);
    HMODULE mod = pLoadLibraryA((char*)strUser32);
    MessageBoxAT pMessageBoxA = (MessageBoxAT) pGetProcAddress(mod, (char*)strMessageboxA);
    pMessageBoxA(NULL, (char*)strHello, (char*)strHello, 0);
}
