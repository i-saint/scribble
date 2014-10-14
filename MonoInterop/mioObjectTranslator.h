#ifndef mioObjectTranslator_h
#define mioObjectTranslator_h
#include "mioTypes.h"


template<class T> struct mioObjctTranslatorImpl;

template<class T> inline const T& mioToCppObject(mioObject &o) { return mioObjctTranslatorImpl<T>().toCpp(o); }
template<class T> inline mioObject mioToCsObject(const T &o) { return mioObjctTranslatorImpl<T>().toCs(o); }


// pointers
template<class T> struct mioObjctTranslatorImpl < T* >
{
    T*& toCpp(mioObject &o) { return (T*&)o; }
    mioObject toCs(const T *&o) { return (mioObject&)o; }
};

// int
template<> struct mioObjctTranslatorImpl <int>
{
    int& toCpp(mioObject &o) { return (int&)o; }
    mioObject toCs(const int &o) { return (mioObject&)o; }
};

#endif // mioObjectTranslator_h
