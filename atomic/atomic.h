#include <cstdint>

extern "C" {

// increment。変更前の値を返す
int8_t  __fastcall atomic_inc8( int8_t  *v);
int16_t __fastcall atomic_inc16(int16_t *v);
int32_t __fastcall atomic_inc32(int32_t *v);

// decrement。変更前の値を返す
int8_t  __fastcall atomic_dec8( int8_t  *v);
int16_t __fastcall atomic_dec16(int16_t *v);
int32_t __fastcall atomic_dec32(int32_t *v);

// add。変更前の値を返す
int8_t  __fastcall atomic_add8( int8_t  *v, int8_t  a);
int16_t __fastcall atomic_add16(int16_t *v, int16_t a);
int32_t __fastcall atomic_add32(int32_t *v, int32_t a);

// subtract。変更前の値を返す
int8_t  __fastcall atomic_sub8( int8_t  *v, int8_t  a);
int16_t __fastcall atomic_sub16(int16_t *v, int16_t a);
int32_t __fastcall atomic_sub32(int32_t *v, int32_t a);

// swap。変更前の値を返す
int8_t  __fastcall atomic_swap8( int8_t  *v, int8_t  a);
int16_t __fastcall atomic_swap16(int16_t *v, int16_t a);
int32_t __fastcall atomic_swap32(int32_t *v, int32_t a);

// compare & swap。変更前の値を返す
int8_t  __fastcall atomic_cas8( int8_t  *v, int8_t  v_old, int8_t  v_new);
int16_t __fastcall atomic_cas16(int16_t *v, int16_t v_old, int16_t v_new);
int32_t __fastcall atomic_cas32(int32_t *v, int32_t v_old, int32_t v_new);

} // extern "C"
