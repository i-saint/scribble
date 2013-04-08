#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "psapi.lib")
#include <windows.h>
#include <dbghelp.h>
#include <psapi.h>
#include <cstdio>

bool Inject(HANDLE hProcess, const char* dllname, const char* funcname);

bool TryInject()
{
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) ) {
        return 1;
    }

    cProcesses = cbNeeded / sizeof(DWORD);
    for( i = 0; i < cProcesses; i++ ) {
        if( aProcesses[i] == 0 ) { continue; }
        HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, aProcesses[i] );
        if(hProcess==NULL) { continue; }

        TCHAR szProcessName[MAX_PATH] = "\0";
        HMODULE hMod;
        DWORD cbNeeded2;
        if(EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded2)) {
            GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName)/sizeof(TCHAR));
            if(strstr(szProcessName, "TestInjected")!=NULL) {
        		Inject(hProcess, "TestDLL.dll", "TestInjection");
				printf("injection completed\n");
				fflush(stdout);
        		return true;
            }
        }
        CloseHandle(hProcess);
    }

    return false;
}

int main(int argc, char *argv[])
{
	for(;;) {
		if(TryInject()) { break; }
		printf("TestInjected.exe not found. retrying...\n");
		fflush(stdout);
		::Sleep(2000);
	}
}
