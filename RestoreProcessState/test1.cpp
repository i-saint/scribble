#include <string>
#include <windows.h>
#include "rps.h"

std::string global_variable = "g";

void func1(int stack_variable)
{
    std::string local_variable = "abc";
    int *heap_variable = new int(123456);

    rpsSaveState("test.stat"); // 1. この時点の状態を保存

    global_variable += " func1()";

    printf( "func1(): \n"
            "  global_variable: %s\n"
            "  heap_variable: %d\n"
            "  stack_variable: %d\n"
            , global_variable.c_str(), *heap_variable, stack_variable );

    local_variable.clear();
    *heap_variable = 0;
}

void func2()
{
    global_variable += " func2()";

    printf( "func2(): \n"
            "  g_global_variable: %s\n\n"
            , global_variable.c_str());

    ::Sleep(2000);
    rpsLoadState("test.stat"); // 2. 1. の状態を復元 (メモリの状態やプログラムカウンタが 1. の時点に戻る)
}


int main(int argc, char *argv[])
{
    func1((int)argv);
    func2();
}

/*
result:

func1():
  global_variable: g func1()
  heap_variable: 123456
  stack_variable: 5358520
func2():
  g_global_variable: g func1() func2()

func1():
  global_variable: g func1()
  heap_variable: 123456
  stack_variable: 5358520
func2():
  g_global_variable: g func1() func2()

func1():
  global_variable: g func1()
  heap_variable: 123456
  stack_variable: 5358520
func2():
  g_global_variable: g func1() func2()

  ... (無限ループ)
*/
