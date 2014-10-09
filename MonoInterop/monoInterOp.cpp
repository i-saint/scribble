#include "monoInterOp.h"
#include <vector>
#include <algorithm>

void mioAddMethod(const char *name, void *addr)
{
    mono_add_internal_call(name, addr);
}

const char* mioType::getName() const
{
    return mono_type_get_name(mtype);
}

const char* mioMethod::getName() const
{
    return mono_method_get_name(mmethod);
}

const char* mioClass::getName() const
{
    return mono_class_get_name(mclass);
}

const char* mioClassField::getName() const
{
    return mono_field_get_name(mfield);
}
