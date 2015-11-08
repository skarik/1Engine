/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//
//	Intersects a line segment with a convex shape, given as a collection of boundary planes.

inline hkBool32 HK_CALL hkcdRayCastConvex(
	const hkcdRay& ray,
	const hkVector4* HK_RESTRICT planeEquations,
	int numPlaneEquations,
	hkSimdReal* HK_RESTRICT fractionInOut,
	hkVector4* HK_RESTRICT normalOut)
{
	hkIntVector bestNrm;	bestNrm.setZero();
	hkVector4 vLambda0 = hkVector4::getConstant<HK_QUADREAL_MINUS1>();
	hkVector4 vLambda1;		vLambda1.setAll(*fractionInOut);	// fraction

	// The main loop reads 4 at a time. Deal with the array not being a multiple of 4
	// (and potentially reading into unmapped memory) by doing the leftovers first.
	// e.g. for a shape with 9 verts, we do 3 loops - [5678], then [0123], then [4567]
	hkIntVector crtIdx;
	hkVector4 planes0, planes1, planes2, planes3;
	{
		int n = numPlaneEquations - 4;
		crtIdx.set( n, n+1, n+2, n+3 );

		planes0 = planeEquations[n+0];
		planes1 = planeEquations[n+1];
		planes2 = planeEquations[n+2];
		planes3 = planeEquations[n+3];
	}
	
	hkIntVector	nextIdx = hkIntVector::getConstant<HK_QUADINT_0123>();

	// Set w to 1 to compute the plane equations
	hkVector4 vFrom, vTo;
	{
		const hkSimdReal one = hkSimdReal::getConstant<HK_QUADREAL_1>();
		vFrom.setXYZ_W(ray.m_origin, one);
		ray.getEndPoint(vTo);
		vTo.setXYZ_W(vTo, one);
	}

	const int lastBatchIndex = numPlaneEquations & ~3;
	int batchIndex = 0;
	while(true)
	{
		// Compute start and end distances to the planes
		hkVector4 dStart, dEnd;	
		hkVector4Util::dot4_1vs4(vFrom, planes0, planes1, planes2, planes3, dStart);
		hkVector4Util::dot4_1vs4(vTo, planes0, planes1, planes2, planes3, dEnd);

		// Calculate the intersection lambda between ray and the current plane
		hkVector4 vLambda;
		vLambda.setSub(dStart, dEnd);
		vLambda.setDiv(dStart, vLambda);

		hkVector4Comparison ds_ge_0		= dStart.greaterEqualZero();	// (dStart >= 0.0f)
		hkVector4Comparison de_ge_0		= dEnd.greaterEqualZero();	// (dEnd >= 0.0f)

		// Check for not colliding
		{
			hkVector4Comparison notColliding;
			notColliding.setAnd(ds_ge_0, de_ge_0);
			if ( notColliding.anyIsSet() )
			{
				return false;	// At least one of the planes is not colliding
			}
		}

		// Select entry lambda
		{
			hkVector4Comparison cmp;
			cmp.setAnd(ds_ge_0, vLambda0.less(vLambda));
			cmp.setAndNot(cmp, de_ge_0);	// valid && (dStart >= 0) && (dEnd < 0) && (lambda0 < lambda)

			vLambda0.setSelect(cmp, vLambda, vLambda0);
			bestNrm.setSelect(cmp, crtIdx, bestNrm);
		}

		// Select exit lambda
		{
			hkVector4Comparison cmp;
			cmp.setAnd(de_ge_0, vLambda1.greater(vLambda));
			cmp.setAndNot(cmp, ds_ge_0);	// valid && (dStart < 0) && (dEnd >= 0) && (lambda < lambda1)

			vLambda1.setSelect(cmp, vLambda, vLambda1);
		}

		if( batchIndex < lastBatchIndex )
		{
			// another full batch to go
			planes0 = planeEquations[batchIndex+0];
			planes1 = planeEquations[batchIndex+1];
			planes2 = planeEquations[batchIndex+2];
			planes3 = planeEquations[batchIndex+3];

			hkIntVector four; four.splatImmediate32<4>();
			crtIdx = nextIdx;
			nextIdx.setAddU32(nextIdx, four);
			batchIndex += 4;
		}
		else
		{
			break;
		}
	}

	// Final checks
	{
		hkSimdReal lambda0 = vLambda0.horizontalMax<4>();
		hkSimdReal lambda1 = vLambda1.horizontalMin<4>();
		*fractionInOut = lambda0;

		// Select best normal
		{
			// Determine the index of the maximum component
			hkVector4 vTmp;	vTmp.setAll(lambda0);
			const int msk = vTmp.equal(vLambda0).getMask();

			// Convert mask to index
			const int xx = (msk >> hkVector4Comparison::INDEX_X) & 1;
			const int yy = (msk >> hkVector4Comparison::INDEX_Y) & 1;
			const int zz = (msk >> hkVector4Comparison::INDEX_Z) & 1;
			const int nrmIdx = (1 - xx) * (3 - 2 * yy - zz * (1 - yy));
			HK_ASSERT(0x52941310, (nrmIdx >= 0) && (nrmIdx < 4) && (msk & hkVector4Comparison::getMaskForComponent(nrmIdx)) );

			*normalOut = planeEquations[bestNrm.getU32(nrmIdx)];
		}

		// (lambda1 < lambda0) || (lambda0 < 0)
		hkVector4Comparison noIntersection;
		noIntersection.setOr(lambda1.less(lambda0), lambda0.lessZero());

		return !noIntersection.anyIsSet();
	}
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
