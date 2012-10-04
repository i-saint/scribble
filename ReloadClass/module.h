#define DECLARE_MODULE_MEMFUN(Ret, Class, Name, Args)\
    Ret MODULE_API Name##Impl##Args;\
    Ret Name##Args;

#ifdef IMPLEMENT_MODULE
#   define MODULE_API __declspec(dllexport)
#   define DEFINE_MODULE_MEMFUN(Ret, Class, Name, Args)\
        extern "C" MODULE_API void (Class::*g_##Class##_##Name)() = &Class::Name##Impl;\
        Ret Class::Name##Impl##Args
#else
#   define MODULE_API __declspec(dllimport)
#   define DEFINE_MAIN_MEMFUN(Ret, Class, Name, Args)\
        Ret (Class::*g_##Class##_##Name)() = &Class::Name##Impl;\
        const char g_##Class##_##Name##_Name[] = "g_" #Class "_" #Name;\
        Ret Class::Name##Args { (this->*g_##Class##_##Name)(); }
#endif



class Hoge
{
public:
    Hoge();
    ~Hoge();

    DECLARE_MODULE_MEMFUN(void, Hoge, doSomething, ());

private:
    int m_data;
};

