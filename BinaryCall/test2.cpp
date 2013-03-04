#include <cassert>
#include "BinaryCall.h"


#define ImplCallBlock(methods) \
    virtual bool call(FunctionID fid, const void *args, void *ret)\
    {\
        switch(fid) { methods }\
        return false;\
    }

#define ImplCall(funcname) \
    case FID_##funcname: BinaryCall(&this_t::funcname, *this, ret, args); return true;

class ICallable;
enum FunctionID;
template<class Arg> inline bool CallImpl(ICallable *e, FunctionID fid, const Arg &args) { return e->call(fid, &args, NULL); }
template<class Arg, class Ret> inline bool CallImpl(ICallable *e, FunctionID fid, const Arg &args, Ret &ret) { return e->call(fid, &args, &ret); }
#define Call(obj, funcname, ...) CallImpl(obj, FID_##funcname, __VA_ARGS__)


enum FunctionID {
    FID_doSomething,
};

class ICallable
{
public:
    virtual bool call(FunctionID fid, const void *args, void *ret=NULL)=0;
};

class Test : public ICallable
{
typedef Test this_t;
public:
    int doSomething(int a) const { return a*10; }

    ImplCallBlock(
        ImplCall(doSomething)
    )
};


int main()
{
    ICallable *test = new Test();
    int arg = 10;
    int ret;

    Call(test, doSomething, arg, ret);
    assert(ret==100);

    delete test;
}

