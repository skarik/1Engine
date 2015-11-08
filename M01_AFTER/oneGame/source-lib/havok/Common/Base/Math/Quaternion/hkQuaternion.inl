/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// construction

HK_FORCE_INLINE const hkVector4& hkQuaternion::getImag() const
{
	return m_vec;
}

HK_FORCE_INLINE void hkQuaternion::setImag(hkVector4Parameter i)
{
	m_vec.setXYZ(i);
}


HK_FORCE_INLINE hkReal hkQuaternion::getReal() const
{
	return m_vec(3);
}

HK_FORCE_INLINE void hkQuaternion::setReal(hkReal r)
{
	m_vec(3) = r;
}

HK_FORCE_INLINE const hkSimdReal hkQuaternion::getRealPart() const
{
	return m_vec.getComponent<3>();
}

HK_FORCE_INLINE void hkQuaternion::setRealPart(hkSimdRealParameter r)
{
	m_vec.setComponent<3>(r);
}

#ifndef HK_DISABLE_MATH_CONSTRUCTORS
HK_FORCE_INLINE hkQuaternion::hkQuaternion(const hkRotation& r)
{
	set(r);
}

HK_FORCE_INLINE hkQuaternion::hkQuaternion(hkReal x, hkReal y, hkReal z, hkReal w)
{
	set(x,y,z,w);
}

HK_FORCE_INLINE hkQuaternion::hkQuaternion(hkSimdRealParameter x, hkSimdRealParameter y, hkSimdRealParameter z, hkSimdRealParameter w)
{
	set(x,y,z,w);
}

HK_FORCE_INLINE hkQuaternion::hkQuaternion(hkVector4Parameter axis, hkReal angle)
{
	setAxisAngle(axis,angle);
}
#endif

HK_FORCE_INLINE void hkQuaternion::operator= (const hkQuaternion& q)
{
	m_vec = q.m_vec;
}

HK_FORCE_INLINE void hkQuaternion::set(hkReal x, hkReal y, hkReal z, hkReal w)
{
	m_vec.set(x,y,z,w);
	HK_ASSERT2(0x1adaad0e,  isOk(), "hkQuaternion components were not normalized." );
}

HK_FORCE_INLINE void hkQuaternion::set(hkSimdRealParameter x, hkSimdRealParameter y, hkSimdRealParameter z, hkSimdRealParameter w)
{
	m_vec.set(x,y,z,w);
	HK_ASSERT2(0x1adaad0e,  isOk(), "hkQuaternion components were not normalized." );
}

HK_FORCE_INLINE void hkQuaternion::setIdentity()
{
	m_vec = hkVector4::getConstant<HK_QUADREAL_0001>();
}

HK_FORCE_INLINE /*static*/ const hkQuaternion& HK_CALL hkQuaternion::getIdentity()
{
	union { const hkQuadReal* r; const hkQuaternion* q; } r2q;
	r2q.r = g_vectorConstants + HK_QUADREAL_0001;
	return *r2q.q;
}

HK_FORCE_INLINE void hkQuaternion::setMul(hkSimdRealParameter r, hkQuaternionParameter q)
{
	m_vec.setMul(q.m_vec, r);
}

HK_FORCE_INLINE void hkQuaternion::addMul(hkSimdRealParameter r, hkQuaternionParameter q)
{
	m_vec.addMul(r,q.m_vec);
}

HK_FORCE_INLINE void hkQuaternion::setMul(hkQuaternionParameter q0, hkQuaternionParameter q1)
{
	const hkVector4 q0Imag = q0.getImag();
	const hkVector4 q1Imag = q1.getImag();
	const hkSimdReal q0Real = q0.getRealPart();
	const hkSimdReal q1Real = q1.getRealPart();

	hkVector4 vec;
	vec.setCross(q0Imag, q1Imag);
	vec.addMul(q0Real, q1Imag);
	vec.addMul(q1Real, q0Imag);
	const hkSimdReal w = (q0Real * q1Real) - q0Imag.dot<3>(q1Imag);
	m_vec.setXYZ_W(vec, w);
}


HK_FORCE_INLINE void hkQuaternion::mul(hkQuaternionParameter q)
{
	const hkVector4 thisImag = getImag();
	const hkVector4 qImag = q.getImag();
	const hkSimdReal thisReal = getRealPart();
	const hkSimdReal qReal = q.getRealPart();

	hkVector4 vec;
	vec.setCross(thisImag, qImag);
	vec.addMul(thisReal, qImag);
	vec.addMul(qReal, thisImag);
	const hkSimdReal w = (thisReal * qReal) - thisImag.dot<3>(qImag);
	m_vec.setXYZ_W(vec, w);
}

HK_FORCE_INLINE void hkQuaternion::setMulInverse(hkQuaternionParameter q0, hkQuaternionParameter q1)
{
	const hkVector4 q0Imag = q0.getImag();
	const hkVector4 q1Imag = q1.getImag();

	hkVector4 xyz; 
	xyz.setCross(q1Imag, q0Imag);
	xyz.subMul(q0.getRealPart(), q1Imag);
	xyz.addMul(q1.getRealPart(), q0Imag);
	const hkSimdReal w = q0Imag.dot<4>(q1Imag);
	m_vec.setXYZ_W(xyz, w);
}

HK_FORCE_INLINE void hkQuaternion::setInverseMul(hkQuaternionParameter q0, hkQuaternionParameter q1)
{
	const hkVector4 q0Imag = q0.getImag();
	const hkVector4 q1Imag = q1.getImag();

	hkVector4 xyz; 
	xyz.setCross(q1Imag, q0Imag);
	xyz.addMul(q0.getRealPart(), q1Imag);
	xyz.subMul(q1.getRealPart(), q0Imag);
	const hkSimdReal w = q0Imag.dot<4>(q1Imag);
	m_vec.setXYZ_W(xyz,w);
}

HK_FORCE_INLINE void hkQuaternion::estimateAngleTo(hkQuaternionParameter to, hkVector4& angleOut) const
{
	const hkVector4 fromImag = getImag();
	const hkVector4 toImag = to.getImag();

	hkVector4 angle;
	angle.setCross(fromImag, toImag);
	angle.subMul(to.getRealPart(), fromImag);
	angle.addMul(   getRealPart(), toImag);
	angle.add(angle);
	angleOut.setFlipSign( angle, toImag.dot<4>(fromImag) );
}


void hkQuaternion::setInverse( hkQuaternionParameter q )
{
	m_vec.setNeg<3>( q.getImag() );
}

const hkReal& hkQuaternion::operator()(int i) const
{
	return m_vec(i);
}

template <int I> 
HK_FORCE_INLINE const hkSimdReal hkQuaternion::getComponent() const
{
	return m_vec.getComponent<I>(); // has asserts
}

HK_FORCE_INLINE hkReal hkQuaternion::getAngle() const
{
	hkSimdReal absangle; absangle.setAbs(m_vec.getComponent<3>());
	hkReal angle = hkMath::acos(absangle.getReal());
	return angle * hkReal(2);
}

HK_FORCE_INLINE hkBool32 hkQuaternion::hasValidAxis() const
{
	return m_vec.lengthSquared<3>().isGreater(hkSimdReal::getConstant<HK_QUADREAL_EPS_SQRD>());
}

HK_FORCE_INLINE void hkQuaternion::getAxis(hkVector4 &axisOut) const
{
	hkVector4 axisTmp = m_vec;

	HK_ASSERT2(0x266e2bd7, hasValidAxis(), "Cannot extract axis from a Quaternion representing (within numerical tolerance) the Identity rotation (or Quaternion may not be normalized).");
	axisTmp.normalize<3>();

#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	const hkVector4Comparison wLessZero = m_vec.getComponent<3>().lessZero();
	axisOut.setFlipSign(axisTmp, wLessZero);
#else
	if(m_vec(3) < hkReal(0))
	{
		axisOut.setNeg<4>(axisTmp);
	}
	else
	{
		axisOut = axisTmp;
	}
#endif
}

HK_FORCE_INLINE void hkQuaternion::setShortestRotation(hkVector4Parameter from, hkVector4Parameter to)
{
	HK_ASSERT2(0xad87ba22, from.isNormalized<3>() && to.isNormalized<3>(), "hkQuaternion::setShortestRotation input vectors are not normalized.");

#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)

	const hkSimdReal dotProd = from.dot<3>( to ); // cos(theta)

	hkSimdReal nearlyOne; nearlyOne.setFromFloat(hkReal(1.0f - 1e-5f));

	if( dotProd.isGreater(nearlyOne) )
	{
		setIdentity();
	}
	else 
	{
		nearlyOne = -nearlyOne;

		if( dotProd.isLess(nearlyOne) )
		{
			setFlippedRotation( from );
		}
		else
		{
			// Using cos(theta) = 2*cos^2(theta/2) - 1
			const hkSimdReal oneHalf = hkSimdReal::getConstant<HK_QUADREAL_INV_2>();
			hkSimdReal c; c.setAddMul(oneHalf, oneHalf, dotProd); // .5 * ( 1 + dotProd) 
			const hkSimdReal sqrtInverseC = c.sqrtInverse();

			//const hkReal cosT2 = hkMath::sqrt( c );

			hkVector4 cross;
			cross.setCross( from, to ); // ~sin(theta)* axis -- this is not exact sin(theta) .. error around 1-e2 for angles close to 180deg

			//hkReal rescaleSin  = 0.5f / cosT2;
			hkSimdReal rescaleSin = oneHalf * sqrtInverseC;

			hkSimdReal somewhatNearlyNegOne; somewhatNearlyNegOne.setFromFloat(hkReal(-(1.0f - 1e-3f)));

			if (dotProd.isLess(somewhatNearlyNegOne))
			{
				// Extra correction needed for angles around 180deg
				//
				//const hkReal sinT2 = hkMath::sqrt( cosT2 * cosT2 - dotProd );
				const hkSimdReal sinT4 = c - dotProd;
				const hkSimdReal invApproxSinT = cross.lengthInverse<3>(); // this has errors around 1-e2 for angles around 180 deg.
				//const hkReal sinT = 2 * sinT2 * cosT2;
				//rescaleSin *= (sinT / approxSinT);

				//rescaleSin = sinT2 / approxSinT;
				rescaleSin = sinT4 * sinT4.sqrtInverse() * invApproxSinT;
			}

			// Using sin(theta) = 2*cos(theta/2)*sin(theta/2)
			cross.mul( rescaleSin );
			m_vec.setXYZ_W( cross, c * sqrtInverseC ); // = sqrt(c)
		}
	}

#else

	const hkReal dotProd = from.dot<3>( to ).getReal(); // cos(theta)
	const hkReal nearlyOne = hkReal(1.0f - 1e-5f);
	const hkReal somewhatNearlyNegativeOne = hkReal(-(1.0f - 1e-3f));
	if( dotProd > nearlyOne )
	{
		setIdentity();
	}
	else if( dotProd < -nearlyOne )
	{
		setFlippedRotation( from );
	}
	else
	{
		// Using cos(theta) = 2*cos^2(theta/2) - 1
		const hkReal c = (dotProd + hkReal(1)) * hkReal(0.5f);
		const hkReal cosT2 = hkMath::sqrt( c );

		hkVector4 cross;
		cross.setCross( from, to ); // ~sin(theta)* axis -- this is not exact sin(theta) .. error around 1-e2 for angles close to 180deg

		hkReal rescaleSin  = hkReal(0.5f) / cosT2;

		if (dotProd < somewhatNearlyNegativeOne)
		{
			// Extra correction needed for angles around 180deg
			//
			const hkReal sinT2 = hkMath::sqrt( cosT2 * cosT2 - dotProd );
			const hkReal approxSinT = cross.length<3>().getReal(); // this has errors around 1-e2 for angles around 180 deg.
			const hkReal sinT = hkReal(2) * sinT2 * cosT2;
			rescaleSin *= (sinT / approxSinT);
		}

		// Using sin(theta) = 2*cos(theta/2)*sin(theta/2)
		cross.mul( hkSimdReal::convert(rescaleSin) );
		cross(3) = cosT2;
		m_vec = cross;
	}

#endif

	HK_ASSERT2(0xad78999d, isOk(), "Resulting quaternion is not normalized.");
}

HK_FORCE_INLINE void hkQuaternion::setShortestRotationDamped(hkVector4Parameter from, hkVector4Parameter to, hkSimdRealParameter gain)
{
	const hkSimdReal one = hkSimdReal::getConstant<HK_QUADREAL_1>();
	const hkSimdReal half = hkSimdReal::getConstant<HK_QUADREAL_INV_2>();

	const hkSimdReal dotProd = from.dot<3>( to ); // cos(theta)
	const hkSimdReal dampedDot = one - gain + gain * dotProd;
	hkSimdReal nearlyOne; nearlyOne.setFromFloat(hkReal(1.0f - 1e-6f));

	const hkSimdReal c = (dampedDot + one) * half;
	if ( c.isLessEqualZero() | dotProd.isLess(-nearlyOne) )
	{
		setFlippedRotation( from );
	}
	else
	{
		if( dotProd.isGreater(nearlyOne) )
		{
			setIdentity();
		}
		else
		{
			const hkSimdReal cosT2 = c.sqrt();

			hkVector4 cross; cross.setCross( from, to ); // sin(theta)* axis

			// Using sin(theta) = 2*cos(theta/2)*sin(theta/2)
			hkSimdReal rescaleSin; rescaleSin.setDiv(gain * half, cosT2);
			cross.mul( rescaleSin );
			cross.setComponent<3>(cosT2);

			// renormalize for gain.
			cross.normalize<4>();
			m_vec = cross;
		}
	}

	HK_ASSERT2(0xad78999e, isOk(), "Resulting quaternion is not normalized.");
}

//
//	Sets the quaternion to the given quaternion, eventually transforming it so they are in the same hemisphere

HK_FORCE_INLINE void hkQuaternion::setClosest(hkQuaternionParameter q, hkQuaternionParameter qReference)
{
	m_vec.setFlipSign(q.m_vec, qReference.m_vec.dot<4>(q.m_vec));
}

//
//	Computes the logarithm of a quaternion

HK_FORCE_INLINE void hkQuaternion::getLog(hkVector4& vOut) const
{
	// Get axis and cos(angle)
	const hkSimdReal cosA = getRealPart();
	vOut = getImag();

	// If the angle is not zero, return angle * axis,
	// otherwise return just axis, because axis = sin(angle) * direction
	// and for very small angles, the angle / sin(angle) -> 1.
	hkSimdReal absCosA; absCosA.setAbs(cosA);
	if ( absCosA.isLess(hkSimdReal::getConstant<HK_QUADREAL_1>()) )
	{
		const hkReal angle = hkMath::acos(getReal()); // use indep cosA
		hkSimdReal a; a.setFromFloat(angle);
		const hkReal sinA = hkMath::sin(angle);
		hkSimdReal sA; sA.setFromFloat(sinA);

		hkSimdReal absSinA; absSinA.setAbs(sA);
		const hkVector4Comparison absSinAGreaterEqualEps = absSinA.greaterEqual(hkSimdReal::getConstant<HK_QUADREAL_EPS>());

		hkSimdReal d; d.setReciprocal(sA); d.mul(a);
		hkVector4 logV; logV.setMul(vOut, d);

		vOut.setSelect(absSinAGreaterEqualEps, logV, vOut);
	}
}

//
//	Set the quaternion as the exponential of the given (axis-angle) vector

HK_FORCE_INLINE void hkQuaternion::setExp(hkVector4Parameter vIn)
{
	const hkSimdReal angle = vIn.length<3>();

	hkSimdReal sA; sA.setFromFloat(hkMath::sin(angle.getReal()));
	hkSimdReal w; w.setFromFloat(hkMath::cos(angle.getReal()));

	hkSimdReal absSinA; absSinA.setAbs(sA);
	const hkVector4Comparison absSinAGreaterEqualEps = absSinA.greaterEqual(hkSimdReal::getConstant<HK_QUADREAL_EPS>());

	hkSimdReal d; d.setReciprocal(angle); d.mul(sA);
	hkVector4 expV; expV.setMul(vIn, d);

	hkVector4 quatDir; quatDir.setSelect(absSinAGreaterEqualEps, expV, vIn);

	m_vec.setXYZ_W(quatDir, w);
}

HK_FORCE_INLINE void hkQuaternion::setExp_Approximate(hkVector4Parameter vIn)
{
#if defined(HK_PLATFORM_WIN32) && (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	hkVector4 angle; angle.setAll(vIn.length<3>());

	hkVector4 sinCos; sinCos.m_quad = hkMath::quadSinCos(angle.m_quad);
	hkSimdReal sA = sinCos.getComponent<0>();

	hkSimdReal absSinA; absSinA.setAbs(sA);
	const hkVector4Comparison absSinAGreaterEqualEps = absSinA.greaterEqual(hkSimdReal::getConstant<HK_QUADREAL_EPS>());

	hkSimdReal d; d.setReciprocal<HK_ACC_12_BIT,HK_DIV_IGNORE>(angle.getComponent<0>()); d.mul(sA);
	hkVector4 expV; expV.setMul(vIn, d);

	hkVector4 quatDir; quatDir.setSelect(absSinAGreaterEqualEps, expV, vIn);

	m_vec.setXYZ_W(quatDir, sinCos.getComponent<1>());
#else
	setExp(vIn);
#endif
}

//
//	HK_FORCE_INLINEd. Sets/initializes this quaternion from a given rotation matrix.
//	The rotation r must be orthonormal.

HK_FORCE_INLINE void hkQuaternion_setFromRotationSimd(const hkRotation& r, hkVector4& vec)
{
	HK_ALIGN_REAL(const hkReal pmmp[4]) = {  1, -1, -1,  1 };
	HK_ALIGN_REAL(const hkReal mpmp[4]) = { -1,  1, -1,  1 };
	HK_ALIGN_REAL(const hkReal mmpp[4]) = { -1, -1,  1,  1 };

	// Get constants
	const hkSimdReal zero = hkSimdReal::getConstant(HK_QUADREAL_0);
	const hkSimdReal half = hkSimdReal::getConstant(HK_QUADREAL_INV_2);
	const hkSimdReal one = hkSimdReal::getConstant(HK_QUADREAL_1);
	// Trace setup
	const hkSimdReal m00 = r.getElement<0,0>();
	const hkSimdReal m11 = r.getElement<1,1>();
	const hkSimdReal m22 = r.getElement<2,2>();

	HK_ON_DEBUG( hkReal t = (m00+m11+m22).getReal() );
	HK_ASSERT2(0x23957faa, t > -1, "The matrix is not special orthogonal (=has a reflection component).");

	hkVector4 trace = hkVector4::getConstant<HK_QUADREAL_1>();
	trace.addMul(m00, *(const hkVector4*)&pmmp);
	trace.addMul(m11, *(const hkVector4*)&mpmp);
	trace.addMul(m22, *(const hkVector4*)&mmpp);

	// Calculate quaternion using the traces
	hkVector4 root;
	root.setSqrt<HK_ACC_23_BIT,HK_SQRT_SET_ZERO>(trace);
	root.mul(half); 

	// Fix reflections
	hkVector4 rotSigns;
	rotSigns.set( r.getElement<2,1>() - r.getElement<1,2>(),
				  r.getElement<0,2>() - r.getElement<2,0>(),
				  r.getElement<1,0>() - r.getElement<0,1>(),
				  half );

	vec.setFlipSign(root, rotSigns);
}

//
//	HK_FORCE_INLINEd. Sets/initializes this quaternion from a given rotation matrix.
//	The rotation r must be orthonormal.

HK_FORCE_INLINE void hkQuaternion_setFromRotationFpu(const hkRotation& r, hkVector4& vec)
{
	const hkReal trace = r(0,0) + r(1,1) + r(2,2);
	const hkReal half = hkReal(0.5f);

	// This is an exceptional case:
	// if trace==-1.0 since this means real=sqrt(trace+1) =0.0
	// hence we can't use real to compute the imaginary terms
	// if trace is close to -1.0, then the normal algorithm is
	// subject to numerical error.
	// Either way, we should use an alternate algorithm.
	// Please see doc "Numerical Problem In Quaternion-Matrix Conversion.doc"

	HK_ALIGN_REAL(hkReal v[4]);
	if( trace > hkReal(0) )
	{
		// else we calculate simply:
		hkReal s = hkMath::sqrt( trace + hkReal(1) );
		hkReal t = half / s;
		v[0] = (r(2,1)-r(1,2)) * t;
		v[1] = (r(0,2)-r(2,0)) * t;
		v[2] = (r(1,0)-r(0,1)) * t;
		v[3] = half * s;
	}
	else
	{
		const int next[] = {1,2,0};
		int i=0;

		if(r(1,1) > r(0,0)) i=1;
		if(r(2,2) > r(i,i)) i=2;

		int j = next[i];
		int k = next[j];

		hkReal s = hkMath::sqrt(r(i,i) - (r(j,j)+r(k,k)) + hkReal(1));
		hkReal t = half / s;

		v[i] = half * s;
		v[3] = (r(k,j)-r(j,k)) * t;
		v[j] = (r(j,i)+r(i,j)) * t;
		v[k] = (r(k,i)+r(i,k)) * t;
	}

	vec.load<4>(v);
}

//
//	HK_FORCE_INLINEd. Sets/initializes this quaternion from a given rotation matrix.
//	The rotation r must be orthonormal.

HK_FORCE_INLINE void hkQuaternion::_set(const hkRotation& r)
{

	hkQuaternion_setFromRotationFpu(r, m_vec);


	// Note: we don't renormalize here as we assume the rotation is orthonormal
	// <pk.todo> this assert breaks setAndNormalize method.
	// HK_ASSERT2(0x70dc41cb, isOk(), "hkRotation used for hkQuaternion construction is invalid. hkQuaternion is not normalized/invalid!");
}

template <hkMathAccuracyMode A, hkMathNegSqrtMode S>
HK_FORCE_INLINE void hkQuaternion::setAndNormalize(const hkRotation& r)
{
	_set(r);
	normalize<A,S>();
	HK_ASSERT2(0x70dc41cc, isOk(), "hkRotation used for hkQuaternion construction is invalid.");
}

HK_FORCE_INLINE void hkQuaternion::setAndNormalize(const hkRotation& r)
{
	_set(r);
	normalize<HK_ACC_23_BIT,HK_SQRT_IGNORE>();
	HK_ASSERT2(0x70dc41cc, isOk(), "hkRotation used for hkQuaternion construction is invalid.");
}

template <hkMathAccuracyMode A, hkMathNegSqrtMode S>
HK_FORCE_INLINE void hkQuaternion::normalize()
{
	m_vec.normalize<4,A,S>();
}

HK_FORCE_INLINE void hkQuaternion::normalize()
{
	m_vec.normalize<4,HK_ACC_23_BIT,HK_SQRT_IGNORE>();
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
