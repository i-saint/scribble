#include "CppBehaviour.h"

//MonoAssembly *as = mono_domain_assembly_open(mono_domain_get(), "Assembly-CSharp.dll");
//MonoImage *img = mono_assembly_get_image(as);

CppBehaviour::CppBehaviour(MonoObject *o)
    : this_cs(o)
{

}

CppBehaviour::~CppBehaviour()
{

}

mioObject CppBehaviour::getCSObject()
{
    return this_cs;
}

mioField CppBehaviour::findField(const char *name)
{
    return this_cs.findField(name);
}

mioProperty CppBehaviour::findProperty(const char *name)
{
    return this_cs.findProperty(name);
}

mioMethod CppBehaviour::findMethod(const char *name)
{
    return this_cs.findMethod(name);
}
