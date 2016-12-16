#include <cstdio>
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")

bool Demangle_Signatured(const char *mangled, char *demangled, size_t buflen)
{
    return ::UnDecorateSymbolName(mangled, demangled, (DWORD)buflen, 
        UNDNAME_NO_MS_KEYWORDS|UNDNAME_NO_ALLOCATION_MODEL|UNDNAME_NO_ALLOCATION_LANGUAGE|
        UNDNAME_NO_MS_THISTYPE|UNDNAME_NO_CV_THISTYPE|UNDNAME_NO_THISTYPE|UNDNAME_NO_ACCESS_SPECIFIERS|
        UNDNAME_NO_RETURN_UDT_MODEL)!=0;
}

bool Demangle_NameOnly(const char *mangled, char *demangled, size_t buflen)
{
    return ::UnDecorateSymbolName(mangled, demangled, (DWORD)buflen, UNDNAME_NAME_ONLY)!=0;
}

int main(int argc, char *argv[])
{
    bool name_only = false;
    char demangled[MAX_SYM_NAME];

    for (int i = 1; i < argc; ++i) {
        if (strncmp(argv[i], "/n", 2) == 0) {
            name_only = true;
        }
        else {
            if (name_only) {
                Demangle_NameOnly(argv[i], demangled, sizeof(demangled));
            }
            else {
                Demangle_Signatured(argv[i], demangled, sizeof(demangled));
            }
            printf("%s\n", demangled);
        }
    }
}
