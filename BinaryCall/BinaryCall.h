#ifndef BinaryCall_h
#define BinaryCall_h
#include <type_traits>
#include "ArgHolder.h"


template<class R>
struct BC_Fn0
{
    typedef R (*F)();
    void RefAsValue(F f, void *r, const void *a)
    {
        typedef ValueHolder<R> RT;
        if(r) { *(RT*)r=f(); }
        else  {         f(); }
    }
    void RefAsPtr(F f, void *r, const void *a)
    {
        typedef ArgHolder<R> RT;
        if(r) { *(RT*)r=f(); }
        else  {         f(); }
    }
};
template<>
struct BC_Fn0<void>
{
    typedef void (*F)();
    void RefAsValue(F f, void *r, const void *a)
    {
        f();
    }
    typedef void (*F)();
    void RefAsPtr(F f, void *r, const void *a)
    {
        f();
    }
};

template<class R, class C>
struct BC_MemFn0
{
    typedef R (C::*F)();
    void RefAsValue(F f, C &o, void *r, const void *a)
    {
        typedef ValueHolder<R> RT;
        if(r) { *(RT*)r=(o.*f)(); }
        else  {         (o.*f)(); }
    }
    void RefAsPtr(F f, C &o, void *r, const void *a)
    {
        typedef ArgHolder<R> RT;
        if(r) { *(RT*)r=(o.*f)(); }
        else  {         (o.*f)(); }
    }
};
template<class C>
struct BC_MemFn0<void, C>
{
    typedef void (C::*F)();
    void RefAsValue(F f, C &o, void *r, const void *a)
    {
        (o.*f)();
    }
    typedef void (C::*F)();
    void RefAsPtr(F f, C &o, void *r, const void *a)
    {
        (o.*f)();
    }
};

template<class R, class C>
struct BC_ConstMemFn0
{
    typedef R (C::*F)() const;
    void RefAsValue(F f, const C &o, void *r, const void *a)
    {
        typedef ValueHolder<R> RT;
        if(r) { *(RT*)r=(o.*f)(); }
        else  {         (o.*f)(); }
    }
    void RefAsPtr(F f, const C &o, void *r, const void *a)
    {
        typedef ArgHolder<R> RT;
        if(r) { *(RT*)r=(o.*f)(); }
        else  {         (o.*f)(); }
    }
};
template<class C>
struct BC_ConstMemFn0<void, C>
{
    typedef void (C::*F)() const;
    void RefAsValue(F f, const C &o, void *r, const void *a)
    {
        (o.*f)();
    }
    typedef void (C::*F)() const;
    void RefAsPtr(F f, const C &o, void *r, const void *a)
    {
        (o.*f)();
    }
};




template<class R, class A0>
struct BC_Fn1
{
    typedef R (*F)(A0);
    void RefAsValue(F f, void *r, const void *a)
    {
        typedef ValueHolder<R> RT;
        typedef ValueList<A0> Args;
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=f(args.a0); }
        else  {         f(args.a0); }
    }
    void RefAsPtr(F f, void *r, const void *a)
    {
        typedef ArgHolder<R> RT;
        typedef ArgList<A0> Args;
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=f(args.a0); }
        else  {         f(args.a0); }
    }
};
template<class A0>
struct BC_Fn1<void, A0>
{
    typedef void (*F)(A0);
    void RefAsValue(F f, void *r, const void *a)
    {
        typedef ValueList<A0> Args;
        Args &args = *(Args*)a;
        f(args.a0);
    }
    void RefAsPtr(F f, void *r, const void *a)
    {
        typedef ArgList<A0> Args;
        Args &args = *(Args*)a;
        f(args.a0);
    }
};

template<class R, class C, class A0>
struct BC_MemFn1
{
    typedef R (C::*F)(A0);
    void RefAsValue(F f, C &o, void *r, const void *a)
    {
        typedef ValueHolder<R> RT;
        typedef ValueList<A0> Args;
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=(o.*f)(args.a0); }
        else  {         (o.*f)(args.a0); }
    }
    void RefAsPtr(F f, C &o, void *r, const void *a)
    {
        typedef ArgHolder<R> RT;
        typedef ArgList<A0> Args;
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=(o.*f)(args.a0); }
        else  {         (o.*f)(args.a0); }
    }
};
template<class C, class A0>
struct BC_MemFn1<void, C, A0>
{
    typedef void (C::*F)(A0);
    void RefAsValue(F f, C &o, void *r, const void *a)
    {
        typedef ValueList<A0> Args;
        Args &args = *(Args*)a;
        (o.*f)(args.a0);
    }
    void RefAsPtr(F f, C &o, void *r, const void *a)
    {
        typedef ArgList<A0> Args;
        Args &args = *(Args*)a;
        (o.*f)(args.a0);
    }
};

template<class R, class C, class A0>
struct BC_ConstMemFn1
{
    typedef R (C::*F)(A0) const;
    void RefAsValue(F f, const C &o, void *r, const void *a)
    {
        typedef ValueHolder<R> RT;
        typedef ValueList<A0> Args;
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=(o.*f)(args.a0); }
        else  {         (o.*f)(args.a0); }
    }
    void RefAsPtr(F f, const C &o, void *r, const void *a)
    {
        typedef ArgHolder<R> RT;
        typedef ArgList<A0> Args;
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=(o.*f)(args.a0); }
        else  {         (o.*f)(args.a0); }
    }
};
template<class C, class A0>
struct BC_ConstMemFn1<void, C, A0>
{
    typedef void (C::*F)(A0) const;
    void RefAsValue(F f, const C &o, void *r, const void *a)
    {
        typedef ValueList<A0> Args;
        Args &args = *(Args*)a;
        (o.*f)(args.a0);
    }
    void RefAsPtr(F f, const C &o, void *r, const void *a)
    {
        typedef ArgList<A0> Args;
        Args &args = *(Args*)a;
        (o.*f)(args.a0);
    }
};

template<class R, class A0, class A1>
struct BC_Fn2
{
    typedef R (*F)(A0, A1);
    void RefAsValue(F f, void *r, const void *a)
    {
        typedef ValueHolder<R> RT;
        typedef ValueList<A0, A1> Args;
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=f(args.a0, args.a1); }
        else  {         f(args.a0, args.a1); }
    }
    void RefAsPtr(F f, void *r, const void *a)
    {
        typedef ArgHolder<R> RT;
        typedef ArgList<A0, A1> Args;
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=f(args.a0, args.a1); }
        else  {         f(args.a0, args.a1); }
    }
};
template<class A0, class A1>
struct BC_Fn2<void, A0, A1>
{
    typedef void (*F)(A0, A1);
    void RefAsValue(F f, void *r, const void *a)
    {
        typedef ValueList<A0, A1> Args;
        Args &args = *(Args*)a;
        f(args.a0, args.a1);
    }
    void RefAsPtr(F f, void *r, const void *a)
    {
        typedef ArgList<A0, A1> Args;
        Args &args = *(Args*)a;
        f(args.a0, args.a1);
    }
};

template<class R, class C, class A0, class A1>
struct BC_MemFn2
{
    typedef R (C::*F)(A0, A1);
    void RefAsValue(F f, C &o, void *r, const void *a)
    {
        typedef ValueHolder<R> RT;
        typedef ValueList<A0, A1> Args;
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=(o.*f)(args.a0, args.a1); }
        else  {         (o.*f)(args.a0, args.a1); }
    }
    void RefAsPtr(F f, C &o, void *r, const void *a)
    {
        typedef ArgHolder<R> RT;
        typedef ArgList<A0, A1> Args;
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=(o.*f)(args.a0, args.a1); }
        else  {         (o.*f)(args.a0, args.a1); }
    }
};
template<class C, class A0, class A1>
struct BC_MemFn2<void, C, A0, A1>
{
    typedef void (C::*F)(A0, A1);
    void RefAsValue(F f, C &o, void *r, const void *a)
    {
        typedef ValueList<A0, A1> Args;
        Args &args = *(Args*)a;
        (o.*f)(args.a0, args.a1);
    }
    void RefAsPtr(F f, C &o, void *r, const void *a)
    {
        typedef ArgList<A0, A1> Args;
        Args &args = *(Args*)a;
        (o.*f)(args.a0, args.a1);
    }
};

template<class R, class C, class A0, class A1>
struct BC_ConstMemFn2
{
    typedef R (C::*F)(A0, A1) const;
    void RefAsValue(F f, const C &o, void *r, const void *a)
    {
        typedef ValueHolder<R> RT;
        typedef ValueList<A0, A1> Args;
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=(o.*f)(args.a0, args.a1); }
        else  {         (o.*f)(args.a0, args.a1); }
    }
    void RefAsPtr(F f, const C &o, void *r, const void *a)
    {
        typedef ArgHolder<R> RT;
        typedef ArgList<A0, A1> Args;
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=(o.*f)(args.a0, args.a1); }
        else  {         (o.*f)(args.a0, args.a1); }
    }
};
template<class C, class A0, class A1>
struct BC_ConstMemFn2<void, C, A0, A1>
{
    typedef void (C::*F)(A0, A1) const;
    void RefAsValue(F f, const C &o, void *r, const void *a)
    {
        typedef ValueList<A0, A1> Args;
        Args &args = *(Args*)a;
        (o.*f)(args.a0, args.a1);
    }
    void RefAsPtr(F f, const C &o, void *r, const void *a)
    {
        typedef ArgList<A0, A1> Args;
        Args &args = *(Args*)a;
        (o.*f)(args.a0, args.a1);
    }
};

template<class R, class A0, class A1, class A2>
struct BC_Fn3
{
    typedef R (*F)(A0, A1, A2);
    void RefAsValue(F f, void *r, const void *a)
    {
        typedef ValueHolder<R> RT;
        typedef ValueList<A0, A1, A2> Args;
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=f(args.a0, args.a1, args.a2); }
        else  {         f(args.a0, args.a1, args.a2); }
    }
    void RefAsPtr(F f, void *r, const void *a)
    {
        typedef ArgHolder<R> RT;
        typedef ArgList<A0, A1, A2> Args;
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=f(args.a0, args.a1, args.a2); }
        else  {         f(args.a0, args.a1, args.a2); }
    }
};
template<class A0, class A1, class A2>
struct BC_Fn3<void, A0, A1, A2>
{
    typedef void (*F)(A0, A1, A2);
    void RefAsValue(F f, void *r, const void *a)
    {
        typedef ValueList<A0, A1, A2> Args;
        Args &args = *(Args*)a;
        f(args.a0, args.a1, args.a2);
    }
    void RefAsPtr(F f, void *r, const void *a)
    {
        typedef ArgList<A0, A1, A2> Args;
        Args &args = *(Args*)a;
        f(args.a0, args.a1, args.a2);
    }
};

template<class R, class C, class A0, class A1, class A2>
struct BC_MemFn3
{
    typedef R (C::*F)(A0, A1, A2);
    void RefAsValue(F f, C &o, void *r, const void *a)
    {
        typedef ValueHolder<R> RT;
        typedef ValueList<A0, A1, A2> Args;
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=(o.*f)(args.a0, args.a1, args.a2); }
        else  {         (o.*f)(args.a0, args.a1, args.a2); }
    }
    void RefAsPtr(F f, C &o, void *r, const void *a)
    {
        typedef ArgHolder<R> RT;
        typedef ArgList<A0, A1, A2> Args;
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=(o.*f)(args.a0, args.a1, args.a2); }
        else  {         (o.*f)(args.a0, args.a1, args.a2); }
    }
};
template<class C, class A0, class A1, class A2>
struct BC_MemFn3<void, C, A0, A1, A2>
{
    typedef void (C::*F)(A0, A1, A2);
    void RefAsValue(F f, C &o, void *r, const void *a)
    {
        typedef ValueList<A0, A1, A2> Args;
        Args &args = *(Args*)a;
        (o.*f)(args.a0, args.a1, args.a2);
    }
    void RefAsPtr(F f, C &o, void *r, const void *a)
    {
        typedef ArgList<A0, A1, A2> Args;
        Args &args = *(Args*)a;
        (o.*f)(args.a0, args.a1, args.a2);
    }
};

template<class R, class C, class A0, class A1, class A2>
struct BC_ConstMemFn3
{
    typedef R (C::*F)(A0, A1, A2) const;
    void RefAsValue(F f, const C &o, void *r, const void *a)
    {
        typedef ValueHolder<R> RT;
        typedef ValueList<A0, A1, A2> Args;
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=(o.*f)(args.a0, args.a1, args.a2); }
        else  {         (o.*f)(args.a0, args.a1, args.a2); }
    }
    void RefAsPtr(F f, const C &o, void *r, const void *a)
    {
        typedef ArgHolder<R> RT;
        typedef ArgList<A0, A1, A2> Args;
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=(o.*f)(args.a0, args.a1, args.a2); }
        else  {         (o.*f)(args.a0, args.a1, args.a2); }
    }
};
template<class C, class A0, class A1, class A2>
struct BC_ConstMemFn3<void, C, A0, A1, A2>
{
    typedef void (C::*F)(A0, A1, A2) const;
    void RefAsValue(F f, const C &o, void *r, const void *a)
    {
        typedef ValueList<A0, A1, A2> Args;
        Args &args = *(Args*)a;
        (o.*f)(args.a0, args.a1, args.a2);
    }
    void RefAsPtr(F f, const C &o, void *r, const void *a)
    {
        typedef ArgList<A0, A1, A2> Args;
        Args &args = *(Args*)a;
        (o.*f)(args.a0, args.a1, args.a2);
    }
};

template<class R, class A0, class A1, class A2, class A3>
struct BC_Fn4
{
    typedef R (*F)(A0, A1, A2, A3);
    void RefAsValue(F f, void *r, const void *a)
    {
        typedef ValueHolder<R> RT;
        typedef ValueList<A0, A1, A2, A3> Args;
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=f(args.a0, args.a1, args.a2, args.a3); }
        else  {         f(args.a0, args.a1, args.a2, args.a3); }
    }
    void RefAsPtr(F f, void *r, const void *a)
    {
        typedef ArgHolder<R> RT;
        typedef ArgList<A0, A1, A2, A3> Args;
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=f(args.a0, args.a1, args.a2, args.a3); }
        else  {         f(args.a0, args.a1, args.a2, args.a3); }
    }
};
template<class A0, class A1, class A2, class A3>
struct BC_Fn4<void, A0, A1, A2, A3>
{
    typedef void (*F)(A0, A1, A2, A3);
    void RefAsValue(F f, void *r, const void *a)
    {
        typedef ValueList<A0, A1, A2, A3> Args;
        Args &args = *(Args*)a;
        f(args.a0, args.a1, args.a2, args.a3);
    }
    void RefAsPtr(F f, void *r, const void *a)
    {
        typedef ArgList<A0, A1, A2, A3> Args;
        Args &args = *(Args*)a;
        f(args.a0, args.a1, args.a2, args.a3);
    }
};

template<class R, class C, class A0, class A1, class A2, class A3>
struct BC_MemFn4
{
    typedef R (C::*F)(A0, A1, A2, A3);
    void RefAsValue(F f, C &o, void *r, const void *a)
    {
        typedef ValueHolder<R> RT;
        typedef ValueList<A0, A1, A2, A3> Args;
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=(o.*f)(args.a0, args.a1, args.a2, args.a3); }
        else  {         (o.*f)(args.a0, args.a1, args.a2, args.a3); }
    }
    void RefAsPtr(F f, C &o, void *r, const void *a)
    {
        typedef ArgHolder<R> RT;
        typedef ArgList<A0, A1, A2, A3> Args;
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=(o.*f)(args.a0, args.a1, args.a2, args.a3); }
        else  {         (o.*f)(args.a0, args.a1, args.a2, args.a3); }
    }
};
template<class C, class A0, class A1, class A2, class A3>
struct BC_MemFn4<void, C, A0, A1, A2, A3>
{
    typedef void (C::*F)(A0, A1, A2, A3);
    void RefAsValue(F f, C &o, void *r, const void *a)
    {
        typedef ValueList<A0, A1, A2, A3> Args;
        Args &args = *(Args*)a;
        (o.*f)(args.a0, args.a1, args.a2, args.a3);
    }
    void RefAsPtr(F f, C &o, void *r, const void *a)
    {
        typedef ArgList<A0, A1, A2, A3> Args;
        Args &args = *(Args*)a;
        (o.*f)(args.a0, args.a1, args.a2, args.a3);
    }
};

template<class R, class C, class A0, class A1, class A2, class A3>
struct BC_ConstMemFn4
{
    typedef R (C::*F)(A0, A1, A2, A3) const;
    void RefAsValue(F f, const C &o, void *r, const void *a)
    {
        typedef ValueHolder<R> RT;
        typedef ValueList<A0, A1, A2, A3> Args;
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=(o.*f)(args.a0, args.a1, args.a2, args.a3); }
        else  {         (o.*f)(args.a0, args.a1, args.a2, args.a3); }
    }
    void RefAsPtr(F f, const C &o, void *r, const void *a)
    {
        typedef ArgHolder<R> RT;
        typedef ArgList<A0, A1, A2, A3> Args;
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=(o.*f)(args.a0, args.a1, args.a2, args.a3); }
        else  {         (o.*f)(args.a0, args.a1, args.a2, args.a3); }
    }
};
template<class C, class A0, class A1, class A2, class A3>
struct BC_ConstMemFn4<void, C, A0, A1, A2, A3>
{
    typedef void (C::*F)(A0, A1, A2, A3) const;
    void RefAsValue(F f, const C &o, void *r, const void *a)
    {
        typedef ValueList<A0, A1, A2, A3> Args;
        Args &args = *(Args*)a;
        (o.*f)(args.a0, args.a1, args.a2, args.a3);
    }
    void RefAsPtr(F f, const C &o, void *r, const void *a)
    {
        typedef ArgList<A0, A1, A2, A3> Args;
        Args &args = *(Args*)a;
        (o.*f)(args.a0, args.a1, args.a2, args.a3);
    }
};



template<class R>
inline void BinaryCall(R (*f)(), void *r, const void *a=NULL)
{ BC_Fn0<R>().RefAsValue(f, r, a); }

template<class R>
inline void BinaryCallRef(R (*f)(), void *r, const void *a=NULL)
{ BC_Fn0<R>().RefAsPtr(f, r, a); }

template<class R, class C>
inline void BinaryCall(R (C::*f)(), C &o, void *r, const void *a=NULL)
{ BC_MemFn0<R,C>().RefAsValue(f, o, r, a); }

template<class R, class C>
inline void BinaryCallRef(R (C::*f)(), C &o, void *r, const void *a=NULL)
{ BC_MemFn0<R,C>().RefAsPtr(f, o, r, a); }

template<class R, class C>
inline void BinaryCall(R (C::*f)() const, const C &o, void *r, const void *a=NULL)
{ BC_ConstMemFn0<R,C>().RefAsValue(f, o, r, a); }

template<class R, class C>
inline void BinaryCallRef(R (C::*f)() const, const C &o, void *r, const void *a=NULL)
{ BC_ConstMemFn0<R,C>().RefAsPtr(f, o, r, a); }




template<class R, class A0>
inline void BinaryCall(R (*f)(A0), void *r, const void *a)
{ BC_Fn1<R,A0>().RefAsValue(f,r,a); }

template<class R, class A0>
inline void BinaryCallRef(R (*f)(A0), void *r, const void *a)
{ BC_Fn1<R,A0>().RefAsPtr(f,r,a); }

template<class R, class C, class A0>
inline void BinaryCall(R (C::*f)(A0), C &o, void *r, const void *a)
{ BC_MemFn1<R,C,A0>().RefAsValue(f,o,r,a); }

template<class R, class C, class A0>
inline void BinaryCallRef(R (C::*f)(A0), C &o, void *r, const void *a)
{ BC_MemFn1<R,C,A0>().RefAsPtr(f,o,r,a); }

template<class R, class C, class A0>
inline void BinaryCall(R (C::*f)(A0) const, const C &o, void *r, const void *a)
{ BC_ConstMemFn1<R,C,A0>().RefAsValue(f,o,r,a); }

template<class R, class C, class A0>
inline void BinaryCallRef(R (C::*f)(A0) const, const C &o, void *r, const void *a)
{ BC_ConstMemFn1<R,C,A0>().RefAsPtr(f,o,r,a); }



template<class R, class A0, class A1>
inline void BinaryCall(R (*f)(A0,A1), void *r, const void *a)
{ BC_Fn2<R,A0,A1>().RefAsValue(f,r,a); }

template<class R, class A0, class A1>
inline void BinaryCallRef(R (*f)(A0,A1), void *r, const void *a)
{ BC_Fn2<R,A0,A1>().RefAsPtr(f,r,a); }

template<class R, class C, class A0, class A1>
inline void BinaryCall(R (C::*f)(A0,A1), C &o, void *r, const void *a)
{ BC_MemFn2<R,C,A0,A1>().RefAsValue(f,o,r,a); }

template<class R, class C, class A0, class A1>
inline void BinaryCallRef(R (C::*f)(A0,A1), C &o, void *r, const void *a)
{ BC_MemFn2<R,C,A0,A1>().RefAsPtr(f,o,r,a); }

template<class R, class C, class A0, class A1>
inline void BinaryCall(R (C::*f)(A0,A1) const, const C &o, void *r, const void *a)
{ BC_ConstMemFn2<R,C,A0,A1>().RefAsValue(f,o,r,a); }

template<class R, class C, class A0, class A1>
inline void BinaryCallRef(R (C::*f)(A0,A1) const, const C &o, void *r, const void *a)
{ BC_ConstMemFn2<R,C,A0,A1>().RefAsPtr(f,o,r,a); }



template<class R, class A0, class A1, class A2>
inline void BinaryCall(R (*f)(A0,A1,A2), void *r, const void *a)
{ BC_Fn3<R,A0,A1,A2>().RefAsValue(f,r,a); }

template<class R, class A0, class A1, class A2>
inline void BinaryCallRef(R (*f)(A0,A1,A2), void *r, const void *a)
{ BC_Fn3<R,A0,A1,A2>().RefAsPtr(f,r,a); }

template<class R, class C, class A0, class A1, class A2>
inline void BinaryCall(R (C::*f)(A0,A1,A2), C &o, void *r, const void *a)
{ BC_MemFn3<R,C,A0,A1,A2>().RefAsValue(f,o,r,a); }

template<class R, class C, class A0, class A1, class A2>
inline void BinaryCallRef(R (C::*f)(A0,A1,A2), C &o, void *r, const void *a)
{ BC_MemFn3<R,C,A0,A1,A2>().RefAsPtr(f,o,r,a); }

template<class R, class C, class A0, class A1, class A2>
inline void BinaryCall(R (C::*f)(A0,A1,A2) const, const C &o, void *r, const void *a)
{ BC_ConstMemFn3<R,C,A0,A1,A2>().RefAsValue(f,o,r,a); }

template<class R, class C, class A0, class A1, class A2>
inline void BinaryCallRef(R (C::*f)(A0,A1,A2) const, const C &o, void *r, const void *a)
{ BC_ConstMemFn3<R,C,A0,A1,A2>().RefAsPtr(f,o,r,a); }



template<class R, class A0, class A1, class A2, class A3>
inline void BinaryCall(R (*f)(A0,A1,A2,A3), void *r, const void *a)
{ BC_Fn4<R,A0,A1,A2,A3>().RefAsValue(f,r,a); }

template<class R, class A0, class A1, class A2, class A3>
inline void BinaryCallRef(R (*f)(A0,A1,A2,A3), void *r, const void *a)
{ BC_Fn4<R,A0,A1,A2,A3>().RefAsPtr(f,r,a); }

template<class R, class C, class A0, class A1, class A2, class A3>
inline void BinaryCall(R (C::*f)(A0,A1,A2,A3), C &o, void *r, const void *a)
{ BC_MemFn4<R,C,A0,A1,A2,A3>().RefAsValue(f,o,r,a); }

template<class R, class C, class A0, class A1, class A2, class A3>
inline void BinaryCallRef(R (C::*f)(A0,A1,A2,A3), C &o, void *r, const void *a)
{ BC_MemFn4<R,C,A0,A1,A2,A3>().RefAsPtr(f,o,r,a); }

template<class R, class C, class A0, class A1, class A2, class A3>
inline void BinaryCall(R (C::*f)(A0,A1,A2,A3) const, const C &o, void *r, const void *a)
{ BC_ConstMemFn4<R,C,A0,A1,A2,A3>().RefAsValue(f,o,r,a); }

template<class R, class C, class A0, class A1, class A2, class A3>
inline void BinaryCallRef(R (C::*f)(A0,A1,A2,A3) const, const C &o, void *r, const void *a)
{ BC_ConstMemFn4<R,C,A0,A1,A2,A3>().RefAsPtr(f,o,r,a); }


#endif // BinaryCall_h
