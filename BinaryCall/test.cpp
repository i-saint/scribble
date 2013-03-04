#include <cassert>
#include "BinaryCall.h"


int Func0() { return 1; }
int Func2(int a, int b) { return a*b; }

struct Int4 {
    int v[4];
    int& operator[](int i) { return v[i]; }
    const int& operator[](int i) const { return v[i]; }
};

struct Test
{
    int value;
    Int4 i4;
    void MemFn0() { value=2; }
    int MemFn2(int a, int b) { return a*b*2; }
    int ConstMemFn0() const { return 3; }
    int ConstMemFn2(int a, int b) const { return a*b*4; }
    const Int4& TestRef2(const Int4& iv1, const Int4& iv2) {
        for(int i=0; i<4; ++i) { i4[i]=iv1[i]*iv2[i]; }
        return i4;
    }
};

int main(int argc, char *argv[])
{
    Test obj;
    {
        int ret;
        ArgList<int,int> args(2, 4);
        BinaryCall(Func0, &ret);
        assert(ret==1);

        BinaryCall(Func2, &ret, &args);
        assert(ret==8);

        BinaryCall(&Test::MemFn0, obj, NULL, NULL);
        assert(obj.value==2);

        BinaryCall(&Test::MemFn2, obj, &ret, &args);
        assert(ret==16);

        BinaryCall(&Test::ConstMemFn0, obj, &ret, NULL);
        assert(ret==3);

        BinaryCall(&Test::ConstMemFn2, obj, &ret, &args);
        assert(ret==32);
    }
    {
        Int4 fv1 = {{1, 2, 3, 4}};
        Int4 fv2 = {{5, 6, 7, 8}};

        ArgList<const Int4&, const Int4&> args(fv1, fv2);
        ArgHolder<const Int4&> ret;
        BinaryCallRef(&Test::TestRef2, obj, &ret, &args);

        const Int4 &r = ret;
        assert(r[0]==5);
        assert(r[1]==12);
        assert(r[2]==21);
        assert(r[3]==32);
    }
}

