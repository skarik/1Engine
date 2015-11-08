/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */



// Intersect a ray and a sphere.
//
// The algorithm works by solving a quadratic equation, constructed from the geometry
// of point lying on both the ray's line and the sphere, and within the ray segment.
// Good references are: 
//	* Christer Ericson's "Real-Time Collision Detection" 
//	* http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter1.htm
//
//
// The implementation adds a few modifications to a straight mathematical implementation 
//	to improve performance and accuracy.
//
//  * The order of operations is changed to prevent a hit fraction > 1.
//
//  * The algorithm works on the quadratic equations by considering an alternative ray start point 
//	   than the one passed in.
//	  The alternative point used is the closest point on the ray's line to the sphere.
//	  The resulting hit fraction is then made relative to the passed in point before returning it.
//
//	 The modification makes the algorithm more accurate when the distance between 
//	  the ray start position and the sphere is much larger than the sphere's radius.
//	  The accuracy gain comes from bringing the quadratic terms 'A', 'B' and 'C' closer to each other
//	  during the critical calculation of the determinant, therefore saving important precision bits.
//		
//	 Using this alternate point, the quadratic term 'B' is always zero and can be simplified from the
//	  calculations.
//		
//	 Using this alternate point, the quadratic term 'C' can be calculated from the length of the vector
//	  from the sphere's center to the alternative ray start point (which also is the closest on the 
//	  ray line to the sphere center).
//	 To find 'C' we first project the sphere's center on the line direction, from the obtained 
//	  line parameter 't' (in the code '-offset') we can calculate the vector from the passed in 
//	  ray start point to the alternate  point, and adding this vector to the vector from 
//	  sphere center to passed in ray start vector, we finally obtain the needed vector 
//	  from the sphere's center to it's closest point on the ray line. 
//  
//	 Exactly these vectors are used because we avoid losing precision bits by adding large vectors to small vectors
//	  as much as possible. 
//	 Per example, the closest point could be calculated by adding the passed in ray start position
//	  to the ray direction times 't', but this would involve addition of very large numbers 
//	  to very small numbers whenever the ray is far from the origin, but close to the sphere.
//
//  * The algorithm is tested for accuracy in the unit tests by comparing it to a reference double precision version,
//		using an extensive sampling of all variations of input parameters.
//
HK_FORCE_INLINE	hkBool32 hkcdRayCastSphere(const hkcdRay& ray, 
														hkVector4Parameter spherePosAndRadius, 
												hkSimdReal* HK_RESTRICT hitFractionInOut,
												hkVector4* HK_RESTRICT normalOut)
{	
	// Early-exit if ray pointing away from sphere.
	const hkVector4 dir = ray.m_direction;
	hkVector4 rayToSphere; rayToSphere.setSub(ray.m_origin, spherePosAndRadius);

	hkSimdReal offset = -dir.dot<3>(rayToSphere);	
	if (offset.isLessEqualZero())
	{
		return false;
	}

	// Calculate r and r^2
	hkSimdReal a = dir.lengthSquared<3>();
	hkSimdReal r = spherePosAndRadius.getComponent<3>();
	HK_ASSERT2(0x6c989739, r.isGreaterZero(), "sphere radius should not be zero");
	hkSimdReal r2 = r * r;	
	
	// projectedVector is the vector from ray.m_origin, to the closest point on the ray to the sphere.
	hkVector4 projectedVector; projectedVector.setMul(ray.m_direction, offset / a);		

	// cr can now be calculated from the projectedVector and rayToSphere.
	hkVector4 cr; cr.setAdd(projectedVector, rayToSphere);
	hkSimdReal c = cr.dot<3>(cr) - r2;

	// det = b2 - a * c
	// Using the projected sphere center, b is always 0.
	hkSimdReal d = - a * c;
	if (d.isLessZero())
	{
		return false;
	}

	// Use the offset to recover the correct hit fraction.
	hkSimdReal t = - d.sqrt() + offset;
	hkSimdReal hitFraction = *hitFractionInOut;
	if (t.isLessZero() | t.isGreaterEqual(a * hitFraction))
	{
		return false;
	}	

	// Normalize hit fraction and calculate normal at intersection point.
	t.setDiv<HK_ACC_23_BIT, HK_DIV_IGNORE>(t,a);
	hkVector4 hitPoint; hitPoint.setAddMul(ray.m_origin, ray.m_direction, t);
	hkVector4 n; n.setSub(hitPoint, spherePosAndRadius);
	n.normalize<3>();

	// Save output results.
	*hitFractionInOut = t;
	*normalOut = n;
	return true;
}

	
	
HK_FORCE_INLINE hkVector4Comparison hkcdRayBundleSphereIntersect(const hkcdRayBundle& rayBundle, hkSimdRealParameter sphereRadius, hkVector4& fractionsInOut, hkFourTransposedPoints& normalsOut)
{
	HK_ASSERT2(0x29d49b87, rayBundle.m_activeRays.anyIsSet(), "Calling castRayBundle with no active rays!");

	hkVector4Comparison activeMask = rayBundle.m_activeRays;

	{
		hkVector4 radius; radius.setAll(sphereRadius);
		hkVector4 radius2; radius2.setMul(radius, radius);

		hkVector4 oneHundred; oneHundred.setAll(100.0f);

		// 
		// solve quadratic function: ax*x + bx + c = 0
		//

		// Scalar version:
		//	A = dir.lengthSquared3();
		//	B = dir.dot3( rayBundle.m_start )
		hkVector4 A;
		hkVector4 B;

		hkFourTransposedPoints vDir;
		{
			vDir.setSub(rayBundle.m_end, rayBundle.m_start);
			vDir.dot3(vDir, A);
			vDir.dot3(rayBundle.m_start, B);
		}

		// Omitting early-out check:  if ( B >= 0 )

		//
		//	Check for long rays (check for startpoints being 10 times outside the radius
		//
		hkVector4 offset;

		hkVector4 AtimeRadius2time100; AtimeRadius2time100.setMul(A, radius2);
		AtimeRadius2time100.mul(oneHundred);
		hkVector4 B2; B2.setMul(B,B);

		hkVector4Comparison longRayCheck = B2.greater(AtimeRadius2time100);

		// Omitting early-out check: if ( B * B > A * radius2 * 100.0f) && if ( A < radius2 )

		hkVector4 zero; zero.setZero();
		
		offset.setSelect(longRayCheck, B, zero);
		offset.setNeg<4>(offset);

		
		//B.setSelect(longRayCheck, zero, B);
		B.setAdd(B, offset);
		B2.setSelect(longRayCheck, zero, B2);

		hkVector4 invA; invA.setReciprocal<HK_ACC_FULL, HK_DIV_IGNORE>(A);
		hkVector4 midPointInterp; midPointInterp.setMul(offset, invA);

		// Scalar version:  C = midPoint.lengthSquared3() - radius2;
		hkVector4 C;
		hkFourTransposedPoints midPoint;
		midPoint.setAddMul(rayBundle.m_start, vDir, midPointInterp);
		midPoint.dot3(midPoint, C);
		C.sub(radius2);

		// Scalar version: det = B*B - A*C;
		hkVector4 det;
		det.setNeg<4>(A);
		det.mul(C);
		det.add(B2);

		hkVector4Comparison infRayDoesHit = det.greaterEqual(zero);
		// We need this modification to the mask: if det<=0, the (infinite) ray doesn't hit.
		activeMask.setAnd(activeMask, infRayDoesHit);


		// Scalar version:
		//	sqDet = hkMath::sqrt( det );
		//	t2 = -B - sqDet;
		//	t = t2 + offset;
		// We're taking the inverse square root of a (possibly) negative number or zero
		// But that's OK, since we mask out the results
		hkVector4 sqDet; 

		
		sqDet.setSqrt(det);
		sqDet.setSelect(infRayDoesHit, sqDet, zero); // this makes sure we're OK if det=0

		hkVector4 tV = offset;
		tV.sub(B);
		tV.sub(sqDet);

		// Scalar version checks  t >= (A * results.m_hitFraction)  and then computes t/A
		// Since we're going to end up dividing anyway, just compare t/A >= results.m_hitFraction
		tV.mul(invA);

		hkVector4Comparison tLessThanHitFrac = tV.less(fractionsInOut);
		hkVector4Comparison tGreaterThan0 = tV.greaterEqual(zero);

		// We have a hit if (t < m_hitFraction) && t>=0
		activeMask.setAnd(activeMask, tLessThanHitFrac);
		activeMask.setAnd(activeMask, tGreaterThan0);

		hkVector4 invRadius; invRadius.setReciprocal(radius);

		// Compute all the normals at once instead of doing scalar divides
		hkFourTransposedPoints fourNormals;
		{
			fourNormals.setAddMul(rayBundle.m_start, vDir, tV);

			fourNormals.mul(invRadius);
		}

		normalsOut.setSelect(activeMask, fourNormals, normalsOut);
		fractionsInOut.setSelect(activeMask, tV, fractionsInOut);
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
