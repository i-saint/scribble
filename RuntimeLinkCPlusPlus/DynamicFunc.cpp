#include <cstdio>
#include <windows.h>
#include "Interface.h"

extern "C" float FloatAdd(float a, float b)
{
    return a+b;
}

extern "C" float FloatSub(float a, float b)
{
    return a-b;
}

extern "C" float FloatMul(float a, float b)
{
    return a*b;
}

extern "C" float FloatDiv(float a, float b)
{
    return a/b;
}

extern "C" void IHogeReceiver(IHoge *hoge)
{
    hoge->DoSomething();
}

extern "C" void CallExternalFunc()
{
    return OutputDebugStringA("CallExternalFunc()\n");
}

void FuncInExe();
extern "C" void CallExeFunc()
{
    return FuncInExe();
}
