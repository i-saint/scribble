#include <cstring>
#include <windows.h>

char shellcode[] =
    // Base16 decoder (thanks to The Shellcoder's Handbook)
    "aaaaaaaaj0X40HPZRXf5A9f5UVfPh0z00X5JEaBPYAAAAAAQhC000X5C7wvH4wPh00a0X527MqPh00CCXf54wfPRXf5zzf5EefPh00M0X508aqH4uPh0G00X50ZgnH4"
    "8PRX5000050M00PYAQX4aHHfPRX4046PRXf50zf50bPYAAAAAAfQRXf50zf50oPYAAAfQRX5555z5ZZZnPAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEB"

    // actual shellcode: MessageBox("hello!")
    "FFILOMIDOMDEFDFGFHMHEFNIHFHDGFHCMHEFNMDDDCCOGEMHEFOAGMGMAAAAMHEFMMENGFHDHDMHEFNAGBGHGFECMHEFNEGPHIEBAAMHEFOEGIGFGMGMMHEFOIGPCBA"
    "AAADDMJGEILDFDAAAAAAAILHGAMILHGBMILEGAIILHOCAILDGDIEPBIHFPDIJEFPIILFFPIILECDMILEEBAHIIDGFPMAAADMCILHACAILHICEILFIBMILEABEADPCAD"
    "PKADNKIJEFPEIFMAHEDGILEFPMILAMIGAPLHAEEHILAEIDADMCIBHMBBAIGBHCHJEBHFAFIJEFPAOLANIBHMBBAEHCGPGDEBHFADIJEFOMPPEFPMILEFPMDLEFPEHCM"
    "KINEFNIFAPPFFPAINENMMFBFAPPFFOMGKAAINENOEFBFBGKAAPPNAFPFOFLMJMDAAQ";

int main()
{
    // VirtualAlloc() to allocate writable executrable memory
    // (#pragma section() does not accept write, execute simultaneously)
    void *buf = VirtualAlloc(nullptr, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    strcpy((char*)buf, shellcode);

    unsigned int x = (unsigned int)buf;
    __asm{
        mov esp, x
        jmp esp
    }
}




#if 0

// source of "actual shellcode" part

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <windows.h>

void FunctionToBeShellcode()
{
    // store strings as integer immediate values

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


    // find kernel32.dll from Process Environment Block
    // thanks to: http://skypher.com/wiki/index.php?title=Hacking/Shellcode/kernel32
    HMODULE kernel32;
    __asm {
        XOR     ECX, ECX; ECX = 0
        MOV     ESI, FS:[0x30]; ESI = &(PEB)([FS:0x30])
        MOV     ESI, [ESI + 0x0C]; ESI = PEB->Ldr
        MOV     ESI, [ESI + 0x1C]; ESI = PEB->Ldr.InInitOrder
    next_module :
        MOV     EAX, [ESI + 0x08]; EAX = InInitOrder[X].base_address
        MOV     EDI, [ESI + 0x20]; EDI = InInitOrder[X].module_name(unicode)
        MOV     ESI, [ESI]; ESI = InInitOrder[X].flink(next module)
        CMP[EDI + 12 * 2], CL; modulename[12] == 0 ?
        JNE     next_module; No: try next module.
        mov     kernel32, EAX
    }

    // find LoadLibraryA and GetProcAddress from Export Address Table in kernel32.dll
    typedef HMODULE(__stdcall *LoadLibraryAT)(const char *);
    typedef void* (__stdcall *GetProcAddressT)(HMODULE, const char*);
    LoadLibraryAT pLoadLibraryA;
    GetProcAddressT pGetProcAddress;
    {
        size_t ImageBase = (size_t)kernel32;
        PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)ImageBase;
        PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)(ImageBase + pDosHeader->e_lfanew);
        DWORD RVAExports = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
        IMAGE_EXPORT_DIRECTORY *pExportDirectory = (IMAGE_EXPORT_DIRECTORY *)(ImageBase + RVAExports);

        DWORD *RVANames = (DWORD*)(ImageBase + pExportDirectory->AddressOfNames);
        WORD *RVANameOrdinals = (WORD*)(ImageBase + pExportDirectory->AddressOfNameOrdinals);
        DWORD *RVAFunctions = (DWORD*)(ImageBase + pExportDirectory->AddressOfFunctions);
        for (int i = 0; i < pExportDirectory->NumberOfFunctions; ++i) {
            int *name = (int*)(ImageBase + RVANames[i]);
            void *func = (void*)(ImageBase + RVAFunctions[RVANameOrdinals[i]]);
            if (name[2] == strLoadLibraryA[2])  { (void*&)pLoadLibraryA = func; }
            else if (name[1] == strGetProcAddress[1]){ (void*&)pGetProcAddress = func; }
        }
    }

    // load user32.dll, get MessageBoxA and call it
    typedef int(__stdcall *MessageBoxAT)(HWND, LPCTSTR, LPCTSTR, UINT);
    HMODULE mod = pLoadLibraryA((char*)strUser32);
    MessageBoxAT pMessageBoxA = (MessageBoxAT)pGetProcAddress(mod, (char*)strMessageboxA);
    pMessageBoxA(NULL, (char*)strHello, (char*)strHello, 0);
}


void Base16Encode()
{
    // FunctionToBeShellcode() in binary form
    char shellcode[] = 
        "\x55\x8B\xEC\x83\xEC\x34\x53\x56\x57\xC7\x45\xD8\x75\x73\x65\x72\xC7\x45\xDC\x33\x32\x2E\x64\xC7\x45\xE0\x6C\x6C\x00\x00\xC7\x45\xCC\x4D\x65\x73\x73\xC7\x45\xD0\x61\x67\x65\x42\xC7\x45\xD4\x6F\x78\x41\x00\xC7\x45\xE4\x68\x65\x6C\x6C\xC7\x45\xE8\x6F\x21\x00\x00\x33\xC9\x64\x8B\x35\x30\x00\x00\x00\x8B\x76\x0C\x8B\x76\x1C\x8B\x46\x08\x8B\x7E\x20\x8B\x36\x38\x4F\x18\x75\xF3\x89\x45\xF8\x8B\x55\xF8\x8B\x42\x3C\x8B\x44\x10\x78\x83\x65\xFC\x00\x03\xC2\x8B\x70\x20\x8B\x78\x24\x8B\x58\x1C\x8B\x40\x14\x03\xF2\x03\xFA\x03\xDA\x89\x45\xF4\x85\xC0\x74\x36\x8B\x45\xFC\x8B\x0C\x86\x0F\xB7\x04\x47\x8B\x04\x83\x03\xC2\x81\x7C\x11\x08\x61\x72\x79\x41\x75\x05\x89\x45\xF0\xEB\x0D\x81\x7C\x11\x04\x72\x6F\x63\x41\x75\x03\x89\x45\xEC\xFF\x45\xFC\x8B\x45\xFC\x3B\x45\xF4\x72\xCA\x8D\x45\xD8\x50\xFF\x55\xF0\x8D\x4D\xCC\x51\x50\xFF\x55\xEC\x6A\x00\x8D\x4D\xE4\x51\x51\x6A\x00\xFF\xD0\x5F\x5E\x5B\xC9\xC3";

    size_t blen = sizeof(shellcode);
    size_t alen = blen * 2 + 1;
    char *buf = (char*)malloc(alen);
    for (size_t i = 0; i < blen; ++i) {
        // split 8-bit into two 4-bit and store them in two alphabets
        buf[i * 2 + 0] = 'A' + ((shellcode[i] & 0xf0) >> 4);
        buf[i * 2 + 1] = 'A' +  (shellcode[i] & 0x0f);
    }
    buf[alen - 1] = 0;
    puts(buf);
    free(buf);
}
#endif
