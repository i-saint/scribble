#ifndef __Interface_h__
#define __Interface_h__

class IHoge
{
public:
    virtual ~IHoge() {}
    virtual void DoSomething()=0;
};

#endif // __Interface_h__
