/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Common/Base/Fwd/hkcmath.h>
#include <Common/Base/Fwd/hkcfloat.h>

#define HK_INT32_MIN		(-2147483647 - 1)	// Minimum (signed) int 32-bit value
#define HK_INT32_MAX		2147483647			// Maximum (signed) int 32-bit value
#define HK_SIMD_REAL(x)		hkSimdReal(x)

#if defined(HK_REAL_IS_DOUBLE)
#	define HK_REAL_PI			3.14159265358979
#	define HK_REAL_DEG_TO_RAD	(3.14159265358979 / 180.0)
#	define HK_REAL_RAD_TO_DEG	(180.0 / 3.14159265358979)
#	define HK_REAL_EPSILON		DBL_EPSILON			// smallest such that 1.0+DBL_EPSILON != 1.0
#	define HK_REAL_MIN			DBL_MIN				// min positive value
#	define HK_REAL_MAX			1.7970e+308		// max value - not actually DBL_MAX since on some systems
// FLT_MAX is indistinguishable from NaN or Inf which we reserve
// for error checking.
#	define HK_REAL_HIGH		1.8446726e+150		// Slightly less that sqrt(HK_REAL_MAX).
#else
#	define HK_REAL_PI			3.14159265358979f
#	define HK_REAL_DEG_TO_RAD	(3.14159265358979f / 180.0f)
#	define HK_REAL_RAD_TO_DEG	(180.0f / 3.14159265358979f)
#	define HK_REAL_EPSILON		FLT_EPSILON			// smallest such that 1.0+FLT_EPSILON != 1.0
#	define HK_REAL_MIN			FLT_MIN				// min positive value
#	define HK_REAL_MAX			3.40282e+38f		// max value - not actually FLT_MAX since on some systems
												// FLT_MAX is indistinguishable from NaN or Inf which we reserve
												// for error checking.
#	define HK_REAL_HIGH		1.8446726e+019f		// Slightly less that sqrt(HK_REAL_MAX).
#endif

//
// math functions, might have special implementations on some platforms
//
namespace hkMath
{

#ifndef HK_MATH_sqrt
	HK_FORCE_INLINE static hkFloat32 HK_CALL sqrt(const hkFloat32 r) 
	{ 
		return HK_STD_NAMESPACE::sqrtf(r); 
	}
	HK_FORCE_INLINE static hkDouble64 HK_CALL sqrt(const hkDouble64 r) 
	{ 
		return HK_STD_NAMESPACE::sqrt(r); 
	}
#endif

#ifndef HK_MATH_sqrtInverse
	HK_FORCE_INLINE static hkFloat32 HK_CALL sqrtInverse(const hkFloat32 r) 
	{ 
		return 1.0f / hkMath::sqrt(r); 
	}
	HK_FORCE_INLINE static hkDouble64 HK_CALL sqrtInverse(const hkDouble64 r) 
	{ 
		return 1.0 / hkMath::sqrt(r); 
	}
#endif

#ifndef HK_MATH_fabs
	HK_FORCE_INLINE static hkFloat32 HK_CALL fabs(const hkFloat32 r) 
	{ 
		return HK_STD_NAMESPACE::fabsf(r); 
	}
	HK_FORCE_INLINE static hkDouble64 HK_CALL fabs(const hkDouble64 r) 
	{ 
		return HK_STD_NAMESPACE::fabs(r); 
	}
#endif

#ifndef HK_MATH_abs
	template<typename T>
	HK_FORCE_INLINE static T HK_CALL abs(T t) { return t < T(0) ? -t : t; }

	template<>
	HK_FORCE_INLINE hkFloat32 HK_CALL abs(hkFloat32 r) { return hkMath::fabs(r); }

	template<>
	HK_FORCE_INLINE hkDouble64 HK_CALL abs(hkDouble64 r) { return hkMath::fabs(r); }
#endif

#ifndef  HK_MATH_pow 
	HK_FORCE_INLINE static hkFloat32 HK_CALL pow( const hkFloat32 r, const hkFloat32 s ) 
	{ 
		return HK_STD_NAMESPACE::powf( r, s ); 
	}
	HK_FORCE_INLINE static hkDouble64 HK_CALL pow( const hkDouble64 r, const hkDouble64 s ) 
	{ 
		return HK_STD_NAMESPACE::pow( r, s ); 
	}
#endif

#ifndef HK_MATH_ceil
	HK_FORCE_INLINE static hkFloat32 HK_CALL ceil( const hkFloat32 r ) 
	{ 
		return HK_STD_NAMESPACE::ceilf( r ); 
	}
	HK_FORCE_INLINE static hkDouble64 HK_CALL ceil( const hkDouble64 r ) 
	{ 
		return HK_STD_NAMESPACE::ceil( r ); 
	}
#endif

#ifndef HK_MATH_sin
	HK_FORCE_INLINE static hkFloat32 HK_CALL sin (const hkFloat32 r) 
	{ 
		return HK_STD_NAMESPACE::sinf(r); 
	}
	HK_FORCE_INLINE static hkDouble64 HK_CALL sin (const hkDouble64 r) 
	{ 
		return HK_STD_NAMESPACE::sin(r); 
	}
#endif

#ifndef HK_MATH_cos
	HK_FORCE_INLINE static hkFloat32 HK_CALL cos (const hkFloat32 r) 
	{ 
		return HK_STD_NAMESPACE::cosf(r); 
	}
	HK_FORCE_INLINE static hkDouble64 HK_CALL cos (const hkDouble64 r) 
	{ 
		return HK_STD_NAMESPACE::cos(r); 
	}
#endif

#ifndef HK_MATH_acos
	HK_FORCE_INLINE static hkFloat32 HK_CALL acos(const hkFloat32 r)
	{
		// be generous about numbers slightly outside range
		HK_ASSERT(0x41278654,  hkMath::fabs(r) < 1.001f );
		if( hkMath::fabs(r) >= 1.0f )
		{
			hkFloat32 ret = ( r>0.0f ) ? 0.0f : float(HK_REAL_PI);
			return ret;
		}
		return HK_STD_NAMESPACE::acosf(r);
	}
	HK_FORCE_INLINE static hkDouble64 HK_CALL acos(const hkDouble64 r)
	{
		// be generous about numbers slightly outside range
		HK_ASSERT(0x41278654,  hkMath::fabs(r) < 1.001 );
		if( hkMath::fabs(r) >= 1.0 )
		{
			hkDouble64 ret = ( r>0.0 ) ? 0.0 : hkDouble64(HK_REAL_PI);
			return ret;
		}
		return HK_STD_NAMESPACE::acos(r);
	}
#endif

#ifndef HK_MATH_asin
	HK_FORCE_INLINE static hkFloat32 HK_CALL asin(const hkFloat32 r)
	{
		// be generous about numbers outside range
		HK_ASSERT(0x286a6f5f,  hkMath::fabs(r) < 1.001f );
		if( hkMath::fabs(r) >= 1.0f )
		{
			hkFloat32 ret = ( r>0.0f )	? 0.5f * float(HK_REAL_PI) : -0.5f * float(HK_REAL_PI);
			return ret;
		}
		return HK_STD_NAMESPACE::asinf(r);
	}
	HK_FORCE_INLINE static hkDouble64 HK_CALL asin(const hkDouble64 r)
	{
		// be generous about numbers outside range
		HK_ASSERT(0x286a6f5f,  hkMath::fabs(r) < 1.001 );
		if( hkMath::fabs(r) >= 1.0 )
		{
			hkDouble64 ret = ( r>0.0 )	? 0.5 * hkDouble64(HK_REAL_PI) : -0.5 * hkDouble64(HK_REAL_PI);
			return ret;
		}
		return HK_STD_NAMESPACE::asin(r);
	}
#endif

#ifndef HK_MATH_log
	HK_FORCE_INLINE static hkFloat32 HK_CALL log(const hkFloat32 r)
	{
		return HK_STD_NAMESPACE::logf(r);
	}
	HK_FORCE_INLINE static hkDouble64 HK_CALL log(const hkDouble64 r)
	{
		return HK_STD_NAMESPACE::log(r);
	}
#endif

#ifndef HK_MATH_log
	HK_FORCE_INLINE static hkFloat32 HK_CALL atan2(const hkFloat32 y, const hkFloat32 x)
	{
		return HK_STD_NAMESPACE::atan2f(y,x);
	}
	HK_FORCE_INLINE static hkDouble64 HK_CALL atan2(const hkDouble64 y, const hkDouble64 x)
	{
		return HK_STD_NAMESPACE::atan2(y,x);
	}
#endif

#ifndef HK_MATH_floor
	HK_FORCE_INLINE static hkFloat32 HK_CALL floor(const hkFloat32 r) 
	{ 
		return HK_STD_NAMESPACE::floorf(r); 
	}
	HK_FORCE_INLINE static hkDouble64 HK_CALL floor(const hkDouble64 r) 
	{ 
		return HK_STD_NAMESPACE::floor(r); 
	}
#endif

#ifndef HK_MATH_quadReciprocal
#error math function quadReciprocal not implemented for this platform
	// prototype:	HK_FORCE_INLINE static hkQuadReal HK_CALL quadReciprocal( const hkQuadReal& r )
#endif

#ifndef HK_MATH_quadReciprocalSquareRoot
#error math function quadReciprocalSquareRoot not implemented for this platform
	// prototype: 	HK_FORCE_INLINE static hkQuadReal HK_CALL quadReciprocalSquareRoot( const hkQuadReal& r )
#endif

#ifndef HK_MATH_quadReciprocalTwoIter
#error math function quadReciprocalTwoIter not implemented for this platform
	// prototype:	HK_FORCE_INLINE static hkQuadReal HK_CALL quadReciprocalTwoIter( const hkQuadReal& r )
#endif

#ifndef HK_MATH_quadReciprocalSquareRootTwoIter
#error math function quadReciprocalSquareRootTwoIter not implemented for this platform
	// prototype: 	HK_FORCE_INLINE static hkQuadReal HK_CALL quadReciprocalSquareRootTwoIter( const hkQuadReal& r )
#endif

#ifndef HK_MATH_prefetch128
	HK_FORCE_INLINE static void HK_CALL prefetch128( const void* p) {}
#endif

#ifndef HK_MATH_forcePrefetch
	template<int SIZE>
	HK_FORCE_INLINE static void HK_CALL forcePrefetch( const void* p ) {}
#endif

#ifndef HK_MATH_signBitSet
	HK_FORCE_INLINE static bool HK_CALL signBitSet(const hkFloat32& r)
	{
		return (*(reinterpret_cast<const unsigned int*>(&r)) & 0x80000000) ? true : false;
	}
	HK_FORCE_INLINE static bool HK_CALL signBitSet(const hkDouble64& r)
	{
		return (*(reinterpret_cast<const unsigned long long*>(&r)) & 0x8000000000000000ull) ? true : false;
	}
#endif

#ifndef HK_MATH_hkFloor
	HK_FORCE_INLINE static hkFloat32 HK_CALL hkFloor(const hkFloat32 r)
	{
		union fiu
		{
			hkFloat32 f;
			int i;
			unsigned u;
		};

		//interpret as int
		fiu convert;
		convert.f = r;

		//mask out the fractional part
		int fracMask = 0xff800000;

		//mask out the sign and mantissa
		unsigned exp = convert.u & 0x7f800000;

		//work out the exponent
		//
		//shift down to bottom of number
		exp >>= 0x017;
		//subtract bias of 127
		exp -= 0x07f;

		//rshift is used to shift the fracmask (down to the fractional part)
		int rshift = 0x17 - int(exp);

		//if the exponent is greater than 0x17 (23 bits), then we don't need a
		//frackmask (there is no fractional part, not enough bits to store it)
		//i.e. if rshift >= 0, then leave alone, otherwise set rshift to zero
		int sign = ~(rshift >> 0x01f);
		rshift &= sign;

		//if none of the bits are set in the original number (see ieee floating point
		//standard), then the number = 0
		//we mask out the sign bit, and check if anything is set
		//if it is, we must keep convert.i, otherwise we can set it to 0
		int nexp = (convert.i) & 0x7fffffff;
		nexp = (nexp - 1) >> 0x01f;
		//if the number had no bits, the sign is also destroyed
		convert.i &= ~nexp;

		//shift down to the fractional part
		//if the exponent had been >= 0x17 (23) then the sign destroys with an AND
		//and preserves with an OR (and vice versa). In the case of sign preserving
		//with AND, we end up with frackmask >> exp, else frackmask = 0xffffffff
		fracMask >>= (0x17 & sign) - rshift;
		fracMask |= ~sign;

		//find out whether the floating point is negative
		//sign -> 0xffffffff if neg. 0x00000000 otherwise
		sign = int(convert.u & 0x80000000);
		sign >>= 0x01f;

		int addMask = 0x00800000;

		//if the number is negative AND NOT whole
		//we increase it's magnitude, this is due
		//to the definition of floor
		addMask >>= int(exp);
		//if negative, do the addition (broadcast with sign)
		addMask &= sign;
		//check to see if there was anything in the fractional part
		addMask &= ~((convert.i & (~fracMask)) - 1);
		convert.i += addMask;

		convert.i &= fracMask;

		//if the exponent is negative AND the number is positive
		//then the number is less than 1.0f and floor sets it to 0.0f
		//if it is negative, it gets set to -1.0f
		nexp = int(exp);
		nexp = nexp >> 0x01f;
		//note: 0xbf800000 is -1.0f - which we need to assign in (easier than calculating)
		exp = (0xbf800000 & nexp) & sign;
		convert.i &= ~nexp;
		convert.u |= exp;

		return convert.f;
	}
	HK_FORCE_INLINE static hkDouble64 HK_CALL hkFloor(const hkDouble64 r)
	{
		return hkMath::floor(r);
	}
#endif

#ifndef HK_MATH_hkFloatToInt
	HK_FORCE_INLINE static int HK_CALL hkFloatToInt(const hkFloat32 r)
	{
		union fiu
		{
			hkFloat32 f;
			int i;
			unsigned u;
		};

		//performs a truncation
		//interpret as int
		fiu convert;
		convert.f = r;

		//mask out the fractional part
		int fracMask = 0xff800000;

		//mask out the sign and mantissa
		unsigned int exp = convert.u & 0x7f800000;

		//work out the exponent
		//
		//shift down to bottom of number
		exp >>= 0x017;
		//subtract bias of 127
		exp -= 0x07f;

		//now split the exp into two shift magnitudes, a left
		//shift and a right shift, one of which must be 0
		int lshift = int(exp) - 0x17;
		int rshift = 0x17 - int(exp);

		//if lshift is <=0, then set to zero, otherwise set rshift to zero
		int sign = (lshift-1) >> 0x01f;
		lshift &= ~sign;
		rshift &= sign;

		//if none of the bits are set in the original number (see ieee floating point
		//standard), then the number = 0
		//we mask out the sign bit, and check if anything is set
		//if it is, we must keep convert.i, otherwise we can set it to 0
		int nexp = (convert.i) & 0x7fffffff;
		nexp = (nexp - 1) >> 0x01f;
		//if the number had no bits, the sign is also destroyed
		convert.i &= ~nexp;

		//shift down to the fractional part
		//if the exponent had been >= 0x17 (23) then the sign destroys with an AND
		//and preserves with an OR (and vice versa). In the case of sign preserving
		//with AND, we end up with frackmask >> exp, else frackmask = 0xffffffff
		fracMask >>= (0x17 & sign) - rshift;
		fracMask |= ~sign;

		//find out whether the floating point is negative
		//sign -> 0xffffffff if neg. 0x00000000 otherwise
		sign = int(convert.u & 0x80000000);
		sign >>= 0x01f;

		//get rid of the exponent and sign
		convert.i &= 0x007fffff;
		//insert the 1 that is assumed in the floating point standard
		convert.i |= 0x00800000;

		//truncate
		convert.i &= fracMask;

		//if the sign is negative, convert to 2's complement
		//otherwise leave untouched (ie subtract x from 0 or
		//subtract from x from 2x => -x or x)
		int temp = 0x0;
		temp = (convert.i << 0x01) - 1;
		temp |= sign;
		convert.i = (temp - convert.i) + 1;

		//if the exponent is negative, then the number is less than 1.0f
		//and float to int truncates that to 0
		nexp = int(exp);
		nexp = ~(nexp >> 0x1f);
		convert.i &= nexp;

		//shift mantissa to correct place (one of these will be zero)
		convert.i >>= rshift;
		convert.i <<= lshift;

		return convert.i;
	}
	HK_FORCE_INLINE static int HK_CALL hkFloatToInt(const hkDouble64 r)
	{
		return int(r);
	}
#endif

#ifndef HK_MATH_hkFloorToInt
	HK_FORCE_INLINE static int HK_CALL hkFloorToInt(const hkFloat32 r)
	{
		union fiu
		{
			hkFloat32 f;
			int i;
			unsigned u;
		};

		//interpret as int
		fiu convert;
		convert.f = r;

		//mask out the fractional part
		int fracMask = 0xff800000;

		//mask out the sign and mantissa
		unsigned exp = convert.u & 0x7f800000;

		//work out the exponent
		//
		//shift down to bottom of number
		exp >>= 0x017;
		//subtract bias of 127
		exp -= 0x07f;

		//now split the exp into two shift magnitudes, a left
		//shift and a right shift, one of which must be 0
		int lshift = int(exp) - 0x17;
		int rshift = 0x17 - int(exp);

		//if lshift is <=0, then set to zero, otherwise set rshift to zero
		int sign = (lshift-1) >> 0x01f;
		lshift &= ~sign;
		rshift &= sign;

		
		// check if shift will result in undefined behavior
		// remove bits greater than sizeof( type )
		// if clipped, set mask to set shift to 0x1f
		int clipShift = ( lshift > 0x1f );
		lshift &= 0x1f;
		clipShift = 0 - clipShift;
		lshift |= ( clipShift & 0x1f );

		// clip rshift
		clipShift = ( rshift > 0x1f );
		rshift &= 0x1f;
		clipShift = 0 - clipShift;
		rshift |= ( clipShift & 0x1f );

		//if none of the bits are set in the original number (see ieee floating point
		//standard), then the number = 0
		//we mask out the sign bit, and check if anything is set
		//if it is, we must keep convert.i, otherwise we can set it to 0
		int nexp = (convert.i) & 0x7fffffff;
		nexp = (nexp - 1) >> 0x01f;
		//if the number had no bits, the sign is also destroyed
		convert.i &= ~nexp;

		//shift down to the fractional part
		//if the exponent had been >= 0x17 (23) then the sign destroys with an AND
		//and preserves with an OR (and vice versa). In the case of sign preserving
		//with AND, we end up with frackmask >> exp, else frackmask = 0xffffffff
		fracMask >>= (0x17 & sign) - rshift;
		fracMask |= ~sign;

		//find out whether the floating point number is whole
		//check if a v large number (lshift will be >= 0, => rshift will == 0)
		int whole = ( ( rshift - 1 ) >> 0x1f );
		//check if not < 1 (fracMask will == -1), and if fraction part == 0
		whole |= ( ( fracMask + 1 ) >> 0x1f ) & ( ( ( convert.i & ~fracMask ) - 1 ) >> 0x1f );

		//find out whether the floating point is negative
		//sign -> 0xffffffff if neg. 0x00000000 otherwise
		sign = int(convert.u & 0x80000000);
		sign >>= 0x01f;

		//get rid of the exponent and sign
		convert.i &= 0x007fffff;
		//insert the 1 that is assumed in the floating point standard
		convert.i |= 0x00800000;

		//truncate
		convert.i &= fracMask;

		//if the sign is negative, convert to 2's complement
		//otherwise leave untouched (ie subtract x from 0 or
		//subtract from x from 2x -> -x or x
		int temp = 0x0;
		temp = (convert.i << 0x01) - 1;
		temp |= sign;
		convert.i = (temp - convert.i) + 1;

		//if the exponent is negative, then the number is less than 1.0f. truncate to 0
		nexp = int(exp);
		nexp = ~(nexp >> 0x1f);
		convert.i &= nexp;

		//shift mantissa to correct place (one of these will be zero)
		convert.i >>= rshift;
		convert.i <<= lshift;

		// floor increases the magnitude of negative, non-whole values by 1
		// subtract 0x01 if negative and not whole
		int incNeg = ( 0x00000001 & sign ) & ~whole;
		convert.i -= incNeg;

		return convert.i;
	}
	HK_FORCE_INLINE static int HK_CALL hkFloorToInt(const hkDouble64 r)
	{
		return int(hkMath::floor(r));
	}
#endif

#ifndef HK_MATH_hkToIntFast
	HK_FORCE_INLINE static int HK_CALL hkToIntFast( const hkReal r )
	{
		return int(r);
	}
#endif

#ifndef HK_MATH_equal
	HK_FORCE_INLINE static bool HK_CALL equal(hkReal x, hkReal y, hkReal tolerance2=hkReal(1e-5f))
	{
		return hkMath::fabs(x-y) < tolerance2;
	}
#endif

#ifndef HK_MATH_max2
	template <typename T>
	HK_FORCE_INLINE static T HK_CALL max2( T x, T y)
	{
		return x > y ? x : y;
	}
#endif

#ifndef HK_MATH_min2
	template <typename T>
	HK_FORCE_INLINE static T HK_CALL min2( T x, T y)
	{
		return x < y ? x : y;
	}
#endif

#ifndef HK_MATH_clamp
	template <typename T>
	HK_FORCE_INLINE static T HK_CALL clamp( T x, T mi, T ma)
	{
		if ( x < mi ) return mi;
		if ( x > ma ) return ma;
		return x;
	}
#endif

#ifndef HK_MATH_isFinite
	HK_FORCE_INLINE static bool HK_CALL isFinite(const hkFloat32 r)
	{
		// Check the 8 exponent bits.
		// Usually NAN == (exponent = all 1, mantissa = non-zero)
		//         INF == (exponent = all 1, mantissa = zero)
		// This simply checks the exponent
		HK_ASSERT(0x2d910c70, sizeof(hkFloat32) == sizeof(unsigned int));
		union {
			hkFloat32 f;
			unsigned int i;
		} val;

		val.f = r;
		return ((val.i & 0x7f800000) != 0x7f800000);
	}
	HK_FORCE_INLINE static bool HK_CALL isFinite(const hkDouble64 r)
	{
		// Check the 11 exponent bits.
		// Usually NAN == (exponent = all 1, mantissa = non-zero)
		//         INF == (exponent = all 1, mantissa = zero)
		// This simply checks the exponent
		HK_ASSERT(0x2d910c70, sizeof(hkDouble64) == sizeof(unsigned long long));
		union {
			hkDouble64 f;
			unsigned long long i;
		} val;
		val.f = r;
		return ((val.i & 0x7ff0000000000000ull) != 0x7ff0000000000000ull);
	}
#endif	

#ifndef HK_MATH_isPower2
	HK_FORCE_INLINE static bool isPower2(unsigned int v)
	{
		return v && !(v & (v - 1));
	}
#endif

#ifndef HK_MATH_fselectGreaterEqualZero
	HK_FORCE_INLINE static hkReal HK_CALL fselectGreaterEqualZero( hkReal testVar, hkReal ifTrue, hkReal ifFalse)
	{
		return (testVar >= hkReal(0)) ? ifTrue : ifFalse;
	}
#endif

#ifndef HK_MATH_fselectGreaterZero
	HK_FORCE_INLINE static hkReal HK_CALL fselectGreaterZero( hkReal testVar, hkReal ifTrue, hkReal ifFalse)
	{
		return (testVar > hkReal(0)) ? ifTrue : ifFalse;
	}
#endif

#ifndef HK_MATH_fselectLessEqualZero
	HK_FORCE_INLINE static hkReal HK_CALL fselectLessEqualZero( hkReal testVar, hkReal ifTrue, hkReal ifFalse)
	{
		return (testVar <= hkReal(0)) ? ifTrue : ifFalse;
	}
#endif

#ifndef HK_MATH_fselectLessZero
	HK_FORCE_INLINE static hkReal HK_CALL fselectLessZero( hkReal testVar, hkReal ifTrue, hkReal ifFalse)
	{
		return (testVar < hkReal(0)) ? ifTrue : ifFalse;
	}
#endif

#ifndef HK_MATH_fselectEqualZero
	HK_FORCE_INLINE static hkReal HK_CALL fselectEqualZero( hkReal testVar, hkReal ifTrue, hkReal ifFalse)
	{
		return (testVar == hkReal(0)) ? ifTrue : ifFalse;
	}
#endif

#ifndef HK_MATH_fselectNotEqualZero
	HK_FORCE_INLINE static hkReal HK_CALL fselectNotEqualZero( hkReal testVar, hkReal ifTrue, hkReal ifFalse)
	{
		return (testVar != hkReal(0)) ? ifTrue : ifFalse;
	}
#endif

#ifndef HK_MATH_atan2Approximation
	HK_FORCE_INLINE static hkReal HK_CALL atan2Approximation( const hkReal& y, const hkReal& x )
	{
		return hkMath::atan2(y,x);
	}
#endif

	/// Old deprecated function, do not use in new code
	/// Average absolute error 0.003315
	/// Max absolute error 0.016747
	/// About 4x faster than ::atan2f
	/// \warning x and y parameter names are wrong! You have to call this with the same
	/// parameter sequence as ::atan2f
	HK_FORCE_INLINE static hkReal HK_CALL atan2fApproximation( hkReal x, hkReal y ) 
	{
		hkReal fx = hkMath::fabs(x);
		hkReal fy = hkMath::fabs(y);

		hkReal result;
		const hkReal c2 = hkReal(-0.121079f);
		const hkReal c3 = HK_REAL_PI * hkReal(0.25f) - hkReal(1) - c2;

		{
			if ( fx <= fy )
			{
				fy += HK_REAL_EPSILON;
				hkReal a = fx / fy;
				result = a;
				result += c2 * a*a;
				result += c3 * a*a*a;
			}
			else
			{
				fx += HK_REAL_EPSILON;
				hkReal a = fy / fx;
				result = a;
				result += c2 * a*a;
				result += c3 * a*a*a;
				result = HK_REAL_PI * hkReal(0.5f) - result;
			}
		}

		if ( y < hkReal(0))
		{
			result = HK_REAL_PI - result;
		}

		if ( x < hkReal(0) )
		{
			result = -result;
		}
		return result;
	}


#ifndef HK_MATH_logApproximation
	HK_FORCE_INLINE static hkReal HK_CALL logApproximation(const hkReal& r)
	{
		return hkMath::log(r);
	}
#endif

#ifndef HK_MATH_intInRange
		/// Returns any nonzero value if lowInclusive<=value and value<highExclusive.
	HK_FORCE_INLINE static hkBool32 HK_CALL intInRange( int value, int lowInclusive, int highExclusive )
	{
		return (lowInclusive <= value) & (value < highExclusive);
	}
#endif

#ifndef HK_MATH_interpolate2d
	/// Interpolates between y0 and y1 using x0,x1 interval as the basis
	HK_FORCE_INLINE static hkReal HK_CALL interpolate2d( hkReal x, hkReal x0, hkReal x1, hkReal y0, hkReal y1 )
	{
		//HK_ASSERT2(0x2342ab9,(x<=x0)||(x>=x1),"x is not from interval <x0,x1>!");
		HK_ASSERT2(0x2342ab9,(x0 != x1), "no proper interval defined!");
		return y0 + (x-x0)*(y1-y0)/(x1-x0);
	}
#endif

#ifndef HK_MATH_fastExp
	// Approximate, fast versions of e^x. See http://cnl.salk.edu/~schraudo/pubs/Schraudolph99.pdf for further info.

	// Double version. Error: Mean 1.5%, RMS: 1.8%, max 4.4%
	HK_FORCE_INLINE hkDouble64 fastExp(hkDouble64 val) 
	{
		int tmp = (int) (1512775 * val + 1072693248 - 68243); // 1512775 = 2^20/ln(2)  1072693248 = (2^10-1)*2^20  68243 = 0.045111411*2^20/ln(2) (lowest mean error) (60801 = lowest RMS)
		double p = 0.0;
#if HK_ENDIAN_LITTLE
		*(1 + (int * ) &p) = tmp;
#else
		*((int * ) &p) = tmp;
#endif
		return p;
	}

	// Float version. Error (over the interval -5.0 to 5.0): Mean 1.5%, RMS is 1.9%, max 4.4%
	HK_FORCE_INLINE hkFloat32 fastExp(hkFloat32 x) 
	{
		int y = (int) (12102203*x + 1065353216 - 545947); 
		// 12102203 = 2^23/ln(2)  1065353216 = (2^7-1)*2^23  545947 = 0.045111411*2^23/ln(2) (lowest mean error)
		return *((hkFloat32*)&y);
	}
#endif

#ifndef HK_MATH_countLeadingZeros
	HK_FORCE_INLINE static hkUint32 countLeadingZeros(hkUint32 x)
	{
		// Fill all the bits to the right of set bits
		x |= (x >> 1);
		x |= (x >> 2);
		x |= (x >> 4);
		x |= (x >> 8);
		x |= (x >> 16);

		// Do a bit population count (number of ones)
		x -= ((x >> 1) & 0x55555555);
		x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
		x = (((x >> 4) + x) & 0x0f0f0f0f);
		x += (x >> 8);
		x += (x >> 16);
		x = (x & 0x0000003f);

		return 32 - x;
	}
#endif

	// Compile time integer log2
	template <int VALUE>	struct Log2		{ enum { ANSWER = Log2<VALUE/2>::ANSWER + 1 }; };
	template <>				struct Log2<1>	{ enum { ANSWER = 0 }; };

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
