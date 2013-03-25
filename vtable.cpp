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
    void __stdcall doSomething()
    {
        printf("doSomething\n");
    }
};

void __stdcall doSomething_Hooked(Hoge *h)
{
    printf("doSomething_Hooked\n");
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
$ cl vtable.cpp /nologo && ./vtable
vtable.cpp
doSomething
doSomething_Hooked

$ g++ vtable.cpp && ./a
doSomething
doSomething_Hooked

$ clang -S vtable.cpp && g++ vtable.s && ./a
doSomething
doSomething_Hooked
*/
