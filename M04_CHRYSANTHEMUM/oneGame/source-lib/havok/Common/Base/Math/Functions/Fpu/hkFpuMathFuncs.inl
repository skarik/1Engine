/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Common/Base/Fwd/hkcmath.h>
#include <Common/Base/Fwd/hkcfloat.h>

#define HK_STD_NAMESPACE /*nothing*/

namespace hkMath
{
	HK_FORCE_INLINE static hkFloat32 HK_CALL invSqrtF32Approx12Bit( const hkFloat32 x )
	{
		const float xhalf = 0.5f * x;
		int i = *(int*)&x;
		i = 0x5f375a86 - (i>>1); // Lomonts initial guess
		float r = *(float*)&i;

		// Newton steps to improve
		r = (1.5f * r) - (xhalf * r) * (r * r);
		r = (1.5f * r) - (xhalf * r) * (r * r);
		return r;
	}

	HK_FORCE_INLINE static hkFloat32 HK_CALL invSqrtF32Approx23Bit( const hkFloat32 x )
	{
		const float xhalf = 0.5f * x;
		int i = *(int*)&x;
		i = 0x5f375a86 - (i>>1); // Lomonts initial guess
		float r = *(float*)&i;

		// Newton steps to improve
		r = (1.5f * r) - (xhalf * r) * (r * r);
		r = (1.5f * r) - (xhalf * r) * (r * r);
		r = (1.5f * r) - (xhalf * r) * (r * r);
		return r;
	}

	HK_FORCE_INLINE static hkFloat32 HK_CALL rcpF32Approx12Bit( const hkFloat32 x )
	{
		int i = (2 * 0x3F800000) - *(int *)&x; // float 1.0 bits
		float r = *(float *)&i;

		// Newton steps to improve
		r = r * (2.0f - x * r);
		r = r * (2.0f - x * r);
		return r;
	}

	HK_FORCE_INLINE static hkFloat32 HK_CALL rcpF32Approx23Bit( const hkFloat32 x )
	{
		int i = (2 * 0x3F800000) - *(int *)&x; // float 1.0 bits
		float r = *(float *)&i;

		// Newton steps to improve
		r = r * (2.0f - x * r);
		r = r * (2.0f - x * r);
		r = r * (2.0f - x * r);
		return r;
	}

	//
	// need to implement
	//
#	define HK_MATH_quadReciprocal
	HK_FORCE_INLINE static hkQuadReal HK_CALL quadReciprocal( const hkQuadReal& r )
	{
		hkQuadReal q;
		q.v[0] = hkReal(rcpF32Approx23Bit(hkFloat32(r.v[0])));
		q.v[1] = hkReal(rcpF32Approx23Bit(hkFloat32(r.v[1])));
		q.v[2] = hkReal(rcpF32Approx23Bit(hkFloat32(r.v[2])));
		q.v[3] = hkReal(rcpF32Approx23Bit(hkFloat32(r.v[3])));
		return q;
	}

#	define HK_MATH_quadReciprocalSquareRoot
	HK_FORCE_INLINE static hkQuadReal HK_CALL quadReciprocalSquareRoot( const hkQuadReal& r )
	{
		hkQuadReal q;
		q.v[0] = hkReal(invSqrtF32Approx23Bit(hkFloat32(r.v[0])));
		q.v[1] = hkReal(invSqrtF32Approx23Bit(hkFloat32(r.v[1])));
		q.v[2] = hkReal(invSqrtF32Approx23Bit(hkFloat32(r.v[2])));
		q.v[3] = hkReal(invSqrtF32Approx23Bit(hkFloat32(r.v[3])));
		return q;
	}

#	define HK_MATH_quadReciprocalTwoIter
	HK_FORCE_INLINE static hkQuadReal HK_CALL quadReciprocalTwoIter( const hkQuadReal& r )
	{
		hkQuadReal q;
		q.v[0] = hkReal(1) / r.v[0];
		q.v[1] = hkReal(1) / r.v[1];
		q.v[2] = hkReal(1) / r.v[2];
		q.v[3] = hkReal(1) / r.v[3];
		return q;
	}

#	define HK_MATH_quadReciprocalSquareRootTwoIter
	HK_FORCE_INLINE static hkQuadReal HK_CALL quadReciprocalSquareRootTwoIter( const hkQuadReal& r )
	{
		hkQuadReal q;
#if defined(HK_REAL_IS_DOUBLE)
		q.v[0] = hkReal(1) / HK_STD_NAMESPACE::sqrt(r.v[0]);
		q.v[1] = hkReal(1) / HK_STD_NAMESPACE::sqrt(r.v[1]);
		q.v[2] = hkReal(1) / HK_STD_NAMESPACE::sqrt(r.v[2]);
		q.v[3] = hkReal(1) / HK_STD_NAMESPACE::sqrt(r.v[3]);
#else
		q.v[0] = hkReal(1) / HK_STD_NAMESPACE::sqrtf(r.v[0]);
		q.v[1] = hkReal(1) / HK_STD_NAMESPACE::sqrtf(r.v[1]);
		q.v[2] = hkReal(1) / HK_STD_NAMESPACE::sqrtf(r.v[2]);
		q.v[3] = hkReal(1) / HK_STD_NAMESPACE::sqrtf(r.v[3]);
#endif
		return q;
	}

	// 
	// FPU optimized implementations
	//

#	define HK_MATH_hkToIntFast
	HK_FORCE_INLINE static int HK_CALL hkToIntFast( const hkFloat32 a )
	{
		union float_bits
		{
			hkFloat32 f;
			hkUint32 u;
		};

		float_bits fb;
		fb.f = a;

		// Convert float to signed int, with AltiVec style overflow
		// (i.e. large float -> 0x7fffffff instead of 0x80000000)

		int e = ((fb.u & 0x7F800000) >> 23) - 127;
		if (e < 0) // clip fractional numbers
			return 0;
		int r = (fb.u & 0x007FFFFF) | 0x00800000;
		int sign = (fb.u & 0x80000000) ? -1 : 1;
		if (e > 23) // adjust with round to zero
			r <<= (e - 23);
		else
			r >>= (23 - e);
		return ((sign * r) & 0x7FFFFFFF) | (fb.u & 0x80000000); // compute dual and set sign bit
	}
	HK_FORCE_INLINE static int HK_CALL hkToIntFast( const hkDouble64 r )
	{
		return hkToIntFast(hkFloat32(r));
	}

#define HK_MATH_min2
	template <typename T>
	HK_FORCE_INLINE static T HK_CALL min2( T x, T y)
	{
		return x < y ? x : y;
	}

#if defined(HK_ARCH_IA32) && ( defined(HK_COMPILER_MSVC) || defined(HK_COMPILER_INTEL) )
	template <>
	HK_FORCE_INLINE static hkFloat32 HK_CALL min2<hkFloat32>( hkFloat32 x, hkFloat32 y)
	{
		hkFloat32 r;
		__asm {
			fld			x
			fld         y
			fucomi		st(0), st(1)
			fcmovnb		st(0), st(1)
			fstp		r
			fstp		st(0)
		}
		return r;
	}
	template <>
	HK_FORCE_INLINE static hkDouble64 HK_CALL min2<hkDouble64>( hkDouble64 x, hkDouble64 y)
	{
		hkDouble64 r;
		__asm {
			fld			x
			fld         y
			fucomi		st(0), st(1)
			fcmovnb		st(0), st(1)
			fstp		r
			fstp		st(0)
		}
		return r;
	}
#endif

#define HK_MATH_max2
	template <typename T>
	HK_FORCE_INLINE static T HK_CALL max2( T x, T y)
	{
		return x > y ? x : y;
	}

#if defined(HK_ARCH_IA32) && ( defined(HK_COMPILER_MSVC) || defined(HK_COMPILER_INTEL) )
	template <>
	HK_FORCE_INLINE static hkFloat32 HK_CALL max2<hkFloat32>( hkFloat32 x, hkFloat32 y)
	{
		hkFloat32 r;
		__asm {
			fld			x
			fld         y
			fucomi		st(0), st(1)
			fcmovb		st(0), st(1)
			fstp		r
			fstp		st(0)
		}
		return r;
	}
	template <>
	HK_FORCE_INLINE static hkDouble64 HK_CALL max2<hkDouble64>( hkDouble64 x, hkDouble64 y)
	{
		hkDouble64 r;
		__asm {
			fld			x
			fld         y
			fucomi		st(0), st(1)
			fcmovb		st(0), st(1)
			fstp		r
			fstp		st(0)
		}
		return r;
	}
#endif //  defined(HK_ARCH_IA32) && ( defined(HK_COMPILER_MSVC) || defined(HK_COMPILER_INTEL) )

#define HK_MATH_clamp
	template <typename T>
	HK_FORCE_INLINE static T HK_CALL clamp( T x, T mi, T ma)
	{
		if ( x < mi ) return mi;
		if ( x > ma ) return ma;
		return x;
	}

#if defined(HK_ARCH_IA32) && ( defined(HK_COMPILER_MSVC) || defined(HK_COMPILER_INTEL) )
	template <>
	HK_FORCE_INLINE static hkFloat32 HK_CALL clamp<hkFloat32>( hkFloat32 x, hkFloat32 mi, hkFloat32 ma)
	{
		hkFloat32 r;
		__asm {
			fld         ma
			fld         mi
			fld			x
			fucomi		st(0), st(1)
			fcmovb		st(0), st(1)
			fucomi		st(0), st(2)
			fcmovnb		st(0), st(2)
			fstp		r
			fstp		st(0)
			fstp		st(0)
		}
		return r;
	}
	template <>
	HK_FORCE_INLINE static hkDouble64 HK_CALL clamp<hkDouble64>( hkDouble64 x, hkDouble64 mi, hkDouble64 ma)
	{
		hkDouble64 r;
		__asm {
			fld         ma
			fld         mi
			fld			x
			fucomi		st(0), st(1)
			fcmovb		st(0), st(1)
			fucomi		st(0), st(2)
			fcmovnb		st(0), st(2)
			fstp		r
			fstp		st(0)
			fstp		st(0)
		}
		return r;
	}

#define HK_MATH_isFinite
	HK_FORCE_INLINE static bool HK_CALL isFinite(const hkFloat32 r)
	{
		hkUint16 statusreg;
		__asm {
			fld			r
			fxam
			fstsw		statusreg
			fstp		st(0)
			fwait
		}
		statusreg &= 0x4500;
		return (statusreg & 0x4400) == statusreg; // accept normal finite numbers and zero
	}
	HK_FORCE_INLINE static bool HK_CALL isFinite(const hkDouble64 r)
	{
		hkUint16 statusreg;
		__asm {
			fld			r
			fxam
			fstsw		statusreg
			fstp		st(0)
			fwait
		}
		statusreg &= 0x4500;
		return (statusreg & 0x4400) == statusreg; // accept normal finite numbers and zero
	}
#endif // defined(HK_ARCH_IA32) && ( defined(HK_COMPILER_MSVC) || defined(HK_COMPILER_INTEL) )


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
