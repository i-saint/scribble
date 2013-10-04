#include <cstdlib>
#include <windows.h>

#pragma section("code", execute)
__declspec(allocate("code")) char sSuspendSelf[] = "\x55\x8B\xEC\x83\xEC\x28\x53\x56\x57\x33\xC9\x64\x8B\x35\x30\x00\x00\x00\x8B\x76\x0C\x8B\x76\x1C\x8B\x46\x08\x8B\x7E\x20\x8B\x36\x38\x4F\x18\x75\xF3\x89\x45\xFC\x8B\x55\xFC\xC7\x45\xE8\x4F\x70\x65\x6E\xC7\x45\xEC\x54\x68\x72\x65\xC7\x45\xF0\x61\x64\x00\x00\xC7\x45\xD8\x53\x75\x73\x70\xC7\x45\xDC\x65\x6E\x64\x54\xC7\x45\xE0\x68\x72\x65\x61\xC7\x45\xE4\x64\x00\x00\x00\x8B\x42\x3C\x8B\x44\x10\x78\x03\xC2\x8B\x48\x1C\x8B\x78\x20\x8B\x58\x24\x8B\x40\x14\x03\xCA\x89\x4D\xF8\x03\xFA\x03\xDA\x33\xC9\x89\x45\xF4\x8B\x75\xF4\x85\xC0\x74\x1F\x8B\x04\x8F\x81\x7C\x10\x04\x72\x6F\x63\x41\x75\x0C\x0F\xB7\x04\x4B\x8B\x75\xF8\x8B\x34\x86\x03\xF2\x41\x3B\x4D\xF4\x72\xE1\x8D\x45\xE8\x50\x52\xFF\xD6\x8B\xF8\x8D\x45\xD8\x50\xFF\x75\xFC\xFF\xD6\x64\x8B\x0D\x24\x00\x00\x00\x51\x6A\x00\x68\xFF\xFF\x1F\x00\x8B\xF0\xFF\xD7\x50\xFF\xD6\x5F\x5E\x5B\xC9";


// ↑の元コード
__declspec(noinline) void SuspendSelf()
{
    // thanks to http://skypher.com/wiki/index.php/Hacking/Shellcode/kernel32
    HMODULE kernel32;
    __asm {
            XOR     ECX, ECX            ; ECX = 0
            MOV     ESI, FS:[0x30]      ; ESI = &(PEB) ([FS:0x30])
            MOV     ESI, [ESI + 0x0C]   ; ESI = PEB->Ldr
            MOV     ESI, [ESI + 0x1C]   ; ESI = PEB->Ldr.InInitOrder
next_module:
        MOV     EAX, [ESI + 0x08]   ; EAX = InInitOrder[X].base_address
            MOV     EDI, [ESI + 0x20]   ; EDI = InInitOrder[X].module_name (unicode)
            MOV     ESI, [ESI]          ; ESI = InInitOrder[X].flink (next module)
            CMP     [EDI + 12*2], CL    ; modulename[12] == 0 ?
            JNE     next_module         ; No: try next module.
            mov     kernel32, EAX
    }

    // GetProcAddress() を最初に探して残りは GetProcAddress() で取得。
    // OpenThread() と SuspendThread() だけ直接取ってもいいがそっちの方がバイナリ長くなった。

    typedef void* (__stdcall *tGetProcAddress)(HMODULE, const char*);
    typedef HMODULE (__stdcall *tOpenThread)(DWORD, BOOL, DWORD);
    typedef DWORD (__stdcall *tSuspendThread)(HANDLE);
    DWORD sGetProcAddress[] ={0x50746547, 0x41636F72, 0x65726464, 0x00007373};
    DWORD sOpenThread[] = {0x6E65704F, 0x65726854, 0x00006461};
    DWORD sSuspendThread[] = {0x70737553, 0x54646E65, 0x61657268, 0x00000064};
    tGetProcAddress pGetProcAddress;
    {
        size_t ImageBase = (size_t)kernel32;
        PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)ImageBase;
        PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)(ImageBase + pDosHeader->e_lfanew);
        DWORD RVAExports = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
        IMAGE_EXPORT_DIRECTORY *pExportDirectory = (IMAGE_EXPORT_DIRECTORY *)(ImageBase + RVAExports);

        DWORD *RVANames = (DWORD*)(ImageBase+pExportDirectory->AddressOfNames);
        WORD *RVANameOrdinals = (WORD*)(ImageBase+pExportDirectory->AddressOfNameOrdinals);
        DWORD *RVAFunctions = (DWORD*)(ImageBase+pExportDirectory->AddressOfFunctions);
        for(DWORD i=0; i<pExportDirectory->NumberOfFunctions; ++i) {
            int *name = (int*)(ImageBase+RVANames[i]);
            void *func = (void*)(ImageBase+RVAFunctions[RVANameOrdinals[i]]);
            if(name[1]==sGetProcAddress[1]){ (void*&)pGetProcAddress=func; }
        }
    }
    tOpenThread pOpenThead = (tOpenThread)pGetProcAddress(kernel32, (char*)sOpenThread);
    tSuspendThread pSuspendThread = (tSuspendThread)pGetProcAddress(kernel32, (char*)sSuspendThread);

    DWORD tid = __readfsdword(0x24);
    HANDLE tread = pOpenThead(THREAD_ALL_ACCESS, FALSE, tid);
    pSuspendThread(tread);
}

int main(int argc, char* argv[])
{
    ((void (*)())&sSuspendSelf)();
    //SuspendSelf();
}
