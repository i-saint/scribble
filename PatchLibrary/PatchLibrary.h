#include <windows.h>

HMODULE WINAPI PatchLibraryA(const char *path);
HMODULE WINAPI PatchLibraryExA(const char *path);
HMODULE WINAPI PatchLibraryW(const wchar_t *path);
HMODULE WINAPI PatchLibraryExW(const wchar_t *path);
BOOL WINAPI UnpatchLibrary(HMODULE mod);
