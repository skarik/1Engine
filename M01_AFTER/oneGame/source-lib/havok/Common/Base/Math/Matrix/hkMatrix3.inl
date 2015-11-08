/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Common/Base/Math/Vector/hkVector4Util.h>

HK_FORCE_INLINE hkVector4& hkMatrix3::getColumn(int x)
{
#ifdef HK_COMPILER_GHS
	return ((hkVector4*)this)[x];
#else
	return (&m_col0)[x];
#endif	
}

HK_FORCE_INLINE const hkVector4& hkMatrix3::getColumn(int x) const
{
#ifdef HK_COMPILER_GHS
	return ((hkVector4*)this)[x];
#else
	return (&m_col0)[x];
#endif	
}

#if defined(HK_TINY_SPU_ELF)
template <int N>
HK_MATRIX3_INLINE const hkVector4& hkMatrix3::getColumn() const
{
	return getColumn(N);
}
#else
template <int N>
HK_MATRIX3_INLINE const hkVector4& hkMatrix3::getColumn() const
{
#ifdef HK_COMPILER_GHS
	return ((hkVector4*)this)[N];
#else
	return (&m_col0)[N];
#endif
}
#endif

template <int N>
HK_MATRIX3_INLINE hkVector4& hkMatrix3::getColumn()
{
	return (&m_col0)[N];
}

template <int N> 
HK_MATRIX3_INLINE void hkMatrix3::setColumn(hkVector4Parameter c)
{
	(&m_col0)[N] = c;
}


#ifndef HK_DISABLE_MATH_CONSTRUCTORS
HK_FORCE_INLINE hkMatrix3::hkMatrix3( const hkMatrix3& t )
{
	m_col0 = t.getColumn<0>();
	m_col1 = t.getColumn<1>();
	m_col2 = t.getColumn<2>();
}
#endif


HK_FORCE_INLINE hkReal& hkMatrix3::operator() (int r, int c)
{
	return getColumn(c)(r);
}

HK_FORCE_INLINE const hkReal& hkMatrix3::operator() (int r, int c) const
{
	return getColumn(c)(r);
}


template <int ROW, int COL> 
HK_FORCE_INLINE const hkSimdReal hkMatrix3::getElement() const
{
// our reflection parser cannot compile this:	return getColumn<COL>().getComponent<ROW>();
	return (&m_col0)[COL].getComponent<ROW>();
}

template <int ROW, int COL>
HK_FORCE_INLINE void hkMatrix3::setElement(hkSimdRealParameter element)
{
	(&m_col0)[COL].setComponent<ROW>(element);
}


HK_FORCE_INLINE void hkMatrix3::setRows( hkVector4Parameter r0, hkVector4Parameter r1, hkVector4Parameter r2)
{
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	m_col0 = r0;
	m_col1 = r1;
	m_col2 = r2;
	HK_TRANSPOSE3(m_col0,m_col1,m_col2);
#else
	hkVector4 c0; c0.set( r0(0), r1(0), r2(0) );
	hkVector4 c1; c1.set( r0(1), r1(1), r2(1) );
	hkVector4 c2; c2.set( r0(2), r1(2), r2(2) );
	hkVector4 c3; c3.set( r0(3), r1(3), r2(3) );

	setColumn<0>(c0);
	setColumn<1>(c1);
	setColumn<2>(c2);
#endif
}


HK_FORCE_INLINE void hkMatrix3::getRows( hkVector4& r0, hkVector4& r1, hkVector4& r2) const
{
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	hkVector4 c0 = getColumn<0>();
	hkVector4 c1 = getColumn<1>();
	hkVector4 c2 = getColumn<2>();

	HK_TRANSPOSE3(c0,c1,c2);
#else
	hkVector4 c0; c0.set( m_col0(0), m_col1(0), m_col2(0) );
	hkVector4 c1; c1.set( m_col0(1), m_col1(1), m_col2(1) );
	hkVector4 c2; c2.set( m_col0(2), m_col1(2), m_col2(2) );
#endif

	r0 = c0;
	r1 = c1;
	r2 = c2;
}

HK_FORCE_INLINE void hkMatrix3::setCols( hkVector4Parameter r0, hkVector4Parameter r1, hkVector4Parameter r2)
{
	m_col0 = r0;
	m_col1 = r1;
	m_col2 = r2;
}

HK_FORCE_INLINE void hkMatrix3::operator= ( const hkMatrix3& a )
{
	m_col0 = a.getColumn<0>();
	m_col1 = a.getColumn<1>();
	m_col2 = a.getColumn<2>();
}

HK_FORCE_INLINE void hkMatrix3::getCols( hkVector4& r0, hkVector4& r1, hkVector4& r2) const
{
	r0 = m_col0;
	r1 = m_col1;
	r2 = m_col2;
}

HK_FORCE_INLINE void hkMatrix3::getRow( int row, hkVector4& r) const
{
	r.set( m_col0(row), m_col1(row), m_col2(row) );
}

template <int I>
HK_FORCE_INLINE void hkMatrix3::getRow( hkVector4& r ) const
{
	r.set( getElement<I,0>(), getElement<I,1>(), getElement<I,2>(), getElement<I,2>() );
}

	/// copy and transpose 
HK_FORCE_INLINE void hkMatrix3::_setTranspose( const hkMatrix3& s )
{
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	hkVector4 c0 = s.getColumn<0>();
	hkVector4 c1 = s.getColumn<1>();
	hkVector4 c2 = s.getColumn<2>();
	HK_TRANSPOSE3(c0,c1,c2);
	m_col0 = c0;
	m_col1 = c1;
	m_col2 = c2;
#else
	hkMatrix3	cs = s;
	m_col0.set( cs.getElement<0,0>(), cs.getElement<0,1>(), cs.getElement<0,2>(), cs.getElement<0,2>() );
	m_col1.set( cs.getElement<1,0>(), cs.getElement<1,1>(), cs.getElement<1,2>(), cs.getElement<1,2>() );
	m_col2.set( cs.getElement<2,0>(), cs.getElement<2,1>(), cs.getElement<2,2>(), cs.getElement<2,2>() );
#endif
}

//
//	Sets this matrix to be the product of a diagonal matrix (given as a vector) and matrix a (this = diag * a)

HK_FORCE_INLINE void hkMatrix3::setDiagonalMul(hkVector4Parameter vDiag, const hkMatrix3& a)
{
	m_col0.setMul(vDiag, a.getColumn<0>());
	m_col1.setMul(vDiag, a.getColumn<1>());
	m_col2.setMul(vDiag, a.getColumn<2>());
}

//
//	Set this matrix to be the product of qa and b.  (this = qa * b)

HK_FORCE_INLINE void hkMatrix3::setMul(hkQuaternionParameter qa, const hkMatrix3& b)
{
#if defined(HK_TINY_SPU_ELF)
	m_col0.setRotatedDir(qa, b.getColumn<0>());
	m_col1.setRotatedDir(qa, b.getColumn<1>());
	m_col2.setRotatedDir(qa, b.getColumn<2>());
#else
	m_col0._setRotatedDir(qa, b.getColumn<0>());
	m_col1._setRotatedDir(qa, b.getColumn<1>());
	m_col2._setRotatedDir(qa, b.getColumn<2>());
#endif
}

/// aTc = aTb * bTc
HK_FORCE_INLINE void hkMatrix3::_setMul( const hkMatrix3& aTb, const hkMatrix3& bTc )
{
	HK_ASSERT(0x6d9d1d43,  this != &aTb );

#if defined(HK_TINY_SPU_ELF)
	m_col0.setRotatedDir(aTb, bTc.getColumn<0>());
	m_col1.setRotatedDir(aTb, bTc.getColumn<1>());
	m_col2.setRotatedDir(aTb, bTc.getColumn<2>());
#else
	m_col0._setRotatedDir(aTb, bTc.getColumn<0>());
	m_col1._setRotatedDir(aTb, bTc.getColumn<1>());
	m_col2._setRotatedDir(aTb, bTc.getColumn<2>());
#endif
}

HK_FORCE_INLINE void hkMatrix3::_setMul( hkSimdRealParameter scale, const hkMatrix3& a)
{
	m_col0.setMul(scale, a.getColumn<0>());
	m_col1.setMul(scale, a.getColumn<1>());
	m_col2.setMul(scale, a.getColumn<2>());
}

HK_FORCE_INLINE void hkMatrix3::_add( const hkMatrix3& a )
{
	m_col0.add( a.getColumn<0>() );
	m_col1.add( a.getColumn<1>() );
	m_col2.add( a.getColumn<2>() );
}

HK_FORCE_INLINE void hkMatrix3::_sub( const hkMatrix3& a )
{
	m_col0.sub( a.getColumn<0>() );
	m_col1.sub( a.getColumn<1>() );
	m_col2.sub( a.getColumn<2>() );
}

HK_FORCE_INLINE void hkMatrix3::_invertSymmetric()
{
	hkVector4 r0; r0.setCross( m_col1, m_col2 );
    hkVector4 r1; r1.setCross( m_col2, m_col0 );
    hkVector4 r2; r2.setCross( m_col0, m_col1 );

    const hkSimdReal determinant = m_col0.dot<3>(r0);
	HK_ASSERT2( 0xf0140200, determinant.getReal() > HK_REAL_EPSILON, "You cannot invert this matrix");

	hkSimdReal dinv; dinv.setReciprocal(determinant);
	m_col0.setMul(r0, dinv);
	m_col1.setMul(r1, dinv);
	m_col2.setMul(r2, dinv);
}

HK_FORCE_INLINE void hkMatrix3::_mul( hkSimdRealParameter scale)
{
	m_col0.mul(scale);
	m_col1.mul(scale);
	m_col2.mul(scale);
}

//
//	Sets the current matrix to be the inverse of the given matrix. Matrix src is assumed to be symmetric non-singular. If the matrix
//	is singular (up to machine precision), the inverse will be set to zero.

HK_FORCE_INLINE void hkMatrix3::setInverseSymmetric(const hkMatrix3& src)
{
	// Get source columns
	const hkVector4 c0 = src.getColumn<0>();
	const hkVector4 c1 = src.getColumn<1>();
	const hkVector4 c2 = src.getColumn<2>();

	hkVector4 r0; r0.setCross(c1, c2);
	hkVector4 r1; r1.setCross(c2, c0);
	hkVector4 r2; r2.setCross(c0, c1);

	// Compute 1 / determinant. Set it to zero in case of singular matrices!
	
	const hkSimdReal determinant = c0.dot<3>(r0);
	const hkVector4Comparison cmp = determinant.greater(hkSimdReal::getConstant<HK_QUADREAL_EPS>());

	hkSimdReal rec;	rec.setReciprocal(determinant);
	hkSimdReal dInv; dInv.setSelect(cmp, rec, hkSimdReal::getConstant<HK_QUADREAL_0>());

	m_col0.setMul(r0, dInv);
	m_col1.setMul(r1, dInv);
	m_col2.setMul(r2, dInv);
}

//
//	Solves the linear system: (this) * v = b, for v. The matrix is assumed non-singular and symmetric

HK_FORCE_INLINE void hkMatrix3::solveSymmetric(hkVector4Parameter b, hkVector4& v)
{
	// Invert matrix
	hkMatrix3 inv;
	inv.setInverseSymmetric(*this);

	// v = inv * b
	inv.multiplyVector(b, v);
}

//
//	Sets this matrix as the cross product between the given vector and each of the given matrix' columns
//	Specifically, it sets this.getColumn(i) = cross(v, src.getColumn(i)

HK_FORCE_INLINE void hkMatrix3::setCross(hkVector4Parameter v, const hkMatrix3& src)
{
	// Get source columns
	const hkVector4 sc0 = src.getColumn<0>();
	const hkVector4 sc1 = src.getColumn<1>();
	const hkVector4 sc2 = src.getColumn<2>();

	// Target columns
	hkVector4 tc0, tc1, tc2;

#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	// First part
	{
		hkVector4 shufV;	// (vy, vz, vx, *)
		shufV.setPermutation<hkVectorPermutation::YZXW>(v);

		hkVector4 shufC0;	// (c0z, c0x, c0y)
		shufC0.setPermutation<hkVectorPermutation::ZXYW>(sc0);

		hkVector4 shufC1;	// (c1z, c1x, c1y)
		shufC1.setPermutation<hkVectorPermutation::ZXYW>(sc1);

		hkVector4 shufC2;	// (c2z, c2x, c2y)
		shufC2.setPermutation<hkVectorPermutation::ZXYW>(sc2);

		tc0.setMul(shufV, shufC0);	// (vy * c0z, vz * c0x, vx * c0y)
		tc1.setMul(shufV, shufC1);	// (vy * c1z, vz * c1x, vx * c1y)
		tc2.setMul(shufV, shufC2);	// (vy * c2z, vz * c2x, vx * c2y)
	}

	// Second part
	{
		hkVector4 shufV;	// (vz, vx, vy, *)
		shufV.setPermutation<hkVectorPermutation::ZXYW>(v);

		hkVector4 shufC0;	// (c0y, c0z, c0x)
		shufC0.setPermutation<hkVectorPermutation::YZXW>(sc0);

		hkVector4 shufC1;	// (c1y, c1z, c1x)
		shufC1.setPermutation<hkVectorPermutation::YZXW>(sc1);

		hkVector4 shufC2;	// (c2y, c2z, c2x)
		shufC2.setPermutation<hkVectorPermutation::YZXW>(sc2);

		tc0.subMul(shufV, shufC0);	// (vy * c0z - vz * c0y, vz * c0x - vx * c0z, vx * c0y - vy * c0x)
		tc1.subMul(shufV, shufC1);	// (vy * c1z - vz * c1y, vz * c1x - vx * c1z, vx * c1y - vy * c1x)
		tc2.subMul(shufV, shufC2);	// (vy * c2z - vz * c2y, vz * c2x - vx * c2z, vx * c2y - vy * c2x)
	}
#else
	tc0.set( v(1) * sc0(2) - v(2) * sc0(1), v(2) * sc0(0) - v(0) * sc0(2), v(0) * sc0(1) - v(1) * sc0(0) );
	tc1.set( v(1) * sc1(2) - v(2) * sc1(1), v(2) * sc1(0) - v(0) * sc1(2), v(0) * sc1(1) - v(1) * sc1(0) );
	tc2.set( v(1) * sc2(2) - v(2) * sc2(1), v(2) * sc2(0) - v(0) * sc2(2), v(0) * sc2(1) - v(1) * sc2(0) );
#endif

	// Store
	m_col0 = tc0;
	m_col1 = tc1;
	m_col2 = tc2;
}

//
//	Sets this matrix to be the reflection of matrix D through matrix P, i.e. this = Transpose(P) * A * P
//	Matrix D is assumed diagonal.

HK_FORCE_INLINE void hkMatrix3::setReflectedDiagonal(const hkMatrix3& P, hkVector4Parameter D)
{
	// Resulting matrix is symmetric and element(i, j) = dot3(P(*, i) * P(*, j), D)
	// Get columns of p
	const hkVector4 p0 = P.getColumn<0>();
	const hkVector4 p1 = P.getColumn<1>();
	const hkVector4 p2 = P.getColumn<2>();

	// Compute dots
	hkVector4 p00;	p00.setMul(p0, p0);
	hkVector4 p01;	p01.setMul(p0, p1);
	hkVector4 p02;	p02.setMul(p0, p2);
	hkVector4 p11;	p11.setMul(p1, p1);
	hkVector4 p12;	p12.setMul(p1, p2);
	hkVector4 p22;	p22.setMul(p2, p2);
	
	hkVector4 dotsA;	hkVector4Util::dot3_1vs4(D, p00, p01, p02, p11, dotsA);	// (m00, m01, m02, m11)
	hkVector4 dotsB;	hkVector4Util::dot3_1vs4(D, p02, p12, p22, p11, dotsB);	// (m02, m12, m22, m11)
	
	m_col0 = dotsA;
	m_col2 = dotsB;

#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	// dotsA = (m01, m11, m02, m00)
	dotsA.setPermutation<hkVectorPermutation::YWZX>(dotsA);

	// dotsB = (m12, m12, m12, m12)
	dotsB.broadcast<1>();

	hkVector4Comparison cmp;
	cmp.set(hkVector4Comparison::MASK_XY);
	m_col1.setSelect(cmp, dotsA, dotsB);
#else
	m_col1.set(dotsA(1), dotsA(3), dotsB(1), dotsB(1));
#endif
}

//
//	Adds the same value to all elements of the diagonal

HK_FORCE_INLINE void hkMatrix3::addDiagonal(hkSimdRealParameter diag)
{
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	m_col0.addMul(diag, hkVector4::getConstant<HK_QUADREAL_1000>());
	m_col1.addMul(diag, hkVector4::getConstant<HK_QUADREAL_0100>());
	m_col2.addMul(diag, hkVector4::getConstant<HK_QUADREAL_0010>());
#else
	const hkReal d = diag.getReal();
	m_col0(0) += d;
	m_col1(1) += d;
	m_col2(2) += d;
#endif
}

HK_FORCE_INLINE void hkMatrix3::multiplyVector (hkVector4Parameter v, hkVector4& resultOut) const
{
	const hkSimdReal xb = v.getComponent<0>();
	const hkSimdReal yb = v.getComponent<1>();
	const hkSimdReal zb = v.getComponent<2>();

	hkVector4 t0;
	t0.setMul( xb, m_col0 );
	t0.addMul( yb, m_col1 );
	t0.addMul( zb, m_col2 );

	resultOut = t0;
}

//
//	Add the product of a and scale (this += a * scale)

HK_FORCE_INLINE void hkMatrix3::_addMul( hkSimdRealParameter scale, const hkMatrix3& a)
{
	m_col0.addMul(scale, a.getColumn<0>());
	m_col1.addMul(scale, a.getColumn<1>());
	m_col2.addMul(scale, a.getColumn<2>());
}

HK_FORCE_INLINE void hkMatrix3::setZero()
{
	m_col0.setZero();
	m_col1.setZero();
	m_col2.setZero();
}

HK_FORCE_INLINE void hkMatrix3::setDiagonal( hkReal m00, hkReal m11, hkReal m22 )
{
	setZero();
	m_col0(0) = m00;
	m_col1(1) = m11;
	m_col2(2) = m22;
}

HK_FORCE_INLINE void hkMatrix3::setDiagonal( hkSimdRealParameter m00, hkSimdRealParameter m11, hkSimdRealParameter m22 )
{
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	m_col0.setMul(hkVector4::getConstant<HK_QUADREAL_1000>(), m00);
	m_col1.setMul(hkVector4::getConstant<HK_QUADREAL_0100>(), m11);
	m_col2.setMul(hkVector4::getConstant<HK_QUADREAL_0010>(), m22);
#else
	setZero();
	m_col0(0) = m00.getReal();
	m_col1(1) = m11.getReal();
	m_col2(2) = m22.getReal();
#endif
}

HK_FORCE_INLINE void hkMatrix3::setDiagonal( hkVector4Parameter vDiagonal )
{
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	m_col0.setMul(hkVector4::getConstant<HK_QUADREAL_1000>(), vDiagonal);
	m_col1.setMul(hkVector4::getConstant<HK_QUADREAL_0100>(), vDiagonal);
	m_col2.setMul(hkVector4::getConstant<HK_QUADREAL_0010>(), vDiagonal);
#else
	setZero();
	m_col0(0) = vDiagonal(0);
	m_col1(1) = vDiagonal(1);
	m_col2(2) = vDiagonal(2);
#endif
}

HK_FORCE_INLINE void hkMatrix3::setDiagonal(hkSimdRealParameter diag)
{
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	m_col0.setMul(hkVector4::getConstant<HK_QUADREAL_1000>(), diag);
	m_col1.setMul(hkVector4::getConstant<HK_QUADREAL_0100>(), diag);
	m_col2.setMul(hkVector4::getConstant<HK_QUADREAL_0010>(), diag);
#else
	setZero();
	const hkReal d = diag.getReal();
	m_col0(0) = d;
	m_col1(1) = d;
	m_col2(2) = d;
#endif
}

HK_FORCE_INLINE	void hkMatrix3::getDiagonal(hkVector4& diagonalOut) const
{
	diagonalOut.set(getElement<0,0>(), getElement<1,1>(), getElement<2,2>(), hkSimdReal::getConstant<HK_QUADREAL_0>());
}

HK_FORCE_INLINE void hkMatrix3::setIdentity()
{
	hkMatrix3* HK_RESTRICT d = this;
	d->m_col0 = hkVector4::getConstant<HK_QUADREAL_1000>();
	d->m_col1 = hkVector4::getConstant<HK_QUADREAL_0100>();
	d->m_col2 = hkVector4::getConstant<HK_QUADREAL_0010>();
}

HK_FORCE_INLINE /*static*/ const hkMatrix3& HK_CALL hkMatrix3::getIdentity()
{
	union { const hkQuadReal* r; const hkMatrix3* m; } r2m;
	r2m.r = g_vectorConstants + HK_QUADREAL_1000;
	return *r2m.m;
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
