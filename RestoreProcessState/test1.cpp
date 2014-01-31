#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <windows.h>
#include "rps.h"

std::string global_variable = "g";

void func1(int stack_variable)
{
    char buf[256];

    std::string local_variable = "abc";
    int *heap_variable = new int(1234567);
    int *page_memory = (int*)::VirtualAlloc(nullptr, 4, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
    *page_memory = 7654321;
    FILE *file_for_read = fopen("test1.cpp", "rb");
    FILE *file_for_write = fopen("test_write.txt", "wb");
    fgets(buf, _countof(buf), file_for_read);
    fputs("before serialization\n", file_for_write);

    rpsSaveState("test.stat"); // 1. この時点の状態を保存

    global_variable += " func1()";

    printf( "func1(): \n"
            "  global_variable: %s\n"
            "  heap_variable: %d\n"
            "  page_memory: %d\n"
            "  stack_variable: %d\n"
            , global_variable.c_str(), *heap_variable, *page_memory, stack_variable );

    fputs("after serialization\n", file_for_write);
    fgets(buf, _countof(buf), file_for_read);
    printf("  fgets: %s\n", buf);
    fclose(file_for_write);
    fclose(file_for_read);

    local_variable.clear();
    *heap_variable = 0;
    *page_memory = 0;
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
