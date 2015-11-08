/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
//HK_REFLECTION_PARSER_EXCLUDE_FILE

#ifndef HK_MXREAL_H
#define HK_MXREAL_H

namespace hkCompileError
{
	template <bool b> struct MXR_NOT_IMPLEMENTED_FOR_THIS_VECTOR_LENGTH;
	template <> struct MXR_NOT_IMPLEMENTED_FOR_THIS_VECTOR_LENGTH<true>{ };
}
#define HK_MXREAL_MX_NOT_IMPLEMENTED HK_COMPILE_TIME_ASSERT2(M==0, MXR_NOT_IMPLEMENTED_FOR_THIS_VECTOR_LENGTH)

#define hkMxRealParameter   const hkMxReal<M>&

template <int M> class hkMxVector;
#include <Common/Base/Math/Vector/Mx/hkMxVector.h>
#include <Common/Base/Math/Vector/Mx/hkMxSingle.h>
#include <Common/Base/Math/Vector/Mx/hkMxMask.h>

/// Long vector for vectorized computing (M scalars).
///
/// This represents M scalars stored SIMD-friendly. In this documentation we use abcd and following letters to
/// refer to one of the 4-component subscalars.
/// Example M=4: layout of the long vector: aaaa eeee iiii mmmm
///
/// The purpose of this vector is to implement efficient loop-unrolled algorithms without branching
/// to efficiently use the SIMD processing hardware capabilities.
///
/// \sa hkMxVector hkMxSingle
template <int M>
class hkMxReal
{
public:

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_MATH, hkMxReal<M>);

	/// The number of SIMD-friendly scalars stored in this vector.
	///
	/// When coding, this value can be retrieved using the class definition as well as from variable instances.
	/// Example: incrementing a loop counter: i += hkMxReal<M>::mxLength;.
	/// Example: declaring an array appropriate with same length as a vector: int array[realvec_var.mxLength];
	enum { mxLength = M };

#ifndef HK_DISABLE_MATH_CONSTRUCTORS
	/// Default constructor. Does not initialize.
	HK_FORCE_INLINE hkMxReal() {}

	/// Set self to broadcast value \a v
	HK_FORCE_INLINE explicit hkMxReal(hkReal& v); 

	/// Set self to broadcast value \a v
	HK_FORCE_INLINE explicit hkMxReal(hkSimdRealParameter v); 

	/// Set self to broadcast \a v.xyzw to x=aaaa y=eeee z=iiii w=mmmm
	/// \warning [M:1-4 only] 
	HK_FORCE_INLINE explicit hkMxReal(hkVector4Parameter v); 
#endif

	/// Copies all components from \a v. ( self = v )
	HK_FORCE_INLINE void operator= ( hkMxRealParameter v );

	/// Set self to broadcast \a r: aaaa eeee iiii mmmm
	HK_FORCE_INLINE void setBroadcast( hkVector4Parameter r );

	/// Set self to broadcast \a r: aaaa aaaa aaaa aaaa
	HK_FORCE_INLINE void setBroadcast( hkSimdRealParameter r );

	/// Read scalars contiguous from memory as aeim and convert to SIMD-friendly storage
	HK_FORCE_INLINE void load(const hkReal* r);
	HK_FORCE_INLINE void loadNotAligned(const hkReal* r);
	HK_FORCE_INLINE void loadNotCached(const hkReal* r);
	/// Convert from SIMD-friendly storage and store contiguous to memory \a rOut = aeim
	HK_FORCE_INLINE void store(hkReal* rOut) const;
	HK_FORCE_INLINE void storeNotAligned(hkReal* rOut) const;
	HK_FORCE_INLINE void storeNotCached(hkReal* rOut) const;

	/// Set to a constant vector.
	template<int vectorConstant> HK_FORCE_INLINE void setConstant();

	/// Read scalars non-contiguous from memory using addresses \a base + (m * byteAddressIncrement) and convert to SIMD-friendly storage
	template <hkUint32 byteAddressIncrement> HK_FORCE_INLINE void gather(const hkReal* base);
	/// Read scalars non-linear indexed from memory using addresses \a base + ( \a indices[m] * byteAddressIncrement) and convert to SIMD-friendly storage
	template <hkUint32 byteAddressIncrement> HK_FORCE_INLINE void gather(const hkReal* base, const hkUint16* indices);
	/// Read scalars non-linear indexed from memory using addresses \a base + ( \a indices[m] * byteAddressIncrement) and convert to SIMD-friendly storage
	template <hkUint32 byteAddressIncrement> HK_FORCE_INLINE void gather(const hkReal* base, const hkInt32* indices);
	/// Read scalars non-linear from memory using addresses \a base[m] + byteAddressOffset and convert to SIMD-friendly storage
	template <hkUint32 byteAddressOffset>    HK_FORCE_INLINE void gatherWithOffset(const void* base[M]);

	/// Convert from SIMD-friendly storage and write scalars non-contiguous to memory using addresses \a base + (m * byteAddressIncrement)
	template <hkUint32 byteAddressIncrement> HK_FORCE_INLINE void scatter(hkReal* base) const;
	/// Convert from SIMD-friendly storage and write scalars non-linear indexed to memory using addresses \a base + ( \a indices[m] * byteAddressIncrement)
	template <hkUint32 byteAddressIncrement> HK_FORCE_INLINE void scatter(hkReal* base, const hkUint16* indices) const;
	/// Convert from SIMD-friendly storage and write scalars non-linear indexed to memory using addresses \a base + ( \a indices[m] * byteAddressIncrement)
	template <hkUint32 byteAddressIncrement> HK_FORCE_INLINE void scatter(hkReal* base, const hkInt32* indices) const;
	/// Convert from SIMD-friendly storage and write scalars non-linear to memory using addresses \a base[m] + byteAddressOffset
	template <hkUint32 byteAddressOffset>    HK_FORCE_INLINE void scatterWithOffset(void* base[M]) const;


	// comparisons
	HK_FORCE_INLINE void less(hkMxVectorParameter v, hkMxMask<M>& mask) const;
	HK_FORCE_INLINE void less(hkMxSingleParameter v, hkMxMask<M>& mask) const;
	HK_FORCE_INLINE void less(hkMxRealParameter v, hkMxMask<M>& mask) const;
	HK_FORCE_INLINE void lessEqual(hkMxVectorParameter v, hkMxMask<M>& mask) const;
	HK_FORCE_INLINE void lessEqual(hkMxSingleParameter v, hkMxMask<M>& mask) const;
	HK_FORCE_INLINE void lessEqual(hkMxRealParameter v, hkMxMask<M>& mask) const;
	HK_FORCE_INLINE void greater(hkMxVectorParameter v, hkMxMask<M>& mask) const;
	HK_FORCE_INLINE void greater(hkMxSingleParameter v, hkMxMask<M>& mask) const;
	HK_FORCE_INLINE void greater(hkMxRealParameter v, hkMxMask<M>& mask) const;
	HK_FORCE_INLINE void greaterEqual(hkMxVectorParameter v, hkMxMask<M>& mask) const;
	HK_FORCE_INLINE void greaterEqual(hkMxSingleParameter v, hkMxMask<M>& mask) const;
	HK_FORCE_INLINE void greaterEqual(hkMxRealParameter v, hkMxMask<M>& mask) const;

	HK_FORCE_INLINE void setSelect(hkMxMaskParameter mask, hkMxRealParameter trueValue, hkMxRealParameter falseValue );
	HK_FORCE_INLINE void setSelect(hkMxMaskParameter mask, hkMxSingleParameter trueValue, hkMxRealParameter falseValue ); // bad interface !
	HK_FORCE_INLINE void setSelect(hkMxMaskParameter mask, hkMxRealParameter trueValue, hkMxSingleParameter falseValue ); // bad interface !

	// no operands
	HK_FORCE_INLINE void sqrt(); // 23 bit, sqrt set 0
	template <hkMathAccuracyMode A, hkMathNegSqrtMode S> HK_FORCE_INLINE void sqrt();

	
	// one operand
	HK_FORCE_INLINE void add(hkMxRealParameter r);
	HK_FORCE_INLINE void sub(hkMxRealParameter r);
	HK_FORCE_INLINE void mul(hkMxRealParameter r);
	HK_FORCE_INLINE void div(hkMxRealParameter r); // 23 bit, div 0 ignore
	template <hkMathAccuracyMode A, hkMathDivByZeroMode D> HK_FORCE_INLINE void div(hkMxRealParameter r);
	HK_FORCE_INLINE void setNeg(hkMxRealParameter r);		

	HK_FORCE_INLINE void setReciprocal(hkMxRealParameter r); // 23 bit, div 0 ignore
	template <hkMathAccuracyMode A, hkMathDivByZeroMode D> HK_FORCE_INLINE void setReciprocal(hkMxRealParameter r);

	// two operands
	HK_FORCE_INLINE void setAdd(hkMxRealParameter v0, hkMxRealParameter v1);
	HK_FORCE_INLINE void setSub(hkMxRealParameter v0, hkMxRealParameter v1);
	HK_FORCE_INLINE void setMul(hkMxRealParameter v0, hkMxRealParameter v1);
	HK_FORCE_INLINE void setDiv(hkMxRealParameter v0, hkMxRealParameter v1); // 23 bit, div 0 ignore
	template <hkMathAccuracyMode A, hkMathDivByZeroMode D> HK_FORCE_INLINE void setDiv(hkMxRealParameter v0, hkMxRealParameter v1);
	HK_FORCE_INLINE void setMax(hkMxRealParameter v0, hkMxRealParameter v1);
	HK_FORCE_INLINE void setMin(hkMxRealParameter v0, hkMxRealParameter v1);

	// mixed operands
	template <int N> HK_FORCE_INLINE void setDot(hkMxVectorParameter v0, hkMxVectorParameter v1);
	template <int N> HK_FORCE_INLINE void setDot(hkMxSingleParameter s, hkMxVectorParameter v);
	HK_FORCE_INLINE void setDot4xyz1(hkMxVectorParameter v0, hkMxVectorParameter v1);
	HK_FORCE_INLINE void setDot4xyz1(hkMxSingleParameter s, hkMxVectorParameter v);

	/// Return the I-th scalar by broadcasting it to \a vOut = [i=0]aaaa [i=1]eeee [i=2]iiii [i=3]mmmm
	template <int I> HK_FORCE_INLINE void getAsBroadcast(hkVector4& vOut) const;
	/// Return the I-th scalar broadcasted as [i=0]aaaa [i=1]eeee [i=2]iiii [i=3]mmmm
	template <int I> HK_FORCE_INLINE const hkVector4 getAsBroadcast() const;

	/// Return the I-th scalar in a SIMD-friendly way.
	template <int I> HK_FORCE_INLINE const hkSimdReal getReal() const;
	/// Set the I-th subscalar to \a r.
	template <int I> HK_FORCE_INLINE void setReal(hkSimdRealParameter r);

	/// Special store method to write to \a vOut = aeim.
	/// Excess components of \a vOut are undefined.
	/// \warning [M:1-4 only] 
	HK_FORCE_INLINE void storePacked(hkVector4& vOut) const;



	hkMxRealStorage<M> m_real; ///< The SIMD-friendly scalars.
};

#if defined(HK_COMPILER_HAS_INTRINSICS_IA32)
	#if (HK_SSE_VERSION >= 0x50) && !defined(HK_REAL_IS_DOUBLE)
	#include <Common/Base/Math/Vector/Mx/hkMxReal_AVX.inl>
	#else
	#include <Common/Base/Math/Vector/Mx/hkMxReal.inl>
	#endif
#else
	#include <Common/Base/Math/Vector/Mx/hkMxReal.inl>
#endif

// convenient shortcut
typedef hkMxReal<4>   hk4xReal;

#endif // HK_MXREAL_H

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
