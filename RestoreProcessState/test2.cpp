#include <string>
#include <windows.h>

std::string global_variable = "long long global string variable";

__declspec(noinline) void func(int stack_variable)
{
    std::string local_variable = "abc";
    int *heap_variable = new int(123456);

    global_variable += " func1()";

    ::Sleep(2000);
    printf( "func1(): \n"
            "  global_variable: %s\n"
            "  heap_variable: %d (%p)\n"
            "  stack_variable: %d\n"
            , global_variable.c_str(), *heap_variable, heap_variable, stack_variable );

    local_variable.clear();
    *heap_variable = 0;
}


int main(int argc, char *argv[])
{
    ::Sleep(5000);
    for(;;) {
        func((int)argv);
    }
}
