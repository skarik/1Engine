/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */




HK_FORCE_INLINE void HK_CALL hkSweptTransformUtil::_lerp2( const hkSweptTransform& sweptTrans, hkReal t, hkQuaternion& quatOut )
{
	hkVector4 rot0 = sweptTrans.m_rotation0.m_vec;
	hkVector4 rot1 = sweptTrans.m_rotation1.m_vec;
	hkSimdReal st; st.setFromFloat(t);
	{
		quatOut.m_vec.setAdd( rot0, rot1 );
		const hkSimdReal len2 = quatOut.m_vec.lengthSquared<4>();
		hkSimdReal threequarter; threequarter.setFromFloat(0.75f);
		hkSimdReal oneeighth; oneeighth.setFromFloat(0.125f);
		hkSimdReal oneandhalf; oneandhalf.setFromFloat(1.5f);
		hkSimdReal r = threequarter - oneeighth * len2;
		r = r * (oneandhalf - hkSimdReal::getConstant<HK_QUADREAL_INV_2>() * len2 * r * r);
		quatOut.m_vec.mul(r);
	}

	if ( st < hkSimdReal::getConstant<HK_QUADREAL_INV_2>() )
	{
		quatOut.m_vec.setInterpolate( rot0, quatOut.m_vec, st * hkSimdReal::getConstant<HK_QUADREAL_2>());
	}
	else
	{
		quatOut.m_vec.setInterpolate( quatOut.m_vec, rot1, (hkSimdReal::getConstant<HK_QUADREAL_2>() * st) - hkSimdReal::getConstant<HK_QUADREAL_1>());
	}
	quatOut.normalize();
}



HK_FORCE_INLINE void HK_CALL hkSweptTransformUtil::_lerp2( const hkSweptTransform& sweptTrans, hkReal t, hkTransform& transformOut )
{
	hkQuaternion qt;
	_lerp2( sweptTrans, t, qt );

	transformOut.setRotation( qt );
	hkSimdReal st; st.setFromFloat(t);
	transformOut.getTranslation().setInterpolate( sweptTrans.m_centerOfMass0, sweptTrans.m_centerOfMass1, st);

	hkVector4 centerShift;
	centerShift._setRotatedDir( transformOut.getRotation(), sweptTrans.m_centerOfMassLocal);
	transformOut.getTranslation().sub( centerShift );
}


HK_FORCE_INLINE void HK_CALL hkSweptTransformUtil::calcTransAtT0( const hkSweptTransform& sweptTrans, hkTransform& transformOut )
{
	const hkQuaternion& qt = sweptTrans.m_rotation0;

	transformOut.setRotation( qt );

	hkVector4 centerShift;
	centerShift._setRotatedDir( transformOut.getRotation(), sweptTrans.m_centerOfMassLocal);
	transformOut.getTranslation().setSub( sweptTrans.m_centerOfMass0, centerShift );
}


HK_FORCE_INLINE void HK_CALL hkSweptTransformUtil::calcTransAtT1( const hkSweptTransform& sweptTrans, hkTransform& transformOut )
{
	const hkQuaternion& qt = sweptTrans.m_rotation1;

	transformOut.setRotation( qt );

	hkVector4 centerShift;
	centerShift._setRotatedDir( transformOut.getRotation(), sweptTrans.m_centerOfMassLocal);
	transformOut.getTranslation().setSub( sweptTrans.m_centerOfMass1, centerShift );
}

HK_FORCE_INLINE void HK_CALL hkSweptTransformUtil::_clipVelocities( const hkMotionState& motionState, hkVector4& linearVelocity, hkVector4& angularVelocity )
{
	hkReal linVelSq = linearVelocity.lengthSquared<3>().getReal();
	hkReal angVelSq = angularVelocity.lengthSquared<3>().getReal();
	const hkReal maxLinear = motionState.m_maxLinearVelocity * motionState.m_maxLinearVelocity;
	const hkReal maxAngularRadians = motionState.m_maxAngularVelocity * motionState.m_maxAngularVelocity;
	if ( (linVelSq > maxLinear) || (linVelSq!=linVelSq) )
	{
		hkSimdReal f; f.setFromFloat( motionState.m_maxLinearVelocity * hkMath::sqrtInverse( linVelSq ) );
		linearVelocity.mul( f );
		if ( linVelSq!=linVelSq )
		{
			linearVelocity = hkVector4::getConstant<HK_QUADREAL_1000>();
			HK_ASSERT2(0xf0124242, false, "NaN velocity detected, something is seriously wrong (bad inertia tensors?)");
		}
	}
	if ((angVelSq > maxAngularRadians) || ( angVelSq != angVelSq ))
	{
		hkSimdReal f; f.setFromFloat( motionState.m_maxAngularVelocity * hkMath::sqrtInverse( angVelSq ) );
		angularVelocity.mul( f );
		if ( angVelSq!=angVelSq )
		{
			angularVelocity = hkVector4::getConstant<HK_QUADREAL_1000>();
			HK_ASSERT2(0xf0143243, false, "NaN velocity detected, something is seriously wrong (bad inertia tensors?)");
		}
	}
}


HK_FORCE_INLINE void HK_CALL hkSweptTransformUtil::_stepMotionState( const hkStepInfo& info,
											 hkVector4& linearVelocity, hkVector4& angularVelocity,
											 hkMotionState& motionState )
{
	hkSimdReal	deltaTime; deltaTime.setFromFloat( info.m_deltaTime.val() * motionState.m_timeFactor );
#ifdef HK_DEBUG
	{
		if ( motionState.getSweptTransform().getInvDeltaTime() != hkReal(0))
		{
			hkReal motionEndTime = motionState.getSweptTransform().getBaseTime() + hkReal(1) / motionState.getSweptTransform().getInvDeltaTime();
			HK_ASSERT(0xf0f0083, hkMath::equal(info.m_startTime, motionEndTime ) ) ;
		}
	}
#endif
	// check for nans in velocities
	// fix nans to (1,0,0)
	{
		hkVector4 absLin; absLin.setAbs( linearVelocity );
		hkVector4 absAng; absAng.setAbs( angularVelocity );

		hkVector4 maxVel; maxVel.m_quad = _stepMotionStateMaxVelf;

		hkVector4Comparison isNormal = absLin.less( maxVel );
		isNormal.setAnd( isNormal, absAng.less( maxVel ) );
		isNormal.setAnd( isNormal, linearVelocity.equal( linearVelocity ) );
		isNormal.setAnd( isNormal, angularVelocity.equal( angularVelocity ) );

		if ( !isNormal.allAreSet(hkVector4Comparison::MASK_XYZ) )
		{
			// velocity to a 'random' non zero velocity
			linearVelocity  = hkTransform::getIdentity().getColumn<0>();
			angularVelocity = hkTransform::getIdentity().getColumn<0>();
		}
	}

	motionState.getSweptTransform().m_centerOfMass0 = motionState.getSweptTransform().m_centerOfMass1;
	motionState.getSweptTransform().m_centerOfMass0(3) = info.m_startTime.val();

	hkReal linVelSq = linearVelocity.lengthSquared<3>().getReal();
	if ( linVelSq > motionState.m_maxLinearVelocity * motionState.m_maxLinearVelocity )
	{
		//HK_WARN_ONCE(0xf0327683, "Object exceeding maximum velocity, velocity clipped" );
		hkSimdReal f; f.setFromFloat( motionState.m_maxLinearVelocity * hkMath::sqrtInverse( linVelSq ) );
		linearVelocity.mul( f );
	}

	motionState.getSweptTransform().m_centerOfMass1.addMul(deltaTime, linearVelocity);
	motionState.getSweptTransform().m_centerOfMass1(3) = info.m_invDeltaTime.val();

	hkQuaternion newRotation = motionState.getSweptTransform().m_rotation1;
	motionState.getSweptTransform().m_rotation0 = newRotation;

	//
	//	Calculate a new rotation, the fabs angle and angle squared
	//
	hkReal angle;
	{
		hkQuaternion rotationRadians;	rotationRadians.m_vec.setMul( deltaTime * hkSimdReal_Inv2 , angularVelocity );
		const hkReal pi = HK_REAL_PI;
		hkReal numHalfRotations2 = rotationRadians.m_vec.lengthSquared<3>().getReal() * (hkReal(4) / (pi*pi));

		
		const hkReal a = hkReal(0.822948f);
		const hkReal b = hkReal(0.130529f);
		const hkReal c = hkReal(0.044408f);

		const hkReal maxAngleHalfRotations = hkMath::min2( hkReal(0.9f), ( ( deltaTime.getReal() * hkReal(motionState.m_maxAngularVelocity) ) / pi ) );

		// clipping angular velocity to be between [0, PI*0.9/dt]
		// info: was "<", is "<=" -- works ok for zero dt now.
		if ( numHalfRotations2 <= maxAngleHalfRotations * maxAngleHalfRotations )
		{
			const hkReal numHalfRotations4 = numHalfRotations2 * numHalfRotations2;
			const hkReal w = hkReal(1) - a * numHalfRotations2 - b * numHalfRotations4 - c * numHalfRotations2 * numHalfRotations4;
			rotationRadians.m_vec(3) = w;
		}
		else
		{
			hkSimdReal factor; factor.setFromFloat(maxAngleHalfRotations * hkMath::sqrtInverse( numHalfRotations2 ));
			angularVelocity.mul( factor );
			rotationRadians.m_vec.mul( factor );

			numHalfRotations2  = maxAngleHalfRotations * maxAngleHalfRotations;
			const hkReal numHalfRotations4  = numHalfRotations2 * numHalfRotations2;
			const hkReal w = hkReal(1) - a * numHalfRotations2 - b * numHalfRotations4 - c * numHalfRotations2 * numHalfRotations4;
			rotationRadians.m_vec(3) = w;
		}

		newRotation.setMul( rotationRadians, newRotation );
		newRotation.normalize();

		motionState.m_deltaAngle.setAdd( rotationRadians.m_vec, rotationRadians.m_vec );
		angle = hkMath::sqrt( numHalfRotations2 ) * pi;
		motionState.m_deltaAngle(3) = angle;
	}
	motionState.getSweptTransform().m_rotation1 = newRotation;
	calcTransAtT1( motionState.getSweptTransform(), motionState.getTransform());
}







HK_FORCE_INLINE void HK_CALL hkSweptTransformUtil::deactivate( hkMotionState& ms )
{
	hkSweptTransform& sweptTransform = ms.getSweptTransform();
	ms.m_deltaAngle.setZero();

	sweptTransform.m_rotation0 = sweptTransform.m_rotation1;
	sweptTransform.m_centerOfMass0 = sweptTransform.m_centerOfMass1;
	sweptTransform.m_centerOfMass1.zeroComponent<3>();
}



//HK_FORCE_INLINE void HK_CALL hkSweptTransformUtil::calcTimInfo( const hkMotionState& ms0, const hkMotionState& ms1, hkVector4& timOut)
//{
//	HK_ASSERT2(0xad44d321, st0.getInvDeltaTime() == st1.getInvDeltaTime(), "Internal error: hkSweptTransform's must correspond to the same deltaTime in order to use void HK_CALL hkSweptTransformUtil::calcTimInfo( const hkMotionState& ms0, const hkMotionState& ms1, hkVector4& timOut)");
//
//	const hkSweptTransform& st0 = ms0.getSweptTransform();
//	const hkSweptTransform& st1 = ms1.getSweptTransform();
//
//	hkVector4 diff0; diff0.setSub( st0.m_centerOfMass0, st0.m_centerOfMass1 );
//	hkVector4 diff1; diff1.setSub( st1.m_centerOfMass1, st1.m_centerOfMass0 );
//
//	timOut.setAdd( diff0, diff1 );
//
//	timOut(3) = ms0.m_deltaAngle(3) * ms0.m_objectRadius + ms1.m_deltaAngle(3) * ms1.m_objectRadius;
//
//}

HK_FORCE_INLINE void HK_CALL hkSweptTransformUtil::calcTimInfo( const hkMotionState& ms0, const hkMotionState& ms1, hkReal deltaTime, hkVector4& timOut)
{
	const hkSweptTransform& st0 = ms0.getSweptTransform();
	const hkSweptTransform& st1 = ms1.getSweptTransform();

	hkVector4 diff0; diff0.setSub( st0.m_centerOfMass0, st0.m_centerOfMass1 );
	hkVector4 diff1; diff1.setSub( st1.m_centerOfMass1, st1.m_centerOfMass0 );

	hkReal f0 = deltaTime * st0.getInvDeltaTime();
	hkReal f1 = deltaTime * st1.getInvDeltaTime();

	HK_ASSERT2(0xad56daaa, f0 <= hkReal(1.01f) && f1 <= hkReal(1.01f), "Internal error: input for TIM calculation may be corrupted.");

	hkSimdReal sf0; sf0.setFromFloat(f0);
	hkSimdReal sf1; sf1.setFromFloat(f1);
	timOut.setMul( sf0, diff0 );
	timOut.addMul( sf1, diff1 );
	timOut(3) = f0 * ms0.m_deltaAngle(3) * ms0.m_objectRadius  + f1 * ms1.m_deltaAngle(3) * ms1.m_objectRadius;

	// we don't project angular velocity just to keep it simple ( no cross products)
}

	// Calculates angular distance (angVelocity * dt) travelled by the bodies. 
HK_FORCE_INLINE void HK_CALL hkSweptTransformUtil::calcAngularTimInfo( const hkMotionState& ms0, const hkMotionState& ms1, hkReal deltaTime, hkVector4* HK_RESTRICT deltaAngleOut0, hkVector4* HK_RESTRICT deltaAngleOut1)
{
	const hkSweptTransform& st0 = ms0.getSweptTransform();
	const hkSweptTransform& st1 = ms1.getSweptTransform();
	hkReal f0 = deltaTime * st0.getInvDeltaTime();
	hkReal f1 = deltaTime * st1.getInvDeltaTime();
	hkSimdReal sf0; sf0.setFromFloat(f0);
	hkSimdReal sf1; sf1.setFromFloat(f1);

	hkVector4 ang0; ang0.setMul( sf0, ms0.m_deltaAngle );
	hkVector4 ang1; ang1.setMul( sf1, ms1.m_deltaAngle );

	deltaAngleOut0[0] = ang0;
	deltaAngleOut1[0] = ang1;
}

HK_FORCE_INLINE void HK_CALL hkSweptTransformUtil::calcCenterOfMassAt( const hkMotionState& ms, hkTime t, hkVector4& centerOut )
{
	hkSimdReal iv; iv.setFromFloat( ms.getSweptTransform().getInterpolationValue( t ) );
	centerOut.setInterpolate( ms.getSweptTransform().m_centerOfMass0, ms.getSweptTransform().m_centerOfMass1, iv );
}

HK_FORCE_INLINE void HK_CALL hkSweptTransformUtil::getVelocity( const hkMotionState& ms, hkVector4& linearVelOut, hkVector4& angularVelOut )
{
	linearVelOut.setSub (ms.getSweptTransform().m_centerOfMass1, ms.getSweptTransform().m_centerOfMass0);
	hkSimdReal idt; idt.setFromFloat(ms.getSweptTransform().getInvDeltaTime());
	linearVelOut.mul( idt );
	angularVelOut.setMul( idt, ms.m_deltaAngle );
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
