#include <windows.h>
#include <cstdio>

#pragma comment(lib,"TestDLL32.lib")
__declspec(dllimport) void DLLFunction();



int main(int argc, char *argv[])
{
    for(;;) {
		DLLFunction();
        ::Sleep(1000);
    }
}
