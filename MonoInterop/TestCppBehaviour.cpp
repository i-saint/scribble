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
    int memfn3(int a1, int a2, int a3);
    int memfn4(int a1, int a2, int a3, int a4);
    int cmemfn1(int a1) const;
    int cmemfn2(int a1, int a2) const;
    int cmemfn3(int a1, int a2, int a3) const;
    int cmemfn4(int a1, int a2, int a3, int a4) const;
    static int smemfn1(int a1);
    static int smemfn2(int a1, int a2);
    static int smemfn3(int a1, int a2, int a3);
    static int smemfn4(int a1, int a2, int a3, int a4);
private:
    int m_frame;
};



#define mioCurrentClass TestCppBehaviour
mioExportClass()
mioExportMethod(start)
mioExportMethod(update)
mioExportMethod(memfn1)
mioExportMethod(memfn2)
mioExportMethod(memfn3)
mioExportMethod(memfn4)
mioExportMethod(cmemfn1)
mioExportMethod(cmemfn2)
mioExportMethod(cmemfn3)
mioExportMethod(cmemfn4)
mioExportMethod(smemfn1)
mioExportMethod(smemfn2)
mioExportMethod(smemfn3)
mioExportMethod(smemfn4)
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

int TestCppBehaviour::memfn3(int a1, int a2, int a3)
{
    mioDebugPrint("TestCppBehaviour::memfn3(%d, %d, %d) : %d\n", a1, a2, a3, m_frame);
    return m_frame;
}

int TestCppBehaviour::memfn4(int a1, int a2, int a3, int a4)
{
    mioDebugPrint("TestCppBehaviour::memfn4(%d, %d, %d, %d) : %d\n", a1, a2, a3, a4, m_frame);
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

int TestCppBehaviour::cmemfn3(int a1, int a2, int a3) const
{
    mioDebugPrint("TestCppBehaviour::cmemfn3(%d, %d, %d) : %d\n", a1, a2, a3, m_frame);
    return m_frame;
}

int TestCppBehaviour::cmemfn4(int a1, int a2, int a3, int a4) const
{
    mioDebugPrint("TestCppBehaviour::cmemfn4(%d, %d, %d, %d) : %d\n", a1, a2, a3, a4, m_frame);
    return m_frame;
}


int TestCppBehaviour::smemfn1(int a1)
{
    mioDebugPrint("TestCppBehaviour::smemfn1(%d) : %d\n", a1, 1);
    return 1;
}

int TestCppBehaviour::smemfn2(int a1, int a2)
{
    mioDebugPrint("TestCppBehaviour::smemfn2(%d, %d) : %d\n", a1, a2, 2);
    return 2;
}

int TestCppBehaviour::smemfn3(int a1, int a2, int a3)
{
    mioDebugPrint("TestCppBehaviour::smemfn3(%d, %d, %d) : %d\n", a1, a2, a3, 3);
    return 3;
}

int TestCppBehaviour::smemfn4(int a1, int a2, int a3, int a4)
{
    mioDebugPrint("TestCppBehaviour::smemfn4(%d, %d, %d, %d) : %d\n", a1, a2, a3, a4, 4);
    return 4;
}
