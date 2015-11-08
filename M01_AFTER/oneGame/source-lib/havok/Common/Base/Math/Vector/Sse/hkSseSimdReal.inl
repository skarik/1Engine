/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#if defined(HK_REAL_IS_DOUBLE)
#error This implementation is for 32-Bit float
#endif

HK_FORCE_INLINE /*static*/ const hkSimdReal HK_CALL hkSimdReal::convert(const hkSingleReal& x)
{
	hkSimdReal sr;
	sr.m_real = x;
	return sr;
	//HK_ASSERT(0x7c3a4d98, x.m128_i32[0] == x.m128_i32[1] && x.m128_i32[0] == x.m128_i32[2] && x.m128_i32[0] == x.m128_i32[3]);
}

template<int vectorConstant>
HK_FORCE_INLINE /*static*/ const hkSimdReal HK_CALL hkSimdReal::getConstant()
{
	HK_COMPILE_TIME_ASSERT2( 
		(vectorConstant!=HK_QUADREAL_1000) && (vectorConstant!=HK_QUADREAL_0100) && (vectorConstant!=HK_QUADREAL_0010) && (vectorConstant!=HK_QUADREAL_0001) &&
		(vectorConstant!=HK_QUADREAL_m11m11) && (vectorConstant!=HK_QUADREAL_1248) && (vectorConstant!=HK_QUADREAL_8421)
		, HK_SIMDREAL_ILLEGAL_CONSTANT_REQUEST);
	return convert(*(g_vectorConstants + vectorConstant));
}

HK_FORCE_INLINE /*static*/ const hkSimdReal HK_CALL hkSimdReal::getConstant(hkVectorConstant constant)
{
	HK_ASSERT2( 0x909ff234,
		(constant!=HK_QUADREAL_1000) && (constant!=HK_QUADREAL_0100) && (constant!=HK_QUADREAL_0010) && (constant!=HK_QUADREAL_0001) &&
		(constant!=HK_QUADREAL_m11m11) && (constant!=HK_QUADREAL_1248) && (constant!=HK_QUADREAL_8421)
		, "not a simdreal constant");
	return convert(*(g_vectorConstants + constant));
}

#ifndef HK_DISABLE_IMPLICIT_SIMDREAL_FLOAT_CONVERSION
HK_FORCE_INLINE hkSimdReal::hkSimdReal(const hkReal& x)
{
	m_real = _mm_set1_ps(x);
}

HK_FORCE_INLINE hkSimdReal::operator hkReal() const
{
	hkReal s;
	_mm_store_ss(&s, m_real);
	return s;
}
#endif

HK_FORCE_INLINE hkReal hkSimdReal::getReal() const
{
	hkReal s;
	_mm_store_ss(&s, m_real);
	return s;
}

HK_FORCE_INLINE void hkSimdReal::setFromFloat(const hkReal& x)
{
	m_real = _mm_set1_ps(x);
}

HK_FORCE_INLINE void hkSimdReal::setFromHalf(const hkHalf& h)
{
	float x = h;
	m_real = _mm_set1_ps(x);
}

HK_FORCE_INLINE void hkSimdReal::setFromInt32(const hkInt32&  x)
{
	m_real = _mm_cvtepi32_ps( _mm_set1_epi32(x) );
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
	m_real = _mm_setzero_ps();
}


HK_FORCE_INLINE void hkSimdReal::storeSaturateInt32(hkInt32* HK_RESTRICT result) const
{
	*result = _mm_cvtsi128_si32(_mm_cvttps_epi32(m_real));
}


HK_FORCE_INLINE void hkSimdReal::storeSaturateUint16(hkUint16* HK_RESTRICT result) const
{
	hkSingleReal clamped   = _mm_max_ps(m_real,_mm_setzero_ps());	
	clamped = _mm_min_ps(clamped,_mm_set1_ps(65535.0f));
	*result = hkUint16(_mm_cvtsi128_si32(_mm_cvttps_epi32(clamped)));
}


HK_FORCE_INLINE const hkSimdReal hkSimdReal::operator+ (hkSimdRealParameter r) const
{
	return hkSimdReal::convert(_mm_add_ps(m_real,r.m_real));
}

HK_FORCE_INLINE const hkSimdReal hkSimdReal::operator- (hkSimdRealParameter r) const
{
	return hkSimdReal::convert(_mm_sub_ps(m_real,r.m_real));
}

HK_FORCE_INLINE const hkSimdReal hkSimdReal::operator* (hkSimdRealParameter r) const
{
	return hkSimdReal::convert(_mm_mul_ps(m_real,r.m_real));
}


#ifdef HK_DISABLE_OLD_VECTOR4_INTERFACE

HK_FORCE_INLINE bool hkSimdReal::operator< (hkSimdRealParameter r) const
{
	return _mm_ucomilt_ss(m_real, r.m_real) ? true : false;
}

HK_FORCE_INLINE bool hkSimdReal::operator<= (hkSimdRealParameter r) const
{
	return _mm_ucomile_ss(m_real, r.m_real) ? true : false;
}

HK_FORCE_INLINE bool hkSimdReal::operator> (hkSimdRealParameter r) const
{
	return _mm_ucomigt_ss(m_real, r.m_real) ? true : false;
}

HK_FORCE_INLINE bool hkSimdReal::operator>= (hkSimdRealParameter r) const
{
	return _mm_ucomige_ss(m_real, r.m_real) ? true : false;
}

HK_FORCE_INLINE bool hkSimdReal::operator== (hkSimdRealParameter r) const
{
	return _mm_ucomieq_ss(m_real, r.m_real) ? true : false;
}

HK_FORCE_INLINE bool hkSimdReal::operator!= (hkSimdRealParameter r) const
{
	return _mm_ucomineq_ss(m_real, r.m_real) ? true : false;
}

#endif

HK_FORCE_INLINE const hkSimdReal hkSimdReal::operator-() const
{
	return hkSimdReal::convert(_mm_sub_ps(_mm_setzero_ps(),m_real));
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::less(hkSimdRealParameter a) const
{
	return hkVector4Comparison::convert(_mm_cmplt_ps(m_real, a.m_real));
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::greater(hkSimdRealParameter a) const
{
	return hkVector4Comparison::convert(_mm_cmpgt_ps(m_real, a.m_real));
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::greaterEqual(hkSimdRealParameter a) const
{
	return hkVector4Comparison::convert(_mm_cmpge_ps(m_real, a.m_real));
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::lessEqual(hkSimdRealParameter a) const
{
	return hkVector4Comparison::convert(_mm_cmple_ps(m_real, a.m_real));
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::equal(hkSimdRealParameter a) const
{
	return hkVector4Comparison::convert(_mm_cmpeq_ps(m_real, a.m_real));
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::notEqual(hkSimdRealParameter a) const
{
	return hkVector4Comparison::convert(_mm_cmpneq_ps(m_real, a.m_real));
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::lessZero() const
{
	return hkVector4Comparison::convert(_mm_cmplt_ps(m_real, _mm_setzero_ps()));
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::lessEqualZero() const
{
	return hkVector4Comparison::convert(_mm_cmple_ps(m_real, _mm_setzero_ps()));
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::greaterZero() const
{
	return hkVector4Comparison::convert(_mm_cmpgt_ps(m_real, _mm_setzero_ps()));
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::greaterEqualZero() const
{
	return hkVector4Comparison::convert(_mm_cmpge_ps(m_real, _mm_setzero_ps()));
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::equalZero() const
{
	return hkVector4Comparison::convert(_mm_cmpeq_ps(m_real, _mm_setzero_ps()));
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::notEqualZero() const
{
	return hkVector4Comparison::convert(_mm_cmpneq_ps(m_real, _mm_setzero_ps()));
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isSignBitSet() const
{
	return _mm_movemask_ps(m_real);
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isSignBitClear() const
{
	return !_mm_movemask_ps(m_real);
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::signBitSet() const
{
	static HK_ALIGN16( const hkUint32 signmask[4] ) = { 0x80000000, 0x80000000, 0x80000000, 0x80000000 };
	const hkSingleReal c = _mm_and_ps(m_real, *(const hkSingleReal*)&signmask);
	return hkVector4Comparison::convert(_mm_castsi128_ps(_mm_cmpeq_epi32(_mm_castps_si128(c), *(const __m128i*)&signmask)));
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::signBitClear() const
{
	static HK_ALIGN16( const hkUint32 signmask[4] ) = { 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff };
	const hkSingleReal c = _mm_and_ps(m_real, *(const hkSingleReal*)&signmask);
	return hkVector4Comparison::convert(_mm_castsi128_ps(_mm_cmpeq_epi32(_mm_castps_si128(c), _mm_castps_si128(m_real))));
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isLess(hkSimdRealParameter a) const
{
	return _mm_ucomilt_ss(m_real, a.m_real);
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isLessEqual(hkSimdRealParameter a) const
{
	return _mm_ucomile_ss(m_real, a.m_real);
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isGreater(hkSimdRealParameter a) const
{
	return _mm_ucomigt_ss(m_real, a.m_real);
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isGreaterEqual(hkSimdRealParameter a) const
{
	return _mm_ucomige_ss(m_real, a.m_real);
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isEqual(hkSimdRealParameter a) const
{
	return _mm_ucomieq_ss(m_real, a.m_real);
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isNotEqual(hkSimdRealParameter a) const
{
	return _mm_ucomineq_ss(m_real, a.m_real);
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isLessZero() const
{
	return _mm_ucomilt_ss(m_real, _mm_setzero_ps());
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isLessEqualZero() const
{
	return _mm_ucomile_ss(m_real, _mm_setzero_ps());
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isGreaterZero() const
{
	return _mm_ucomigt_ss(m_real, _mm_setzero_ps());
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isGreaterEqualZero() const
{
	return _mm_ucomige_ss(m_real, _mm_setzero_ps());
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isEqualZero() const
{
	return _mm_ucomieq_ss(m_real, _mm_setzero_ps());
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isNotEqualZero() const
{
	return _mm_ucomineq_ss(m_real, _mm_setzero_ps());
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isOk() const
{
	const hkSingleReal nanMask = _mm_cmpunord_ps(m_real, _mm_setzero_ps());
	return !_mm_movemask_ps(nanMask);
}

HK_FORCE_INLINE void hkSimdReal::setSelect( hkVector4ComparisonParameter comp, hkSimdRealParameter trueValue, hkSimdRealParameter falseValue )
{
	HK_ASSERT2(0xc78f08e, comp.allAreSet() || (comp.getMask() == hkVector4Comparison::MASK_NONE), "illegal compare mask");
#if HK_SSE_VERSION >= 0x41
	m_real = _mm_blendv_ps(falseValue.m_real, trueValue.m_real, comp.m_mask);
#else
	m_real = _mm_or_ps( _mm_and_ps(comp.m_mask, trueValue.m_real), _mm_andnot_ps(comp.m_mask, falseValue.m_real) );
#endif
}

HK_FORCE_INLINE void hkSimdReal::setMin(  hkSimdRealParameter a, hkSimdRealParameter b ) 
{
	m_real = _mm_min_ps( a.m_real, b.m_real );
}

HK_FORCE_INLINE void hkSimdReal::setMax(  hkSimdRealParameter a, hkSimdRealParameter b ) 
{
	m_real = _mm_max_ps( a.m_real, b.m_real );
}

HK_FORCE_INLINE void hkSimdReal::setAbs(  hkSimdRealParameter a )
{
	m_real = hkMath::quadFabs(a.m_real);
}

HK_FORCE_INLINE void hkSimdReal::setFlipSign(hkSimdRealParameter v, hkSimdRealParameter sSign)
{
	static HK_ALIGN16( const hkUint32 signmask[4] ) = { 0x80000000, 0x80000000, 0x80000000, 0x80000000 };
	m_real = _mm_xor_ps(v.m_real, _mm_and_ps(sSign.m_real, *(const hkSingleReal*)&signmask));
}

HK_FORCE_INLINE void hkSimdReal::setFlipSign(hkSimdRealParameter v, hkVector4ComparisonParameter mask)
{
	HK_ASSERT2(0x5831c04, mask.allAreSet() || (mask.getMask() == hkVector4Comparison::MASK_NONE), "illegal compare mask");
	static HK_ALIGN16( const hkUint32 signmask[4] ) = { 0x80000000, 0x80000000, 0x80000000, 0x80000000 };
	m_real = _mm_xor_ps(v.m_real, _mm_and_ps(mask.m_mask, *(const hkSingleReal*)&signmask));
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
		case HK_ACC_23_BIT: self = hkMath::quadReciprocal(a.m_real); break;
		case HK_ACC_12_BIT: self = _mm_rcp_ps(a.m_real); break;
		default:         self = _mm_div_ps(g_vectorConstants[HK_QUADREAL_1],a.m_real); break; // HK_ACC_FULL
	}
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_SET_ZERO> { HK_FORCE_INLINE static void apply(hkSingleReal& self, hkSimdRealParameter a)
{
	const hkSingleReal equalsZero = _mm_cmpeq_ps(a.m_real, _mm_setzero_ps());
	hkSingleReal e; unroll_setReciprocal<A, HK_DIV_IGNORE>::apply(e, a);
	self = _mm_andnot_ps(equalsZero, e);
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_SET_HIGH> { HK_FORCE_INLINE static void apply(hkSingleReal& self, hkSimdRealParameter a)
{
	const hkSingleReal equalsZero = _mm_cmpeq_ps(a.m_real, _mm_setzero_ps());
	hkSingleReal e; unroll_setReciprocal<A, HK_DIV_IGNORE>::apply(e, a);
	const hkSingleReal huge = _mm_set1_ps(HK_REAL_HIGH);
#if HK_SSE_VERSION >= 0x41
	self = _mm_blendv_ps(e, huge, equalsZero);
#else
	self = _mm_or_ps( _mm_and_ps(equalsZero, huge), _mm_andnot_ps(equalsZero, e) );
#endif
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_SET_MAX> { HK_FORCE_INLINE static void apply(hkSingleReal& self, hkSimdRealParameter a)
{
	const hkSingleReal equalsZero = _mm_cmpeq_ps(a.m_real, _mm_setzero_ps());
	hkSingleReal e; unroll_setReciprocal<A, HK_DIV_IGNORE>::apply(e, a);
	const hkSingleReal huge = _mm_set1_ps(HK_REAL_MAX);
#if HK_SSE_VERSION >= 0x41
	self = _mm_blendv_ps(e, huge, equalsZero);
#else
	self = _mm_or_ps( _mm_and_ps(equalsZero, huge), _mm_andnot_ps(equalsZero, e) );
#endif
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_SET_ZERO_AND_ONE> { HK_FORCE_INLINE static void apply(hkSingleReal& self, hkSimdRealParameter a)
{
	unroll_setReciprocal<A, HK_DIV_SET_ZERO>::apply(self, a);
	const hkSingleReal one = g_vectorConstants[HK_QUADREAL_1];
	const hkSingleReal absVal = hkMath::quadFabs(self);
	const hkSingleReal absValLessOne = _mm_sub_ps(absVal, one);
	const hkSingleReal lessEqualEps = _mm_cmple_ps(absValLessOne, g_vectorConstants[HK_QUADREAL_EPS]);
#if HK_SSE_VERSION >= 0x41
	self = _mm_blendv_ps(self, one, lessEqualEps);
#else
	self = _mm_or_ps( _mm_and_ps(lessEqualEps, one), _mm_andnot_ps(lessEqualEps, self) );
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
	self.m_real = _mm_mul_ps(a.m_real,t);
} };
template <hkMathAccuracyMode A>
struct unroll_setDiv<A, HK_DIV_IGNORE> { HK_FORCE_INLINE static void apply(hkSimdReal& self, hkSimdRealParameter a, hkSimdRealParameter b)
{
	if (A == HK_ACC_FULL) 
	{ 
		self.m_real = _mm_div_ps(a.m_real, b.m_real); 
	}
	else
	{
		hkSingleReal t;
		unroll_setReciprocal<A,HK_DIV_IGNORE>::apply(t,b);
		self.m_real = _mm_mul_ps(a.m_real,t);
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
	return _mm_setzero_ps();
} };
template <hkMathAccuracyMode A>
struct unroll_sqrt<A, HK_SQRT_IGNORE> { HK_FORCE_INLINE static hkSingleReal apply(hkSimdRealParameter self)
{
	switch (A)
	{
		case HK_ACC_23_BIT: return _mm_mul_ps(self.m_real,hkMath::quadReciprocalSquareRoot(self.m_real)); break;
		case HK_ACC_12_BIT: return _mm_mul_ps(self.m_real,_mm_rsqrt_ps(self.m_real)); break;
		default:         return _mm_sqrt_ps(self.m_real); break; // HK_ACC_FULL
	}
} };
template <hkMathAccuracyMode A>
struct unroll_sqrt<A, HK_SQRT_SET_ZERO> { HK_FORCE_INLINE static hkSingleReal apply(hkSimdRealParameter self)
{
	const hkSingleReal equalsZero = _mm_cmple_ps(self.m_real, _mm_setzero_ps());
	const hkSingleReal e = unroll_sqrt<A, HK_SQRT_IGNORE>::apply(self);
	return _mm_andnot_ps(equalsZero, e);
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
	return _mm_setzero_ps();
} };
template <hkMathAccuracyMode A>
struct unroll_sqrtInverse<A, HK_SQRT_IGNORE> { HK_FORCE_INLINE static hkSingleReal apply(hkSimdRealParameter self)
{
	switch (A)
	{
		case HK_ACC_23_BIT: return hkMath::quadReciprocalSquareRoot(self.m_real); break;
		case HK_ACC_12_BIT: return _mm_rsqrt_ps(self.m_real); break;
		default:			return _mm_div_ps(g_vectorConstants[HK_QUADREAL_1], _mm_sqrt_ps(self.m_real)); break; // HK_ACC_FULL
	}
} };
template <hkMathAccuracyMode A>
struct unroll_sqrtInverse<A, HK_SQRT_SET_ZERO> { HK_FORCE_INLINE static hkSingleReal apply(hkSimdRealParameter self)
{
	const hkSingleReal equalsZero = _mm_cmple_ps(self.m_real, _mm_setzero_ps());
	const hkSingleReal e = unroll_sqrtInverse<A, HK_SQRT_IGNORE>::apply(self);
	return _mm_andnot_ps(equalsZero, e);
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



namespace hkSimdReal_AdvancedInterface
{
template <hkMathIoMode A>
struct unroll_load { HK_FORCE_INLINE static void apply(hkSingleReal& self, const hkReal* HK_RESTRICT p)
{
	HK_SIMDREAL_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <>
struct unroll_load<HK_IO_BYTE_ALIGNED> { HK_FORCE_INLINE static void apply(hkSingleReal& self, const hkReal* HK_RESTRICT p)
{
	self = _mm_load1_ps(p);
} };
template <>
struct unroll_load<HK_IO_NATIVE_ALIGNED> { HK_FORCE_INLINE static void apply(hkSingleReal& self, const hkReal* HK_RESTRICT p)
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkReal)-1) ) == 0, "pointer must be aligned to native size of hkReal.");
	unroll_load<HK_IO_BYTE_ALIGNED>::apply(self,p);
} };
template <>
struct unroll_load<HK_IO_SIMD_ALIGNED> { HK_FORCE_INLINE static void apply(hkSingleReal& self, const hkReal* HK_RESTRICT p)
{
	unroll_load<HK_IO_NATIVE_ALIGNED>::apply(self,p);
} };
template <>
struct unroll_load<HK_IO_NOT_CACHED> { HK_FORCE_INLINE static void apply(hkSingleReal& self, const hkReal* HK_RESTRICT p)
{
	unroll_load<HK_IO_SIMD_ALIGNED>::apply(self,p);
} };
} // namespace 

template <int N, hkMathIoMode A>
HK_FORCE_INLINE void hkSimdReal::load(const hkReal *p )
{
	HK_SIMDREAL_DIMENSION_CHECK;
	hkSimdReal_AdvancedInterface::unroll_load<A>::apply(m_real, p);
}

template <int N>
HK_FORCE_INLINE void hkSimdReal::load(const hkReal *p )
{
	HK_SIMDREAL_DIMENSION_CHECK;
	hkSimdReal_AdvancedInterface::unroll_load<HK_IO_SIMD_ALIGNED>::apply(m_real, p);
}




namespace hkSimdReal_AdvancedInterface
{
template <hkMathIoMode A>
struct unroll_loadH { HK_FORCE_INLINE static void apply(hkSingleReal& self, const hkHalf* HK_RESTRICT p)
{
	HK_SIMDREAL_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <>
struct unroll_loadH<HK_IO_BYTE_ALIGNED> { HK_FORCE_INLINE static void apply(hkSingleReal& self, const hkHalf* HK_RESTRICT p)
{
	self = _mm_set1_ps(hkReal(p[0]));
} };
template <>
struct unroll_loadH<HK_IO_NATIVE_ALIGNED> { HK_FORCE_INLINE static void apply(hkSingleReal& self, const hkHalf* HK_RESTRICT p)
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkHalf)-1) ) == 0, "pointer must be aligned to native size of hkHalf.");
	unroll_loadH<HK_IO_BYTE_ALIGNED>::apply(self,p);
} };
template <>
struct unroll_loadH<HK_IO_SIMD_ALIGNED> { HK_FORCE_INLINE static void apply(hkSingleReal& self, const hkHalf* HK_RESTRICT p)
{
	unroll_loadH<HK_IO_NATIVE_ALIGNED>::apply(self,p);
} };
template <>
struct unroll_loadH<HK_IO_NOT_CACHED> { HK_FORCE_INLINE static void apply(hkSingleReal& self, const hkHalf* HK_RESTRICT p)
{
	unroll_loadH<HK_IO_SIMD_ALIGNED>::apply(self,p);
} };
} // namespace 

template <int N, hkMathIoMode A>
HK_FORCE_INLINE void hkSimdReal::load(const hkHalf *p )
{
	HK_SIMDREAL_DIMENSION_CHECK;
	hkSimdReal_AdvancedInterface::unroll_loadH<A>::apply(m_real, p);
}

template <int N>
HK_FORCE_INLINE void hkSimdReal::load(const hkHalf *p )
{
	HK_SIMDREAL_DIMENSION_CHECK;
	hkSimdReal_AdvancedInterface::unroll_loadH<HK_IO_SIMD_ALIGNED>::apply(m_real, p);
}




namespace hkSimdReal_AdvancedInterface
{
template <hkMathIoMode A>
struct unroll_loadF16 { HK_FORCE_INLINE static void apply(hkSingleReal& self, const hkFloat16* HK_RESTRICT p)
{
	HK_SIMDREAL_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <>
struct unroll_loadF16<HK_IO_BYTE_ALIGNED> { HK_FORCE_INLINE static void apply(hkSingleReal& self, const hkFloat16* HK_RESTRICT p)
{
	hkFloat32 tmp = p[0].getReal();
	self = _mm_load1_ps((const float*)&tmp);
} };
template <>
struct unroll_loadF16<HK_IO_NATIVE_ALIGNED> { HK_FORCE_INLINE static void apply(hkSingleReal& self, const hkFloat16* HK_RESTRICT p)
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkFloat16)-1) ) == 0, "pointer must be aligned to native size of hkFloat16.");
	unroll_loadF16<HK_IO_BYTE_ALIGNED>::apply(self,p);
} };
template <>
struct unroll_loadF16<HK_IO_SIMD_ALIGNED> { HK_FORCE_INLINE static void apply(hkSingleReal& self, const hkFloat16* HK_RESTRICT p)
{
	unroll_loadF16<HK_IO_NATIVE_ALIGNED>::apply(self,p);
} };
template <>
struct unroll_loadF16<HK_IO_NOT_CACHED> { HK_FORCE_INLINE static void apply(hkSingleReal& self, const hkFloat16* HK_RESTRICT p)
{
	unroll_loadF16<HK_IO_SIMD_ALIGNED>::apply(self,p);
} };
} // namespace 

template <int N, hkMathIoMode A>
HK_FORCE_INLINE void hkSimdReal::load(const hkFloat16 *p )
{
	HK_SIMDREAL_DIMENSION_CHECK;
	hkSimdReal_AdvancedInterface::unroll_loadF16<A>::apply(m_real, p);
}

template <int N>
HK_FORCE_INLINE void hkSimdReal::load(const hkFloat16 *p )
{
	HK_SIMDREAL_DIMENSION_CHECK;
	hkSimdReal_AdvancedInterface::unroll_loadF16<HK_IO_SIMD_ALIGNED>::apply(m_real, p);
}



namespace hkSimdReal_AdvancedInterface
{
template <hkMathIoMode A>
struct unroll_store { HK_FORCE_INLINE static void apply(const hkSingleReal& self, hkReal* HK_RESTRICT p)
{
	HK_SIMDREAL_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <>
struct unroll_store<HK_IO_BYTE_ALIGNED> { HK_FORCE_INLINE static void apply(const hkSingleReal& self, hkReal* HK_RESTRICT p)
{
	_mm_store_ss( p, self );
} };
template <>
struct unroll_store<HK_IO_NATIVE_ALIGNED> { HK_FORCE_INLINE static void apply(const hkSingleReal& self, hkReal* HK_RESTRICT p)
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkReal)-1) ) == 0, "pointer must be aligned to native size of hkReal.");
	unroll_store<HK_IO_BYTE_ALIGNED>::apply(self,p);
} };
template <>
struct unroll_store<HK_IO_SIMD_ALIGNED> { HK_FORCE_INLINE static void apply(const hkSingleReal& self, hkReal* HK_RESTRICT p)
{
	unroll_store<HK_IO_NATIVE_ALIGNED>::apply(self,p);
} };
template <>
struct unroll_store<HK_IO_NOT_CACHED> { HK_FORCE_INLINE static void apply(const hkSingleReal& self, hkReal* HK_RESTRICT p)
{
	unroll_store<HK_IO_SIMD_ALIGNED>::apply(self,p);
} };
} // namespace 

template <int N, hkMathIoMode A>
HK_FORCE_INLINE void hkSimdReal::store(  hkReal *p ) const
{
	HK_SIMDREAL_DIMENSION_CHECK;
	hkSimdReal_AdvancedInterface::unroll_store<A>::apply(m_real, p);
}

template <int N>
HK_FORCE_INLINE void hkSimdReal::store(  hkReal *p ) const
{
	HK_SIMDREAL_DIMENSION_CHECK;
	hkSimdReal_AdvancedInterface::unroll_store<HK_IO_SIMD_ALIGNED>::apply(m_real, p);
}




namespace hkSimdReal_AdvancedInterface
{
template <hkMathIoMode A, hkMathRoundingMode R>
struct unroll_storeH { HK_FORCE_INLINE static void apply(const hkSingleReal& self, hkHalf* HK_RESTRICT p)
{
	HK_SIMDREAL_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <hkMathRoundingMode R>
struct unroll_storeH<HK_IO_BYTE_ALIGNED,R> { HK_FORCE_INLINE static void apply(const hkSingleReal& self, hkHalf* HK_RESTRICT p)
{
	__m128  vs0;
	if (R == HK_ROUND_NEAREST)
		vs0 = _mm_mul_ps( self, g_vectorConstants[HK_QUADREAL_PACK_HALF] );
	else
		vs0 = self;
	__m128i tmp0 = _mm_srai_epi32( _mm_castps_si128(vs0), 16 );
	__m128i tmp1 = _mm_packs_epi32(tmp0, tmp0);
	float ftmp1; _mm_store_ss(&ftmp1, _mm_castsi128_ps(tmp1));
	const hkHalf* HK_RESTRICT htmp1 = (const hkHalf* HK_RESTRICT)&ftmp1;
	p[0] = htmp1[0];
} };
template <hkMathRoundingMode R>
struct unroll_storeH<HK_IO_NATIVE_ALIGNED,R> { HK_FORCE_INLINE static void apply(const hkSingleReal& self, hkHalf* HK_RESTRICT p)
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkHalf)-1) ) == 0, "pointer must be aligned to native size of hkHalf.");
	unroll_storeH<HK_IO_BYTE_ALIGNED,R>::apply(self,p);
} };
template <hkMathRoundingMode R>
struct unroll_storeH<HK_IO_SIMD_ALIGNED,R> { HK_FORCE_INLINE static void apply(const hkSingleReal& self, hkHalf* HK_RESTRICT p)
{
	unroll_storeH<HK_IO_NATIVE_ALIGNED,R>::apply(self,p);
} };
template <hkMathRoundingMode R>
struct unroll_storeH<HK_IO_NOT_CACHED,R> { HK_FORCE_INLINE static void apply(const hkSingleReal& self, hkHalf* HK_RESTRICT p)
{
	unroll_storeH<HK_IO_SIMD_ALIGNED,R>::apply(self,p);
} };
} // namespace 

template <int N, hkMathIoMode A, hkMathRoundingMode R>
HK_FORCE_INLINE void hkSimdReal::store(  hkHalf *p ) const
{
	HK_SIMDREAL_DIMENSION_CHECK;
	hkSimdReal_AdvancedInterface::unroll_storeH<A,R>::apply(m_real, p);
}

template <int N>
HK_FORCE_INLINE void hkSimdReal::store(  hkHalf *p ) const
{
	HK_SIMDREAL_DIMENSION_CHECK;
	hkSimdReal_AdvancedInterface::unroll_storeH<HK_IO_SIMD_ALIGNED,HK_ROUND_NEAREST>::apply(m_real, p);
}




namespace hkSimdReal_AdvancedInterface
{
template <hkMathIoMode A, hkMathRoundingMode R>
struct unroll_storeF16 { HK_FORCE_INLINE static void apply(const hkSingleReal& self, hkFloat16* HK_RESTRICT p)
{
	HK_SIMDREAL_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <hkMathRoundingMode R>
struct unroll_storeF16<HK_IO_BYTE_ALIGNED,R> { HK_FORCE_INLINE static void apply(const hkSingleReal& self, hkFloat16* HK_RESTRICT p)
{
	hkFloat32 tmp;
	_mm_store_ss(&tmp, self);
	p[0].setReal<(R == HK_ROUND_NEAREST)>(tmp);
} };
template <hkMathRoundingMode R>
struct unroll_storeF16<HK_IO_NATIVE_ALIGNED,R> { HK_FORCE_INLINE static void apply(const hkSingleReal& self, hkFloat16* HK_RESTRICT p)
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkFloat16)-1) ) == 0, "pointer must be aligned to native size of hkFloat16.");
	unroll_storeF16<HK_IO_BYTE_ALIGNED,R>::apply(self,p);
} };
template <hkMathRoundingMode R>
struct unroll_storeF16<HK_IO_SIMD_ALIGNED,R> { HK_FORCE_INLINE static void apply(const hkSingleReal& self, hkFloat16* HK_RESTRICT p)
{
	unroll_storeF16<HK_IO_NATIVE_ALIGNED,R>::apply(self,p);
} };
template <hkMathRoundingMode R>
struct unroll_storeF16<HK_IO_NOT_CACHED,R> { HK_FORCE_INLINE static void apply(const hkSingleReal& self, hkFloat16* HK_RESTRICT p)
{
	unroll_storeF16<HK_IO_SIMD_ALIGNED,R>::apply(self,p);
} };
} // namespace 

template <int N, hkMathIoMode A, hkMathRoundingMode R>
HK_FORCE_INLINE void hkSimdReal::store(  hkFloat16 *p ) const
{
	HK_SIMDREAL_DIMENSION_CHECK;
	hkSimdReal_AdvancedInterface::unroll_storeF16<A,R>::apply(m_real, p);
}

template <int N>
HK_FORCE_INLINE void hkSimdReal::store(  hkFloat16 *p ) const
{
	HK_SIMDREAL_DIMENSION_CHECK;
	hkSimdReal_AdvancedInterface::unroll_storeF16<HK_IO_SIMD_ALIGNED,HK_ROUND_NEAREST>::apply(m_real, p);
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
