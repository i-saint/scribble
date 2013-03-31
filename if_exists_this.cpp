#include <cstdio>

#define PrintMemfnOfNot()\
    __if_exists(this)       { printf("memfun\n"); }\
    __if_not_exists(this)   { printf("not memfun\n"); }


class Hoge
{
public:
    Hoge()
    {
        printf("Hoge::Hoge(): ");
        PrintMemfnOfNot();
    }
};

int main(int argc, char *argv[])
{
    Hoge hoge;

    printf("main(): ");
    PrintMemfnOfNot();
}

/*
$ cl if_exists_this.cpp
$ ./if_exists_this
Hoge::Hoge(): memfun
main(): not memfun


$ clang if_exists_this.cpp -fms-extensions
if_exists_this.cpp:14:9: error: expected unqualified-id
        PrintMemfnOfNot();
        ^
if_exists_this.cpp:4:17: note: expanded from macro 'PrintMemfnOfNot'
    __if_exists_this.this)       { printf("memfun\n"); }\
...
*/