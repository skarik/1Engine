/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


#include <Common/Base/Types/Geometry/Aabb/hkAabb.h>

void HK_CALL hkAabbUtil::calcAabb( const hkTransform& localToWorld, const hkVector4& halfExtents, hkSimdRealParameter extraRadius, hkAabb& aabbOut )
{
	hkVector4 he0; he0.setBroadcast<0>( halfExtents );
	hkVector4 he1; he1.setBroadcast<1>( halfExtents );
	hkVector4 he2; he2.setBroadcast<2>( halfExtents );

	hkVector4 transformedX; transformedX.setMul( he0, localToWorld.getRotation().getColumn(0));
	hkVector4 transformedY; transformedY.setMul( he1, localToWorld.getRotation().getColumn(1));
	hkVector4 transformedZ; transformedZ.setMul( he2, localToWorld.getRotation().getColumn(2));
	
	transformedX.setAbs( transformedX );
	transformedY.setAbs( transformedY );
	transformedZ.setAbs( transformedZ );

	hkVector4 extra; extra.setAll( extraRadius );
	transformedZ.add( extra );

	hkVector4 max;
	max.setAdd( transformedX, transformedY );
	max.add( transformedZ );

	hkVector4 min;	min.setNeg<4>(max);

	max.add( localToWorld.getTranslation() );
	min.add( localToWorld.getTranslation()  );

	aabbOut.m_max = max;
	aabbOut.m_min = min;
}


void HK_CALL hkAabbUtil::calcAabb( const hkTransform& localToWorld, const hkVector4& halfExtents, const hkVector4& center, hkSimdRealParameter extraRadius, hkAabb& aabbOut )
{
	hkVector4 he0; he0.setBroadcast<0>( halfExtents );
	hkVector4 he1; he1.setBroadcast<1>( halfExtents );
	hkVector4 he2; he2.setBroadcast<2>( halfExtents );

	hkVector4 transformedX; transformedX.setMul( he0, localToWorld.getRotation().getColumn(0));
	hkVector4 transformedY; transformedY.setMul( he1, localToWorld.getRotation().getColumn(1));
	hkVector4 transformedZ; transformedZ.setMul( he2, localToWorld.getRotation().getColumn(2));

	transformedX.setAbs( transformedX );
	transformedY.setAbs( transformedY );
	transformedZ.setAbs( transformedZ );


	hkVector4 extra; extra.setAll( extraRadius );
	transformedZ.add( extra );

	hkVector4 max;
	max.setAdd( transformedX, transformedY );
	max.add( transformedZ );

	hkVector4 min;	min.setNeg<4>(max);

	hkVector4 temp;	temp._setTransformedPos(localToWorld, center);

	max.add( temp );
	min.add( temp );

	aabbOut.m_max = max;
	aabbOut.m_min = min;
}

void HK_CALL hkAabbUtil::calcAabb( const hkTransform& localToWorld, const hkVector4& halfExtents, const hkVector4& center, hkAabb& aabbOut )
{
	hkVector4 he0; he0.setBroadcast<0>( halfExtents );
	hkVector4 he1; he1.setBroadcast<1>( halfExtents );
	hkVector4 he2; he2.setBroadcast<2>( halfExtents );

	hkVector4 transformedX; transformedX.setMul( he0, localToWorld.getRotation().getColumn(0));
	hkVector4 transformedY; transformedY.setMul( he1, localToWorld.getRotation().getColumn(1));
	hkVector4 transformedZ; transformedZ.setMul( he2, localToWorld.getRotation().getColumn(2));

	transformedX.setAbs( transformedX );
	transformedY.setAbs( transformedY );
	transformedZ.setAbs( transformedZ );

	hkVector4 max;
	max.setAdd( transformedX, transformedY );
	max.add( transformedZ );

	hkVector4 min;	min.setNeg<4>(max);

	hkVector4 temp;	temp._setTransformedPos(localToWorld, center);

	max.add( temp );
	min.add( temp );

	aabbOut.m_max = max;
	aabbOut.m_min = min;
}

void HK_CALL hkAabbUtil::transformAabbIntoLocalSpace( const hkTransform& localToWorld, const hkAabb& aabb, hkAabb& aabbOut )
{
	hkVector4 center;		aabb.getCenter(center);
	hkVector4 halfExtents;	aabb.getHalfExtents(halfExtents);

	hkRotation rot; rot._setTranspose( localToWorld.getRotation() );

	hkVector4 transformedX; transformedX.setMul( halfExtents.getComponent<0>(), rot.getColumn(0));
	hkVector4 transformedY; transformedY.setMul( halfExtents.getComponent<1>(), rot.getColumn(1));
	hkVector4 transformedZ; transformedZ.setMul( halfExtents.getComponent<2>(), rot.getColumn(2));
	transformedX.setAbs( transformedX );
	transformedY.setAbs( transformedY );
	transformedZ.setAbs( transformedZ );

	hkVector4 max;
	max.setAdd( transformedX, transformedY );
	max.add( transformedZ );

	hkVector4 min;	min.setNeg<4>(max);

	hkVector4 temp;	temp._setTransformedInversePos(localToWorld, center);

	min.add( temp );
	max.add( temp );

	aabbOut.m_max = max;
	aabbOut.m_min = min;
}



//
void hkAabbUtil::calcAabb( const hkVector4& center, hkSimdRealParameter innerRadius, hkAabb& aabbOut)
{
	hkVector4	ir;	ir.setAll(innerRadius);
	aabbOut.m_min.setSub(center,ir);
	aabbOut.m_max.setAdd(center,ir);
	aabbOut.m_min.zeroComponent<3>();
	aabbOut.m_max.zeroComponent<3>();
}

//
inline hkReal hkAabbUtil::getOuterRadius(const hkAabb& aabb)
{
	return(0.5f*aabb.m_min.distanceTo(aabb.m_max).getReal());
}

//
inline hkReal	hkAabbUtil::distanceSquared(const hkAabb& a,const hkVector4& b)
{
	if(a.containsPoint(b))
	{
		return(0);
	}
	else
	{
		hkVector4	c;		
		c.setMin(b,a.m_max);
		hkVector4	d;
		d.setMax(c,a.m_min);
		return(d.distanceToSquared(b).getReal());
	}
}

//
inline void		hkAabbUtil::getVertex(const hkAabb& aabb, int index, hkVector4& vertexOut)
{
	HK_ASSERT2(0x1ACBA392,0 <= index && index <=7,"Invalid vertex index");
	const hkVector4	bounds[]={aabb.m_min,aabb.m_max};
	vertexOut(0) = bounds[(index>>0)&1](0);
	vertexOut(1) = bounds[(index>>1)&1](1);
	vertexOut(2) = bounds[(index>>2)&1](2);
}

//
HK_FORCE_INLINE	void hkAabbUtil::scaleAabb(const hkVector4& scale, const hkAabb& aabbIn, hkAabb& aabbOut)
{
	hkVector4	newMin; newMin.setMul(aabbIn.m_min,scale);
	hkVector4	newMax; newMax.setMul(aabbIn.m_max,scale);
	aabbOut.m_min.setMin(newMin,newMax);
	aabbOut.m_max.setMax(newMin,newMax);
}

//
HK_FORCE_INLINE void	hkAabbUtil::expandAabbByMotion(const hkAabb& aabbIn, const hkVector4& motion, hkAabb& aabbOut)
{
	hkVector4	radius; radius.setAll(motion.getComponent<3>());
	hkVector4	minOffset; minOffset.setAdd(aabbIn.m_min, motion);
	hkVector4	maxOffset; maxOffset.setAdd(aabbIn.m_max, motion);
	hkVector4	newMin; newMin.setMin(minOffset, aabbIn.m_min);
	hkVector4	newMax; newMax.setMax(maxOffset, aabbIn.m_max);

	aabbOut.m_min.setSub(newMin, radius);
	aabbOut.m_max.setAdd(newMax, radius);
}

//
HK_FORCE_INLINE void	hkAabbUtil::projectAabbOnAxis(const hkVector4& axis, const hkAabb& aabb, hkSimdReal& minOut, hkSimdReal& maxOut)
{
	hkVector4	coeffsMin; coeffsMin.setMul(axis, aabb.m_min);
	hkVector4	coeffsMax; coeffsMax.setMul(axis, aabb.m_max);
	hkVector4	prjMin; prjMin.setMin(coeffsMin, coeffsMax);
	hkVector4	prjMax; prjMax.setMax(coeffsMin, coeffsMax);
	minOut = prjMin.horizontalAdd<3>();
	maxOut = prjMax.horizontalAdd<3>();
}

//
HK_FORCE_INLINE void	hkAabbUtil::projectAabbMinOnAxis(const hkVector4& axis, const hkAabb& aabb, hkSimdReal& prjOut)
{
	hkVector4	coeffsMin; coeffsMin.setMul(axis, aabb.m_min);
	hkVector4	coeffsMax; coeffsMax.setMul(axis, aabb.m_max);
	hkVector4	prj; prj.setMin(coeffsMin, coeffsMax);
	prjOut = prj.horizontalAdd<3>();
}

//
HK_FORCE_INLINE void	hkAabbUtil::projectAabbMaxOnAxis(const hkVector4& axis, const hkAabb& aabb, hkSimdReal& prjOut)
{
	hkVector4	coeffsMin; coeffsMin.setMul(axis, aabb.m_min);
	hkVector4	coeffsMax; coeffsMax.setMul(axis, aabb.m_max);
	hkVector4	prj; prj.setMax(coeffsMin, coeffsMax);
	prjOut = prj.horizontalAdd<3>();
}

//
HK_FORCE_INLINE void	hkAabbUtil::computeAabbPlaneSpan(const hkVector4& plane, const hkAabb& aabb, hkSimdReal& minOut, hkSimdReal& maxOut)
{
	hkSimdReal prj[2];
	projectAabbOnAxis(plane, aabb, prj[0], prj[1]);
	minOut = prj[0] + plane.getComponent<3>();
	maxOut = prj[1] + plane.getComponent<3>();
}

void hkAabbUtil::sweepAabb(const hkMotionState* motionState, hkReal tolerance, const hkAabb& aabbIn, hkAabb& aabbOut)
{
	//
	//  Expand the AABB by the angular part
	//

	hkReal radius = motionState->m_deltaAngle(3) * motionState->m_objectRadius;
	hkVector4 rotationalGain; rotationalGain.setAll(radius);
	aabbOut.m_min.setSub(aabbIn.m_min, rotationalGain);
	aabbOut.m_max.setAdd(aabbIn.m_max, rotationalGain);

	//
	// restrict the size of the AABB to the worst case radius size
	//
	{
		hkSimdReal objectRadius; objectRadius.setFromFloat(motionState->m_objectRadius);
		hkSimdReal toleranceSr;  toleranceSr.setFromFloat(tolerance);
		hkVector4 radius4; radius4.setAll( objectRadius + toleranceSr );
		hkVector4 maxR; maxR.setAdd( motionState->getSweptTransform().m_centerOfMass1, radius4 );
		hkVector4 minR; minR.setSub( motionState->getSweptTransform().m_centerOfMass1, radius4 );

		HK_ON_DEBUG
		(
			// I consider success when (aabbInOut.m_min <= maxR) and (minR <= aabbInOut.m_max) are true for all XYZ
			hkVector4Comparison a = aabbIn.m_min.lessEqual( maxR );
			hkVector4Comparison b = minR.lessEqual( aabbIn.m_max );
			hkVector4Comparison both; both.setAnd(a,b);
			HK_ASSERT2(0x366ca7b2, both.allAreSet(hkVector4Comparison::MASK_XYZ), "Invalid Radius. Did you make changes to a shape or change its center of mass while the owner was added to the world?" );
		);

		aabbOut.m_min.setMax( aabbOut.m_min, minR );
		aabbOut.m_min.setMin( aabbOut.m_min, aabbIn.m_min );

		aabbOut.m_max.setMin( aabbOut.m_max, maxR );
		aabbOut.m_max.setMax( aabbOut.m_max, aabbIn.m_max );
	}

	//
	// Sweep/expand the AABB along the motion path
	//

	hkVector4 invPath; invPath.setSub( motionState->getSweptTransform().m_centerOfMass0, motionState->getSweptTransform().m_centerOfMass1 );
	hkVector4 zero; zero.setZero();
	hkVector4 minPath; minPath.setMin( zero, invPath );
	hkVector4 maxPath; maxPath.setMax( zero, invPath );

	aabbOut.m_min.add( minPath );
	aabbOut.m_max.add( maxPath );

#if defined(HK_DEBUG)
	hkReal diffMinX = aabbIn.m_min(0) - aabbOut.m_min(0);
	hkReal diffMinY = aabbIn.m_min(1) - aabbOut.m_min(1);
	hkReal diffMinZ = aabbIn.m_min(2) - aabbOut.m_min(2);
	hkReal diffMaxX = aabbOut.m_max(0) - aabbIn.m_max(0);
	hkReal diffMaxY = aabbOut.m_max(1) - aabbIn.m_max(1);
	hkReal diffMaxZ = aabbOut.m_max(2) - aabbIn.m_max(2);
	HK_ASSERT2( 0xaf63e413, diffMinX >= 0.0f && diffMinY >= 0.0f && diffMinZ >= 0.0f && diffMaxX >= 0.0f && diffMaxY >= 0.0f && diffMaxZ >= 0.0f, "Expanded AABB is smaller than the unexpanded AABB." );
#endif
}


HK_ON_CPU( HK_FORCE_INLINE void HK_CALL hkAabbUtil::sweepOffsetAabb(const hkAabbUtil::OffsetAabbInput& input, const hkAabb& aabbIn, hkAabb& aabbOut) )
HK_ON_SPU( HK_FORCE_INLINE void HK_CALL hkAabbUtil_sweepOffsetAabb(const hkAabbUtil::OffsetAabbInput& input, const hkAabb& aabbIn, hkAabb& aabbOut) )
{
	hkSimdReal half = hkSimdReal_Half;
	hkVector4 aabbHalfSize; aabbHalfSize.setSub(aabbIn.m_max, aabbIn.m_min);  aabbHalfSize.mul(half);
	hkVector4 aabbCenter; aabbCenter.setInterpolate(aabbIn.m_max, aabbIn.m_min, half);
	hkVector4 arm; arm._setTransformedPos(input.m_endTransformInv, aabbCenter);


	hkVector4 min = aabbCenter;
	hkVector4 max = aabbCenter;
	{
		hkVector4 p; p._setTransformedPos(input.m_startTransform, arm);
		min.setMin(min, p);
		max.setMax(max, p);
	}

	// extended arm for the in-between transforms (cos(22.5deg)
	const hkVector4 centerOfMassLocal = input.m_motionState->getSweptTransform().m_centerOfMassLocal;

	for (int i = 0; i < input.m_numTransforms; i++)
	{	
		hkVector4 extendedArm;
		extendedArm.setSub(arm, centerOfMassLocal);
		extendedArm.mul(input.m_transforms[i].getTranslation().getComponent<3>());
		extendedArm.add(centerOfMassLocal);

		hkVector4 p; p._setTransformedPos(input.m_transforms[i], extendedArm);
		min.setMin(min, p);
		max.setMax(max, p);
	}

	//
	// Expand the AABB due to angular rotation of the shape around the aabbIn's center
	//
	{
		hkSimdReal r = aabbHalfSize.length<3>();
		hkVector4 radius; radius.setAll(r);

		// Limit increase to largest expansion that is actually possible: (sqrt(3)-1) * r
		hkSimdReal limit; limit.setFromFloat(0.732050808f);
		
		r.setMin( input.m_motionState->m_deltaAngle.getComponent<3>(), limit * r );

		aabbHalfSize.addMul(r, radius);
		aabbHalfSize.setMin(radius, aabbHalfSize);
	}

	aabbOut.m_min.setSub(min, aabbHalfSize);
	aabbOut.m_max.setAdd(max, aabbHalfSize);

	// We need to ensure that in order for our hkAabbUint32 compression to work
	aabbOut.m_min.setMin(aabbIn.m_min, aabbOut.m_min);
	aabbOut.m_max.setMax(aabbIn.m_max, aabbOut.m_max);
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
