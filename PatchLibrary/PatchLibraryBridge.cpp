#include "plInternal.h"
#include "plNetwork.h"
#include <tlhelp32.h>

#if defined(_M_IX86)
#   define plCoreDLL "PatchLibrary32.dll"
#elif defined(_M_X64)
#   define plCoreDLL "PatchLibrary64.dll"
#endif


const char* plGetMainModuleFilename()
{
    static char s_path[MAX_PATH+1];
    static char *s_filename = nullptr;
    if(!s_filename) {
        ::GetModuleFileNameA(nullptr, s_path, sizeof(s_path));
        s_filename = s_path;
        for(int i=0; ; ++i) {
            if(s_path[i]=='\\') { s_filename=s_path+i+1; }
            else if(s_path[i]=='\0') { break; }
        }
    }
    return s_filename;
}

const char* plGetMainModuleDirectory()
{
    static char s_path[MAX_PATH + 1];
    if (s_path[0]=='\0') {
        ::GetModuleFileNameA(nullptr, s_path, sizeof(s_path));
        int last_separator = 0;
        for (int i = 0;; ++i) {
            if (s_path[i] == '\\') { last_separator = i; }
            else if (s_path[i] == '\0') { break; }
        }
        s_path[last_separator] = '\0';
    }
    return s_path;
}

DWORD FindProcess(const char *name)
{
    DWORD result = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(hSnapshot) {
        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hSnapshot, &pe32)) {
            do {
                if (strstr(pe32.szExeFile, name)==0) {
                    result = pe32.th32ProcessID;
                }
            } while (Process32Next(hSnapshot, &pe32));
        }
        CloseHandle(hSnapshot);
    }
    return result;
}

bool IsAlreadyInjected(DWORD pid, const char* dllname)
{
    bool result = false;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    if (hSnapshot) {
        MODULEENTRY32 me32;
        me32.dwSize = sizeof(MODULEENTRY32);
        Module32First(hSnapshot, &me32);
        do {
            if (strstr(me32.szModule, dllname)==0) {
                result = true;
            }
        } while (Module32Next(hSnapshot, &me32));
        CloseHandle(hSnapshot);
    }
    return result;
}

bool InjectDLL(HANDLE hProcess, const char* dllname)
{
    SIZE_T bytesRet = 0;
    DWORD oldProtect = 0;
    LPVOID remote_addr = NULL;
    HANDLE hThread = NULL;
    size_t len = strlen(dllname) + 1;

    remote_addr = ::VirtualAllocEx(hProcess, 0, 1024, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (remote_addr == NULL) { return false; }
    ::VirtualProtectEx(hProcess, remote_addr, len, PAGE_EXECUTE_READWRITE, &oldProtect);
    ::WriteProcessMemory(hProcess, remote_addr, dllname, len, &bytesRet);
    ::VirtualProtectEx(hProcess, remote_addr, len, oldProtect, &oldProtect);

    hThread = ::CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)((void*)&LoadLibraryA), remote_addr, 0, NULL);
    ::WaitForSingleObject(hThread, 3000);
    ::VirtualFreeEx(hProcess, remote_addr, 0, MEM_RELEASE);
    return true;
}

bool InjectDLL(DWORD pid, const char* dllname)
{
    bool result = false;
    HANDLE process = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (process != nullptr) {
        result = InjectDLL(process, dllname);
        ::CloseHandle(process);
    }
    return result;
}


int main(int argc, char *argv[])
{
    if(argc<3) {
        printf("usage %s /target:target_aplication.exe /patch:c:\\fulllpath\\to\\patch.dll\n", plGetMainModuleFilename());
        return 0;
    }

    DWORD target_pid = 0;
    plString patch_fullpath;
    for (int i = 1; i < argc; ++i) {
        if (strncmp(argv[i], "/target:", 8)==0) {
            const char *name = argv[i]+8;
            if (strncmp(name,"pid:", 4)==0) {
                target_pid = atoi(name+4);
            }
            else {
                target_pid = FindProcess(name);
            }
        }
        else if (strncmp(argv[i], "/patch:", 7) == 0) {
            patch_fullpath = argv[i] + 7;
        }
    }
    if (target_pid==0 || patch_fullpath.empty()) {
        return 0;
    }

    if (!IsAlreadyInjected(target_pid, plCoreDLL)) {
        plString coredllpath = plString(plGetMainModuleDirectory()) + "\\" + plCoreDLL;
        if (!InjectDLL(target_pid, coredllpath.c_str())) {
            printf("injection failed\n");
            return 0;
        }
    }

    {
        plInitializeNetwork();

        const char *host = "127.0.0.1";
        uint16_t port = plDefaultPort;
        plString command = "patch " + patch_fullpath;

        plProtocolSocket sock;
        if (sock.open(host, port)) {
            sock.write(command.c_str(), command.size());
        }
    }
}
