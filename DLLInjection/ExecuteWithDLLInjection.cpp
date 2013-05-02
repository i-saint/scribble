#pragma comment(lib, "comdlg32.lib")
#include "windows.h"

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
    ::WaitForSingleObject(hThread, INFINITE); 
    ::VirtualFreeEx(hProcess, remote_addr, 0, MEM_RELEASE);
    return true;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prev, LPWSTR cmd, int show)
{
    char exePath[MAX_PATH] = {0};
    char dllPath[MAX_PATH] = {0};

    {
        OPENFILENAMEA fn;
        ::ZeroMemory(&fn, sizeof(fn));
        fn.lStructSize = sizeof(fn);
        fn.lpstrTitle = "select .dll to inject";
        fn.lpstrFilter = "DLL\0*.dll\0";
        fn.nFilterIndex = 1;
        fn.lpstrFile = dllPath;
        fn.nMaxFile = MAX_PATH;
        fn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;
        if(!GetOpenFileNameA(&fn)) {
            return 0;
        }
    }
    {
        OPENFILENAMEA fn;
        ::ZeroMemory(&fn, sizeof(fn));
        fn.lStructSize = sizeof(fn);
        fn.lpstrTitle = "select .exe to execute";
        fn.lpstrFilter = "Executables\0*.exe\0";
        fn.nFilterIndex = 1;
        fn.lpstrFile = exePath;
        fn.nMaxFile = MAX_PATH;
        fn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;
        if(!GetOpenFileNameA(&fn)) {
            return 0;
        }
    }

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ::ZeroMemory(&si, sizeof(si));
    ::ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);
    BOOL ret = ::CreateProcessA(exePath, NULL, NULL, NULL, FALSE,
        NORMAL_PRIORITY_CLASS|CREATE_SUSPENDED, NULL, NULL, &si, &pi);
    if(ret) {
        InjectDLL(pi.hProcess, dllPath);
        ::ResumeThread(pi.hThread);
    }

    return 0;
}
