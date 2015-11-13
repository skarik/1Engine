/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
//HK_REFLECTION_PARSER_EXCLUDE_FILE

#ifndef HK_MXSINGLE_H
#define HK_MXSINGLE_H

#define hkMxSingleParameter const hkMxSingle<M>&

/// Long vector for vectorized computing (Mx1 vectors).
///
/// This represents M 4-component vectors which are all the same. 
/// Note that although the subvectors are all the same, this vector still has a distinct length
/// as you cannot rely in general on how the storage is optimized for the replicated subvectors.
/// In this documentation we use abcd and following letters to
/// refer to one of the 4-component subvectors.
/// Example M=4: layout of the long vector: abcd abcd abcd abcd
///
/// The purpose of this vector is to implement efficient loop-unrolled algorithms without branching
/// to efficiently use the SIMD processing hardware capabilities.
///
/// \sa hkMxVector hkMxReal
template <int M>
class hkMxSingle
{
public:

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_MATH, hkMxSingle<M>);

	/// The number of subvectors stored in this vector.
	///
	/// When coding, this value can be retrieved using the class definition as well as from variable instances.
	/// Example: incrementing a loop counter: i += hkMxSingle<M>::mxLength;.
	/// Example: declaring an array appropriate with same length as a vector: int array[svec_var.mxLength];
	enum { mxLength = M };

#ifndef HK_DISABLE_MATH_CONSTRUCTORS
	/// Default constructor. Does not initialize.
	HK_FORCE_INLINE hkMxSingle() {}

	/// Set self to broadcast \a v: aaaa aaaa aaaa aaaa
	HK_FORCE_INLINE explicit hkMxSingle(hkSimdRealParameter v);
	/// Set self to replicate \a v: abcd abcd abcd abcd
 	HK_FORCE_INLINE explicit hkMxSingle(hkVector4Parameter v);
#endif

	/// Copies all components from \a v. ( self = v )
	HK_FORCE_INLINE void operator= ( hkMxSingleParameter v );

	/// Get the replicated subvector: return abcd
	HK_FORCE_INLINE const hkVector4& getVector() const;
	HK_FORCE_INLINE void setVector(hkVector4Parameter r);

	/// Set to zero.
	HK_FORCE_INLINE void setZero();

	template<int C> HK_FORCE_INLINE void setConstant();


	hkMxSingleStorage<M> m_single; ///< The replicated subvector.
};

#if defined(HK_COMPILER_HAS_INTRINSICS_IA32)
	#if (HK_SSE_VERSION >= 0x50) && !defined(HK_REAL_IS_DOUBLE)
	#include <Common/Base/Math/Vector/Mx/hkMxSingle_AVX.inl>
	#else
	#include <Common/Base/Math/Vector/Mx/hkMxSingle.inl>
	#endif
#else
	#include <Common/Base/Math/Vector/Mx/hkMxSingle.inl>
#endif

// convenient shortcut
typedef hkMxSingle<4> hk4xSingle;

#endif // HK_MXSINGLE_H

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
