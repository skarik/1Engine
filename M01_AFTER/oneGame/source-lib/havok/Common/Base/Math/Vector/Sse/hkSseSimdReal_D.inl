/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#if !defined(HK_REAL_IS_DOUBLE)
#error This implementation is for 64-Bit double
#endif

HK_FORCE_INLINE /*static*/ const hkSimdReal HK_CALL hkSimdReal::convert(const hkSingleReal& x)
{
	hkSimdReal sr;
	sr.m_real = x;
	return sr;
	//HK_ASSERT(0x7c3a4d98, *(hkUint64*)&(x.m128d_f64[0]) == *(hkUint64*)&(x.m128d_f64[1]) );
}

template<int vectorConstant>
HK_FORCE_INLINE /*static*/ const hkSimdReal HK_CALL hkSimdReal::getConstant()
{
	HK_COMPILE_TIME_ASSERT2( 
		(vectorConstant!=HK_QUADREAL_1000) && (vectorConstant!=HK_QUADREAL_0100) && (vectorConstant!=HK_QUADREAL_0010) && (vectorConstant!=HK_QUADREAL_0001) &&
		(vectorConstant!=HK_QUADREAL_m11m11) && (vectorConstant!=HK_QUADREAL_1248) && (vectorConstant!=HK_QUADREAL_8421)
		, HK_SIMDREAL_ILLEGAL_CONSTANT_REQUEST);
#if HK_SSE_VERSION >= 0x50
	return convert(*(const hkSingleReal*)(g_vectorConstants + vectorConstant));
#else
	return convert((*(g_vectorConstants + vectorConstant)).xy);
#endif
}

HK_FORCE_INLINE /*static*/ const hkSimdReal HK_CALL hkSimdReal::getConstant(hkVectorConstant constant)
{
	HK_ASSERT2( 0x909ff234,
		(constant!=HK_QUADREAL_1000) && (constant!=HK_QUADREAL_0100) && (constant!=HK_QUADREAL_0010) && (constant!=HK_QUADREAL_0001) &&
		(constant!=HK_QUADREAL_m11m11) && (constant!=HK_QUADREAL_1248) && (constant!=HK_QUADREAL_8421)
		, "not a simdreal constant");
#if HK_SSE_VERSION >= 0x50
	return convert(*(const hkSingleReal*)(g_vectorConstants + constant));
#else
	return convert((*(g_vectorConstants + constant)).xy);
#endif
}

#ifndef HK_DISABLE_IMPLICIT_SIMDREAL_FLOAT_CONVERSION
HK_FORCE_INLINE hkSimdReal::hkSimdReal(const hkReal& x)
{
	m_real = _mm_set1_pd(x);
}

HK_FORCE_INLINE hkSimdReal::operator hkReal() const
{
	hkReal s;
	_mm_store_sd(&s, m_real);
	return s;
}
#endif

HK_FORCE_INLINE hkReal hkSimdReal::getReal() const
{
	hkReal s;
	_mm_store_sd(&s, m_real);
	return s;
}

HK_FORCE_INLINE void hkSimdReal::setFromFloat(const hkReal& x)
{
	m_real = _mm_set1_pd(x);
}

HK_FORCE_INLINE void hkSimdReal::setFromHalf(const hkHalf& h)
{
	float x = h;
	m_real = _mm_set1_pd(double(x));
}

HK_FORCE_INLINE void hkSimdReal::setFromInt32(const hkInt32&  x)
{
	m_real = _mm_cvtepi32_pd( _mm_set1_epi32(x) );
}


HK_FORCE_INLINE void hkSimdReal::setFromUint16(const hkUint16& x)
{
	int i = x;
	setFromInt32(i);
}

HK_FORCE_INLINE void hkSimdReal::setFromUint8(const hkUint8& x)
{
	int i = x;
	setFromInt32(i);
}

HK_FORCE_INLINE void hkSimdReal::setZero()
{
	m_real = _mm_setzero_pd();
}


HK_FORCE_INLINE void hkSimdReal::storeSaturateInt32(hkInt32* HK_RESTRICT result) const
{
	*result = _mm_cvtsi128_si32(_mm_cvttpd_epi32(m_real));
}


HK_FORCE_INLINE void hkSimdReal::storeSaturateUint16(hkUint16* HK_RESTRICT result) const
{
	hkSingleReal clamped   = _mm_max_pd(m_real,_mm_setzero_pd());	
	clamped = _mm_min_pd(clamped,_mm_set1_pd(65535.0));
	*result = hkUint16(_mm_cvtsi128_si32(_mm_cvttpd_epi32(clamped)));
}

HK_FORCE_INLINE const hkSimdReal hkSimdReal::operator+ (hkSimdRealParameter r) const
{
	return hkSimdReal::convert(_mm_add_pd(m_real,r.m_real));
}

HK_FORCE_INLINE const hkSimdReal hkSimdReal::operator- (hkSimdRealParameter r) const
{
	return hkSimdReal::convert(_mm_sub_pd(m_real,r.m_real));
}

HK_FORCE_INLINE const hkSimdReal hkSimdReal::operator* (hkSimdRealParameter r) const
{
	return hkSimdReal::convert(_mm_mul_pd(m_real,r.m_real));
}

#ifdef HK_DISABLE_OLD_VECTOR4_INTERFACE

HK_FORCE_INLINE bool hkSimdReal::operator< (hkSimdRealParameter r) const
{
	return _mm_ucomilt_sd(m_real, r.m_real) ? true : false;
}

HK_FORCE_INLINE bool hkSimdReal::operator<= (hkSimdRealParameter r) const
{
	return _mm_ucomile_sd(m_real, r.m_real) ? true : false;
}

HK_FORCE_INLINE bool hkSimdReal::operator> (hkSimdRealParameter r) const
{
	return _mm_ucomigt_sd(m_real, r.m_real) ? true : false;
}

HK_FORCE_INLINE bool hkSimdReal::operator>= (hkSimdRealParameter r) const
{
	return _mm_ucomige_sd(m_real, r.m_real) ? true : false;
}

HK_FORCE_INLINE bool hkSimdReal::operator== (hkSimdRealParameter r) const
{
	return _mm_ucomieq_sd(m_real, r.m_real) ? true : false;
}

HK_FORCE_INLINE bool hkSimdReal::operator!= (hkSimdRealParameter r) const
{
	return _mm_ucomineq_sd(m_real, r.m_real) ? true : false;
}

#endif

HK_FORCE_INLINE const hkSimdReal hkSimdReal::operator-() const
{
	return hkSimdReal::convert(_mm_sub_pd(_mm_setzero_pd(),m_real));
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::less(hkSimdRealParameter a) const
{
	hkVector4Comparison mask;
	const hkSingleReal m = _mm_cmplt_pd(m_real, a.m_real);
#if HK_SSE_VERSION >= 0x50
	mask.m_mask = _mm256_broadcast_pd(&m);
#else
	mask.m_mask.xy = m;
	mask.m_mask.zw = m;
#endif
	return mask;
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::greater(hkSimdRealParameter a) const
{
	hkVector4Comparison mask;
	const hkSingleReal m = _mm_cmpgt_pd(m_real, a.m_real);
#if HK_SSE_VERSION >= 0x50
	mask.m_mask = _mm256_broadcast_pd(&m);
#else
	mask.m_mask.xy = m;
	mask.m_mask.zw = m;
#endif
	return mask;
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::greaterEqual(hkSimdRealParameter a) const
{
	hkVector4Comparison mask;
	const hkSingleReal m = _mm_cmpge_pd(m_real, a.m_real);
#if HK_SSE_VERSION >= 0x50
	mask.m_mask = _mm256_broadcast_pd(&m);
#else
	mask.m_mask.xy = m;
	mask.m_mask.zw = m;
#endif
	return mask;
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::lessEqual(hkSimdRealParameter a) const
{
	hkVector4Comparison mask;
	const hkSingleReal m = _mm_cmple_pd(m_real, a.m_real);
#if HK_SSE_VERSION >= 0x50
	mask.m_mask = _mm256_broadcast_pd(&m);
#else
	mask.m_mask.xy = m;
	mask.m_mask.zw = m;
#endif
	return mask;
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::equal(hkSimdRealParameter a) const
{
	hkVector4Comparison mask;
	const hkSingleReal m = _mm_cmpeq_pd(m_real, a.m_real);
#if HK_SSE_VERSION >= 0x50
	mask.m_mask = _mm256_broadcast_pd(&m);
#else
	mask.m_mask.xy = m;
	mask.m_mask.zw = m;
#endif
	return mask;
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::notEqual(hkSimdRealParameter a) const
{
	hkVector4Comparison mask;
	const hkSingleReal m = _mm_cmpneq_pd(m_real, a.m_real);
#if HK_SSE_VERSION >= 0x50
	mask.m_mask = _mm256_broadcast_pd(&m);
#else
	mask.m_mask.xy = m;
	mask.m_mask.zw = m;
#endif
	return mask;
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::lessZero() const
{
	hkVector4Comparison mask;
	const hkSingleReal m = _mm_cmplt_pd(m_real, _mm_setzero_pd());
#if HK_SSE_VERSION >= 0x50
	mask.m_mask = _mm256_broadcast_pd(&m);
#else
	mask.m_mask.xy = m;
	mask.m_mask.zw = m;
#endif
	return mask;
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::lessEqualZero() const
{
	hkVector4Comparison mask;
	const hkSingleReal m = _mm_cmple_pd(m_real, _mm_setzero_pd());
#if HK_SSE_VERSION >= 0x50
	mask.m_mask = _mm256_broadcast_pd(&m);
#else
	mask.m_mask.xy = m;
	mask.m_mask.zw = m;
#endif
	return mask;
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::greaterZero() const
{
	hkVector4Comparison mask;
	const hkSingleReal m = _mm_cmpgt_pd(m_real, _mm_setzero_pd());
#if HK_SSE_VERSION >= 0x50
	mask.m_mask = _mm256_broadcast_pd(&m);
#else
	mask.m_mask.xy = m;
	mask.m_mask.zw = m;
#endif
	return mask;
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::greaterEqualZero() const
{
	hkVector4Comparison mask;
	const hkSingleReal m = _mm_cmpge_pd(m_real, _mm_setzero_pd());
#if HK_SSE_VERSION >= 0x50
	mask.m_mask = _mm256_broadcast_pd(&m);
#else
	mask.m_mask.xy = m;
	mask.m_mask.zw = m;
#endif
	return mask;
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::equalZero() const
{
	hkVector4Comparison mask;
	const hkSingleReal m = _mm_cmpeq_pd(m_real, _mm_setzero_pd());
#if HK_SSE_VERSION >= 0x50
	mask.m_mask = _mm256_broadcast_pd(&m);
#else
	mask.m_mask.xy = m;
	mask.m_mask.zw = m;
#endif
	return mask;
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::notEqualZero() const
{
	hkVector4Comparison mask;
	const hkSingleReal m = _mm_cmpneq_pd(m_real, _mm_setzero_pd());
#if HK_SSE_VERSION >= 0x50
	mask.m_mask = _mm256_broadcast_pd(&m);
#else
	mask.m_mask.xy = m;
	mask.m_mask.zw = m;
#endif
	return mask;
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isSignBitSet() const
{
	return _mm_movemask_pd(m_real);
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isSignBitClear() const
{
	return !_mm_movemask_pd(m_real);
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::signBitSet() const
{
	hkVector4Comparison mask;
	static HK_ALIGN16( const hkUint64 signmask[2] ) = { 0x8000000000000000, 0x8000000000000000 };
	const hkSingleReal c = _mm_and_pd(m_real, *(const hkSingleReal*)&signmask);
#if HK_SSE_VERSION >= 0x41
	const hkSingleReal m = _mm_castsi128_pd(_mm_cmpeq_epi64(_mm_castpd_si128(c), *(const __m128i*)&signmask));
#else
	const __m128i mm = _mm_cmpeq_epi32(_mm_castpd_si128(c), *(const __m128i*)&signmask);
	const hkSingleReal m = _mm_castsi128_pd(_mm_shuffle_epi32(mm, _MM_SHUFFLE(2,2,0,0)));
#endif
#if HK_SSE_VERSION >= 0x50
	mask.m_mask = _mm256_broadcast_pd(&m);
#else
	mask.m_mask.xy = m;
	mask.m_mask.zw = m;
#endif
	return mask;
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::signBitClear() const
{
	hkVector4Comparison mask;
	static HK_ALIGN16( const hkUint64 signmask[2] ) = { 0x7fffffffffffffff, 0x7fffffffffffffff };
	const hkSingleReal c = _mm_and_pd(m_real, *(const hkSingleReal*)&signmask);
	const hkSingleReal m = _mm_cmpeq_pd(c, m_real);
#if HK_SSE_VERSION >= 0x50
	mask.m_mask = _mm256_broadcast_pd(&m);
#else
	mask.m_mask.xy = m;
	mask.m_mask.zw = m;
#endif
	return mask;
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isLess(hkSimdRealParameter a) const
{
	return _mm_ucomilt_sd(m_real, a.m_real);
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isLessEqual(hkSimdRealParameter a) const
{
	return _mm_ucomile_sd(m_real, a.m_real);
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isGreater(hkSimdRealParameter a) const
{
	return _mm_ucomigt_sd(m_real, a.m_real);
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isGreaterEqual(hkSimdRealParameter a) const
{
	return _mm_ucomige_sd(m_real, a.m_real);
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isEqual(hkSimdRealParameter a) const
{
	return _mm_ucomieq_sd(m_real, a.m_real);
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isNotEqual(hkSimdRealParameter a) const
{
	return _mm_ucomineq_sd(m_real, a.m_real);
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isLessZero() const
{
	return _mm_ucomilt_sd(m_real, _mm_setzero_pd());
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isLessEqualZero() const
{
	return _mm_ucomile_sd(m_real, _mm_setzero_pd());
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isGreaterZero() const
{
	return _mm_ucomigt_sd(m_real, _mm_setzero_pd());
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isGreaterEqualZero() const
{
	return _mm_ucomige_sd(m_real, _mm_setzero_pd());
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isEqualZero() const
{
	return _mm_ucomieq_sd(m_real, _mm_setzero_pd());
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isNotEqualZero() const
{
	return _mm_ucomineq_sd(m_real, _mm_setzero_pd());
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isOk() const
{
	const hkSingleReal nanMask = _mm_cmpunord_pd(m_real, _mm_setzero_pd());
	return !_mm_movemask_pd(nanMask);
}

HK_FORCE_INLINE void hkSimdReal::setSelect( hkVector4ComparisonParameter comp, hkSimdRealParameter trueValue, hkSimdRealParameter falseValue )
{
	HK_ASSERT2(0xc78f08e, comp.allAreSet() || (comp.getMask() == hkVector4Comparison::MASK_NONE), "illegal compare mask");
#if HK_SSE_VERSION >= 0x50
	m_real = _mm_blendv_pd(falseValue.m_real, trueValue.m_real, _mm256_extractf128_pd(comp.m_mask,0));
#elif HK_SSE_VERSION >= 0x41
	m_real = _mm_blendv_pd(falseValue.m_real, trueValue.m_real, comp.m_mask.xy);
#else
	m_real = _mm_or_pd( _mm_and_pd(comp.m_mask.xy, trueValue.m_real), _mm_andnot_pd(comp.m_mask.xy, falseValue.m_real) );
#endif
}

HK_FORCE_INLINE void hkSimdReal::setMin(  hkSimdRealParameter a, hkSimdRealParameter b ) 
{
	m_real = _mm_min_pd( a.m_real, b.m_real );
}

HK_FORCE_INLINE void hkSimdReal::setMax(  hkSimdRealParameter a, hkSimdRealParameter b ) 
{
	m_real = _mm_max_pd( a.m_real, b.m_real );
}

HK_FORCE_INLINE void hkSimdReal::setAbs(  hkSimdRealParameter a )
{
	static HK_ALIGN16( const hkUint64 absMask[2] ) = { 0x7fffffffffffffff, 0x7fffffffffffffff }; // todo constant
	m_real = _mm_and_pd(a.m_real, *(const hkSingleReal*)&absMask);
}

HK_FORCE_INLINE void hkSimdReal::setFlipSign(hkSimdRealParameter v, hkSimdRealParameter sSign)
{
	static HK_ALIGN16( const hkUint64 signmask[2] ) = { 0x8000000000000000, 0x8000000000000000 };
	m_real = _mm_xor_pd(v.m_real, _mm_and_pd(sSign.m_real, *(const hkSingleReal*)&signmask));
}

HK_FORCE_INLINE void hkSimdReal::setFlipSign(hkSimdRealParameter v, hkVector4ComparisonParameter mask)
{
	HK_ASSERT2(0x5831c04, mask.allAreSet() || (mask.getMask() == hkVector4Comparison::MASK_NONE), "illegal compare mask");
	static HK_ALIGN16( const hkUint64 signmask[2] ) = { 0x8000000000000000, 0x8000000000000000 };
#if HK_SSE_VERSION >= 0x50
	m_real = _mm_xor_pd(v.m_real, _mm_and_pd(_mm256_extractf128_pd(mask.m_mask,0), *(const hkSingleReal*)&signmask));
#else
	m_real = _mm_xor_pd(v.m_real, _mm_and_pd(mask.m_mask.xy, *(const hkSingleReal*)&signmask));
#endif
}


//
// advanced interface
//

namespace hkSimdReal_AdvancedInterface
{

template <hkMathAccuracyMode A, hkMathDivByZeroMode D>
struct unroll_setReciprocal { HK_FORCE_INLINE static void apply(hkSingleReal& self, hkSimdRealParameter a)
{
	HK_SIMDREAL_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_IGNORE> { HK_FORCE_INLINE static void apply(hkSingleReal& self, hkSimdRealParameter a)
{
	switch (A)
	{
		case HK_ACC_23_BIT: 
			{
				const __m128 fr = _mm_cvtpd_ps(a.m_real);
				const __m128 r = _mm_rcp_ps(fr);
				const hkSingleReal rb = _mm_cvtps_pd(r);
				// One Newton-Raphson refinement iteration
				const hkSingleReal rbr = _mm_mul_pd(a.m_real, rb);
				const hkSingleReal d = _mm_sub_pd(*(const hkSingleReal*)(g_vectorConstants + HK_QUADREAL_2), rbr);
				self = _mm_mul_pd(rb, d);
			}
			break;
		case HK_ACC_12_BIT: 
			{
				const __m128 fr = _mm_cvtpd_ps(a.m_real);
				const __m128 r = _mm_rcp_ps(fr);
				self = _mm_cvtps_pd(r);
			}
			break;
		default:
			{
				self = _mm_div_pd(*(const hkSingleReal*)(g_vectorConstants + HK_QUADREAL_1),a.m_real); 
			}
			break; // HK_ACC_FULL
	}
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_SET_ZERO> { HK_FORCE_INLINE static void apply(hkSingleReal& self, hkSimdRealParameter a)
{
	const hkSingleReal equalsZero = _mm_cmpeq_pd(a.m_real, _mm_setzero_pd());
	hkSingleReal e; unroll_setReciprocal<A, HK_DIV_IGNORE>::apply(e, a);
	self = _mm_andnot_pd(equalsZero, e);
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_SET_HIGH> { HK_FORCE_INLINE static void apply(hkSingleReal& self, hkSimdRealParameter a)
{
	const hkSingleReal equalsZero = _mm_cmpeq_pd(a.m_real, _mm_setzero_pd());
	hkSingleReal e; unroll_setReciprocal<A, HK_DIV_IGNORE>::apply(e, a);
	const hkSingleReal huge = _mm_set1_pd(HK_REAL_HIGH);
#if HK_SSE_VERSION >= 0x41
	self = _mm_blendv_pd(e, huge, equalsZero);
#else
	self = _mm_or_pd( _mm_and_pd(equalsZero, huge), _mm_andnot_pd(equalsZero, e) );
#endif
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_SET_MAX> { HK_FORCE_INLINE static void apply(hkSingleReal& self, hkSimdRealParameter a)
{
	const hkSingleReal equalsZero = _mm_cmpeq_pd(a.m_real, _mm_setzero_pd());
	hkSingleReal e; unroll_setReciprocal<A, HK_DIV_IGNORE>::apply(e, a);
	const hkSingleReal huge = _mm_set1_pd(HK_REAL_MAX);
#if HK_SSE_VERSION >= 0x41
	self = _mm_blendv_pd(e, huge, equalsZero);
#else
	self = _mm_or_pd( _mm_and_pd(equalsZero, huge), _mm_andnot_pd(equalsZero, e) );
#endif
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_SET_ZERO_AND_ONE> { HK_FORCE_INLINE static void apply(hkSingleReal& self, hkSimdRealParameter a)
{
	static HK_ALIGN16( const hkUint64 absMask[2] ) = { 0x7fffffffffffffff, 0x7fffffffffffffff }; // todo constant
	unroll_setReciprocal<A, HK_DIV_SET_ZERO>::apply(self, a);
	const hkSingleReal one = g_vectorConstants[HK_QUADREAL_1].xy;
	const hkSingleReal absVal = _mm_and_pd(self, *(const hkSingleReal*)&absMask);
	const hkSingleReal absValLessOne = _mm_sub_pd(absVal, one);
	const hkSingleReal lessEqualEps = _mm_cmple_pd(absValLessOne, g_vectorConstants[HK_QUADREAL_EPS].xy);
#if HK_SSE_VERSION >= 0x41
	self = _mm_blendv_pd(self, one, lessEqualEps);
#else
	self = _mm_or_pd( _mm_and_pd(lessEqualEps, one), _mm_andnot_pd(lessEqualEps, self) );
#endif
} };

} // namespace 

template <hkMathAccuracyMode A, hkMathDivByZeroMode D> 
HK_FORCE_INLINE void hkSimdReal::setReciprocal(hkSimdRealParameter a)
{
	hkSimdReal_AdvancedInterface::unroll_setReciprocal<A,D>::apply(m_real,a);
}

HK_FORCE_INLINE void hkSimdReal::setReciprocal(hkSimdRealParameter a)
{
	hkSimdReal_AdvancedInterface::unroll_setReciprocal<HK_ACC_23_BIT,HK_DIV_IGNORE>::apply(m_real,a);
}



namespace hkSimdReal_AdvancedInterface
{

template <hkMathAccuracyMode A, hkMathDivByZeroMode D>
struct unroll_setDiv { HK_FORCE_INLINE static void apply(hkSimdReal& self, hkSimdRealParameter a, hkSimdRealParameter b)
{
	hkSingleReal t;
	unroll_setReciprocal<A,D>::apply(t,b);
	self.m_real = _mm_mul_pd(a.m_real,t);
} };
template <hkMathAccuracyMode A>
struct unroll_setDiv<A, HK_DIV_IGNORE> { HK_FORCE_INLINE static void apply(hkSimdReal& self, hkSimdRealParameter a, hkSimdRealParameter b)
{
	if (A == HK_ACC_FULL) 
	{ 
		self.m_real = _mm_div_pd(a.m_real, b.m_real); 
	}
	else
	{
		hkSingleReal t;
		unroll_setReciprocal<A,HK_DIV_IGNORE>::apply(t,b);
		self.m_real = _mm_mul_pd(a.m_real,t);
	}
} };

} // namespace 

template <hkMathAccuracyMode A, hkMathDivByZeroMode D> 
HK_FORCE_INLINE void hkSimdReal::setDiv(hkSimdRealParameter a, hkSimdRealParameter b)
{
	hkSimdReal_AdvancedInterface::unroll_setDiv<A,D>::apply(*this,a,b);
}

HK_FORCE_INLINE void hkSimdReal::setDiv(hkSimdRealParameter a, hkSimdRealParameter b)
{
	hkSimdReal_AdvancedInterface::unroll_setDiv<HK_ACC_23_BIT,HK_DIV_IGNORE>::apply(*this,a,b);
}

template <hkMathAccuracyMode A, hkMathDivByZeroMode D> 
HK_FORCE_INLINE void hkSimdReal::div(hkSimdRealParameter a)
{
	setDiv<A,D>( *this, a );
}

HK_FORCE_INLINE void hkSimdReal::div(hkSimdRealParameter a)
{
	setDiv( *this, a );
}

HK_FORCE_INLINE const hkSimdReal hkSimdReal::operator/ (hkSimdRealParameter r) const
{
	hkSimdReal q;
	q.setDiv(*this,r);
	return q;
}


namespace hkSimdReal_AdvancedInterface
{

template <hkMathAccuracyMode A, hkMathNegSqrtMode S>
struct unroll_sqrt { HK_FORCE_INLINE static hkSingleReal apply(hkSimdRealParameter self)
{
	HK_SIMDREAL_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
	return _mm_setzero_pd();
} };
template <hkMathAccuracyMode A>
struct unroll_sqrt<A, HK_SQRT_IGNORE> { HK_FORCE_INLINE static hkSingleReal apply(hkSimdRealParameter self)
{
	switch (A)
	{
		case HK_ACC_23_BIT: 
			{
				const __m128 fr = _mm_cvtpd_ps(self.m_real);
				const __m128 re = _mm_rsqrt_ps(fr);
				const hkSingleReal e = _mm_cvtps_pd(re);
				// One Newton-Raphson refinement iteration
				const hkSingleReal he = _mm_mul_pd(*(const hkSingleReal*)(g_vectorConstants + HK_QUADREAL_INV_2),e);
				const hkSingleReal ree = _mm_mul_pd(_mm_mul_pd(self.m_real,e),e);
				hkSingleReal refined = _mm_mul_pd(he, _mm_sub_pd(*(const hkSingleReal*)(g_vectorConstants + HK_QUADREAL_3), ree) );
				return _mm_mul_pd(self.m_real, refined);
			}
			break;
		case HK_ACC_12_BIT: 
			{
				const __m128 fr = _mm_cvtpd_ps(self.m_real);
				const __m128 re = _mm_rsqrt_ps(fr);
				hkSingleReal e = _mm_cvtps_pd(re);
				return _mm_mul_pd(self.m_real, e);
			}
			break;
		default:
			{
				return _mm_sqrt_pd(self.m_real); 
			}
			break; // HK_ACC_FULL
	}
} };
template <hkMathAccuracyMode A>
struct unroll_sqrt<A, HK_SQRT_SET_ZERO> { HK_FORCE_INLINE static hkSingleReal apply(hkSimdRealParameter self)
{
	const hkSingleReal equalsZero = _mm_cmple_pd(self.m_real, _mm_setzero_pd());
	const hkSingleReal e = unroll_sqrt<A, HK_SQRT_IGNORE>::apply(self);
	return _mm_andnot_pd(equalsZero, e);
} };

} // namespace 

template <hkMathAccuracyMode A, hkMathNegSqrtMode S> 
HK_FORCE_INLINE const hkSimdReal hkSimdReal::sqrt() const
{
	return hkSimdReal::convert(hkSimdReal_AdvancedInterface::unroll_sqrt<A,S>::apply(*this));
}

HK_FORCE_INLINE const hkSimdReal hkSimdReal::sqrt() const
{
	return hkSimdReal::convert(hkSimdReal_AdvancedInterface::unroll_sqrt<HK_ACC_23_BIT,HK_SQRT_SET_ZERO>::apply(*this));
}


namespace hkSimdReal_AdvancedInterface
{

template <hkMathAccuracyMode A, hkMathNegSqrtMode S>
struct unroll_sqrtInverse { HK_FORCE_INLINE static hkSingleReal apply(hkSimdRealParameter self)
{
	HK_SIMDREAL_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
	return _mm_setzero_pd();
} };
template <hkMathAccuracyMode A>
struct unroll_sqrtInverse<A, HK_SQRT_IGNORE> { HK_FORCE_INLINE static hkSingleReal apply(hkSimdRealParameter self)
{
	switch (A)
	{
		case HK_ACC_23_BIT: 
			{
				const __m128 fr = _mm_cvtpd_ps(self.m_real);
				const __m128 re = _mm_rsqrt_ps(fr);
				const hkSingleReal e = _mm_cvtps_pd(re);
				// One Newton-Raphson refinement iteration
				const hkSingleReal he = _mm_mul_pd(*(const hkSingleReal*)(g_vectorConstants + HK_QUADREAL_INV_2),e);
				const hkSingleReal ree = _mm_mul_pd(_mm_mul_pd(self.m_real,e),e);
				return _mm_mul_pd(he, _mm_sub_pd(*(const hkSingleReal*)(g_vectorConstants + HK_QUADREAL_3), ree) );
			}
			break;
		case HK_ACC_12_BIT: 
			{
				const __m128 fr = _mm_cvtpd_ps(self.m_real);
				const __m128 re = _mm_rsqrt_ps(fr);
				return _mm_cvtps_pd(re);
			}
			break;
		default:         
			{
				return _mm_div_pd(*(const hkSingleReal*)(g_vectorConstants + HK_QUADREAL_1), _mm_sqrt_pd(self.m_real));
			}
			break; // HK_ACC_FULL
	}
} };
template <hkMathAccuracyMode A>
struct unroll_sqrtInverse<A, HK_SQRT_SET_ZERO> { HK_FORCE_INLINE static hkSingleReal apply(hkSimdRealParameter self)
{
	const hkSingleReal equalsZero = _mm_cmple_pd(self.m_real, _mm_setzero_pd());
	const hkSingleReal e = unroll_sqrtInverse<A, HK_SQRT_IGNORE>::apply(self);
	return _mm_andnot_pd(equalsZero, e);
} };
} // namespace 

template <hkMathAccuracyMode A, hkMathNegSqrtMode S> 
HK_FORCE_INLINE const hkSimdReal hkSimdReal::sqrtInverse() const
{
	return hkSimdReal::convert(hkSimdReal_AdvancedInterface::unroll_sqrtInverse<A,S>::apply(*this));
}

HK_FORCE_INLINE const hkSimdReal hkSimdReal::sqrtInverse() const
{
	return hkSimdReal::convert(hkSimdReal_AdvancedInterface::unroll_sqrtInverse<HK_ACC_23_BIT,HK_SQRT_SET_ZERO>::apply(*this));
}


template <int N, hkMathIoMode A>
HK_FORCE_INLINE void hkSimdReal::load(const hkReal *p )
{
	HK_SIMDREAL_DIMENSION_CHECK;
	m_real = _mm_load1_pd(p);
}

template <int N>
HK_FORCE_INLINE void hkSimdReal::load(const hkReal *p )
{
	load<N,HK_IO_BYTE_ALIGNED>(p);
}


template <int N, hkMathIoMode A>
HK_FORCE_INLINE void hkSimdReal::load(const hkHalf *p )
{
	HK_SIMDREAL_DIMENSION_CHECK;
	HK_ALIGN16(hkHalf tmp[2]);
	tmp[0] = tmp[1] = p[0];
	__m128i twohalfs = _mm_castps_si128( _mm_load_ss((const float*)tmp) );
	__m128  twofloats = _mm_castsi128_ps( _mm_unpacklo_epi16(_mm_setzero_si128(), twohalfs) );
	m_real = _mm_cvtps_pd(twofloats);
}

template <int N>
HK_FORCE_INLINE void hkSimdReal::load(const hkHalf *p )
{
	load<N,HK_IO_BYTE_ALIGNED>(p);
}


template <int N, hkMathIoMode A>
HK_FORCE_INLINE void hkSimdReal::load(const hkFloat16 *p )
{
	HK_SIMDREAL_DIMENSION_CHECK;
	HK_ALIGN16(hkFloat16 tmp[2]);
	tmp[0] = tmp[1] = p[0];
	__m128i twohalfs = _mm_castps_si128(_mm_load_ss((const float*)tmp));
	__m128 twofloats;
	unpackF16F32(twohalfs, twofloats);
	m_real = _mm_cvtps_pd(twofloats);
}

template <int N>
HK_FORCE_INLINE void hkSimdReal::load(const hkFloat16 *p )
{
	load<N,HK_IO_BYTE_ALIGNED>(p);
}



template <int N, hkMathIoMode A>
HK_FORCE_INLINE void hkSimdReal::store(  hkReal *p ) const
{
	HK_SIMDREAL_DIMENSION_CHECK;
	_mm_store_sd( p, m_real );
}

template <int N>
HK_FORCE_INLINE void hkSimdReal::store(  hkReal *p ) const
{
	store<N,HK_IO_BYTE_ALIGNED>(p);
}


template <int N, hkMathIoMode A, hkMathRoundingMode R>
HK_FORCE_INLINE void hkSimdReal::store(  hkHalf *p ) const
{
	HK_SIMDREAL_DIMENSION_CHECK;
	__m128  vs0;
	if (R == HK_ROUND_NEAREST)
		vs0 = _mm_cvtpd_ps(_mm_mul_pd( m_real, g_vectorConstants[HK_QUADREAL_PACK_HALF].xy ));
	else
		vs0 = _mm_cvtpd_ps(m_real);
	__m128i tmp0 = _mm_srai_epi32( _mm_castps_si128(vs0), 16 );
	__m128i tmp1 = _mm_packs_epi32(tmp0, tmp0);
	float ftmp1; _mm_store_ss(&ftmp1, _mm_castsi128_ps(tmp1));
	const hkHalf* HK_RESTRICT htmp1 = (const hkHalf* HK_RESTRICT)&ftmp1;
	p[0] = htmp1[0];
}

template <int N>
HK_FORCE_INLINE void hkSimdReal::store(  hkHalf *p ) const
{
	store<N,HK_IO_BYTE_ALIGNED,HK_ROUND_NEAREST>(p);
}




namespace hkSimdReal_AdvancedInterface
{
template <hkMathRoundingMode R>
HK_FORCE_INLINE static void convertSingleRealF16(const hkSingleReal& self, __m128i& packed)
{
	static HK_ALIGN16(const unsigned int abs[4])      = {0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF};
	static HK_ALIGN16(const unsigned int infinity[4]) = {0x47FFE000, 0x47FFE000, 0x47FFE000, 0x47FFE000};
	static HK_ALIGN16(const unsigned int denormal[4]) = {0x38800000, 0x38800000, 0x38800000, 0x38800000};
	static HK_ALIGN16(const unsigned int fixup[4])    = {0x48000000, 0x48000000, 0x48000000, 0x48000000};
	static HK_ALIGN16(const unsigned int round1[4])   = {0x00000001, 0x00000001, 0x00000001, 0x00000001};
	static HK_ALIGN16(const unsigned int round2[4])   = {0x00000FFF, 0x00000FFF, 0x00000FFF, 0x00000FFF};
	static HK_ALIGN16(const unsigned int sign[4])     = {0x80000000, 0x80000000, 0x80000000, 0x80000000};
	static HK_ALIGN16(const unsigned int base[4])     = {0x00007FFF, 0x00007FFF, 0x00007FFF, 0x00007FFF};
	static HK_ALIGN16(const unsigned int integer[4])  = {0x52000000, 0x52000000, 0x52000000, 0x52000000};

	__m128d dxy;
	if (R == HK_ROUND_NEAREST)
	{
		dxy = _mm_mul_pd(self,g_vectorConstants[HK_QUADREAL_PACK_HALF].xy);
	}
	else
	{
		dxy = self;
	}
	__m128 xyzw = _mm_cvtpd_ps(dxy);

	// Compute masks
	__m128 r_abs = _mm_and_ps(xyzw, *(__m128*)abs);
	__m128 r_inf = _mm_cmpnlt_ps(r_abs, *(__m128*)infinity);
	__m128 r_den = _mm_cmplt_ps(r_abs, *(__m128*)denormal);

	// Denormal case
	__m128i r_int = _mm_cvttps_epi32( _mm_mul_ps(r_abs, *(__m128*)integer) );

	// Normal case and combine
	__m128i fix = _mm_add_epi32(_mm_castps_si128(r_abs), *(__m128i*)fixup);
	__m128i select_den = _mm_and_si128(r_int, _mm_castps_si128(r_den));
	__m128i select_fix = _mm_andnot_si128(_mm_castps_si128(r_den), fix);
	__m128i all = _mm_or_si128(select_den, select_fix);

	__m128i all_rounded;
	if (R == HK_ROUND_NEAREST)
	{
		// Correct rounding
		__m128i rounded = _mm_add_epi32(_mm_and_si128(_mm_srli_epi32(all, 13), *(__m128i*)round1), *(__m128i*)round2);
		all_rounded = _mm_add_epi32(rounded, all);
	}
	else
	{
		all_rounded = all;
	}

	// Combine with sign and infinity
	__m128i extract_sign = _mm_srai_epi32(_mm_and_si128(_mm_castps_si128(xyzw), *(__m128i*)sign), 16);
	__m128i mantissa = _mm_and_si128(_mm_or_si128(_mm_srli_epi32(all_rounded, 13), _mm_castps_si128(r_inf)), *(__m128i*)base);
	__m128i assembled = _mm_or_si128(mantissa, extract_sign); // Result in lower words of each element

	// Pack
	packed = _mm_packs_epi32(assembled, assembled); // result in lower elements
}
} // namespace 

template <int N, hkMathIoMode A, hkMathRoundingMode R>
HK_FORCE_INLINE void hkSimdReal::store(  hkFloat16 *p ) const
{
	HK_SIMDREAL_DIMENSION_CHECK;
	__m128i packed;
	convertSingleRealF16(m_real,packed);
	HK_ALIGN16(hkFloat16 tmp[2];)
	_mm_store_ss((float*)tmp, _mm_castsi128_ps(packed));
	p[0] = tmp[0];
}

template <int N>
HK_FORCE_INLINE void hkSimdReal::store(  hkFloat16 *p ) const
{
	store<N,HK_IO_BYTE_ALIGNED,HK_ROUND_NEAREST>(p);
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
