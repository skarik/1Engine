/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

HK_FORCE_INLINE const hkVector4& hkMatrix4::getColumn(int x) const
{
#ifdef HK_COMPILER_GHS
	return ((hkVector4*)this)[x];
#else
	return (&m_col0)[x];
#endif
}

HK_FORCE_INLINE hkVector4& hkMatrix4::getColumn(int x)
{
#ifdef HK_COMPILER_GHS
	return ((hkVector4*)this)[x];
#else
	return (&m_col0)[x];
#endif
}

HK_FORCE_INLINE void hkMatrix4::setColumn(int x, hkVector4Parameter c)
{
#ifdef HK_COMPILER_GHS
	((hkVector4*)this)[x] = c;
#else
	(&m_col0)[x] = c;
#endif
}

template <>
HK_FORCE_INLINE const hkVector4& hkMatrix4::getColumn<3>() const
{
	return m_col3;
}

template <>
HK_FORCE_INLINE const hkVector4& hkMatrix4::getColumn<2>() const
{
	return m_col2;
}

template <>
HK_FORCE_INLINE const hkVector4& hkMatrix4::getColumn<1>() const
{
	return m_col1;
}

template <>
HK_FORCE_INLINE const hkVector4& hkMatrix4::getColumn<0>() const
{
	return m_col0;
}

template <int N>
HK_FORCE_INLINE const hkVector4& hkMatrix4::getColumn() const
{
	HK_MATRIX4_NOT_IMPLEMENTED;
	return hkVector4::getConstant<HK_QUADREAL_0>();
}

template <>
HK_FORCE_INLINE void hkMatrix4::setColumn<3>(hkVector4Parameter c)
{
	m_col3 = c;
}

template <>
HK_FORCE_INLINE void hkMatrix4::setColumn<2>(hkVector4Parameter c)
{
	m_col2 = c;
}

template <>
HK_FORCE_INLINE void hkMatrix4::setColumn<1>(hkVector4Parameter c)
{
	m_col1 = c;
}

template <>
HK_FORCE_INLINE void hkMatrix4::setColumn<0>(hkVector4Parameter c)
{
	m_col0 = c;
}

template <int N>
HK_FORCE_INLINE void hkMatrix4::setColumn(hkVector4Parameter c)
{
	HK_MATRIX4_NOT_IMPLEMENTED;
}

HK_FORCE_INLINE hkReal& hkMatrix4::operator() (int r, int c)
{
	return (&m_col0)[c](r);
}

HK_FORCE_INLINE const hkReal& hkMatrix4::operator() (int r, int c) const
{
	return getColumn(c)(r);
}

template <int ROW, int COL>
HK_FORCE_INLINE const hkSimdReal hkMatrix4::getElement() const
{
// our reflection parser cannot compile this:	return getColumn<COL>().getComponent<ROW>();
	return (&m_col0)[COL].getComponent<ROW>();
}

template <int ROW, int COL>
HK_FORCE_INLINE void hkMatrix4::setElement(hkSimdRealParameter element)
{
	(&m_col0)[COL].setComponent<ROW>(element);
}

HK_FORCE_INLINE void hkMatrix4::setRows( hkVector4Parameter r0, hkVector4Parameter r1, hkVector4Parameter r2, hkVector4Parameter r3)
{
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	m_col0 = r0;
	m_col1 = r1;
	m_col2 = r2;
	m_col3 = r3;
	HK_TRANSPOSE4(m_col0,m_col1,m_col2,m_col3);
#else
	hkVector4 c0; c0.set( r0(0), r1(0), r2(0), r3(0) );
	hkVector4 c1; c1.set( r0(1), r1(1), r2(1), r3(1) );
	hkVector4 c2; c2.set( r0(2), r1(2), r2(2), r3(2) );
	hkVector4 c3; c3.set( r0(3), r1(3), r2(3), r3(3) );

	setColumn<0>(c0);
	setColumn<1>(c1);
	setColumn<2>(c2);
	setColumn<3>(c3);
#endif
}


HK_FORCE_INLINE void hkMatrix4::getRows( hkVector4& r0, hkVector4& r1, hkVector4& r2, hkVector4& r3) const
{
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	hkVector4 c0 = getColumn<0>();
	hkVector4 c1 = getColumn<1>();
	hkVector4 c2 = getColumn<2>();
	hkVector4 c3 = getColumn<3>();

	HK_TRANSPOSE4(c0,c1,c2,c3);
#else
	hkVector4 c0; c0.set( m_col0(0), m_col1(0), m_col2(0), m_col3(0) );
	hkVector4 c1; c1.set( m_col0(1), m_col1(1), m_col2(1), m_col3(1) );
	hkVector4 c2; c2.set( m_col0(2), m_col1(2), m_col2(2), m_col3(2) );
	hkVector4 c3; c3.set( m_col0(3), m_col1(3), m_col2(3), m_col3(3) );
#endif

	r0 = c0;
	r1 = c1;
	r2 = c2;
	r3 = c3;
}

HK_FORCE_INLINE void hkMatrix4::setCols( hkVector4Parameter c0, hkVector4Parameter c1, hkVector4Parameter c2, hkVector4Parameter c3)
{
	m_col0 = c0;
	m_col1 = c1;
	m_col2 = c2;
	m_col3 = c3;
}

HK_FORCE_INLINE void hkMatrix4::operator= ( const hkMatrix4& a )
{
	m_col0 = a.getColumn<0>();
	m_col1 = a.getColumn<1>();
	m_col2 = a.getColumn<2>();
	m_col3 = a.getColumn<3>();
}

HK_FORCE_INLINE void hkMatrix4::getCols( hkVector4& c0, hkVector4& c1, hkVector4& c2, hkVector4& c3) const
{
	c0 = m_col0;
	c1 = m_col1;
	c2 = m_col2;
	c3 = m_col3;
}

HK_FORCE_INLINE void hkMatrix4::getRow( int row, hkVector4& r) const
{
	r.set( m_col0(row), m_col1(row), m_col2(row), m_col3(row) );
}

template <int I>
HK_FORCE_INLINE void hkMatrix4::getRow( hkVector4& r ) const
{
	r.set( getElement<I,0>(), getElement<I,1>(), getElement<I,2>(), getElement<I,3>() );
}

HK_FORCE_INLINE void hkMatrix4::setRow( int row, hkVector4Parameter r )
{
	m_col0(row) = r(0);
	m_col1(row) = r(1);
	m_col2(row) = r(2);
	m_col3(row) = r(3);
}

template <int I>
HK_FORCE_INLINE void hkMatrix4::setRow( hkVector4Parameter r )
{
	setElement<I,0>( r.getComponent<0>() );
	setElement<I,1>( r.getComponent<1>() );
	setElement<I,2>( r.getComponent<2>() );
	setElement<I,3>( r.getComponent<3>() );
}

HK_FORCE_INLINE void hkMatrix4::setZero()
{
	m_col0.setZero();
	m_col1.setZero();
	m_col2.setZero();
	m_col3.setZero();
}

HK_FORCE_INLINE void hkMatrix4::setDiagonal( hkReal m00, hkReal m11, hkReal m22, hkReal m33 )
{
	setZero();
	m_col0(0) = m00;
	m_col1(1) = m11;
	m_col2(2) = m22;
	m_col3(3) = m33;
}

HK_FORCE_INLINE void hkMatrix4::setDiagonal( hkSimdRealParameter m00, hkSimdRealParameter m11, hkSimdRealParameter m22, hkSimdRealParameter m33 )
{
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	m_col0.setMul(hkVector4::getConstant<HK_QUADREAL_1000>(), m00);
	m_col1.setMul(hkVector4::getConstant<HK_QUADREAL_0100>(), m11);
	m_col2.setMul(hkVector4::getConstant<HK_QUADREAL_0010>(), m22);
	m_col3.setMul(hkVector4::getConstant<HK_QUADREAL_0001>(), m33);
#else
	setZero();
	m_col0(0) = m00.getReal();
	m_col1(1) = m11.getReal();
	m_col2(2) = m22.getReal();
	m_col3(3) = m33.getReal();
#endif
}

HK_FORCE_INLINE void hkMatrix4::setDiagonal( hkVector4Parameter v )
{
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	m_col0.setMul(hkVector4::getConstant<HK_QUADREAL_1000>(), v);
	m_col1.setMul(hkVector4::getConstant<HK_QUADREAL_0100>(), v);
	m_col2.setMul(hkVector4::getConstant<HK_QUADREAL_0010>(), v);
	m_col3.setMul(hkVector4::getConstant<HK_QUADREAL_0001>(), v);
#else
	setZero();
	m_col0(0) = v(0);
	m_col1(1) = v(1);
	m_col2(2) = v(2);
	m_col3(3) = v(3);
#endif
}

HK_FORCE_INLINE void hkMatrix4::setIdentity()
{
	m_col0 = hkVector4::getConstant<HK_QUADREAL_1000>();
	m_col1 = hkVector4::getConstant<HK_QUADREAL_0100>();
	m_col2 = hkVector4::getConstant<HK_QUADREAL_0010>();
	m_col3 = hkVector4::getConstant<HK_QUADREAL_0001>();
}

HK_FORCE_INLINE void hkMatrix4::transformPosition (hkVector4Parameter positionIn, hkVector4& positionOut) const
{
	
	//HK_WARN_ON_DEBUG_IF(!isAffineTransformation(),0x872bbf1a, "Trying to transform a position by a 4x4 matrix not representing an affine transformation");	// removing warning (note: there is still hkTransform)

	hkVector4 xb; xb.setBroadcast<0>(positionIn);
	hkVector4 yb; yb.setBroadcast<1>(positionIn);
	hkVector4 zb; zb.setBroadcast<2>(positionIn);

	hkVector4 t0;	
	t0.setAddMul( m_col3, xb, m_col0 );
	t0.addMul( yb, m_col1 );
	t0.addMul( zb, m_col2 );

	positionOut = t0;
}

HK_FORCE_INLINE void hkMatrix4::multiplyVector (hkVector4Parameter v, hkVector4& resultOut) const
{
	hkVector4 xb; xb.setBroadcast<0>(v);
	hkVector4 yb; yb.setBroadcast<1>(v);
	hkVector4 zb; zb.setBroadcast<2>(v);
	hkVector4 wb; wb.setBroadcast<3>(v);

	hkVector4 t0;
	t0.setMul( xb, m_col0 );
	t0.addMul( yb, m_col1 );
	t0.addMul( zb, m_col2 );
	t0.addMul( wb, m_col3 );

	resultOut = t0;
}

HK_FORCE_INLINE void hkMatrix4::transformDirection (hkVector4Parameter directionIn, hkVector4& directionOut) const
{
	HK_WARN_ON_DEBUG_IF(!isAffineTransformation(),0x872bbf1c, "Trying to transform a direction by a 4x4 matrix not representing an affine transformation");

	hkVector4 xb; xb.setBroadcast<0>(directionIn);
	hkVector4 yb; yb.setBroadcast<1>(directionIn);
	hkVector4 zb; zb.setBroadcast<2>(directionIn);

	hkVector4 t0;
	t0.setMul( xb, m_col0 );
	t0.addMul( yb, m_col1 );
	t0.addMul( zb, m_col2 );

	directionOut = t0;
}

HK_FORCE_INLINE void hkMatrix4::setTranspose( const hkMatrix4& s )
{
	setRows(s.getColumn<0>(), s.getColumn<1>(), s.getColumn<2>(), s.getColumn<3>());
}

HK_FORCE_INLINE void hkMatrix4::transformInverseDirection (hkVector4Parameter directionIn, hkVector4& directionOut) const
{
	hkMatrix4	t; t.setTranspose(*this);
	t.transformDirection(directionIn,directionOut);
}

HK_FORCE_INLINE void hkMatrix4::setMulAffine ( const hkMatrix4& a, const hkMatrix4& b )
{
	HK_WARN_ON_DEBUG_IF(!a.isAffineTransformation(),0x872bbf1d, "Matrix A in setMulAffine() is not an affine transformation");
	HK_WARN_ON_DEBUG_IF(!b.isAffineTransformation(),0x872bbf1e, "Matrix B in setMulAffine() is not an affine transformation");

	// We need to make it alias save
	hkVector4 col0; a.transformDirection(b.m_col0, col0);
	hkVector4 col1; a.transformDirection(b.m_col1, col1);
	hkVector4 col2; a.transformDirection(b.m_col2, col2);
	hkVector4 col3; a.transformPosition(b.m_col3, col3);

	m_col0 = col0; 
	m_col1 = col1; 
	m_col2 = col2; 
	m_col3 = col3;
}

HK_FORCE_INLINE void hkMatrix4::mulAffine ( const hkMatrix4& a )
{
	// SetMul is alias safe
	setMulAffine(*this, a);
}

HK_FORCE_INLINE /*static*/ const hkMatrix4& HK_CALL hkMatrix4::getIdentity()
{
	return reinterpret_cast<const hkMatrix4&>( g_vectorConstants[HK_QUADREAL_1000] );
}

HK_FORCE_INLINE bool hkMatrix4::isApproximatelyEqual( const hkMatrix4& m, hkReal epsilon) const
{
	hkVector4 epsilon_v; epsilon_v.setAll(epsilon);

	hkVector4 t0; t0.setSub(m_col0, m.getColumn<0>());	t0.setAbs( t0 );
	hkVector4 t1; t1.setSub(m_col1, m.getColumn<1>());	t1.setAbs( t1 );
	hkVector4 t2; t2.setSub(m_col2, m.getColumn<2>());	t2.setAbs( t2 );
	hkVector4 t3; t3.setSub(m_col3, m.getColumn<3>());	t3.setAbs( t3 );

	t0.setMax( t0, t1 );
	t2.setMax( t2, t3 );
	t0.setMax(t0, t2 );
	return t0.lessEqual( epsilon_v ).allAreSet();
}

HK_FORCE_INLINE void hkMatrix4::setMul( const hkMatrix4& a, const hkMatrix4& b )
{
	HK_ASSERT(0x6d9d1d43,  this != &a );
	HK_ASSERT(0x64a8df81,  this != &b );

#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	hkVector4 col0, col1, col2, col3;

	col0.setMul( b.m_col0.getComponent<0>(), a.m_col0);
	col1.setMul( b.m_col1.getComponent<0>(), a.m_col0);
	col2.setMul( b.m_col2.getComponent<0>(), a.m_col0);
	col3.setMul( b.m_col3.getComponent<0>(), a.m_col0);

	col0.addMul( b.m_col0.getComponent<1>(), a.m_col1);
	col1.addMul( b.m_col1.getComponent<1>(), a.m_col1);
	col2.addMul( b.m_col2.getComponent<1>(), a.m_col1);
	col3.addMul( b.m_col3.getComponent<1>(), a.m_col1);

	col0.addMul( b.m_col0.getComponent<2>(), a.m_col2);
	col1.addMul( b.m_col1.getComponent<2>(), a.m_col2);
	col2.addMul( b.m_col2.getComponent<2>(), a.m_col2);
	col3.addMul( b.m_col3.getComponent<2>(), a.m_col2);

	col0.addMul( b.m_col0.getComponent<3>(), a.m_col3 );
	col1.addMul( b.m_col1.getComponent<3>(), a.m_col3 );
	col2.addMul( b.m_col2.getComponent<3>(), a.m_col3 );
	col3.addMul( b.m_col3.getComponent<3>(), a.m_col3 );
	
	m_col0 = col0;
	m_col1 = col1;
	m_col2 = col2;
	m_col3 = col3;

#else
	hkMatrix4 aTrans = a;
	aTrans.transpose();

	hkMatrix4& result = *this;

	for (int c=0; c < 4; c++)
	{
		for (int r=0; r < 4; r++)
		{
			result(r,c) = aTrans.getColumn(r).dot<4>( b.getColumn(c) ).getReal();
		}
	}
#endif
}


HK_FORCE_INLINE void hkMatrix4::mul( const hkMatrix4& a )
{
	
	hkMatrix4 temp;
	temp.setMul( *this, a );
	*this = temp;
}

HK_FORCE_INLINE void hkMatrix4::setMul( hkSimdRealParameter scale, const hkMatrix4& a)
{
	m_col0.setMul(a.getColumn<0>(), scale);
	m_col1.setMul(a.getColumn<1>(), scale);
	m_col2.setMul(a.getColumn<2>(), scale);
	m_col3.setMul(a.getColumn<3>(), scale);
}

HK_FORCE_INLINE void hkMatrix4::mul( hkSimdRealParameter scale)
{
	m_col0.mul(scale);
	m_col1.mul(scale);
	m_col2.mul(scale);
	m_col3.mul(scale);
}

HK_FORCE_INLINE void hkMatrix4::resetFourthRow ()
{
	m_col0.zeroComponent<3>();
	m_col1.zeroComponent<3>();
	m_col2.zeroComponent<3>();
	m_col3.setComponent<3>( hkSimdReal::getConstant<HK_QUADREAL_1>() );
}

HK_FORCE_INLINE void hkMatrix4::set(const hkTransform& t)
{
	m_col0 = t.getColumn<0>();
	m_col1 = t.getColumn<1>();
	m_col2 = t.getColumn<2>();
	m_col3 = t.getTranslation();
	resetFourthRow();
}

HK_FORCE_INLINE void hkMatrix4::get(hkTransform& t) const
{
	t.setColumn<0>(m_col0);
	t.setColumn<1>(m_col1);
	t.setColumn<2>(m_col2);
	t.setColumn<3>(m_col3);
}

HK_FORCE_INLINE void hkMatrix4::_add( const hkMatrix4& a )
{
	m_col0.add( a.getColumn<0>() );
	m_col1.add( a.getColumn<1>() );
	m_col2.add( a.getColumn<2>() );
	m_col3.add( a.getColumn<3>() );
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
