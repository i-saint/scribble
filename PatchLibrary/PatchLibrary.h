#include <windows.h>

HMODULE WINAPI PatchLibrary(const char *path);
BOOL WINAPI UnpatchLibrary(HMODULE mod);
