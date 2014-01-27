#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <vector>
#include <algorithm>
#include <windows.h>

#if defined(_M_IX86)
#   define rpsCoreDLL "rps32.dll"
#elif defined(_M_X64)
#   define rpsCoreDLL "rps64.dll"
#endif

bool InjectDLL(HANDLE hProcess, const char* dllname)
{
    SIZE_T bytesRet = 0;
    DWORD oldProtect = 0;
    LPVOID remote_addr = NULL;
    HANDLE hThread = NULL;
    size_t len = strlen(dllname) + 1;

    remote_addr = ::VirtualAllocEx(hProcess, 0, 1024, MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if(remote_addr==NULL) { return false; }
    ::VirtualProtectEx(hProcess, remote_addr, len, PAGE_EXECUTE_READWRITE, &oldProtect);
    ::WriteProcessMemory(hProcess, remote_addr, dllname, len, &bytesRet);
    ::VirtualProtectEx(hProcess, remote_addr, len, oldProtect, &oldProtect);

    hThread = ::CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)((void*)&LoadLibraryA), remote_addr, 0, NULL);
    ::WaitForSingleObject(hThread, 5000); 
    ::VirtualFreeEx(hProcess, remote_addr, 0, MEM_RELEASE);
    return true;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prev, LPWSTR cmd, int show)
{
    char exepath[MAX_PATH+1];
    char *exefilename = nullptr;
    if(!exefilename) {
        ::GetModuleFileNameA(nullptr, exepath, sizeof(exepath));
        exefilename = exepath;
        for(int i=0; ; ++i) {
            if(exepath[i]=='\\') { exefilename=exepath+i+1; }
            else if(exepath[i]=='\0') { break; }
        }
    }

    if(__argc<2) {
        printf("usage: %s path-to-exe\n", exefilename);
        return 0;
    }

    {
        DWORD flags = NORMAL_PRIORITY_CLASS | CREATE_SUSPENDED;
        STARTUPINFOW si;
        PROCESS_INFORMATION pi;
        ::ZeroMemory(&si, sizeof(si));
        ::ZeroMemory(&pi, sizeof(pi));
        si.cb = sizeof(si);
        BOOL ret = ::CreateProcessW(nullptr, __wargv[1], nullptr, nullptr, FALSE, flags, nullptr, nullptr, &si, &pi);
        if(ret) {
            std::string dllpath(exepath, (size_t)exefilename-(size_t)exepath);
            dllpath += rpsCoreDLL;

            InjectDLL(pi.hProcess, dllpath.c_str());
            ::ResumeThread(pi.hThread);
            ::CloseHandle(pi.hThread);
            ::CloseHandle(pi.hProcess);
            return pi.dwProcessId;
        }
    }
    return 0;
}
