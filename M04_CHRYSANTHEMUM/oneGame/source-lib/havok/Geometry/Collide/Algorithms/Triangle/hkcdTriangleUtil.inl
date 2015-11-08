/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//
//	Method to calculate the (non-normalized) triangle normal.
//	Given the vertices, set normal to be the non-unit normal.
//		normal (Output) The normal of the triangle
//		a First vertex.
//		b Second vertex.
//		c Third vertex.

inline void HK_CALL hkcdTriangleUtil::calcUnitNormal(hkVector4Parameter vA, hkVector4Parameter vB, hkVector4Parameter vC, hkVector4& normal)
{
	hkVector4 vAB;	vAB.setSub(vB, vA);
	hkVector4 vAC;	vAC.setSub(vC, vA);
	hkVector4 vN;	vN.setCross(vAB, vAC);

	vN.normalize<3>();
	normal = vN;
}

//
//	Method to calculate the (non-normalized) triangle normal.
//	Given the vertices, set normal to be the non-unit normal.
//		normal (Output) The normal of the triangle
//		a First vertex.
//		b Second vertex.
//		c Third vertex.

inline void HK_CALL hkcdTriangleUtil::calcNonUnitNormal(hkVector4Parameter vA, hkVector4Parameter vB, hkVector4Parameter vC, hkVector4& normal)
{
	hkVector4 vAB;	vAB.setSub(vB, vA);
	hkVector4 vAC;	vAC.setSub(vC, vA);
	normal.setCross(vAB, vAC);
}

//
//	Computes the normalized plane equation from the three given vertices

inline void HK_CALL hkcdTriangleUtil::calcUnitPlaneEquation(hkVector4Parameter vA, hkVector4Parameter vB, hkVector4Parameter vC, hkVector4& planeOut)
{
	// Compute triangle normal
	hkVector4 vN;
	hkcdTriangleUtil::calcUnitNormal(vA, vB, vC, vN);

	// Compute and store plane offset in w
	planeOut.setXYZ_W(vN, -vN.dot<3>(vA));
}

//
//	Computes the unnormalized plane equation from the three given vertices

inline void HK_CALL hkcdTriangleUtil::calcNonUnitPlaneEquation(hkVector4Parameter vA, hkVector4Parameter vB, hkVector4Parameter vC, hkVector4& planeOut)
{
	// Compute triangle normal
	hkVector4 vN;
	hkcdTriangleUtil::calcNonUnitNormal(vA, vB, vC, vN);

	// Compute and store plane offset in w
	planeOut.setXYZ_W(vN, -vN.dot<3>(vA));
}

//
//	Computes twice the area of the given triangle

inline hkSimdReal HK_CALL hkcdTriangleUtil::calcDoubleArea(hkVector4Parameter vA, hkVector4Parameter vB, hkVector4Parameter vC)
{
	hkVector4 vAB;	vAB.setSub(vB, vA);
	hkVector4 vAC;	vAC.setSub(vC, vA);
	hkVector4 vN;	vN.setCross(vAB, vAC);
	return vN.length<3>();
}

//
//	Method to calculate the triangle centroid.
//	Given the vertices list, set the centroid to be the centroid of the three vertices.
//		centroid (output) The centroid of the triangle
//		a First vertex.
//		b Second vertex.
//		c Third vertex.

inline void HK_CALL hkcdTriangleUtil::calcCentroid(hkVector4Parameter a, hkVector4Parameter b, hkVector4Parameter c, hkVector4& centroid)
{
	centroid.setAdd(a, b);
	centroid.add(c);

	hkSimdReal third = hkSimdReal::getConstant<HK_QUADREAL_INV_3>();
	centroid.mul(third);
}

//
//	Returns true if the point is in front of the Triangle.
//	Given the plane in which the triangle is embedded, the point is in front if (and only if)
//	the point is in the half space (defined by the plane) that the normal points into.
//		point The point to examine
//		a First vertex.
//		b Second vertex.
//		c Third vertex.

inline hkBool32 HK_CALL hkcdTriangleUtil::isPointInFront(hkVector4Parameter point, hkVector4Parameter a, hkVector4Parameter b, hkVector4Parameter c)
{
	hkVector4 triangleNormal;
	hkcdTriangleUtil::calcNonUnitNormal(a, b, c, triangleNormal);

	hkVector4 d;
	d.setSub(point, a);

	hkSimdReal dot = d.dot<3>(triangleNormal);

	return dot.isGreaterZero();
}

//
//	END!
//

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
