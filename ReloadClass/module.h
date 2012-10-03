#ifdef IMPLEMENT_MODULE
#   define MODULE_API __declspec(dllexport)
#else
#   define MODULE_API __declspec(dllimport)
#endif


class Hoge
{
public:
    Hoge();
    ~Hoge();

    void MODULE_API doSomething();

private:
    int m_data;
};

