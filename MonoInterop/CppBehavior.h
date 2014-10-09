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
    void eachField(const std::function<void(mioObject&)> &f);
    void eachMethod(const std::function<void(mioMethod&)> &f);

protected:
    mioObject m_mobj;
};
