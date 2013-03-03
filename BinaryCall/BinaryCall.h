#ifndef BinaryCall_h
#define BinaryCall_h
#include <type_traits>

typedef char                int8;
typedef short               int16;
typedef int                 int32;
typedef long long           int64;
typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned long long  uint64;

#define RemoveCR(T) typename std::remove_const<typename std::remove_reference<T>::type>::type

template<class R>
struct BC_Fn0
{
    typedef R (*F)();
    typedef RemoveCR(R) RT;
    void operator()(F f, void *r, const void *a)
    {
        if(r) { *(RT*)r=f(); }
        else  {         f(); }
    }
};
template<>
struct BC_Fn0<void>
{
    typedef void (*F)();
    void operator()(F f, void *r, const void *a)
    {
        f();
    }
};

template<class R, class C>
struct BC_MemFn0
{
    typedef R (C::*F)();
    typedef RemoveCR(R) RT;
    void operator()(F f, C &o, void *r, const void *a)
    {
        if(r) { *(RT*)r=(o.*f)(); }
        else  {         (o.*f)(); }
    }
};
template<class C>
struct BC_MemFn0<void, C>
{
    typedef void (C::*F)();
    void operator()(F f, C &o, void *r, const void *a)
    {
        (o.*f)();
    }
};

template<class R, class C>
struct BC_ConstMemFn0
{
    typedef R (C::*F)() const;
    typedef RemoveCR(R) RT;
    void operator()(F f, const C &o, void *r, const void *a)
    {
        if(r) { *(RT*)r=(o.*f)(); }
        else  {         (o.*f)(); }
    }
};
template<class C>
struct BC_ConstMemFn0<void, C>
{
    typedef void (C::*F)() const;
    void operator()(F f, const C &o, void *r, const void *a)
    {
        (o.*f)();
    }
};


template<class R, class A0>
struct BC_Fn1
{
    typedef R (*F)(A0);
    typedef RemoveCR(R) RT;
    typedef RemoveCR(A0) A0T;
    struct Args {
        A0T a0;
    };
    void operator()(F f, void *r, const void *a)
    {
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=f(args.a0); }
        else  {         f(args.a0); }
    }
};
template<class A0>
struct BC_Fn1<void, A0>
{
    typedef void (*F)(A0);
    typedef RemoveCR(A0) A0T;
    struct Args {
        A0T a0;
    };
    void operator()(F f, void *r, const void *a)
    {
        Args &args = *(Args*)a;
        f(args.a0);
    }
};

template<class R, class C, class A0>
struct BC_MemFn1
{
    typedef R (C::*F)(A0);
    typedef RemoveCR(R) RT;
    typedef RemoveCR(A0) A0T;
    struct Args {
        A0T a0;
    };
    void operator()(F f, C &o, void *r, const void *a)
    {
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=(o.*f)(args.a0); }
        else  {         (o.*f)(args.a0); }
    }
};
template<class C, class A0>
struct BC_MemFn1<void, C, A0>
{
    typedef void (C::*F)(A0);
    typedef RemoveCR(A0) A0T;
    struct Args {
        A0T a0;
    };
    void operator()(F f, C &o, void *r, const void *a)
    {
        Args &args = *(Args*)a;
        (o.*f)(args.a0);
    }
};

template<class R, class C, class A0>
struct BC_ConstMemFn1
{
    typedef R (C::*F)(A0) const;
    typedef RemoveCR(R) RT;
    typedef RemoveCR(A0) A0T;
    struct Args {
        A0T a0;
    };
    void operator()(F f, const C &o, void *r, const void *a)
    {
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=(o.*f)(args.a0); }
        else  {         (o.*f)(args.a0); }
    }
};
template<class C, class A0>
struct BC_ConstMemFn1<void, C, A0>
{
    typedef void (C::*F)(A0) const;
    typedef RemoveCR(A0) A0T;
    struct Args {
        A0T a0;
    };
    void operator()(F f, const C &o, void *r, const void *a)
    {
        Args &args = *(Args*)a;
        (o.*f)(args.a0);
    }
};

template<class R, class A0, class A1>
struct BC_Fn2
{
    typedef R (*F)(A0, A1);
    typedef RemoveCR(R) RT;
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    struct Args {
        A0T a0;
        A1T a1;
    };
    void operator()(F f, void *r, const void *a)
    {
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=f(args.a0, args.a1); }
        else  {         f(args.a0, args.a1); }
    }
};
template<class A0, class A1>
struct BC_Fn2<void, A0, A1>
{
    typedef void (*F)(A0, A1);
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    struct Args {
        A0T a0;
        A1T a1;
    };
    void operator()(F f, void *r, const void *a)
    {
        Args &args = *(Args*)a;
        f(args.a0, args.a1);
    }
};

template<class R, class C, class A0, class A1>
struct BC_MemFn2
{
    typedef R (C::*F)(A0, A1);
    typedef RemoveCR(R) RT;
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    struct Args {
        A0T a0;
        A1T a1;
    };
    void operator()(F f, C &o, void *r, const void *a)
    {
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=(o.*f)(args.a0, args.a1); }
        else  {         (o.*f)(args.a0, args.a1); }
    }
};
template<class C, class A0, class A1>
struct BC_MemFn2<void, C, A0, A1>
{
    typedef void (C::*F)(A0, A1);
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    struct Args {
        A0T a0;
        A1T a1;
    };
    void operator()(F f, C &o, void *r, const void *a)
    {
        Args &args = *(Args*)a;
        (o.*f)(args.a0, args.a1);
    }
};

template<class R, class C, class A0, class A1>
struct BC_ConstMemFn2
{
    typedef R (C::*F)(A0, A1) const;
    typedef RemoveCR(R) RT;
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    struct Args {
        A0T a0;
        A1T a1;
    };
    void operator()(F f, const C &o, void *r, const void *a)
    {
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=(o.*f)(args.a0, args.a1); }
        else  {         (o.*f)(args.a0, args.a1); }
    }
};
template<class C, class A0, class A1>
struct BC_ConstMemFn2<void, C, A0, A1>
{
    typedef void (C::*F)(A0, A1) const;
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    struct Args {
        A0T a0;
        A1T a1;
    };
    void operator()(F f, const C &o, void *r, const void *a)
    {
        Args &args = *(Args*)a;
        (o.*f)(args.a0, args.a1);
    }
};

template<class R, class A0, class A1, class A2>
struct BC_Fn3
{
    typedef R (*F)(A0, A1, A2);
    typedef RemoveCR(R) RT;
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    typedef RemoveCR(A2) A2T;
    struct Args {
        A0T a0;
        A1T a1;
        A2T a2;
    };
    void operator()(F f, void *r, const void *a)
    {
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=f(args.a0, args.a1, args.a2); }
        else  {         f(args.a0, args.a1, args.a2); }
    }
};
template<class A0, class A1, class A2>
struct BC_Fn3<void, A0, A1, A2>
{
    typedef void (*F)(A0, A1, A2);
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    typedef RemoveCR(A2) A2T;
    struct Args {
        A0T a0;
        A1T a1;
        A2T a2;
    };
    void operator()(F f, void *r, const void *a)
    {
        Args &args = *(Args*)a;
        f(args.a0, args.a1, args.a2);
    }
};

template<class R, class C, class A0, class A1, class A2>
struct BC_MemFn3
{
    typedef R (C::*F)(A0, A1, A2);
    typedef RemoveCR(R) RT;
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    typedef RemoveCR(A2) A2T;
    struct Args {
        A0T a0;
        A1T a1;
        A2T a2;
    };
    void operator()(F f, C &o, void *r, const void *a)
    {
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=(o.*f)(args.a0, args.a1, args.a2); }
        else  {         (o.*f)(args.a0, args.a1, args.a2); }
    }
};
template<class C, class A0, class A1, class A2>
struct BC_MemFn3<void, C, A0, A1, A2>
{
    typedef void (C::*F)(A0, A1, A2);
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    typedef RemoveCR(A2) A2T;
    struct Args {
        A0T a0;
        A1T a1;
        A2T a2;
    };
    void operator()(F f, C &o, void *r, const void *a)
    {
        Args &args = *(Args*)a;
        (o.*f)(args.a0, args.a1, args.a2);
    }
};

template<class R, class C, class A0, class A1, class A2>
struct BC_ConstMemFn3
{
    typedef R (C::*F)(A0, A1, A2) const;
    typedef RemoveCR(R) RT;
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    typedef RemoveCR(A2) A2T;
    struct Args {
        A0T a0;
        A1T a1;
        A2T a2;
    };
    void operator()(F f, const C &o, void *r, const void *a)
    {
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=(o.*f)(args.a0, args.a1, args.a2); }
        else  {         (o.*f)(args.a0, args.a1, args.a2); }
    }
};
template<class C, class A0, class A1, class A2>
struct BC_ConstMemFn3<void, C, A0, A1, A2>
{
    typedef void (C::*F)(A0, A1, A2) const;
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    typedef RemoveCR(A2) A2T;
    struct Args {
        A0T a0;
        A1T a1;
        A2T a2;
    };
    void operator()(F f, const C &o, void *r, const void *a)
    {
        Args &args = *(Args*)a;
        (o.*f)(args.a0, args.a1, args.a2);
    }
};

template<class R, class A0, class A1, class A2, class A3>
struct BC_Fn4
{
    typedef R (*F)(A0, A1, A2, A3);
    typedef RemoveCR(R) RT;
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    typedef RemoveCR(A2) A2T;
    typedef RemoveCR(A3) A3T;
    struct Args {
        A0T a0;
        A1T a1;
        A2T a2;
        A3T a3;
    };
    void operator()(F f, void *r, const void *a)
    {
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=f(args.a0, args.a1, args.a2, args.a3); }
        else  {         f(args.a0, args.a1, args.a2, args.a3); }
    }
};
template<class A0, class A1, class A2, class A3>
struct BC_Fn4<void, A0, A1, A2, A3>
{
    typedef void (*F)(A0, A1, A2, A3);
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    typedef RemoveCR(A2) A2T;
    typedef RemoveCR(A3) A3T;
    struct Args {
        A0T a0;
        A1T a1;
        A2T a2;
        A3T a3;
    };
    void operator()(F f, void *r, const void *a)
    {
        Args &args = *(Args*)a;
        f(args.a0, args.a1, args.a2, args.a3);
    }
};

template<class R, class C, class A0, class A1, class A2, class A3>
struct BC_MemFn4
{
    typedef R (C::*F)(A0, A1, A2, A3);
    typedef RemoveCR(R) RT;
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    typedef RemoveCR(A2) A2T;
    typedef RemoveCR(A3) A3T;
    struct Args {
        A0T a0;
        A1T a1;
        A2T a2;
        A3T a3;
    };
    void operator()(F f, C &o, void *r, const void *a)
    {
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=(o.*f)(args.a0, args.a1, args.a2, args.a3); }
        else  {         (o.*f)(args.a0, args.a1, args.a2, args.a3); }
    }
};
template<class C, class A0, class A1, class A2, class A3>
struct BC_MemFn4<void, C, A0, A1, A2, A3>
{
    typedef void (C::*F)(A0, A1, A2, A3);
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    typedef RemoveCR(A2) A2T;
    typedef RemoveCR(A3) A3T;
    struct Args {
        A0T a0;
        A1T a1;
        A2T a2;
        A3T a3;
    };
    void operator()(F f, C &o, void *r, const void *a)
    {
        Args &args = *(Args*)a;
        (o.*f)(args.a0, args.a1, args.a2, args.a3);
    }
};

template<class R, class C, class A0, class A1, class A2, class A3>
struct BC_ConstMemFn4
{
    typedef R (C::*F)(A0, A1, A2, A3) const;
    typedef RemoveCR(R) RT;
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    typedef RemoveCR(A2) A2T;
    typedef RemoveCR(A3) A3T;
    struct Args {
        A0T a0;
        A1T a1;
        A2T a2;
        A3T a3;
    };
    void operator()(F f, const C &o, void *r, const void *a)
    {
        Args &args = *(Args*)a;
        if(r) { *(RT*)r=(o.*f)(args.a0, args.a1, args.a2, args.a3); }
        else  {         (o.*f)(args.a0, args.a1, args.a2, args.a3); }
    }
};
template<class C, class A0, class A1, class A2, class A3>
struct BC_ConstMemFn4<void, C, A0, A1, A2, A3>
{
    typedef void (C::*F)(A0, A1, A2, A3) const;
    typedef RemoveCR(A0) A0T;
    typedef RemoveCR(A1) A1T;
    typedef RemoveCR(A2) A2T;
    typedef RemoveCR(A3) A3T;
    struct Args {
        A0T a0;
        A1T a1;
        A2T a2;
        A3T a3;
    };
    void operator()(F f, const C &o, void *r, const void *a)
    {
        Args &args = *(Args*)a;
        (o.*f)(args.a0, args.a1, args.a2, args.a3);
    }
};

#undef RemoveCR


template<class R>
inline void BinaryCall(R (*f)(), void *r, const void *a=NULL)
{ BC_Fn0<R>()(f, r, a); }

template<class R, class C>
inline void BinaryCall(R (C::*f)(), C &o, void *r, const void *a=NULL)
{ BC_MemFn0<R,C>()(f, o, r, a); }

template<class R, class C>
inline void BinaryCall(R (C::*f)() const, const C &o, void *r, const void *a=NULL)
{ BC_ConstMemFn0<R,C>()(f, o, r, a); }


template<class R, class A0>
inline void BinaryCall(R (*f)(A0), void *r, const void *a)
{ BC_Fn1<R,A0>()(f, r, a); }

template<class R, class C, class A0>
inline void BinaryCall(R (C::*f)(A0), C &o, void *r, const void *a)
{ BC_MemFn1<R,C,A0>()(f, o, r, a); }

template<class R, class C, class A0>
inline void BinaryCall(R (C::*f)(A0) const, const C &o, void *r, const void *a)
{ BC_ConstMemFn1<R,C,A0>()(f, o, r, a); }


template<class R, class A0, class A1>
inline void BinaryCall(R (*f)(A0,A1), void *r, const void *a)
{ BC_Fn2<R,A0,A1>()(f, r, a); }

template<class R, class C, class A0, class A1>
inline void BinaryCall(R (C::*f)(A0,A1), C &o, void *r, const void *a)
{ BC_MemFn2<R,C,A0,A1>()(f, o, r, a); }

template<class R, class C, class A0, class A1>
inline void BinaryCall(R (C::*f)(A0,A1) const, const C &o, void *r, const void *a)
{ BC_ConstMemFn2<R,C,A0,A1>()(f, o, r, a); }


template<class R, class A0, class A1, class A2>
inline void BinaryCall(R (*f)(A0,A1,A2), void *r, const void *a)
{ BC_Fn3<R,A0,A1,A2>()(f, r, a); }

template<class R, class C, class A0, class A1, class A2>
inline void BinaryCall(R (C::*f)(A0,A1,A2), C &o, void *r, const void *a)
{ BC_MemFn3<R,C,A0,A1,A2>()(f, o, r, a); }

template<class R, class C, class A0, class A1, class A2>
inline void BinaryCall(R (C::*f)(A0,A1,A2) const, const C &o, void *r, const void *a)
{ BC_ConstMemFn3<R,C,A0,A1,A2>()(f, o, r, a); }


template<class R, class A0, class A1, class A2, class A3>
inline void BinaryCall(R (*f)(A0,A1,A2,A3), void *r, const void *a)
{ BC_Fn4<R,A0,A1,A2,A3>()(f, r, a); }

template<class R, class C, class A0, class A1, class A2, class A3>
inline void BinaryCall(R (C::*f)(A0,A1,A2,A3), C &o, void *r, const void *a)
{ BC_MemFn4<R,C,A0,A1,A2,A3>()(f, o, r, a); }

template<class R, class C, class A0, class A1, class A2, class A3>
inline void BinaryCall(R (C::*f)(A0,A1,A2,A3) const, const C &o, void *r, const void *a)
{ BC_ConstMemFn4<R,C,A0,A1,A2,A3>()(f, o, r, a); }


#endif // BinaryCall_h
