
#include <intrin.h>
#include <algorithm>
#pragma intrinsic(memcpy)
#pragma intrinsic(memset)
#pragma intrinsic(memcmp)

typedef __m128i             uint128;
typedef unsigned long long  uint64;
typedef unsigned int        uint32;
typedef unsigned short      uint16;
typedef unsigned char       uint8;

__forceinline void memset128(uint128 *dst, uint128 pattern, size_t size_byte)
{
    // 可能であれば 128 byte 一気に埋める 
    size_t n = size_byte >> 4;
    size_t blocks = n >> 3;
    for(size_t i=0; i<blocks; ++i) {
        uint128 *d = dst+(i*8);
        d[0] = pattern;
        d[1] = pattern;
        d[2] = pattern;
        d[3] = pattern;
        d[4] = pattern;
        d[5] = pattern;
        d[6] = pattern;
        d[7] = pattern;
    }

    // 余った領域を埋める
    size_t remain = n & 7;
    uint128 *d = dst + (n & ~7);
    for(size_t i=0; i<remain; ++i) {
        d[i] = pattern;
    }
}

__forceinline void memset64(uint64 *dst, uint64 pattern, size_t size_byte)
{
    // 可能な部分は memset128 で埋める
    uint128 *dst128 = (uint128*)(((size_t)dst + 0xf) & ~0xf);
    size_t gap = (size_t)dst128-(size_t)dst;
    __declspec(align(16)) uint64 pattern128[2] = {pattern, pattern};
    memset128(dst128, (uint128&)pattern128, size_byte-gap);

    // 最初と最後の要素が埋まってない可能性があるので埋める
    dst[0] = pattern;
    dst[(size_byte>>3)-1] = pattern;
}

__forceinline void memset32(uint32 *dst, uint32 pattern, size_t size_byte)
{
    // std::fill_n は 32bit 以下の型だと　rep stos　に化けて、これは SSE レジスタ使う fill より速い
    // (少なくとも VisualC++2010 の場合)
    std::fill_n(dst, size_byte/sizeof(uint32), pattern);
}

__forceinline void memset16(uint16 *dst, uint16 pattern, size_t size_byte)
{
    // 同上
    std::fill_n(dst, size_byte/sizeof(uint16), pattern);
}




// 以下テストコード


#include <windows.h>
typedef float float32;

class Timer
{
public:
    Timer()
    {
        ::QueryPerformanceFrequency( &m_freq );
        reset();
    }

    void reset()
    {
        m_start.QuadPart = 0;
        ::QueryPerformanceCounter( &m_start );
    }

    float32 getElapsedMillisec() const
    {
        LARGE_INTEGER end;
        ::QueryPerformanceCounter( &end );
        return ((float32)(end.QuadPart - m_start.QuadPart) / (float32)m_freq.QuadPart)*1000.0f;
    }

private:
    LARGE_INTEGER m_freq;
    LARGE_INTEGER m_start;
};

// 最適化でデータやそれに関する処理を消さないようにする関数
#pragma optimize("", off)
void PreventOptimizeOut(void *p) {}
#pragma optimize("", on)


void test()
{
    Timer timer;
    const size_t data_size = 1024*1024*4; // 4MB
    const size_t num_try = 1024;
    const uint16 pattern16 = 0xffff;
    const uint32 pattern32 = 0xffffffff;
    const uint64 pattern64 = 0xffffffffffffffff;
    const uint128 pattern128 = _mm_set1_epi32(0xffffffff);
    char *data = (char*)_aligned_malloc(data_size, 16);

    printf("fill %d byte, %d times\n", data_size, num_try);


    timer.reset();
    for(size_t i=0; i<num_try; ++i) {
        memset(data, 0xff, data_size);
        PreventOptimizeOut(data);
    }
    printf("memset: %fms\n", timer.getElapsedMillisec());


    timer.reset();
    for(size_t i=0; i<num_try; ++i) {
        std::fill_n((uint16*)data, data_size/sizeof(uint16), pattern16);
        PreventOptimizeOut(data);
    }
    printf("std::fill_n<uint16>: %fms\n", timer.getElapsedMillisec());


    timer.reset();
    for(size_t i=0; i<num_try; ++i) {
        std::fill_n((uint32*)data, data_size/sizeof(uint32), pattern32);
        PreventOptimizeOut(data);
    }
    printf("std::fill_n<uint32>: %fms\n", timer.getElapsedMillisec());


    timer.reset();
    for(size_t i=0; i<num_try; ++i) {
        std::fill_n((uint64*)data, data_size/sizeof(uint64), pattern64);
        PreventOptimizeOut(data);
    }
    printf("std::fill_n<uint64>: %fms\n", timer.getElapsedMillisec());


    timer.reset();
    for(size_t i=0; i<num_try; ++i) {
        std::fill_n((uint128*)data, data_size/sizeof(uint128), pattern128);
        PreventOptimizeOut(data);
    }
    printf("std::fill_n<uint128>: %fms\n", timer.getElapsedMillisec());


    timer.reset();
    for(size_t i=0; i<num_try; ++i) {
        memset64((uint64*)(data+8), pattern64, data_size-16);
        PreventOptimizeOut(data);
    }
    printf("memset64: %fms\n", timer.getElapsedMillisec());


    timer.reset();
    for(size_t i=0; i<num_try; ++i) {
        memset128((uint128*)data, pattern128, data_size);
        PreventOptimizeOut(data);
    }
    printf("memset128: %fms\n", timer.getElapsedMillisec());


    _aligned_free(data);
}

int main()
{
	test();
}

/*
$ cl /Ox /GL /arch:SSE2 memset.cpp
$ ./memset

fill 4194304 byte, 1024 times
memset: 137.229843ms
std::fill_n<uint16>: 135.547592ms
std::fill_n<uint32>: 134.988266ms
std::fill_n<uint64>: 912.609436ms
std::fill_n<uint128>: 451.081573ms
memset64: 172.819122ms
memset128: 172.916916ms
*/
