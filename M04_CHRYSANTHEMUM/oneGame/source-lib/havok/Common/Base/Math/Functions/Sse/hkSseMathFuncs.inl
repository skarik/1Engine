/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#define HK_STD_NAMESPACE /*nothing*/

namespace hkMath
{
	//
	// need to implement
	//
#	define HK_MATH_quadReciprocal
	HK_FORCE_INLINE static hkQuadReal HK_CALL quadReciprocal( const hkQuadReal& r )
	{
#if defined(HK_REAL_IS_DOUBLE)
#if HK_SSE_VERSION >= 0x50
		return _mm256_div_pd(_mm256_set1_pd(1.0), r);
#else
		const __m128 xy = _mm_cvtpd_ps(r.xy);
		const __m128 zw = _mm_cvtpd_ps(r.zw);
		const __m128 xyzw = _mm_shuffle_ps(xy,zw,_MM_SHUFFLE(1,0,1,0));
		const __m128 rb = _mm_rcp_ps(xyzw);
		// One Newton-Raphson refinement iteration
		const __m128 rbr = _mm_mul_ps(xyzw, rb);
		const __m128 d = _mm_sub_ps(_mm_set1_ps(2.0f), rbr);
		const __m128 re = _mm_mul_ps(rb, d);
		hkQuadReal result;
		result.xy = _mm_cvtps_pd(re);
		result.zw = _mm_cvtps_pd(_mm_shuffle_ps(re,re,_MM_SHUFFLE(1,0,3,2)));
		return result;
#endif
#else
		const hkQuadReal rb = _mm_rcp_ps(r);
		// One Newton-Raphson refinement iteration
		const hkQuadReal rbr = _mm_mul_ps(r, rb);
		const hkQuadReal d = _mm_sub_ps(_mm_set1_ps(2.0f), rbr);
		return _mm_mul_ps(rb, d);
#endif
	}

#	define HK_MATH_quadReciprocalSquareRoot
	HK_FORCE_INLINE static hkQuadReal HK_CALL quadReciprocalSquareRoot( const hkQuadReal& r )
	{
#if defined(HK_REAL_IS_DOUBLE)
#if HK_SSE_VERSION >= 0x50
		return _mm256_div_pd(_mm256_set1_pd(1.0), _mm256_sqrt_pd(r));
#else
		const __m128 xy = _mm_cvtpd_ps(r.xy);
		const __m128 zw = _mm_cvtpd_ps(r.zw);
		const __m128 xyzw = _mm_shuffle_ps(xy,zw,_MM_SHUFFLE(1,0,1,0));
		const __m128 e = _mm_rsqrt_ps(xyzw);
		// One Newton-Raphson refinement iteration
		const __m128 he = _mm_mul_ps(_mm_set1_ps(0.5f),e);
		const __m128 ree = _mm_mul_ps(_mm_mul_ps(xyzw,e),e);
		const __m128 re = _mm_mul_ps(he, _mm_sub_ps(_mm_set1_ps(3.0f), ree) );
		hkQuadReal result;
		result.xy = _mm_cvtps_pd(re);
		result.zw = _mm_cvtps_pd(_mm_shuffle_ps(re,re,_MM_SHUFFLE(1,0,3,2)));
		return result;
#endif
#else
		const hkQuadReal e = _mm_rsqrt_ps(r);
		// One Newton-Raphson refinement iteration
		const hkQuadReal he = _mm_mul_ps(_mm_set1_ps(0.5f),e);
		const hkQuadReal ree = _mm_mul_ps(_mm_mul_ps(r,e),e);
		return _mm_mul_ps(he, _mm_sub_ps(_mm_set1_ps(3.0f), ree) );
#endif
	}

#	define HK_MATH_quadReciprocalTwoIter
	HK_FORCE_INLINE static hkQuadReal HK_CALL quadReciprocalTwoIter( const hkQuadReal& r )
	{
#if defined(HK_REAL_IS_DOUBLE)
#if HK_SSE_VERSION >= 0x50
		return _mm256_div_pd(_mm256_set1_pd(1.0), r);
#else
		const __m128 two = _mm_set1_ps(2.0f);
		const __m128 xy = _mm_cvtpd_ps(r.xy);
		const __m128 zw = _mm_cvtpd_ps(r.zw);
		const __m128 xyzw = _mm_shuffle_ps(xy,zw,_MM_SHUFFLE(1,0,1,0));
		const __m128 rb = _mm_rcp_ps(xyzw);
		//One round of Newton-Raphson refinement
		const __m128 rbr = _mm_mul_ps(xyzw, rb);
		const __m128 d = _mm_sub_ps(two, rbr);
		const __m128 rb1 = _mm_mul_ps(rb, d);
		//Another round
		const __m128 rbr2 = _mm_mul_ps(xyzw, rb1);
		const __m128 d2 = _mm_sub_ps(two, rbr2);
		const __m128 re = _mm_mul_ps(rb1, d2);
		hkQuadReal result;
		result.xy = _mm_cvtps_pd(re);
		result.zw = _mm_cvtps_pd(_mm_shuffle_ps(re,re,_MM_SHUFFLE(1,0,3,2)));
		return result;
#endif
#else
		const __m128 two = _mm_set1_ps(2.0f);
		const hkQuadReal rb = _mm_rcp_ps(r);
		//One round of Newton-Raphson refinement
		const hkQuadReal rbr = _mm_mul_ps(r, rb);
		const hkQuadReal d = _mm_sub_ps(two, rbr);
		const hkQuadReal rb1 = _mm_mul_ps(rb, d);
		//Another round
		const hkQuadReal rbr2 = _mm_mul_ps(r, rb1);
		const hkQuadReal d2 = _mm_sub_ps(two, rbr2);
		return _mm_mul_ps(rb1, d2);
#endif
	}

#	define HK_MATH_quadReciprocalSquareRootTwoIter
	HK_FORCE_INLINE static hkQuadReal HK_CALL quadReciprocalSquareRootTwoIter( const hkQuadReal& r )
	{
#if defined(HK_REAL_IS_DOUBLE)
#if HK_SSE_VERSION >= 0x50
		return _mm256_div_pd(_mm256_set1_pd(1.0), _mm256_sqrt_pd(r));
#else
		const __m128 half = _mm_set1_ps(0.5f);
		const __m128 three = _mm_set1_ps(3.0f);
		const __m128 xy = _mm_cvtpd_ps(r.xy);
		const __m128 zw = _mm_cvtpd_ps(r.zw);
		const __m128 xyzw = _mm_shuffle_ps(xy,zw,_MM_SHUFFLE(1,0,1,0));
		const __m128 e = _mm_rsqrt_ps(xyzw);
		// One Newton-Raphson refinement iteration
		const __m128 he = _mm_mul_ps(half,e);
		const __m128 ree = _mm_mul_ps(_mm_mul_ps(xyzw,e),e);
		const __m128 e1 = _mm_mul_ps(he, _mm_sub_ps(three, ree) );
		//Another round
		const __m128 he2 = _mm_mul_ps(half,e1);
		const __m128 ree2 = _mm_mul_ps(_mm_mul_ps(xyzw,e1),e1);
		const __m128 re = _mm_mul_ps(he2, _mm_sub_ps(three, ree2) );
		hkQuadReal result;
		result.xy = _mm_cvtps_pd(re);
		result.zw = _mm_cvtps_pd(_mm_shuffle_ps(re,re,_MM_SHUFFLE(1,0,3,2)));
		return result;
#endif
#else
		const __m128 half = _mm_set1_ps(0.5f);
		const __m128 three = _mm_set1_ps(3.0f);
		const hkQuadReal e = _mm_rsqrt_ps(r);
		// One Newton-Raphson refinement iteration
		const hkQuadReal he = _mm_mul_ps(half,e);
		const hkQuadReal ree = _mm_mul_ps(_mm_mul_ps(r,e),e);
		const hkQuadReal e1 = _mm_mul_ps(he, _mm_sub_ps(three, ree) );
		//Another round
		const hkQuadReal he2 = _mm_mul_ps(half,e1);
		const hkQuadReal ree2 = _mm_mul_ps(_mm_mul_ps(r,e1),e1);
		return _mm_mul_ps(he2, _mm_sub_ps(three, ree2) );
#endif
	}

	// 
	// SSE optimized implementations
	//


	/// Average absolute error 0.000046
	/// Max absolute error 169.777725 for r=1
	/// Max absolute error 0.001070 elsewhere
	/// About 3x faster than ::logf for 4 simultaneous values
	HK_FORCE_INLINE static hkQuadReal HK_CALL quadLog(const hkQuadReal &r)
	{
		static HK_ALIGN16(const int minNormalizedPosNumber[4])	= {  0x00800000,  0x00800000,  0x00800000,  0x00800000 }; // 1.1754943508222875E-38 (smallest normalized positive number)
		static HK_ALIGN16(const int one[4])						= {  0x3F800000,  0x3F800000,  0x3F800000,  0x3F800000 }; // 1.0
		static HK_ALIGN16(const int invMantissaMask[4])			= { ~0x7f800000, ~0x7f800000, ~0x7f800000, ~0x7f800000 };
		static HK_ALIGN16(const int int0x7f[4])					= {  0x0000007f,  0x0000007f,  0x0000007f,  0x0000007f };
		static HK_ALIGN16(const int logP0[4])					= {  0xBF4A21EF,  0xBF4A21EF,  0xBF4A21EF,  0xBF4A21EF }; // -7.89580278884799154124e-1
		static HK_ALIGN16(const int logQ0[4])					= {  0xC20EB06A,  0xC20EB06A,  0xC20EB06A,  0xC20EB06A }; // -3.56722798256324312549e1
		static HK_ALIGN16(const int logP1[4])					= {  0x418317E4,  0x418317E4,  0x418317E4,  0x418317E4 }; // 1.63866645699558079767e1
		static HK_ALIGN16(const int logQ1[4])					= {  0x439C0C01,  0x439C0C01,  0x439C0C01,  0x439C0C01 }; // 3.12093766372244180303e2
		static HK_ALIGN16(const int logP2[4])					= {  0xC2804831,  0xC2804831,  0xC2804831,  0xC2804831 }; // -6.41409952958715622951e1
		static HK_ALIGN16(const int logQ2[4])					= {  0xC4406C49,  0xC4406C49,  0xC4406C49,  0xC4406C49 }; // -7.69691943550460008604e2
		static HK_ALIGN16(const int logC0[4])					= {  0x3F317218,  0x3F317218,  0x3F317218,  0x3F317218 }; // 0.693147180559945

#if defined(HK_REAL_IS_DOUBLE)
#if HK_SSE_VERSION >= 0x50
		__m128 x = _mm256_cvtpd_ps(r);
#else
		const __m128 xy = _mm_cvtpd_ps(r.xy);
		const __m128 zw = _mm_cvtpd_ps(r.zw);
		__m128 x = _mm_shuffle_ps(xy,zw,_MM_SHUFFLE(1,0,1,0));
#endif
#else
		__m128 x = r;
#endif
		// cut off denormalized stuff
		x = _mm_max_ps(x, *(__m128*)minNormalizedPosNumber);

		__m128i emm0 = _mm_srli_epi32(_mm_castps_si128(x), 23);

		// keep only the fractional part
		x = _mm_or_ps(_mm_and_ps(x, *(__m128*)invMantissaMask), *(__m128*)one);

		x = _mm_mul_ps(_mm_sub_ps(x, *(__m128*)one), _mm_rcp_ps(_mm_add_ps(x, *(__m128*)one)));
		x = _mm_add_ps(x, x);
		__m128 bs = x;

		x = _mm_mul_ps(x, x);

		__m128 x4 = _mm_mul_ps(*(__m128*)logP0, x);
		__m128 x6 = _mm_mul_ps(*(__m128*)logQ0, x);

		x4 = _mm_add_ps(x4, *(__m128*)logP1);
		x6 = _mm_add_ps(x6, *(__m128*)logQ1);

		x4 = _mm_mul_ps(x4, x);
		x6 = _mm_mul_ps(x6, x);

		x4 = _mm_add_ps(x4, *(__m128*)logP2);
		x6 = _mm_add_ps(x6, *(__m128*)logQ2);

		x = _mm_mul_ps(x, x4);
		x6 = _mm_rcp_ps(x6);

		x = _mm_mul_ps(x, x6);
		x = _mm_mul_ps(x, bs);

		emm0 = _mm_sub_epi32(emm0, *(__m128i*)int0x7f);
		__m128 x1 = _mm_mul_ps(_mm_cvtepi32_ps(emm0), *(__m128*)logC0);

		x = _mm_add_ps(x, bs);
		x = _mm_add_ps(x, x1);

#if defined(HK_REAL_IS_DOUBLE)
#if HK_SSE_VERSION >= 0x50
		return _mm256_cvtps_pd(x);
#else
		hkQuadReal result;
		result.xy = _mm_cvtps_pd(x);
		result.zw = _mm_cvtps_pd(_mm_shuffle_ps(x,x,_MM_SHUFFLE(1,0,3,2)));
		return result;
#endif
#else
		return x;
#endif
	}

	/// Average absolute error 0.000059
	/// Max absolute error 0.000529
	/// About 6x faster than ::atan2f for 4 simultaneous values
	HK_FORCE_INLINE static hkQuadReal HK_CALL quadAtan2(const hkQuadReal &y, const hkQuadReal& x)
	{
		static HK_ALIGN16(const int signMask[4])		= { 0x80000000, 0x80000000, 0x80000000, 0x80000000 };
		static HK_ALIGN16(const int one[4])				= { 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000 }; //  1.0
		static HK_ALIGN16(const int minus1[4])			= { 0xBF800000, 0xBF800000, 0xBF800000, 0xBF800000 }; // -1.0
		static HK_ALIGN16(const int pi[4])				= { 0x40490FDB, 0x40490FDB, 0x40490FDB, 0x40490FDB }; //  PI
		static HK_ALIGN16(const int piOver2[4])			= { 0x3FC90FDB, 0x3FC90FDB, 0x3FC90FDB, 0x3FC90FDB }; //  PI / 2
		static HK_ALIGN16(const int t0[4])				= { 0xBDBBB0F6, 0xBDBBB0F6, 0xBDBBB0F6, 0xBDBBB0F6 }; // -0.91646118527267623468e-1
		static HK_ALIGN16(const int t1[4])				= { 0xBFB2A61F, 0xBFB2A61F, 0xBFB2A61F, 0xBFB2A61F }; // -0.13956945682312098640e1
		static HK_ALIGN16(const int t2[4])				= { 0xC2BCC9B1, 0xC2BCC9B1, 0xC2BCC9B1, 0xC2BCC9B1 }; // -0.94393926122725531747e2
		static HK_ALIGN16(const int t3[4])				= { 0x414E36D1, 0x414E36D1, 0x414E36D1, 0x414E36D1 }; //  0.12888383034157279340e2
		static HK_ALIGN16(const int s0[4])				= { 0x3FA3CF0F, 0x3FA3CF0F, 0x3FA3CF0F, 0x3FA3CF0F }; //  0.12797564625607904396e1
		static HK_ALIGN16(const int s1[4])				= { 0x400C9F34, 0x400C9F34, 0x400C9F34, 0x400C9F34 }; //  0.21972168858277355914e1
		static HK_ALIGN16(const int s2[4])				= { 0x40DA37C2, 0x40DA37C2, 0x40DA37C2, 0x40DA37C2 }; //  0.68193064729268275701e1
		static HK_ALIGN16(const int s3[4])				= { 0x41E1A443, 0x41E1A443, 0x41E1A443, 0x41E1A443 }; //  0.28205206687035841409e2

		hkQuadReal rx = hkMath::quadReciprocal(x);

#if defined(HK_REAL_IS_DOUBLE)
#if HK_SSE_VERSION >= 0x50
		__m128 qrx = _mm256_cvtpd_ps(rx);
		__m128 qy = _mm256_cvtpd_ps(y);
#else
		const __m128 rx_xy = _mm_cvtpd_ps(rx.xy);
		const __m128 rx_zw = _mm_cvtpd_ps(rx.zw);
		__m128 qrx = _mm_shuffle_ps(rx_xy,rx_zw,_MM_SHUFFLE(1,0,1,0));
		const __m128 y_xy = _mm_cvtpd_ps(y.xy);
		const __m128 y_zw = _mm_cvtpd_ps(y.zw);
		__m128 qy = _mm_shuffle_ps(y_xy,y_zw,_MM_SHUFFLE(1,0,1,0));
#endif
#else
		__m128 qrx = rx;
		__m128 qy = y;
#endif

		__m128 y_div_x = _mm_mul_ps(qy, qrx);
		__m128 esp4 = _mm_or_ps(_mm_and_ps(*(const __m128*)signMask, qy), *(const __m128*)pi);
		__m128 x_ge_0 = _mm_cmple_ps(_mm_setzero_ps(), qrx);

		__m128 x5 = _mm_or_ps(_mm_cmplt_ps(*(const __m128*)one, y_div_x), _mm_cmpnle_ps(*(const __m128*)minus1, y_div_x));
		__m128 x7 = x5;

		__m128 x4 = _mm_and_ps(_mm_rcp_ps(y_div_x), x5);
		x5 = _mm_andnot_ps(x5, y_div_x);
		x4 = _mm_or_ps(x4, x5);
		__m128 x0 = x4;

		x4 = _mm_mul_ps(x4, x4);

		__m128 x1 = _mm_add_ps(*(const __m128*)s0, x4);
		x1 = _mm_mul_ps(_mm_rcp_ps(x1), *(const __m128*)t0);
		__m128 x3 = _mm_add_ps(*(const __m128*)s1, x4);
		x1 = _mm_add_ps(x1, x3);

		x1 = _mm_mul_ps(_mm_rcp_ps(x1), *(const __m128*)t1);
		x5 = _mm_add_ps(*(const __m128*)s2, x4);
		x1 = _mm_add_ps(x1, x5);

		x1 = _mm_mul_ps(_mm_rcp_ps(x1), *(const __m128*)t2);
		x3 = _mm_add_ps(*(const __m128*)s3, x4);
		__m128 x6 = _mm_mul_ps(*(const __m128*)t3, x0);
		x1 = _mm_add_ps(x1, x3);

		x0 = _mm_and_ps(x0, *(const __m128*)signMask);
		x1 = _mm_mul_ps(_mm_rcp_ps(x1), x6);

		x0 = _mm_sub_ps(_mm_or_ps(x0, *(const __m128*)piOver2), x1);

		x0 = _mm_and_ps(x0, x7);
		x7 = _mm_andnot_ps(x7, x1);
		x0 = _mm_or_ps(x0, x7);

		// select case
		x1 = x0;
		x0 = _mm_and_ps(x0, x_ge_0);
		x1 = _mm_add_ps(x1, esp4);
		x3 = _mm_andnot_ps(x_ge_0, x1);
		x0 = _mm_or_ps(x0, x3);

#if defined(HK_REAL_IS_DOUBLE)
#if HK_SSE_VERSION >= 0x50
		return _mm256_cvtps_pd(x0);
#else
		hkQuadReal result;
		result.xy = _mm_cvtps_pd(x0);
		result.zw = _mm_cvtps_pd(_mm_shuffle_ps(x0,x0,_MM_SHUFFLE(1,0,3,2)));
		return result;
#endif
#else
		return x0;
#endif
	}

#	define HK_MATH_prefetch128
	HK_FORCE_INLINE static void HK_CALL prefetch128( const void* p)
	{
		_mm_prefetch( (const char*)p, _MM_HINT_NTA );
	}

#	define HK_MATH_forcePrefetch
	template<int SIZE>
	HK_FORCE_INLINE static void HK_CALL forcePrefetch( const void* p )
	{
		const char* q = (const char*)p;
		_mm_prefetch( q, _MM_HINT_NTA );
		if ( SIZE > 64){  _mm_prefetch( q + 64, _MM_HINT_NTA ); }
		if ( SIZE > 128){ _mm_prefetch( q + 128, _MM_HINT_NTA ); }
		if ( SIZE > 192){ _mm_prefetch( q + 192, _MM_HINT_NTA ); }
	}

#	define HK_MATH_hkToIntFast
	HK_FORCE_INLINE static int HK_CALL hkToIntFast( const hkDouble64 r )
	{
		return _mm_cvttsd_si32(_mm_load_sd(&r));
	}
	HK_FORCE_INLINE static int HK_CALL hkToIntFast( const hkFloat32 r )
	{
		return _mm_cvtt_ss2si(_mm_load_ss(&r));
	}

#	define HK_MATH_hkFloatToInt
	HK_FORCE_INLINE static int HK_CALL hkFloatToInt(const hkFloat32 r)
	{
		return hkMath::hkToIntFast(r);
	}
	HK_FORCE_INLINE static int HK_CALL hkFloatToInt(const hkDouble64 r)
	{
		return hkMath::hkToIntFast(r);
	}

	HK_FORCE_INLINE static hkQuadReal HK_CALL quadFabs(const hkQuadReal &v)
	{
#if defined(HK_REAL_IS_DOUBLE)
#if HK_SSE_VERSION >= 0x50
		static HK_ALIGN_REAL( const hkUint64 mask[4] ) = { 0x7fffffffffffffff, 0x7fffffffffffffff, 0x7fffffffffffffff, 0x7fffffffffffffff }; // todo constant
		return _mm256_and_pd(v, *(const hkQuadReal*)&mask);
#else
		hkQuadReal r;
		r.xy = _mm_castsi128_pd( _mm_srli_epi64( _mm_slli_epi64( _mm_castpd_si128(v.xy), 1 ), 1 ) ); 
		r.zw = _mm_castsi128_pd( _mm_srli_epi64( _mm_slli_epi64( _mm_castpd_si128(v.zw), 1 ), 1 ) ); 
		return r;
#endif
#else
		return _mm_castsi128_ps( _mm_srli_epi32( _mm_slli_epi32( _mm_castps_si128(v), 1 ), 1 ) ); 
#endif
	}

#	define HK_MATH_fabs
	HK_FORCE_INLINE static hkFloat32 HK_CALL fabs(const hkFloat32 r) 
	{ 
		const __m128 v = _mm_load_ss(&r);
		const __m128 abs = _mm_castsi128_ps( _mm_srli_epi32( _mm_slli_epi32( _mm_castps_si128(v), 1 ), 1 ) ); 
		return HK_M128(abs).m128_f32[0];
	}
	HK_FORCE_INLINE static hkDouble64 HK_CALL fabs(const hkDouble64 r) 
	{ 
		const __m128d v = _mm_load_sd(&r);
		const __m128d abs = _mm_castsi128_pd( _mm_srli_epi64( _mm_slli_epi64( _mm_castpd_si128(v), 1 ), 1 ) ); 
		return HK_M128(abs).m128d_f64[0];
	}

#	define HK_MATH_floor
	HK_FORCE_INLINE static hkFloat32 HK_CALL floor(const hkFloat32 r) 
	{ 
#if HK_SSE_VERSION >= 0x41
		const __m128 v = _mm_load_ss(&r);
		const __m128 result = _mm_floor_ss(v, v);
		return HK_M128(result).m128_f32[0];
#else
		static HK_ALIGN16( const hkUint32 two23[4] )  = { 0x4B000000, 0x4B000000, 0x4B000000, 0x4B000000 }; // 2^23 as float

		const __m128 v = _mm_load_ss(&r);
		const __m128 b = _mm_castsi128_ps( _mm_srli_epi32( _mm_slli_epi32( _mm_castps_si128(v), 1 ), 1 ) ); // fabs(v)
		const __m128 d = _mm_sub_ps( _mm_add_ps( _mm_add_ps( _mm_sub_ps( v, *(__m128*)&two23 ), *(__m128*)&two23 ), *(__m128*)&two23 ), *(__m128*)&two23 ); // the meat of floor
		const __m128 largeMaskE = _mm_cmpgt_ps( b, *(__m128*)&two23 ); // $ffffffff if v >= 2^23
		const __m128 g = _mm_cmplt_ps( v, d ); // check for possible off by one error
		const __m128 h = _mm_cvtepi32_ps( _mm_castps_si128(g) ); // convert positive check result to -1.0, negative to 0.0
		const __m128 t = _mm_add_ps( d, h ); // add in the error if there is one

		// Select between output result and input value based on v >= 2^23
		const __m128 result = _mm_or_ps( _mm_and_ps(largeMaskE, v), _mm_andnot_ps(largeMaskE, t) );

		return HK_M128(result).m128_f32[0];
#endif
	}
	HK_FORCE_INLINE static hkDouble64 HK_CALL floor(const hkDouble64 r) 
	{ 
#if HK_SSE_VERSION >= 0x41
		const __m128d v = _mm_load_sd(&r);
		const __m128d result = _mm_floor_sd(v, v);
		return HK_M128(result).m128d_f64[0];
#else
		static HK_ALIGN16( const hkUint64 two52[2] )  = { 0x4330000000000000ull, 0x4330000000000000ull }; // 2^52 as double

		const __m128d v = _mm_load_sd(&r);
		const __m128d b = _mm_castsi128_pd( _mm_srli_epi64( _mm_slli_epi64( _mm_castpd_si128(v), 1 ), 1 ) ); // fabs(v)
		const __m128d d = _mm_sub_pd( _mm_add_pd( _mm_add_pd( _mm_sub_pd( v, *(__m128d*)&two52 ), *(__m128d*)&two52 ), *(__m128d*)&two52 ), *(__m128d*)&two52 ); // the meat of floor
		const __m128d largeMaskE = _mm_cmpgt_pd( b, *(__m128d*)&two52 ); // $ffffffffffffffff if v >= 2^52
		const __m128d g = _mm_cmplt_pd( v, d ); // check for possible off by one error
		const __m128d h = _mm_cvtepi32_pd( _mm_castpd_si128(g) ); // convert positive check result to -1.0, negative to 0.0 (only need the lower 64 bit anyways, so cvtepi32 is fine)
		const __m128d t = _mm_add_pd( d, h ); // add in the error if there is one

		// Select between output result and input value based on v >= 2^52
		const __m128d result = _mm_or_pd( _mm_and_pd(largeMaskE, v), _mm_andnot_pd(largeMaskE, t) );

		return HK_M128(result).m128d_f64[0];
#endif
	}

#	define HK_MATH_hkFloor
	HK_FORCE_INLINE static hkFloat32 HK_CALL hkFloor(const hkFloat32 r)
	{
		return hkMath::floor(r);
	}
	HK_FORCE_INLINE static hkDouble64 HK_CALL hkFloor(const hkDouble64 r)
	{
		return hkMath::floor(r);
	}

#	define HK_MATH_hkFloorToInt
	HK_FORCE_INLINE static int HK_CALL hkFloorToInt(const hkFloat32 r)
	{
#if HK_SSE_VERSION >= 0x41
		const __m128 v = _mm_load_ss(&r);
		const __m128 result = _mm_floor_ss(v, v);
		return _mm_cvtt_ss2si(result);
#else
		static HK_ALIGN16( const hkUint32 two23[4] )  = { 0x4B000000, 0x4B000000, 0x4B000000, 0x4B000000 }; // 2^23 as float

		const __m128 v = _mm_load_ss(&r);
		const __m128 b = _mm_castsi128_ps( _mm_srli_epi32( _mm_slli_epi32( _mm_castps_si128(v), 1 ), 1 ) ); // fabs(v)
		const __m128 d = _mm_sub_ps( _mm_add_ps( _mm_add_ps( _mm_sub_ps( v, *(__m128*)&two23 ), *(__m128*)&two23 ), *(__m128*)&two23 ), *(__m128*)&two23 ); // the meat of floor
		const __m128 largeMaskE = _mm_cmpgt_ps( b, *(__m128*)&two23 ); // $ffffffff if v >= 2^23
		const __m128 g = _mm_cmplt_ps( v, d ); // check for possible off by one error
		const __m128 h = _mm_cvtepi32_ps( _mm_castps_si128(g) ); // convert positive check result to -1.0, negative to 0.0
		const __m128 t = _mm_add_ps( d, h ); // add in the error if there is one

		// Select between output result and input value based on v >= 2^23
		const __m128 result = _mm_or_ps( _mm_and_ps(largeMaskE, v), _mm_andnot_ps(largeMaskE, t) );

		return _mm_cvtt_ss2si(result);
#endif
	}
	HK_FORCE_INLINE static int HK_CALL hkFloorToInt(const hkDouble64 r)
	{
#if HK_SSE_VERSION >= 0x41
		const __m128d v = _mm_load_sd(&r);
		const __m128d result = _mm_floor_sd(v, v);
		return _mm_cvttsd_si32(result);
#else
		static HK_ALIGN16( const hkUint64 two52[2] )  = { 0x4330000000000000ull, 0x4330000000000000ull }; // 2^52 as double

		const __m128d v = _mm_load_sd(&r);
		const __m128d b = _mm_castsi128_pd( _mm_srli_epi64( _mm_slli_epi64( _mm_castpd_si128(v), 1 ), 1 ) ); // fabs(v)
		const __m128d d = _mm_sub_pd( _mm_add_pd( _mm_add_pd( _mm_sub_pd( v, *(__m128d*)&two52 ), *(__m128d*)&two52 ), *(__m128d*)&two52 ), *(__m128d*)&two52 ); // the meat of floor
		const __m128d largeMaskE = _mm_cmpgt_pd( b, *(__m128d*)&two52 ); // $ffffffffffffffff if v >= 2^52
		const __m128d g = _mm_cmplt_pd( v, d ); // check for possible off by one error
		const __m128d h = _mm_cvtepi32_pd( _mm_castpd_si128(g) ); // convert positive check result to -1.0, negative to 0.0 (only need the lower 64 bit anyways, so cvtepi32 is fine)
		const __m128d t = _mm_add_pd( d, h ); // add in the error if there is one

		// Select between output result and input value based on v >= 2^52
		const __m128d result = _mm_or_pd( _mm_and_pd(largeMaskE, v), _mm_andnot_pd(largeMaskE, t) );

		return _mm_cvttsd_si32(result);
#endif
	}

#if HK_SSE_VERSION >= 0x41
#	define HK_MATH_ceil
	HK_FORCE_INLINE static hkFloat32 HK_CALL ceil( const hkFloat32 r ) 
	{ 
		const __m128 v = _mm_load_ss(&r);
		const __m128 result = _mm_ceil_ss(v, v);
		return HK_M128(result).m128_f32[0];
	}
	HK_FORCE_INLINE static hkDouble64 HK_CALL ceil( const hkDouble64 r ) 
	{ 
		const __m128d v = _mm_load_sd(&r);
		const __m128d result = _mm_ceil_sd(v, v);
		return HK_M128(result).m128d_f64[0];
	}
#endif

#	define HK_MATH_logApproximation
	HK_FORCE_INLINE static hkReal HK_CALL logApproximation(const hkReal& r)
	{
#if defined(HK_REAL_IS_DOUBLE)
#if HK_SSE_VERSION >= 0x50
		hkQuadReal q = _mm256_set1_pd(r);
		hkQuadReal l = quadLog(q);
		return l.m256d_f64[0];
#else
		hkQuadReal q;
		q.xy = _mm_load1_pd(&r);
		q.zw = _mm_load1_pd(&r);
		hkQuadReal l = quadLog(q); // change this!
		return HK_M128(l.xy).m128d_f64[0];
#endif
#else
		const hkQuadReal q = _mm_load1_ps(&r);
		const hkQuadReal l = quadLog(q);
		return HK_M128(l).m128_f32[0];
#endif
	}

#	define HK_MATH_atan2Approximation
	HK_FORCE_INLINE static hkReal HK_CALL atan2Approximation( const hkReal& y, const hkReal& x )
	{
#if defined(HK_REAL_IS_DOUBLE)
#if HK_SSE_VERSION >= 0x50
		hkQuadReal qy = _mm256_set1_pd(y);
		hkQuadReal qx = _mm256_set1_pd(x);
		hkQuadReal a = quadAtan2(qy,qx);
		return a.m256d_f64[0];
#else
		hkQuadReal qy;
		qy.xy = _mm_load1_pd(&y);
		qy.zw = _mm_load1_pd(&y);
		hkQuadReal qx;
		qx.xy = _mm_load1_pd(&x);
		qx.zw = _mm_load1_pd(&x);
		hkQuadReal a = quadAtan2(qy,qx);
		return HK_M128(a.xy).m128d_f64[0];
#endif
#else
		const hkQuadReal qy = _mm_load1_ps(&y);
		const hkQuadReal qx = _mm_load1_ps(&x);
		const hkQuadReal a = quadAtan2(qy,qx);
		return HK_M128(a).m128_f32[0];
#endif
	}

	/// Average absolute error 0.001961
	/// Max absolute error 0.013429
	/// About 5x faster than ::sinf for 4 simultaneous values
	HK_FORCE_INLINE static hkQuadReal HK_CALL quadSin(const hkQuadReal& inX)
	{
		static HK_ALIGN16( const hkUint32 twopi[4] )  = { 0x40C90FDB, 0x40C90FDB, 0x40C90FDB, 0x40C90FDB };
		static HK_ALIGN16( const hkUint32 pi[4] )     = { 0x40490FDB, 0x40490FDB, 0x40490FDB, 0x40490FDB };
		static HK_ALIGN16( const hkUint32 B[4] )      = { 0x3FA2F983, 0x3FA2F983, 0x3FA2F983, 0x3FA2F983 }; // 4/pi
		static HK_ALIGN16( const hkUint32 C[4] )      = { 0xBECF817B, 0xBECF817B, 0xBECF817B, 0xBECF817B }; // -4/(pi*pi)
		static HK_ALIGN16( const hkUint32 P[4] )      = { 0x3E666666, 0x3E666666, 0x3E666666, 0x3E666666 }; // 0.225

#if defined(HK_REAL_IS_DOUBLE)
#if HK_SSE_VERSION >= 0x50
		__m128 x = _mm256_cvtpd_ps(inX);
#else
		const __m128 x_xy = _mm_cvtpd_ps(inX.xy);
		const __m128 x_zw = _mm_cvtpd_ps(inX.zw);
		__m128 x = _mm_shuffle_ps(x_xy,x_zw,_MM_SHUFFLE(1,0,1,0));
#endif
#else
		__m128 x = inX;
#endif

		// xx = remap to [0,pi]
		__m128 gePi = _mm_cmpnlt_ps(x, *(__m128*)&pi);
		__m128 limit = _mm_and_ps(gePi, *(__m128*)&twopi);
		__m128 xx = _mm_sub_ps(x, limit);

		// y = B * x + C * x * abs(xx)
		__m128 abs = _mm_castsi128_ps( _mm_srli_epi32( _mm_slli_epi32( _mm_castps_si128(xx), 1 ), 1 ) ); 
		__m128 y = _mm_mul_ps(abs, xx);
		__m128 Bx = _mm_mul_ps(xx, *(__m128*)&B);
		__m128 Cx_abs = _mm_mul_ps(y, *(__m128*)&C);
		__m128 parabola = _mm_add_ps(Bx, Cx_abs);

		// y = P * (y * abs(y) - y) + y
		abs = _mm_castsi128_ps( _mm_srli_epi32( _mm_slli_epi32( _mm_castps_si128(parabola), 1 ), 1 ) ); 
		y = _mm_sub_ps(_mm_mul_ps(parabola, abs),parabola);
		__m128 blend = _mm_mul_ps(y, *(__m128*)&P);
		__m128 result = _mm_add_ps(blend, parabola);

#if defined(HK_REAL_IS_DOUBLE)
#if HK_SSE_VERSION >= 0x50
		return _mm256_cvtps_pd(result);
#else
		hkQuadReal res;
		res.xy = _mm_cvtps_pd(result);
		res.zw = _mm_cvtps_pd(_mm_shuffle_ps(result,result,_MM_SHUFFLE(1,0,3,2)));
		return res;
#endif
#else
		return result;
#endif
	}

	/// Average absolute error 0.001952
	/// Max absolute error 0.013619
	/// About 5x faster than ::cosf for 4 simultaneous values
	HK_FORCE_INLINE static hkQuadReal HK_CALL quadCos(const hkQuadReal& x)
	{
		// cos(x) = sin(x + pi/2)
#if defined(HK_REAL_IS_DOUBLE)
#if HK_SSE_VERSION >= 0x50
		static HK_ALIGN32( const hkUint64 pihalf[4] ) = { 0x3FF921FB54442D18ull, 0x3FF921FB54442D18ull, 0x3FF921FB54442D18ull, 0x3FF921FB54442D18ull };
		__m256d xx = _mm256_add_pd(x, *(__m256d*)&pihalf);
#else
		static HK_ALIGN16( const hkUint64 pihalf[2] ) = { 0x3FF921FB54442D18ull, 0x3FF921FB54442D18ull };
		hkQuadReal xx;
		xx.xy = _mm_add_pd(x.xy, *(__m128d*)&pihalf);
		xx.zw = _mm_add_pd(x.zw, *(__m128d*)&pihalf);
#endif
#else
		static HK_ALIGN16( const hkUint32 pihalf[4] ) = { 0x3FC90FDB, 0x3FC90FDB, 0x3FC90FDB, 0x3FC90FDB };
		__m128 xx = _mm_add_ps(x, *(__m128*)&pihalf);
#endif
		return quadSin(xx);
	}

	/// Calc cos/sin at the same time: xyzw -> sin(x)cos(y)sin(z)cos(w)
	HK_FORCE_INLINE static hkQuadReal HK_CALL quadSinCos(const hkQuadReal& x)
	{
		// cos(x) = sin(x + pi/2)
#if defined(HK_REAL_IS_DOUBLE)
#if HK_SSE_VERSION >= 0x50
		static HK_ALIGN32( const hkUint64 offset[4] ) = { 0x0000000000000000ull, 0x3FF921FB54442D18ull, 0x0000000000000000ull, 0x3FF921FB54442D18ull };
		__m256d xx = _mm256_add_pd(x, *(__m256d*)&offset);
#else
		static HK_ALIGN16( const hkUint64 offset[2] ) = { 0x0000000000000000ull, 0x3FF921FB54442D18ull };
		hkQuadReal xx;
		xx.xy = _mm_add_pd(x.xy, *(__m128d*)&offset);
		xx.zw = _mm_add_pd(x.zw, *(__m128d*)&offset);
#endif
#else
		static HK_ALIGN16( const hkUint32 offset[4] ) = { 0x00000000, 0x3FC90FDB, 0x00000000, 0x3FC90FDB };
		hkQuadReal xx = _mm_add_ps(x, *(__m128*)&offset);
#endif
		return quadSin(xx);
	}

#	define HK_MATH_sinApproximation
	HK_FORCE_INLINE static hkReal HK_CALL sinApproximation( const hkReal& x )
	{
#if defined(HK_REAL_IS_DOUBLE)
#if HK_SSE_VERSION >= 0x50
		hkQuadReal qx = _mm256_set1_pd(x);
		hkQuadReal a = quadSin(qx);
		return a.m256d_f64[0];
#else
		hkQuadReal qx;
		qx.xy = _mm_load1_pd(&x);
		qx.zw = _mm_load1_pd(&x);
		hkQuadReal a = quadSin(qx);
		return HK_M128(a.xy).m128d_f64[0];
#endif
#else
		const hkQuadReal qx = _mm_load1_ps(&x);
		const hkQuadReal a = quadSin(qx);
		return HK_M128(a).m128_f32[0];
#endif
	}

#	define HK_MATH_cosApproximation
	HK_FORCE_INLINE static hkReal HK_CALL cosApproximation( const hkReal& x )
	{
#if defined(HK_REAL_IS_DOUBLE)
#if HK_SSE_VERSION >= 0x50
		hkQuadReal qx = _mm256_set1_pd(x);
		hkQuadReal a = quadCos(qx);
		return a.m256d_f64[0];
#else
		hkQuadReal qx;
		qx.xy = _mm_load1_pd(&x);
		qx.zw = _mm_load1_pd(&x);
		hkQuadReal a = quadCos(qx);
		return HK_M128(a.xy).m128d_f64[0];
#endif
#else
		const hkQuadReal qx = _mm_load1_ps(&x);
		const hkQuadReal a = quadCos(qx);
		return HK_M128(a).m128_f32[0];
#endif
	}

}

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
