/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

HK_FORCE_INLINE hkpCapsuleShape* hkpCapsuleShape::createInPlace(hkUint8*& memPtr, const hkVector4& vertexA,const hkVector4& vertexB, hkReal radius)
{
	hkpCapsuleShape* shape;


	shape = new(memPtr) hkpCapsuleShape(vertexA, vertexB, radius);	


	HKCD_PATCH_SHAPE_VTABLE( shape );

	// Increment memory location
	const int memSize = HK_NEXT_MULTIPLE_OF(16, sizeof(hkpCapsuleShape));
	memPtr = hkAddByteOffset<hkUint8>(memPtr, memSize);

	// Return the shape
	return shape;
}

void hkpCapsuleShape::setVertex(int i, const hkVector4& position )
{
	HK_ASSERT2(0x4d78b768,  i>=0 && i < 2, "A capsule has only 2 vertices. getVertex() must be passed either 0 or 1. ");
	(&m_vertexA)[i] = position;
	(&m_vertexA)[i](3) = m_radius;

#ifdef HK_DEBUG
	hkVector4 diff; diff.setSub( m_vertexA, m_vertexB);
	HK_ASSERT2( 0xf010345, diff.length<3>().getReal() != 0.0f, "Capsules cannot have identical vertices. Use a sphere.");
#endif
}


const hkVector4* hkpCapsuleShape::getVertices() const
{
	return &m_vertexA;
}

const hkVector4& hkpCapsuleShape::getVertex(int i) const
{
	HK_ASSERT2(0x5e57e9fc,  i>=0 && i < 2, "A capsule has only 2 vertices. getVertex() must be passed either 0 or 1. ");
	return (getVertices())[i];
}

HK_FORCE_INLINE int hkpCapsuleShape::getNumCollisionSpheres() const
{
	return 2;
}

HK_FORCE_INLINE void hkpCapsuleShape::getCentre(hkVector4& centreOut) const
{
	centreOut.setAdd(m_vertexA, m_vertexB);
	centreOut.mul(hkSimdReal_Inv2);
}

HK_FORCE_INLINE void hkpCapsuleShape::convertVertexIdsToVertices(const hkpVertexId* ids, int numIds, hkcdVertex* verticesOut) const
{
	for (int i = numIds - 1; i >= 0; i--)
	{
		int id = ids[0];
		static_cast<hkVector4&>(verticesOut[0]) =  *hkAddByteOffsetConst<hkVector4>( getVertices(), id );	// do a quick address calculation
		verticesOut[0].setInt24W( id );
		verticesOut++;
		ids++;
	}
}

HK_FORCE_INLINE void hkpCapsuleShape::getAabb(const hkTransform& localToWorld, hkReal tolerance, hkAabb& out) const
{
	hkVector4 tol4;
	tol4.setAll(tolerance + m_radius);
	tol4.zeroComponent<3>();
	
	hkVector4 obj[2];
	hkVector4Util::transformPoints( localToWorld, getVertices(), 2, obj );

	out.m_min.setMin( obj[0], obj[1] );
	out.m_min.sub( tol4 );

	out.m_max.setMax( obj[0], obj[1] );
	out.m_max.add( tol4 );
}

HK_FORCE_INLINE void hkpCapsuleShape::getSupportingVertex(hkVector4Parameter direction, hkcdVertex& supportingVertexOut) const
{
	// direction is already in "this" space, so:
	hkVector4 diff; diff.setSub( m_vertexB, m_vertexA );
	const hkSimdReal dot1 = diff.dot<3>(direction);

	hkVector4 vA = m_vertexA;	vA.setInt24W(0);
	hkVector4 vB = m_vertexB;	vB.setInt24W(hkSizeOf(hkVector4));
	supportingVertexOut.setSelect(dot1.lessZero(), vA, vB);
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
