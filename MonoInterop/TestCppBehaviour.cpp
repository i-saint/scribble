#include "CppBehaviour.h"

class TestCppBehaviour : public CppBehaviour
{
typedef CppBehaviour super;
public:
    TestCppBehaviour(MonoObject *o);
    virtual ~TestCppBehaviour();
    void start();
    void update();

    int memfn1(int a1);
    int memfn2(int a1, int a2);
    int cmemfn1(int a1) const;
    int cmemfn2(int a1, int a2) const;
private:
    int m_frame;
};



#define mioCurrentClass TestCppBehaviour
mioExportClass()
mioExportMethod(start)
mioExportMethod(update)
mioExportMethod(memfn1)
mioExportMethod(memfn2)
mioExportMethod(cmemfn1)
mioExportMethod(cmemfn2)
#undef mioCurrentClass



TestCppBehaviour::TestCppBehaviour(MonoObject *o)
: super(o), m_frame(0)
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
    if (++m_frame % 60 == 0) {
        mioDebugPrint("TestCppBehaviour::update()\n");
        if (mioMethod method = findMethod("ThisFunctionWillBeCalledFromCpp")) {
            method.invoke(m_mobj, nullptr);
        }
    }
}

int TestCppBehaviour::memfn1(int a1)
{
    mioDebugPrint("TestCppBehaviour::memfn1(%d) : %d\n", a1, m_frame);
    return m_frame;
}

int TestCppBehaviour::memfn2(int a1, int a2)
{
    mioDebugPrint("TestCppBehaviour::memfn2(%d, %d) : %d\n", a1, a2, m_frame);
    return m_frame;
}

int TestCppBehaviour::cmemfn1(int a1) const
{
    mioDebugPrint("TestCppBehaviour::cmemfn1(%d) : %d\n", a1, m_frame);
    return m_frame;
}

int TestCppBehaviour::cmemfn2(int a1, int a2) const
{
    mioDebugPrint("TestCppBehaviour::cmemfn2(%d, %d) : %d\n", a1, a2, m_frame);
    return m_frame;
}
