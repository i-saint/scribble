#include <cstdio>
#include "atomic.h"

#ifdef _WIN64
#   define PRINT_ARGS " i8:%d->%d i16:%d->%d i32:%d->%d i64:%lld->%lld\n", (int)t8,(int)i8, (int)t16,(int)i16, (int)t32,(int)i32, t64,i64
#else // _WIN64
#   define PRINT_ARGS " i8:%d->%d i16:%d->%d i32:%d->%d\n", (int)t8,(int)i8, (int)t16,(int)i16, (int)t32,(int)i32
#endif // _WIN64

int main(int argc, char *argv[])
{
    int8_t  i8  = 0;
    int16_t i16 = 0;
    int32_t i32 = 0;
    int64_t i64 = 0;
    int8_t  t8;
    int16_t t16;
    int32_t t32;
    int64_t t64;

    t8  = atomic_inc8( &i8);
    t16 = atomic_inc16(&i16);
    t32 = atomic_inc32(&i32);
#ifdef _WIN64
    t64 = atomic_inc64(&i64);
#endif // _WIN64
    printf("inc" PRINT_ARGS);

    t8  = atomic_dec8( &i8);
    t16 = atomic_dec16(&i16);
    t32 = atomic_dec32(&i32);
#ifdef _WIN64
    t64 = atomic_dec64(&i64);
#endif // _WIN64
    printf("dec" PRINT_ARGS);

    t8  = atomic_add8( &i8,  2);
    t16 = atomic_add16(&i16, 2);
    t32 = atomic_add32(&i32, 2);
#ifdef _WIN64
    t64 = atomic_add64(&i64, 2);
#endif // _WIN64
    printf("add" PRINT_ARGS);

    t8  = atomic_sub8( &i8,  2);
    t16 = atomic_sub16(&i16, 2);
    t32 = atomic_sub32(&i32, 2);
#ifdef _WIN64
    t64 = atomic_sub64(&i64, 2);
#endif // _WIN64
    printf("sub" PRINT_ARGS);

    t8  = atomic_swap8( &i8,  2);
    t16 = atomic_swap16(&i16, 2);
    t32 = atomic_swap32(&i32, 2);
#ifdef _WIN64
    t64 = atomic_swap64(&i64, 2);
#endif // _WIN64
    printf("swap" PRINT_ARGS);

    t8  = atomic_cas8( &i8,  0, 3);
    t16 = atomic_cas16(&i16, 0, 3);
    t32 = atomic_cas32(&i32, 0, 3);
#ifdef _WIN64
    t64 = atomic_cas64(&i64, 0, 3);
#endif // _WIN64
    printf("cas1" PRINT_ARGS);

    t8  = atomic_cas8( &i8,  2, 4);
    t16 = atomic_cas16(&i16, 2, 4);
    t32 = atomic_cas32(&i32, 2, 4);
#ifdef _WIN64
    t64 = atomic_cas64(&i64, 2, 4);
#endif // _WIN64
    printf("cas2" PRINT_ARGS);
}
