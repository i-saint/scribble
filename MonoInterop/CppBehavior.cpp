#include "CppBehavior.h"

CppBehavior::CppBehavior(MonoObject *o)
    : m_mobj(o)
{

}

CppBehavior::~CppBehavior()
{

}

mioObject CppBehavior::findField(const char *name)
{
    return mioObject(nullptr);
}

mioMethod CppBehavior::findMethod(const char *name)
{
    return mioMethod(nullptr);
}

void CppBehavior::eachField(std::function<void(mioObject&)>)
{

}

void CppBehavior::eachMethod(std::function<void(mioMethod&)>)
{

}

