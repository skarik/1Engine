/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


inline void hkKdTreeMath::setInverseScale( hkVector4& vecOut )
{
	const hkQuadReal oneOver65K = HK_QUADREAL_CONSTANT(1.0f / hkReal(hkKdTree::SCALE_MULTIPLIER), 1.0f / hkReal(hkKdTree::SCALE_MULTIPLIER), 1.0f / hkReal(hkKdTree::SCALE_MULTIPLIER), 1.0f / hkReal(hkKdTree::SCALE_MULTIPLIER));
	vecOut.m_quad = oneOver65K;
}


inline hkBool32 hkKdTreeMath::isComponentEqZero(hkVector4Parameter v, int component)
{
	hkVector4 zero; zero.setZero();
	hkVector4Comparison compMask = v.equal(zero);
	return compMask.allAreSet(hkVector4Comparison::getMaskForComponent(component));
}

inline hkBool32 hkKdTreeMath::isComponentGtZero(hkVector4Parameter v, int component)
{
	hkVector4 zero; zero.setZero();
	hkVector4Comparison compMask = v.greater(zero);
	return compMask.allAreSet(hkVector4Comparison::getMaskForComponent(component));
}


// sets v to its minimum component
inline void hkKdTreeMath::vectorBroadcastMin3(hkVector4& v)
{
	hkVector4 temp0, temp1, temp2, temp;

	temp0.setBroadcast<0>(v);
	temp1.setBroadcast<1>(v);
	temp.setMin(temp0, temp1);
	temp2.setBroadcast<2>(v);
	v.setMin(temp, temp2);
}

inline void hkKdTreeMath::vectorBroadcastMin3(hkVector4& v, hkVector4ComparisonParameter ignoreMask)
{
	hkQuadReal allMax = HK_QUADREAL_CONSTANT(HK_REAL_MAX, HK_REAL_MAX, HK_REAL_MAX, HK_REAL_MAX);

	HK_ASSERT(0x2dc51fe7, !ignoreMask.allAreSet(hkVector4Comparison::MASK_XYZ)); // trying to ignore all 3 components
	hkVector4 temp0, temp1, temp2, temp;

	hkVector4 allMaxV; allMaxV.m_quad = allMax;
	v.setSelect(ignoreMask, allMaxV, v); 

	temp0.setBroadcast<0>(v);
	temp1.setBroadcast<1>(v);
	temp.setMin(temp0, temp1);
	temp2.setBroadcast<2>(v);
	v.setMin(temp, temp2);
}

// sets v to its maximum component
inline void hkKdTreeMath::vectorBroadcastMax3(hkVector4& v)
{
	hkVector4 temp0, temp1, temp2, temp;

	temp0.setBroadcast<0>(v);
	temp1.setBroadcast<1>(v);
	temp.setMax(temp0, temp1);
	temp2.setBroadcast<2>(v);
	v.setMax(temp, temp2);
}

inline void hkKdTreeMath::vectorBroadcastMax3(hkVector4& v, hkVector4ComparisonParameter ignoreMask)
{
	hkQuadReal allMin = HK_QUADREAL_CONSTANT(-HK_REAL_MAX, -HK_REAL_MAX, -HK_REAL_MAX, -HK_REAL_MAX);

	HK_ASSERT(0x2dc51fe7, !ignoreMask.allAreSet(hkVector4Comparison::MASK_XYZ)); // trying to ignore all 3 components
	hkVector4 temp0, temp1, temp2, temp;

	hkVector4 allMinV; allMinV.m_quad = allMin;
	v.setSelect(ignoreMask, allMinV, v); 

	temp0.setBroadcast<0>(v);
	temp1.setBroadcast<1>(v);
	temp.setMax(temp0, temp1);
	temp2.setBroadcast<2>(v);
	v.setMax(temp, temp2);
}

inline hkBool hkKdTreeMath::rayAabbCheckSlow(hkVector4Parameter orig, hkVector4Parameter invDir, hkReal tNear, hkReal tFar, const hkAabb& aabb)
{
	for (int i=0; i<3; i++)
	{
		if (invDir(i) == 0.0f) // Ray is flat in this direction; can't exclude
		{
			// check that origin is within the bounds of the other planes
			if (orig(i) > aabb.m_max(i) || orig(i) < aabb.m_min(i))
			{
				return false;
			}
			continue;
		}

		hkReal splitMin = aabb.m_min(i);
		hkReal splitMax = aabb.m_max(i);
		hkReal dNear = ( splitMin - orig( i ) ) * invDir( i );
		hkReal dFar  = ( splitMax - orig( i ) ) * invDir( i );

		if (invDir(i) < 0.0f)
		{
			hkReal temp = dFar;
			dFar = dNear;
			dNear = temp;
		}

		tNear = hkMath::max2(dNear, tNear);
		tFar =  hkMath::min2(dFar, tFar);
	}

	return (tNear < tFar);
}

inline hkBool32 hkKdTreeMath::rayAabbCheck(hkVector4Parameter orig, hkVector4Parameter invDir, hkReal _tNear, hkReal _tFar, const hkAabb& aabb)
{
	hkVector4 dNearTemp;
	dNearTemp = aabb.m_min;
	hkVector4 dFarTemp;
	dFarTemp = aabb.m_max;

	dNearTemp.sub(orig);
	dFarTemp.sub( orig);
	dNearTemp.mul(invDir);
	dFarTemp.mul( invDir);

	hkVector4 zero; zero.setZero();
	hkVector4Comparison invDirLtZero = invDir.greaterEqual(zero);

	hkVector4 dNear, dFar;
	dNear.setSelect(invDirLtZero, dNearTemp, dFarTemp);
	dFar.setSelect( invDirLtZero, dFarTemp, dNearTemp);

	hkVector4 tNear, tFar;
	tNear.setAll(_tNear);
	tFar.setAll( _tFar );

	hkVector4Comparison invDirEqZero;
	invDirEqZero = invDir.equal(zero);

	hkVector4Comparison origInAabb = orig.greaterEqual(aabb.m_min);
	origInAabb.setAnd(origInAabb, orig.lessEqual(aabb.m_max));

	vectorBroadcastMax3(dNear, invDirEqZero);
	vectorBroadcastMin3(dFar,  invDirEqZero);

	tNear.setMax(tNear, dNear);
	tFar.setMin(tFar, dFar);

	hkVector4Comparison nearLtFar = tNear.less(tFar);
	hkVector4Comparison isOk; isOk.setSelect(invDirEqZero, origInAabb, nearLtFar);
	//return nearLtFar.allAreSet();
	return isOk.allAreSet(hkVector4Comparison::MASK_XYZ);
}

inline hkVector4Comparison hkKdTreeMath::rayBundleAabbCheck(const hkFourTransposedPoints& orig, const hkFourTransposedPoints& invDir, hkVector4Parameter _tNear, hkVector4Parameter _tFar, const hkVector4* aabbData)
{
	hkVector4 tNear = _tNear;
	hkVector4 tFar = _tFar;

	hkVector4Comparison flatOrigOK; flatOrigOK.set(hkVector4Comparison::MASK_XYZW);

	for (int i=0; i<3; i++)
	{
		hkVector4 aabbMin, aabbMax;
		aabbMin = *aabbData++;
		aabbMax = *aabbData++;

		hkVector4 dNear, dFar, dNearTemp, dFarTemp;
		dNearTemp.setSub(aabbMin, orig.m_vertices[i]);
		dFarTemp.setSub( aabbMax, orig.m_vertices[i]);
		dNearTemp.mul(invDir.m_vertices[i]);
		dFarTemp.mul( invDir.m_vertices[i]);

		hkVector4 zero; zero.setZero();
		hkVector4Comparison dirLtZero = invDir.m_vertices[ i ].greaterEqual(zero);
		hkVector4Comparison dirEqZero = invDir.m_vertices[ i ].equal(zero);


		// Check that the origins in flat directions (dir=0) are within the AABB in that direction
		// OK = OK && ( (!flat) || (flat && inRange) )
		//    = OK && select(true, inRange, flat)
		//    = OK && select(OK, inRange, flat)

		hkVector4Comparison inRange;
		inRange = orig.m_vertices[i].lessEqual(aabbMax);
		inRange.setAnd(inRange, orig.m_vertices[i].greaterEqual(aabbMin) );
		hkVector4Comparison thisComponentIsOk; thisComponentIsOk.setSelect(dirEqZero, inRange, flatOrigOK);
		flatOrigOK.setAnd(flatOrigOK, thisComponentIsOk);

		dNear.setSelect(dirLtZero, dNearTemp, dFarTemp);
		dFar.setSelect( dirLtZero, dFarTemp, dNearTemp);

		// If the ray is flat in this direction, effectively skip the direction
		dNear.setSelect(dirEqZero, tNear, dNear);
		dFar.setSelect( dirEqZero, tFar, dFar);

		tNear.setMax(dNear, tNear);
		tFar.setMin(dFar, tFar);
	}
	//return tNear.compareLessThan(tFar);

	// The ray hits if tNear<tFar and the origins are within the proper ranges
	hkVector4Comparison tOk = tNear.less(tFar);
	tOk.setAnd(tOk, flatOrigOK);
	return tOk;
}


inline void hkKdTreeMath::getAabbFromProjectedEntry(const hkKdTreeProjectedEntry* aabbData, hkAabb& aabb, hkVector4Parameter offset, hkVector4Parameter _scale)
{
	hkVector4 scale; hkKdTreeMath::setInverseScale(scale);
	scale.mul(_scale); 

	hkVector4 min, max;
	convertProjectedEntryToHkVector4(aabbData, min, max);
	aabb.m_min.setAddMul(offset, scale, min); // offset + (scale*min)
	aabb.m_max.setAddMul(offset, scale, max);

	// isEmpty means that max < min, which happens when the primitive is invalidated
	HK_ASSERT(0x781378a0, aabb.isValid() || aabb.isEmpty() );
}

// Vectors are output in the order minX, maxX, minY, maxY, minZ, maxZ
inline void hkKdTreeMath::getAabbVecsFromProjectedEntry(const hkKdTreeProjectedEntry* aabbData, hkVector4* HK_RESTRICT vecsOut, hkVector4Parameter offset, hkVector4Parameter scaleIn)
{
	hkVector4 scale; hkKdTreeMath::setInverseScale(scale);
	scale.mul(scaleIn);

	hkVector4 min, max;
	convertProjectedEntryToHkVector4(aabbData, min, max);

	min.setAddMul(offset, scale, min); // offset + (scale*min)
	max.setAddMul(offset, scale, max);

	vecsOut[0].setBroadcast<0>(min);
	vecsOut[1].setBroadcast<0>(max);

	vecsOut[2].setBroadcast<1>(min);
	vecsOut[3].setBroadcast<1>(max);

	vecsOut[4].setBroadcast<2>(min);
	vecsOut[5].setBroadcast<2>(max);
}

inline void hkKdTreeMath::convertProjectedEntryToHkVector4(const hkKdTreeProjectedEntry* splits, hkVector4& v1, hkVector4& v2)
{
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED) && (HK_POINTER_SIZE == 4)

	// The compressed AABBs are always 16-byte aligned
	HK_ASSERT(0x4f8286ad, ((hkUlong)splits & 0xf) == 0);

	hkIntVector b = *(hkIntVector*)splits; // b = xxyy zz00 XXYY ZZ00
	hkIntVector zero; zero.setZero();

	hkIntVector minUint, maxUint;
	minUint.setCombineHead16To32(zero, b); // minUint = 00xx 00yy 00zz 0000
	maxUint.setCombineTail16To32(zero, b);  // maxUint = 00XX 00YY 00ZZ 0000

	minUint.convertU32ToF32(v1);
	maxUint.convertU32ToF32(v2);

#else

	v1.set(hkReal(splits->m_min[0]), hkReal(splits->m_min[1]), hkReal(splits->m_min[2]), 0.0f);
	v2.set(hkReal(splits->m_max[0]), hkReal(splits->m_max[1]), hkReal(splits->m_max[2]), 0.0f);
#endif
}

// Leave an extra line at the EOF

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
