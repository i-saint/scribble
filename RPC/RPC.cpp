#include <cstdio>
#include <cstring>
#include <algorithm>

template<size_t Size>
struct ArgmentData
{
    ArgmentData() {}

    template<typename T>
    ArgmentData(const T &arg)
    {
        assert(sizeof(T) <= Size);
        *((T*)data) = arg;
    }

    template<typename T, size_t S>
    ArgmentData(const T (&arg)[S])
    {
        assert(sizeof(T)*s <= Size);
        std::copy(arg, arg+S, (T*)data);
    }

    char data[Size];
};

enum ArgTypes {
    ArgType_Int,
    ArgType_Float,
    ArgType_Other,
};

struct ArgmentTypeInfo
{
    int type; // ArgTypes
    int size;
};

struct APCInfo
{
    char module_name[64];
    void *function;
    int num_args;
    // ArgmentTypeInfo types[num_args];
    // ArgmentData<types[0].size> data[0]
    // ArgmentData<types[1].size> data[1]
    // ...
};


template<typename Ret>
APCInfo* CreateRPCInfo(Ret (*f)())
{
    size_t num_args = 0;
    size_t alloc_size = sizeof(ArgmentTypeInfo) + sizeof(APCInfo)*num_args;
}

template<typename Ret, typename Arg1>
APCInfo* CreateRPCInfo(Ret (*f)(Arg1), Arg1 arg1)
{
    size_t num_args = 1;
    size_t alloc_size = sizeof(ArgmentTypeInfo) + sizeof(APCInfo)*num_args
        + sizeof(Arg1);
}

template<typename Ret, typename Arg1, typename Arg2>
APCInfo* CreateRPCInfo(Ret (*f)(Arg1, Arg2), Arg1 arg1, Arg2 arg2)
{
    size_t num_args = 2;
    size_t alloc_size = sizeof(ArgmentTypeInfo) + sizeof(APCInfo)*num_args
        + sizeof(Arg1) + sizeof(Arg2);
}

template<typename Ret, typename Arg1, typename Arg2, typename Arg3>
APCInfo* CreateRPCInfo(Ret (*f)(Arg1, Arg2, Arg3), Arg1 arg1, Arg2 arg2, Arg3 arg3)
{
    size_t num_args = 3;
    size_t alloc_size = sizeof(ArgmentTypeInfo) + sizeof(APCInfo)*num_args
        + sizeof(Arg1) + sizeof(Arg2) + sizeof(Arg3);
}

template<typename Ret, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
APCInfo* CreateRPCInfo(Ret (*f)(Arg1, Arg2, Arg3, Arg4), Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
{
    size_t num_args = 4;
    size_t alloc_size = sizeof(ArgmentTypeInfo) + sizeof(APCInfo)*num_args
        + sizeof(Arg1) + sizeof(Arg2) + sizeof(Arg3) + sizeof(Arg4);
}

extern "C" void RPCCall(const void *args);



void TestFunc(void* arg1, short arg2, ArgmentData<8> arg3, float arg4)
{
    printf("hoge\n");
}


int main()
{
    struct Test {
        void *fp;
        const char *format;
        size_t idata;
    } testdata = {&printf, "data: %d\n", 100};
    RPCCall(&testdata);

    TestFunc(NULL, 10, ArgmentData<8>(), 10.0f);
}
