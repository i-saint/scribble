#include <windows.h>
#include <cstdio>

#if defined(_M_IX86)
    #pragma comment(lib,"TestDLL32.lib")
#elif defined(_M_X64)
    #pragma comment(lib,"TestDLL64.lib")
#endif

__declspec(dllimport) void DLLFunction();



int main(int argc, char *argv[])
{
    for(;;) {
        DLLFunction();
        ::Sleep(1000);
    }
}
