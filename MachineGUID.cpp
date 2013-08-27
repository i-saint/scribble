#include <windows.h>
#include <string>
#include <cstdio>
#pragma comment(lib,"advapi32.lib")

// MachineGUID (Windows インストール時に生成され、再インストールするまでは変わらない ID) を取得。
// UUID が欲しい場合は WinAPI の UuidCreate() とかを。
// 
// thanks to
// http://stackoverflow.com/questions/99880/generating-a-unique-machine-id
std::string GetMachineGUID()
{
    std::string ret;
    char value[64];
    DWORD size = _countof(value);
    DWORD type = REG_SZ;
    HKEY key;
    LONG retKey = ::RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Cryptography", 0, KEY_READ|KEY_WOW64_64KEY, &key);
    LONG retVal = ::RegQueryValueExA(key, "MachineGuid", nullptr, &type, (LPBYTE)value, &size );
    if( retKey==ERROR_SUCCESS && retVal==ERROR_SUCCESS  ) {
        ret = value;
    }
    ::RegCloseKey( key );
    return ret;
}


int main(int argc, char *argv[])
{
    std::string guid = GetMachineGUID();
    printf("GetMachineGUID(): %s\n", guid.c_str());
}

// $ cl MachineGUID.cpp /EHsc /Zi && ./MachineGUID
// GetMachineGUID(): xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
