#include <windows.h>
#include <cstdio>

// write protect がかかっているメモリ領域を強引に書き換える
template<class T>
inline void ForceWrite(T &dst, const T &src)
{
    DWORD old_flag;
    ::VirtualProtect(&dst, sizeof(T), PAGE_EXECUTE_READWRITE, &old_flag);
    dst = src;
    ::VirtualProtect(&dst, sizeof(T), old_flag, &old_flag);
}


class IHoge
{
public:
    virtual ~IHoge() {}
    virtual void __stdcall doSomething()=0;
};

class Hoge : public IHoge
{
public:
    virtual void __stdcall doSomething()
    {
        printf("%p: doSomething\n", this);
    }
};

void __stdcall doSomething_Hooked(Hoge *_this)
{
    printf("%p: doSomething_Hooked\n", _this);
}


int main()
{
    IHoge *h = new Hoge();
    h->doSomething();

    void **&vftable = ((void***)h)[0];
#ifdef _MSC_VER // VisualC++
    ForceWrite(vftable[1], (void*)&doSomething_Hooked);
#else // gcc, clang, etc
    // gcc 系の場合なぜか vftable の 0,1 番目は両方共デストラクタ
    ForceWrite(vftable[2], (void*)&doSomething_Hooked);
#endif
    h->doSomething();

    delete h;
}

/*
$ cl vftableOverride.cpp && ./vftableOverride
00508FC0: doSomething
00508FC0: doSomething_Hooked

$ g++ vftableOverride.cpp && ./a
0x800103e8: doSomething
0x800103e8: doSomething_Hooked

$ clang -S vftableOverride.cpp && g++ vftableOverride.s && ./a
0x80010358: doSomething
0x80010358: doSomething_Hooked
*/
