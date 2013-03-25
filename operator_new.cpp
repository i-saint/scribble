#include <cstdio>
#include <cstdlib>

class HogeBase
{
public:
    virtual ~HogeBase() {}

    static void* operator new(size_t size)
    {
        printf("HogeBase::operator new(%u)\n", size);
        return ::malloc(size);
    }
    static void operator delete(void *p)
    {
        printf("HogeBase::operator delete(%p)\n", p);
        ::free(p);
    }

private:
    char m_data[64];
};

class Hoge : public HogeBase
{
public:
    virtual ~Hoge() {}

    static void* operator new(size_t size)
    {
        printf("Hoge::operator new(%u)\n", size);
        return ::malloc(size);
    }
    static void operator delete(void *p)
    {
        printf("Hoge::operator delete(%p)\n", p);
        ::free(p);
    }

private:
    char m_data2[64];
};


int main()
{
    HogeBase *h1 = new HogeBase();
    delete h1;

    HogeBase *h2 = new Hoge();
    delete h2;

    Hoge     *h3 = new Hoge();
    delete h3;
}

/*
$ cl operator_new.cpp
$ ./operator_new
HogeBase::operator new(68)
HogeBase::operator delete(00468488)
Hoge::operator new(132)
Hoge::operator delete(00468488)
Hoge::operator new(132)
Hoge::operator delete(00468488)
*/
