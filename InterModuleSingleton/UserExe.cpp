#include <windows.h>
#include <cstdio>
#define IMPLEMENT_MODULE
#include "TestClass.h"

IMPLEMENT_INSTANCE(TestClass)

void TestClass::DoSomething()
{
    printf("TestClass::DoSomething()\n");
}


void DoSomethingOnExe()
{
    printf("fron exe: TestClass::GetInstance() %p\n", TestClass::GetInstance());
    TestClass::GetInstance()->DoSomething();
}

int main()
{
    DoSomethingOnExe();
    {
        typedef void (*DoSomethingOnDLLT)();
        HMODULE hm = ::LoadLibrary("UserDll.dll");
        DoSomethingOnDLLT DoSomethingOnDLL = (DoSomethingOnDLLT)GetProcAddress(hm, "DoSomethingOnDLL");
        printf("%p\n", DoSomethingOnDLL);
        DoSomethingOnDLL();
    }

    char c;
    scanf("%c", &c);
}
