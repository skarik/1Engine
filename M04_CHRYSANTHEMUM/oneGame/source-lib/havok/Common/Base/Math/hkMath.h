/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HK_MATH_MATH_H
#define HK_MATH_MATH_H

#include <Common/Base/Types/hkBaseTypes.h>
#include <Common/Base/Config/hkConfigSimd.h>

// Temporarily disable the alignment warning for AMD64
#if defined(HK_PLATFORM_X64)
#	pragma warning( disable: 4328 )
#endif

#define hkSimdReal_Minus1 (hkSimdReal::getConstant<HK_QUADREAL_MINUS1>())
#define hkSimdReal_0 (hkSimdReal::getConstant<HK_QUADREAL_0>())
#define hkSimdReal_1 (hkSimdReal::getConstant<HK_QUADREAL_1>())
#define hkSimdReal_2 (hkSimdReal::getConstant<HK_QUADREAL_2>())
#define hkSimdReal_3 (hkSimdReal::getConstant<HK_QUADREAL_3>())
#define hkSimdReal_4 (hkSimdReal::getConstant<HK_QUADREAL_4>())
#define hkSimdReal_5 (hkSimdReal::getConstant<HK_QUADREAL_5>())
#define hkSimdReal_6 (hkSimdReal::getConstant<HK_QUADREAL_6>())
#define hkSimdReal_7 (hkSimdReal::getConstant<HK_QUADREAL_7>())
#define hkSimdReal_15 (hkSimdReal::getConstant<HK_QUADREAL_15>())
#define hkSimdReal_16 (hkSimdReal::getConstant<HK_QUADREAL_16>())
#define hkSimdReal_255 (hkSimdReal::getConstant<HK_QUADREAL_255>())
#define hkSimdReal_256 (hkSimdReal::getConstant<HK_QUADREAL_256>())

#define hkSimdReal_Inv0 (hkSimdReal::getConstant<HK_QUADREAL_INV_0>())
#define hkSimdReal_Inv1 (hkSimdReal::getConstant<HK_QUADREAL_INV_1>())
#define hkSimdReal_Inv2 (hkSimdReal::getConstant<HK_QUADREAL_INV_2>())
#define hkSimdReal_Inv3 (hkSimdReal::getConstant<HK_QUADREAL_INV_3>())
#define hkSimdReal_Inv4 (hkSimdReal::getConstant<HK_QUADREAL_INV_4>())
#define hkSimdReal_Inv5 (hkSimdReal::getConstant<HK_QUADREAL_INV_5>())
#define hkSimdReal_Inv6 (hkSimdReal::getConstant<HK_QUADREAL_INV_6>())
#define hkSimdReal_Inv7 (hkSimdReal::getConstant<HK_QUADREAL_INV_7>())

#define hkSimdReal_Inv_15 (hkSimdReal::getConstant<HK_QUADREAL_INV_15>())
#define hkSimdReal_Inv_127 (hkSimdReal::getConstant<HK_QUADREAL_INV_127>())
#define hkSimdReal_Inv_255 (hkSimdReal::getConstant<HK_QUADREAL_INV_255>())

#define hkSimdReal_Max (hkSimdReal::getConstant<HK_QUADREAL_MAX>())
#define hkSimdReal_Eps (hkSimdReal::getConstant<HK_QUADREAL_EPS>())
#define hkSimdReal_EpsSqrd (hkSimdReal::getConstant<HK_QUADREAL_EPS_SQRD>())

#define hkSimdReal_PackHalf (hkSimdReal::getConstant<HK_QUADREAL_PACK_HALF>())
#define hkSimdReal_Pack16 (hkSimdReal::getConstant<HK_QUADREAL_PACK16_UNIT_VEC>())
#define hkSimdReal_Unpack16 (hkSimdReal::getConstant<HK_QUADREAL_UNPACK16_UNIT_VEC>())

#define hkSimdReal_Pi (hkSimdReal::getConstant<HK_QUADREAL_PI>())
#define hkSimdReal_PiOver2 (hkSimdReal::getConstant<HK_QUADREAL_PI_HALF>())
#define hkSimdReal_PiOver4 (hkSimdReal::getConstant<HK_QUADREAL_PI_QUARTER>())

//duplicate of hkSimdReal_Inv2 
#define hkSimdReal_Half (hkSimdReal::getConstant<HK_QUADREAL_INV_2>())





	/// A list of constants.
	/// \warning Do not change the order of the following constants and keep synchronized with value array: g_vectorConstants[] because algorithms rely on this order.
enum hkVectorConstant
{
	HK_QUADREAL_MINUS1,
	HK_QUADREAL_0,
	HK_QUADREAL_1,
	HK_QUADREAL_2,
	HK_QUADREAL_3,
	HK_QUADREAL_4,
	HK_QUADREAL_5,
	HK_QUADREAL_6,
	HK_QUADREAL_7,
	HK_QUADREAL_15,
	HK_QUADREAL_16,
	HK_QUADREAL_255,
	HK_QUADREAL_256,
	HK_QUADREAL_2_POW_23,

	HK_QUADREAL_INV_0,	///< 0.0f, 0.0f, 0.0f
	HK_QUADREAL_INV_1,	///< 1 1 1 1
	HK_QUADREAL_INV_2,	///< .5 .5 .5 .5
	HK_QUADREAL_INV_3,	///< 1/3 1/3 1/3 1/3
	HK_QUADREAL_INV_4,
	HK_QUADREAL_INV_5,
	HK_QUADREAL_INV_6,
	HK_QUADREAL_INV_7,
	HK_QUADREAL_INV_15,		///< 1/15 x 4
	HK_QUADREAL_INV_127,	///< 1/127 x 4
	HK_QUADREAL_INV_226,	///< 1/226 x 4, Used to unpack AABB.
	HK_QUADREAL_INV_255,	///< 1/255 x 4
	HK_QUADREAL_1000,   ///< The next 4 are also the matrix identity, don't change the order.
	HK_QUADREAL_0100,
	HK_QUADREAL_0010,
	HK_QUADREAL_0001,
	HK_QUADREAL_MAX,
	HK_QUADREAL_EPS,
	HK_QUADREAL_EPS_SQRD,	///< Eps squared, really really tiny number
	HK_QUADREAL_m11m11,	///< -1,1,-1,1
	HK_QUADREAL_1010,   ///< 1,0,1,0
	HK_QUADREAL_1100,   ///< 1,1,0,0
	HK_QUADREAL_1248,   ///< 1,2,4,8
	HK_QUADREAL_8421,   ///< 8,4,2,1
	HK_QUADREAL_PACK_HALF,	///< Used to pack hkHalfs
	HK_QUADREAL_PACK16_UNIT_VEC,	///< Used to compress unit vectors
	HK_QUADREAL_UNPACK16_UNIT_VEC,	///< Used to uncompress unit vectors
	HK_QUADREAL_PI,
	HK_QUADREAL_PI_HALF,
	HK_QUADREAL_PI_QUARTER,

	HK_QUADREAL_END
};

	/// A list of constants.
	/// \warning Do not change the order of the following constants and keep synchronized with value array: g_intVectorConstants[] because algorithms rely on this order.
enum hkIntVectorConstant
{
	HK_QUADINT_0,
	HK_QUADINT_1,
	HK_QUADINT_2,
	HK_QUADINT_4,
	HK_QUADINT_0123,
	HK_QUADINT_0123_INT24W,	///< lower bits hold 0123, upper bits are set, so that reinterpreting this value as float will give a valid floating point number

	// Permutation constants, for Xbox broadcast(i), do not change order!
	HK_QUADINT_PERM_XXXX,
	HK_QUADINT_PERM_YYYY,
	HK_QUADINT_PERM_ZZZZ,
	HK_QUADINT_PERM_WWWW,

	HK_QUADINT_END
};

/// Permutations of 4-component vectors hkVector4 and hkIntVector
struct hkVectorPermutation
{
	enum Permutation 
	{
		XYZW = 0x0123, // identity
		WZYX = 0x3210, // reverse

		WXYZ = 0x3012, // shift
		ZWXY = 0x2301,
		YZWX = 0x1230,

		XYWZ = 0x0132, // swap
		YXWZ = 0x1032,
		YXZW = 0x1023,
		YZXW = 0x1203,
		YWZX = 0x1320,
		WYZX = 0x3120,
		XZYW = 0x0213,
		ZXYW = 0x2013,

		XYXY = 0x0101, // pairs
		XXYY = 0x0011,
		YYXX = 0x1100,
		ZWZW = 0x2323,
		ZYXZ = 0x2102,
		ZYZZ = 0x2122,
		XZXZ = 0x0202,
		XZYZ = 0x0212,
		YWYW = 0x1313,
		YXWW = 0x1033,
		YXYX = 0x1010,
		XYWW = 0x0133,
		XYZZ = 0x0122,
		XWYW = 0x0313,
		WZWZ = 0x3232,

		XWYZ = 0x0312,
		XZWY = 0x0231, 

		XXXX = 0x0000, // broadcasts
		YYYY = 0x1111,
		ZZZZ = 0x2222,
		WWWW = 0x3333,
		XWWW = 0x0333,
		WXWW = 0x3033,
		WWXW = 0x3303,
		XXZZ = 0x0022,
		YYWW = 0x1133,
		WYWW = 0x3133,
		WWYW = 0x3313,
		WWZW = 0x3323,
		ZWWW = 0x2333,

		ZXYZ = 0x2012,
		YZXZ = 0x1202,
	};
};

/// Flag for advanced hkSimdReal/hkVector4 interface to control the
/// floating point accuracy of division and sqrt calculations.
/// Note that for full precision the returned accuracy depends on the definition
/// of hkReal.
enum hkMathAccuracyMode
{
	HK_ACC_FULL,    ///< Do calculations using full floating point precision and produce IEEE compliant return values.
	HK_ACC_23_BIT,  ///< Do calculations such that at least 23 mantissa bits are accurate. Denormals handling is allowed to be different to IEEE if it results in better performance.
	HK_ACC_12_BIT   ///< Do calculations such that at least 12 mantissa bits are accurate. Denormals handling is allowed to be different to IEEE if it results in better performance.
};

/// Flag for advanced hkSimdReal/hkVector4 interface to control the
/// behavior of the method to check for division by zero.
enum hkMathDivByZeroMode
{
	HK_DIV_IGNORE,           ///< No checking. The result is platform dependent and can signal floating point errors.
	HK_DIV_SET_ZERO,         ///< Check and return zero in case. This will not produce floating point errors.
	HK_DIV_SET_ZERO_AND_ONE, ///< Check and return zero in case. This will not produce floating point errors. Additionally round all values to 1.0 which are closer than machine epsilon.
	HK_DIV_SET_MAX,          ///< Check and return the maximum representable floating point value in case. This will not produce floating point errors.
	HK_DIV_SET_HIGH          ///< Check and return a huge value in case. This will not produce floating point errors.
};

/// Flag for advanced hkSimdReal/hkVector4 interface to control the
/// behavior of the method to check for negative arguments of square root.
/// For inverse square roots, the checking versions also catch divide by zero.
enum hkMathNegSqrtMode
{
	HK_SQRT_IGNORE,   ///< No checking. The result is platform dependent and can signal floating point errors.
	HK_SQRT_SET_ZERO  ///< Check and return zero in case. This will not produce floating point errors.
};

/// Flag for advanced hkSimdReal/hkVector4 interface to control the
/// way values are loaded/written from memory. In general more aligned addresses
/// give faster reads/writes.
enum hkMathIoMode
{
	HK_IO_BYTE_ALIGNED,		///< The pointer can have an arbitrary address.
	HK_IO_NATIVE_ALIGNED,	///< The pointer must be aligned to the native size of the data pointed to. Example: load floats: align to sizeof(float)
	HK_IO_SIMD_ALIGNED,		///< The pointer must be aligned to the next power of 2 of the number of bytes to load. Example: load 3 floats: 3*4=12 -> 16
	HK_IO_NOT_CACHED		///< HK_IO_SIMD_ALIGNED + On platforms which support it, the values are loaded bypassing the processor caches. Use for reading data which is only needed once or for writing data which is not needed until much later.
};

/// Flag for advanced hkSimdReal/hkVector4 interface to control the
/// rounding behavior when floating point values are written to lower precision formats.
enum hkMathRoundingMode
{
	HK_ROUND_TRUNCATE,	///< The values get truncated to the packed representation.
	HK_ROUND_NEAREST	///< The values get rounded to the packed representation.
};

#if defined(HK_PLATFORM_CAFE)
#	include <Common/Base/Math/Types/Cafe/hkCafeMathTypes.inl>
#elif HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED
#	if defined(HK_COMPILER_HAS_INTRINSICS_IA32)
#		include <Common/Base/Math/Types/Sse/hkSseMathTypes.inl>
#	elif defined(HK_PLATFORM_XBOX360)
#		include <Common/Base/Math/Types/Xbox360/hkXbox360MathTypes.inl>
#	elif defined(HK_ARCH_PS3) || defined(HK_ARCH_PS3SPU)
#		define HK_PLATFORM_SUPPORTS_MANY_SIMD_REGISTERS
#		include <Common/Base/Math/Types/Ps3/hkPs3MathTypes.inl>
#	elif defined(HK_COMPILER_HAS_INTRINSICS_NEON)
#		define HK_PLATFORM_SUPPORTS_MANY_SIMD_REGISTERS
#		include <Common/Base/Math/Types/Neon/hkNeonMathTypes.inl>
#	elif defined(HK_PLATFORM_LRB)
#		include <Common/Base/Math/Types/Lrb/hkLrbMathTypes.inl>
#	else // simd
#		error Unknown platform for SIMD
#	endif
#else // not simd
#		include <Common/Base/Math/Types/Fpu/hkFpuMathTypes.inl>
#endif

#ifndef HK_MASK_TO_VECTOR
#define HK_MASK_TO_VECTOR(x) (hkQuadReal)x
#endif

extern const hkQuadReal g_vectorConstants[HK_QUADREAL_END];

#if defined(HK_PLATFORM_CAFE)
#include <Common/Base/Math/Functions/Cafe/hkCafeMathFuncs.inl>
#elif HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED
#	if defined(HK_COMPILER_HAS_INTRINSICS_IA32)
#		include <Common/Base/Math/Functions/Sse/hkSseMathFuncs.inl>
#	elif defined(HK_PLATFORM_XBOX360)
#		include <Common/Base/Math/Functions/Xbox360/hkXbox360MathFuncs.inl>
#	elif defined(HK_ARCH_PS3) || defined(HK_ARCH_PS3SPU)
#		define HK_PLATFORM_SUPPORTS_MANY_SIMD_REGISTERS
#		include <Common/Base/Math/Functions/Ps3/hkPs3MathFuncs.inl>
#	elif defined(HK_COMPILER_HAS_INTRINSICS_NEON)
#		define HK_PLATFORM_SUPPORTS_MANY_SIMD_REGISTERS
#		include <Common/Base/Math/Functions/Neon/hkNeonMathFuncs.inl>
#	else // simd
#		error Unknown platform for SIMD
#	endif
#else // not simd
#		include <Common/Base/Math/Functions/Fpu/hkFpuMathFuncs.inl>
#endif

// platform independent
#include <Common/Base/Math/Functions/hkMathFuncs.inl>



// Union that allows us to initialize the int values of an int vector
union hkQuadIntUnion
{
	hkInt32 u[4];
	hkQuadUint q;
};

// hkIntVector vector constants
extern HK_ALIGN_REAL(const hkUint32 g_intVectorConstants[HK_QUADINT_END][4]);


class hkVector4;
class hkVector4Comparison;
class hkSimdReal;
class hkQuaternion;
class hkMatrix3;
class hkRotation;
class hkTransform;
class hkQTransform;
class hkQsTransform;

	// declarations
#include <Common/Base/Math/Vector/hkVector4.h>
#include <Common/Base/Math/Vector/hkVector4Comparison.h>
#include <Common/Base/Math/Vector/hkSimdReal.h>
#include <Common/Base/Math/Quaternion/hkQuaternion.h>
#include <Common/Base/Math/Matrix/hkMatrix3.h>
#include <Common/Base/Math/Matrix/hkRotation.h>
#include <Common/Base/Math/Matrix/hkTransform.h>
#include <Common/Base/Math/QsTransform/hkQsTransform.h>
#include <Common/Base/Math/QTransform/hkQTransform.h>
#include <Common/Base/Math/Matrix/hkMatrix4.h>

// platform specific implementations
#if defined(HK_PLATFORM_CAFE)
#	include <Common/Base/Math/Vector/Cafe/hkCafeVector4.inl>
#	include <Common/Base/Math/Vector/Fpu/hkFpuVector4Comparison.inl>
#	include <Common/Base/Math/Vector/Fpu/hkFpuSimdReal.inl>
#elif HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED
#	if defined(HK_COMPILER_HAS_INTRINSICS_IA32)
#		if defined(HK_REAL_IS_DOUBLE)
#			if HK_SSE_VERSION >= 0x50
#				include <Common/Base/Math/Vector/Sse/hkSseVector4_D_AVX.inl>
#				include <Common/Base/Math/Vector/Sse/hkSseVector4Comparison_D_AVX.inl>
#			else
#				include <Common/Base/Math/Vector/Sse/hkSseVector4_D.inl>
#				include <Common/Base/Math/Vector/Sse/hkSseVector4Comparison_D.inl>
#			endif
#			include <Common/Base/Math/Vector/Sse/hkSseSimdReal_D.inl>
#		else
#			include <Common/Base/Math/Vector/Sse/hkSseVector4.inl>
#			include <Common/Base/Math/Vector/Sse/hkSseVector4Comparison.inl>
#			include <Common/Base/Math/Vector/Sse/hkSseSimdReal.inl>
#		endif
#	elif defined(HK_PLATFORM_XBOX360)
#		include <Common/Base/Math/Vector/Xbox360/hkXbox360Vector4.inl>
#		include <Common/Base/Math/Vector/Xbox360/hkXbox360Vector4Comparison.inl>
#		include <Common/Base/Math/Vector/Xbox360/hkXbox360SimdReal.inl>
#	elif defined(HK_PLATFORM_PS3_PPU) || defined(HK_PLATFORM_PS3_SPU)
#		include <Common/Base/Math/Vector/Ps3/hkPs3Vector4.inl>
#		include <Common/Base/Math/Vector/Ps3/hkPs3Vector4Comparison.inl>
#		include <Common/Base/Math/Vector/Ps3/hkPs3SimdReal.inl>
#	elif defined(HK_COMPILER_HAS_INTRINSICS_NEON)
#		include <Common/Base/Math/Vector/Neon/hkNeonVector4.inl>
#		include <Common/Base/Math/Vector/Neon/hkNeonVector4Comparison.inl>
#		include <Common/Base/Math/Vector/Neon/hkNeonSimdReal.inl>
#	elif defined(HK_PLATFORM_LRB)
#		include <Common/Base/Math/Vector/Lrb/hkLrbVector4.inl>
#	else
#		error Dont know how to do simd on this platform
#	endif
#else // no SIMD
#		include <Common/Base/Math/Vector/Fpu/hkFpuVector4.inl>
#		include <Common/Base/Math/Vector/Fpu/hkFpuVector4Comparison.inl>
#		include <Common/Base/Math/Vector/Fpu/hkFpuSimdReal.inl>
#endif // HK_CONFIG_SIMD

	// common implementations for all platforms
#include <Common/Base/Math/Vector/hkVector4.inl>
#include <Common/Base/Math/Vector/hkVector4Comparison.inl>
#include <Common/Base/Math/Vector/hkSimdReal.inl>
#include <Common/Base/Math/Matrix/hkMatrix3.inl>
#include <Common/Base/Math/Quaternion/hkQuaternion.inl>
#include <Common/Base/Math/Matrix/hkTransform.inl>
#include <Common/Base/Math/Matrix/hkMatrix4.inl>
#include <Common/Base/Math/QsTransform/hkQsTransform.inl>
#include <Common/Base/Math/QTransform/hkQTransform.inl>

// long vector types
#include <Common/Base/Math/Types/Mx/hkMxMathTypes.inl>

#endif // HK_MATH_MATH_H

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
