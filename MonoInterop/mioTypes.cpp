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

mioMethod mioClass::findMethod(const char *name) const
{
    if (!mclass) { return nullptr; }
    return mono_class_get_method_from_name(mclass, name, -1);
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