#include <windows.h>
#include <cstdio>
#include <string>
#pragma comment(lib, "Advapi32.lib")


size_t dpReadRegistry(HKEY hk, const char *pos, const char *name, void *out_buf, size_t bufsize)
{
    DWORD ret = 0;
    HKEY key;
    DWORD size = (DWORD)bufsize;
    if(::RegOpenKeyExA(hk, pos, 0, KEY_READ, &key)==ERROR_SUCCESS) {
        if(::RegQueryValueExA(key, name, nullptr, nullptr, (LPBYTE)out_buf, &size )==ERROR_SUCCESS) {
            ret = size;
        }
        ::RegCloseKey( key );
    }
    return ret;
}

bool dpWriteRegistry(HKEY hk, const char *pos, const char *name, void *buf, size_t bufsize)
{
    bool ret = false;
    HKEY key;
    if( ::RegCreateKeyExA(hk, pos, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &key, nullptr)==ERROR_SUCCESS) {
        if(::RegSetValueExA(key, name, 0, 0, (LPBYTE)buf, bufsize)==ERROR_SUCCESS  ) {
            ret = true;
        }
        ::RegCloseKey( key );
    }
    return ret;
}


int main(int argc, char *argv[])
{
    const char reg_subkey[] = "SOFTWARE\\primitive\\scribble";
    const char value_name[] = "TestValue";
    std::string data = "this is test";

	if(dpWriteRegistry(HKEY_CURRENT_USER, reg_subkey, value_name, &data[0], data.size())) {
        char buf[256];
        size_t ret = dpReadRegistry(HKEY_CURRENT_USER, reg_subkey, value_name, buf, sizeof(buf));
        printf("value: %s\n", buf);
    }
}
