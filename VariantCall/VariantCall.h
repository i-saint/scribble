#include <type_traits>
#include "Variant.h"



#define RemoveCR(T) typename std::remove_const<typename std::remove_reference<T>::type>::type

template<class R, size_t SR>
struct VC_Fn0
{
    typedef R (*F)();
    typedef TVariant<SR> VR;
    void operator()(F f, VR *r)
    {
        if(r){ *r = f(); }
        else { f(); }
    }
};
template<size_t SR>
struct VC_Fn0<void, SR>
{
    typedef void (*F)();
    typedef TVariant<SR> VR;
    void operator()(F f, VR *)
    { f(); }
};

template<class R, class C, size_t SR>
struct VC_MemFn0
{
    typedef R (C::*F)();
    typedef TVariant<SR> VR;
    void operator()(F f, C *o, VR *r)
    {
        if(r){ *r = (o->*f)(); }
        else { (o->*f)(); }
    }
};
template<class C, size_t SR>
struct VC_MemFn0<void, C, SR>
{
    typedef void (C::*F)();
    typedef TVariant<SR> VR;
    void operator()(F f, C *o, VR *)
    { (o->*f)(); }
};

template<class R, class C, size_t SR>
struct VC_ConstMemFn0
{
    typedef R (C::*F)() const;
    typedef TVariant<SR> VR;
    void operator()(F f, const C *o, VR *r)
    {
        if(r){ *r = (o->*f)(); }
        else { (o->*f)(); }
    }
};
template<class C, size_t SR>
struct VC_ConstMemFn0<void, C, SR>
{
    typedef void (C::*F)() const;
    typedef TVariant<SR> VR;
    void operator()(F f, const C *o, VR *)
    { (o->*f)(); }
};




template<class R, class A0, size_t SR, size_t SA0>
struct VC_Fn1
{
    typedef R (*F)(A0);
    typedef RemoveCR(A0) A0T;
    typedef TVariant<SR> VR;
    typedef TVariant<SA0> VA0;
    void operator()(F f, VR *r, const VA0 &a0)
    {
        if(r){ *r=f(const_cast<A0T&>(a0.cast<A0T>())); }
        else {    f(const_cast<A0T&>(a0.cast<A0T>())); }
    }
};
template<class A0, size_t SR, size_t SA0>
struct VC_Fn1<void, A0, SR, SA0>
{
    typedef void (*F)(A0);
    typedef RemoveCR(A0) A0T;
    typedef TVariant<SR> VR;
    typedef TVariant<SA0> VA0;
    void operator()(F f, VR *r, const VA0 &a0)
    {
        f(const_cast<A0T&>(a0.cast<A0T>()));
    }
};

template<class R, class C, class A0, size_t SR, size_t SA0>
struct VC_MemFn1
{
    typedef R (C::*F)(A0);
    typedef RemoveCR(A0) A0T;
    typedef TVariant<SR> VR;
    typedef TVariant<SA0> VA0;
    void operator()(F f, C *o, VR *r, const VA0 &a0)
    {
        if(r){ *r=(o->*f)(const_cast<A0T&>(a0.cast<A0T>())); }
        else {    (o->*f)(const_cast<A0T&>(a0.cast<A0T>())); }
    }
};
template<class C, class A0, size_t SR, size_t SA0>
struct VC_MemFn1<void, C, A0, SR, SA0>
{
    typedef void (C::*F)(A0);
    typedef RemoveCR(A0) A0T;
    typedef TVariant<SR> VR;
    typedef TVariant<SA0> VA0;
    void operator()(F f, C *o, VR *r, const VA0 &a0)
    {
        (o->*f)(const_cast<A0T&>(a0.cast<A0T>()));
    }
};

template<class R, class C, class A0, size_t SR, size_t SA0>
struct VC_ConstMemFn1
{
    typedef R (C::*F)(A0) const;
    typedef RemoveCR(A0) A0T;
    typedef TVariant<SR> VR;
    typedef TVariant<SA0> VA0;
    void operator()(F f, const C *o, VR *r, const VA0 &a0)
    {
        if(r){ *r=(o->*f)(const_cast<A0T&>(a0.cast<A0T>())); }
        else {    (o->*f)(const_cast<A0T&>(a0.cast<A0T>())); }
    }
};
template<class C, class A0, size_t SR, size_t SA0>
struct VC_ConstMemFn1<void, C, A0, SR, SA0>
{
    typedef void (C::*F)(A0) const;
    typedef RemoveCR(A0) A0T;
    typedef TVariant<SR> VR;
    typedef TVariant<SA0> VA0;
    void operator()(F f, const C *o, VR *r, const VA0 &a0)
    {
        (o->*f)(const_cast<A0T&>(a0.cast<A0T>()));
    }
};
template<class R, class A0, class A1, size_t SR, size_t SA0, size_t SA1>
struct VC_Fn2
{
    typedef R (*F)(A0, A1);
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    typedef TVariant<SR> VR;
    typedef TVariant<SA0> VA0;
    typedef TVariant<SA1> VA1;
    void operator()(F f, VR *r, const VA0 &a0, const VA1 &a1)
    {
        if(r){ *r=f(const_cast<A0T&>(a0.cast<A0T>()), const_cast<A1T&>(a1.cast<A1T>())); }
        else {    f(const_cast<A0T&>(a0.cast<A0T>()), const_cast<A1T&>(a1.cast<A1T>())); }
    }
};
template<class A0, class A1, size_t SR, size_t SA0, size_t SA1>
struct VC_Fn2<void, A0, A1, SR, SA0, SA1>
{
    typedef void (*F)(A0, A1);
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    typedef TVariant<SR> VR;
    typedef TVariant<SA0> VA0;
    typedef TVariant<SA1> VA1;
    void operator()(F f, VR *r, const VA0 &a0, const VA1 &a1)
    {
        f(const_cast<A0T&>(a0.cast<A0T>()), const_cast<A1T&>(a1.cast<A1T>()));
    }
};

template<class R, class C, class A0, class A1, size_t SR, size_t SA0, size_t SA1>
struct VC_MemFn2
{
    typedef R (C::*F)(A0, A1);
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    typedef TVariant<SR> VR;
    typedef TVariant<SA0> VA0;
    typedef TVariant<SA1> VA1;
    void operator()(F f, C *o, VR *r, const VA0 &a0, const VA1 &a1)
    {
        if(r){ *r=(o->*f)(const_cast<A0T&>(a0.cast<A0T>()), const_cast<A1T&>(a1.cast<A1T>())); }
        else {    (o->*f)(const_cast<A0T&>(a0.cast<A0T>()), const_cast<A1T&>(a1.cast<A1T>())); }
    }
};
template<class C, class A0, class A1, size_t SR, size_t SA0, size_t SA1>
struct VC_MemFn2<void, C, A0, A1, SR, SA0, SA1>
{
    typedef void (C::*F)(A0, A1);
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    typedef TVariant<SR> VR;
    typedef TVariant<SA0> VA0;
    typedef TVariant<SA1> VA1;
    void operator()(F f, C *o, VR *r, const VA0 &a0, const VA1 &a1)
    {
        (o->*f)(const_cast<A0T&>(a0.cast<A0T>()), const_cast<A1T&>(a1.cast<A1T>()));
    }
};

template<class R, class C, class A0, class A1, size_t SR, size_t SA0, size_t SA1>
struct VC_ConstMemFn2
{
    typedef R (C::*F)(A0, A1) const;
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    typedef TVariant<SR> VR;
    typedef TVariant<SA0> VA0;
    typedef TVariant<SA1> VA1;
    void operator()(F f, const C *o, VR *r, const VA0 &a0, const VA1 &a1)
    {
        if(r){ *r=(o->*f)(const_cast<A0T&>(a0.cast<A0T>()), const_cast<A1T&>(a1.cast<A1T>())); }
        else {    (o->*f)(const_cast<A0T&>(a0.cast<A0T>()), const_cast<A1T&>(a1.cast<A1T>())); }
    }
};
template<class C, class A0, class A1, size_t SR, size_t SA0, size_t SA1>
struct VC_ConstMemFn2<void, C, A0, A1, SR, SA0, SA1>
{
    typedef void (C::*F)(A0, A1) const;
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    typedef TVariant<SR> VR;
    typedef TVariant<SA0> VA0;
    typedef TVariant<SA1> VA1;
    void operator()(F f, const C *o, VR *r, const VA0 &a0, const VA1 &a1)
    {
        (o->*f)(const_cast<A0T&>(a0.cast<A0T>()), const_cast<A1T&>(a1.cast<A1T>()));
    }
};
template<class R, class A0, class A1, class A2, size_t SR, size_t SA0, size_t SA1, size_t SA2>
struct VC_Fn3
{
    typedef R (*F)(A0, A1, A2);
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    typedef RemoveCR(A2) A2T;
    typedef TVariant<SR> VR;
    typedef TVariant<SA0> VA0;
    typedef TVariant<SA1> VA1;
    typedef TVariant<SA2> VA2;
    void operator()(F f, VR *r, const VA0 &a0, const VA1 &a1, const VA2 &a2)
    {
        if(r){ *r=f(const_cast<A0T&>(a0.cast<A0T>()), const_cast<A1T&>(a1.cast<A1T>()), const_cast<A2T&>(a2.cast<A2T>())); }
        else {    f(const_cast<A0T&>(a0.cast<A0T>()), const_cast<A1T&>(a1.cast<A1T>()), const_cast<A2T&>(a2.cast<A2T>())); }
    }
};
template<class A0, class A1, class A2, size_t SR, size_t SA0, size_t SA1, size_t SA2>
struct VC_Fn3<void, A0, A1, A2, SR, SA0, SA1, SA2>
{
    typedef void (*F)(A0, A1, A2);
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    typedef RemoveCR(A2) A2T;
    typedef TVariant<SR> VR;
    typedef TVariant<SA0> VA0;
    typedef TVariant<SA1> VA1;
    typedef TVariant<SA2> VA2;
    void operator()(F f, VR *r, const VA0 &a0, const VA1 &a1, const VA2 &a2)
    {
        f(const_cast<A0T&>(a0.cast<A0T>()), const_cast<A1T&>(a1.cast<A1T>()), const_cast<A2T&>(a2.cast<A2T>()));
    }
};

template<class R, class C, class A0, class A1, class A2, size_t SR, size_t SA0, size_t SA1, size_t SA2>
struct VC_MemFn3
{
    typedef R (C::*F)(A0, A1, A2);
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    typedef RemoveCR(A2) A2T;
    typedef TVariant<SR> VR;
    typedef TVariant<SA0> VA0;
    typedef TVariant<SA1> VA1;
    typedef TVariant<SA2> VA2;
    void operator()(F f, C *o, VR *r, const VA0 &a0, const VA1 &a1, const VA2 &a2)
    {
        if(r){ *r=(o->*f)(const_cast<A0T&>(a0.cast<A0T>()), const_cast<A1T&>(a1.cast<A1T>()), const_cast<A2T&>(a2.cast<A2T>())); }
        else {    (o->*f)(const_cast<A0T&>(a0.cast<A0T>()), const_cast<A1T&>(a1.cast<A1T>()), const_cast<A2T&>(a2.cast<A2T>())); }
    }
};
template<class C, class A0, class A1, class A2, size_t SR, size_t SA0, size_t SA1, size_t SA2>
struct VC_MemFn3<void, C, A0, A1, A2, SR, SA0, SA1, SA2>
{
    typedef void (C::*F)(A0, A1, A2);
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    typedef RemoveCR(A2) A2T;
    typedef TVariant<SR> VR;
    typedef TVariant<SA0> VA0;
    typedef TVariant<SA1> VA1;
    typedef TVariant<SA2> VA2;
    void operator()(F f, C *o, VR *r, const VA0 &a0, const VA1 &a1, const VA2 &a2)
    {
        (o->*f)(const_cast<A0T&>(a0.cast<A0T>()), const_cast<A1T&>(a1.cast<A1T>()), const_cast<A2T&>(a2.cast<A2T>()));
    }
};

template<class R, class C, class A0, class A1, class A2, size_t SR, size_t SA0, size_t SA1, size_t SA2>
struct VC_ConstMemFn3
{
    typedef R (C::*F)(A0, A1, A2) const;
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    typedef RemoveCR(A2) A2T;
    typedef TVariant<SR> VR;
    typedef TVariant<SA0> VA0;
    typedef TVariant<SA1> VA1;
    typedef TVariant<SA2> VA2;
    void operator()(F f, const C *o, VR *r, const VA0 &a0, const VA1 &a1, const VA2 &a2)
    {
        if(r){ *r=(o->*f)(const_cast<A0T&>(a0.cast<A0T>()), const_cast<A1T&>(a1.cast<A1T>()), const_cast<A2T&>(a2.cast<A2T>())); }
        else {    (o->*f)(const_cast<A0T&>(a0.cast<A0T>()), const_cast<A1T&>(a1.cast<A1T>()), const_cast<A2T&>(a2.cast<A2T>())); }
    }
};
template<class C, class A0, class A1, class A2, size_t SR, size_t SA0, size_t SA1, size_t SA2>
struct VC_ConstMemFn3<void, C, A0, A1, A2, SR, SA0, SA1, SA2>
{
    typedef void (C::*F)(A0, A1, A2) const;
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    typedef RemoveCR(A2) A2T;
    typedef TVariant<SR> VR;
    typedef TVariant<SA0> VA0;
    typedef TVariant<SA1> VA1;
    typedef TVariant<SA2> VA2;
    void operator()(F f, const C *o, VR *r, const VA0 &a0, const VA1 &a1, const VA2 &a2)
    {
        (o->*f)(const_cast<A0T&>(a0.cast<A0T>()), const_cast<A1T&>(a1.cast<A1T>()), const_cast<A2T&>(a2.cast<A2T>()));
    }
};
template<class R, class A0, class A1, class A2, class A3, size_t SR, size_t SA0, size_t SA1, size_t SA2, size_t SA3>
struct VC_Fn4
{
    typedef R (*F)(A0, A1, A2, A3);
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    typedef RemoveCR(A2) A2T;
    typedef RemoveCR(A3) A3T;
    typedef TVariant<SR> VR;
    typedef TVariant<SA0> VA0;
    typedef TVariant<SA1> VA1;
    typedef TVariant<SA2> VA2;
    typedef TVariant<SA3> VA3;
    void operator()(F f, VR *r, const VA0 &a0, const VA1 &a1, const VA2 &a2, const VA3 &a3)
    {
        if(r){ *r=f(const_cast<A0T&>(a0.cast<A0T>()), const_cast<A1T&>(a1.cast<A1T>()), const_cast<A2T&>(a2.cast<A2T>()), const_cast<A3T&>(a3.cast<A3T>())); }
        else {    f(const_cast<A0T&>(a0.cast<A0T>()), const_cast<A1T&>(a1.cast<A1T>()), const_cast<A2T&>(a2.cast<A2T>()), const_cast<A3T&>(a3.cast<A3T>())); }
    }
};
template<class A0, class A1, class A2, class A3, size_t SR, size_t SA0, size_t SA1, size_t SA2, size_t SA3>
struct VC_Fn4<void, A0, A1, A2, A3, SR, SA0, SA1, SA2, SA3>
{
    typedef void (*F)(A0, A1, A2, A3);
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    typedef RemoveCR(A2) A2T;
    typedef RemoveCR(A3) A3T;
    typedef TVariant<SR> VR;
    typedef TVariant<SA0> VA0;
    typedef TVariant<SA1> VA1;
    typedef TVariant<SA2> VA2;
    typedef TVariant<SA3> VA3;
    void operator()(F f, VR *r, const VA0 &a0, const VA1 &a1, const VA2 &a2, const VA3 &a3)
    {
        f(const_cast<A0T&>(a0.cast<A0T>()), const_cast<A1T&>(a1.cast<A1T>()), const_cast<A2T&>(a2.cast<A2T>()), const_cast<A3T&>(a3.cast<A3T>()));
    }
};

template<class R, class C, class A0, class A1, class A2, class A3, size_t SR, size_t SA0, size_t SA1, size_t SA2, size_t SA3>
struct VC_MemFn4
{
    typedef R (C::*F)(A0, A1, A2, A3);
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    typedef RemoveCR(A2) A2T;
    typedef RemoveCR(A3) A3T;
    typedef TVariant<SR> VR;
    typedef TVariant<SA0> VA0;
    typedef TVariant<SA1> VA1;
    typedef TVariant<SA2> VA2;
    typedef TVariant<SA3> VA3;
    void operator()(F f, C *o, VR *r, const VA0 &a0, const VA1 &a1, const VA2 &a2, const VA3 &a3)
    {
        if(r){ *r=(o->*f)(const_cast<A0T&>(a0.cast<A0T>()), const_cast<A1T&>(a1.cast<A1T>()), const_cast<A2T&>(a2.cast<A2T>()), const_cast<A3T&>(a3.cast<A3T>())); }
        else {    (o->*f)(const_cast<A0T&>(a0.cast<A0T>()), const_cast<A1T&>(a1.cast<A1T>()), const_cast<A2T&>(a2.cast<A2T>()), const_cast<A3T&>(a3.cast<A3T>())); }
    }
};
template<class C, class A0, class A1, class A2, class A3, size_t SR, size_t SA0, size_t SA1, size_t SA2, size_t SA3>
struct VC_MemFn4<void, C, A0, A1, A2, A3, SR, SA0, SA1, SA2, SA3>
{
    typedef void (C::*F)(A0, A1, A2, A3);
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    typedef RemoveCR(A2) A2T;
    typedef RemoveCR(A3) A3T;
    typedef TVariant<SR> VR;
    typedef TVariant<SA0> VA0;
    typedef TVariant<SA1> VA1;
    typedef TVariant<SA2> VA2;
    typedef TVariant<SA3> VA3;
    void operator()(F f, C *o, VR *r, const VA0 &a0, const VA1 &a1, const VA2 &a2, const VA3 &a3)
    {
        (o->*f)(const_cast<A0T&>(a0.cast<A0T>()), const_cast<A1T&>(a1.cast<A1T>()), const_cast<A2T&>(a2.cast<A2T>()), const_cast<A3T&>(a3.cast<A3T>()));
    }
};

template<class R, class C, class A0, class A1, class A2, class A3, size_t SR, size_t SA0, size_t SA1, size_t SA2, size_t SA3>
struct VC_ConstMemFn4
{
    typedef R (C::*F)(A0, A1, A2, A3) const;
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    typedef RemoveCR(A2) A2T;
    typedef RemoveCR(A3) A3T;
    typedef TVariant<SR> VR;
    typedef TVariant<SA0> VA0;
    typedef TVariant<SA1> VA1;
    typedef TVariant<SA2> VA2;
    typedef TVariant<SA3> VA3;
    void operator()(F f, const C *o, VR *r, const VA0 &a0, const VA1 &a1, const VA2 &a2, const VA3 &a3)
    {
        if(r){ *r=(o->*f)(const_cast<A0T&>(a0.cast<A0T>()), const_cast<A1T&>(a1.cast<A1T>()), const_cast<A2T&>(a2.cast<A2T>()), const_cast<A3T&>(a3.cast<A3T>())); }
        else {    (o->*f)(const_cast<A0T&>(a0.cast<A0T>()), const_cast<A1T&>(a1.cast<A1T>()), const_cast<A2T&>(a2.cast<A2T>()), const_cast<A3T&>(a3.cast<A3T>())); }
    }
};
template<class C, class A0, class A1, class A2, class A3, size_t SR, size_t SA0, size_t SA1, size_t SA2, size_t SA3>
struct VC_ConstMemFn4<void, C, A0, A1, A2, A3, SR, SA0, SA1, SA2, SA3>
{
    typedef void (C::*F)(A0, A1, A2, A3) const;
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    typedef RemoveCR(A2) A2T;
    typedef RemoveCR(A3) A3T;
    typedef TVariant<SR> VR;
    typedef TVariant<SA0> VA0;
    typedef TVariant<SA1> VA1;
    typedef TVariant<SA2> VA2;
    typedef TVariant<SA3> VA3;
    void operator()(F f, const C *o, VR *r, const VA0 &a0, const VA1 &a1, const VA2 &a2, const VA3 &a3)
    {
        (o->*f)(const_cast<A0T&>(a0.cast<A0T>()), const_cast<A1T&>(a1.cast<A1T>()), const_cast<A2T&>(a2.cast<A2T>()), const_cast<A3T&>(a3.cast<A3T>()));
    }
};

#undef RemoveCR




template<class R, size_t SR>
inline void VariantCall(R (*f)(), TVariant<SR> *r)
{ VC_Fn0<R,SR>()(f, r); }

template<class R>
inline void VariantCall(R (*f)())
{ VC_Fn0<R,4>()(f, NULL); }


template<class R, class C, size_t SR>
inline void VariantCall(R (C::*f)(), C *o, TVariant<SR> *r)
{ VC_MemFn0<R,C,SR>()(f, o, r); }

template<class R, class C>
inline void VariantCall(R (C::*f)(), C *o)
{ VC_MemFn0<R,C,4>()(f, o, NULL); }


template<class R, class C, size_t SR>
inline void VariantCall(R (C::*f)() const, const C *o, TVariant<SR> *r)
{ VC_ConstMemFn0<R,C,SR>()(f, o, r); }

template<class R, class C>
inline void VariantCall(R (C::*f)() const, const C *o)
{ VC_ConstMemFn0<R,C,4>()(f, o, NULL); }




template<class R, class A0, size_t SR, size_t SA0>
inline void VariantCall(R (*f)(A0), TVariant<SR> *r, TVariant<SA0> &a0)
{ VC_Fn1<R,A0,SR,SA0>()(f, r, a0); }

template<class R, class A0, size_t SR, size_t SA>
inline void VariantCall(R (*f)(A0), TVariant<SR> *r, const TVariant<SA> *va)
{ VC_Fn1<R,A0,SR,SA>()(f, r, va[0]); }

template<class R, class A0, size_t SA0>
inline void VariantCall(R (*f)(A0), TVariant<SA0> &a0)
{ VC_Fn1<R,A0,4,SA0>()(f, NULL, a0); }

template<class R, class A0, size_t SA>
inline void VariantCall(R (*f)(A0), const TVariant<SA> *va)
{ VC_Fn1<R,A0,4,SA>()(f, NULL, va[0]); }


template<class R, class C, class A0, size_t SR, size_t SA0>
inline void VariantCall(R (C::*f)(A0), C *o, TVariant<SR> *r, TVariant<SA0> &a0)
{ VC_MemFn1<R,C,A0,SR,SA0>()(f, o, r, a0); }

template<class R, class C, class A0, size_t SR, size_t SA>
inline void VariantCall(R (C::*f)(A0), C *o, TVariant<SR> *r, const TVariant<SA> *va)
{ VC_MemFn1<R,C,A0,SR,SA>()(f, o, r, va[0]); }

template<class R, class C, class A0, size_t SA0>
inline void VariantCall(R (C::*f)(A0), C *o, TVariant<SA0> &a0)
{ VC_MemFn1<R,C,A0,4,SA0>()(f, o, NULL, a0); }

template<class R, class C, class A0, size_t SA>
inline void VariantCall(R (C::*f)(A0), C *o, const TVariant<SA> *va)
{ VC_MemFn1<R,C,A0,4,SA>()(f, o, NULL, va[0]); }


template<class R, class C, class A0, size_t SR, size_t SA0>
inline void VariantCall(R (C::*f)(A0) const, const C *o, TVariant<SR> *r, TVariant<SA0> &a0)
{ VC_ConstMemFn1<R,C,A0,SR,SA0>()(f, o, r, a0); }

template<class R, class C, class A0, size_t SR, size_t SA>
inline void VariantCall(R (C::*f)(A0) const, const C *o, TVariant<SR> *r, const TVariant<SA> *va)
{ VC_ConstMemFn1<R,C,A0,SR,SA>()(f, o, r, va[0]); }

template<class R, class C, class A0, size_t SA0>
inline void VariantCall(R (C::*f)(A0) const, const C *o, TVariant<SA0> &a0)
{ VC_ConstMemFn1<R,C,A0,4,SA0>()(f, o, NULL, a0); }

template<class R, class C, class A0, size_t SA>
inline void VariantCall(R (C::*f)(A0) const, const C *o, const TVariant<SA> *va)
{ VC_ConstMemFn1<R,C,A0,4,SA>()(f, o, NULL, va[0]); }



template<class R, class A0, class A1, size_t SR, size_t SA0, size_t SA1>
inline void VariantCall(R (*f)(A0,A1), TVariant<SR> *r, TVariant<SA0> &a0, TVariant<SA1> &a1)
{ VC_Fn2<R,A0,A1,SR,SA0,SA1>()(f, r, a0, a1); }

template<class R, class A0, class A1, size_t SR, size_t SA>
inline void VariantCall(R (*f)(A0,A1), TVariant<SR> *r, const TVariant<SA> *va)
{ VC_Fn2<R,A0,A1,SR,SA,SA>()(f, r, va[0], va[1]); }

template<class R, class A0, class A1, size_t SA0, size_t SA1>
inline void VariantCall(R (*f)(A0,A1), TVariant<SA0> &a0, TVariant<SA1> &a1)
{ VC_Fn2<R,A0,A1,4,SA0,SA1>()(f, NULL, a0, a1); }

template<class R, class A0, class A1, size_t SA>
inline void VariantCall(R (*f)(A0,A1), const TVariant<SA> *va)
{ VC_Fn2<R,A0,A1,4,SA,SA>()(f, NULL, va[0], va[1]); }


template<class R, class C, class A0, class A1, size_t SR, size_t SA0, size_t SA1>
inline void VariantCall(R (C::*f)(A0,A1), C *o, TVariant<SR> *r, TVariant<SA0> &a0, TVariant<SA1> &a1)
{ VC_MemFn2<R,C,A0,A1,SR,SA0,SA1>()(f, o, r, a0, a1); }

template<class R, class C, class A0, class A1, size_t SR, size_t SA>
inline void VariantCall(R (C::*f)(A0,A1), C *o, TVariant<SR> *r, const TVariant<SA> *va)
{ VC_MemFn2<R,C,A0,A1,SR,SA,SA>()(f, o, r, va[0], va[1]); }

template<class R, class C, class A0, class A1, size_t SA0, size_t SA1>
inline void VariantCall(R (C::*f)(A0,A1), C *o, TVariant<SA0> &a0, TVariant<SA1> &a1)
{ VC_MemFn2<R,C,A0,A1,4,SA0,SA1>()(f, o, NULL, a0, a1); }

template<class R, class C, class A0, class A1, size_t SA>
inline void VariantCall(R (C::*f)(A0,A1), C *o, const TVariant<SA> *va)
{ VC_MemFn2<R,C,A0,A1,4,SA,SA>()(f, o, NULL, va[0], va[1]); }


template<class R, class C, class A0, class A1, size_t SR, size_t SA0, size_t SA1>
inline void VariantCall(R (C::*f)(A0,A1) const, const C *o, TVariant<SR> *r, TVariant<SA0> &a0, TVariant<SA1> &a1)
{ VC_ConstMemFn2<R,C,A0,A1,SR,SA0,SA1>()(f, o, r, a0, a1); }

template<class R, class C, class A0, class A1, size_t SR, size_t SA>
inline void VariantCall(R (C::*f)(A0,A1) const, const C *o, TVariant<SR> *r, const TVariant<SA> *va)
{ VC_ConstMemFn2<R,C,A0,A1,SR,SA,SA>()(f, o, r, va[0], va[1]); }

template<class R, class C, class A0, class A1, size_t SA0, size_t SA1>
inline void VariantCall(R (C::*f)(A0,A1) const, const C *o, TVariant<SA0> &a0, TVariant<SA1> &a1)
{ VC_ConstMemFn2<R,C,A0,A1,4,SA0,SA1>()(f, o, NULL, a0, a1); }

template<class R, class C, class A0, class A1, size_t SA>
inline void VariantCall(R (C::*f)(A0,A1) const, const C *o, const TVariant<SA> *va)
{ VC_ConstMemFn2<R,C,A0,A1,4,SA,SA>()(f, o, NULL, va[0], va[1]); }



template<class R, class A0, class A1, class A2, size_t SR, size_t SA0, size_t SA1, size_t SA2>
inline void VariantCall(R (*f)(A0,A1,A2), TVariant<SR> *r, TVariant<SA0> &a0, TVariant<SA1> &a1, TVariant<SA2> &a2)
{ VC_Fn3<R,A0,A1,A2,SR,SA0,SA1,SA2>()(f, r, a0, a1, a2); }

template<class R, class A0, class A1, class A2, size_t SR, size_t SA>
inline void VariantCall(R (*f)(A0,A1,A2), TVariant<SR> *r, const TVariant<SA> *va)
{ VC_Fn3<R,A0,A1,A2,SR,SA,SA,SA>()(f, r, va[0], va[1], va[2]); }

template<class R, class A0, class A1, class A2, size_t SA0, size_t SA1, size_t SA2>
inline void VariantCall(R (*f)(A0,A1,A2), TVariant<SA0> &a0, TVariant<SA1> &a1, TVariant<SA2> &a2)
{ VC_Fn3<R,A0,A1,A2,4,SA0,SA1,SA2>()(f, NULL, a0, a1, a2); }

template<class R, class A0, class A1, class A2, size_t SA>
inline void VariantCall(R (*f)(A0,A1,A2), const TVariant<SA> *va)
{ VC_Fn3<R,A0,A1,A2,4,SA,SA,SA>()(f, NULL, va[0], va[1], va[2]); }


template<class R, class C, class A0, class A1, class A2, size_t SR, size_t SA0, size_t SA1, size_t SA2>
inline void VariantCall(R (C::*f)(A0,A1,A2), C *o, TVariant<SR> *r, TVariant<SA0> &a0, TVariant<SA1> &a1, TVariant<SA2> &a2)
{ VC_MemFn3<R,C,A0,A1,A2,SR,SA0,SA1,SA2>()(f, o, r, a0, a1, a2); }

template<class R, class C, class A0, class A1, class A2, size_t SR, size_t SA>
inline void VariantCall(R (C::*f)(A0,A1,A2), C *o, TVariant<SR> *r, const TVariant<SA> *va)
{ VC_MemFn3<R,C,A0,A1,A2,SR,SA,SA,SA>()(f, o, r, va[0], va[1], va[2]); }

template<class R, class C, class A0, class A1, class A2, size_t SA0, size_t SA1, size_t SA2>
inline void VariantCall(R (C::*f)(A0,A1,A2), C *o, TVariant<SA0> &a0, TVariant<SA1> &a1, TVariant<SA2> &a2)
{ VC_MemFn3<R,C,A0,A1,A2,4,SA0,SA1,SA2>()(f, o, NULL, a0, a1, a2); }

template<class R, class C, class A0, class A1, class A2, size_t SA>
inline void VariantCall(R (C::*f)(A0,A1,A2), C *o, const TVariant<SA> *va)
{ VC_MemFn3<R,C,A0,A1,A2,4,SA,SA,SA>()(f, o, NULL, va[0], va[1], va[2]); }


template<class R, class C, class A0, class A1, class A2, size_t SR, size_t SA0, size_t SA1, size_t SA2>
inline void VariantCall(R (C::*f)(A0,A1,A2) const, const C *o, TVariant<SR> *r, TVariant<SA0> &a0, TVariant<SA1> &a1, TVariant<SA2> &a2)
{ VC_ConstMemFn3<R,C,A0,A1,A2,SR,SA0,SA1,SA2>()(f, o, r, a0, a1, a2); }

template<class R, class C, class A0, class A1, class A2, size_t SR, size_t SA>
inline void VariantCall(R (C::*f)(A0,A1,A2) const, const C *o, TVariant<SR> *r, const TVariant<SA> *va)
{ VC_ConstMemFn3<R,C,A0,A1,A2,SR,SA,SA,SA>()(f, o, r, va[0], va[1], va[2]); }

template<class R, class C, class A0, class A1, class A2, size_t SA0, size_t SA1, size_t SA2>
inline void VariantCall(R (C::*f)(A0,A1,A2) const, const C *o, TVariant<SA0> &a0, TVariant<SA1> &a1, TVariant<SA2> &a2)
{ VC_ConstMemFn3<R,C,A0,A1,A2,4,SA0,SA1,SA2>()(f, o, NULL, a0, a1, a2); }

template<class R, class C, class A0, class A1, class A2, size_t SA>
inline void VariantCall(R (C::*f)(A0,A1,A2) const, const C *o, const TVariant<SA> *va)
{ VC_ConstMemFn3<R,C,A0,A1,A2,4,SA,SA,SA>()(f, o, NULL, va[0], va[1], va[2]); }



template<class R, class A0, class A1, class A2, class A3, size_t SR, size_t SA0, size_t SA1, size_t SA2, size_t SA3>
inline void VariantCall(R (*f)(A0,A1,A2,A3), TVariant<SR> *r, TVariant<SA0> &a0, TVariant<SA1> &a1, TVariant<SA2> &a2, TVariant<SA3> &a3)
{ VC_Fn4<R,A0,A1,A2,A3,SR,SA0,SA1,SA2,SA3>()(f, r, a0, a1, a2, a3); }

template<class R, class A0, class A1, class A2, class A3, size_t SR, size_t SA>
inline void VariantCall(R (*f)(A0,A1,A2,A3), TVariant<SR> *r, const TVariant<SA> *va)
{ VC_Fn4<R,A0,A1,A2,A3,SR,SA,SA,SA,SA>()(f, r, va[0], va[1], va[2], va[3]); }

template<class R, class A0, class A1, class A2, class A3, size_t SA0, size_t SA1, size_t SA2, size_t SA3>
inline void VariantCall(R (*f)(A0,A1,A2,A3), TVariant<SA0> &a0, TVariant<SA1> &a1, TVariant<SA2> &a2, TVariant<SA3> &a3)
{ VC_Fn4<R,A0,A1,A2,A3,4,SA0,SA1,SA2,SA3>()(f, NULL, a0, a1, a2, a3); }

template<class R, class A0, class A1, class A2, class A3, size_t SA>
inline void VariantCall(R (*f)(A0,A1,A2,A3), const TVariant<SA> *va)
{ VC_Fn4<R,A0,A1,A2,A3,4,SA,SA,SA,SA>()(f, NULL, va[0], va[1], va[2], va[3]); }


template<class R, class C, class A0, class A1, class A2, class A3, size_t SR, size_t SA0, size_t SA1, size_t SA2, size_t SA3>
inline void VariantCall(R (C::*f)(A0,A1,A2,A3), C *o, TVariant<SR> *r, TVariant<SA0> &a0, TVariant<SA1> &a1, TVariant<SA2> &a2, TVariant<SA3> &a3)
{ VC_MemFn4<R,C,A0,A1,A2,A3,SR,SA0,SA1,SA2,SA3>()(f, o, r, a0, a1, a2, a3); }

template<class R, class C, class A0, class A1, class A2, class A3, size_t SR, size_t SA>
inline void VariantCall(R (C::*f)(A0,A1,A2,A3), C *o, TVariant<SR> *r, const TVariant<SA> *va)
{ VC_MemFn4<R,C,A0,A1,A2,A3,SR,SA,SA,SA,SA>()(f, o, r, va[0], va[1], va[2], va[3]); }

template<class R, class C, class A0, class A1, class A2, class A3, size_t SA0, size_t SA1, size_t SA2, size_t SA3>
inline void VariantCall(R (C::*f)(A0,A1,A2,A3), C *o, TVariant<SA0> &a0, TVariant<SA1> &a1, TVariant<SA2> &a2, TVariant<SA3> &a3)
{ VC_MemFn4<R,C,A0,A1,A2,A3,4,SA0,SA1,SA2,SA3>()(f, o, NULL, a0, a1, a2, a3); }

template<class R, class C, class A0, class A1, class A2, class A3, size_t SA>
inline void VariantCall(R (C::*f)(A0,A1,A2,A3), C *o, const TVariant<SA> *va)
{ VC_MemFn4<R,C,A0,A1,A2,A3,4,SA,SA,SA,SA>()(f, o, NULL, va[0], va[1], va[2], va[3]); }


template<class R, class C, class A0, class A1, class A2, class A3, size_t SR, size_t SA0, size_t SA1, size_t SA2, size_t SA3>
inline void VariantCall(R (C::*f)(A0,A1,A2,A3) const, const C *o, TVariant<SR> *r, TVariant<SA0> &a0, TVariant<SA1> &a1, TVariant<SA2> &a2, TVariant<SA3> &a3)
{ VC_ConstMemFn4<R,C,A0,A1,A2,A3,SR,SA0,SA1,SA2,SA3>()(f, o, r, a0, a1, a2, a3); }

template<class R, class C, class A0, class A1, class A2, class A3, size_t SR, size_t SA>
inline void VariantCall(R (C::*f)(A0,A1,A2,A3) const, const C *o, TVariant<SR> *r, const TVariant<SA> *va)
{ VC_ConstMemFn4<R,C,A0,A1,A2,A3,SR,SA,SA,SA,SA>()(f, o, r, va[0], va[1], va[2], va[3]); }

template<class R, class C, class A0, class A1, class A2, class A3, size_t SA0, size_t SA1, size_t SA2, size_t SA3>
inline void VariantCall(R (C::*f)(A0,A1,A2,A3) const, const C *o, TVariant<SA0> &a0, TVariant<SA1> &a1, TVariant<SA2> &a2, TVariant<SA3> &a3)
{ VC_ConstMemFn4<R,C,A0,A1,A2,A3,4,SA0,SA1,SA2,SA3>()(f, o, NULL, a0, a1, a2, a3); }

template<class R, class C, class A0, class A1, class A2, class A3, size_t SA>
inline void VariantCall(R (C::*f)(A0,A1,A2,A3) const, const C *o, const TVariant<SA> *va)
{ VC_ConstMemFn4<R,C,A0,A1,A2,A3,4,SA,SA,SA,SA>()(f, o, NULL, va[0], va[1], va[2], va[3]); }

