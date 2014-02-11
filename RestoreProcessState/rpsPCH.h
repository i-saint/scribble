#ifndef rpsPCH
#define rpsPCH

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <windows.h>
#include <ws2tcpip.h>
#include <winsock2.h>
#include <dbghelp.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <d3d9.h>
#include <d3d11.h>
#ifdef max
#   undef max
#   undef min
#endif // max

#endif // rpsPCH
