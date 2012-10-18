#include <cstdio>
#include <cstdlib>

struct TestData
{
    int data[8];
};

void Hoge(TestData arg)
{
    for(size_t i=0; i<_countof(arg.data); ++i) {
        printf("%d\n", arg.data[i]);
    }
}

int main()
{
    TestData arg;
    for(size_t i=0; i<_countof(arg.data); ++i) { arg.data[i]=i; }

    typedef void (*FP)(TestData*);
    FP func = (FP)&Hoge;
    func(&arg);
}

/*
result:

0
1
2
3
4
5
6
7
*/
