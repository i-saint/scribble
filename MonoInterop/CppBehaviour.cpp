#include "CppBehaviour.h"

//MonoAssembly *as = mono_domain_assembly_open(mono_domain_get(), "Assembly-CSharp.dll");
//MonoImage *img = mono_assembly_get_image(as);

CppBehaviour::CppBehaviour(MonoObject *o)
    : m_mobj(o)
{

}

CppBehaviour::~CppBehaviour()
{

}

mioObject CppBehaviour::getCSObject()
{
    return m_mobj;
}

mioField CppBehaviour::findField(const char *name)
{
    return m_mobj.findField(name);
}

mioProperty CppBehaviour::findProperty(const char *name)
{
    return m_mobj.findProperty(name);
}

mioMethod CppBehaviour::findMethod(const char *name)
{
    return m_mobj.findMethod(name);
}
