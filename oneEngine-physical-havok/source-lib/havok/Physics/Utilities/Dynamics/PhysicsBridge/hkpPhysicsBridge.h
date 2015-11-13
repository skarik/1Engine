/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_PHYSICS_BRIDGE_H
#define HK_PHYSICS_BRIDGE_H

/// This is an abstract helper struct which allows for running 2 physics engines in parallel from the demos
class hkpPhysicsBridge: public hkReferencedObject
{
public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_UTILITIES);
	virtual void applyMouseSpring( hkUlong bodyId, hkVector4Parameter worldPosition, hkVector4Parameter localPosition){}
	virtual void stepDelta( class hkJobQueue* jobQueue, class hkJobThreadPool* jobThreadPool, hkReal timeStep ) = 0;
};
#endif // HK_PHYSICS_BRIDGE_H

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
