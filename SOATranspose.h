#include <intrin.h>

#define SSE_SHUFFLE(x,y,z,w) _MM_SHUFFLE(w,z,y,x)


typedef __m128  simdvec4;

struct soavec24
{
    simdvec4 sv[2];

    soavec24() { sv[0]=sv[1]=_mm_set1_ps(0.0f); }
    soavec24(const simdvec4 *_v) { sv[0]=_sv[0]; sv[1]=_sv[1]; sv[2]=_sv[2]; }
    soavec24(const simdvec4 &_x, const simdvec4 &_y) { sv[0]=_x; sv[1]=_y; }
    simdvec4& operator[](int i) { return sv[i]; }
    const simdvec4& operator[](int i) const { return sv[i]; }
};

struct soavec34
{
    simdvec4 sv[3];

    soavec34() { sv[0]=sv[1]=sv[2]=_mm_set1_ps(0.0f); }
    soavec34(const simdvec4 *_v) { sv[0]=_sv[0]; sv[1]=_sv[1]; sv[2]=_sv[2]; }
    soavec34(const simdvec4 &_x, const simdvec4 &_y, const simdvec4 &_z) { sv[0]=_x; sv[1]=_y; sv[2]=_z; }
    simdvec4& operator[](int i) { return sv[i]; }
    const simdvec4& operator[](int i) const { return sv[i]; }
};

struct soavec44
{
    simdvec4 sv[4];

    soavec44() { sv[0]=sv[1]=sv[2]=sv[3]=_mm_set1_ps(0.0f); }
    soavec44(const simdvec4 *_v) { sv[0]=_sv[0]; sv[1]=_sv[1]; sv[2]=_sv[2]; sv[3]=_sv[3]; }
    soavec44(const simdvec4 &_x, const simdvec4 &_y, const simdvec4 &_z, const simdvec4 &_w) { sv[0]=_x; sv[1]=_y; sv[2]=_z; sv[3]=_w; }
    simdvec4& operator[](int i) { return sv[i]; }
    const simdvec4& operator[](int i) const { return sv[i]; }
};



// 2 要素 SoA ベクトルへ

// in:  {x,y,z,w}[1]
// out: 
//      x[0], 0.0f, 0.0f, 0.0f
//      y[0], 0.0f, 0.0f, 0.0f
inline soavec24 soa_transpose24(const simdvec4 &v0)
{
    __m128 zero = _mm_set_ps1(0.0f);
    __m128 r1 = _mm_unpacklo_ps(v0, zero);
    __m128 r2 = zero;
    return soavec24(
        _mm_shuffle_ps(r1, r2, SSE_SHUFFLE(0,1,0,1)),
        _mm_shuffle_ps(r1, r2, SSE_SHUFFLE(2,3,2,3)) );
}

// in:  {x,y,z,w}[2]
// out: 
//      x[0], x[1], 0.0f, 0.0f
//      y[0], y[1], 0.0f, 0.0f
inline soavec24 soa_transpose24(const simdvec4 &v0, const simdvec4 &v1)
{
    __m128 zero = _mm_set_ps1(0.0f);
    __m128 r1 = _mm_unpacklo_ps(v0, v1);
    __m128 r2 = zero;
    return soavec24(
        _mm_shuffle_ps(r1, r2, SSE_SHUFFLE(0,1,0,1)),
        _mm_shuffle_ps(r1, r2, SSE_SHUFFLE(2,3,2,3)) );
}

// in:  {x,y,z,w}[3]
// out: 
//      x[0], x[1], x[2], 0.0f
//      y[0], y[1], y[2], 0.0f
inline soavec24 soa_transpose24(const simdvec4 &v0, const simdvec4 &v1, const simdvec4 &v2)
{
    __m128 zero = _mm_set_ps1(0.0f);
    __m128 r1 = _mm_unpacklo_ps(v0, v1);
    __m128 r2 = _mm_unpacklo_ps(v2, zero);
    return soavec24(
        _mm_shuffle_ps(r1, r2, SSE_SHUFFLE(0,1,0,1)),
        _mm_shuffle_ps(r1, r2, SSE_SHUFFLE(2,3,2,3)) );
}

// in:  {x,y,z,w}[4]
// out: 
//      x[0], x[1], x[2], x[3]
//      y[0], y[1], y[2], y[3]
inline soavec24 soa_transpose24(const simdvec4 &v0, const simdvec4 &v1, const simdvec4 &v2, const simdvec4 &v3)
{
    __m128 r1 = _mm_unpacklo_ps(v0, v1);
    __m128 r2 = _mm_unpacklo_ps(v2, v3);
    return soavec24(
        _mm_shuffle_ps(r1, r2, SSE_SHUFFLE(0,1,0,1)),
        _mm_shuffle_ps(r1, r2, SSE_SHUFFLE(2,3,2,3)) );
}


// 3 要素 SoA ベクトルへ

// in:  {x,y,z,w}[1]
// out: 
//      x[0], 0.0f, 0.0f, 0.0f
//      y[0], 0.0f, 0.0f, 0.0f
//      z[0], 0.0f, 0.0f, 0.0f
inline soavec34 soa_transpose34(const simdvec4 &v0)
{
    __m128 zero = _mm_set_ps1(0.0f);
    __m128 r1 = _mm_unpacklo_ps(v0, zero);
    __m128 r2 = zero;
    __m128 r3 = _mm_unpackhi_ps(v0, zero);
    __m128 r4 = zero;
    return soavec34(
        _mm_shuffle_ps(r1, r2, SSE_SHUFFLE(0,1,0,1)),
        _mm_shuffle_ps(r1, r2, SSE_SHUFFLE(2,3,2,3)),
        _mm_shuffle_ps(r3, r4, SSE_SHUFFLE(0,1,0,1)) );
}

// in:  {x,y,z,w}[2]
// out: 
//      x[0], x[1], 0.0f, 0.0f
//      y[0], y[1], 0.0f, 0.0f
//      z[0], z[1], 0.0f, 0.0f
inline soavec34 soa_transpose34(const simdvec4 &v0, const simdvec4 &v1)
{
    __m128 zero = _mm_set_ps1(0.0f);
    __m128 r1 = _mm_unpacklo_ps(v0, v1);
    __m128 r2 = zero;
    __m128 r3 = _mm_unpackhi_ps(v0, v1);
    __m128 r4 = zero;
    return soavec34(
        _mm_shuffle_ps(r1, r2, SSE_SHUFFLE(0,1,0,1)),
        _mm_shuffle_ps(r1, r2, SSE_SHUFFLE(2,3,2,3)),
        _mm_shuffle_ps(r3, r4, SSE_SHUFFLE(0,1,0,1)) );
}

// in:  {x,y,z,w}[3]
// out: 
//      x[0], x[1], x[2], 0.0f
//      y[0], y[1], y[2], 0.0f
//      z[0], z[1], z[2], 0.0f
inline soavec34 soa_transpose34(const simdvec4 &v0, const simdvec4 &v1, const simdvec4 &v2)
{
    __m128 zero = _mm_set_ps1(0.0f);
    __m128 r1 = _mm_unpacklo_ps(v0, v1);
    __m128 r2 = _mm_unpacklo_ps(v2, zero);
    __m128 r3 = _mm_unpackhi_ps(v0, v1);
    __m128 r4 = _mm_unpackhi_ps(v2, zero);
    return soavec34(
        _mm_shuffle_ps(r1, r2, SSE_SHUFFLE(0,1,0,1)),
        _mm_shuffle_ps(r1, r2, SSE_SHUFFLE(2,3,2,3)),
        _mm_shuffle_ps(r3, r4, SSE_SHUFFLE(0,1,0,1)) );
}

// in:  {x,y,z,w}[4]
// out: 
//      x[0], x[1], x[2], x[3]
//      y[0], y[1], y[2], y[3]
//      z[0], z[1], z[2], z[3]
inline soavec34 soa_transpose34(const simdvec4 &v0, const simdvec4 &v1, const simdvec4 &v2, const simdvec4 &v3)
{
    __m128 r1 = _mm_unpacklo_ps(v0, v1);
    __m128 r2 = _mm_unpacklo_ps(v2, v3);
    __m128 r3 = _mm_unpackhi_ps(v0, v1);
    __m128 r4 = _mm_unpackhi_ps(v2, v3);
    return soavec34(
        _mm_shuffle_ps(r1, r2, SSE_SHUFFLE(0,1,0,1)),
        _mm_shuffle_ps(r1, r2, SSE_SHUFFLE(2,3,2,3)),
        _mm_shuffle_ps(r3, r4, SSE_SHUFFLE(0,1,0,1)) );
}


// 4 要素 SoA ベクトルへ

// in:  {x,y,z,w}[1]
// out: 
//      x[0], 0.0f, 0.0f, 0.0f
//      y[0], 0.0f, 0.0f, 0.0f
//      z[0], 0.0f, 0.0f, 0.0f
//      w[0], 0.0f, 0.0f, 0.0f
inline soavec44 soa_transpose44(const simdvec4 &v0)
{
    __m128 zero = _mm_set_ps1(0.0f);
    __m128 r1 = _mm_unpacklo_ps(v0, zero);
    __m128 r2 = zero;
    __m128 r3 = _mm_unpackhi_ps(v0, zero);
    __m128 r4 = zero;
    return soavec44(
        _mm_shuffle_ps(r1, r2, SSE_SHUFFLE(0,1,0,1)),
        _mm_shuffle_ps(r1, r2, SSE_SHUFFLE(2,3,2,3)),
        _mm_shuffle_ps(r3, r4, SSE_SHUFFLE(0,1,0,1)),
        _mm_shuffle_ps(r3, r4, SSE_SHUFFLE(2,3,2,3)) );
}

// in:  {x,y,z,w}[2]
// out: 
//      x[0], x[1], 0.0f, 0.0f
//      y[0], y[1], 0.0f, 0.0f
//      z[0], z[1], 0.0f, 0.0f
//      w[0], w[1], 0.0f, 0.0f
inline soavec44 soa_transpose44(const simdvec4 &v0, const simdvec4 &v1)
{
    __m128 zero = _mm_set_ps1(0.0f);
    __m128 r1 = _mm_unpacklo_ps(v0, v1);
    __m128 r2 = zero;
    __m128 r3 = _mm_unpackhi_ps(v0, v1);
    __m128 r4 = zero;
    return soavec44(
        _mm_shuffle_ps(r1, r2, SSE_SHUFFLE(0,1,0,1)),
        _mm_shuffle_ps(r1, r2, SSE_SHUFFLE(2,3,2,3)),
        _mm_shuffle_ps(r3, r4, SSE_SHUFFLE(0,1,0,1)),
        _mm_shuffle_ps(r3, r4, SSE_SHUFFLE(2,3,2,3)) );
}

// in:  {x,y,z,w}[3]
// out: 
//      x[0], x[1], x[2], 0.0f
//      y[0], y[1], y[2], 0.0f
//      z[0], z[1], z[2], 0.0f
//      w[0], w[1], w[2], 0.0f
inline soavec44 soa_transpose44(const simdvec4 &v0, const simdvec4 &v1, const simdvec4 &v2)
{
    __m128 zero = _mm_set_ps1(0.0f);
    __m128 r1 = _mm_unpacklo_ps(v0, v1);
    __m128 r2 = _mm_unpacklo_ps(v2, zero);
    __m128 r3 = _mm_unpackhi_ps(v0, v1);
    __m128 r4 = _mm_unpackhi_ps(v2, zero);
    return soavec44(
        _mm_shuffle_ps(r1, r2, SSE_SHUFFLE(0,1,0,1)),
        _mm_shuffle_ps(r1, r2, SSE_SHUFFLE(2,3,2,3)),
        _mm_shuffle_ps(r3, r4, SSE_SHUFFLE(0,1,0,1)),
        _mm_shuffle_ps(r3, r4, SSE_SHUFFLE(2,3,2,3)) );
}

// in:  {x,y,z,w}[4]
// out: 
//      x[0], x[1], x[2], x[3]
//      y[0], y[1], y[2], y[3]
//      z[0], z[1], z[2], z[3]
//      w[0], w[1], w[2], w[3]
inline soavec44 soa_transpose44(const simdvec4 &v0, const simdvec4 &v1, const simdvec4 &v2, const simdvec4 &v3)
{
    __m128 r1 = _mm_unpacklo_ps(v0, v1);
    __m128 r2 = _mm_unpacklo_ps(v2, v3);
    __m128 r3 = _mm_unpackhi_ps(v0, v1);
    __m128 r4 = _mm_unpackhi_ps(v2, v3);
    return soavec44(
        _mm_shuffle_ps(r1, r2, SSE_SHUFFLE(0,1,0,1)),
        _mm_shuffle_ps(r1, r2, SSE_SHUFFLE(2,3,2,3)),
        _mm_shuffle_ps(r3, r4, SSE_SHUFFLE(0,1,0,1)),
        _mm_shuffle_ps(r3, r4, SSE_SHUFFLE(2,3,2,3)) );
}

#undef SSE_SHUFFLE
