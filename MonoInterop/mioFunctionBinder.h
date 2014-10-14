#ifndef mioFunctionBinder_h
#define mioFunctionBinder_h

#include "mioTypes.h"
#include "mioObjectTranslator.h"
#include <type_traits>
#define mioUnRefConst(T) typename std::remove_const<typename std::remove_reference<T>::type>::type

#pragma region mio_function_binder_detail

template<class R>
struct mioFB_Fn0
{
    typedef R (*F)();
    mioObject invoke(F f, va_list args)
    {
        typedef mioUnRefConst(R) RV;
        return mioToCsObject<RV>(f());
    }
};
template<>
struct mioFB_Fn0<void>
{
    typedef void (*F)();
    mioObject invoke(F f, va_list args)
    {
        f();
        return nullptr;
    }
};

template<class R, class C>
struct mioFB_MemFn0
{
    typedef R (C::*F)();
    mioObject invoke(F f, C &o, va_list args)
    {
        typedef mioUnRefConst(R) RV;
        return mioToCsObject<RV>((o.*f)());
    }
};
template<class C>
struct mioFB_MemFn0<void, C>
{
    typedef void (C::*F)();
    mioObject invoke(F f, C &o, va_list args)
    {
        (o.*f)();
        return nullptr;
    }
};

template<class R, class C>
struct mioFB_ConstMemFn0
{
    typedef R (C::*F)() const;
    mioObject invoke(F f, const C &o, va_list args)
    {
        typedef mioUnRefConst(R) RV;
        return mioToCsObject<RV>((o.*f)());
    }
};
template<class C>
struct mioFB_ConstMemFn0<void, C>
{
    typedef void (C::*F)() const;
    mioObject invoke(F f, const C &o, va_list args)
    {
        (o.*f)();
        return nullptr;
    }
};



template<class R, class A0>
struct mioFB_Fn1
{
    typedef R (*F)(A0);
    mioObject invoke(F f, va_list args)
    {
        typedef mioUnRefConst(R) RV;
        typedef mioUnRefConst(A0) A0V;
        mioObject a0 = va_arg(args, MonoObject*);
        return mioToCsObject<RV>(f(mioToCppObject<A0V>(a0)));
    }
};
template<class A0>
struct mioFB_Fn1 < void, A0 >
{
    typedef void (*F)(A0);
    mioObject invoke(F f, va_list args)
    {
        typedef mioUnRefConst(A0) A0V;
        mioObject a0 = va_arg(args, MonoObject*);
        f(mioToCppObject<A0V>(a0));
        return nullptr;
    }
};

template<class R, class C, class A0>
struct mioFB_MemFn1
{
    typedef R (C::*F)(A0);
    mioObject invoke(F f, C &o, va_list args)
    {
        typedef mioUnRefConst(R) RV;
        typedef mioUnRefConst(A0) A0V;
        mioObject a0 = va_arg(args, MonoObject*);
        return mioToCsObject<RV>((o.*f)(mioToCppObject<A0V>(a0)));
    }
};
template<class C, class A0>
struct mioFB_MemFn1 < void, C, A0 >
{
    typedef void (C::*F)(A0);
    mioObject invoke(F f, C &o, va_list args)
    {
        typedef mioUnRefConst(A0) A0V;
        mioObject a0 = va_arg(args, MonoObject*);
        (o.*f)(mioToCppObject<A0V>(a0));
        return nullptr;
    }
};

template<class R, class C, class A0>
struct mioFB_ConstMemFn1
{
    typedef R (C::*F)(A0) const;
    mioObject invoke(F f, const C &o, va_list args)
    {
        typedef mioUnRefConst(R) RV;
        typedef mioUnRefConst(A0) A0V;
        mioObject a0 = va_arg(args, MonoObject*);
        return mioToCsObject<RV>((o.*f)(mioToCppObject<A0V>(a0)));
    }
};
template<class C, class A0>
struct mioFB_ConstMemFn1 < void, C, A0 >
{
    typedef void (C::*F)(A0) const;
    mioObject invoke(F f, const C &o, va_list args)
    {
        typedef mioUnRefConst(A0) A0V;
        mioObject a0 = va_arg(args, MonoObject*);
        (o.*f)(mioToCppObject<A0V>(a0));
        return nullptr;
    }
};



template<class R, class A0, class A1>
struct mioFB_Fn2
{
    typedef R(*F)(A0, A1);
    mioObject invoke(F f, va_list args)
    {
        typedef mioUnRefConst(R) RV;
        typedef mioUnRefConst(A0) A0V;
        typedef mioUnRefConst(A1) A1V;
        mioObject a0 = va_arg(args, MonoObject*);
        mioObject a1 = va_arg(args, MonoObject*);
        return mioToCsObject<RV>(f(mioToCppObject<A0V>(a0), mioToCppObject<A1V>(a1)));
    }
};
template<class A0, class A1>
struct mioFB_Fn2 < void, A0, A1 >
{
    typedef void(*F)(A0, A1);
    mioObject invoke(F f, va_list args)
    {
        typedef mioUnRefConst(A0) A0V;
        typedef mioUnRefConst(A1) A1V;
        mioObject a0 = va_arg(args, MonoObject*);
        mioObject a1 = va_arg(args, MonoObject*);
        f(mioToCppObject<A0V>(a0), mioToCppObject<A1V>(a1));
        return nullptr;
    }
};

template<class R, class C, class A0, class A1>
struct mioFB_MemFn2
{
    typedef R(C::*F)(A0, A1);
    mioObject invoke(F f, C &o, va_list args)
    {
        typedef mioUnRefConst(R) RV;
        typedef mioUnRefConst(A0) A0V;
        typedef mioUnRefConst(A1) A1V;
        mioObject a0 = va_arg(args, MonoObject*);
        mioObject a1 = va_arg(args, MonoObject*);
        return mioToCsObject<RV>((o.*f)(mioToCppObject<A0V>(a0), mioToCppObject<A1V>(a1)));
    }
};
template<class C, class A0, class A1>
struct mioFB_MemFn2 < void, C, A0, A1 >
{
    typedef void (C::*F)(A0, A1);
    mioObject invoke(F f, C &o, va_list args)
    {
        typedef mioUnRefConst(A0) A0V;
        typedef mioUnRefConst(A1) A1V;
        mioObject a0 = va_arg(args, MonoObject*);
        mioObject a1 = va_arg(args, MonoObject*);
        (o.*f)(mioToCppObject<A0V>(a0), mioToCppObject<A1V>(a1));
        return nullptr;
    }
};

template<class R, class C, class A0, class A1>
struct mioFB_ConstMemFn2
{
    typedef R(C::*F)(A0, A1) const;
    mioObject invoke(F f, const C &o, va_list args)
    {
        typedef mioUnRefConst(R) RV;
        typedef mioUnRefConst(A0) A0V;
        typedef mioUnRefConst(A1) A1V;
        mioObject a0 = va_arg(args, MonoObject*);
        mioObject a1 = va_arg(args, MonoObject*);
        return mioToCsObject<RV>((o.*f)(mioToCppObject<A0V>(a0), mioToCppObject<A1V>(a1)));
    }
};
template<class C, class A0, class A1>
struct mioFB_ConstMemFn2 < void, C, A0, A1 >
{
    typedef void (C::*F)(A0, A1) const;
    mioObject invoke(F f, const C &o, va_list args)
    {
        typedef mioUnRefConst(A0) A0V;
        typedef mioUnRefConst(A1) A1V;
        mioObject a0 = va_arg(args, MonoObject*);
        mioObject a1 = va_arg(args, MonoObject*);
        (o.*f)(mioToCppObject<A0V>(a0), mioToCppObject<A1V>(a1));
        return nullptr;
    }
};



template<class R, class A0, class A1, class A2>
struct mioFB_Fn3
{
    typedef R(*F)(A0, A1, A2);
    mioObject invoke(F f, va_list args)
    {
        typedef mioUnRefConst(R) RV;
        typedef mioUnRefConst(A0) A0V;
        typedef mioUnRefConst(A1) A1V;
        typedef mioUnRefConst(A2) A2V;
        mioObject a0 = va_arg(args, MonoObject*);
        mioObject a1 = va_arg(args, MonoObject*);
        mioObject a2 = va_arg(args, MonoObject*);
        return mioToCsObject<RV>(f(mioToCppObject<A0V>(a0), mioToCppObject<A1V>(a1), mioToCppObject<A2V>(a2)));
    }
};
template<class A0, class A1, class A2>
struct mioFB_Fn3 < void, A0, A1, A2 >
{
    typedef void(*F)(A0, A1, A2);
    mioObject invoke(F f, va_list args)
    {
        typedef mioUnRefConst(A0) A0V;
        typedef mioUnRefConst(A1) A1V;
        typedef mioUnRefConst(A2) A2V;
        mioObject a0 = va_arg(args, MonoObject*);
        mioObject a1 = va_arg(args, MonoObject*);
        mioObject a2 = va_arg(args, MonoObject*);
        f(mioToCppObject<A0V>(a0), mioToCppObject<A1V>(a1), mioToCppObject<A2V>(a2));
        return nullptr;
    }
};

template<class R, class C, class A0, class A1, class A2>
struct mioFB_MemFn3
{
    typedef R(C::*F)(A0, A1, A2);
    mioObject invoke(F f, C &o, va_list args)
    {
        typedef mioUnRefConst(R) RV;
        typedef mioUnRefConst(A0) A0V;
        typedef mioUnRefConst(A1) A1V;
        typedef mioUnRefConst(A2) A2V;
        mioObject a0 = va_arg(args, MonoObject*);
        mioObject a1 = va_arg(args, MonoObject*);
        mioObject a2 = va_arg(args, MonoObject*);
        return mioToCsObject<RV>((o.*f)(mioToCppObject<A0V>(a0), mioToCppObject<A1V>(a1), mioToCppObject<A2V>(a2)));
    }
};
template<class C, class A0, class A1, class A2>
struct mioFB_MemFn3 < void, C, A0, A1, A2 >
{
    typedef void (C::*F)(A0, A1, A2);
    mioObject invoke(F f, C &o, va_list args)
    {
        typedef mioUnRefConst(A0) A0V;
        typedef mioUnRefConst(A1) A1V;
        typedef mioUnRefConst(A2) A2V;
        mioObject a0 = va_arg(args, MonoObject*);
        mioObject a1 = va_arg(args, MonoObject*);
        mioObject a2 = va_arg(args, MonoObject*);
        (o.*f)(mioToCppObject<A0V>(a0), mioToCppObject<A1V>(a1), mioToCppObject<A2V>(a2));
        return nullptr;
    }
};

template<class R, class C, class A0, class A1, class A2>
struct mioFB_ConstMemFn3
{
    typedef R(C::*F)(A0, A1, A2) const;
    mioObject invoke(F f, const C &o, va_list args)
    {
        typedef mioUnRefConst(R) RV;
        typedef mioUnRefConst(A0) A0V;
        typedef mioUnRefConst(A1) A1V;
        typedef mioUnRefConst(A2) A2V;
        mioObject a0 = va_arg(args, MonoObject*);
        mioObject a1 = va_arg(args, MonoObject*);
        mioObject a2 = va_arg(args, MonoObject*);
        return mioToCsObject<RV>((o.*f)(mioToCppObject<A0V>(a0), mioToCppObject<A1V>(a1), mioToCppObject<A2V>(a2)));
    }
};
template<class C, class A0, class A1, class A2>
struct mioFB_ConstMemFn3 < void, C, A0, A1, A2 >
{
    typedef void (C::*F)(A0, A1, A2) const;
    mioObject invoke(F f, const C &o, va_list args)
    {
        typedef mioUnRefConst(A0) A0V;
        typedef mioUnRefConst(A1) A1V;
        typedef mioUnRefConst(A2) A2V;
        mioObject a0 = va_arg(args, MonoObject*);
        mioObject a1 = va_arg(args, MonoObject*);
        mioObject a2 = va_arg(args, MonoObject*);
        (o.*f)(mioToCppObject<A0V>(a0), mioToCppObject<A1V>(a1), mioToCppObject<A2V>(a2));
        return nullptr;
    }
};



template<class R, class A0, class A1, class A2, class A3>
struct mioFB_Fn4
{
    typedef R(*F)(A0, A1, A2, A3);
    mioObject invoke(F f, va_list args)
    {
        typedef mioUnRefConst(R) RV;
        typedef mioUnRefConst(A0) A0V;
        typedef mioUnRefConst(A1) A1V;
        typedef mioUnRefConst(A2) A2V;
        typedef mioUnRefConst(A3) A3V;
        mioObject a0 = va_arg(args, MonoObject*);
        mioObject a1 = va_arg(args, MonoObject*);
        mioObject a2 = va_arg(args, MonoObject*);
        mioObject a3 = va_arg(args, MonoObject*);
        return mioToCsObject<RV>(f(mioToCppObject<A0V>(a0), mioToCppObject<A1V>(a1), mioToCppObject<A2V>(a2), mioToCppObject<A3V>(a3)));
    }
};
template<class A0, class A1, class A2, class A3>
struct mioFB_Fn4 < void, A0, A1, A2, A3 >
{
    typedef void(*F)(A0, A1, A2, A3);
    mioObject invoke(F f, va_list args)
    {
        typedef mioUnRefConst(A0) A0V;
        typedef mioUnRefConst(A1) A1V;
        typedef mioUnRefConst(A2) A2V;
        typedef mioUnRefConst(A3) A3V;
        mioObject a0 = va_arg(args, MonoObject*);
        mioObject a1 = va_arg(args, MonoObject*);
        mioObject a2 = va_arg(args, MonoObject*);
        mioObject a3 = va_arg(args, MonoObject*);
        f(mioToCppObject<A0V>(a0), mioToCppObject<A1V>(a1), mioToCppObject<A2V>(a2), mioToCppObject<A3V>(a3));
        return nullptr;
    }
};

template<class R, class C, class A0, class A1, class A2, class A3>
struct mioFB_MemFn4
{
    typedef R(C::*F)(A0, A1, A2, A3);
    mioObject invoke(F f, C &o, va_list args)
    {
        typedef mioUnRefConst(R) RV;
        typedef mioUnRefConst(A0) A0V;
        typedef mioUnRefConst(A1) A1V;
        typedef mioUnRefConst(A2) A2V;
        typedef mioUnRefConst(A3) A3V;
        mioObject a0 = va_arg(args, MonoObject*);
        mioObject a1 = va_arg(args, MonoObject*);
        mioObject a2 = va_arg(args, MonoObject*);
        mioObject a3 = va_arg(args, MonoObject*);
        return mioToCsObject<RV>((o.*f)(mioToCppObject<A0V>(a0), mioToCppObject<A1V>(a1), mioToCppObject<A2V>(a2), mioToCppObject<A3V>(a3)));
    }
};
template<class C, class A0, class A1, class A2, class A3>
struct mioFB_MemFn4 < void, C, A0, A1, A2, A3 >
{
    typedef void (C::*F)(A0, A1, A2, A3);
    mioObject invoke(F f, C &o, va_list args)
    {
        typedef mioUnRefConst(A0) A0V;
        typedef mioUnRefConst(A1) A1V;
        typedef mioUnRefConst(A2) A2V;
        typedef mioUnRefConst(A3) A3V;
        mioObject a0 = va_arg(args, MonoObject*);
        mioObject a1 = va_arg(args, MonoObject*);
        mioObject a2 = va_arg(args, MonoObject*);
        mioObject a3 = va_arg(args, MonoObject*);
        (o.*f)(mioToCppObject<A0V>(a0), mioToCppObject<A1V>(a1), mioToCppObject<A2V>(a2), mioToCppObject<A3V>(a3));
        return nullptr;
    }
};

template<class R, class C, class A0, class A1, class A2, class A3>
struct mioFB_ConstMemFn4
{
    typedef R(C::*F)(A0, A1, A2, A3) const;
    mioObject invoke(F f, const C &o, va_list args)
    {
        typedef mioUnRefConst(R) RV;
        typedef mioUnRefConst(A0) A0V;
        typedef mioUnRefConst(A1) A1V;
        typedef mioUnRefConst(A2) A2V;
        typedef mioUnRefConst(A3) A3V;
        mioObject a0 = va_arg(args, MonoObject*);
        mioObject a1 = va_arg(args, MonoObject*);
        mioObject a2 = va_arg(args, MonoObject*);
        mioObject a3 = va_arg(args, MonoObject*);
        return mioToCsObject<RV>((o.*f)(mioToCppObject<A0V>(a0), mioToCppObject<A1V>(a1), mioToCppObject<A2V>(a2), mioToCppObject<A3V>(a3)));
    }
};
template<class C, class A0, class A1, class A2, class A3>
struct mioFB_ConstMemFn4 < void, C, A0, A1, A2, A3 >
{
    typedef void (C::*F)(A0, A1, A2, A3) const;
    mioObject invoke(F f, const C &o, va_list args)
    {
        typedef mioUnRefConst(A0) A0V;
        typedef mioUnRefConst(A1) A1V;
        typedef mioUnRefConst(A2) A2V;
        typedef mioUnRefConst(A3) A3V;
        mioObject a0 = va_arg(args, MonoObject*);
        mioObject a1 = va_arg(args, MonoObject*);
        mioObject a2 = va_arg(args, MonoObject*);
        mioObject a3 = va_arg(args, MonoObject*);
        (o.*f)(mioToCppObject<A0V>(a0), mioToCppObject<A1V>(a1), mioToCppObject<A2V>(a2), mioToCppObject<A3V>(a3));
        return nullptr;
    }
};





template<class R, class C>
inline mioObject mioCSCall(R(*f)(), C *o, va_list args)
{
    return mioFB_Fn0<R>().invoke(f, args);
}
template<class R, class C>
inline mioObject mioCSCall(R(C::*f)(), C *o, va_list args)
{
    return mioFB_MemFn0<R, C>().invoke(f, *o, args);
}
template<class R, class C>
inline mioObject mioCSCall(R(C::*f)() const, const C o, va_list args)
{
    return mioFB_ConstMemFn0<R, C>().invoke(f, *o, args);
}


template<class R, class C, class A0>
inline mioObject mioCSCall(R(*f)(A0), C *o, va_list args)
{
    return mioFB_Fn1<R, A0>().invoke(f, args);
}
template<class R, class C, class A0>
inline mioObject mioCSCall(R(C::*f)(A0), C *o, va_list args)
{
    return mioFB_MemFn1<R, C, A0>().invoke(f, *o, args);
}
template<class R, class C, class A0>
inline mioObject mioCSCall(R(C::*f)(A0) const, const C *o, va_list args)
{
    return mioFB_ConstMemFn1<R, C, A0>().invoke(f, *o, args);
}


template<class R, class C, class A0, class A1>
inline mioObject mioCSCall(R(*f)(A0, A1), C *o, va_list args)
{
    return mioFB_Fn2<R, A0, A1>().invoke(f, args);
}
template<class R, class C, class A0, class A1>
inline mioObject mioCSCall(R(C::*f)(A0, A1), C *o, va_list args)
{
    return mioFB_MemFn2<R, C, A0, A1>().invoke(f, *o, args);
}
template<class R, class C, class A0, class A1>
inline mioObject mioCSCall(R(C::*f)(A0, A1) const, const C *o, va_list args)
{
    return mioFB_ConstMemFn2<R, C, A0, A1>().invoke(f, *o, args);
}


template<class R, class C, class A0, class A1, class A2>
inline mioObject mioCSCall(R(*f)(A0, A1, A2), C *o, va_list args)
{
    return mioFB_Fn3<R, A0, A1, A2>().invoke(f, args);
}
template<class R, class C, class A0, class A1, class A2>
inline mioObject mioCSCall(R(C::*f)(A0, A1, A2), C *o, va_list args)
{
    return mioFB_MemFn3<R, C, A0, A1, A2>().invoke(f, *o, args);
}
template<class R, class C, class A0, class A1, class A2>
inline mioObject mioCSCall(R(C::*f)(A0, A1, A2) const, const C *o, va_list args)
{
    return mioFB_ConstMemFn3<R, C, A0, A1, A2>().invoke(f, *o, args);
}


template<class R, class C, class A0, class A1, class A2, class A3>
inline mioObject mioCSCall(R(*f)(A0, A1, A2, A3), C *o, va_list args)
{
    return mioFB_Fn4<R, A0, A1, A2, A3>().invoke(f, args);
}
template<class R, class C, class A0, class A1, class A2, class A3>
inline mioObject mioCSCall(R(C::*f)(A0, A1, A2, A3), C *o, va_list args)
{
    return mioFB_MemFn4<R, C, A0, A1, A2, A3>().invoke(f, *o, args);
}
template<class R, class C, class A0, class A1, class A2, class A3>
inline mioObject mioCSCall(R(C::*f)(A0, A1, A2, A3) const, const C *o, va_list args)
{
    return mioFB_ConstMemFn4<R, C, A0, A1, A2, A3>().invoke(f, *o, args);
}



#pragma endregion


void mioAddMethod(const char *name, void *addr);
struct mioAddMethodHelper
{
    mioAddMethodHelper(const char *name, void *addr) { mioAddMethod(name, addr); }
};


template<class T>
inline mioField mioGetCppThisField(mioObject o)
{
    static mioField s_field = o.findField("this_cpp");
    return s_field;
}
template<class T>
inline T* mioGetCppThis(mioObject o)
{
    T *ret = nullptr;
    mioGetCppThisField<T>(o).getValue<T*>(o, ret);
    return ret;
}
template<class T>
inline void mioSetCppThis(mioObject o, T *_this)
{
    mioGetCppThisField<T>(o).setValue<T*>(o, _this);
}





template<class T>
inline T* mioGetFieldValuePtr(mioObject parent, const char *field_name)
{
    int offset = parent.findField(field_name).getOffset();
    return (T*)((size_t&)parent + offset);
}

template<class T>
inline T* mioGetFieldValuePtr(mioObject parent, mioField field)
{
    int offset = field.getOffset();
    return (T*)((size_t&)parent + offset);
}



#define mioS2(...) #__VA_ARGS__
#define mioS(...) mioS2(__VA_ARGS__)
#define mioP(...) __VA_ARGS__

#define mioExportClass()\
    mioExport void mioP(mioCurrentClass)##_ctor(MonoObject *o)\
    {\
        typedef mioP(mioCurrentClass) this_t;\
        mioSetCppThis<this_t>(o, new this_t(o));\
    }\
    mioExport void mioP(mioCurrentClass)##_dtor(MonoObject *o)\
    {\
        typedef mioP(mioCurrentClass) this_t;\
        delete mioGetCppThis<this_t>(o);\
    }\
    mioAddMethodHelper mioP(mioCurrentClass)##_ctor_(mioS(mioCurrentClass) "::ctor", &mioP(mioCurrentClass)##_ctor);\
    mioAddMethodHelper mioP(mioCurrentClass)##_dtor_(mioS(mioCurrentClass) "::dtor", &mioP(mioCurrentClass)##_dtor);

#define mioExportMethod(MethodName)\
    mioExport MonoObject mioP(mioCurrentClass)##_##MethodName(MonoObject *o, ...)\
    {\
        typedef mioP(mioCurrentClass) this_t;\
        mioObject ret = nullptr;\
        va_list args;\
        va_start(args, o);\
        ret = mioCSCall(&this_t::##MethodName, mioGetCppThis<this_t>(o), args);\
        va_end(args);\
        return ret;\
    }\
    mioAddMethodHelper mioP(mioCurrentClass)##_##MethodName##_(mioS(mioCurrentClass) "::" #MethodName, &mioP(mioCurrentClass)##_##MethodName);


#endif // mioFunctionBinder_h
