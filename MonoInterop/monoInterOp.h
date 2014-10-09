#include "monoAPI.h"

#ifdef WIN32
#include <windows.h>
#define mioDebugPrint OutputDebugStringA
#else
#define mioDebugPrint printf
#endif // WIN32


class mioObject;
class mioType;
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

    mioField    findField(const char *name) const;
    mioProperty findProperty(const char *name) const;
    mioMethod   findMethod(const char *name) const;

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



void mioAddMethod(const char *name, void *addr);
struct mioAddMethodHelper
{
    mioAddMethodHelper(const char *name, void *addr) { mioAddMethod(name, addr); }
};

#define mioExport extern "C" __declspec(dllexport)
#define mioS2(...) #__VA_ARGS__
#define mioS(...) mioS2(__VA_ARGS__)
#define mioP(...) __VA_ARGS__


#define mioExportCtor()\
    mioExport MonoObject mioP(mioCurrentClass)##_ctor(MonoObject *o)\
    {\
        return new mioP(mioCurrentClass)(o);\
    }\
    mioAddMethodHelper mioP(mioCurrentClass)##_ctor_(mioS(mioCurrentClass) "::ctor", &mioP(mioCurrentClass)##_ctor);


#define mioExportDtor()\
    mioExport void mioP(mioCurrentClass)##_dtor(MonoObject *mo, mioP(mioCurrentClass) *o)\
    {\
        delete o;\
    }\
    mioAddMethodHelper mioP(mioCurrentClass)##_dtor_(mioS(mioCurrentClass) "::dtor", &mioP(mioCurrentClass)##_dtor);

#define mioExportMethod(MethodName)\
    mioExport MonoObject mioP(mioCurrentClass)##_##MethodName(MonoObject *o, ...)\
    {\
        mioP(mioCurrentClass) *inst = nullptr;\
        mioObject mo(o);\
        mo.findField("cppobj").getValue(mo, inst);\
        if(inst) {\
            va_list vl;\
            va_start(vl, o);\
            inst->MethodName();\
            va_end(vl);\
        }\
        return nullptr;\
    }\
    mioAddMethodHelper mioP(mioCurrentClass)##_##MethodName##_(mioS(mioCurrentClass) "::" #MethodName, &mioP(mioCurrentClass)##_##MethodName);

