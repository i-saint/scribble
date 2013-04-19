#ifndef __INTERMODULESINGLETON_H__
#define __INTERMODULESINGLETON_H__

#ifdef IMPLEMENT_MODULE
    #define INTERMODULE __declspec(dllexport)

    #define IMPLEMENT_INSTANCE(T)\
        T* IntermoduleSingleton<T>::GetInstance()\
        {\
            static T s_instance;\
            return &s_instance;\
        }

#else
    #define INTERMODULE __declspec(dllimport)

#endif


template<class T>
class INTERMODULE IntermoduleSingleton
{
public:
    static T* GetInstance();
};

#endif __INTERMODULESINGLETON_H__
