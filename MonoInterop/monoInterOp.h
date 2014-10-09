#include "monoAPI.h"


class mioObject
{
public:
    mioObject(MonoObject *o) : mobj(o) {}
    operator MonoObject*() const { return mobj; }
    operator bool() const { return mobj!=nullptr; }
    const char* getName() const;

    template<class T> T& getValue() { return *(T*)o; }
    template<class T> void setValue(const T &v) { *(T*)o = v; }

    MonoObject *mobj;
};

class mioType
{
    mioType(MonoType *m) : mtype(m) {}
    operator MonoType*() const { return mtype; }
    operator bool() const { return mtype != nullptr; }
    const char* getName() const;

    MonoType *mtype;
};

class mioMethod
{
public:
    mioMethod(MonoMethod *mm) : mmethod(mm) {}
    operator MonoMethod*() const { return mmethod; }
    operator bool() const { return mmethod != nullptr; }
    const char* getName() const;

    mioObject call(mioObject obj, mioObject **args);

    MonoMethod *mmethod;
};


class mioClass
{
public:
    mioClass(MonoClass *mc) : mclass(mc) {}
    operator MonoClass*() const { return mclass; }
    operator bool() const { return mclass != nullptr; }
    const char* getName() const;

    MonoClass *mclass;
};

class mioClassField
{
public:
    mioClassField(MonoClassField *mc) : mfield(mc) {}
    operator MonoClassField*() const { return mfield; }
    operator bool() const { return mfield != nullptr; }
    const char* getName() const;

    MonoClassField *mfield;
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
    mioExport void mioP(mioCurrentClass)##_dtor(mioP(mioCurrentClass) *o)\
    {\
        delete o;\
    }\
    mioAddMethodHelper mioP(mioCurrentClass)##_dtor_(mioS(mioCurrentClass) "::dtor", &mioP(mioCurrentClass)##_ctor);

#define mioExportMethod(MethodName)\
    mioExport MonoObject mioP(mioCurrentClass)##_##MethodName(mioP(mioCurrentClass) *o, ...)\
    {\
        va_list vl;\
        va_start(vl, o);\
        o->MethodName();\
        va_end(vl);\
        return nullptr;\
    }\
    mioAddMethodHelper mioP(mioCurrentClass)##_##MethodName##_(mioS(mioCurrentClass) "::" #MethodName, &mioP(mioCurrentClass)##_##MethodName);

