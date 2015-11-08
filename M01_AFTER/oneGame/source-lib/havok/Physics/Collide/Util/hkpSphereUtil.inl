/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Physics/Collide/hkpCollide.h>
#include <Common/Base/hkBase.h>
#include <Physics/Collide/Util/hkpSphereUtil.h>
#include <Geometry/Internal/Algorithms/RayCast/hkcdRayCastSphere.h>


HK_FORCE_INLINE hkBool hkpRayCastSphere(hkVector4Parameter rayFrom, hkVector4Parameter rayTo, hkReal radius, hkpShapeRayCastOutput& results)
{
	// Adapt the parameters for calling hkcdRayCastSphere.
	hkcdRay ray; ray.setEndPoints(rayFrom, rayTo);
	hkVector4 spherePosRadius; spherePosRadius.set(0.0f, 0.0f, 0.0f, radius);
	hkSimdReal hitFraction(results.m_hitFraction);

	if(hkcdRayCastSphere(ray, spherePosRadius, &hitFraction, &results.m_normal))
	{
		hitFraction.store<1>( (hkReal*)&results.m_hitFraction );
		results.setKey(HK_INVALID_SHAPE_KEY);
		return true;
	}

	return false;
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
