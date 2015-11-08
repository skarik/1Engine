/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HK_MATH_VECTOR3_UTIL_H
#define HK_MATH_VECTOR3_UTIL_H

#include <Common/Base/Types/Geometry/Aabb/hkAabb.h>
#include <Common/Base/Math/Vector/hkIntVector.h>

class hkStringBuf;

union hkIntUnion64
{
	hkInt64 i64;
	hkUint64 u64;
	hkInt32 i32[2];
	hkUint32 u32[2];
	hkInt16 i16[4];
	hkUint16 u16[4];
	hkInt8 i8[8];
	hkUint8 u8[8];
};

namespace hkVector4Util
{
		/// Sets the calling vector to be the normal to the 2 vectors (b-a) and (c-a).
		///
		/// NOTE: The calculated result is not explicitly normalized.
		/// This function is particularly fast on PlayStation(R)2.
		/// Result = (b-a) cross (c-a)
	HK_FORCE_INLINE void HK_CALL setNormalOfTriangle(hkVector4& result, hkVector4Parameter a, hkVector4Parameter b, hkVector4Parameter c);

	HK_FORCE_INLINE void HK_CALL atan2Approximation(hkVector4Parameter x, hkVector4Parameter y, hkVector4& result);
	HK_FORCE_INLINE void HK_CALL logApproximation(hkVector4Parameter v, hkVector4& result);

	HK_FORCE_INLINE void HK_CALL convertQuaternionToRotation( hkQuaternionParameter qi, hkRotation& rotationOut );
	void HK_CALL convertQuaternionToRotationNoInline( hkQuaternionParameter qi, hkRotation& rotationOut );

		/// convert 4 quaternions to 4 rotations, much faster on Xbox 360/PlayStation(R)3, no speedup on PC
	void HK_CALL convert4QuaternionsToRotations( const hkQuaternion* quats, hkRotation* r0Out, hkRotation* r1Out, hkRotation* r2Out, hkRotation* r3Out );

		/// 
	HK_FORCE_INLINE void HK_CALL convertFromHalf( const hkHalf& a, const hkHalf& b, const hkHalf& c, const hkHalf& d, hkVector4& out );

		/// 
	HK_FORCE_INLINE void HK_CALL convertToUint16( hkVector4Parameter in, hkVector4Parameter offset, hkVector4Parameter scale, hkIntUnion64& out );

		/// 
	HK_FORCE_INLINE void HK_CALL convertToUint16WithClip( hkVector4Parameter in, hkVector4Parameter offset, hkVector4Parameter scale, hkVector4Parameter min, hkVector4Parameter max, hkIntUnion64& out);

		/// calculates a value x so that convertToUint16WithClip( out, in + x/scale, ... ) == out = int(floor( (in+offset)*scale
	hkReal HK_CALL getFloatToInt16FloorCorrection();

		/// 
	HK_FORCE_INLINE void HK_CALL convertToUint32( hkVector4Parameter in, hkVector4Parameter offset, hkVector4Parameter scale, hkUint32* out );

		///
	HK_FORCE_INLINE void HK_CALL convertToUint32WithClip( hkVector4Parameter in, hkVector4Parameter offset, hkVector4Parameter scale, hkVector4Parameter min, hkVector4Parameter max, hkUint32* out);

		/// calculates a value x so that convertToUint32WithClip( out, in + x/scale, ... ) == out = int(floor( (in+offset)*scale
	hkReal HK_CALL getFloatToInt32FloorCorrection();

		/// Converts a floating-point AABB to an integer AABB
	HK_FORCE_INLINE void HK_CALL convertAabbToUint32( const hkAabb& aabb, hkVector4Parameter offsetLow, hkVector4Parameter offsetHigh, hkVector4Parameter scale, hkAabbUint32& aabbOut );

		/// Converts an integer AABB to a floating-point AABB
	HK_FORCE_INLINE void HK_CALL convertAabbFromUint32( const hkAabbUint32& aabbIn, hkVector4Parameter offsetLow, hkVector4Parameter scale, hkAabb& aabbOut);

		/// Store the difference between a big expandedAabbInt and a smaller, enclosed unexpandedAabbInt_InOut in m_expansionXXX components of unexpandedAabbInt_InOut.
	HK_FORCE_INLINE void HK_CALL compressExpandedAabbUint32(const hkAabbUint32& expandedAabbInt, hkAabbUint32& unexpandedAabbInt_InOut);

		/// Expand a compressed unexpandedAabbInt to an expandedAabbOut using the information stored in the m_expansionXXX components.
	HK_FORCE_INLINE void HK_CALL uncompressExpandedAabbUint32(const hkAabbUint32& unexpandedAabbInt, hkAabbUint32& expandedAabbOut);

		/// Finds a vector that is perpendicular to a line segment.
		///
		/// Achieved by constructing a vector from the segment vector with the following properties
		/// (segment vector is any vector parallel to the line segment):
		/// 1) Component with the smallest index is set to 0.
		/// 2) The remaining two component are copied into the new vector but are swapped in position.
		/// 3) One of the copied components is multiplied by -1.0 (has its sign flipped!).
		///
		/// leaving (for example):
		/// segmentVector = (x, y, z), with let's say y as the smallest component.
		/// segmentNormal = (-z, 0 , x) or (z, 0, -x), either will do.
		///
		/// This will in fact be orthogonal as (in the example) the dot product will be zero.
		/// I.e., x*-z + y*0 + z*x = 0
		///
	HK_FORCE_INLINE void HK_CALL calculatePerpendicularVector(hkVector4Parameter vectorIn, hkVector4& biVectorOut);

		/// Transforms an array of points.
	HK_FORCE_INLINE void HK_CALL transformPoints( const hkTransform& t, const hkVector4* vectorsIn, int numVectors, hkVector4* vectorsOut );
	HK_FORCE_INLINE void HK_CALL transformPoints( const hkQTransform& t, const hkVector4* vectorsIn, int numVectors, hkVector4* vectorsOut );
	HK_FORCE_INLINE void HK_CALL transformPoints( const hkQsTransform& t, const hkVector4* vectorsIn, int numVectors, hkVector4* vectorsOut );

		/// Transforms an array of points including the .w component
	HK_FORCE_INLINE void HK_CALL mul4xyz1Points( const hkTransform& t, const hkVector4* vectorsIn, int numVectors, hkVector4* vectorsOut );

		/// Transforms an array of spheres (keeps the .w component as the radius)
	HK_FORCE_INLINE void HK_CALL transformSpheres( const hkTransform& t, const hkVector4* vectorsIn, int numVectors, hkVector4* vectorsOut );

	/// Transforms an array of spheres (keeps the .w component as the radius)
	HK_FORCE_INLINE void HK_CALL transformSpheres( const hkQsTransform& t, const hkVector4* vectorsIn, int numVectors, hkVector4* vectorsOut );

		/// Transforms an array of plane equations
	HK_FORCE_INLINE void HK_CALL transformPlaneEquations( const hkTransform& t, const hkVector4* planesIn, int numPlanes, hkVector4* planesOuts );




		/// Rotate an array of points.
	HK_FORCE_INLINE void HK_CALL rotatePoints( const hkMatrix3& t, const hkVector4* vectorsIn, int numVectors, hkVector4* vectorsOut );

		/// Invert rotate an array of points.
	HK_FORCE_INLINE void HK_CALL rotateInversePoints( const hkRotation& t, const hkVector4* vectorsIn, int numVectors, hkVector4* vectorsOut );

		/// set aTcOut = aTb * bTc
	HK_FORCE_INLINE void HK_CALL setMul( const hkMatrix3& aTb, const hkMatrix3& bTc, hkMatrix3& aTcOut );

		/// set aTcOut = bTa^1 * bTc
	HK_FORCE_INLINE void HK_CALL setInverseMul( const hkRotation& bTa, const hkMatrix3& bTc, hkMatrix3& aTcOut );


		/// Sets this vector components: this(0) = a0.dot<3>(b0), this(1) = a1.dot<3>(b1) ...
	HK_FORCE_INLINE void HK_CALL dot3_3vs3( hkVector4Parameter a0, hkVector4Parameter b0, hkVector4Parameter a1, hkVector4Parameter b1, hkVector4Parameter a2, hkVector4Parameter b2, hkVector4& dotsOut);

		/// Sets this vector components: this(0) = a0.dot<3>(b0) ... this(3) = a3.dot<3>(b3)
	HK_FORCE_INLINE void HK_CALL dot3_4vs4( hkVector4Parameter a0, hkVector4Parameter b0, hkVector4Parameter a1, hkVector4Parameter b1, hkVector4Parameter a2, hkVector4Parameter b2, hkVector4Parameter a3, hkVector4Parameter b3, hkVector4& dotsOut);

		/// Sets this vector components: this(0) = a0.dot<4>(b0) ... this(3) = a3.dot<4>(b3)
	HK_FORCE_INLINE void HK_CALL dot4_4vs4( hkVector4Parameter a0, hkVector4Parameter b0, hkVector4Parameter a1, hkVector4Parameter b1, hkVector4Parameter a2, hkVector4Parameter b2, hkVector4Parameter a3, hkVector4Parameter b3, hkVector4& dotsOut);

		/// Sets this vector components: this(i) = vector.dot<3>(AI) for i=0..3
	HK_FORCE_INLINE void HK_CALL dot3_1vs4( hkVector4Parameter vectorIn, hkVector4Parameter a0, hkVector4Parameter a1, hkVector4Parameter a2, hkVector4Parameter a3, hkVector4& dotsOut);

		/// Sets this vector components: this(i) = vector.dot<4>(AI) for i=0..3
	HK_FORCE_INLINE void HK_CALL dot4_1vs4( hkVector4Parameter vectorIn, hkVector4Parameter a0, hkVector4Parameter a1, hkVector4Parameter a2, hkVector4Parameter a3, hkVector4& dotsOut);

		/// Sets this vector components: this(i+j) = AI.dot<3>(bj)
	HK_FORCE_INLINE void HK_CALL dot3_2vs2( hkVector4Parameter a0, hkVector4Parameter a2, hkVector4Parameter b0, hkVector4Parameter b1, hkVector4& dotsOut);

		/// Computes an = (a x n), bn = (b x n), cn = (c x n)
	HK_FORCE_INLINE void HK_CALL cross_3vs1(hkVector4Parameter a, hkVector4Parameter b, hkVector4Parameter c, hkVector4Parameter n,
											hkVector4& an, hkVector4& bn, hkVector4& cn);

		/// Computes an = (a x n), bn = (b x n), cn = (c x n), dn = (d x n)
	HK_FORCE_INLINE void HK_CALL cross_4vs1(hkVector4Parameter a, hkVector4Parameter b, hkVector4Parameter c, hkVector4Parameter d, hkVector4Parameter n,
											hkVector4& an, hkVector4& bn, hkVector4& cn, hkVector4& dn);

		/// Computes the cross products: (vA, vB), (vB, vC), (vC, vA)
	HK_FORCE_INLINE void computeCyclicCrossProducts(	hkVector4Parameter vA, hkVector4Parameter vB, hkVector4Parameter vC,
														hkVector4& vAB, hkVector4& vBC, hkVector4& vCA);


		/// build an orthonormal matrix, where the first column matches the parameter dir.
		/// Note: Dir must be normalized
	HK_FORCE_INLINE void HK_CALL buildOrthonormal( hkVector4Parameter dir, hkMatrix3& out );

		/// build an orthonormal matrix, where the first column matches the parameter dir and the second
		/// column matches dir2 as close as possible.
		/// Note: Dir must be normalized
	HK_FORCE_INLINE void HK_CALL buildOrthonormal( hkVector4Parameter dir, hkVector4Parameter dir2, hkMatrix3& out );

		/// Resets the fpu after using MMX instructions on x86. No-op for other architectures.
	HK_FORCE_INLINE void HK_CALL exitMmx();

		/// Returns the squared distance from p to the line segment ab
	HK_FORCE_INLINE const hkSimdReal HK_CALL distToLineSquared( hkVector4Parameter a, hkVector4Parameter b, hkVector4Parameter p );

		/// Set the of 'out' to the maximum of 'a','b','c' and 'd'
	HK_FORCE_INLINE void HK_CALL setMax44(hkVector4Parameter a, hkVector4Parameter b, hkVector4Parameter c, hkVector4Parameter d, hkVector4& out);

		/// Set the of 'out' to the minimum of 'a','b','c' and 'd'
	HK_FORCE_INLINE void HK_CALL setMin44(hkVector4Parameter a, hkVector4Parameter b, hkVector4Parameter c, hkVector4Parameter d, hkVector4& out);

		/// Helper function for use with hkAlgorithm::findMinimumIndex, etc.
		/// Sample usage (finding the index of the element of "vectors" that is closest to "point"):
		/// \code int minIndex = hkAlgorithm::findMinimumIndex( vectors.begin(), vectors.getSize(), hkVector4Util::DistanceToPoint( point ) ); \endcode
	struct DistanceToPoint
	{
		HK_FORCE_INLINE DistanceToPoint( hkVector4Parameter p ) : m_p(p) { }
		HK_FORCE_INLINE hkReal operator() (hkVector4Parameter p) const { return m_p.distanceToSquared(p).getReal(); }

		hkVector4 m_p;
	};

		//
		//	conversions
		//

		/// convert an hkVector4 to a string representing the x,y,z components in that order.
	const char* HK_CALL toString3(hkVector4Parameter x, hkStringBuf& s, char separator=',');

		/// convert an hkVector4 to a string representing the x,y,z,w components in that order.
	const char* HK_CALL toString4(hkVector4Parameter x, hkStringBuf& s, char separator=',');

		//
		//	compression
		//

		// packs a normalized quaternion into a single 4*8 bit integer. The error is roughly 0.01f per component
	hkUint32 HK_CALL packQuaternionIntoInt32( hkVector4Parameter qin);

		// unpack an 32 bit integer into quaternion. Note: The resulting quaternion is not normalized ( |q.length<4>()-1.0f| < 0.04f )
	HK_FORCE_INLINE void HK_CALL unPackInt32IntoQuaternion( hkUint32 ivalue, hkVector4& qout );

	HK_FORCE_INLINE void memClear16( void* dest, int numQuads); 

	extern hkUint32 m_reservedRegisters;

	enum { AABB_UINT32_MAX_FVALUE = 0x7ffe0000 };
	extern hkQuadReal hkAabbUint32MaxVal;
}


#if HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED
#	if defined(HK_COMPILER_HAS_INTRINSICS_IA32)
#		if defined(HK_REAL_IS_DOUBLE)
#			if HK_SSE_VERSION >= 0x50
#				include <Common/Base/Math/Vector/Sse/hkSseVector4Util_D_AVX.inl>
#			else
#				include <Common/Base/Math/Vector/Sse/hkSseVector4Util_D.inl>
#			endif
#		else
#			include <Common/Base/Math/Vector/Sse/hkSseVector4Util.inl>
#		endif
#	elif defined(HK_PLATFORM_PS3_SPU) || defined(HK_PLATFORM_PS3_PPU)
#		include <Common/Base/Math/Vector/Ps3/hkPs3Vector4Util.inl>
#	elif defined(HK_PLATFORM_XBOX360)
#		include <Common/Base/Math/Vector/Xbox360/hkXbox360Vector4Util.inl>
#	endif
#endif // HK_CONFIG_SIMD

#include <Common/Base/Math/Vector/hkVector4Util.inl>

#endif // HK_MATH_VECTOR3_UTIL_H

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
