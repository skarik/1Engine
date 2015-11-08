/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
//HK_REFLECTION_PARSER_EXCLUDE_FILE

#ifndef HKCD_RAY_TRIANGLE_INTERSECTION_H
#define HKCD_RAY_TRIANGLE_INTERSECTION_H

#include <Common/Base/Math/Vector/hkIntVector.h>
#include <Geometry/Internal/Types/hkcdRay.h>


	
	/// Performs a line-triangle intersection test.
	/// Returns true if the given ray r=pos+t*dir intersects the triangle.
	///	\param ray (Input) The ray.
	///	\param a First vertex.
	///	\param b Second vertex.
	///	\param c Third vertex.
	///	\param tolerance Tolerance below which a plane and ray can be considered parallel
	///	\param distanceOut (Output) Signed distance along the ray direction (t) to the intersection point if hit.
hkBool32 HK_CALL hkcdLineTriangleIntersect(
	const hkcdRay& ray,
	hkVector4Parameter vA, hkVector4Parameter vB, hkVector4Parameter vC,
	hkSimdRealParameter tolerance, 
	hkSimdReal& fractionOut);

	/// Returns true and the fraction if the ray intersect the triangle between 0 and fractionInOut.
HK_FORCE_INLINE hkBool32 HK_CALL hkcdSegmentTriangleIntersect(
	const hkcdRay& ray,
	hkVector4Parameter a, hkVector4Parameter b, hkVector4Parameter c,
	hkVector4& nonUnitNormalOut,
	hkSimdReal& fractionInOut);

	/// Intersects a line segment with a triangle. Returns 1 if the segment intersects the triangle,
	/// and 0 otherwise.
	/// \param vFrom		(Input) Start point of the line segment.
	/// \param vTo			(Input) End point of the line segment.
	/// \param vA			(Input) First triangle vertex.
	/// \param vB			(Input) Second triangle vertex.
	/// \param vC			(Input) Third triangle vertex.
	/// \param normalOut	(Output) Normal at the intersection point in case of successful intersection, undefined otherwise.
HK_FORCE_INLINE hkBool32 HK_CALL hkcdSegmentTriangleIntersect(
	const hkcdRay& ray,
	hkVector4Parameter vA, hkVector4Parameter vB, hkVector4Parameter vC,
	hkSimdRealParameter tolerance,
	hkVector4& normalOut,
	hkSimdReal& fractionInOut);

	/// Intersect a ray with a triangle.
HK_FORCE_INLINE hkBool32 hkcdRayTriangleIntersect(
	const hkcdRay& ray,
	const hkVector4& a, const hkVector4& b, const hkVector4& c,
	hkSimdReal* HK_RESTRICT fractionOut = HK_NULL, hkVector4* HK_RESTRICT normalOut = HK_NULL, hkBool twoSided = true);

	/// Intersects a ray bundle with the triangle.
HK_FORCE_INLINE hkVector4Comparison HK_CALL hkcdSegmentBundleTriangleIntersect(
	const hkcdRayBundle& rayBundle,
	hkVector4Parameter vA, hkVector4Parameter vB, hkVector4Parameter vC,
	hkSimdRealParameter tolerance, hkFourTransposedPoints& normalsOut, hkVector4& fractionsInOut);

#include <Geometry/Internal/Algorithms/RayCast/hkcdRayCastTriangle.inl>

#endif	//	HKCD_RAY_TRIANGLE_INTERSECTION_H

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
