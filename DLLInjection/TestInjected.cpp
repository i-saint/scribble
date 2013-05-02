#include <windows.h>
#include <cstdio>

void Test(int i)
{
    void *p = malloc(1024);
    free(p);

    printf("loop %d\n", i);
    fflush(stdout);
    ::Sleep(3000);
}

int main(int argc, char *argv[])
{
    for(int i=0; ; ++i) {
        Test(i);
    }
}
