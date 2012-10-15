#include <cstdio>
#define IMPLEMENT_MODULE
#include "module.h"

class Test
{
public:
    Test()
    {
        printf("Test::Test()\n");
    }

    ~Test()
    {
        printf("Test::~Test()\n");
    }
} g_test;


DEFINE_MODULE_MEMFUN(void, Hoge, doSomething, ())
{
    printf("Hoge::doSomethingImpl() after: %d\n", m_data);
}
