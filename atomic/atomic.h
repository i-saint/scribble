#include <cstdint>

extern "C" {

// 64bit 版は Win64 でのみ提供。
// 戻り値は全て変更前の値

// increment
int8_t  __fastcall atomic_inc8( int8_t  *v);
int16_t __fastcall atomic_inc16(int16_t *v);
int32_t __fastcall atomic_inc32(int32_t *v);
int64_t __fastcall atomic_inc64(int64_t *v);

// decrement
int8_t  __fastcall atomic_dec8( int8_t  *v);
int16_t __fastcall atomic_dec16(int16_t *v);
int32_t __fastcall atomic_dec32(int32_t *v);
int64_t __fastcall atomic_dec64(int64_t *v);

// add
int8_t  __fastcall atomic_add8( int8_t  *v, int8_t  a);
int16_t __fastcall atomic_add16(int16_t *v, int16_t a);
int32_t __fastcall atomic_add32(int32_t *v, int32_t a);
int64_t __fastcall atomic_add64(int64_t *v, int64_t a);

// subtract
int8_t  __fastcall atomic_sub8( int8_t  *v, int8_t  a);
int16_t __fastcall atomic_sub16(int16_t *v, int16_t a);
int32_t __fastcall atomic_sub32(int32_t *v, int32_t a);
int64_t __fastcall atomic_sub64(int64_t *v, int64_t a);

// swap
int8_t  __fastcall atomic_swap8( int8_t  *v, int8_t  a);
int16_t __fastcall atomic_swap16(int16_t *v, int16_t a);
int32_t __fastcall atomic_swap32(int32_t *v, int32_t a);
int64_t __fastcall atomic_swap64(int64_t *v, int64_t a);

// compare & swap
int8_t  __fastcall atomic_cas8( int8_t  *v, int8_t  v_old, int8_t  v_new);
int16_t __fastcall atomic_cas16(int16_t *v, int16_t v_old, int16_t v_new);
int32_t __fastcall atomic_cas32(int32_t *v, int32_t v_old, int32_t v_new);
int64_t __fastcall atomic_cas64(int64_t *v, int64_t v_old, int64_t v_new);

} // extern "C"
