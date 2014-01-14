#include <cstdio>

extern "C" {

int test_add(int a, int b)
{
    return a+b;
}

void test_call(int a)
{
    printf("test_call(%d)\n", a);
}

}; // extern "C"
