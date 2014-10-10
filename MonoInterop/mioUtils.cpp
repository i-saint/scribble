#ifdef WIN32
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#endif // WIN32
#include "mioUtils.h"


void mioDebugPrint(const char *format, ...)
{
    va_list args;
    va_start(args, format);
#ifdef WIN32
    char buf[1024 * 2];
    vsprintf(buf, format, args);
    ::OutputDebugStringA(buf);
#else  // WIN32
    vprintf(format, args);
#endif // WIN32
    va_end(args);
}

