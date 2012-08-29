#include "stdafx.h"
#include <windows.h>
#include "RuntimeLinkCPlusPlus.h"
#pragma warning(disable: 4996) // _s じゃない CRT 関数使うとでるやつ


#define istPrint(...) DebugPrint(__VA_ARGS__)

template<size_t N>
inline int istvsprintf(char (&buf)[N], const char *format, va_list vl)
{
    return _vsnprintf(buf, N, format, vl);
}

static const int DPRINTF_MES_LENGTH  = 4096;
void DebugPrintV(const char* fmt, va_list vl)
{
    char buf[DPRINTF_MES_LENGTH];
    istvsprintf(buf, fmt, vl);
    ::OutputDebugStringA(buf);
}

void DebugPrint(const char* fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    DebugPrintV(fmt, vl);
    va_end(vl);
}



// .obj から呼ぶ関数。最適化で消えないように RLCPP_Fixate つけておく
RLCPP_Fixate void FuncInExe()
{
    istPrint("FuncInExe()\n");
}


class IHoge
{
public:
    virtual ~IHoge() {}
    virtual void DoSomething()=0;
};

class Hoge : public IHoge
{
public:
    virtual void DoSomething()
    {
        istPrint("Hoge::DoSomething()\n");
    }
};


RLCPP_DefineObjFunc(float, FloatAdd, float, float);
RLCPP_DefineObjFunc(void, CallExternalFunc);
RLCPP_DefineObjFunc(void, CallExeFunc);
RLCPP_DefineObjFunc(void, IHogeReceiver, IHoge*);
RLCPP_DefineObjFunc(IHoge*, CreateObjHoge);

int main(int argc, _TCHAR* argv[])
{
    RLCPP_InitializeLoader();
    RLCPP_Load("DynamicFunc.obj");
    RLCPP_Link();

    RLCPP_GetFunction(FloatAdd);
    RLCPP_GetFunction(CallExternalFunc);
    RLCPP_GetFunction(CallExeFunc);
    RLCPP_GetFunction(IHogeReceiver);
    RLCPP_GetFunction(CreateObjHoge);

    istPrint("%.2f\n", FloatAdd(1.0f, 2.0f));

    CallExternalFunc();
    CallExeFunc();

    {
        Hoge hoge;
        IHogeReceiver(&hoge);
    }
    {
        IHoge *hoge = CreateObjHoge();
        hoge->DoSomething();
        delete hoge;
    }

    RLCPP_FinalizeLoader();

    return 0;
}
