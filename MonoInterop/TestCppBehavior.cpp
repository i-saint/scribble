#include "CppBehavior.h"

class TestCppBehavior : public CppBehavior
{
typedef CppBehavior super;
public:
    TestCppBehavior(MonoObject *o);
    virtual ~TestCppBehavior();
    void Start();
    void Update();

    int FuncWithParams(int param);
private:
    int m_data;
};



#define mioCurrentClass TestCppBehavior
mioExportCtor()
mioExportDtor()
mioExportMethod(Start)
mioExportMethod(Update)
//mioExportMethod(FuncWithParams)
#undef mioCurrentClass TestCppBehavior



TestCppBehavior::TestCppBehavior(MonoObject *o)
: super(o)
{
    printf("TestCppObject::TestCppObject()\n");
}

TestCppBehavior::~TestCppBehavior()
{
    printf("TestCppObject:~TestCppObject()\n");
}

void TestCppBehavior::Start()
{
    printf("TestCppObject::Start()\n");
}

void TestCppBehavior::Update()
{
    printf("TestCppObject::Update()\n");
}

int TestCppBehavior::FuncWithParams(int param)
{
    printf("%d TestCppObject::FuncWithParams(%d)\n", m_data, param);
    return m_data;
}
