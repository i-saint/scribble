#include <cstdio>
#include <windows.h>
#include "RuntimeLinkCPlusPlus.h"
#include "Test.h"



RLCPP_ObjExport float FloatAdd(float a, float b)
{
    return a+b;
}

RLCPP_ObjExport void CallExternalFunc()
{
    OutputDebugStringA("CallExternalFunc()\n");
}

void FuncInExe();
RLCPP_ObjExport void CallExeFunc()
{
    return FuncInExe();
}


class ObjHoge : public IHoge
{
public:
    ObjHoge()
    {
        OutputDebugStringA("ObjHoge::ObjHoge()\n");
    }

    virtual ~ObjHoge()
    {
        OutputDebugStringA("ObjHoge::~ObjHoge()\n");
    }

    virtual void DoSomething()
    {
        OutputDebugStringA("ObjHoge::DoSomething()\n");
    }
};

RLCPP_ObjExport void IHogeReceiver(IHoge *hoge)
{
    hoge->DoSomething();
}

RLCPP_ObjExport IHoge* CreateObjHoge()
{
    return new ObjHoge();
}



RLCPP_OnLoad(
    OutputDebugStringA("RLCPP_OnLoad Test\n");
)

RLCPP_OnUnload(
    OutputDebugStringA("RLCPP_OnUnload Test\n");
)
