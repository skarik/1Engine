/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
//HK_REFLECTION_PARSER_EXCLUDE_FILE

#ifndef HKCD_RAY_CYLINDER_INTERSECTION_H
#define HKCD_RAY_CYLINDER_INTERSECTION_H

#include <Common/Base/Algorithm/Collide/LineSegment/hkLineSegmentUtil.h>
#include <Geometry/Internal/Types/hkcdRay.h>

	/// Raycast against a capped cylinder.
	/// See \ref RayCastDocumentation for common input and output parameter descriptions.
HK_FORCE_INLINE hkBool32 HK_CALL hkcdRayCastCylinder(
	const hkcdRay& ray,
	hkVector4Parameter vStart,
	hkVector4Parameter vEnd,
	hkSimdRealParameter radius,
	hkSimdReal* HK_RESTRICT fractionInOut,
	hkVector4* HK_RESTRICT normalOut);

#include <Geometry/Internal/Algorithms/RayCast/hkcdRayCastCylinder.inl>

#endif	//	HKCD_RAY_CYLINDER_INTERSECTION_H

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
