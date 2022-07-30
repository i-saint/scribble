#pragma comment(lib, "comdlg32.lib")
#include "windows.h"
#include <filesystem>

bool InjectDLL(HANDLE hProcess, const char* dllname)
{
    SIZE_T bytesRet = 0;
    DWORD oldProtect = 0;
    LPVOID remote_addr = nullptr;
    HANDLE hThread = nullptr;
    size_t len = strlen(dllname) + 1;

    remote_addr = ::VirtualAllocEx(hProcess, 0, 1024, MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if(!remote_addr)
        return false;

    ::VirtualProtectEx(hProcess, remote_addr, len, PAGE_EXECUTE_READWRITE, &oldProtect);
    ::WriteProcessMemory(hProcess, remote_addr, dllname, len, &bytesRet);
    ::VirtualProtectEx(hProcess, remote_addr, len, oldProtect, &oldProtect);

    hThread = ::CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)((void*)&LoadLibraryA), remote_addr, 0, NULL);
    ::WaitForSingleObject(hThread, INFINITE); 
    ::VirtualFreeEx(hProcess, remote_addr, 0, MEM_RELEASE);
    return true;
}

int main(int argc, char *argv[])
{
    char exePath[MAX_PATH]{};
    char dllPath[MAX_PATH]{};

    // コマンドライン引数その 1 は実行する exe のパス、その 2 は inject される dll のパスと。
    // コマンドライン引数がない場合はファイルダイアログを出して指定させる。

    if (argc >= 3) {
        strcpy(exePath, argv[1]);
        strcpy(dllPath, argv[2]);
    }
    else {
        {
            OPENFILENAMEA fn{};
            fn.lStructSize = sizeof(fn);
            fn.lpstrTitle = "select .exe to execute";
            fn.lpstrFilter = "Executables\0*.exe\0";
            fn.nFilterIndex = 1;
            fn.lpstrFile = exePath;
            fn.nMaxFile = MAX_PATH;
            fn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
            if (!GetOpenFileNameA(&fn)) {
                return -1;
            }
        }
        {
            OPENFILENAMEA fn{};
            fn.lStructSize = sizeof(fn);
            fn.lpstrTitle = "select .dll to inject";
            fn.lpstrFilter = "DLL\0*.dll\0";
            fn.nFilterIndex = 1;
            fn.lpstrFile = dllPath;
            fn.nMaxFile = MAX_PATH;
            fn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
            if (!GetOpenFileNameA(&fn)) {
                return -1;
            }
        }
    }

    if (!std::filesystem::exists(exePath) || !std::filesystem::exists(dllPath)) {
        printf("usage: %s [exe path to execute] [dll path to be injected]", argv[0]);
        return -1;
    }

    STARTUPINFOA si{};
    PROCESS_INFORMATION pi{};
    si.cb = sizeof(si);
    BOOL ret = ::CreateProcessA(exePath, NULL, NULL, NULL, FALSE,
        NORMAL_PRIORITY_CLASS | CREATE_SUSPENDED, NULL, NULL, &si, &pi);
    if (!ret) {
        return -1;
    }

    InjectDLL(pi.hProcess, dllPath);
    ::ResumeThread(pi.hThread);
    return 0;
}

// cl ExecuteWithDLLInjection.cpp /std:c++17 /EHsc
