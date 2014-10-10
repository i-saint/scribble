#ifndef mioObjectTranslator_h
#define mioObjectTranslator_h


template<class T> inline const T& mioToCppObject(mioObject &o);
template<class T> inline mioObject mioToCsObject(const T &o);

template<> inline const int& mioToCppObject<int>(mioObject &o) { return (int&)o; }
template<> inline mioObject mioToCsObject<int>(const int &o) { return nullptr; }

#endif // mioObjectTranslator_h
