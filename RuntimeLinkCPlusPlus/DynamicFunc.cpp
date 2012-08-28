#include <cstdio>

extern "C" float FloatAdd(float a, float b)
{
    return a+b;
}

extern "C" float FloatSub(float a, float b)
{
    return a-b;
}

extern "C" float FloatMul(float a, float b)
{
    return a*b;
}

extern "C" float FloatDiv(float a, float b)
{
    return a/b;
}
