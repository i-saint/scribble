#include "monoInterOp.h"
#include <functional>
#include <cstdio>
#include <cstdarg>

class CppBehavior
{
public:
    CppBehavior(MonoObject *o);
    virtual ~CppBehavior();

    mioObject findField(const char *name);
    mioMethod findMethod(const char *name);
    void eachField(std::function<void(mioObject&)>);
    void eachMethod(std::function<void(mioMethod&)>);

protected:
    mioObject m_mobj;
};
