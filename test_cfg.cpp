// experiment code for Visual Studio 2015's Control Flow Guard
// ( http://blogs.msdn.com/b/vcblog/archive/2014/12/08/visual-studio-2015-preview-work-in-progress-security-feature.aspx )

#include <cstdio>

typedef void (*funcptr)(char *);

void test2(char *data)
{
    printf("test2()\n");
}

void test1(char *data)
{
    printf("test1()\n");
    *((void**)(data+0x10)) = &test2; // rewrite fp in main() to test2
}

int main()
{
    char data[0x10];
    funcptr fp = &test1;
    fp(data);
    fp(data);
}


// $ cl  /GS- test_cfg.cpp && ./test_cfg
// test1()
// test2()
// 
// $ cl  /GS- /d2guard4 test_cfg.cpp /link /guard:cf && ./test_cfg
// test1()
// test1()
