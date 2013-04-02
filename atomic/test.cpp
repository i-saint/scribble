#include <cstdio>
#include "atomic.h"

int main(int argc, char *argv[])
{
    int8_t  i8  = 0;
    int16_t i16 = 0;
    int32_t i32 = 0;
    int8_t  t8;
    int16_t t16;
    int32_t t32;

    t8  = atomic_inc8( &i8);
    t16 = atomic_inc16(&i16);
    t32 = atomic_inc32(&i32);
    printf("inc i8:%d->%d i16:%d->%d i32:%d->%d\n", (int)t8,(int)i8, (int)t16,(int)i16, (int)t32,(int)i32);

    t8  = atomic_dec8( &i8);
    t16 = atomic_dec16(&i16);
    t32 = atomic_dec32(&i32);
    printf("dec i8:%d->%d i16:%d->%d i32:%d->%d\n", (int)t8,(int)i8, (int)t16,(int)i16, (int)t32,(int)i32);

    t8  = atomic_add8( &i8,  2);
    t16 = atomic_add16(&i16, 2);
    t32 = atomic_add32(&i32, 2);
    printf("add i8:%d->%d i16:%d->%d i32:%d->%d\n", (int)t8,(int)i8, (int)t16,(int)i16, (int)t32,(int)i32);

    t8  = atomic_sub8( &i8,  2);
    t16 = atomic_sub16(&i16, 2);
    t32 = atomic_sub32(&i32, 2);
    printf("sub i8:%d->%d i16:%d->%d i32:%d->%d\n", (int)t8,(int)i8, (int)t16,(int)i16, (int)t32,(int)i32);

    t8  = atomic_swap8( &i8,  2);
    t16 = atomic_swap16(&i16, 2);
    t32 = atomic_swap32(&i32, 2);
    printf("swap i8:%d->%d i16:%d->%d i32:%d->%d\n", (int)t8,(int)i8, (int)t16,(int)i16, (int)t32,(int)i32);

    t8  = atomic_cas8( &i8,  0, 3);
    t16 = atomic_cas16(&i16, 0, 3);
    t32 = atomic_cas32(&i32, 0, 3);
    printf("cas1 i8:%d->%d i16:%d->%d i32:%d->%d\n", (int)t8,(int)i8, (int)t16,(int)i16, (int)t32,(int)i32);

    t8  = atomic_cas8( &i8,  2, 4);
    t16 = atomic_cas16(&i16, 2, 4);
    t32 = atomic_cas32(&i32, 2, 4);
    printf("cas2 i8:%d->%d i16:%d->%d i32:%d->%d\n", (int)t8,(int)i8, (int)t16,(int)i16, (int)t32,(int)i32);
}
