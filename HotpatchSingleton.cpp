// ↓でコンパイル
// cl HotpatchSingleton.cpp /hotpatch /link /functionpadmin

#include <cstdio>
#include <windows.h>

class Singleton
{
public:
    void hoge()
    {
        printf("Singleton::Hoge()\n");
    }

private:
    Singleton()
    {
        printf("Singleton::Singleton()\n");
    }

    static Singleton *s_instance;

public:
    static __declspec(noinline) Singleton* getInstance();
    static __declspec(noinline) Singleton* getInstance_NoCheck();
};

Singleton* Singleton::s_instance = NULL;

Singleton* Singleton::getInstance()
{
    if(s_instance==NULL) {
        s_instance = new Singleton();

        // Singleton::getInstance() の先頭を書き換えて getInstance_NoCheck() にリダイレクトさせる
        DWORD old;
        BYTE *f = (BYTE*)&getInstance;
        ::VirtualProtect(f-5, 7, PAGE_EXECUTE_READWRITE, &old);
        f[-5]=0xE9;
        *((int*)(f-4)) = (int)((int)&getInstance_NoCheck-(int)f);
        f[0]=0xEB; f[1]=0xF9;
        ::VirtualProtect(f-5, 7, old, &old);
    }
    printf("Singleton::getInstance()()\n");
    return s_instance;
}

Singleton* Singleton::getInstance_NoCheck()
{
    printf("Singleton::getInstance_NoCheck()()\n");
    return s_instance;
}

int main(int argc, char *argv[])
{
    for(int i=0; i<5; ++i) {
        Singleton::getInstance()->hoge();
    }
	char c;
	scanf("%c", &c);
}

/*
result:

Singleton::Singleton()
Singleton::getInstance()()
Singleton::Hoge()
Singleton::getInstance_NoCheck()()
Singleton::Hoge()
Singleton::getInstance_NoCheck()()
Singleton::Hoge()
Singleton::getInstance_NoCheck()()
Singleton::Hoge()
Singleton::getInstance_NoCheck()()
Singleton::Hoge()
*/
