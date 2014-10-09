#include "CppBehavior.h"

class TestCppBehavior : public CppBehavior
{
typedef CppBehavior super;
public:
    TestCppBehavior(MonoObject *o);
    virtual ~TestCppBehavior();
    void start();
    void update();

    int funcWithParams(int param);
private:
    int m_data;
};



#define mioCurrentClass TestCppBehavior
mioExportCtor()
mioExportDtor()
mioExportMethod(start)
mioExportMethod(update)
//mioExportMethod(funcWithParams)
#undef mioCurrentClass TestCppBehavior



TestCppBehavior::TestCppBehavior(MonoObject *o)
: super(o)
{
    mioDebugPrint("TestCppObject::TestCppObject()\n");
}

TestCppBehavior::~TestCppBehavior()
{
    mioDebugPrint("TestCppObject:~TestCppObject()\n");
}

void TestCppBehavior::start()
{
    mioDebugPrint("TestCppObject::start()\n");
}

void TestCppBehavior::update()
{
    mioDebugPrint("TestCppObject::update()\n");
    if (mioMethod method = findMethod("ThisFunctionWillBeCalledFromCpp")) {
        method.call(m_mobj, nullptr);
    }
}

int TestCppBehavior::funcWithParams(int param)
{
    //mioDebugPrint("%d TestCppObject::FuncWithParams(%d)\n", m_data, param);
    return m_data;
}
