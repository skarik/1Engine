/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
//HK_REFLECTION_PARSER_EXCLUDE_FILE

//
HK_FORCE_INLINE	void hkcdRay::setDirection(hkVector4Parameter direction, hkSimdRealParameter fraction)
{
	hkVector4	maxCst  = hkVector4::getConstant<HK_QUADREAL_MAX>();
		
	m_direction.setXYZ_W(direction, fraction);
		
	hkVector4 invDir; invDir.setReciprocal(direction);
	m_invDirection.setSelect(direction.equalZero(), maxCst, invDir);
}

//
HK_FORCE_INLINE	void hkcdRay::setEndPoints(hkVector4Parameter start, hkVector4Parameter end, hkSimdRealParameter fraction)
{
	hkVector4	direction; direction.setSub(end, start);
	setOriginDirection(start, direction, fraction);
}

//
HK_FORCE_INLINE	void hkcdRay::setOriginDirection(hkVector4Parameter origin, hkVector4Parameter direction, hkSimdRealParameter fraction)
{
	setDirection(direction, fraction);
	m_origin	=	origin;
}

//
//	Computes the ray end-point

HK_FORCE_INLINE void hkcdRay::getEndPoint(hkVector4& vEndPoint) const
{
	vEndPoint.setAdd(m_origin, m_direction);
}

//
//	Returns the direction

inline void hkcdRayBundle::getDirection(hkFourTransposedPoints& directionOut) const
{
	directionOut.setSub(m_end, m_start);
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
