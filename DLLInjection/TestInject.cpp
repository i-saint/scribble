#pragma comment(lib, "psapi.lib")
#include <windows.h>
#include <psapi.h>
#include <cstdio>

// VirtualAllocEx で dll の path を対象プロセスに書き込み、
// CreateRemoteThread で対象プロセスにスレッドを作って、↑で書き込んだ dll path をパラメータにして LoadLibraryA を呼ぶ。
// 結果、対象プロセス内で任意の dll を実行させる。 
bool InjectDLL(HANDLE hProcess, const char* dllname)
{
    DWORD bytesRet = 0;
    DWORD oldProtect = 0;
    LPVOID remote_addr = NULL;
    HANDLE hThread = NULL;
    size_t len = strlen(dllname) + 1;

    remote_addr = VirtualAllocEx(hProcess, 0, 1024, MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if(remote_addr==NULL) { return false; }
    VirtualProtectEx(hProcess, remote_addr, len, PAGE_EXECUTE_READWRITE, &oldProtect);
    WriteProcessMemory(hProcess, remote_addr, dllname, len, &bytesRet);
    VirtualProtectEx(hProcess, remote_addr, len, oldProtect, &oldProtect);
    FlushInstructionCache(hProcess, remote_addr, len);

    hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)((void*)&LoadLibraryA), remote_addr, 0, NULL);
    WaitForSingleObject(hThread, INFINITE); 
    VirtualFreeEx(hProcess, remote_addr, 0, MEM_RELEASE);
    return true;
}

bool TryInject()
{
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    if(!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
        return 1;
    }

    cProcesses = cbNeeded / sizeof(DWORD);
    for( i = 0; i < cProcesses; i++ ) {
        if( aProcesses[i] == 0 ) { continue; }
        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, aProcesses[i]);
        if(hProcess==NULL) { continue; }

        TCHAR szProcessName[MAX_PATH] = "\0";
        HMODULE hMod;
        DWORD cbNeeded2;
        if(EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded2)) {
            GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName)/sizeof(TCHAR));
            if(strstr(szProcessName, "TestInjected")!=NULL && InjectDLL(hProcess, "TestDLL.dll")) {
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
