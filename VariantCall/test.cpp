#include <cassert>
#include "VariantCall.h"


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
    typedef variant16 var;

    Test obj;
    var ret;
    var args[2] = {int32(2), int(4)};
    
    VariantCall(Func0, ret);
    assert(ret.cast<int32>()==1);

    VariantCall(Func2, ret, args);
    assert(ret.cast<int32>()==8);

    VariantCall(&Test::MemFn0, obj);
    assert(obj.value==2);

    VariantCall(&Test::MemFn2, obj, ret, args);
    assert(ret.cast<int32>()==16);

    VariantCall(&Test::ConstMemFn0, obj, ret);
    assert(ret.cast<int32>()==3);

    VariantCall(&Test::ConstMemFn2, obj, ret, args);
    assert(ret.cast<int32>()==32);
}
