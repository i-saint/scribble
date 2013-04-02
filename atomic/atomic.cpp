#include <cstdio>
#include <cstdint>

extern "C" {
int8_t  __fastcall atomic_add8(int8_t *v, int8_t a);
int16_t __fastcall atomic_add16(int16_t *v, int16_t a);
int32_t __fastcall atomic_add32(int32_t *v, int32_t a);
int8_t  __fastcall atomic_cas8(int8_t *v, int8_t v_old, int8_t v_new);
int16_t __fastcall atomic_cas16(int16_t *v, int16_t v_old, int16_t v_new);
int32_t __fastcall atomic_cas32(int32_t *v, int32_t v_old, int32_t v_new);
} // extern "C"


int8_t __fastcall ref_cas8(int8_t *v, int8_t v_old, int8_t v_new)
{
	if(*v==v_old) { *v=v_new; }
	return v_old;
}

int main(int argc, char *argv[])
{
	int8_t i8 = 1;
	int16_t i16 = 1;
	int32_t i32 = 1;
	atomic_add8(&i8, 1);
	atomic_add16(&i16, 2);
	atomic_add32(&i32, 3);
	printf("%d %d %d\n", (int)i8, (int)i16, (int)i32);

	//ref_cas8(&i8, 2, 3);
	int r = atomic_cas8(&i8, 4, 3);
	printf("%d(%d) %d %d\n", (int)i8, r, (int)i16, (int)i32);

}