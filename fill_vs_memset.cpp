#include <cstdio>
#include <cstring>
#include <cstdint>
#include <algorithm>
#include <chrono>
#include <vector>


// 1GB
#define DataSize 0x40000000
#define NumTry 16


#ifdef _MSC_VER
    #define _noinline __declspec(noinline)

#else
    #define _noinline __attribute__((noinline))
#endif


_noinline void Fill(char *dst, size_t size, uint8_t c)
{
    std::fill(dst, dst + size, c);
}

_noinline void Memset(char *dst, size_t size, uint8_t c)
{
    memset(dst, c, size);
}


int main(int argc, char *argv[])
{
    std::vector<char> data(DataSize);
    std::chrono::time_point<std::chrono::system_clock> start;
    std::chrono::duration<float> elapsed;

    start = std::chrono::system_clock::now();
    for (int i = 0; i < NumTry; ++i) {
        Fill(&data[0], data.size(), i);
    }
    elapsed = std::chrono::system_clock::now() - start;
    printf("Fill: %.4f\n", elapsed.count());


    start = std::chrono::system_clock::now();
    for (int i = 0; i < NumTry; ++i) {
        Memset(&data[0], data.size(), i);
    }
    elapsed = std::chrono::system_clock::now() - start;
    printf("Memset: %.4f\n", elapsed.count());
}

/*

$ cl /nologo /O2 /EHsc fill_vs_memset.cpp && ./fill_vs_memset
fill_vs_memset.cpp
Fill: 5.3456
Memset: 0.8350

$ g++ -O2 -std=gnu++11 fill_vs_memset.cpp && ./a
Fill: 4.8300
Memset: 0.8497


assembly output on Visual Studio 2015 Update1

?Fill@@YAXPEAD_KE@Z (void __cdecl Fill(char *,unsigned __int64,unsigned char)):
0000000000000000: 48 8D 04 11        lea         rax,[rcx+rdx]
0000000000000004: 33 D2              xor         edx,edx
0000000000000006: 4C 8B C8           mov         r9,rax
0000000000000009: 4C 2B C9           sub         r9,rcx
000000000000000C: 48 3B C8           cmp         rcx,rax
000000000000000F: 4C 0F 47 CA        cmova       r9,rdx
0000000000000013: 4D 85 C9           test        r9,r9
0000000000000016: 74 18              je          0000000000000030
0000000000000018: 48 8B D1           mov         rdx,rcx
000000000000001B: 48 F7 DA           neg         rdx
000000000000001E: 66 90              xchg        ax,ax
0000000000000020: 44 88 01           mov         byte ptr [rcx],r8b
0000000000000023: 48 8D 49 01        lea         rcx,[rcx+1]
0000000000000027: 48 8D 04 0A        lea         rax,[rdx+rcx]
000000000000002B: 49 3B C1           cmp         rax,r9
000000000000002E: 75 F0              jne         0000000000000020
0000000000000030: C3                 ret

?Memset@@YAXPEAD_KE@Z (void __cdecl Memset(char *,unsigned __int64,unsigned char)):
0000000000000000: 48 8B C2           mov         rax,rdx
0000000000000003: 41 0F B6 D0        movzx       edx,r8b
0000000000000007: 4C 8B C0           mov         r8,rax
000000000000000A: E9 00 00 00 00     jmp         memset

*/
