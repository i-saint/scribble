#include "monoInterOp.h"
#include <functional>
#include <cstdio>
#include <cstdarg>

class CppBehaviour
{
public:
    CppBehaviour(MonoObject *o);
    virtual ~CppBehaviour();

    mioObject getCSObject();
    mioField findField(const char *name);
    mioProperty findProperty(const char *name);
    mioMethod findMethod(const char *name);

protected:
    mioObject m_mobj;
};
