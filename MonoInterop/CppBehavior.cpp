#include "CppBehavior.h"

//MonoAssembly *as = mono_domain_assembly_open(mono_domain_get(), "Assembly-CSharp.dll");
//MonoImage *img = mono_assembly_get_image(as);

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
    if (!m_mobj) { return nullptr;  }

    MonoMethod *ret = nullptr;
    MonoDomain *domain = mono_object_get_domain(m_mobj);
    MonoClass *c = mono_object_get_class(m_mobj);
    if (domain && c) {
        ret = mono_class_get_method_from_name(c, name, -1);
    }

    return mioMethod(ret);
}

void CppBehavior::eachField(const std::function<void(mioObject&)> &f)
{

}

void CppBehavior::eachMethod(const std::function<void(mioMethod&)> &f)
{
    void* iter;
    MonoMethod *method;
    MonoClass *c = mono_object_get_class(m_mobj);
    while (method = mono_class_get_methods(c, &iter)) {
        f(mioMethod(method));
    }
}

