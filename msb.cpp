#include <intrin.h>
#include <cstdio>

inline int msb(int a)
{
#ifdef _MSC_VER
    unsigned long r;
    _BitScanReverse(&r, (unsigned long)a);
    return (int)r;
#else  // _MSC_VER
    return a == 0 ? 0 : 31 - __builtin_clz(a);
#endif // _MSC_VER
}


int main()
{
	int b = msb(128);
	printf("%d\n", 1 << b);
}
