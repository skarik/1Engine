/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_DYNAMICS2_INTERNAL_CONSTRAINT_UTILS_H
#define HK_DYNAMICS2_INTERNAL_CONSTRAINT_UTILS_H

extern "C"
{
	/// Calculates the relative angle between twist axes given plane axis
	void HK_CALL hkInternalConstraintUtils_calcRelativeAngle( 
		const hkVector4& twistAxisAinWorld, const hkVector4& twistAxisBinWorld, 
		const hkVector4& planeAxisAinWorld, const hkVector4& planeAxisBinWorld,
		hkVector4& axisOut, hkPadSpu<hkReal>& angleOut );
}


HK_FORCE_INLINE void HK_CALL hkInternalConstraintUtils_inlineCalcRelativeAngle( 
	const hkVector4& twistAxisAinWorld, const hkVector4& twistAxisBinWorld, 
	const hkVector4& planeAxisAinWorld, const hkVector4& planeAxisBinWorld,
	hkVector4& axisOut, hkPadSpu<hkReal>& angleOut, hkPadSpu<hkReal>& tauMaxOut ) 
{
	// Note : This algorithm is replicated inside the physics tools (max/maya)
	//		  Any changes should be ported there also.

	hkVector4 twist_axis_ws;	twist_axis_ws.setAdd( twistAxisBinWorld , twistAxisAinWorld );

	const hkSimdReal twist_axis_ws_length3 = twist_axis_ws.length<3>();
	if (twist_axis_ws_length3 > hkSimdReal::getConstant<HK_QUADREAL_EPS_SQRD>() )
	{
		hkSimdReal tauMax = twist_axis_ws_length3 * hkSimdReal::getConstant<HK_QUADREAL_INV_2>();
		tauMax.store<1>((hkReal*)&tauMaxOut);

		hkSimdReal invF; invF.setReciprocal(twist_axis_ws_length3);
		twist_axis_ws.mul(invF);
	}
	else
	{
		tauMaxOut = 0.0f;
		twist_axis_ws = twistAxisBinWorld;
	}

	hkRotation m_ws_us;
	{
		hkVector4& c0 = m_ws_us.getColumn(0);
		hkVector4& c1 = m_ws_us.getColumn(1);
		hkVector4& c2 = m_ws_us.getColumn(2);
		c0 = twist_axis_ws;
		c1.setCross(twist_axis_ws, planeAxisBinWorld);
		c2.setCross(c1, c0);
	}
		
	hkSimdReal d1 = m_ws_us.getColumn(1).dot<3>( planeAxisAinWorld );
	hkSimdReal d2 = m_ws_us.getColumn(2).dot<3>( planeAxisAinWorld );
	hkReal currentTwistAngle  = hkMath::atan2fApproximation( d1.getReal(), d2.getReal());
		// Explicitly specify angle calculated above. 
	angleOut = currentTwistAngle;
	axisOut = twist_axis_ws;
}

//
//	Computes the inertia scale factors for a linear constraint. These are used to make the constraint more stable
//	by scaling down the angular parts of the virtual mass.

HK_FORCE_INLINE void HK_CALL hkInternalConstraintUtils_computeInertiaScaleFactors(	const hkVector4& invInertiaA, const hkVector4& invInertiaB, 
																					const hkSimdReal& armLenA, const hkSimdReal& armLenB,
																					const hkSimdReal& scaleFactor,
																					hkSimdReal& fOutA, hkSimdReal& fOutB)
{
	const hkSimdReal maxInvInertiaA = invInertiaA.horizontalMax<3>();
	const hkSimdReal maxInvInertiaB = invInertiaB.horizontalMax<3>();

	hkSimdReal fA = maxInvInertiaA * armLenA * scaleFactor;	// For a given impulse, the angular velocity of body A is proportional to this term
	hkSimdReal fB = maxInvInertiaB * armLenB * scaleFactor;	// For a given impulse, the angular velocity of body B is proportional to this term

	const hkSimdReal eps = hkSimdReal::getConstant<HK_QUADREAL_EPS>();
	const hkVector4Comparison cmpA = fA.greater(eps);
	const hkVector4Comparison cmpB = fB.greater(eps);
	fA.setReciprocal(fA);
	fB.setReciprocal(fB);
	fA = fA * invInertiaA.getComponent<3>();
	fB = fB * invInertiaB.getComponent<3>();

	const hkSimdReal one = hkSimdReal::getConstant<HK_QUADREAL_1>();
	fA.setSelect(cmpA, fA, one);
	fB.setSelect(cmpB, fB, one);

	// Choose sub-unitary factors. If > 1 we are not modifying the problem
	// Artifacts appear if we use the same factor for both bodies!!
	fA.setMin(fA, one);
	fB.setMin(fB, one);
	fOutA = fA * armLenA;
	fOutB = fB * armLenB;
}

#endif // HK_DYNAMICS2_INTERNAL_CONSTRAINT_UTILS_H

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
