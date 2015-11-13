/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Geometry/Internal/Types/hkcdRay.h>
#include <Geometry/Internal/Algorithms/Distance/hkcdDistancePointLine.h>
#include <Geometry/Internal/Algorithms/ClosestPoint/hkcdClosestPointLineLine.h>
#include <Geometry/Internal/Algorithms/RayCast/hkcdRayCastSphere.h>

	
HK_FORCE_INLINE hkBool32 hkcdRayCastCapsule(
	const hkcdRay& ray,
	hkVector4Parameter vertex0,
	hkVector4Parameter vertex1,
	hkSimdRealParameter radius,
	hkSimdReal* HK_RESTRICT fractionInOut,
	hkVector4* HK_RESTRICT normalOut,
	hkcdRayCastCapsuleHitType* HK_RESTRICT hitTypeOut )
{
	{
		hkSimdReal sToCylDistSq = hkcdPointSegmentDistanceSquared( ray.m_origin, vertex0, vertex1 );
		if( sToCylDistSq < (radius*radius) )
		{
			return false; // origin inside the cylinder
		}
	}

	// Work out closest points to cylinder
	hkSimdReal infInfDistSquared;
	hkSimdReal t, u;

	const hkVector4 dA = ray.m_direction;
	hkVector4 dB; dB.setSub( vertex1, vertex0 );

	// Get distance between inf lines + parametric description (t, u) of closest points,
	{
		
		hkcdClosestPointLineLine(ray.m_origin, dA, vertex0, dB, t, u);

		hkVector4 p,q;
		p.setAddMul(ray.m_origin, dA, t);
		q.setAddMul(vertex0, dB, u);
		hkVector4 diff; diff.setSub(p, q);
		infInfDistSquared = diff.lengthSquared<3>();

		if( infInfDistSquared > (radius*radius) )
		{
			return false; // Infinite ray is outside radius of infinite cylinder
		}
	}

	hkSimdReal axisLength;
	hkVector4 axis;
	hkSimdReal ipT;
	hkSimdReal zero; zero.setZero();
	{
		axis = dB;

		// Check for zero axis
		{
			hkSimdReal axisLengthSqrd = axis.lengthSquared<3>();
			const hkSimdReal eps = hkSimdReal::getConstant<HK_QUADREAL_EPS>();
			hkVector4Comparison mask = axisLengthSqrd.greater( eps );

			axisLength.setSelect( mask, axis.normalizeWithLength<3>(), zero );
			hkVector4 zeroVector; zeroVector.setZero();
			axis.setSelect( mask, axis, zeroVector );
		}

		hkSimdReal component = dA.dot<3>(axis);

		// Flatdir is now a ray firing in the "plane" of the cylinder.
		hkVector4 flatDir; flatDir.setAddMul(dA, axis, -component);

		// Convert d to a parameterization instead of absolute distance along ray.
		hkSimdReal flatDirLengthSquared3 = flatDir.lengthSquared<3>();
		
		// If flatDir is zero, the ray is parallel to the cylinder axis. In this case we set ipT to a
		// negative value to bypass the cylinder intersection test and go straight to the cap tests.
		hkVector4Comparison mask = flatDirLengthSquared3.equal(zero);
		hkSimdReal d = (radius * radius - infInfDistSquared) / flatDirLengthSquared3;
		ipT.setSelect(mask, hkSimdReal_Minus1, t - d.sqrt());			
	}

	// Intersection parameterization with infinite cylinder is outside length of ray
	// or is greater than a previous hit fraction
	if (ipT  >= *fractionInOut )
	{
		return false;
	}
	
	hkSimdReal ptHeight;
	hkSimdReal pointAProj = vertex0.dot<3>(axis);

	// Find intersection point of actual ray with infinite cylinder
	hkVector4 intersectPt; intersectPt.setAddMul( ray.m_origin, ray.m_direction, ipT );

	// Test height of intersection point w.r.t. cylinder axis
	// to determine hit against actual cylinder
	// Intersection point projected against cylinder
	hkSimdReal ptProj = intersectPt.dot<3>(axis);
	ptHeight = ptProj - pointAProj;

	if ( ipT.isGreaterEqualZero() ) // Actual ray (not infinite ray) must intersect with infinite cylinder
	{
		if ( ptHeight.isGreaterZero() && ptHeight.isLess(axisLength) ) // Report hit against cylinder part
		{
			// Calculate normal
			hkVector4 projPtOnAxis;
			projPtOnAxis.setInterpolate( vertex0, vertex1, ptHeight / axisLength );
			hkVector4 normal; normal.setSub( intersectPt, projPtOnAxis );
			normal.normalize<3>();
			*normalOut = normal;
			*fractionInOut = ipT; // This is a parameterization along the ray
			*hitTypeOut = HK_CD_RAY_CAPSULE_BODY;
			return true;
		}
	}

	// Cap tests
	{
		// Check whether start point is inside infinite cylinder or not
		hkSimdReal fromLocalProj = ray.m_origin.dot<3>(axis);
		hkSimdReal projParam = fromLocalProj - pointAProj;

		hkVector4 fromPtProjAxis;
		fromPtProjAxis.setInterpolate( vertex0, vertex1, projParam / axisLength );

		hkVector4 axisToRayStart;
		axisToRayStart.setSub( ray.m_origin, fromPtProjAxis);

		if ( ipT.isLessZero() && axisToRayStart.lengthSquared<3>().isGreater(radius * radius) )
		{
			// Ray starts outside infinite cylinder and points away... must be no hit
			return false;
		}

		// Ray can only hit 1 cap... Use intersection point
		// to determine which sphere to test against (NB: this only works because
		// we have discarded cases where ray starts inside)
		
		hkVector4 capEnd;
		hkVector4Comparison mask = ptHeight.lessEqualZero();
		capEnd.setSelect( mask, vertex0, vertex1 );
		hkVector4 rayFrom; rayFrom.setSub( ray.m_origin, capEnd );
		hkVector4 rayTo; ray.getEndPoint( rayTo );
		rayTo.setSub( rayTo, capEnd );
		hkVector4 posAndRadius; posAndRadius.setZero();
		posAndRadius.setW( radius );

		hkcdRay ray2;
		ray2.setEndPoints(rayFrom, rayTo);

		if( hkcdRayCastSphere( ray2, posAndRadius, fractionInOut, normalOut) )
		{
			*hitTypeOut = mask.anyIsSet() ? HK_CD_RAY_CAPSULE_CAP0 : HK_CD_RAY_CAPSULE_CAP1;
			return true;
		}

		return false;
	}
}

HK_FORCE_INLINE hkVector4Comparison hkcdRayBundleCapsuleIntersect( const hkcdRayBundle& rayBundle,
																  hkVector4Parameter vertex0, hkVector4Parameter vertex1, hkSimdRealParameter radius, 
																  hkVector4& fractionsInOut, hkFourTransposedPoints& normalsOut )
{
	hkVector4 sToCylDistSq = hkcdPointSegmentDistanceSquared( rayBundle.m_start, vertex0, vertex1 );
	
	hkVector4 radiusSqr; radiusSqr.setAll(radius * radius);
	hkVector4Comparison activeMask = rayBundle.m_activeRays;
	hkVector4Comparison rayStartOutside = sToCylDistSq.greaterEqual( radiusSqr );
	activeMask.setAnd(activeMask, rayStartOutside);
	if ( !activeMask.anyIsSet() )
	{
		return activeMask;
	}

	// Work out closest points to cylinder
	hkVector4 infInfDistSquared; //infInfDistSquared.setConstant<HK_QUADREAL_MAX>();
	hkVector4 t, u;

	hkFourTransposedPoints dA;
	dA.setSub(rayBundle.m_end, rayBundle.m_start);
	hkVector4 dB;
	dB.setSub( vertex1, vertex0 );

	// Get distance between inf lines + parametric description (t, u) of closest points,
	{
		hkcdClosestPointLineLine(rayBundle.m_start, dA, vertex0, dB, t, u);

		hkFourTransposedPoints p,q;
		p.setAddMul(rayBundle.m_start, dA, t);
		hkFourTransposedPoints B4; B4.setAll(vertex0);
		hkFourTransposedPoints dB4; dB4.setAll(dB);
		q.setAddMul(B4, dB4, u);
		hkFourTransposedPoints diff; diff.setSub(p, q);
		diff.dot3(diff, infInfDistSquared);
	}

	// Is infinite ray within radius of infinite cylinder?
	hkVector4Comparison isInfRayWithinCylinder = infInfDistSquared.lessEqual( radiusSqr );
	activeMask.setAnd(activeMask, isInfRayWithinCylinder);
	if ( !activeMask.anyIsSet() )
	{
		return activeMask;
	}

	hkSimdReal axisLength;
	hkVector4 axis;
	hkVector4 ipT;
	hkSimdReal zero; zero.setZero();
	hkVector4 zeroVector; zeroVector.setZero();
	{
		axis = dB;

		// Check for zero axis
		hkSimdReal axisLengthSqrd = axis.lengthSquared<3>();
		hkSimdReal eps = hkSimdReal::getConstant<HK_QUADREAL_EPS>();
		hkVector4Comparison mask = axisLengthSqrd.greater( eps );

		axisLength.setSelect( mask, axis.normalizeWithLength<3>(), zero );
		axis.setSelect( mask, axis, zeroVector );

		hkVector4 component; dA.dot3(axis, component);
		component.setNeg<4>(component);

		hkFourTransposedPoints flatDir;
		hkFourTransposedPoints axis4; axis4.setAll(axis);
		flatDir.setAddMul(dA, axis4, component);

		// Flatdir is now a ray firing in the "plane" of the cylinder.

		// Convert d to a parameterization instead of absolute distance along ray.
		// Avoid division by zero in case of ray parallel to infinite cylinder.
		hkVector4 flatDirLengthSquared3;
		flatDir.dot3(flatDir, flatDirLengthSquared3);

		hkVector4 d; d.setSub(radiusSqr, infInfDistSquared);
		d.div(flatDirLengthSquared3);
		d.setSqrt(d);
		d.setSub(t, d);

		mask = flatDirLengthSquared3.equal(zeroVector);
		hkVector4 minusOne = hkVector4::getConstant<HK_QUADREAL_MINUS1>();
		ipT.setSelect(mask, minusOne, d);			
	}

	// Intersection parameterization with infinite cylinder is outside length of ray
	// or is greater than a previous hit fraction
	hkVector4Comparison isFractionOk = ipT.less( fractionsInOut );
	activeMask.setAnd(activeMask, isFractionOk);
	if( !activeMask.anyIsSet() )
	{
		return activeMask;
	}

	hkVector4 ptHeight;
	hkVector4 pointAProj; pointAProj.setAll(vertex0.dot<3>(axis));

	// Find intersection point of actual ray with infinite cylinder
	hkFourTransposedPoints intersectPt;
	intersectPt.setAddMul( rayBundle.m_start, dA, ipT );

	// Test height of intersection point w.r.t. cylinder axis
	// to determine hit against actual cylinder
	// Intersection point projected against cylinder
	hkVector4 ptProj; intersectPt.dot3(axis, ptProj);
	ptHeight.setSub(ptProj, pointAProj);

	hkVector4 axisLengthVector; axisLengthVector.setAll(axisLength);
	hkFourTransposedPoints vertex0x4; vertex0x4.setAll(vertex0);
	hkFourTransposedPoints vertex1x4; vertex1x4.setAll(vertex1);
	hkFourTransposedPoints dBx4; dBx4.setAll(dB);

	hkVector4Comparison isRayWithinCylinder = ipT.greaterEqual(zeroVector);
	hkVector4Comparison gtZero = ptHeight.greater(zeroVector);
	hkVector4Comparison ltAxisLength = ptHeight.less(axisLengthVector);

	hkVector4Comparison hitBodyMask; hitBodyMask.setAnd(gtZero, ltAxisLength);
	hitBodyMask.setAnd(hitBodyMask, isRayWithinCylinder);
	
	if ( hitBodyMask.anyIsSet() ) // Actual ray (not infinite ray) must intersect with infinite cylinder
	{
		// Calculate normal
		hkVector4 ratio; ratio.setDiv(ptHeight, axisLengthVector);
		hkFourTransposedPoints projPtOnAxis;		
		projPtOnAxis.setAddMul( vertex0x4, dBx4, ratio );
		
		hkFourTransposedPoints normals;
		normals.setSub( intersectPt, projPtOnAxis );
		normals.normalize();

		normalsOut.setSelect(hitBodyMask, normals, normalsOut);

		// Output fraction
		fractionsInOut.setSelect(hitBodyMask, ipT, fractionsInOut); // This is a parameterization along the ray
	}
	
	hitBodyMask.setAndNot(activeMask, hitBodyMask);
	if (!hitBodyMask.anyIsSet())
	{
		return activeMask;
	}

	// Cap tests
	{
		// Check whether start point is inside infinite cylinder or not
		hkVector4 fromLocalProj;
		rayBundle.m_start.dot3(axis, fromLocalProj);

		hkVector4 projParam; projParam.setSub(fromLocalProj, pointAProj);

		hkVector4 ratio; ratio.setDiv(projParam, axisLengthVector);
		hkFourTransposedPoints fromPtProjAxis;
		fromPtProjAxis.setAddMul( vertex0x4, dBx4, ratio );

		hkFourTransposedPoints axisToRayStart;
		axisToRayStart.setSub( rayBundle.m_start, fromPtProjAxis );

		hkVector4 axisToRayStartLenSq;
		axisToRayStart.dot3(axisToRayStart, axisToRayStartLenSq);

		hkVector4Comparison gteZero = ipT.greaterEqual(zeroVector);
		hkVector4Comparison lteRadiusSqr = axisToRayStartLenSq.lessEqual(radiusSqr);
		hkVector4Comparison maskOr; maskOr.setOr(gteZero, lteRadiusSqr);
		activeMask.setAnd(activeMask, maskOr);
		if ( !activeMask.anyIsSet() )
		{
			// Ray starts outside infinite cylinder and points away... must be no hit
			return activeMask;
		}

		// Ray can only hit 1 cap... Use intersection point
		// to determine which sphere to test against (NB: this only works because
		// we have discarded cases where ray starts inside)

		hkFourTransposedPoints extra;
		hkVector4Comparison mask = ptHeight.lessEqual(zeroVector);
		extra.setSelect( mask, vertex0x4, vertex1x4 );
		
		hkcdRayBundle raySphere;
		raySphere.m_start.setSub(rayBundle.m_start, extra);
		raySphere.m_end.setSub(rayBundle.m_end, extra);
		raySphere.m_activeRays = hitBodyMask;

		hkVector4Comparison sphereMask = hkcdRayBundleSphereIntersect( raySphere, radius, fractionsInOut, normalsOut);
		activeMask.setSelect(hitBodyMask, sphereMask, activeMask);
		
	}
	return activeMask;
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
