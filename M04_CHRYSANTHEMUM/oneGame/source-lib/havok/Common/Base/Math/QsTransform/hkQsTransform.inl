/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

HK_FORCE_INLINE void hkQsTransform::setIdentity()
{
	m_translation.setZero();
	m_rotation.setIdentity();
	m_scale = hkVector4::getConstant<HK_QUADREAL_1>();
}

HK_FORCE_INLINE void hkQsTransform::setZero()
{
	m_translation.setZero();
	m_rotation.m_vec.setZero();
	m_scale.setZero();
}

HK_FORCE_INLINE const hkVector4& hkQsTransform::getTranslation() const
{
	return m_translation;
}

HK_FORCE_INLINE void hkQsTransform::setTranslation(hkVector4Parameter t)
{
	m_translation = t;
}

HK_FORCE_INLINE const hkQuaternion& hkQsTransform::getRotation() const
{
	return m_rotation;
}

HK_FORCE_INLINE void hkQsTransform::setRotation(const hkRotation& rotation)
{	
	m_rotation.set(rotation);
}

HK_FORCE_INLINE void hkQsTransform::setRotation(hkQuaternionParameter rotation)
{
	m_rotation = rotation;
}

HK_FORCE_INLINE const hkVector4& hkQsTransform::getScale() const
{
	return m_scale;
}

HK_FORCE_INLINE void hkQsTransform::setScale(hkVector4Parameter s)
{
	m_scale = s;
}

#ifndef HK_DISABLE_MATH_CONSTRUCTORS

HK_FORCE_INLINE hkQsTransform::hkQsTransform(const hkQsTransform& t)
{
	m_translation = t.m_translation;
	m_rotation = t.m_rotation;
	m_scale = t.m_scale;
}

HK_FORCE_INLINE hkQsTransform::hkQsTransform( IdentityInitializer /*init*/ )
{
	setIdentity();
}

HK_FORCE_INLINE hkQsTransform::hkQsTransform( ZeroInitializer /*init*/ )
{
	setZero();
}

HK_FORCE_INLINE hkQsTransform::hkQsTransform(hkVector4Parameter translation, hkQuaternionParameter rotation, hkVector4Parameter scale)
:	m_translation(translation), m_rotation(rotation), m_scale(scale)
{
}

HK_FORCE_INLINE hkQsTransform::hkQsTransform(hkVector4Parameter translation, hkQuaternionParameter rotation)
:	m_translation(translation), m_rotation(rotation)
{
	m_scale = hkVector4::getConstant<HK_QUADREAL_1>();
}

#endif

HK_FORCE_INLINE void hkQsTransform::set(hkVector4Parameter translation, hkQuaternionParameter rotation, hkVector4Parameter scale)
{
	m_translation = translation;
	m_rotation = rotation;
	m_scale = scale;
}

HK_FORCE_INLINE void hkQsTransform::set(hkVector4Parameter translation, hkQuaternionParameter rotation)
{
	m_translation = translation;
	m_rotation = rotation;
	m_scale = hkVector4::getConstant<HK_QUADREAL_1>();
}

HK_FORCE_INLINE /*static*/ const hkQsTransform& HK_CALL hkQsTransform::getIdentity()
{
	extern hkReal hkQsTransform_identityStorage[];
	union { const hkReal* r; const hkQsTransform* q; } r2q;
	r2q.r = hkQsTransform_identityStorage;
	return *r2q.q;
}

HK_FORCE_INLINE void hkQsTransform::setInverse( const hkQsTransform& t )
{
	// We are not "reference-safe" because quaternion inversion is not reference-safe
	HK_ASSERT2(0x1acceb8d, (&t != this) , "Using unsafe references in math operation");

	m_translation.setRotatedInverseDir(t.m_rotation, t.m_translation);
	m_translation.setNeg<4>(m_translation);
	m_rotation.setInverse(t.m_rotation);

	m_scale.setReciprocal(t.m_scale); 
	
	m_scale.zeroComponent<3>();
}

HK_FORCE_INLINE void hkQsTransform::setInterpolate4( const hkQsTransform& a, const hkQsTransform& b, hkSimdRealParameter t)
{
	m_scale.setInterpolate(a.m_scale, b.m_scale, t);
	m_translation.setInterpolate(a.m_translation, b.m_translation, t);

	const hkSimdReal oneMinusT = hkSimdReal::getConstant<HK_QUADREAL_1>() - t;

	// Check quaternion polarity
	hkVector4Comparison lessZero = a.m_rotation.m_vec.dot<4>(b.m_rotation.m_vec).lessZero();
	hkSimdReal signedT; signedT.setSelect(lessZero, -t, t);

	hkQuaternion rot;
	rot.m_vec.setMul( signedT, b.m_rotation.m_vec);
	rot.m_vec.addMul( oneMinusT, a.m_rotation.m_vec);
	rot.normalize();

	m_rotation = rot;
}

/*
** Blending stuff
*/

HK_FORCE_INLINE void hkQsTransform::blendAddMul(const hkQsTransform& other, hkSimdRealParameter weight)
{
	m_translation.addMul(weight, other.m_translation);
	m_scale.addMul(weight, other.m_scale);

	// Check quaternion polarity
	const hkVector4Comparison lessZero = m_rotation.m_vec.dot<4>(other.m_rotation.m_vec).lessZero();
	hkVector4 negRotation; negRotation.setNeg<4>(other.m_rotation.m_vec);
	hkVector4 polarityRotation; polarityRotation.setSelect( lessZero, negRotation, other.m_rotation.m_vec );
	m_rotation.m_vec.addMul(weight, polarityRotation);
}

HK_FORCE_INLINE void hkQsTransform::blendAddMul(const hkQsTransform& other)
{
	// Weight = 1
	m_translation.add(other.m_translation);
	m_scale.add(other.m_scale);

	// Check quaternion polarity
	const hkVector4Comparison lessZero = m_rotation.m_vec.dot<4>(other.m_rotation.m_vec).lessZero();
	hkVector4 negRotation; negRotation.setNeg<4>(other.m_rotation.m_vec);
	hkVector4 polarityRotation; polarityRotation.setSelect( lessZero, negRotation, other.m_rotation.m_vec );
	m_rotation.m_vec.add(polarityRotation);
}

HK_FORCE_INLINE void hkQsTransform::blendWeight(hkSimdRealParameter weight )
{
	m_translation.mul(weight);
	m_scale.mul(weight);
	m_rotation.m_vec.mul(weight);
}

HK_FORCE_INLINE void hkQsTransform::blendNormalize( hkSimdRealParameter totalWeight )
{
	const hkSimdReal eps = hkSimdReal::getConstant<HK_QUADREAL_EPS>();

	// If weight is almost zero, blend is identity
	hkSimdReal absWeight; absWeight.setAbs(totalWeight);
	if (absWeight < eps )
	{
		setIdentity();
		return;
	}

	// Weight all accumulators by inverse 
	{
		hkSimdReal invWeight; invWeight.setReciprocal(totalWeight);

		m_translation.mul(invWeight);

		m_scale.mul(invWeight);
	}

	// Now, check for the special cases

	// Rotation
	{
		const hkSimdReal quatNorm = m_rotation.m_vec.lengthSquared<4>();
		if (quatNorm < eps)
		{
			// no rotations blended (or canceled each other) -> set to identity
			m_rotation.setIdentity();
		}
		else
		{
			// normalize
			m_rotation.normalize();
		}
	}

	// Scale
	{
		// no scale blended (or scale canceled each other) -> set to identity
		const hkSimdReal scaleNorm = m_scale.lengthSquared<3>();
		const hkVector4Comparison smallScale = scaleNorm.less(eps);
		m_scale.setSelect(smallScale, hkVector4::getConstant<HK_QUADREAL_1>(), m_scale);
	}
}

HK_FORCE_INLINE void hkQsTransform::blendNormalize()
{
	hkSimdReal weight = hkSimdReal::getConstant(HK_QUADREAL_1);
	blendNormalize(weight);
	const hkSimdReal eps = hkSimdReal::getConstant<HK_QUADREAL_EPS>();

	// Weight = 1, so just check for the special cases

	// Rotation
	{
		const hkSimdReal quatNorm = m_rotation.m_vec.lengthSquared<4>();
		if (quatNorm < eps)
		{
			// no rotations blended (or canceled each other) -> set to identity
			m_rotation.setIdentity();
		}
		else
		{
			// normalize
			m_rotation.normalize();
		}
	}

	// Scale
	{
		// no scale blended (or scale canceled each other) -> set to identity
		const hkSimdReal scaleNorm = m_scale.lengthSquared<3>();
		const hkVector4Comparison smallScale = scaleNorm.less(eps);
		m_scale.setSelect(smallScale, hkVector4::getConstant<HK_QUADREAL_1>(), m_scale);
	}
}

HK_FORCE_INLINE void hkQsTransform::fastRenormalize( hkSimdRealParameter totalWeight )
{
	hkSimdReal invWeight; invWeight.setReciprocal(totalWeight);

	m_translation.mul(invWeight);

	m_scale.mul(invWeight);

	m_rotation.normalize();
}

HK_FORCE_INLINE void hkQsTransform::fastRenormalize()
{
	hkSimdReal weight = hkSimdReal::getConstant(HK_QUADREAL_1);
	fastRenormalize(weight);
	// Weight = 1
	m_rotation.normalize();
}

// aTc = aTb * bTc
HK_FORCE_INLINE void hkQsTransform::setMul( const hkQsTransform& aTb, const hkQsTransform& bTc )
{
	// Rotation and position go together
	hkVector4 extraTrans;	
	extraTrans._setRotatedDir(aTb.m_rotation, bTc.m_translation);
	m_translation.setAdd(aTb.m_translation, extraTrans);
	m_rotation.setMul(aTb.m_rotation, bTc.m_rotation);

	// Scale goes apart
	m_scale.setMul(aTb.m_scale, bTc.m_scale);
}

// T3 = T1 * T2
HK_FORCE_INLINE void hkQsTransform::setMulScaled( const hkQsTransform& T1, const hkQsTransform& T2 )
{
	// basic affine transform:
	//   T(x) = RSx+t
	//
	// composite transform:
	//    T1(T2(x)) = R1 S1(R2 S2 x + t2) + t1 
	//              = R1 S1 R2 S2 x + R1 S1 t2 + t1
	//                -----\/----     ------\/-----
	//                    R3S3              t3
	// If S1 only contains uniform scaling, skewing is 
	// avoided and the new transform becomes R1 R2 S1 S2. 
	// We assume S1 is uniform scaling, even though it may
	// not be.

	// translational part of new transform
	hkVector4 translation;
	translation.setMul(T1.m_scale,T2.m_translation);
	translation._setRotatedDir(T1.m_rotation, translation);
	translation.add(T1.m_translation);

	// rotational part
	hkQuaternion rotation;
	rotation.setMul(T1.m_rotation, T2.m_rotation);

	// scaling part (assume T1 is uniform scaling)
	hkVector4 scale;
	scale.setMul(T1.m_scale, T2.m_scale);

	m_translation = translation;
	m_rotation = rotation;
	m_scale = scale;
}

HK_FORCE_INLINE void hkQsTransform::setMulInverseMul( const hkQsTransform& a, const hkQsTransform &b )
{
	hkQsTransform h;
	h.setInverse( a );
	setMul(h, b);
}

HK_FORCE_INLINE void hkQsTransform::setMulMulInverse( const hkQsTransform &a, const hkQsTransform &b )
{
	hkQsTransform h;
	h.setInverse( b );
	setMul( a, h);	
}

HK_FORCE_INLINE void hkQsTransform::setMulEq( const hkQsTransform& b )
{
	// We are not "reference-safe" because quaternion multiplication is not reference-safe
	HK_ASSERT2(0x1acceb8d, (&b!=this) , "Using unsafe references in math operation");

	// Rotation and position go together
	hkVector4 extraTrans;
	extraTrans._setRotatedDir(m_rotation, b.m_translation);
	m_translation.add(extraTrans);
	m_rotation.mul(b.m_rotation);

	// Scale goes apart
	m_scale.mul(b.m_scale);
}

HK_FORCE_INLINE hkBool32 hkQsTransform::isScaleUniform() const
{
	hkVector4 scaleX; scaleX.setAll(m_scale.getComponent<0>());
	return m_scale.allExactlyEqual<3>(scaleX);
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
