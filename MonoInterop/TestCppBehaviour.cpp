#include "CppBehaviour.h"

class TestCppBehaviour : public CppBehaviour
{
typedef CppBehaviour super;
public:
    TestCppBehaviour(MonoObject *o);
    virtual ~TestCppBehaviour();
    void start();
    void update();

    int funcWithParams(int param);
private:
    int m_data;
};



#define mioCurrentClass TestCppBehaviour
mioExportCtor()
mioExportDtor()
mioExportMethod(start)
mioExportMethod(update)
//mioExportMethod(funcWithParams)
#undef mioCurrentClass



TestCppBehaviour::TestCppBehaviour(MonoObject *o)
: super(o)
{
    mioDebugPrint("TestCppBehaviour::TestCppBehaviour()\n");
}

TestCppBehaviour::~TestCppBehaviour()
{
    mioDebugPrint("TestCppBehaviour:~TestCppBehaviour()\n");
}

void TestCppBehaviour::start()
{
    mioDebugPrint("TestCppBehaviour::start()\n");
}

void TestCppBehaviour::update()
{
    mioDebugPrint("TestCppBehaviour::update()\n");
    if (mioMethod method = findMethod("ThisFunctionWillBeCalledFromCpp")) {
        method.invoke(m_mobj, nullptr);
    }
}

int TestCppBehaviour::funcWithParams(int param)
{
    //mioDebugPrint("%d TestCppBehaviour::FuncWithParams(%d)\n", m_data, param);
    return m_data;
}
