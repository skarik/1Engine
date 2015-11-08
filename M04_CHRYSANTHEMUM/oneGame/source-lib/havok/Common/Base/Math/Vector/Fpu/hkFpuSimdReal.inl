/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

HK_FORCE_INLINE /*static*/ const hkSimdReal HK_CALL hkSimdReal::convert(const hkSingleReal& x)
{
	hkSimdReal sr;
	sr.m_real = x;
	return sr;
}

template<int vectorConstant>
HK_FORCE_INLINE /*static*/ const hkSimdReal HK_CALL hkSimdReal::getConstant()
{
	HK_COMPILE_TIME_ASSERT2( 
		(vectorConstant!=HK_QUADREAL_1000) && (vectorConstant!=HK_QUADREAL_0100) && (vectorConstant!=HK_QUADREAL_0010) && (vectorConstant!=HK_QUADREAL_0001) &&
		(vectorConstant!=HK_QUADREAL_m11m11) && (vectorConstant!=HK_QUADREAL_1248) && (vectorConstant!=HK_QUADREAL_8421)
		, HK_SIMDREAL_ILLEGAL_CONSTANT_REQUEST);
#if !defined(HK_PLATFORM_RVL) && !defined(HK_PLATFORM_CAFE)
	return convert((g_vectorConstants + vectorConstant)->v[0]);
#else
	return *(const hkSimdReal*) (g_vectorConstants + vectorConstant);
#endif
}

HK_FORCE_INLINE /*static*/ const hkSimdReal HK_CALL hkSimdReal::getConstant(hkVectorConstant constant)
{	
	HK_ASSERT2( 0x909ff234,
		(constant!=HK_QUADREAL_1000) && (constant!=HK_QUADREAL_0100) && (constant!=HK_QUADREAL_0010) && (constant!=HK_QUADREAL_0001) &&
		(constant!=HK_QUADREAL_m11m11) && (constant!=HK_QUADREAL_1248) && (constant!=HK_QUADREAL_8421)
		, "not a simdreal constant");
#if !defined(HK_PLATFORM_RVL) && !defined(HK_PLATFORM_CAFE)
	return convert((g_vectorConstants + constant)->v[0]);
#else
	return *(const hkSimdReal*) (g_vectorConstants + constant);
#endif
}

#ifndef HK_DISABLE_IMPLICIT_SIMDREAL_FLOAT_CONVERSION
HK_FORCE_INLINE hkSimdReal::hkSimdReal(const hkReal& x)
{
	m_real = x;
}

HK_FORCE_INLINE hkSimdReal::operator hkReal() const
{
	return m_real;
}
#endif

HK_FORCE_INLINE hkReal hkSimdReal::getReal() const
{
	return m_real;
}

HK_FORCE_INLINE void hkSimdReal::setFromFloat(const hkReal& x)
{
	m_real = x;
}

HK_FORCE_INLINE void hkSimdReal::setFromHalf(const hkHalf& h)
{
	m_real = hkReal(h);
}

HK_FORCE_INLINE void hkSimdReal::setFromInt32(const hkInt32&  x)
{
	m_real = hkReal(x);
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
	m_real = hkReal(0);
}

HK_FORCE_INLINE void hkSimdReal::storeSaturateInt32(hkInt32* HK_RESTRICT result) const
{
	*result = hkMath::hkToIntFast(m_real);
}


HK_FORCE_INLINE void hkSimdReal::storeSaturateUint16(hkUint16* result) const
{
	const hkInt32 i = hkMath::hkToIntFast(m_real);
	const hkInt32 clampPos    = hkMath::max2(i, 0x0);
	const hkInt32 clampPosNeg = hkMath::min2(clampPos, 0xffff);
	*result = hkUint16(clampPosNeg);
}


HK_FORCE_INLINE const hkSimdReal hkSimdReal::operator+ (hkSimdRealParameter r) const
{
	return hkSimdReal::convert(m_real + r.m_real);
}

HK_FORCE_INLINE const hkSimdReal hkSimdReal::operator- (hkSimdRealParameter r) const
{
	return hkSimdReal::convert(m_real - r.m_real);
}

HK_FORCE_INLINE const hkSimdReal hkSimdReal::operator* (hkSimdRealParameter r) const
{
	return hkSimdReal::convert(m_real * r.m_real);
}


#ifdef HK_DISABLE_OLD_VECTOR4_INTERFACE

HK_FORCE_INLINE bool hkSimdReal::operator< (hkSimdRealParameter r) const
{
	return (m_real < r.m_real);
}

HK_FORCE_INLINE bool hkSimdReal::operator<= (hkSimdRealParameter r) const
{
	return (m_real <= r.m_real);
}

HK_FORCE_INLINE bool hkSimdReal::operator> (hkSimdRealParameter r) const
{
	return (m_real > r.m_real);
}

HK_FORCE_INLINE bool hkSimdReal::operator>= (hkSimdRealParameter r) const
{
	return (m_real >= r.m_real);
}

HK_FORCE_INLINE bool hkSimdReal::operator== (hkSimdRealParameter r) const
{
	return (m_real == r.m_real);
}

HK_FORCE_INLINE bool hkSimdReal::operator!= (hkSimdRealParameter r) const
{
	return (m_real != r.m_real);
}

#endif

HK_FORCE_INLINE const hkSimdReal hkSimdReal::operator-() const
{
	return hkSimdReal::convert(-m_real);
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::less(hkSimdRealParameter a) const
{
	hkVector4Comparison res;
	res.m_mask = (m_real < a.m_real) ? hkVector4Comparison::MASK_XYZW : hkVector4Comparison::MASK_NONE;
	return res;
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::greater(hkSimdRealParameter a) const
{
	hkVector4Comparison res;
	res.m_mask = (m_real > a.m_real) ? hkVector4Comparison::MASK_XYZW : hkVector4Comparison::MASK_NONE;
	return res;
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::greaterEqual(hkSimdRealParameter a) const
{
	hkVector4Comparison res;
	res.m_mask = (m_real >= a.m_real) ? hkVector4Comparison::MASK_XYZW : hkVector4Comparison::MASK_NONE;
	return res;
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::lessEqual(hkSimdRealParameter a) const
{
	hkVector4Comparison res;
	res.m_mask = (m_real <= a.m_real) ? hkVector4Comparison::MASK_XYZW : hkVector4Comparison::MASK_NONE;
	return res;
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::equal(hkSimdRealParameter a) const
{
	hkVector4Comparison res;
	res.m_mask = (m_real == a.m_real) ? hkVector4Comparison::MASK_XYZW : hkVector4Comparison::MASK_NONE;
	return res;
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::notEqual(hkSimdRealParameter a) const
{
	hkVector4Comparison res;
	res.m_mask = (m_real != a.m_real) ? hkVector4Comparison::MASK_XYZW : hkVector4Comparison::MASK_NONE;
	return res;
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::lessZero() const
{
	hkVector4Comparison res;
	res.m_mask = (m_real < hkReal(0)) ? hkVector4Comparison::MASK_XYZW : hkVector4Comparison::MASK_NONE;
	return res;
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::lessEqualZero() const
{
	hkVector4Comparison res;
	res.m_mask = (m_real <= hkReal(0)) ? hkVector4Comparison::MASK_XYZW : hkVector4Comparison::MASK_NONE;
	return res;
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::greaterZero() const
{
	hkVector4Comparison res;
	res.m_mask = (m_real > hkReal(0)) ? hkVector4Comparison::MASK_XYZW : hkVector4Comparison::MASK_NONE;
	return res;
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::greaterEqualZero() const
{
	hkVector4Comparison res;
	res.m_mask = (m_real >= hkReal(0)) ? hkVector4Comparison::MASK_XYZW : hkVector4Comparison::MASK_NONE;
	return res;
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::equalZero() const
{
	hkVector4Comparison res;
	res.m_mask = (m_real == hkReal(0)) ? hkVector4Comparison::MASK_XYZW : hkVector4Comparison::MASK_NONE;
	return res;
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::notEqualZero() const
{
	hkVector4Comparison res;
	res.m_mask = (m_real != hkReal(0)) ? hkVector4Comparison::MASK_XYZW : hkVector4Comparison::MASK_NONE;
	return res;
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isSignBitSet() const
{
	return hkMath::signBitSet(m_real);
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isSignBitClear() const
{
	return !hkMath::signBitSet(m_real);
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::signBitSet() const
{
	hkVector4Comparison res;
	res.m_mask = hkMath::signBitSet(m_real) ? hkVector4Comparison::MASK_XYZW : hkVector4Comparison::MASK_NONE;
	return res;
}

HK_FORCE_INLINE const hkVector4Comparison hkSimdReal::signBitClear() const
{
	hkVector4Comparison res;
	res.m_mask = hkMath::signBitSet(m_real) ? hkVector4Comparison::MASK_NONE : hkVector4Comparison::MASK_XYZW;
	return res;
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isLess(hkSimdRealParameter a) const
{
	return (m_real < a.m_real);
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isLessEqual(hkSimdRealParameter a) const
{
	return (m_real <= a.m_real);
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isGreater(hkSimdRealParameter a) const
{
	return (m_real > a.m_real);
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isGreaterEqual(hkSimdRealParameter a) const
{
	return (m_real >= a.m_real);
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isEqual(hkSimdRealParameter a) const
{
	return (m_real == a.m_real);
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isNotEqual(hkSimdRealParameter a) const
{
	return (m_real != a.m_real);
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isLessZero() const
{
	return (m_real < hkReal(0));
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isLessEqualZero() const
{
	return (m_real <= hkReal(0));
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isGreaterZero() const
{
	return (m_real > hkReal(0));
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isGreaterEqualZero() const
{
	return (m_real >= hkReal(0));
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isEqualZero() const
{
	return (m_real == hkReal(0));
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isNotEqualZero() const
{
	return (m_real != hkReal(0));
}

HK_FORCE_INLINE hkBool32 hkSimdReal::isOk() const
{
	return hkMath::isFinite(m_real);
}

HK_FORCE_INLINE void hkSimdReal::setSelect( hkVector4ComparisonParameter comp, hkSimdRealParameter a, hkSimdRealParameter b )
{
	HK_ASSERT2(0x125f0f99, comp.allAreSet() || (comp.getMask() == hkVector4Comparison::MASK_NONE), "illegal compare mask");
	m_real = (comp.m_mask & hkVector4Comparison::MASK_XYZW) ? a.m_real : b.m_real;
}

HK_FORCE_INLINE void hkSimdReal::setMin(  hkSimdRealParameter a, hkSimdRealParameter b ) 
{
	m_real = hkMath::min2( a.m_real, b.m_real );
}

HK_FORCE_INLINE void hkSimdReal::setMax(  hkSimdRealParameter a, hkSimdRealParameter b ) 
{
	m_real = hkMath::max2( a.m_real, b.m_real );
}

HK_FORCE_INLINE void hkSimdReal::setAbs(  hkSimdRealParameter a )
{
	m_real = hkMath::fabs(a.m_real);
}

HK_FORCE_INLINE void hkSimdReal::setFlipSign(hkSimdRealParameter v, hkSimdRealParameter sSign)
{
	m_real = hkMath::signBitSet(sSign.m_real) ? -v.m_real : v.m_real;
}

HK_FORCE_INLINE void hkSimdReal::setFlipSign(hkSimdRealParameter v, hkVector4ComparisonParameter mask)
{
	HK_ASSERT2(0x39186bef, mask.allAreSet() || (mask.getMask() == hkVector4Comparison::MASK_NONE), "illegal compare mask");
	m_real = (mask.m_mask & hkVector4Comparison::MASK_XYZW) ? -v.m_real : v.m_real;
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
		case HK_ACC_23_BIT: { hkFloat32 x = hkMath::rcpF32Approx23Bit(hkFloat32(a.m_real)); self = hkReal(x); } break;
		case HK_ACC_12_BIT: { hkFloat32 x = hkMath::rcpF32Approx12Bit(hkFloat32(a.m_real)); self = hkReal(x); } break;
		default:         self = hkReal(1) / a.m_real; break; // HK_ACC_FULL
	}
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_SET_ZERO> { HK_FORCE_INLINE static void apply(hkSingleReal& self, hkSimdRealParameter a)
{
	if (a.m_real == hkReal(0)) { self = hkReal(0); return; }
	unroll_setReciprocal<A, HK_DIV_IGNORE>::apply(self,a);
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_SET_HIGH> { HK_FORCE_INLINE static void apply(hkSingleReal& self, hkSimdRealParameter a)
{
	if (a.m_real == hkReal(0)) { self = HK_REAL_HIGH; return; }
	unroll_setReciprocal<A, HK_DIV_IGNORE>::apply(self,a);
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_SET_MAX> { HK_FORCE_INLINE static void apply(hkSingleReal& self, hkSimdRealParameter a)
{
	if (a.m_real == hkReal(0)) { self = HK_REAL_MAX; return; }
	unroll_setReciprocal<A, HK_DIV_IGNORE>::apply(self,a);
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_SET_ZERO_AND_ONE> { HK_FORCE_INLINE static void apply(hkSingleReal& self, hkSimdRealParameter a)
{
	unroll_setReciprocal<A, HK_DIV_SET_ZERO>::apply(self,a);
	const hkSingleReal absValLessOne = hkMath::fabs(self) - hkReal(1);
	if (absValLessOne <= HK_REAL_EPSILON)
		self = hkReal(1);
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
	hkSimdReal t; t.setReciprocal<A,D>(b);
	self.setMul(a,t);
} };
template <hkMathAccuracyMode A>
struct unroll_setDiv<A, HK_DIV_IGNORE> { HK_FORCE_INLINE static void apply(hkSimdReal& self, hkSimdRealParameter a, hkSimdRealParameter b)
{
	if (A == HK_ACC_FULL) 
	{ 
		self.m_real = a.m_real / b.m_real; 
	}
	else
	{
		hkSimdReal t; t.setReciprocal<A,HK_DIV_IGNORE>(b);
		self.setMul(a,t);
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
	return hkReal(0);
} };
template <hkMathAccuracyMode A>
struct unroll_sqrt<A, HK_SQRT_IGNORE> { HK_FORCE_INLINE static hkSingleReal apply(hkSimdRealParameter self)
{
	switch (A)
	{
		case HK_ACC_23_BIT: return hkReal( hkFloat32(self.m_real) * hkMath::invSqrtF32Approx23Bit(hkFloat32(self.m_real)) ); break;
		case HK_ACC_12_BIT: return hkReal( hkFloat32(self.m_real) * hkMath::invSqrtF32Approx12Bit(hkFloat32(self.m_real)) ); break;
		default:         return hkMath::sqrt(self.m_real); break; // HK_ACC_FULL
	}
} };
template <hkMathAccuracyMode A>
struct unroll_sqrt<A, HK_SQRT_SET_ZERO> { HK_FORCE_INLINE static hkSingleReal apply(hkSimdRealParameter self)
{
	if (self.m_real <= hkReal(0)) { return hkReal(0); }
	return unroll_sqrt<A, HK_SQRT_IGNORE>::apply(self);
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
	return hkReal(0);
} };
template <hkMathAccuracyMode A>
struct unroll_sqrtInverse<A, HK_SQRT_IGNORE> { HK_FORCE_INLINE static hkSingleReal apply(hkSimdRealParameter self)
{
	switch (A)
	{
		case HK_ACC_23_BIT: return hkReal(hkMath::invSqrtF32Approx23Bit(hkFloat32(self.m_real))); break;
		case HK_ACC_12_BIT: return hkReal(hkMath::invSqrtF32Approx12Bit(hkFloat32(self.m_real))); break;
		default:			return hkMath::sqrtInverse(self.m_real); break; // HK_ACC_FULL
	}
} };
template <hkMathAccuracyMode A>
struct unroll_sqrtInverse<A, HK_SQRT_SET_ZERO> { HK_FORCE_INLINE static hkSingleReal apply(hkSimdRealParameter self)
{
	if (self.m_real <= hkReal(0)) return hkReal(0);
	return unroll_sqrtInverse<A, HK_SQRT_IGNORE>::apply(self);
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
	self = p[0];
} };
template <>
struct unroll_load<HK_IO_NATIVE_ALIGNED> { HK_FORCE_INLINE static void apply(hkSingleReal& self, const hkReal* HK_RESTRICT p)
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkReal)-1) ) == 0, "pointer must be aligned to native size of hkReal.");
	self = p[0];
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
	self = hkReal(hkFloat32(p[0]));
} };
template <>
struct unroll_loadH<HK_IO_NATIVE_ALIGNED> { HK_FORCE_INLINE static void apply(hkSingleReal& self, const hkHalf* HK_RESTRICT p)
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkHalf)-1) ) == 0, "pointer must be aligned to native size of hkHalf.");
	self = hkReal(hkFloat32(p[0]));
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
	self = p[0].getReal();
} };
template <>
struct unroll_loadF16<HK_IO_NATIVE_ALIGNED> { HK_FORCE_INLINE static void apply(hkSingleReal& self, const hkFloat16* HK_RESTRICT p)
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkFloat16)-1) ) == 0, "pointer must be aligned to native size of hkFloat16.");
	self = p[0].getReal();
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
	p[0] = self;
} };
template <>
struct unroll_store<HK_IO_NATIVE_ALIGNED> { HK_FORCE_INLINE static void apply(const hkSingleReal& self, hkReal* HK_RESTRICT p)
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkReal)-1) ) == 0, "pointer must be aligned to native size of hkReal.");
	p[0] = self;
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
	if (R == HK_ROUND_NEAREST)
	{
		const hkReal* packHalf = (const hkReal*)(g_vectorConstants + HK_QUADREAL_PACK_HALF);
		p[0] = hkFloat32(self * (*packHalf));
	}
	else
	{
		p[0] = hkFloat32(self);
	}
} };
template <hkMathRoundingMode R>
struct unroll_storeH<HK_IO_NATIVE_ALIGNED,R> { HK_FORCE_INLINE static void apply(const hkSingleReal& self, hkHalf* HK_RESTRICT p)
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkHalf)-1) ) == 0, "pointer must be aligned to native size of hkHalf.");
	if (R == HK_ROUND_NEAREST)
	{
		const hkReal* packHalf = (const hkReal*)(g_vectorConstants + HK_QUADREAL_PACK_HALF);
		p[0] = hkFloat32(self * (*packHalf));
	}
	else
	{
		p[0] = hkFloat32(self);
	}
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
	p[0].setReal<(R == HK_ROUND_NEAREST)>(hkFloat32(self));
} };
template <hkMathRoundingMode R>
struct unroll_storeF16<HK_IO_NATIVE_ALIGNED,R> { HK_FORCE_INLINE static void apply(const hkSingleReal& self, hkFloat16* HK_RESTRICT p)
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkFloat16)-1) ) == 0, "pointer must be aligned to native size of hkFloat16.");
	p[0].setReal<(R == HK_ROUND_NEAREST)>(hkFloat32(self));
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
