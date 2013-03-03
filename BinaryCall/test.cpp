#include <cassert>
#include "BinaryCall.h"


int Func0() { return 1; }
int Func2(int a, int b) { return a*b; }

struct Test
{
    int value;
    void MemFn0() { value=2; }
    int MemFn2(int a, int b) { return a*b*2; }
    int ConstMemFn0() const { return 3; }
    int ConstMemFn2(int a, int b) const { return a*b*4; }
};

int main(int argc, char *argv[])
{
    Test obj;
    int ret;
    ArgList<int,int> args = {2, 4};

    BinaryCall(Func0, &ret);
    assert(ret==1);

    BinaryCall(Func2, &ret, &args);
    assert(ret==8);

    BinaryCall(&Test::MemFn0, obj, &ret);
    assert(obj.value==2);

    BinaryCall(&Test::MemFn2, obj, &ret, &args);
    assert(ret==16);

    BinaryCall(&Test::ConstMemFn0, obj, &ret);
    assert(ret==3);

    BinaryCall(&Test::ConstMemFn2, obj, &ret, &args);
    assert(ret==32);
}

