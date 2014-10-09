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

mioObject mioMethod::call(mioObject obj, void **args)
{
    return mono_runtime_invoke(mmethod, obj, args, nullptr);
}

int mioMethod::getParamCount() const
{
    if (mmethod) {
        MonoMethodSignature *sig = mono_method_signature(mmethod);
        return mono_signature_get_param_count(sig);
    }
    return -1;
}


const char* mioClass::getName() const
{
    return mono_class_get_name(mclass);
}


const char* mioClassField::getName() const
{
    return mono_field_get_name(mfield);
}

mioExport void mioInitialize()
{
    mioDebugPrint("mioInitialize() mod\n");
}