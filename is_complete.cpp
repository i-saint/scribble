#include <cstdio>

template<class T>
struct is_complete
{
// VisualC++ only
// other approach: http://stackoverflow.com/questions/1625105/how-to-write-is-complete-template/1625360
static const bool value = __alignof(T)!=0;
};

class Hoge;
class Hage {};


int main()
{
    printf("is_complete<Hoge>: %d\n", is_complete<Hoge>::value);
    printf("is_complete<Hage>: %d\n", is_complete<Hage>::value);
}

// $ cl is_complete.cpp && ./is_complete
// is_complete<Hoge>: 0
// is_complete<Hage>: 1
