/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


HK_FORCE_INLINE void hkVector4::_setRotatedDir(hkQuaternionParameter quat, hkVector4Parameter direction)
{
#if HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED
	hkVector4 minusHalf; minusHalf.setNeg<4>(getConstant<HK_QUADREAL_INV_2>()); // minusHalf.m_quad = hkQuadRealMinusHalf; // todo constant
	hkVector4 qreal;	qreal.setBroadcast<3>(quat.m_vec);
	hkVector4 q2minus1;	q2minus1.setAddMul( minusHalf, qreal, qreal );

	hkVector4 ret;	ret.setMul( direction, q2minus1 );

	hkSimdReal imagDotDir = quat.getImag().dot<3>( direction );
#else
	hkSimdReal qreal = quat.m_vec.getComponent<3>();
	hkSimdReal q2minus1 = qreal*qreal - hkSimdReal_Half;

	hkVector4 ret;
	ret.setMul( direction, q2minus1 );

	hkSimdReal imagDotDir = quat.getImag().dot<3>( direction );
#endif
	ret.addMul( quat.getImag(), imagDotDir ); 

	hkVector4 imagCrossDir;
	imagCrossDir.setCross( quat.getImag(), direction );
	ret.addMul( imagCrossDir, qreal );

	setAdd( ret, ret );
}

HK_FORCE_INLINE void hkVector4::_setRotatedInverseDir(hkQuaternionParameter quat, hkVector4Parameter direction)
{
#if HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED
	hkVector4 minusHalf; minusHalf.setNeg<4>(getConstant<HK_QUADREAL_INV_2>()); //minusHalf.m_quad = hkQuadRealMinusHalf; // todo constant
	hkVector4 qreal;
	qreal.setBroadcast<3>(quat.m_vec);
	hkVector4 q2minus1;
	q2minus1.setAddMul( minusHalf, qreal, qreal );

	hkVector4 ret;
	ret.setMul( direction, q2minus1 );

	hkVector4 imagDotDir; 
	imagDotDir.setAll( quat.getImag().dot<3>( direction ) );
#else
	hkSimdReal qreal = quat.m_vec.getComponent<3>();
	hkSimdReal q2minus1 = qreal*qreal - hkSimdReal_Half;

	hkVector4 ret;
	ret.setMul( direction, q2minus1 );

	hkSimdReal imagDotDir = quat.getImag().dot<3>( direction );
#endif
	ret.addMul( quat.getImag(), imagDotDir ); 

	hkVector4 imagCrossDir;
	imagCrossDir.setCross( direction, quat.getImag() );
	ret.addMul( imagCrossDir, qreal );

	setAdd( ret, ret );
}

HK_FORCE_INLINE void hkVector4::setPlaneConstant(hkVector4Parameter p)
{
	const hkSimdReal w = dot<3>(p);
	setW(-w);
}

template <>
HK_FORCE_INLINE hkBool32 hkVector4::allExactlyEqual<1>(hkVector4Parameter v) const
{
	return getComponent<0>().isEqual(v.getComponent<0>());
}

template <>
HK_FORCE_INLINE hkBool32 hkVector4::allExactlyEqual<2>(hkVector4Parameter v) const
{
	// non_euclidean, manhattan based
	hkVector4 t;
	t.setSub(*this, v);
	return t.equalZero().allAreSet( hkVector4Comparison::MASK_XY );
}

template <>
HK_FORCE_INLINE hkBool32 hkVector4::allExactlyEqual<3>(hkVector4Parameter v) const
{
	// non_euclidean, manhattan based
	hkVector4 t;
	t.setSub(*this, v);
	return t.equalZero().allAreSet( hkVector4Comparison::MASK_XYZ );
}

template <>
HK_FORCE_INLINE hkBool32 hkVector4::allExactlyEqual<4>(hkVector4Parameter v) const
{
	// non_euclidean, manhattan based
	hkVector4 t;
	t.setSub(*this, v);
	return t.equalZero().allAreSet();
}

template <int N>
HK_FORCE_INLINE hkBool32 hkVector4::allExactlyEqual(hkVector4Parameter v) const
{
	HK_VECTOR4_NOT_IMPLEMENTED;
	return false;
}

template <>
HK_FORCE_INLINE hkBool32 hkVector4::allExactlyEqualZero<1>() const
{
	return getComponent<0>().isEqualZero();
}

template <>
HK_FORCE_INLINE hkBool32 hkVector4::allExactlyEqualZero<2>() const
{
	// non_euclidean, manhattan based
	return equalZero().allAreSet( hkVector4Comparison::MASK_XY );
}

template <>
HK_FORCE_INLINE hkBool32 hkVector4::allExactlyEqualZero<3>() const
{
	// non_euclidean, manhattan based
	return equalZero().allAreSet( hkVector4Comparison::MASK_XYZ );
}

template <>
HK_FORCE_INLINE hkBool32 hkVector4::allExactlyEqualZero<4>() const
{
	// non_euclidean, manhattan based
	return equalZero().allAreSet();
}

template <int N>
HK_FORCE_INLINE hkBool32 hkVector4::allExactlyEqualZero() const
{
	HK_VECTOR4_NOT_IMPLEMENTED;
	return false;
}

template <>
HK_FORCE_INLINE hkBool32 hkVector4::allEqual<1>(hkVector4Parameter v, hkSimdRealParameter epsilon) const
{
	hkSimdReal t = getComponent<0>() - v.getComponent<0>();
	t.setAbs( t );
	return t.isLess(epsilon);
}

template <>
HK_FORCE_INLINE hkBool32 hkVector4::allEqual<2>(hkVector4Parameter v, hkSimdRealParameter epsilon) const
{
	// non_euclidean, manhattan based
	hkVector4 t;
	t.setSub(*this, v);
	t.setAbs( t );
	hkVector4 epsilon_v;
	epsilon_v.setAll(epsilon);
	return t.less( epsilon_v ).allAreSet( hkVector4Comparison::MASK_XY );
}

template <>
HK_FORCE_INLINE hkBool32 hkVector4::allEqual<3>(hkVector4Parameter v, hkSimdRealParameter epsilon) const
{
	// non_euclidean, manhattan based
	hkVector4 t;
	t.setSub(*this, v);
	t.setAbs( t );
	hkVector4 epsilon_v;
	epsilon_v.setAll(epsilon);
	return t.less( epsilon_v ).allAreSet( hkVector4Comparison::MASK_XYZ );
}

template <>
HK_FORCE_INLINE hkBool32 hkVector4::allEqual<4>(hkVector4Parameter v, hkSimdRealParameter epsilon) const
{
	// non_euclidean, manhattan based
	hkVector4 t;
	t.setSub(*this, v);
	t.setAbs( t );
	hkVector4 epsilon_v;
	epsilon_v.setAll(epsilon);
	return t.less( epsilon_v ).allAreSet();
}

template <int N>
HK_FORCE_INLINE hkBool32 hkVector4::allEqual(hkVector4Parameter v, hkSimdRealParameter epsilon) const
{
	HK_VECTOR4_NOT_IMPLEMENTED;
	return false;
}

template <>
HK_FORCE_INLINE hkBool32 hkVector4::allEqualZero<1>(hkSimdRealParameter epsilon) const
{
	hkSimdReal t = getComponent<0>();
	t.setAbs( t );
	return t.isLess(epsilon);
}

template <>
HK_FORCE_INLINE hkBool32 hkVector4::allEqualZero<2>(hkSimdRealParameter epsilon) const
{
	// non_euclidean, manhattan based
	hkVector4 t;
	t.setAbs( *this );
	hkVector4 epsilon_v;
	epsilon_v.setAll(epsilon);
	return t.less( epsilon_v ).allAreSet( hkVector4Comparison::MASK_XY );
}

template <>
HK_FORCE_INLINE hkBool32 hkVector4::allEqualZero<3>(hkSimdRealParameter epsilon) const
{
	// non_euclidean, manhattan based
	hkVector4 t;
	t.setAbs( *this );
	hkVector4 epsilon_v;
	epsilon_v.setAll(epsilon);
	return t.less( epsilon_v ).allAreSet( hkVector4Comparison::MASK_XYZ );
}

template <>
HK_FORCE_INLINE hkBool32 hkVector4::allEqualZero<4>(hkSimdRealParameter epsilon) const
{
	// non_euclidean, manhattan based
	hkVector4 t;
	t.setAbs( *this );
	hkVector4 epsilon_v;
	epsilon_v.setAll(epsilon);
	return t.less( epsilon_v ).allAreSet();
}

template <int N>
HK_FORCE_INLINE hkBool32 hkVector4::allEqualZero(hkSimdRealParameter epsilon) const
{
	HK_VECTOR4_NOT_IMPLEMENTED;
	return false;
}

template <>
HK_FORCE_INLINE hkBool32 hkVector4::allLess<1>(hkVector4Parameter a) const
{
	return getComponent<0>().isLess(a.getComponent<0>());
}

template <>
HK_FORCE_INLINE hkBool32 hkVector4::allLess<2>(hkVector4Parameter a) const
{
	const hkVector4Comparison l = less(a);
	return l.allAreSet(hkVector4Comparison::MASK_XY);
}

template <>
HK_FORCE_INLINE hkBool32 hkVector4::allLess<3>(hkVector4Parameter a) const
{
	const hkVector4Comparison l = less(a);
	return l.allAreSet(hkVector4Comparison::MASK_XYZ);
}

template <>
HK_FORCE_INLINE hkBool32 hkVector4::allLess<4>(hkVector4Parameter a) const
{
	return less(a).allAreSet();
}

template <int N>
HK_FORCE_INLINE hkBool32 hkVector4::allLess(hkVector4Parameter a) const
{
	HK_VECTOR4_NOT_IMPLEMENTED;
	return false;
}

template <>
HK_FORCE_INLINE hkBool32 hkVector4::allLessZero<1>() const
{
	return getComponent<0>().isLessZero();
}

template <>
HK_FORCE_INLINE hkBool32 hkVector4::allLessZero<2>() const
{
	const hkVector4Comparison l = lessZero();
	return l.allAreSet(hkVector4Comparison::MASK_XY);
}

template <>
HK_FORCE_INLINE hkBool32 hkVector4::allLessZero<3>() const
{
	const hkVector4Comparison l = lessZero();
	return l.allAreSet(hkVector4Comparison::MASK_XYZ);
}

template <>
HK_FORCE_INLINE hkBool32 hkVector4::allLessZero<4>() const
{
	return lessZero().allAreSet();
}

template <int N>
HK_FORCE_INLINE hkBool32 hkVector4::allLessZero() const
{
	HK_VECTOR4_NOT_IMPLEMENTED;
	return false;
}

HK_FORCE_INLINE void hkVector4::setW(hkSimdRealParameter w)
{
	setComponent<3>(w);
}

template <int I>
HK_FORCE_INLINE void hkVector4::setBroadcast(hkVector4Parameter v)
{
	setAll( v.getComponent<I>() );
}

HK_FORCE_INLINE void hkVector4::setBroadcast(const int i, hkVector4Parameter v)
{
	setAll( v.getComponent(i) );
}

HK_FORCE_INLINE const hkSimdReal hkVector4::getW() const
{
	return getComponent<3>();
}

template <int N>
HK_FORCE_INLINE int hkVector4::getIndexOfMaxAbsComponent() const
{
	hkVector4 abs; abs.setAbs( *this );
	return abs.getIndexOfMaxComponent<N>();
}

template <>
HK_FORCE_INLINE int hkVector4::getIndexOfMaxComponent<4>() const
{
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	hkVector4 ma; ma.setHorizontalMax<4>( *this );
	const hkVector4Comparison isMax = greaterEqual(ma);
	HK_ON_DEBUG(const int mask = isMax.getMask();)
	HK_ASSERT2(0x2842fb1, mask > 0, "inconsistent max value of self");
	return isMax.getIndexOfLastComponentSet();
#else
	const hkReal* HK_RESTRICT tmp = (const hkReal* HK_RESTRICT)this;

	int		xyIndex = 0;
	hkReal	xyValue = tmp[0];
	if ( tmp[0] <= tmp[1] )
	{
		xyIndex = 1;
		xyValue = tmp[1];
	}

	int		zwIndex = 2;
	hkReal	zwValue = tmp[2];
	if ( tmp[2] <= tmp[3] )
	{
		zwIndex = 3;
		zwValue = tmp[3];
	}
	if ( xyValue <= zwValue )
	{
		return zwIndex;
	}
	return xyIndex;
#endif
}

template <>
HK_FORCE_INLINE int hkVector4::getIndexOfMaxComponent<3>() const
{
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	hkVector4 tmp; tmp.setXYZ_W(*this, -hkSimdReal_Max);
	hkVector4 ma; ma.setHorizontalMax<4>(tmp);
	const hkVector4Comparison isMax = tmp.greaterEqual(ma);
	HK_ON_DEBUG(const int mask = isMax.getMask();)
	HK_ASSERT2(0x2842fb1, mask > 0, "inconsistent max value of self");
	HK_ASSERT2(0x2842fb2, (mask & hkVector4Comparison::MASK_W) == 0, "-max compare failed");
	return isMax.getIndexOfLastComponentSet();
#else
	const hkReal* HK_RESTRICT tmp = (const hkReal* HK_RESTRICT)this;

	int		xyIndex = 0;
	hkReal	xyValue = tmp[0];
	if ( tmp[0] <= tmp[1] )
	{
		xyIndex = 1;
		xyValue = tmp[1];
	}

	if ( xyValue <= tmp[2] )
	{
		return 2;
	}
	return xyIndex;
#endif
}

template <>
HK_FORCE_INLINE int hkVector4::getIndexOfMaxComponent<2>() const
{
	return getComponent<0>().isGreater(getComponent<1>()) ? 0 : 1;
}

template <>
HK_FORCE_INLINE int hkVector4::getIndexOfMaxComponent<1>() const
{
	return 0;
}

template <int N>
HK_FORCE_INLINE int hkVector4::getIndexOfMaxComponent() const
{
	HK_VECTOR4_NOT_IMPLEMENTED;
	return 0;
}

template <int N>
HK_FORCE_INLINE int hkVector4::getIndexOfMinAbsComponent() const
{
	hkVector4 abs; abs.setAbs( *this );
	return abs.getIndexOfMinComponent<N>();
}

template <>
HK_FORCE_INLINE int hkVector4::getIndexOfMinComponent<4>() const
{
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	hkVector4 mi; mi.setHorizontalMin<4>( *this );
	const hkVector4Comparison isMin = lessEqual(mi);
	HK_ON_DEBUG(const int mask = isMin.getMask();)
	HK_ASSERT2(0x2842fb1, mask > 0, "inconsistent min value of self");
	return isMin.getIndexOfFirstComponentSet();
#else
	const hkReal* HK_RESTRICT tmp = (const hkReal* HK_RESTRICT)this;

	int		xyIndex = 0;
	hkReal	xyValue = tmp[0];
	if ( tmp[0] > tmp[1] )
	{
		xyIndex = 1;
		xyValue = tmp[1];
	}

	int		zwIndex = 2;
	hkReal	zwValue = tmp[2];
	if ( tmp[2] > tmp[3] )
	{
		zwIndex = 3;
		zwValue = tmp[3];
	}
	if ( xyValue > zwValue )
	{
		return zwIndex;
	}
	return xyIndex;
#endif
}

template <>
HK_FORCE_INLINE int hkVector4::getIndexOfMinComponent<3>() const
{
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	hkVector4 tmp; tmp.setXYZ_W(*this, hkSimdReal_Max);
	hkVector4 mi; mi.setHorizontalMin<4>(tmp);
	const hkVector4Comparison isMin = tmp.lessEqual(mi);
	HK_ON_DEBUG(const int mask = isMin.getMask();)
	HK_ASSERT2(0x2842fb1, mask > 0, "inconsistent min value of self");
	HK_ASSERT2(0x2842fb2, (mask & hkVector4Comparison::MASK_W) == 0, "max compare failed");
	return isMin.getIndexOfFirstComponentSet();
#else
	const hkReal* HK_RESTRICT tmp = (const hkReal* HK_RESTRICT)this;

	int		xyIndex = 0;
	hkReal	xyValue = tmp[0];
	if ( tmp[0] > tmp[1] )
	{
		xyIndex = 1;
		xyValue = tmp[1];
	}

	if ( xyValue > tmp[2] )
	{
		return 2;
	}
	return xyIndex;
#endif
}

template <>
HK_FORCE_INLINE int hkVector4::getIndexOfMinComponent<2>() const
{
	return getComponent<0>().isGreater(getComponent<1>()) ? 1 : 0;
}

template <>
HK_FORCE_INLINE int hkVector4::getIndexOfMinComponent<1>() const
{
	return 0;
}

template <int N>
HK_FORCE_INLINE int hkVector4::getIndexOfMinComponent() const
{
	HK_VECTOR4_NOT_IMPLEMENTED;
	return 0;
}

HK_FORCE_INLINE /*static*/ const hkVector4& HK_CALL hkVector4::getZero()
{
	return getConstant<HK_QUADREAL_0>();
}

template<int vectorConstant>
HK_FORCE_INLINE /*static*/ const hkVector4& HK_CALL hkVector4::getConstant()
{
	return *(const hkVector4*) (g_vectorConstants + vectorConstant);
}

HK_FORCE_INLINE /*static*/ const hkVector4& HK_CALL hkVector4::getConstant(hkVectorConstant vectorConstant)
{
	return *(const hkVector4*) (g_vectorConstants + vectorConstant);
}

HK_FORCE_INLINE const hkSimdReal hkVector4::distanceToSquared(hkVector4Parameter p) const
{
	hkVector4 d; d.setSub(*this, p);
	return d.lengthSquared<3>();
}

template <int N>
HK_FORCE_INLINE const hkSimdReal hkVector4::lengthSquared() const
{
	return dot<N>(*this);
}

template <int N>
HK_FORCE_INLINE void hkVector4::setDot(hkVector4Parameter a, hkVector4Parameter b)
{
	setAll( a.dot<N>(b) );
}

HK_FORCE_INLINE void hkVector4::operator= (hkVector4Parameter v)
{
	m_quad = v.m_quad;
}

HK_FORCE_INLINE void hkVector4::add(hkVector4Parameter a)
{
	setAdd( *this, a );
}

HK_FORCE_INLINE void hkVector4::sub(hkVector4Parameter a)
{
	setSub( *this, a );
}

HK_FORCE_INLINE void hkVector4::mul(hkVector4Parameter a)
{
	setMul( *this, a );
}

HK_FORCE_INLINE void hkVector4::mul(hkSimdRealParameter s)
{              
	setMul( *this, s );
}

HK_FORCE_INLINE void hkVector4::setMul(hkSimdRealParameter r, hkVector4Parameter v1)
{
	setMul(v1,r);
}

HK_FORCE_INLINE void hkVector4::addMul(hkVector4Parameter x, hkVector4Parameter y)
{
	setAddMul(*this, x, y);
}

HK_FORCE_INLINE void hkVector4::subMul(hkVector4Parameter x, hkVector4Parameter y)
{
	setSubMul(*this, x, y);
}

HK_FORCE_INLINE void hkVector4::subMul(hkSimdRealParameter r, hkVector4Parameter a)
{
	setSubMul(*this, a, r);
}

template <int N> 
HK_FORCE_INLINE void hkVector4::setHorizontalAdd(hkVector4Parameter v)
{
	setAll( v.horizontalAdd<N>() );
}

template <int N> 
HK_FORCE_INLINE void hkVector4::setHorizontalMax(hkVector4Parameter v)
{
	setAll( v.horizontalMax<N>() );
}

template <int N> 
HK_FORCE_INLINE void hkVector4::setHorizontalMin(hkVector4Parameter v)
{
	setAll( v.horizontalMin<N>() );
}

template <int I>
HK_FORCE_INLINE void hkVector4::broadcast()
{
	setBroadcast<I>(*this);
}

HK_FORCE_INLINE void hkVector4::broadcast(const int i)
{
	setBroadcast(i, *this);
}

HK_FORCE_INLINE void hkVector4::setInterpolate(hkVector4Parameter v0, hkVector4Parameter v1, hkSimdRealParameter t)
{
	// v0 + t * (v1 - v0)
	hkVector4 d; d.setSub(v1, v0);
	setAddMul( v0, d, t );
}


template <int N> 
HK_FORCE_INLINE bool hkVector4::isNormalized(hkReal epsilon) const
{
	if( isOk<N>() )
	{
		const hkSimdReal len = lengthSquared<N>();
		return hkMath::fabs( len.getReal() - hkReal(1) ) < epsilon;
	}
	return false;
}

HK_FORCE_INLINE void hkVector4::_setTransformedPos(const hkTransform& t, hkVector4Parameter b)
{
	hkVector4 tb; tb._setRotatedDir(t.getRotation(), b);
	setAdd(tb, t.getTranslation());
}

HK_FORCE_INLINE void hkVector4::_setTransformedInversePos(const hkTransform& a, hkVector4Parameter b)
{
	hkVector4 t0; t0.setSub( b, a.getTranslation() );
	_setRotatedInverseDir(a.getRotation(), t0);
}

HK_FORCE_INLINE void hkVector4::_setTransformedPos(const hkQsTransform& a, hkVector4Parameter b)
{
	hkVector4 temp = b;
	temp.mul(a.getScale());
	temp._setRotatedDir(a.getRotation(), temp);
	setAdd(temp, a.getTranslation());
}

HK_FORCE_INLINE void hkVector4::_setTransformedInversePos(const hkQsTransform& a, hkVector4Parameter b)
{
	hkVector4 temp = b;
	temp.sub(a.getTranslation());
	temp._setRotatedInverseDir(a.getRotation(), temp);

	hkVector4 invScale; invScale.setReciprocal<HK_ACC_23_BIT,HK_DIV_IGNORE>(a.getScale());
	setMul(temp, invScale);
}

HK_FORCE_INLINE void hkVector4::_setTransformedPos(const hkQTransform& a, hkVector4Parameter b)
{
	hkVector4 temp = b;
	temp._setRotatedDir(a.getRotation(), temp);
	setAdd(temp, a.getTranslation());
}

HK_FORCE_INLINE void hkVector4::_setTransformedInversePos(const hkQTransform& a, hkVector4Parameter b)
{
	hkVector4 temp = b;
	temp.sub(a.getTranslation());
	_setRotatedInverseDir(a.getRotation(), temp);
}

HK_FORCE_INLINE void hkVector4::addMul(hkVector4Parameter v1, hkSimdRealParameter r)
{
	setAddMul(*this, v1, r);
}

HK_FORCE_INLINE void hkVector4::addMul(hkSimdRealParameter r, hkVector4Parameter v1)
{
	setAddMul(*this, v1, r);
}

HK_FORCE_INLINE void hkVector4::subMul(hkVector4Parameter v1, hkSimdRealParameter r)
{
	setSubMul(*this, v1, r);
}

HK_FORCE_INLINE void hkVector4::setClamped( hkVector4Parameter a, hkVector4Parameter minVal, hkVector4Parameter maxVal )
{
	// This ensures that if a is NAN, clamped will be maxVal afterwards	
	const hkVector4Comparison maxValGtA = maxVal.greater(a);
	hkVector4 clamped; clamped.setSelect(maxValGtA, a, maxVal);
	setMax(minVal, clamped);
}

HK_FORCE_INLINE void hkVector4::setClampedToMaxLength(hkVector4Parameter vSrc, hkSimdRealParameter constMaxLen)
{
	// Make sure maxLen is > 0
	hkSimdReal maxLen;
	maxLen.setMax(constMaxLen, hkSimdReal::getConstant<HK_QUADREAL_EPS>());

	// maxLen can be big, i.e. HK_REAL_MAX, so we can't square it
	hkSimdReal len = vSrc.length<3>();
	const hkVector4Comparison cmp = len.greater(maxLen);

	// len = (len > maxLen) ? len : maxLen
	len.setSelect(cmp, len, maxLen);
	hkSimdReal invLen;
	invLen.setReciprocal(len);

	// f must be >= 1, otherwise we don't clamp
	hkSimdReal f = maxLen * invLen;
	f.setMin(f, hkSimdReal::getConstant<HK_QUADREAL_1>());
	setMul(f, vSrc);
}

HK_FORCE_INLINE void hkVector4::setInt24W( int value )
{
	HK_ASSERT2(0xf0781100, (value & 0xff000000) == 0, "can only store 24 bit integer");
#if defined(HK_REAL_IS_DOUBLE)
	hkInt64 wideValue = hkInt64(value) | 0x3ff0000000000000ull;
	(reinterpret_cast<hkInt64*>(this))[3] = wideValue;
#else
	hkInt32 fValue = hkInt32(value) | 0x3f000000;
	(reinterpret_cast<hkInt32*>(this))[3] = fValue;
#endif
}

HK_FORCE_INLINE int hkVector4::getInt24W( ) const
{
#if defined(HK_REAL_IS_DOUBLE)
	return int ((reinterpret_cast<const hkInt64*>(this))[3] & ~0x3ff0000000000000ull);
#else
	return (reinterpret_cast<const hkInt32*>(this))[3] & ~0x3f000000;
#endif
}

HK_FORCE_INLINE int hkVector4::getInt16W( ) const
{
#if HK_ENDIAN_LITTLE == 1
#if defined(HK_REAL_IS_DOUBLE)
	return (reinterpret_cast<const hkUint16*>(this))[12];
#else
	return (reinterpret_cast<const hkUint16*>(this))[6];
#endif
#else
#if defined(HK_REAL_IS_DOUBLE)
	return (reinterpret_cast<const hkUint16*>(this))[15];
#else
	return (reinterpret_cast<const hkUint16*>(this))[7];
#endif
#endif
}

template <>
HK_FORCE_INLINE int hkVector4::findComponent<4>(hkSimdRealParameter value) const
{
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	hkVector4 v; v.setAll(value);
	const hkVector4Comparison isEqual = equal(v);
	if (isEqual.anyIsSet())
		return isEqual.getIndexOfFirstComponentSet();
#else
	const hkReal* HK_RESTRICT tmp = (const hkReal* HK_RESTRICT)this;
	const hkReal* HK_RESTRICT v   = (const hkReal* HK_RESTRICT)&value;

	if (v[0] == tmp[0]) return 0;
	if (v[0] == tmp[1]) return 1;
	if (v[0] == tmp[2]) return 2;
	if (v[0] == tmp[3]) return 3;
#endif
	return -1;
}

template <>
HK_FORCE_INLINE int hkVector4::findComponent<3>(hkSimdRealParameter value) const
{
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	hkVector4 v; v.setAll(value);
	const hkVector4Comparison isEqual = equal(v);
	if (isEqual.anyIsSet(hkVector4Comparison::MASK_XYZ))
		return isEqual.getIndexOfFirstComponentSet();
#else
	const hkReal* HK_RESTRICT tmp = (const hkReal* HK_RESTRICT)this;
	const hkReal* HK_RESTRICT v   = (const hkReal* HK_RESTRICT)&value;

	if (v[0] == tmp[0]) return 0;
	if (v[0] == tmp[1]) return 1;
	if (v[0] == tmp[2]) return 2;
#endif
	return -1;
}

template <>
HK_FORCE_INLINE int hkVector4::findComponent<2>(hkSimdRealParameter value) const
{
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	hkVector4 v; v.setAll(value);
	const hkVector4Comparison isEqual = equal(v);
	if (isEqual.anyIsSet(hkVector4Comparison::MASK_XY))
		return isEqual.getIndexOfFirstComponentSet();
#else
	const hkReal* HK_RESTRICT tmp = (const hkReal* HK_RESTRICT)this;
	const hkReal* HK_RESTRICT v   = (const hkReal* HK_RESTRICT)&value;

	if (v[0] == tmp[0]) return 0;
	if (v[0] == tmp[1]) return 1;
#endif
	return -1;
}

template <>
HK_FORCE_INLINE int hkVector4::findComponent<1>(hkSimdRealParameter value) const
{
	return getComponent<0>().isEqual(value) ? 0 : -1;
}

template <int N>
HK_FORCE_INLINE int hkVector4::findComponent(hkSimdRealParameter value) const
{
	HK_VECTOR4_NOT_IMPLEMENTED;
	return 0;
}


#ifndef HK_DISABLE_OLD_VECTOR4_INTERFACE

//
// old interface
//

#include <Common/Base/Math/Vector/hkVector4Util.h>

HK_FORCE_INLINE hkQuadReal& hkVector4::getQuad() { return m_quad; }
HK_FORCE_INLINE const hkQuadReal& hkVector4::getQuad() const { return m_quad; }
HK_FORCE_INLINE void hkVector4::operator=(const hkQuadReal& r) { m_quad = r; }
HK_FORCE_INLINE void hkVector4::add4(hkVector4Parameter v) { add(v); }
HK_FORCE_INLINE void hkVector4::sub4(hkVector4Parameter v) { sub(v); }
HK_FORCE_INLINE void hkVector4::mul4(hkVector4Parameter a) { mul(a); }
HK_FORCE_INLINE void hkVector4::mul4(hkSimdRealParameter a) { mul(a); }
HK_FORCE_INLINE void hkVector4::div4(hkVector4Parameter a) { div<HK_ACC_FULL,HK_DIV_IGNORE>(a); }
HK_FORCE_INLINE void hkVector4::div4fast(hkVector4Parameter a) { div<HK_ACC_23_BIT,HK_DIV_IGNORE>(a); }
HK_FORCE_INLINE hkSimdReal hkVector4::dot3(hkVector4Parameter v) const { return dot<3>(v); }
HK_FORCE_INLINE hkSimdReal hkVector4::dot4(hkVector4Parameter a) const { return dot<4>(a); }
HK_FORCE_INLINE hkReal hkVector4::dot3fpu(hkVector4Parameter a) const { const hkVector4& t = *this; return (t(0) * a(0)) + ( t(1) * a(1)) + ( t(2) * a(2) ); }
HK_FORCE_INLINE void hkVector4::setMul4(hkVector4Parameter a, hkVector4Parameter b) { setMul(a,b); }
HK_FORCE_INLINE void hkVector4::setMul4(hkSimdRealParameter a, hkVector4Parameter b) { setMul(b,a); }
HK_FORCE_INLINE void hkVector4::_setMul4xyz1(const hkTransform& a, hkVector4Parameter b ) { _setTransformedPos(a,b); }
HK_FORCE_INLINE void hkVector4::subMul4(hkVector4Parameter a, hkVector4Parameter b) { subMul(a,b); }
HK_FORCE_INLINE void hkVector4::subMul4(hkSimdRealParameter a, hkVector4Parameter b) { subMul(b,a); }
HK_FORCE_INLINE void hkVector4::setSubMul4(hkVector4Parameter a, hkVector4Parameter x, hkVector4Parameter y) { setSubMul(a,x,y); }
HK_FORCE_INLINE void hkVector4::setSubMul4(hkVector4Parameter a, hkVector4Parameter x, hkSimdRealParameter y) { setSubMul(a,x,y); }
HK_FORCE_INLINE void hkVector4::setDot3(hkVector4Parameter a, hkVector4Parameter b) { setDot<3>(a,b); }
HK_FORCE_INLINE void hkVector4::setDot4(hkVector4Parameter a, hkVector4Parameter b) { setDot<4>(a,b); }
HK_FORCE_INLINE void hkVector4::setSelect4( hkVector4ComparisonParameter comp, hkVector4Parameter trueValue, hkVector4Parameter falseValue) { setSelect(comp,trueValue,falseValue); }
HK_FORCE_INLINE void hkVector4::select32( hkVector4Parameter falseValue, hkVector4Parameter trueValue, hkVector4ComparisonParameter comp) { setSelect(comp,trueValue,falseValue); }
HK_FORCE_INLINE void hkVector4::setBroadcast(hkVector4Parameter v, int i) { setBroadcast(i,v); }
HK_FORCE_INLINE void hkVector4::setBroadcast3clobberW(hkVector4Parameter v, int i) { setBroadcastXYZ(i,v); }
HK_FORCE_INLINE void hkVector4::setXYZW(hkVector4Parameter xyz, hkVector4Parameter w) { setXYZ_W(xyz,w); }
HK_FORCE_INLINE void hkVector4::setXYZW(hkVector4Parameter xyz, hkSimdRealParameter w) { setXYZ_W(xyz,w); }
HK_FORCE_INLINE void hkVector4::setXYZ0(hkVector4Parameter xyz) { setXYZ_0(xyz); }
HK_FORCE_INLINE void hkVector4::addMul4(hkVector4Parameter a, hkVector4Parameter b) { addMul(a,b); }
HK_FORCE_INLINE void hkVector4::addMul4(hkSimdRealParameter a, hkVector4Parameter b) { addMul(b,a); }
HK_FORCE_INLINE void hkVector4::setZero4() { setZero(); }
HK_FORCE_INLINE void hkVector4::setAll3(hkReal x) { setZero(); setXYZ(x); }
HK_FORCE_INLINE void hkVector4::zeroElement( int i ) { zeroComponent(i); }
HK_FORCE_INLINE void hkVector4::setNeg3(hkVector4Parameter v) { setNeg<3>(v); }
HK_FORCE_INLINE void hkVector4::setNeg4(hkVector4Parameter v) { setNeg<4>(v); }
HK_FORCE_INLINE void hkVector4::setNegMask4(hkVector4Parameter v, int mask) { hkVector4Comparison cmp; cmp.set((hkVector4Comparison::Mask)mask); setFlipSign(v, cmp); }
HK_FORCE_INLINE void hkVector4::setDiv4(hkVector4Parameter a, hkVector4Parameter b) { setDiv<HK_ACC_FULL,HK_DIV_IGNORE>(a,b); }
HK_FORCE_INLINE void hkVector4::setDiv4fast(hkVector4Parameter a, hkVector4Parameter b) { setDiv<HK_ACC_12_BIT,HK_DIV_IGNORE>(a,b); }
HK_FORCE_INLINE hkSimdReal hkVector4::getSimdAt(int i) const { return getComponent(i); }
HK_FORCE_INLINE void hkVector4::normalize3() { normalize<3,HK_ACC_23_BIT,HK_SQRT_SET_ZERO>(); }
HK_FORCE_INLINE void hkVector4::normalize4() { normalize<4,HK_ACC_23_BIT,HK_SQRT_SET_ZERO>(); }
HK_FORCE_INLINE void hkVector4::fastNormalize3() { normalize<3,HK_ACC_12_BIT,HK_SQRT_SET_ZERO>(); }
HK_FORCE_INLINE void hkVector4::fastNormalize3NonZero() { normalize<3,HK_ACC_12_BIT,HK_SQRT_IGNORE>(); }
HK_FORCE_INLINE void hkVector4::setFastNormalize3NonZero(hkVector4Parameter a) { *this = a; normalize<3,HK_ACC_12_BIT,HK_SQRT_IGNORE>(); }
HK_FORCE_INLINE hkSimdReal hkVector4::normalizeWithLength3() { return normalizeWithLength<3,HK_ACC_23_BIT,HK_SQRT_SET_ZERO>(); }
HK_FORCE_INLINE hkSimdReal hkVector4::normalizeWithLength4() { return normalizeWithLength<4,HK_ACC_23_BIT,HK_SQRT_SET_ZERO>(); }
HK_FORCE_INLINE hkSimdReal hkVector4::fastNormalizeWithLength3() { return normalizeWithLength<3,HK_ACC_12_BIT,HK_SQRT_SET_ZERO>(); }
HK_FORCE_INLINE hkResult hkVector4::normalize3IfNotZero () { return (normalizeIfNotZero<3>() ? HK_SUCCESS : HK_FAILURE); }
HK_FORCE_INLINE hkSimdReal hkVector4::length3() const { return length<3>(); }
HK_FORCE_INLINE hkSimdReal hkVector4::length4() const { return length<4>(); }
HK_FORCE_INLINE hkSimdReal hkVector4::lengthSquared3() const { return lengthSquared<3>(); }
HK_FORCE_INLINE hkSimdReal hkVector4::lengthSquared4() const { return lengthSquared<4>(); }
HK_FORCE_INLINE hkSimdReal hkVector4::lengthInverse3() const { return lengthInverse<3,HK_ACC_23_BIT,HK_SQRT_SET_ZERO>(); }
HK_FORCE_INLINE hkSimdReal hkVector4::lengthInverse4() const { return lengthInverse<4,HK_ACC_23_BIT,HK_SQRT_SET_ZERO>(); }
HK_FORCE_INLINE void hkVector4::setAdd4(hkVector4Parameter a, hkVector4Parameter b) { setAdd(a,b); }
HK_FORCE_INLINE void hkVector4::setSub4(hkVector4Parameter a, hkVector4Parameter b) { setSub(a,b); }
HK_FORCE_INLINE void hkVector4::setSqrtInverse4(hkVector4Parameter v) { setSqrtInverse<HK_ACC_23_BIT,HK_SQRT_IGNORE>(v); }
HK_FORCE_INLINE void hkVector4::setSqrtInverse4_7BitAccuracy(hkVector4Parameter v) { setSqrtInverse<HK_ACC_12_BIT,HK_SQRT_IGNORE>(v); }
HK_FORCE_INLINE void hkVector4::setReciprocal3(hkVector4Parameter v) { setReciprocal<HK_ACC_23_BIT,HK_DIV_IGNORE>(v); setComponent<3>(hkSimdReal::getConstant(HK_QUADREAL_1)); }
HK_FORCE_INLINE void hkVector4::setReciprocal4(hkVector4Parameter v) { setReciprocal<HK_ACC_23_BIT,HK_DIV_IGNORE>(v); }
HK_FORCE_INLINE void hkVector4::setAddMul4(hkVector4Parameter a, hkVector4Parameter x, hkVector4Parameter y) { setAddMul(a,x,y); }
HK_FORCE_INLINE void hkVector4::setAddMul4(hkVector4Parameter a, hkVector4Parameter b, hkSimdRealParameter r) { setAddMul(a,b,r); }
HK_FORCE_INLINE void hkVector4::setAbs4(hkVector4Parameter v) { setAbs(v); }
HK_FORCE_INLINE void hkVector4::setMin4(hkVector4Parameter a, hkVector4Parameter b) { setMin(a,b); }
HK_FORCE_INLINE void hkVector4::setMax4(hkVector4Parameter a, hkVector4Parameter b) { setMax(a,b); }
HK_FORCE_INLINE hkBool32 hkVector4::equals3(const hkVector4 &v, hkReal epsilon ) const
{
	hkVector4 t;
	t.setSub(*this, v);
	t.setAbs( t );
	hkVector4 epsilon_v;
	epsilon_v.setAll(epsilon);
	return t.lessEqual( epsilon_v ).allAreSet( hkVector4Comparison::MASK_XYZ );
}	
HK_FORCE_INLINE hkBool32 hkVector4::equals4(const hkVector4 &v, hkReal epsilon ) const
{
	hkVector4 t;
	t.setSub(*this, v);
	t.setAbs( t );
	hkVector4 epsilon_v;
	epsilon_v.setAll(epsilon);
	return t.lessEqual( epsilon_v ).allAreSet();
}
HK_FORCE_INLINE hkVector4Comparison hkVector4::compareEqual4(hkVector4Parameter a) const { return equal(a); }
HK_FORCE_INLINE hkVector4Comparison hkVector4::compareLessThan4(hkVector4Parameter a) const { return less(a); }
HK_FORCE_INLINE hkVector4Comparison hkVector4::compareLessThanEqual4(hkVector4Parameter a) const { return lessEqual(a); }
HK_FORCE_INLINE hkVector4Comparison hkVector4::compareGreaterThan4(hkVector4Parameter a) const { return greater(a); }
HK_FORCE_INLINE hkVector4Comparison hkVector4::compareGreaterThanEqual4(hkVector4Parameter a) const { return greaterEqual(a); }
HK_FORCE_INLINE hkVector4Comparison hkVector4::compareLessThanZero4() const { return lessZero(); }
HK_FORCE_INLINE hkBool32 hkVector4::allLessThan3(hkVector4Parameter a) const { return allLess<3>(a); }
HK_FORCE_INLINE hkBool32 hkVector4::allLessThan4(hkVector4Parameter a) const { return allLess<4>(a); }
HK_FORCE_INLINE void hkVector4::setInterpolate4( hkVector4Parameter a, hkVector4Parameter b, hkSimdRealParameter t ) { setInterpolate(a,b,t); }
HK_FORCE_INLINE hkSimdReal hkVector4::distanceTo3( hkVector4Parameter p ) const { return distanceTo(p); }
HK_FORCE_INLINE hkSimdReal hkVector4::distanceToSquared3( hkVector4Parameter p ) const { return distanceToSquared(p); }
HK_FORCE_INLINE hkSimdReal hkVector4::horizontalAdd3() const { return horizontalAdd<3>(); }
HK_FORCE_INLINE void hkVector4::setHorizontalMax4( hkVector4Parameter p) { setHorizontalMax<4>(p); }
HK_FORCE_INLINE hkSimdReal hkVector4::getHorizontalMin3() const { return horizontalMin<3>(); }
HK_FORCE_INLINE hkSimdReal hkVector4::getHorizontalMax3() const { return horizontalMax<3>(); }
HK_FORCE_INLINE void hkVector4::add3clobberW(hkVector4Parameter a) { addXYZ(a); }
HK_FORCE_INLINE hkSimdReal hkVector4::horizontalAdd4() const { return horizontalAdd<4>(); }
HK_FORCE_INLINE void hkVector4::setClamped(hkVector4Parameter vSrc, const hkSimdReal& constMaxLen) { setClampedToMaxLength(vSrc, constMaxLen); }
HK_FORCE_INLINE	int hkVector4::getMaxElementIndex4() const { return getIndexOfMaxComponent<4>(); }
HK_FORCE_INLINE	int hkVector4::getMajorAxis3() const { return getIndexOfMaxAbsComponent<3>(); }
HK_FORCE_INLINE void hkVector4::storeUncached( void* dest) const { store<4,HK_IO_NOT_CACHED>((hkReal*)dest); }
#if defined(HK_REAL_IS_DOUBLE)
HK_FORCE_INLINE void hkVector4::storeX( hkFloat32* dest)  const { hkDouble64 d; store<1>(&d); *dest= hkFloat32(d); }
HK_FORCE_INLINE void hkVector4::storeX( hkDouble64* dest) const { store<1>(dest); }
#else
HK_FORCE_INLINE void hkVector4::storeX( hkFloat32* dest)  const { store<1>(dest); }
HK_FORCE_INLINE void hkVector4::storeX( hkDouble64* dest) const { hkFloat32 d;  store<1>(&d); *dest=d; }
#endif
HK_FORCE_INLINE void hkVector4::sub3clobberW(hkVector4Parameter a) { subXYZ(a); }
HK_FORCE_INLINE void hkVector4::_setMul3(const hkMatrix3& a, hkVector4Parameter b) { _setRotatedDir(a,b); }
template <int S> HK_FORCE_INLINE void hkVector4::setShuffle(hkVector4Parameter v) { setPermutation<(hkVectorPermutation::Permutation)S>(v); }
HK_FORCE_INLINE void hkVector4::setAnd( hkVector4Parameter v0, hkVector4Parameter v1 ) { HK_ERROR(0x650a4a31,"not implemented"); }
HK_FORCE_INLINE void hkVector4::setSwapXY(const hkVector4& w) { setPermutation<hkVectorPermutation::YXZW>(w); }
HK_FORCE_INLINE void hkVector4::setMulSigns4(hkVector4Parameter a, hkVector4Parameter signs) { setFlipSign(a, signs); }
HK_FORCE_INLINE void hkVector4::setMulSigns4(hkVector4Parameter a, hkSimdRealParameter sharedSign) { setFlipSign(a, sharedSign); }
HK_FORCE_INLINE void hkVector4::setDot3_1vs4(hkVector4Parameter v, hkVector4Parameter a0, hkVector4Parameter a1, hkVector4Parameter a2, hkVector4Parameter a3) { hkVector4Util::dot3_1vs4(v,a0,a1,a2,a3,*this); }
HK_FORCE_INLINE hkBool hkVector4::isNormalized3(hkReal eps) const { return isNormalized<3>(eps); }
HK_FORCE_INLINE hkBool hkVector4::isNormalized4(hkReal eps) const { return isNormalized<4>(eps); }
HK_FORCE_INLINE hkBool hkVector4::isOk3() const { return isOk<3>(); }
HK_FORCE_INLINE hkBool hkVector4::isOk4() const { return isOk<4>(); }
HK_FORCE_INLINE const hkVector4Comparison hkVector4::isNegative() const { return signBitSet(); }
HK_FORCE_INLINE const hkVector4Comparison hkVector4::isPositive() const { return signBitClear(); }
#if defined(HK_REAL_IS_DOUBLE)
HK_FORCE_INLINE void hkVector4::load3(const hkFloat32* p) { HK_ALIGN_REAL(hkDouble64 pp[3]); pp[0]=p[0]; pp[1]=p[1]; pp[2]=p[2]; load<3,HK_IO_SIMD_ALIGNED>(pp); }
HK_FORCE_INLINE void hkVector4::load4(const hkFloat32* p) { HK_ALIGN_REAL(hkDouble64 pp[4]); pp[0]=p[0]; pp[1]=p[1]; pp[2]=p[2]; pp[3]=p[3]; load<4,HK_IO_SIMD_ALIGNED>(pp); }
HK_FORCE_INLINE void hkVector4::load4a(const hkFloat32* p) { HK_ALIGN_REAL(hkDouble64 pp[4]); pp[0]=p[0]; pp[1]=p[1]; pp[2]=p[2]; pp[3]=p[3]; load<4,HK_IO_SIMD_ALIGNED>(pp); }
HK_FORCE_INLINE void hkVector4::load3(const hkDouble64* p) { load<3,HK_IO_NATIVE_ALIGNED>(p); }
HK_FORCE_INLINE void hkVector4::load4(const hkDouble64* p) { load<4,HK_IO_NATIVE_ALIGNED>(p); }
HK_FORCE_INLINE void hkVector4::load4a(const hkDouble64* p) { load<4,HK_IO_SIMD_ALIGNED>(p); }
HK_FORCE_INLINE void hkVector4::store3(hkFloat32* p) const { HK_ALIGN_REAL(hkDouble64 pp[3]); store<3,HK_IO_SIMD_ALIGNED>(pp); p[0]=hkFloat32(pp[0]); p[1]=hkFloat32(pp[1]); p[2]=hkFloat32(pp[2]); }
HK_FORCE_INLINE void hkVector4::store4(hkFloat32* p) const { HK_ALIGN_REAL(hkDouble64 pp[4]); store<4,HK_IO_SIMD_ALIGNED>(pp); p[0]=hkFloat32(pp[0]); p[1]=hkFloat32(pp[1]); p[2]=hkFloat32(pp[2]); p[3]=hkFloat32(pp[3]); }
HK_FORCE_INLINE void hkVector4::store4a(hkFloat32* p) const { HK_ALIGN_REAL(hkDouble64 pp[4]); store<4,HK_IO_SIMD_ALIGNED>(pp); p[0]=hkFloat32(pp[0]); p[1]=hkFloat32(pp[1]); p[2]=hkFloat32(pp[2]); p[3]=hkFloat32(pp[3]); }
HK_FORCE_INLINE void hkVector4::store3(hkDouble64* p) const { store<3,HK_IO_NATIVE_ALIGNED>(p); }
HK_FORCE_INLINE void hkVector4::store4(hkDouble64* p) const { store<4,HK_IO_NATIVE_ALIGNED>(p); }
HK_FORCE_INLINE void hkVector4::store4a(hkDouble64* p) const { store<4,HK_IO_SIMD_ALIGNED>(p); }
#else
HK_FORCE_INLINE void hkVector4::load3(const hkFloat32* p) { load<3,HK_IO_NATIVE_ALIGNED>(p); }
HK_FORCE_INLINE void hkVector4::load4(const hkFloat32* p) { load<4,HK_IO_NATIVE_ALIGNED>(p); }
HK_FORCE_INLINE void hkVector4::load4a(const hkFloat32* p) { load<4,HK_IO_SIMD_ALIGNED>(p); }
HK_FORCE_INLINE void hkVector4::load3(const hkDouble64* p) { HK_ALIGN_REAL(hkFloat32 pp[3]); pp[0]=hkFloat32(p[0]); pp[1]=hkFloat32(p[1]); pp[2]=hkFloat32(p[2]); load<3,HK_IO_SIMD_ALIGNED>(pp); }
HK_FORCE_INLINE void hkVector4::load4(const hkDouble64* p) { HK_ALIGN_REAL(hkFloat32 pp[4]); pp[0]=hkFloat32(p[0]); pp[1]=hkFloat32(p[1]); pp[2]=hkFloat32(p[2]); pp[3]=hkFloat32(p[3]); load<4,HK_IO_SIMD_ALIGNED>(pp); }
HK_FORCE_INLINE void hkVector4::load4a(const hkDouble64* p) { HK_ALIGN_REAL(hkFloat32 pp[4]); pp[0]=hkFloat32(p[0]); pp[1]=hkFloat32(p[1]); pp[2]=hkFloat32(p[2]); pp[3]=hkFloat32(p[3]); load<4,HK_IO_SIMD_ALIGNED>(pp); }
HK_FORCE_INLINE void hkVector4::store3(hkFloat32* p) const { store<3,HK_IO_NATIVE_ALIGNED>(p); }
HK_FORCE_INLINE void hkVector4::store4(hkFloat32* p) const { store<4,HK_IO_NATIVE_ALIGNED>(p); }
HK_FORCE_INLINE void hkVector4::store4a(hkFloat32* p) const { store<4,HK_IO_SIMD_ALIGNED>(p); }
HK_FORCE_INLINE void hkVector4::store3(hkDouble64* p) const { HK_ALIGN_REAL(hkFloat32 pp[3]); store<3,HK_IO_SIMD_ALIGNED>(pp); p[0]=pp[0]; p[1]=pp[1]; p[2]=pp[2]; }
HK_FORCE_INLINE void hkVector4::store4(hkDouble64* p) const { HK_ALIGN_REAL(hkFloat32 pp[4]); store<4,HK_IO_SIMD_ALIGNED>(pp); p[0]=pp[0]; p[1]=pp[1]; p[2]=pp[2]; p[3]=pp[3]; }
HK_FORCE_INLINE void hkVector4::store4a(hkDouble64* p) const { HK_ALIGN_REAL(hkFloat32 pp[4]); store<4,HK_IO_SIMD_ALIGNED>(pp); p[0]=pp[0]; p[1]=pp[1]; p[2]=pp[2]; p[3]=pp[3]; }
#endif
HK_FORCE_INLINE void hkVector4::setMul3(const hkMatrix3& a, hkVector4Parameter b ) { _setRotatedDir(a,b); }
HK_FORCE_INLINE void hkVector4::_setMul4(const hkMatrix3& a, hkVector4Parameter b ) { _setRotatedDir(a,b); }

#endif

//
// advanced interface
//

template <int N, hkMathAccuracyMode A, hkMathNegSqrtMode S> 
HK_FORCE_INLINE const hkSimdReal hkVector4::length() const
{
	const hkSimdReal len2 = lengthSquared<N>();
	return len2.sqrt<A,S>();
}

template <int N>
HK_FORCE_INLINE const hkSimdReal hkVector4::length() const
{
	return length<N,HK_ACC_23_BIT,HK_SQRT_SET_ZERO>();
}

template <int N, hkMathAccuracyMode A, hkMathNegSqrtMode S> 
HK_FORCE_INLINE const hkSimdReal hkVector4::lengthInverse() const
{
	const hkSimdReal len2 = lengthSquared<N>();
	return len2.sqrtInverse<A,S>();
}

template <int N>
HK_FORCE_INLINE const hkSimdReal hkVector4::lengthInverse() const
{
	return lengthInverse<N,HK_ACC_23_BIT,HK_SQRT_SET_ZERO>();
}

template <int N, hkMathAccuracyMode A, hkMathNegSqrtMode S>
HK_FORCE_INLINE void hkVector4::normalize()
{
	mul( lengthInverse<N,A,S>() );
}

template <int N>
HK_FORCE_INLINE void hkVector4::normalize()
{
	normalize<N,HK_ACC_23_BIT,HK_SQRT_SET_ZERO>();
}

template <int N, hkMathAccuracyMode A, hkMathNegSqrtMode S>
HK_FORCE_INLINE const hkSimdReal hkVector4::normalizeWithLength()
{
	const hkSimdReal len2 = lengthSquared<N>();
	const hkSimdReal lenInv = len2.sqrtInverse<A,S>();
	mul(lenInv);
	return (len2 * lenInv); // quicker to return x^2/x
}

template <int N>
HK_FORCE_INLINE const hkSimdReal hkVector4::normalizeWithLength()
{
	return normalizeWithLength<N,HK_ACC_23_BIT,HK_SQRT_SET_ZERO>();
}

template <int N, hkMathAccuracyMode A, hkMathNegSqrtMode S>
HK_FORCE_INLINE hkBool32 hkVector4::normalizeIfNotZero()
{
	const hkSimdReal lengthSqrd = lengthSquared<N>();
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	const hkVector4Comparison less0 = ( -lengthSqrd ).lessZero();

	hkVector4 normThis; normThis.setMul(*this, lengthSqrd.sqrtInverse<A,S>());
	setSelect(less0, normThis, *this);

	return less0.anyIsSet();
#else
	if ( ( -lengthSqrd ).isLessZero() )
	{
		mul(lengthSqrd.sqrtInverse<A,S>());
		return 1;
	}
	return 0;
#endif
}

template <int N>
HK_FORCE_INLINE hkBool32 hkVector4::normalizeIfNotZero()
{
	return normalizeIfNotZero<N,HK_ACC_23_BIT,HK_SQRT_SET_ZERO>();
}

template <hkMathAccuracyMode A, hkMathNegSqrtMode S>
HK_FORCE_INLINE const hkSimdReal hkVector4::distanceTo(hkVector4Parameter p) const
{
	const hkSimdReal d2 = distanceToSquared(p);
	const hkSimdReal ri = d2.sqrtInverse<A,S>();
	return (d2 * ri);
}

HK_FORCE_INLINE const hkSimdReal hkVector4::distanceTo(hkVector4Parameter p) const
{
	return distanceToSquared(p).sqrt<HK_ACC_23_BIT,HK_SQRT_SET_ZERO>();
}


template <int N, hkMathAccuracyMode A>
HK_FORCE_INLINE const hkSimdReal hkVector4::setNormalizedEnsureUnitLength(hkVector4Parameter v)
{
	const hkSimdReal length2 = v.lengthSquared<N>();
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	const hkVector4Comparison lengthLessEps = length2.less(hkSimdReal::getConstant<HK_QUADREAL_EPS_SQRD>());

	hkSimdReal invLength;
	if ( A == HK_ACC_FULL )
	{
		 invLength = length2.sqrtInverse<A,HK_SQRT_SET_ZERO>();
	}
	else
	{
		 invLength = length2.sqrtInverse<A,HK_SQRT_IGNORE>();
	}
	hkVector4 normV; normV.setMul(v,invLength);
	setSelect(lengthLessEps, hkVector4::getConstant<HK_QUADREAL_1000>(), normV);

	hkSimdReal oldLength; oldLength.setSelect( lengthLessEps, hkSimdReal_0, length2 * invLength );
	return oldLength;
#else
	if (length2.isGreaterEqual(hkSimdReal::getConstant<HK_QUADREAL_EPS_SQRD>()))
	{
		const hkSimdReal invLength = length2.sqrtInverse<A,HK_SQRT_IGNORE>();
		setMul(v,invLength);
		return (length2 * invLength);
	}
	m_quad = g_vectorConstants[HK_QUADREAL_1000];
	return hkSimdReal_0;
#endif
}

template <int N>
HK_FORCE_INLINE const hkSimdReal hkVector4::setNormalizedEnsureUnitLength(hkVector4Parameter v)
{
	return setNormalizedEnsureUnitLength<N,HK_ACC_23_BIT>(v);
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
