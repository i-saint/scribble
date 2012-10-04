#include <cstdio>
#define IMPLEMENT_MODULE
#include "module.h"

DEFINE_MODULE_MEMFUN(void, Hoge, doSomething, ())
{
    printf("Hoge::doSomethingImpl() after: %d\n", m_data);
}
