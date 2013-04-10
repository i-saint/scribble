//#include <windows.h>
//int main()
//{
//    typedef int (__stdcall *MessageBoxAT)(HWND , LPCTSTR , LPCTSTR , UINT);
//    typedef HMODULE (__stdcall *LoadLibraryAT)(const char *);
//    typedef void* (__stdcall *GetProcAddressT)(HMODULE, const char*);
//    int user32[3]; // "user32.dll"
//    int messageboxa[3]; // "MessageBoxA"
//    int hello[2]; // "hello!"
//    user32[0] = 0x72657375;
//    user32[1] = 0x642E3233;
//    user32[2] = 0x00006C6C;
//    messageboxa[0] = 0x7373654D;
//    messageboxa[1] = 0x42656761;
//    messageboxa[2] = 0x0041786F;
//    hello[0] = 0x6C6C6568;
//    hello[1] = 0x0000216F;
//    HMODULE mod = ((LoadLibraryAT)0x767249BF)((char*)user32);
//    MessageBoxAT p = (MessageBoxAT) ((GetProcAddressT)0x76721222)(mod, (char*)messageboxa);
//    p(NULL, (char*)hello, (char*)hello, 0);
//}

// g++ shellcode.cpp && ./a
char main[] = "\x83\xEC\x20\x8D\x44\x24\x08\x50\xB9\xBF\x49\x72\x76\xC7\x44\x24\x0C\x75\x73\x65\x72\xC7\x44\x24\x10\x33\x32\x2E\x64\xC7\x44\x24\x14\x6C\x6C\x00\x00\xC7\x44\x24\x18\x4D\x65\x73\x73\xC7\x44\x24\x1C\x61\x67\x65\x42\xC7\x44\x24\x20\x6F\x78\x41\x00\xC7\x44\x24\x04\x68\x65\x6C\x6C\xC7\x44\x24\x08\x6F\x21\x00\x00\xFF\xD1\x8D\x54\x24\x14\x52\x50\xB8\x22\x12\x72\x76\xFF\xD0\x6A\x00\x8D\x4C\x24\x04\x51\x8B\xD1\x52\x6A\x00\xFF\xD0\x33\xC0\x83\xC4\x20\xC3";
