#include <cstdio>
#include <windows.h>
#include "module.h"
#pragma comment(lib, "before.lib")


Hoge::Hoge()
    : m_data(0)
{
}

Hoge::~Hoge()
{
}

DEFINE_MAIN_MEMFUN(void, Hoge, doSomething, ());




template<class To, class From>
To ForceCast(From from)
{
    union { From from; To to; } u;
    u.from = from;
    return u.to;
}

template<class To, class From>
To ForceCast(From from, To /*å^êÑë™ÇÃÇΩÇﬂÇæÇØÇÃà¯êî*/)
{
    return ForceCast<To, From>(from);
}


int main()
{
    Hoge hoge;
    hoge.doSomething();

    {
        HMODULE module_main = ::GetModuleHandle("main.exe");
        HMODULE module_after = ::LoadLibrary("after.dll");
        printf("%s\n", g_Hoge_doSomething_Name);
        g_Hoge_doSomething = *ForceCast(::GetProcAddress(module_after, g_Hoge_doSomething_Name), &g_Hoge_doSomething);
    }

    hoge.doSomething();
}
/*
Hoge::doSomething() before: 0
Hoge::doSomething() after: 0
*/
