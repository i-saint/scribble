#include <cstdio>
#include <windows.h>
#include <eh.h>

class Hoge
{
public:
    Hoge() { printf("Hoge::Hoge()\n"); }
    ~Hoge() { printf("Hoge::~Hoge()\n"); }
};

void Test()
{
    Hoge hoge;
    *reinterpret_cast<int*>(NULL) = 0xffffffff;
}

void se_translator(unsigned int exception_code, struct _EXCEPTION_POINTERS* exception_information)
{
    throw exception_code;
};

int main()
{
    _set_se_translator(se_translator);
    try {
        Test();
    }
    catch(...) {
        printf("catched\n");
    }
}

/*
$ cl /EHa SEH.cpp
$ ./SEH
Hoge::Hoge()
Hoge::~Hoge()
catched
*/
