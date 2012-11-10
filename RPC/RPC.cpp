#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <stdint.h>
#include <intrin.h>

template<size_t Size>
struct ArgData
{
    ArgData() {}

    template<typename T>
    ArgData(const T &arg)
    {
        assert(sizeof(T) <= Size);
        *((T*)data) = arg;
    }

    template<typename T, size_t S>
    ArgData(const T (&arg)[S])
    {
        assert(sizeof(T)*S <= Size);
        for(size_t i=0; i<S; ++i) {
            ((T*)data)[i] = arg[i];
        }
    }

    char data[Size];
};

enum ArgTypes {
    ArgType_Int,
    ArgType_Float,
    ArgType_Other,
};

enum CallingConvention {
    CC_cdecl,
    CC_thiscall,
    CC_stdcall,
};

struct ArgTypeInfo
{
    size_t type; // ArgTypes
    size_t size;

    template<typename T> static void Create(ArgTypeInfo &t) { t.type=ArgType_Other; t.size=(sizeof(T)+15) & ~15; }
    template<> static void Create<  int8_t>(ArgTypeInfo &t) { t.type=  ArgType_Int; t.size=16; }
    template<> static void Create< uint8_t>(ArgTypeInfo &t) { t.type=  ArgType_Int; t.size=16; }
    template<> static void Create< int16_t>(ArgTypeInfo &t) { t.type=  ArgType_Int; t.size=16; }
    template<> static void Create<uint16_t>(ArgTypeInfo &t) { t.type=  ArgType_Int; t.size=16; }
    template<> static void Create< int32_t>(ArgTypeInfo &t) { t.type=  ArgType_Int; t.size=16; }
    template<> static void Create<uint32_t>(ArgTypeInfo &t) { t.type=  ArgType_Int; t.size=16; }
    template<> static void Create< int64_t>(ArgTypeInfo &t) { t.type=  ArgType_Int; t.size=16; }
    template<> static void Create<uint64_t>(ArgTypeInfo &t) { t.type=  ArgType_Int; t.size=16; }
    template<> static void Create<   float>(ArgTypeInfo &t) { t.type=ArgType_Float; t.size=16; }
    template<> static void Create<  double>(ArgTypeInfo &t) { t.type=ArgType_Float; t.size=16; }
    template<> static void Create<  __m128>(ArgTypeInfo &t) { t.type=ArgType_Float; t.size=16; }

    template<typename T> static void CopyData(void *d, const T v)          { memcpy(d, &v, sizeof(T)); }
    template<> static void CopyData<  int8_t>(void *d, const int8_t v)     { *(uint32_t*)d = v; }
    template<> static void CopyData< uint8_t>(void *d, const uint8_t v)    { *(uint32_t*)d = v; }
    template<> static void CopyData< int16_t>(void *d, const int16_t v)    { *(uint32_t*)d = v; }
    template<> static void CopyData<uint16_t>(void *d, const uint16_t v)   { *(uint32_t*)d = v; }
    template<> static void CopyData< int32_t>(void *d, const int32_t v)    { *(uint32_t*)d = v; }
    template<> static void CopyData<uint32_t>(void *d, const uint32_t v)   { *(uint32_t*)d = v; }
    template<> static void CopyData< int64_t>(void *d, const int64_t v)    { *(uint32_t*)d = v; }
    template<> static void CopyData<uint64_t>(void *d, const uint64_t v)   { *(uint32_t*)d = v; }
    template<> static void CopyData<   float>(void *d, const float v)      { *(__m128d*)d = _mm_set_sd(v); }
    template<> static void CopyData<  double>(void *d, const double v)     { *(__m128d*)d = _mm_set_sd(v); }
    template<> static void CopyData<  __m128>(void *d, const __m128 v)     { *(__m128*)d = v; }
};

struct RPCInfo
{
    char module_name[64];
    union {
        struct {
            void *function;
            short num_args;
            short calling_convention;
        };
        char pad[16];
    };
    // ArgTypeInfo types[num_args];
    // ArgData<types[0].size> data[0]
    // ArgData<types[1].size> data[1]
    // ...
};


template<typename Ret>
RPCInfo* _CreateRPCInfo(Ret (*f)())
{
    size_t alloc_size = sizeof(ArgTypeInfo);
    RPCInfo *ret = (RPCInfo*)_aligned_malloc(alloc_size, 16);
    ret->function = f;
    ret->num_args = 0;

    return ret;
}

template<typename Ret, typename Arg1>
RPCInfo* _CreateRPCInfo(Ret (*f)(Arg1), Arg1 arg1)
{
    ArgTypeInfo types[1];
    ArgTypeInfo::Create<Arg1>(types[0]);

    size_t alloc_size = sizeof(ArgTypeInfo) + sizeof(RPCInfo)*_countof(types) +
                        types[0].size;
    RPCInfo *ret = (RPCInfo*)_aligned_malloc(alloc_size, 16);
    ret->function = f;
    ret->num_args = _countof(types);

    ArgTypeInfo *t = (ArgTypeInfo*)(ret+1);
    char *d = (char*)(t+_countof(types));
    for(size_t i=0; i<_countof(types); ++i) { t[i]=types[i]; }
    ArgTypeInfo::CopyData(d, arg1); d+=types[0].size;

    return ret;
}

template<typename Ret, typename Arg1, typename Arg2>
RPCInfo* _CreateRPCInfo(Ret (*f)(Arg1, Arg2), Arg1 arg1, Arg2 arg2)
{
    ArgTypeInfo types[2];
    ArgTypeInfo::Create<Arg1>(types[0]);
    ArgTypeInfo::Create<Arg2>(types[1]);

    size_t alloc_size = sizeof(ArgTypeInfo) + sizeof(RPCInfo)*_countof(types) +
                        types[0].size + types[1].size;
    RPCInfo *ret = (RPCInfo*)_aligned_malloc(alloc_size, 16);
    ret->function = f;
    ret->num_args = _countof(types);

    ArgTypeInfo *t = (ArgTypeInfo*)(ret+1);
    char *d = (char*)(t+_countof(types));
    for(size_t i=0; i<_countof(types); ++i) { t[i]=types[i]; }
    ArgTypeInfo::CopyData(d, arg1); d+=types[0].size;
    ArgTypeInfo::CopyData(d, arg2); d+=types[1].size;

    return ret;
}

template<typename Ret, typename Arg1, typename Arg2, typename Arg3>
RPCInfo* _CreateRPCInfo(Ret (*f)(Arg1, Arg2, Arg3), Arg1 arg1, Arg2 arg2, Arg3 arg3)
{
    ArgTypeInfo types[3];
    ArgTypeInfo::Create<Arg1>(types[0]);
    ArgTypeInfo::Create<Arg2>(types[1]);
    ArgTypeInfo::Create<Arg3>(types[2]);

    size_t alloc_size = sizeof(ArgTypeInfo) + sizeof(RPCInfo)*_countof(types) +
                        types[0].size + types[1].size + types[2].size;
    RPCInfo *ret = (RPCInfo*)_aligned_malloc(alloc_size, 16);
    ret->function = f;
    ret->num_args = _countof(types);

    ArgTypeInfo *t = (ArgTypeInfo*)(ret+1);
    char *d = (char*)(t+_countof(types));
    for(size_t i=0; i<_countof(types); ++i) { t[i]=types[i]; }
    ArgTypeInfo::CopyData(d, arg1); d+=types[0].size;
    ArgTypeInfo::CopyData(d, arg2); d+=types[1].size;
    ArgTypeInfo::CopyData(d, arg3); d+=types[2].size;

    return ret;
}

template<typename Ret, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
RPCInfo* _CreateRPCInfo(Ret (*f)(Arg1, Arg2, Arg3, Arg4), Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
{
    ArgTypeInfo types[4];
    ArgTypeInfo::Create<Arg1>(types[0]);
    ArgTypeInfo::Create<Arg2>(types[1]);
    ArgTypeInfo::Create<Arg3>(types[2]);
    ArgTypeInfo::Create<Arg3>(types[3]);

    size_t alloc_size = sizeof(ArgTypeInfo) + sizeof(RPCInfo)*_countof(types) +
                        types[0].size + types[1].size + types[2].size + types[3].size;
    RPCInfo *ret = (RPCInfo*)_aligned_malloc(alloc_size, 16);
    ret->function = f;
    ret->num_args = _countof(types);

    ArgTypeInfo *t = (ArgTypeInfo*)(ret+1);
    char *d = (char*)(t+_countof(types));
    for(size_t i=0; i<_countof(types); ++i) { t[i]=types[i]; }
    ArgTypeInfo::CopyData(d, arg1); d+=types[0].size;
    ArgTypeInfo::CopyData(d, arg2); d+=types[1].size;
    ArgTypeInfo::CopyData(d, arg3); d+=types[2].size;
    ArgTypeInfo::CopyData(d, arg4); d+=types[3].size;

    return ret;
}

template<typename Ret> RPCInfo* CreateRPCInfo(Ret (*f)());
template<typename Ret, typename Arg1> RPCInfo* CreateRPCInfo(Ret (*f)(Arg1), Arg1 arg1);
template<typename Ret, typename Arg1, typename Arg2> RPCInfo* CreateRPCInfo(Ret (*f)(Arg1, Arg2), Arg1 arg1, Arg2 arg2);
template<typename Ret, typename Arg1, typename Arg2, typename Arg3> RPCInfo* CreateRPCInfo(Ret (*f)(Arg1, Arg2, Arg3), Arg1 arg1, Arg2 arg2, Arg3 arg3);
template<typename Ret, typename Arg1, typename Arg2, typename Arg3, typename Arg4> RPCInfo* CreateRPCInfo(Ret (*f)(Arg1, Arg2, Arg3, Arg4), Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4);


extern "C" void RPCCall(const void *args);


void Test1(float f)
{
    printf("Test1(%f)\n", f);
}


int main()
{
    Test1(100.0f);
    {
        RPCInfo *ri = CreateRPCInfo(&Test1, 100.0f);
        RPCCall(ri);
        _aligned_free(ri);
    }
    {
        typedef int (*FP)(ArgData<64>, int, float);
        char format[64] = "test %d %.2f\n";
        RPCInfo *ri = CreateRPCInfo((FP)&printf, ArgData<64>(format), 10, 100.0f);
        RPCCall(ri);
        _aligned_free(ri);
    }
}
