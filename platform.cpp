#if defined(_WIN32) && (_M_X64)
    #define __Windows_x86_64__
#elif defined(__linux__ ) && defined(__x86_64__)
    #define __Linux_x86_64__
#elif defined(__APPLE__) && defined(__x86_64__)
    #define __Mac_x86_64__
#elif defined(__APPLE__) && defined(__arm64__)
    #define __iOS_ARM64__
#elif defined(__ANDROID__) && defined(__arm64__)
    #define __Android_ARM64__
#endif


#include <cstdio>

int main(int argc, char *argv[])
{
    puts(
    #ifdef __Windows_x86_64__
        "__Windows_x86_64__"
    #elif defined(__Linux_x86_64__)
        "__Linux_x86_64__"
    #elif defined(__Mac_x86_64__)
        "__Mac_x86_64__"
    #elif defined(__iOS_ARM64__)
        "__iOS_ARM64__"
    #elif defined(__Android_ARM64__)
        "__Android_ARM64__"
    #endif
    );
}
