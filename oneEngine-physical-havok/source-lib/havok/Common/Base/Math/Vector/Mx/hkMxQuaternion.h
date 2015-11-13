/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
//HK_REFLECTION_PARSER_EXCLUDE_FILE

#ifndef HK_MXQUATERNION_H
#define HK_MXQUATERNION_H

#define hkMxQuaternionParameter const hkMxQuaternion<M>&

#include <Common/Base/Math/Vector/Mx/hkMxVector.h>
#include <Common/Base/Math/Vector/hkVector4Util.h>

/// Long vector for vectorized computing (M quaternions).
///
/// This represents M quaternions. In this documentation we use abcd and following letters to
/// refer to one of the 4-component subquaternions.
/// Example M=4: layout of the long vector: abcd efgh ijkl mnop
///
/// The purpose of this vector is to implement efficient loop-unrolled algorithms without branching
/// to efficiently use the SIMD processing hardware capabilities.
///
/// \sa hkMxVector
template <int M>
class hkMxQuaternion : public hkMxVector<M>
{
public:

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_MATH, hkMxQuaternion<M>);

	/// Set each subquaternion to the inverse of the corresponding subquaternion in \a p.
 	HK_FORCE_INLINE void setInverse(hkMxQuaternionParameter p);

	/// Set each subquaternion to the multiply each subquaternion in \a p with the corresponding subquaternion in \a q.
	HK_FORCE_INLINE void setMulQ(hkMxQuaternionParameter p, hkMxQuaternionParameter q);

	/// Set each subquaternion to the multiply each subquaternion in \a p with the corresponding inverse subquaternion in \a q.
	HK_FORCE_INLINE void setMulInverseQ(hkMxQuaternionParameter p, hkMxQuaternionParameter q);

	/// Return the I-th subquaternion. [i=0]abcd [i=1]efgh [i=2]ijkl [i=3]mnop
	template <int I> HK_FORCE_INLINE const hkQuaternion& getQuaternion() const;

	/// Read subquaternions non-contiguous from memory using addresses \a base + (m * byteAddressIncrement)
	template <hkUint32 byteAddressIncrement> HK_FORCE_INLINE void gather(const hkQuaternion* base);

	/// Write subquaternions non-contiguous to memory using addresses \a base + (m * byteAddressIncrement)
	template <hkUint32 byteAddressIncrement> HK_FORCE_INLINE void scatter(hkQuaternion* base) const;

	/// Convert subquaternions to rotations and write to non-contiguous to memory using addresses \a base + (m * byteAddressIncrement)
	template <hkUint32 byteAddressIncrement> HK_FORCE_INLINE void convertAndScatter(hkRotation* base) const;
};

#if defined(HK_COMPILER_HAS_INTRINSICS_IA32)
	#if (HK_SSE_VERSION >= 0x50) && !defined(HK_REAL_IS_DOUBLE)
	#include <Common/Base/Math/Vector/Mx/hkMxQuaternion_AVX.inl>
	#else
	#include <Common/Base/Math/Vector/Mx/hkMxQuaternion.inl>
	#endif
#else
	#include <Common/Base/Math/Vector/Mx/hkMxQuaternion.inl>
#endif

// convenient shortcut
typedef hkMxQuaternion<4>	  hk4xQuaternion;

#endif // HK_MXQUATERNION_H

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
