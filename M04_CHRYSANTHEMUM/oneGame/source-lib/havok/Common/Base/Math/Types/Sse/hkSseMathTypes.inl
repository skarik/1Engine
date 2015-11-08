/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// include SSE
#include <xmmintrin.h>

#if HK_SSE_VERSION >= 0x20
#include <emmintrin.h>
#endif

#if HK_SSE_VERSION >= 0x30
#include <pmmintrin.h>
#endif

#if HK_SSE_VERSION >= 0x31
#include <tmmintrin.h>
#endif

#if HK_SSE_VERSION >= 0x40
#include <wmmintrin.h>
#endif

#if HK_SSE_VERSION >= 0x41
#include <smmintrin.h>
#endif

#if HK_SSE_VERSION >= 0x42
#include <nmmintrin.h>
#endif

#if HK_SSE_VERSION >= 0x50
#include <immintrin.h>
#endif

#if defined(HK_REAL_IS_DOUBLE)

#if HK_SSE_VERSION >= 0x50

// x3 - x0 must have either value 0 or 1
#define _MM256_SHUFFLE(x3, x2, x1, x0) ((x3 << 3) | (x2 << 2) | (x1 << 1) | (x0))

#define HK_TRANSPOSE4(row0, row1, row2, row3) {                 \
            const hkQuadReal tmp0   = _mm256_shuffle_pd(row0.m_quad, row1.m_quad, _MM256_SHUFFLE(0,0,0,0));          \
            const hkQuadReal tmp1   = _mm256_shuffle_pd(row0.m_quad, row1.m_quad, _MM256_SHUFFLE(1,1,1,1));          \
            const hkQuadReal tmp2   = _mm256_shuffle_pd(row2.m_quad, row3.m_quad, _MM256_SHUFFLE(0,0,0,0));          \
            const hkQuadReal tmp3   = _mm256_shuffle_pd(row2.m_quad, row3.m_quad, _MM256_SHUFFLE(1,1,1,1));          \
            row0.m_quad = _mm256_permute2f128_pd(tmp0, tmp2, 0x20);              \
            row1.m_quad = _mm256_permute2f128_pd(tmp1, tmp3, 0x20);              \
            row2.m_quad = _mm256_permute2f128_pd(tmp0, tmp2, 0x31);              \
            row3.m_quad = _mm256_permute2f128_pd(tmp1, tmp3, 0x31);              \
        }

// transpose 3 hkVector4: w component is undefined
#define HK_TRANSPOSE3(row0, row1, row2) {                 \
			const hkQuadReal zero = _mm256_setzero_pd();			\
            const hkQuadReal tmp0   = _mm256_shuffle_pd(row0.m_quad, row1.m_quad, _MM256_SHUFFLE(0,0,0,0));          \
            const hkQuadReal tmp1   = _mm256_shuffle_pd(row0.m_quad, row1.m_quad, _MM256_SHUFFLE(1,1,1,1));          \
            const hkQuadReal tmp2   = _mm256_shuffle_pd(row2.m_quad, zero,		 _MM256_SHUFFLE(0,0,0,0));          \
            const hkQuadReal tmp3   = _mm256_shuffle_pd(row2.m_quad, zero,		 _MM256_SHUFFLE(1,1,1,1));          \
            row0.m_quad = _mm256_permute2f128_pd(tmp0, tmp2, 0x20);              \
            row1.m_quad = _mm256_permute2f128_pd(tmp1, tmp3, 0x20);              \
            row2.m_quad = _mm256_permute2f128_pd(tmp0, tmp2, 0x31);              \
        }

// storage type for hkVector4 (and thus hkQuaternion)
typedef __m256d hkQuadReal;

#else

#define HK_TRANSPOSE4(v0,v1,v2,v3) { __m128d __t1,__t2; \
		__t1 = v1.m_quad.xy; \
		v1.m_quad.xy = _mm_unpackhi_pd(v0.m_quad.xy, v1.m_quad.xy); \
		v0.m_quad.xy = _mm_unpacklo_pd(v0.m_quad.xy, __t1); \
		__t2 = v3.m_quad.zw; \
		v3.m_quad.zw = _mm_unpackhi_pd(v2.m_quad.zw, v3.m_quad.zw); \
		v2.m_quad.zw = _mm_unpacklo_pd(v2.m_quad.zw, __t2); \
		__t1 = v0.m_quad.zw; __t2 = v1.m_quad.zw; \
		v0.m_quad.zw = _mm_unpacklo_pd(v2.m_quad.xy, v3.m_quad.xy); \
		v1.m_quad.zw = _mm_unpackhi_pd(v2.m_quad.xy, v3.m_quad.xy); \
		v2.m_quad.xy = _mm_unpacklo_pd(__t1,__t2); \
		v3.m_quad.xy = _mm_unpackhi_pd(__t1,__t2); \
	}

// transpose 3 hkVector4: w component is undefined
#define HK_TRANSPOSE3(v0,v1,v2) { __m128d __t1,__t2; \
		__m128d __zero = _mm_setzero_pd(); \
		__t1 = v1.m_quad.xy; \
		v1.m_quad.xy = _mm_unpackhi_pd(v0.m_quad.xy, v1.m_quad.xy); \
		v0.m_quad.xy = _mm_unpacklo_pd(v0.m_quad.xy, __t1); \
		v2.m_quad.zw = _mm_unpacklo_pd(v2.m_quad.zw, __zero); \
		__t1 = v0.m_quad.zw; __t2 = v1.m_quad.zw; \
		v0.m_quad.zw = _mm_unpacklo_pd(v2.m_quad.xy, __zero); \
		v1.m_quad.zw = _mm_unpackhi_pd(v2.m_quad.xy, __zero); \
		v2.m_quad.xy = _mm_unpacklo_pd(__t1,__t2); \
	}

// storage type for hkVector4 (and thus hkQuaternion)
struct hkQuadReal
{
	__m128d xy;
	__m128d zw;
};

#endif

// storage type for hkSimdReal
typedef __m128d hkSingleReal;

// storage type for hkVector4Comparison
typedef hkQuadReal hkVector4Mask;

#else

#define HK_TRANSPOSE4(A,B,C,D) _MM_TRANSPOSE4_PS(A.m_quad,B.m_quad,C.m_quad,D.m_quad)

// transpose 3 hkVector4: w component is undefined
#define HK_TRANSPOSE3(_a,_b,_c) { \
	const hkQuadReal _tmp = _mm_unpacklo_ps(_a.m_quad,_b.m_quad);	\
	const hkQuadReal _ctmp = _mm_shuffle_ps(_mm_unpackhi_ps(_a.m_quad,_b.m_quad),_c.m_quad,_MM_SHUFFLE(3,2,1,0));	\
	_a.m_quad = _mm_movelh_ps(_tmp,_c.m_quad);	\
	_b.m_quad = _mm_shuffle_ps(_mm_movehl_ps(_a.m_quad,_tmp),_c.m_quad,_MM_SHUFFLE(3,1,1,0));	\
	_c.m_quad = _ctmp; }



// storage type for hkVector4 (and thus hkQuaternion)
typedef __m128 hkQuadReal;

// storage type for hkSimdReal
typedef __m128 hkSingleReal;

// storage type for hkVector4Comparison
typedef __m128 hkVector4Mask;

#endif // HK_REAL_IS_DOUBLE


// storage type for hkIntVector
typedef __m128i hkQuadUint;

#if defined(HK_COMPILER_GCC) || (defined(_MSC_VER) && (_MSC_VER<1500)) // vs 2005 is missing these
	union hkQuadRealUnionM128
	{
		hkQuadRealUnionM128(__m128  v)  : m128_vecf(v) {}
		hkQuadRealUnionM128(__m128i v)  : m128_veci(v) {}
		hkQuadRealUnionM128(__m128d v)  : m128_vecd(v) {}
		__m128     m128_vecf;
		__m128i    m128_veci;
		__m128d    m128_vecd;
		float      m128_f32[4];
		double     m128d_f64[2];
		hkInt8     m128_i8[16];
		hkInt16    m128_i16[8];
		hkInt16    m128_i32[4];
		hkInt16    m128_i64[2];
		hkUint8    m128_u8[16];
		hkUint16   m128_u16[8];
		hkUint32   m128_u32[4];
		hkUint64   m128_u64[2];
		hkInt8     m128i_i8[16];
		hkInt16    m128i_i16[8];
		hkInt16    m128i_i32[4];
		hkInt16    m128i_i64[2];
		hkUint8    m128i_u8[16];
		hkUint16   m128i_u16[8];
		hkUint32   m128i_u32[4];
		hkUint64   m128i_u64[2];
	};

#if defined(_MSC_VER) && (_MSC_VER<1500)

		// msvc2005 is missing some intrinsics, fake them
		HK_FORCE_INLINE __m128  _mm_castpd_ps   (__m128d x) { return hkQuadRealUnionM128(x).m128_vecf; }
		HK_FORCE_INLINE __m128i _mm_castpd_si128(__m128d x) { return hkQuadRealUnionM128(x).m128_veci; }
		HK_FORCE_INLINE __m128d _mm_castps_pd   (__m128 x)  { return hkQuadRealUnionM128(x).m128_vecd; }
		HK_FORCE_INLINE __m128i _mm_castps_si128(__m128 x)  { return hkQuadRealUnionM128(x).m128_veci; }
		HK_FORCE_INLINE __m128  _mm_castsi128_ps(__m128i x) { return hkQuadRealUnionM128(x).m128_vecf; }
		HK_FORCE_INLINE __m128d _mm_castsi128_pd(__m128i x) { return hkQuadRealUnionM128(x).m128_vecd; }
		#define HK_M128(x) x

	#else
		#error
	#endif
#else
	#define HK_M128(x) x
#endif

// argument types
class hkVector4;
typedef const hkVector4& hkVector4Parameter;

class hkSimdReal;
typedef const hkSimdReal& hkSimdRealParameter;

class hkVector4Comparison;
typedef const hkVector4Comparison& hkVector4ComparisonParameter;

class hkQuaternion;
typedef const hkQuaternion& hkQuaternionParameter;

class hkIntVector;
typedef const hkIntVector& hkIntVectorParameter;


// this causes problems for the optimizer, use for debug checks only
#define HK_QUADREAL_CONSTANT(a, b, c, d)	{a, b, c, d}
#define HK_QUADINT_CONSTANT(a, b, c, d)		{a, b, c, d}
union hkQuadRealUnion
{
	hkReal r[4];
	hkQuadReal q;
};

union hkQuadUintUnion
{
	hkUint32 u[4];
	hkQuadUint q;
};

union hkQuadShortUnion
{
	hkInt16 u[8];
	hkQuadUint q;
};

union hkQuadUshortUnion
{
	hkUint16 u[8];
	hkQuadUint q;
};

union hkQuadCharUnion
{
	hkUint8 u[16];
	hkQuadUint q;
};

union hkQuadUcharUnion
{
	hkUint8 u[16];
	hkQuadUint q;
};

/*
 * Havok SDK - NO SOURCE PC DOWNLOAD, BUILD(#20120119)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2012
 * Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
 * Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
 * rights, and intellectual property rights in the Havok software remain in
 * Havok and/or its suppliers.
 * 
 * Use of this software for evaluation purposes is subject to and indicates
 * acceptance of the End User licence Agreement for this product. A copy of
 * the license is included with this software and is also available at www.havok.com/tryhavok.
 * 
 */
