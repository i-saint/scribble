#ifndef ArgHolder_h
#define ArgHolder_h
#include <type_traits>


template<class T>
struct ValueHolder
{
public:
    typedef typename std::remove_const<typename std::remove_reference<T>::type>::type value_type;
    ValueHolder() : m_value() {}
    ValueHolder(const value_type &v) : m_value(v) {}
    ValueHolder& operator=(value_type v) { m_value=v; return *this; }
    operator value_type&() { return m_value; }
    operator const value_type&() const { return m_value; }
private:
    value_type m_value;
};


template<class T>
struct ArgHolder
{
public:
    typedef T value_type;
    ArgHolder() : m_value() {}
    ArgHolder(value_type v) : m_value(v) {}
    ArgHolder& operator=(value_type v) { m_value=v; return *this; }
    operator value_type&() const { return m_value; }
    T m_value;
};
template<class T>
struct ArgHolder<const T>
{
public:
    typedef T value_type;
    ArgHolder() : m_value() {}
    ArgHolder(const value_type v) : m_value(v) {}
    ArgHolder& operator=(value_type v) { m_value=v; return *this; }
    operator value_type() { return m_value; }
    operator const value_type() const { return m_value; }
    value_type m_value;
};
template<class T>
struct ArgHolder<T&>
{
public:
    typedef T value_type;
    ArgHolder() : m_value(NULL) {}
    ArgHolder(value_type &v) : m_value(v) {}
    ArgHolder& operator=(value_type &v) { m_value=&v; return *this; }
    operator value_type&() { return *m_value; }
    operator const value_type&() const { return *m_value; }
    value_type *m_value;
};
template<class T>
struct ArgHolder<const T&>
{
public:
    typedef T value_type;
    ArgHolder() : m_value(NULL) {}
    ArgHolder(const value_type &v) : m_value(&v) {}
    ArgHolder& operator=(const value_type &v) { m_value=&v; return *this; }
    operator const value_type&() const { return *m_value; }
    const value_type *m_value;
};


template<class A0=void, class A1=void, class A2=void, class A3=void, class A4=void, class A5=void>
struct ValueList;

template<>
struct ValueList<>
{
    ValueList() {}
};
template<class A0>
struct ValueList<A0>
{
    ValueHolder<A0> a0;
    ValueList(
        A0 _0=ValueHolder<A0>::value_type()
        ) : a0(_0) {}
};
template<class A0, class A1>
struct ValueList<A0, A1>
{
    ValueHolder<A0> a0;
    ValueHolder<A1> a1;
    ValueList(
        A0 _0=ValueHolder<A0>::value_type(),
        A1 _1=ValueHolder<A1>::value_type()
        ) : a0(_0), a1(_1) {}
};
template<class A0, class A1, class A2>
struct ValueList<A0, A1, A2>
{
    ValueHolder<A0> a0;
    ValueHolder<A1> a1;
    ValueHolder<A2> a2;
    ValueList(
        A0 _0=ValueHolder<A0>::value_type(),
        A1 _1=ValueHolder<A0>::value_type(),
        A2 _2=ValueHolder<A0>::value_type()
        ) : a0(_0), a1(_1), a2(_2) {}
};
template<class A0, class A1, class A2, class A3>
struct ValueList<A0, A1, A2, A3>
{
    ValueHolder<A0> a0;
    ValueHolder<A1> a1;
    ValueHolder<A2> a2;
    ValueHolder<A3> a3;
    ValueList(
        A0 _0=ValueHolder<A0>::value_type(),
        A1 _1=ValueHolder<A0>::value_type(),
        A2 _2=ValueHolder<A0>::value_type(),
        A3 _3=ValueHolder<A0>::value_type()
        ) : a0(_0), a1(_1), a2(_2), a3(_3) {}
};

template<class A0=void, class A1=void, class A2=void, class A3=void, class A4=void, class A5=void>
struct ArgList; // reference as pointer

template<>
struct ArgList<>
{
    ArgList() {}
};
template<class A0>
struct ArgList<A0>
{
    ArgHolder<A0> a0;
    ArgList() {}
    ArgList(A0 _0) : a0(_0) {}
};
template<class A0, class A1>
struct ArgList<A0, A1>
{
    ArgHolder<A0> a0;
    ArgHolder<A1> a1;
    ArgList() {}
    ArgList(A0 _0, A1 _1) : a0(_0), a1(_1) {}
};
template<class A0, class A1, class A2>
struct ArgList<A0, A1, A2>
{
    ArgHolder<A0> a0;
    ArgHolder<A1> a1;
    ArgHolder<A2> a2;
    ArgList() {}
    ArgList(A0 _0, A1 _1, A2 _2) : a0(_0), a1(_1), a2(_2) {}
};
template<class A0, class A1, class A2, class A3>
struct ArgList<A0, A1, A2, A3>
{
    ArgHolder<A0> a0;
    ArgHolder<A1> a1;
    ArgHolder<A2> a2;
    ArgHolder<A3> a3;
    ArgList() {}
    ArgList(A0 _0, A1 _1, A2 _2, A3 _3) : a0(_0), a1(_1), a2(_2), a3(_3) {}
};


#endif // ArgHolder_h
