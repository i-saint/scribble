#include "mioTypes.h"
#include "mioUtils.h"
#include <vector>
#include <algorithm>

void mioAddMethod(const char *name, void *addr)
{
    mono_add_internal_call(name, addr);
}


const char* mioType::getName() const
{
    if (!mtype) { return nullptr; }
    return mono_type_get_name(mtype);
}

mioClass mioType::getClass() const
{
    return mono_type_get_class(mtype);
}



const char* mioField::getName() const
{
    if (!mfield) { return nullptr; }
    return mono_field_get_name(mfield);
}

void mioField::getValueImpl(mioObject obj, void *p) const
{
    if (!mfield) { return; }
    mono_field_get_value(obj, mfield, p);
}

void mioField::setValueImpl(mioObject obj, const void *p)
{
    if (!mfield) { return; }
    mono_field_set_value(obj, mfield, (void*)p);
}


const char* mioProperty::getName() const
{
    if (!mproperty) { return nullptr; }
    return mono_property_get_name(mproperty);
}

mioMethod mioProperty::getGetter() const
{
    if (!mproperty) { return nullptr; }
    return mono_property_get_get_method(mproperty);
}

mioMethod mioProperty::getSetter() const
{
    if (!mproperty) { return nullptr; }
    return mono_property_get_set_method(mproperty);
}



const char* mioMethod::getName() const
{
    if (!mmethod) { return nullptr; }
    return mono_method_get_name(mmethod);
}

mioObject mioMethod::invoke(mioObject obj, void **args)
{
    if (!mmethod) { return nullptr; }
    return mono_runtime_invoke(mmethod, obj, args, nullptr);
}

int mioMethod::getParamCount() const
{
    if (!mmethod) { return -1; }
    MonoMethodSignature *sig = mono_method_signature(mmethod);
    return mono_signature_get_param_count(sig);
}



const char* mioClass::getName() const
{
    if (!mclass) { return nullptr; }
    return mono_class_get_name(mclass);
}

mioType mioClass::getType() const
{
    if (!mclass) { return nullptr; }
    return mono_class_get_type(mclass);
}

mioField mioClass::findField(const char *name) const
{
    if (!mclass) { return nullptr; }
    return mono_class_get_field_from_name(mclass, name);
}

mioProperty mioClass::findProperty(const char *name) const
{
    if (!mclass) { return nullptr; }
    return mono_class_get_property_from_name(mclass, name);
}

mioMethod mioClass::findMethod(const char *name, int num_args) const
{
    if (!mclass) { return nullptr; }
    return mono_class_get_method_from_name(mclass, name, num_args);
}

void mioClass::eachFields(const std::function<void(mioField&)> &f)
{
    MonoClassField *field;
    gpointer iter = nullptr;
    while ((field = mono_class_get_fields(mclass, &iter))) {
        mioField mf = field;
        f(mf);
    }
}

void mioClass::eachProperties(const std::function<void(mioProperty&)> &f)
{
    MonoProperty *prop;
    gpointer iter = nullptr;
    while ((prop = mono_class_get_properties(mclass, &iter))) {
        mioProperty mp = prop;
        f(mp);
    }
}

void mioClass::eachMethods(const std::function<void(mioMethod&)> &f)
{
    MonoMethod *method;
    gpointer iter = nullptr;
    while ((method = mono_class_get_methods(mclass, &iter))) {
        mioMethod mm = method;
        f(mm);
    }
}

void mioClass::eachFieldsUpwards(const std::function<void(mioField&)> &f)
{
    eachFields(f);
    mioClass parent = getParent();
    do {
        parent.eachFields(f);
        parent = parent.getParent();
    } while (parent);
}

void mioClass::eachPropertiesUpwards(const std::function<void(mioProperty&)> &f)
{
    eachProperties(f);
    mioClass parent = getParent();
    do {
        parent.eachProperties(f);
        parent = parent.getParent();
    } while (parent);
}

void mioClass::eachMethodsUpwards(const std::function<void(mioMethod&)> &f)
{
    eachMethods(f);
    mioClass parent = getParent();
    do {
        parent.eachMethods(f);
        parent = parent.getParent();
    } while (parent);
}


mioClass mioClass::getParent() const
{
    return mono_class_get_parent(mclass);
}


mioClass mioObject::getClass() const
{
    return mono_object_get_class(mobj);
}

mioField mioObject::findField(const char *name) const
{
    return getClass().findField(name);
}

mioProperty mioObject::findProperty(const char *name) const
{
    return getClass().findProperty(name);
}

mioMethod mioObject::findMethod(const char *name) const
{
    return getClass().findMethod(name);
}


mioExport void mioInitialize()
{
    mioDebugPrint("mioInitialize()\n");
}