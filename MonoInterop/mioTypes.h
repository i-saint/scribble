#ifndef mioTypes_h
#define mioTypes_h

#include "monoAPI.h"
#include <functional>
#include <cstdio>
#include <cstdarg>

#define mioExport extern "C" __declspec(dllexport)


class mioObject;
class mioType;
class mioClass;
class mioMethod;
class mioField;
class mioProperty;


class mioType
{
public:
    mioType(MonoType *m) : mtype(m) {}
    operator MonoType*() const { return mtype; }
    operator bool() const { return mtype != nullptr; }
    const char* getName() const;
    mioClass getClass() const;

    MonoType *mtype;
};

class mioField
{
public:
    mioField(MonoClassField *mc) : mfield(mc) {}
    operator MonoClassField*() const { return mfield; }
    operator bool() const { return mfield != nullptr; }
    const char* getName() const;

    mioType getType() const;
    template<class T> void getValue(mioObject obj, T &o) const { getValueImpl(obj, &o); }
    template<class T> void setValue(mioObject obj, const T &o) { setValueImpl(obj, &o); }

    void getValueImpl(mioObject obj, void *p) const;
    void setValueImpl(mioObject obj, const void *p);

    MonoClassField *mfield;
};

class mioProperty
{
public:
    mioProperty(MonoProperty *mc) : mproperty(mc) {}
    operator MonoProperty*() const { return mproperty; }
    operator bool() const { return mproperty != nullptr; }
    const char* getName() const;

    mioMethod getGetter() const;
    mioMethod getSetter() const;

    MonoProperty *mproperty;
};

class mioMethod
{
public:
    mioMethod(MonoMethod *mm) : mmethod(mm) {}
    operator MonoMethod*() const { return mmethod; }
    operator bool() const { return mmethod != nullptr; }
    const char* getName() const;

    int getParamCount() const;
    mioType getReturnType() const;
    
    mioObject invoke(mioObject obj, void **args);

    MonoMethod *mmethod;
};

class mioClass
{
public:
    mioClass(MonoClass *mc) : mclass(mc) {}
    operator MonoClass*() const { return mclass; }
    operator bool() const { return mclass != nullptr; }
    const char* getName() const;
    mioType     getType() const;
    mioClass    getParent() const;

    mioField    findField(const char *name) const;
    mioProperty findProperty(const char *name) const;
    mioMethod   findMethod(const char *name, int num_args=-1) const;
    void eachFields(const std::function<void(mioField&)> &f);
    void eachProperties(const std::function<void(mioProperty&)> &f);
    void eachMethods(const std::function<void(mioMethod&)> &f);

    // include parent classes
    mioField    findFieldUpwards(const char *name) const;
    mioProperty findPropertyUpwards(const char *name) const;
    mioMethod   findMethodUpwards(const char *name, int num_args = -1) const;
    void eachFieldsUpwards(const std::function<void(mioField&)> &f);
    void eachPropertiesUpwards(const std::function<void(mioProperty&)> &f);
    void eachMethodsUpwards(const std::function<void(mioMethod&)> &f);

    MonoClass *mclass;
};

class mioObject
{
public:
    mioObject(MonoObject *o) : mobj(o) {}
    operator MonoObject*() const { return mobj; }
    operator bool() const { return mobj != nullptr; }

    mioClass    getClass() const;
    mioField    findField(const char *name) const;
    mioProperty findProperty(const char *name) const;
    mioMethod   findMethod(const char *name) const;

    MonoObject *mobj;
};


#endif // mioTypes_h
