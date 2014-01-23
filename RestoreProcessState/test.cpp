#include <string>
#include <windows.h>
#include "rps.h"

int main(int argc, char *argv[])
{
    rpsInitialize();

    std::string test = std::string("abc");
    printf("before serialize. test: %s\n", test.c_str());

    rpsSaveState("test.stat"); // 1. 

    printf("after serialize. test: %s\n", test.c_str());
    test += "def";
    printf("after test+=\"def\". test: %s\n", test.c_str());

    ::Sleep(2000);
    rpsLoadState("test.stat"); // 1. の時の内部状態に戻す

}
