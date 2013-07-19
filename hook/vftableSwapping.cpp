#include <cstdio>

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

    void **&vtable = ((void***)h)[0];
#ifdef _MSC_VER // VisualC++
    void *new_vtbl[] = {vtable[0], (void*)&doSomething_Hooked};
    vtable = new_vtbl;
#else // gcc, clang, etc
    // gcc 系の場合なぜか vtable の 0,1 番目は両方共デストラクタ
    void *new_vtbl[] = {vtable[0], vtable[1], (void*)&doSomething_Hooked};
    vtable = new_vtbl;
#endif
    h->doSomething();

    delete h;
}

/*
$ cl vftableSwapping.cpp && ./vftableSwapping
00508FC0: doSomething
00508FC0: doSomething_Hooked

$ g++ vftableSwapping.cpp && ./a
0x80010358: doSomething
0x80010358: doSomething_Hooked

$ clang -S vftableSwapping.cpp && g++ vftableSwapping.s && ./a
0x80010358: doSomething
0x80010358: doSomething_Hooked
*/
