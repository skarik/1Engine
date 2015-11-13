/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/* quad, here for inlining */

#ifndef HK_DISABLE_MATH_CONSTRUCTORS
HK_FORCE_INLINE hkVector4::hkVector4(hkReal a, hkReal b, hkReal c, hkReal d)
{
	m_quad.v[0] = a;
	m_quad.v[1] = b;
	m_quad.v[2] = c;
	m_quad.v[3] = d;
}

HK_FORCE_INLINE hkVector4::hkVector4(const hkQuadReal& q)
{
	hkMemUtil::memCpyOneAligned<4*sizeof(hkReal), 16>(&m_quad, &q);
}

HK_FORCE_INLINE hkVector4::hkVector4( const hkVector4& v)
{
	hkMemUtil::memCpyOneAligned<4*sizeof(hkReal), 16>(&m_quad, &v.m_quad);
}
#endif

HK_FORCE_INLINE void hkVector4::set(hkReal a, hkReal b, hkReal c, hkReal d)
{
	m_quad.v[0] = a;
	m_quad.v[1] = b;
	m_quad.v[2] = c;
	m_quad.v[3] = d;
}

HK_FORCE_INLINE void hkVector4::set( hkSimdRealParameter a, hkSimdRealParameter b, hkSimdRealParameter c, hkSimdRealParameter d )
{
	m_quad.v[0] = a.getReal();
	m_quad.v[1] = b.getReal();
	m_quad.v[2] = c.getReal();
	m_quad.v[3] = d.getReal();
}

HK_FORCE_INLINE void hkVector4::setAll(const hkReal& a)
{
	m_quad.v[0] = a;
	m_quad.v[1] = a;
	m_quad.v[2] = a;
	m_quad.v[3] = a;
}

HK_FORCE_INLINE void hkVector4::setAll(hkSimdRealParameter a)
{
	setAll( a.getReal() );
}

HK_FORCE_INLINE void hkVector4::setZero()
{
	m_quad.v[0] = hkReal(0);
	m_quad.v[1] = hkReal(0);
	m_quad.v[2] = hkReal(0);
	m_quad.v[3] = hkReal(0);
}

template <int I> 
HK_FORCE_INLINE void hkVector4::zeroComponent()
{
	HK_VECTOR4_SUBINDEX_CHECK;
	m_quad.v[I] = hkReal(0);
}

HK_FORCE_INLINE void hkVector4::zeroComponent(const int i)
{
	HK_ASSERT2(0x3bc36625, (i>=0) && (i<4), "Component index out of range");
	m_quad.v[i] = hkReal(0);
}

HK_FORCE_INLINE void hkVector4::setAdd(hkVector4Parameter v0, hkVector4Parameter v1)
{
	m_quad.v[0] = v0.m_quad.v[0] + v1.m_quad.v[0];
	m_quad.v[1] = v0.m_quad.v[1] + v1.m_quad.v[1];
	m_quad.v[2] = v0.m_quad.v[2] + v1.m_quad.v[2];
	m_quad.v[3] = v0.m_quad.v[3] + v1.m_quad.v[3];
}

HK_FORCE_INLINE void hkVector4::setSub(hkVector4Parameter v0, hkVector4Parameter v1)
{
	m_quad.v[0] = v0.m_quad.v[0] - v1.m_quad.v[0];
	m_quad.v[1] = v0.m_quad.v[1] - v1.m_quad.v[1];
	m_quad.v[2] = v0.m_quad.v[2] - v1.m_quad.v[2];
	m_quad.v[3] = v0.m_quad.v[3] - v1.m_quad.v[3];
}

HK_FORCE_INLINE void hkVector4::setMul(hkVector4Parameter v0, hkVector4Parameter v1)
{
	m_quad.v[0] = v0.m_quad.v[0] * v1.m_quad.v[0];
	m_quad.v[1] = v0.m_quad.v[1] * v1.m_quad.v[1];
	m_quad.v[2] = v0.m_quad.v[2] * v1.m_quad.v[2];
	m_quad.v[3] = v0.m_quad.v[3] * v1.m_quad.v[3];
}

HK_FORCE_INLINE void hkVector4::setMul(hkVector4Parameter a, hkSimdRealParameter rs)
{
	const hkReal r = rs.getReal();
	m_quad.v[0] = r * a.m_quad.v[0];
	m_quad.v[1] = r * a.m_quad.v[1];
	m_quad.v[2] = r * a.m_quad.v[2];
	m_quad.v[3] = r * a.m_quad.v[3];
}

HK_FORCE_INLINE void hkVector4::setSubMul(hkVector4Parameter a, hkVector4Parameter b, hkSimdRealParameter r)
{
	const hkReal rr = r.getReal();
	m_quad.v[0] = a.m_quad.v[0] - rr * b.m_quad.v[0];
	m_quad.v[1] = a.m_quad.v[1] - rr * b.m_quad.v[1];
	m_quad.v[2] = a.m_quad.v[2] - rr * b.m_quad.v[2];
	m_quad.v[3] = a.m_quad.v[3] - rr * b.m_quad.v[3];
}

HK_FORCE_INLINE void hkVector4::setAddMul(hkVector4Parameter a, hkVector4Parameter b, hkSimdRealParameter r)
{
	const hkReal rr = r.getReal();
	m_quad.v[0] = a.m_quad.v[0] + rr * b.m_quad.v[0];
	m_quad.v[1] = a.m_quad.v[1] + rr * b.m_quad.v[1];
	m_quad.v[2] = a.m_quad.v[2] + rr * b.m_quad.v[2];
	m_quad.v[3] = a.m_quad.v[3] + rr * b.m_quad.v[3];
}

HK_FORCE_INLINE void hkVector4::setAddMul(hkVector4Parameter a, hkVector4Parameter m0, hkVector4Parameter m1)
{
	m_quad.v[0] = a.m_quad.v[0] + m0.m_quad.v[0] * m1.m_quad.v[0];
	m_quad.v[1] = a.m_quad.v[1] + m0.m_quad.v[1] * m1.m_quad.v[1];
	m_quad.v[2] = a.m_quad.v[2] + m0.m_quad.v[2] * m1.m_quad.v[2];
	m_quad.v[3] = a.m_quad.v[3] + m0.m_quad.v[3] * m1.m_quad.v[3];
}

HK_FORCE_INLINE void hkVector4::setSubMul(hkVector4Parameter a, hkVector4Parameter m0, hkVector4Parameter m1)
{
	m_quad.v[0] = a.m_quad.v[0] - m0.m_quad.v[0] * m1.m_quad.v[0];
	m_quad.v[1] = a.m_quad.v[1] - m0.m_quad.v[1] * m1.m_quad.v[1];
	m_quad.v[2] = a.m_quad.v[2] - m0.m_quad.v[2] * m1.m_quad.v[2];
	m_quad.v[3] = a.m_quad.v[3] - m0.m_quad.v[3] * m1.m_quad.v[3];
}

HK_FORCE_INLINE void hkVector4::setCross( hkVector4Parameter v1, hkVector4Parameter v2 )
{
	const hkReal nx = v1.m_quad.v[1]*v2.m_quad.v[2] - v1.m_quad.v[2]*v2.m_quad.v[1];
	const hkReal ny = v1.m_quad.v[2]*v2.m_quad.v[0] - v1.m_quad.v[0]*v2.m_quad.v[2];
	const hkReal nz = v1.m_quad.v[0]*v2.m_quad.v[1] - v1.m_quad.v[1]*v2.m_quad.v[0];
	set( nx, ny, nz , hkReal(0) );
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::equal(hkVector4Parameter a) const
{
	hkVector4Comparison ret;
	ret.m_mask =
		((m_quad.v[0]==a.m_quad.v[0]) ? hkVector4Comparison::MASK_X : hkVector4Comparison::MASK_NONE) |
		((m_quad.v[1]==a.m_quad.v[1]) ? hkVector4Comparison::MASK_Y : hkVector4Comparison::MASK_NONE) |
		((m_quad.v[2]==a.m_quad.v[2]) ? hkVector4Comparison::MASK_Z : hkVector4Comparison::MASK_NONE) |
		((m_quad.v[3]==a.m_quad.v[3]) ? hkVector4Comparison::MASK_W : hkVector4Comparison::MASK_NONE);
	return ret;
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::notEqual(hkVector4Parameter a) const
{
	hkVector4Comparison ret;
	ret.m_mask =
		((m_quad.v[0]==a.m_quad.v[0]) ? hkVector4Comparison::MASK_NONE : hkVector4Comparison::MASK_X) |
		((m_quad.v[1]==a.m_quad.v[1]) ? hkVector4Comparison::MASK_NONE : hkVector4Comparison::MASK_Y) |
		((m_quad.v[2]==a.m_quad.v[2]) ? hkVector4Comparison::MASK_NONE : hkVector4Comparison::MASK_Z) |
		((m_quad.v[3]==a.m_quad.v[3]) ? hkVector4Comparison::MASK_NONE : hkVector4Comparison::MASK_W);
	return ret;
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::less(hkVector4Parameter a) const
{
	hkVector4Comparison ret;
	ret.m_mask =
		((m_quad.v[0]<a.m_quad.v[0]) ? hkVector4Comparison::MASK_X : hkVector4Comparison::MASK_NONE) |
		((m_quad.v[1]<a.m_quad.v[1]) ? hkVector4Comparison::MASK_Y : hkVector4Comparison::MASK_NONE) |
		((m_quad.v[2]<a.m_quad.v[2]) ? hkVector4Comparison::MASK_Z : hkVector4Comparison::MASK_NONE) |
		((m_quad.v[3]<a.m_quad.v[3]) ? hkVector4Comparison::MASK_W : hkVector4Comparison::MASK_NONE);
	return ret;
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::lessEqual(hkVector4Parameter a) const
{
	hkVector4Comparison ret;
	ret.m_mask =
		((m_quad.v[0]<=a.m_quad.v[0]) ? hkVector4Comparison::MASK_X : hkVector4Comparison::MASK_NONE) |
		((m_quad.v[1]<=a.m_quad.v[1]) ? hkVector4Comparison::MASK_Y : hkVector4Comparison::MASK_NONE) |
		((m_quad.v[2]<=a.m_quad.v[2]) ? hkVector4Comparison::MASK_Z : hkVector4Comparison::MASK_NONE) |
		((m_quad.v[3]<=a.m_quad.v[3]) ? hkVector4Comparison::MASK_W : hkVector4Comparison::MASK_NONE);
	return ret;
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::greater(hkVector4Parameter a) const
{
	hkVector4Comparison ret;
	ret.m_mask =
		((m_quad.v[0]>a.m_quad.v[0]) ? hkVector4Comparison::MASK_X : hkVector4Comparison::MASK_NONE) |
		((m_quad.v[1]>a.m_quad.v[1]) ? hkVector4Comparison::MASK_Y : hkVector4Comparison::MASK_NONE) |
		((m_quad.v[2]>a.m_quad.v[2]) ? hkVector4Comparison::MASK_Z : hkVector4Comparison::MASK_NONE) |
		((m_quad.v[3]>a.m_quad.v[3]) ? hkVector4Comparison::MASK_W : hkVector4Comparison::MASK_NONE);
	return ret;
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::greaterEqual(hkVector4Parameter a) const
{
	hkVector4Comparison ret;
	ret.m_mask =
		((m_quad.v[0]>=a.m_quad.v[0]) ? hkVector4Comparison::MASK_X : hkVector4Comparison::MASK_NONE) |
		((m_quad.v[1]>=a.m_quad.v[1]) ? hkVector4Comparison::MASK_Y : hkVector4Comparison::MASK_NONE) |
		((m_quad.v[2]>=a.m_quad.v[2]) ? hkVector4Comparison::MASK_Z : hkVector4Comparison::MASK_NONE) |
		((m_quad.v[3]>=a.m_quad.v[3]) ? hkVector4Comparison::MASK_W : hkVector4Comparison::MASK_NONE);
	return ret;
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::lessZero() const
{
	hkVector4Comparison ret;
	ret.m_mask =
		((m_quad.v[0]<hkReal(0)) ? hkVector4Comparison::MASK_X : hkVector4Comparison::MASK_NONE) |
		((m_quad.v[1]<hkReal(0)) ? hkVector4Comparison::MASK_Y : hkVector4Comparison::MASK_NONE) |
		((m_quad.v[2]<hkReal(0)) ? hkVector4Comparison::MASK_Z : hkVector4Comparison::MASK_NONE) |
		((m_quad.v[3]<hkReal(0)) ? hkVector4Comparison::MASK_W : hkVector4Comparison::MASK_NONE);
	return ret;
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::lessEqualZero() const
{
	hkVector4Comparison ret;
	ret.m_mask =
		((m_quad.v[0]<=hkReal(0)) ? hkVector4Comparison::MASK_X : hkVector4Comparison::MASK_NONE) |
		((m_quad.v[1]<=hkReal(0)) ? hkVector4Comparison::MASK_Y : hkVector4Comparison::MASK_NONE) |
		((m_quad.v[2]<=hkReal(0)) ? hkVector4Comparison::MASK_Z : hkVector4Comparison::MASK_NONE) |
		((m_quad.v[3]<=hkReal(0)) ? hkVector4Comparison::MASK_W : hkVector4Comparison::MASK_NONE);
	return ret;
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::greaterZero() const
{
	hkVector4Comparison ret;
	ret.m_mask =
		((m_quad.v[0]>hkReal(0)) ? hkVector4Comparison::MASK_X : hkVector4Comparison::MASK_NONE) |
		((m_quad.v[1]>hkReal(0)) ? hkVector4Comparison::MASK_Y : hkVector4Comparison::MASK_NONE) |
		((m_quad.v[2]>hkReal(0)) ? hkVector4Comparison::MASK_Z : hkVector4Comparison::MASK_NONE) |
		((m_quad.v[3]>hkReal(0)) ? hkVector4Comparison::MASK_W : hkVector4Comparison::MASK_NONE);
	return ret;
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::greaterEqualZero() const
{
	hkVector4Comparison ret;
	ret.m_mask =
		((m_quad.v[0]>=hkReal(0)) ? hkVector4Comparison::MASK_X : hkVector4Comparison::MASK_NONE) |
		((m_quad.v[1]>=hkReal(0)) ? hkVector4Comparison::MASK_Y : hkVector4Comparison::MASK_NONE) |
		((m_quad.v[2]>=hkReal(0)) ? hkVector4Comparison::MASK_Z : hkVector4Comparison::MASK_NONE) |
		((m_quad.v[3]>=hkReal(0)) ? hkVector4Comparison::MASK_W : hkVector4Comparison::MASK_NONE);
	return ret;
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::equalZero() const
{
	hkVector4Comparison ret;
	ret.m_mask =
		((m_quad.v[0]==hkReal(0)) ? hkVector4Comparison::MASK_X : hkVector4Comparison::MASK_NONE) |
		((m_quad.v[1]==hkReal(0)) ? hkVector4Comparison::MASK_Y : hkVector4Comparison::MASK_NONE) |
		((m_quad.v[2]==hkReal(0)) ? hkVector4Comparison::MASK_Z : hkVector4Comparison::MASK_NONE) |
		((m_quad.v[3]==hkReal(0)) ? hkVector4Comparison::MASK_W : hkVector4Comparison::MASK_NONE);
	return ret;
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::notEqualZero() const
{
	hkVector4Comparison ret;
	ret.m_mask =
		((m_quad.v[0]==hkReal(0)) ? hkVector4Comparison::MASK_NONE : hkVector4Comparison::MASK_X) |
		((m_quad.v[1]==hkReal(0)) ? hkVector4Comparison::MASK_NONE : hkVector4Comparison::MASK_Y) |
		((m_quad.v[2]==hkReal(0)) ? hkVector4Comparison::MASK_NONE : hkVector4Comparison::MASK_Z) |
		((m_quad.v[3]==hkReal(0)) ? hkVector4Comparison::MASK_NONE : hkVector4Comparison::MASK_W);
	return ret;
}

HK_FORCE_INLINE void hkVector4::setSelect( hkVector4ComparisonParameter comp, hkVector4Parameter trueValue, hkVector4Parameter falseValue )
{
	m_quad.v[0] = comp.anyIsSet(hkVector4Comparison::MASK_X) ? trueValue.m_quad.v[0] : falseValue.m_quad.v[0];
	m_quad.v[1] = comp.anyIsSet(hkVector4Comparison::MASK_Y) ? trueValue.m_quad.v[1] : falseValue.m_quad.v[1];
	m_quad.v[2] = comp.anyIsSet(hkVector4Comparison::MASK_Z) ? trueValue.m_quad.v[2] : falseValue.m_quad.v[2];
	m_quad.v[3] = comp.anyIsSet(hkVector4Comparison::MASK_W) ? trueValue.m_quad.v[3] : falseValue.m_quad.v[3];
}

template <int N>
HK_FORCE_INLINE void hkVector4::setNeg(hkVector4Parameter v)
{
	HK_VECTOR4_UNSUPPORTED_LENGTH_CHECK;
	int i=0;
	for (; i<N; ++i) m_quad.v[i] = -v.m_quad.v[i];
	for (; i<4; ++i) m_quad.v[i] =  v.m_quad.v[i];
}

HK_FORCE_INLINE void hkVector4::setAbs(hkVector4Parameter v)
{
	m_quad.v[0] = hkMath::fabs(v.m_quad.v[0]);
	m_quad.v[1] = hkMath::fabs(v.m_quad.v[1]);
	m_quad.v[2] = hkMath::fabs(v.m_quad.v[2]);
	m_quad.v[3] = hkMath::fabs(v.m_quad.v[3]);
}

HK_FORCE_INLINE void hkVector4::setMin(hkVector4Parameter a, hkVector4Parameter b)
{
	m_quad.v[0] = hkMath::min2(a.m_quad.v[0], b.m_quad.v[0]);
	m_quad.v[1] = hkMath::min2(a.m_quad.v[1], b.m_quad.v[1]);
	m_quad.v[2] = hkMath::min2(a.m_quad.v[2], b.m_quad.v[2]);
	m_quad.v[3] = hkMath::min2(a.m_quad.v[3], b.m_quad.v[3]);
}

HK_FORCE_INLINE void hkVector4::setMax(hkVector4Parameter a, hkVector4Parameter b)
{
	m_quad.v[0] = hkMath::max2(a.m_quad.v[0], b.m_quad.v[0]);
	m_quad.v[1] = hkMath::max2(a.m_quad.v[1], b.m_quad.v[1]);
	m_quad.v[2] = hkMath::max2(a.m_quad.v[2], b.m_quad.v[2]);
	m_quad.v[3] = hkMath::max2(a.m_quad.v[3], b.m_quad.v[3]);
}

HK_FORCE_INLINE void hkVector4::_setRotatedDir(const hkMatrix3& r, hkVector4Parameter v )
{
	const hkSimdReal v0 = v.getComponent<0>();
	const hkSimdReal v1 = v.getComponent<1>();
	const hkSimdReal v2 = v.getComponent<2>();
	set(r.getElement<0,0>()*v0 + r.getElement<0,1>()*v1 + r.getElement<0,2>()*v2 ,
		r.getElement<1,0>()*v0 + r.getElement<1,1>()*v1 + r.getElement<1,2>()*v2 ,
		r.getElement<2,0>()*v0 + r.getElement<2,1>()*v1 + r.getElement<2,2>()*v2 ,
		r.getElement<3,0>()*v0 + r.getElement<3,1>()*v1 + r.getElement<3,2>()*v2 ); // needed to conform with SSE processing
}

HK_FORCE_INLINE void hkVector4::_setRotatedInverseDir(const hkMatrix3& r, hkVector4Parameter v )
{
	const hkSimdReal v0 = v.getComponent<0>();
	const hkSimdReal v1 = v.getComponent<1>();
	const hkSimdReal v2 = v.getComponent<2>();
	set(r.getElement<0,0>()*v0 + r.getElement<1,0>()*v1 + r.getElement<2,0>()*v2 ,
		r.getElement<0,1>()*v0 + r.getElement<1,1>()*v1 + r.getElement<2,1>()*v2 ,
		r.getElement<0,2>()*v0 + r.getElement<1,2>()*v1 + r.getElement<2,2>()*v2 ,
		r.getElement<0,3>()*v0 + r.getElement<1,3>()*v1 + r.getElement<2,3>()*v2 ); // needed to conform with SSE processing
}

template <int N>
HK_FORCE_INLINE const hkSimdReal hkVector4::dot(hkVector4Parameter a) const
{
	HK_VECTOR4_UNSUPPORTED_LENGTH_CHECK;
	hkReal sum = hkReal(0);
	for (int i=0; i<N; ++i) sum += (m_quad.v[i] * a.m_quad.v[i]);
	return hkSimdReal::convert(sum);
}

HK_FORCE_INLINE const hkSimdReal hkVector4::dot4xyz1(hkVector4Parameter a) const
{
	return	hkSimdReal::convert(	(m_quad.v[0] * a.m_quad.v[0]) + 
						(m_quad.v[1] * a.m_quad.v[1]) + 
						(m_quad.v[2] * a.m_quad.v[2]) + 
						 m_quad.v[3]                      );
}

template <int N>
HK_FORCE_INLINE const hkSimdReal hkVector4::horizontalAdd() const
{
	HK_VECTOR4_UNSUPPORTED_LENGTH_CHECK;
	hkReal sum = hkReal(0);
	for (int i=0; i<N; ++i) sum += m_quad.v[i];
	return hkSimdReal::convert(sum);
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::horizontalMax<1>() const
{
	return getComponent<0>();
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::horizontalMax<2>() const
{
	return hkSimdReal::convert(hkMath::max2(m_quad.v[0], m_quad.v[1]));
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::horizontalMax<3>() const
{
	const hkReal m = hkMath::max2(m_quad.v[0], m_quad.v[1]);
	return hkSimdReal::convert(hkMath::max2(m, m_quad.v[2]));
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::horizontalMax<4>() const
{
	const hkReal ma = hkMath::max2(m_quad.v[0], m_quad.v[1]);
	const hkReal mb = hkMath::max2(m_quad.v[2], m_quad.v[3]);
	return hkSimdReal::convert(hkMath::max2(ma, mb));
}

template <int N>
HK_FORCE_INLINE const hkSimdReal hkVector4::horizontalMax() const
{
	HK_VECTOR4_NOT_IMPLEMENTED;
	return hkSimdReal::getConstant<HK_QUADREAL_0>();
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::horizontalMin<1>() const
{
	return getComponent<0>();
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::horizontalMin<2>() const
{
	return hkSimdReal::convert(hkMath::min2(m_quad.v[0], m_quad.v[1]));
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::horizontalMin<3>() const
{
	const hkReal m = hkMath::min2(m_quad.v[0], m_quad.v[1]);
	return hkSimdReal::convert(hkMath::min2(m, m_quad.v[2]));
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::horizontalMin<4>() const
{
	const hkReal ma = hkMath::min2(m_quad.v[0], m_quad.v[1]);
	const hkReal mb = hkMath::min2(m_quad.v[2], m_quad.v[3]);
	return hkSimdReal::convert(hkMath::min2(ma, mb));
}

template <int N>
HK_FORCE_INLINE const hkSimdReal hkVector4::horizontalMin() const
{
	HK_VECTOR4_NOT_IMPLEMENTED;
	return hkSimdReal::getConstant<HK_QUADREAL_0>();
}

/* operator () */

HK_FORCE_INLINE hkReal& hkVector4::operator() (int a)
{
	HK_ASSERT2(0x6d0c31d7, a>=0 && a<4, "index out of bounds for component access");
	return m_quad.v[a];
}

HK_FORCE_INLINE const hkReal& hkVector4::operator() (int a) const
{
	HK_ASSERT2(0x6d0c31d7, a>=0 && a<4, "index out of bounds for component access");
	return const_cast<const hkReal&>(m_quad.v[a]);
}

HK_FORCE_INLINE void hkVector4::setXYZ_W(hkVector4Parameter xyz, hkVector4Parameter ww)
{
	m_quad.v[0] = xyz.m_quad.v[0];
	m_quad.v[1] = xyz.m_quad.v[1];
	m_quad.v[2] = xyz.m_quad.v[2];
	m_quad.v[3] =  ww.m_quad.v[3];
}

HK_FORCE_INLINE void hkVector4::setXYZ_W(hkVector4Parameter xyz, hkSimdRealParameter ww)
{
	m_quad.v[0] = xyz.m_quad.v[0];
	m_quad.v[1] = xyz.m_quad.v[1];
	m_quad.v[2] = xyz.m_quad.v[2];
	m_quad.v[3] = ww.getReal();
}

HK_FORCE_INLINE void hkVector4::setW(hkVector4Parameter w)
{
	m_quad.v[3] = w.m_quad.v[3];
}

HK_FORCE_INLINE void hkVector4::setXYZ(hkVector4Parameter xyz)
{
	m_quad.v[0] = xyz.m_quad.v[0];
	m_quad.v[1] = xyz.m_quad.v[1];
	m_quad.v[2] = xyz.m_quad.v[2];
}

HK_FORCE_INLINE void hkVector4::addXYZ(hkVector4Parameter xyz)
{
	m_quad.v[0] += xyz.m_quad.v[0];
	m_quad.v[1] += xyz.m_quad.v[1];
	m_quad.v[2] += xyz.m_quad.v[2];
#if defined(HK_REAL_IS_DOUBLE)
	HK_ON_DEBUG( *((hkUint64*)&(m_quad.v[3])) = 0xffffffffffffffffull; )
#else
	HK_ON_DEBUG( *((hkUint32*)&(m_quad.v[3])) = 0xffffffff; )
#endif
}

HK_FORCE_INLINE void hkVector4::subXYZ(hkVector4Parameter xyz)
{
	m_quad.v[0] -= xyz.m_quad.v[0];
	m_quad.v[1] -= xyz.m_quad.v[1];
	m_quad.v[2] -= xyz.m_quad.v[2];
#if defined(HK_REAL_IS_DOUBLE)
	HK_ON_DEBUG( *((hkUint64*)&(m_quad.v[3])) = 0xffffffffffffffffull; )
#else
	HK_ON_DEBUG( *((hkUint32*)&(m_quad.v[3])) = 0xffffffff; )
#endif
}

HK_FORCE_INLINE void hkVector4::setXYZ(hkReal v)
{
	m_quad.v[0] = v;
	m_quad.v[1] = v;
	m_quad.v[2] = v;
}

HK_FORCE_INLINE void hkVector4::setXYZ(hkSimdRealParameter vv)
{
	setXYZ( vv.getReal() );
}

HK_FORCE_INLINE void hkVector4::setXYZ_0(hkVector4Parameter xyz)
{
	setXYZ( xyz );
	m_quad.v[3] = hkReal(0);
}

HK_FORCE_INLINE void hkVector4::setBroadcastXYZ(const int i, hkVector4Parameter v)
{
	setXYZ( v.m_quad.v[i] );
#if defined(HK_REAL_IS_DOUBLE)
	HK_ON_DEBUG( *((hkUint64*)&(m_quad.v[3])) = 0xffffffffffffffffull; )
#else
	HK_ON_DEBUG( *((hkUint32*)&(m_quad.v[3])) = 0xffffffff; )
#endif
}

HK_FORCE_INLINE const hkSimdReal hkVector4::getComponent(const int i) const
{
	HK_ASSERT2(0x6d0c31d7, i>=0 && i<4, "index out of bounds for component access");
	return hkSimdReal::convert(m_quad.v[i]);
}

template <int I>
HK_FORCE_INLINE const hkSimdReal hkVector4::getComponent() const
{
	HK_VECTOR4_SUBINDEX_CHECK;
	return hkSimdReal::convert(m_quad.v[I]);
}

HK_FORCE_INLINE void hkVector4::setComponent(const int i, hkSimdRealParameter val)
{
	HK_ASSERT2(0x6d0c31d7, i>=0 && i<4, "index out of bounds for component access");
	m_quad.v[i] = val.getReal();
}

template <int I>
HK_FORCE_INLINE void hkVector4::setComponent(hkSimdRealParameter val)
{
	HK_VECTOR4_SUBINDEX_CHECK;
	m_quad.v[I] = val.getReal();
}

HK_FORCE_INLINE void hkVector4::reduceToHalfPrecision()
{
	static const hkUint32 precisionMask = 0xffff0000;
	const hkUint32* src = reinterpret_cast<const hkUint32*>( &m_quad );
	hkUint32* dest = reinterpret_cast<hkUint32*>( &m_quad );
	dest[0] = src[0] & precisionMask;
	dest[1] = src[1] & precisionMask;
	dest[2] = src[2] & precisionMask;
	dest[3] = src[3] & precisionMask;
}

template <int N> 
HK_FORCE_INLINE hkBool32 hkVector4::isOk() const
{
	HK_VECTOR4_UNSUPPORTED_LENGTH_CHECK;
	for(int i=0; i<N; ++i) 
	{
		if (!hkMath::isFinite(m_quad.v[i])) 
		{
			return false;
		}
	}
	return true;
}

template <> 
HK_FORCE_INLINE void hkVector4::setPermutation<hkVectorPermutation::XYZW>(hkVector4Parameter v)
{
	m_quad = v.m_quad;
}

template <hkVectorPermutation::Permutation P> 
HK_FORCE_INLINE void hkVector4::setPermutation(hkVector4Parameter v)
{
    // Store in regs before writing to the destination - to handle case when v and this are the same
    const hkReal t0 = v.m_quad.v[(P & 0x3000) >> 12];
    const hkReal t1 = v.m_quad.v[(P & 0x0300) >>  8];
    const hkReal t2 = v.m_quad.v[(P & 0x0030) >>  4];
    const hkReal t3 = v.m_quad.v[(P & 0x0003) >>  0];

    m_quad.v[0] = t0;
    m_quad.v[1] = t1;
    m_quad.v[2] = t2;
    m_quad.v[3] = t3;
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::signBitSet() const
{
	hkVector4Comparison mask;
	mask.m_mask  = hkVector4Comparison::MASK_NONE;
	mask.m_mask |= hkMath::signBitSet(m_quad.v[0]) ? hkVector4Comparison::MASK_X : hkVector4Comparison::MASK_NONE;
	mask.m_mask |= hkMath::signBitSet(m_quad.v[1]) ? hkVector4Comparison::MASK_Y : hkVector4Comparison::MASK_NONE;
	mask.m_mask |= hkMath::signBitSet(m_quad.v[2]) ? hkVector4Comparison::MASK_Z : hkVector4Comparison::MASK_NONE;
	mask.m_mask |= hkMath::signBitSet(m_quad.v[3]) ? hkVector4Comparison::MASK_W : hkVector4Comparison::MASK_NONE;
	return mask;
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::signBitClear() const
{
	hkVector4Comparison mask;
	mask.m_mask  = hkVector4Comparison::MASK_NONE;
	mask.m_mask |= hkMath::signBitSet(m_quad.v[0]) ? hkVector4Comparison::MASK_NONE : hkVector4Comparison::MASK_X;
	mask.m_mask |= hkMath::signBitSet(m_quad.v[1]) ? hkVector4Comparison::MASK_NONE : hkVector4Comparison::MASK_Y;
	mask.m_mask |= hkMath::signBitSet(m_quad.v[2]) ? hkVector4Comparison::MASK_NONE : hkVector4Comparison::MASK_Z;
	mask.m_mask |= hkMath::signBitSet(m_quad.v[3]) ? hkVector4Comparison::MASK_NONE : hkVector4Comparison::MASK_W;
	return mask;
}

HK_FORCE_INLINE void hkVector4::setFlipSign(hkVector4Parameter v, hkVector4ComparisonParameter mask)
{
	m_quad.v[0] = mask.anyIsSet(hkVector4Comparison::MASK_X) ? -v.m_quad.v[0] : v.m_quad.v[0];
	m_quad.v[1] = mask.anyIsSet(hkVector4Comparison::MASK_Y) ? -v.m_quad.v[1] : v.m_quad.v[1];
	m_quad.v[2] = mask.anyIsSet(hkVector4Comparison::MASK_Z) ? -v.m_quad.v[2] : v.m_quad.v[2];
	m_quad.v[3] = mask.anyIsSet(hkVector4Comparison::MASK_W) ? -v.m_quad.v[3] : v.m_quad.v[3];
}

HK_FORCE_INLINE void hkVector4::setFlipSign(hkVector4Parameter a, hkVector4Parameter signs)
{
	m_quad.v[0] = hkMath::signBitSet(signs.m_quad.v[0]) ? -a.m_quad.v[0] : a.m_quad.v[0];
	m_quad.v[1] = hkMath::signBitSet(signs.m_quad.v[1]) ? -a.m_quad.v[1] : a.m_quad.v[1];
	m_quad.v[2] = hkMath::signBitSet(signs.m_quad.v[2]) ? -a.m_quad.v[2] : a.m_quad.v[2];
	m_quad.v[3] = hkMath::signBitSet(signs.m_quad.v[3]) ? -a.m_quad.v[3] : a.m_quad.v[3];
}

HK_FORCE_INLINE void hkVector4::setFlipSign(hkVector4Parameter a, hkSimdRealParameter sharedSign)
{
	const bool flip = hkMath::signBitSet(sharedSign.getReal());
	if (flip)
	{
		m_quad.v[0] = -a.m_quad.v[0];
		m_quad.v[1] = -a.m_quad.v[1];
		m_quad.v[2] = -a.m_quad.v[2];
		m_quad.v[3] = -a.m_quad.v[3];
	}
	else
	{
		hkMemUtil::memCpyOneAligned<4*sizeof(hkReal), 16>(&m_quad, &a.m_quad);
	}
}



//
// advanced interface
//

namespace hkVector4_AdvancedInterface
{

template <hkMathAccuracyMode A, hkMathDivByZeroMode D>
struct unroll_setReciprocal { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a)
{
	HK_VECTOR4_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_IGNORE> { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a)
{
	switch (A)
	{
		case HK_ACC_23_BIT: 
			{
				self.v[0] = hkReal(hkMath::rcpF32Approx23Bit(hkFloat32(a.m_quad.v[0])));
				self.v[1] = hkReal(hkMath::rcpF32Approx23Bit(hkFloat32(a.m_quad.v[1])));
				self.v[2] = hkReal(hkMath::rcpF32Approx23Bit(hkFloat32(a.m_quad.v[2])));
				self.v[3] = hkReal(hkMath::rcpF32Approx23Bit(hkFloat32(a.m_quad.v[3])));
			}
			break;
		case HK_ACC_12_BIT: 
			{
				self.v[0] = hkReal(hkMath::rcpF32Approx12Bit(hkFloat32(a.m_quad.v[0])));
				self.v[1] = hkReal(hkMath::rcpF32Approx12Bit(hkFloat32(a.m_quad.v[1])));
				self.v[2] = hkReal(hkMath::rcpF32Approx12Bit(hkFloat32(a.m_quad.v[2])));
				self.v[3] = hkReal(hkMath::rcpF32Approx12Bit(hkFloat32(a.m_quad.v[3])));
			}
			break;
		default:         
			{
				self.v[0] = hkReal(1) / a.m_quad.v[0];
				self.v[1] = hkReal(1) / a.m_quad.v[1];
				self.v[2] = hkReal(1) / a.m_quad.v[2];
				self.v[3] = hkReal(1) / a.m_quad.v[3];
			}
			break; // HK_ACC_FULL
	}
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_SET_ZERO> { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a)
{
	switch (A)
	{
		case HK_ACC_23_BIT: 
			{
				self.v[0] = ((a.m_quad.v[0] == hkReal(0)) ? hkReal(0) : hkReal(hkMath::rcpF32Approx23Bit(hkFloat32(a.m_quad.v[0]))));
				self.v[1] = ((a.m_quad.v[1] == hkReal(0)) ? hkReal(0) : hkReal(hkMath::rcpF32Approx23Bit(hkFloat32(a.m_quad.v[1]))));
				self.v[2] = ((a.m_quad.v[2] == hkReal(0)) ? hkReal(0) : hkReal(hkMath::rcpF32Approx23Bit(hkFloat32(a.m_quad.v[2]))));
				self.v[3] = ((a.m_quad.v[3] == hkReal(0)) ? hkReal(0) : hkReal(hkMath::rcpF32Approx23Bit(hkFloat32(a.m_quad.v[3]))));
			}
			break;
		case HK_ACC_12_BIT: 
			{
				self.v[0] = ((a.m_quad.v[0] == hkReal(0)) ? hkReal(0) : hkReal(hkMath::rcpF32Approx12Bit(hkFloat32(a.m_quad.v[0]))));
				self.v[1] = ((a.m_quad.v[1] == hkReal(0)) ? hkReal(0) : hkReal(hkMath::rcpF32Approx12Bit(hkFloat32(a.m_quad.v[1]))));
				self.v[2] = ((a.m_quad.v[2] == hkReal(0)) ? hkReal(0) : hkReal(hkMath::rcpF32Approx12Bit(hkFloat32(a.m_quad.v[2]))));
				self.v[3] = ((a.m_quad.v[3] == hkReal(0)) ? hkReal(0) : hkReal(hkMath::rcpF32Approx12Bit(hkFloat32(a.m_quad.v[3]))));
			}
			break;
		default:         
			{
				self.v[0] = ((a.m_quad.v[0] == hkReal(0)) ? hkReal(0) : hkReal(1) / a.m_quad.v[0]);
				self.v[1] = ((a.m_quad.v[1] == hkReal(0)) ? hkReal(0) : hkReal(1) / a.m_quad.v[1]);
				self.v[2] = ((a.m_quad.v[2] == hkReal(0)) ? hkReal(0) : hkReal(1) / a.m_quad.v[2]);
				self.v[3] = ((a.m_quad.v[3] == hkReal(0)) ? hkReal(0) : hkReal(1) / a.m_quad.v[3]);
			}
			break; // HK_ACC_FULL
	}
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_SET_HIGH> { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a)
{
	switch (A)
	{
	case HK_ACC_23_BIT: 
		{
			self.v[0] = ((a.m_quad.v[0] == hkReal(0)) ? HK_REAL_HIGH : hkReal(hkMath::rcpF32Approx23Bit(hkFloat32(a.m_quad.v[0]))));
			self.v[1] = ((a.m_quad.v[1] == hkReal(0)) ? HK_REAL_HIGH : hkReal(hkMath::rcpF32Approx23Bit(hkFloat32(a.m_quad.v[1]))));
			self.v[2] = ((a.m_quad.v[2] == hkReal(0)) ? HK_REAL_HIGH : hkReal(hkMath::rcpF32Approx23Bit(hkFloat32(a.m_quad.v[2]))));
			self.v[3] = ((a.m_quad.v[3] == hkReal(0)) ? HK_REAL_HIGH : hkReal(hkMath::rcpF32Approx23Bit(hkFloat32(a.m_quad.v[3]))));
		}
		break;
	case HK_ACC_12_BIT: 
		{
			self.v[0] = ((a.m_quad.v[0] == hkReal(0)) ? HK_REAL_HIGH : hkReal(hkMath::rcpF32Approx12Bit(hkFloat32(a.m_quad.v[0]))));
			self.v[1] = ((a.m_quad.v[1] == hkReal(0)) ? HK_REAL_HIGH : hkReal(hkMath::rcpF32Approx12Bit(hkFloat32(a.m_quad.v[1]))));
			self.v[2] = ((a.m_quad.v[2] == hkReal(0)) ? HK_REAL_HIGH : hkReal(hkMath::rcpF32Approx12Bit(hkFloat32(a.m_quad.v[2]))));
			self.v[3] = ((a.m_quad.v[3] == hkReal(0)) ? HK_REAL_HIGH : hkReal(hkMath::rcpF32Approx12Bit(hkFloat32(a.m_quad.v[3]))));
		}
		break;
	default:         
		{
			self.v[0] = ((a.m_quad.v[0] == hkReal(0)) ? HK_REAL_HIGH : hkReal(1) / a.m_quad.v[0]);
			self.v[1] = ((a.m_quad.v[1] == hkReal(0)) ? HK_REAL_HIGH : hkReal(1) / a.m_quad.v[1]);
			self.v[2] = ((a.m_quad.v[2] == hkReal(0)) ? HK_REAL_HIGH : hkReal(1) / a.m_quad.v[2]);
			self.v[3] = ((a.m_quad.v[3] == hkReal(0)) ? HK_REAL_HIGH : hkReal(1) / a.m_quad.v[3]);
		}
		break; // HK_ACC_FULL
	}
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_SET_MAX> { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a)
{
	switch (A)
	{
	case HK_ACC_23_BIT: 
		{
			self.v[0] = ((a.m_quad.v[0] == hkReal(0)) ? HK_REAL_MAX : hkReal(hkMath::rcpF32Approx23Bit(hkFloat32(a.m_quad.v[0]))));
			self.v[1] = ((a.m_quad.v[1] == hkReal(0)) ? HK_REAL_MAX : hkReal(hkMath::rcpF32Approx23Bit(hkFloat32(a.m_quad.v[1]))));
			self.v[2] = ((a.m_quad.v[2] == hkReal(0)) ? HK_REAL_MAX : hkReal(hkMath::rcpF32Approx23Bit(hkFloat32(a.m_quad.v[2]))));
			self.v[3] = ((a.m_quad.v[3] == hkReal(0)) ? HK_REAL_MAX : hkReal(hkMath::rcpF32Approx23Bit(hkFloat32(a.m_quad.v[3]))));
		}
		break;
	case HK_ACC_12_BIT: 
		{
			self.v[0] = ((a.m_quad.v[0] == hkReal(0)) ? HK_REAL_MAX : hkReal(hkMath::rcpF32Approx12Bit(hkFloat32(a.m_quad.v[0]))));
			self.v[1] = ((a.m_quad.v[1] == hkReal(0)) ? HK_REAL_MAX : hkReal(hkMath::rcpF32Approx12Bit(hkFloat32(a.m_quad.v[1]))));
			self.v[2] = ((a.m_quad.v[2] == hkReal(0)) ? HK_REAL_MAX : hkReal(hkMath::rcpF32Approx12Bit(hkFloat32(a.m_quad.v[2]))));
			self.v[3] = ((a.m_quad.v[3] == hkReal(0)) ? HK_REAL_MAX : hkReal(hkMath::rcpF32Approx12Bit(hkFloat32(a.m_quad.v[3]))));
		}
		break;
	default:         
		{
			self.v[0] = ((a.m_quad.v[0] == hkReal(0)) ? HK_REAL_MAX : hkReal(1) / a.m_quad.v[0]);
			self.v[1] = ((a.m_quad.v[1] == hkReal(0)) ? HK_REAL_MAX : hkReal(1) / a.m_quad.v[1]);
			self.v[2] = ((a.m_quad.v[2] == hkReal(0)) ? HK_REAL_MAX : hkReal(1) / a.m_quad.v[2]);
			self.v[3] = ((a.m_quad.v[3] == hkReal(0)) ? HK_REAL_MAX : hkReal(1) / a.m_quad.v[3]);
		}
		break; // HK_ACC_FULL
	}
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_SET_ZERO_AND_ONE> { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a)
{
	hkQuadReal val; unroll_setReciprocal<A, HK_DIV_SET_ZERO>::apply(val,a);

	hkQuadReal absValLessOne;
	absValLessOne.v[0] = hkMath::fabs(val.v[0]) - hkReal(1);
	absValLessOne.v[1] = hkMath::fabs(val.v[1]) - hkReal(1);
	absValLessOne.v[2] = hkMath::fabs(val.v[2]) - hkReal(1);
	absValLessOne.v[3] = hkMath::fabs(val.v[3]) - hkReal(1);

	self.v[0] = ((absValLessOne.v[0] <= HK_REAL_EPSILON) ? hkReal(1) : val.v[0]);
	self.v[1] = ((absValLessOne.v[1] <= HK_REAL_EPSILON) ? hkReal(1) : val.v[1]);
	self.v[2] = ((absValLessOne.v[2] <= HK_REAL_EPSILON) ? hkReal(1) : val.v[2]);
	self.v[3] = ((absValLessOne.v[3] <= HK_REAL_EPSILON) ? hkReal(1) : val.v[3]);
} };

} // namespace 

template <hkMathAccuracyMode A, hkMathDivByZeroMode D>
HK_FORCE_INLINE void hkVector4::setReciprocal(hkVector4Parameter v)
{
	hkVector4_AdvancedInterface::unroll_setReciprocal<A,D>::apply(m_quad,v);
}

HK_FORCE_INLINE void hkVector4::setReciprocal(hkVector4Parameter v)
{
	hkVector4_AdvancedInterface::unroll_setReciprocal<HK_ACC_23_BIT,HK_DIV_IGNORE>::apply(m_quad,v);
}


namespace hkVector4_AdvancedInterface
{

template <hkMathAccuracyMode A, hkMathDivByZeroMode D>
struct unroll_setDiv { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a, hkVector4Parameter b)
{
	HK_VECTOR4_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <hkMathAccuracyMode A>
struct unroll_setDiv<A, HK_DIV_IGNORE> { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a, hkVector4Parameter b)
{
	switch (A)
	{
	case HK_ACC_23_BIT: 
		{
			self.v[0] = a.m_quad.v[0] * hkReal(hkMath::rcpF32Approx23Bit(hkFloat32(b.m_quad.v[0])));
			self.v[1] = a.m_quad.v[1] * hkReal(hkMath::rcpF32Approx23Bit(hkFloat32(b.m_quad.v[1])));
			self.v[2] = a.m_quad.v[2] * hkReal(hkMath::rcpF32Approx23Bit(hkFloat32(b.m_quad.v[2])));
			self.v[3] = a.m_quad.v[3] * hkReal(hkMath::rcpF32Approx23Bit(hkFloat32(b.m_quad.v[3])));
		}
		break;
	case HK_ACC_12_BIT: 
		{
			self.v[0] = a.m_quad.v[0] * hkReal(hkMath::rcpF32Approx12Bit(hkFloat32(b.m_quad.v[0])));
			self.v[1] = a.m_quad.v[1] * hkReal(hkMath::rcpF32Approx12Bit(hkFloat32(b.m_quad.v[1])));
			self.v[2] = a.m_quad.v[2] * hkReal(hkMath::rcpF32Approx12Bit(hkFloat32(b.m_quad.v[2])));
			self.v[3] = a.m_quad.v[3] * hkReal(hkMath::rcpF32Approx12Bit(hkFloat32(b.m_quad.v[3])));
		}
		break;
	default:         
		{
			self.v[0] = a.m_quad.v[0] / b.m_quad.v[0];
			self.v[1] = a.m_quad.v[1] / b.m_quad.v[1];
			self.v[2] = a.m_quad.v[2] / b.m_quad.v[2];
			self.v[3] = a.m_quad.v[3] / b.m_quad.v[3];
		}
		break; // HK_ACC_FULL
	}
} };
template <hkMathAccuracyMode A>
struct unroll_setDiv<A, HK_DIV_SET_ZERO> { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a, hkVector4Parameter b)
{
	switch (A)
	{
	case HK_ACC_23_BIT: 
		{
			self.v[0] = ((b.m_quad.v[0] == hkReal(0)) ? hkReal(0) : (a.m_quad.v[0] * hkReal(hkMath::rcpF32Approx23Bit(hkFloat32(b.m_quad.v[0])))));
			self.v[1] = ((b.m_quad.v[1] == hkReal(0)) ? hkReal(0) : (a.m_quad.v[1] * hkReal(hkMath::rcpF32Approx23Bit(hkFloat32(b.m_quad.v[1])))));
			self.v[2] = ((b.m_quad.v[2] == hkReal(0)) ? hkReal(0) : (a.m_quad.v[2] * hkReal(hkMath::rcpF32Approx23Bit(hkFloat32(b.m_quad.v[2])))));
			self.v[3] = ((b.m_quad.v[3] == hkReal(0)) ? hkReal(0) : (a.m_quad.v[3] * hkReal(hkMath::rcpF32Approx23Bit(hkFloat32(b.m_quad.v[3])))));
		}
		break;
	case HK_ACC_12_BIT: 
		{
			self.v[0] = ((b.m_quad.v[0] == hkReal(0)) ? hkReal(0) : (a.m_quad.v[0] * hkReal(hkMath::rcpF32Approx12Bit(hkFloat32(b.m_quad.v[0])))));
			self.v[1] = ((b.m_quad.v[1] == hkReal(0)) ? hkReal(0) : (a.m_quad.v[1] * hkReal(hkMath::rcpF32Approx12Bit(hkFloat32(b.m_quad.v[1])))));
			self.v[2] = ((b.m_quad.v[2] == hkReal(0)) ? hkReal(0) : (a.m_quad.v[2] * hkReal(hkMath::rcpF32Approx12Bit(hkFloat32(b.m_quad.v[2])))));
			self.v[3] = ((b.m_quad.v[3] == hkReal(0)) ? hkReal(0) : (a.m_quad.v[3] * hkReal(hkMath::rcpF32Approx12Bit(hkFloat32(b.m_quad.v[3])))));
		}
		break;
	default:         
		{
			self.v[0] = ((b.m_quad.v[0] == hkReal(0)) ? hkReal(0) : (a.m_quad.v[0] / b.m_quad.v[0]));
			self.v[1] = ((b.m_quad.v[1] == hkReal(0)) ? hkReal(0) : (a.m_quad.v[1] / b.m_quad.v[1]));
			self.v[2] = ((b.m_quad.v[2] == hkReal(0)) ? hkReal(0) : (a.m_quad.v[2] / b.m_quad.v[2]));
			self.v[3] = ((b.m_quad.v[3] == hkReal(0)) ? hkReal(0) : (a.m_quad.v[3] / b.m_quad.v[3]));
		}
		break; // HK_ACC_FULL
	}
} };
template <hkMathAccuracyMode A>
struct unroll_setDiv<A, HK_DIV_SET_HIGH> { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a, hkVector4Parameter b)
{
	switch (A)
	{
	case HK_ACC_23_BIT: 
		{
			self.v[0] = ((b.m_quad.v[0] == hkReal(0)) ? HK_REAL_HIGH : (a.m_quad.v[0] * hkReal(hkMath::rcpF32Approx23Bit(hkFloat32(b.m_quad.v[0])))));
			self.v[1] = ((b.m_quad.v[1] == hkReal(0)) ? HK_REAL_HIGH : (a.m_quad.v[1] * hkReal(hkMath::rcpF32Approx23Bit(hkFloat32(b.m_quad.v[1])))));
			self.v[2] = ((b.m_quad.v[2] == hkReal(0)) ? HK_REAL_HIGH : (a.m_quad.v[2] * hkReal(hkMath::rcpF32Approx23Bit(hkFloat32(b.m_quad.v[2])))));
			self.v[3] = ((b.m_quad.v[3] == hkReal(0)) ? HK_REAL_HIGH : (a.m_quad.v[3] * hkReal(hkMath::rcpF32Approx23Bit(hkFloat32(b.m_quad.v[3])))));
		}
		break;
	case HK_ACC_12_BIT: 
		{
			self.v[0] = ((b.m_quad.v[0] == hkReal(0)) ? HK_REAL_HIGH : (a.m_quad.v[0] * hkReal(hkMath::rcpF32Approx12Bit(hkFloat32(b.m_quad.v[0])))));
			self.v[1] = ((b.m_quad.v[1] == hkReal(0)) ? HK_REAL_HIGH : (a.m_quad.v[1] * hkReal(hkMath::rcpF32Approx12Bit(hkFloat32(b.m_quad.v[1])))));
			self.v[2] = ((b.m_quad.v[2] == hkReal(0)) ? HK_REAL_HIGH : (a.m_quad.v[2] * hkReal(hkMath::rcpF32Approx12Bit(hkFloat32(b.m_quad.v[2])))));
			self.v[3] = ((b.m_quad.v[3] == hkReal(0)) ? HK_REAL_HIGH : (a.m_quad.v[3] * hkReal(hkMath::rcpF32Approx12Bit(hkFloat32(b.m_quad.v[3])))));
		}
		break;
	default:         
		{
			self.v[0] = ((b.m_quad.v[0] == hkReal(0)) ? HK_REAL_HIGH : (a.m_quad.v[0] / b.m_quad.v[0]));
			self.v[1] = ((b.m_quad.v[1] == hkReal(0)) ? HK_REAL_HIGH : (a.m_quad.v[1] / b.m_quad.v[1]));
			self.v[2] = ((b.m_quad.v[2] == hkReal(0)) ? HK_REAL_HIGH : (a.m_quad.v[2] / b.m_quad.v[2]));
			self.v[3] = ((b.m_quad.v[3] == hkReal(0)) ? HK_REAL_HIGH : (a.m_quad.v[3] / b.m_quad.v[3]));
		}
		break; // HK_ACC_FULL
	}
} };
template <hkMathAccuracyMode A>
struct unroll_setDiv<A, HK_DIV_SET_MAX> { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a, hkVector4Parameter b)
{
	switch (A)
	{
	case HK_ACC_23_BIT: 
		{
			self.v[0] = ((b.m_quad.v[0] == hkReal(0)) ? HK_REAL_MAX : (a.m_quad.v[0] * hkReal(hkMath::rcpF32Approx23Bit(hkFloat32(b.m_quad.v[0])))));
			self.v[1] = ((b.m_quad.v[1] == hkReal(0)) ? HK_REAL_MAX : (a.m_quad.v[1] * hkReal(hkMath::rcpF32Approx23Bit(hkFloat32(b.m_quad.v[1])))));
			self.v[2] = ((b.m_quad.v[2] == hkReal(0)) ? HK_REAL_MAX : (a.m_quad.v[2] * hkReal(hkMath::rcpF32Approx23Bit(hkFloat32(b.m_quad.v[2])))));
			self.v[3] = ((b.m_quad.v[3] == hkReal(0)) ? HK_REAL_MAX : (a.m_quad.v[3] * hkReal(hkMath::rcpF32Approx23Bit(hkFloat32(b.m_quad.v[3])))));
		}
		break;
	case HK_ACC_12_BIT: 
		{
			self.v[0] = ((b.m_quad.v[0] == hkReal(0)) ? HK_REAL_MAX : (a.m_quad.v[0] * hkReal(hkMath::rcpF32Approx12Bit(hkFloat32(b.m_quad.v[0])))));
			self.v[1] = ((b.m_quad.v[1] == hkReal(0)) ? HK_REAL_MAX : (a.m_quad.v[1] * hkReal(hkMath::rcpF32Approx12Bit(hkFloat32(b.m_quad.v[1])))));
			self.v[2] = ((b.m_quad.v[2] == hkReal(0)) ? HK_REAL_MAX : (a.m_quad.v[2] * hkReal(hkMath::rcpF32Approx12Bit(hkFloat32(b.m_quad.v[2])))));
			self.v[3] = ((b.m_quad.v[3] == hkReal(0)) ? HK_REAL_MAX : (a.m_quad.v[3] * hkReal(hkMath::rcpF32Approx12Bit(hkFloat32(b.m_quad.v[3])))));
		}
		break;
	default:         
		{
			self.v[0] = ((b.m_quad.v[0] == hkReal(0)) ? HK_REAL_MAX : (a.m_quad.v[0] / b.m_quad.v[0]));
			self.v[1] = ((b.m_quad.v[1] == hkReal(0)) ? HK_REAL_MAX : (a.m_quad.v[1] / b.m_quad.v[1]));
			self.v[2] = ((b.m_quad.v[2] == hkReal(0)) ? HK_REAL_MAX : (a.m_quad.v[2] / b.m_quad.v[2]));
			self.v[3] = ((b.m_quad.v[3] == hkReal(0)) ? HK_REAL_MAX : (a.m_quad.v[3] / b.m_quad.v[3]));
		}
		break; // HK_ACC_FULL
	}
} };

} // namespace 

template <hkMathAccuracyMode A, hkMathDivByZeroMode D>
HK_FORCE_INLINE void hkVector4::setDiv(hkVector4Parameter v0, hkVector4Parameter v1)
{
	hkVector4_AdvancedInterface::unroll_setDiv<A,D>::apply(m_quad,v0,v1);
}

HK_FORCE_INLINE void hkVector4::setDiv(hkVector4Parameter v0, hkVector4Parameter v1)
{
	hkVector4_AdvancedInterface::unroll_setDiv<HK_ACC_23_BIT,HK_DIV_IGNORE>::apply(m_quad,v0,v1);
}

template <hkMathAccuracyMode A, hkMathDivByZeroMode D>
HK_FORCE_INLINE void hkVector4::div(hkVector4Parameter a)
{
	setDiv<A,D>( *this, a );
}

HK_FORCE_INLINE void hkVector4::div(hkVector4Parameter a)
{
	setDiv( *this, a );
}



namespace hkVector4_AdvancedInterface
{

template <hkMathAccuracyMode A, hkMathNegSqrtMode S>
struct unroll_setSqrt { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a)
{
	HK_VECTOR4_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <hkMathAccuracyMode A>
struct unroll_setSqrt<A, HK_SQRT_IGNORE> { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a)
{
	switch (A)
	{
		case HK_ACC_23_BIT: 
			{
				self.v[0] = hkReal(hkMath::rcpF32Approx23Bit(hkMath::invSqrtF32Approx23Bit(hkFloat32(a.m_quad.v[0]))));
				self.v[1] = hkReal(hkMath::rcpF32Approx23Bit(hkMath::invSqrtF32Approx23Bit(hkFloat32(a.m_quad.v[1]))));
				self.v[2] = hkReal(hkMath::rcpF32Approx23Bit(hkMath::invSqrtF32Approx23Bit(hkFloat32(a.m_quad.v[2]))));
				self.v[3] = hkReal(hkMath::rcpF32Approx23Bit(hkMath::invSqrtF32Approx23Bit(hkFloat32(a.m_quad.v[3]))));
			}
			break;
		case HK_ACC_12_BIT: 
			{
				self.v[0] = hkReal(hkMath::rcpF32Approx12Bit(hkMath::invSqrtF32Approx12Bit(hkFloat32(a.m_quad.v[0]))));
				self.v[1] = hkReal(hkMath::rcpF32Approx12Bit(hkMath::invSqrtF32Approx12Bit(hkFloat32(a.m_quad.v[1]))));
				self.v[2] = hkReal(hkMath::rcpF32Approx12Bit(hkMath::invSqrtF32Approx12Bit(hkFloat32(a.m_quad.v[2]))));
				self.v[3] = hkReal(hkMath::rcpF32Approx12Bit(hkMath::invSqrtF32Approx12Bit(hkFloat32(a.m_quad.v[3]))));
			}
			break;
		default:         
			{
				self.v[0] = hkMath::sqrt(a.m_quad.v[0]);
				self.v[1] = hkMath::sqrt(a.m_quad.v[1]);
				self.v[2] = hkMath::sqrt(a.m_quad.v[2]);
				self.v[3] = hkMath::sqrt(a.m_quad.v[3]);
			}
			break; // HK_ACC_FULL
	}
} };
template <hkMathAccuracyMode A>
struct unroll_setSqrt<A, HK_SQRT_SET_ZERO> { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a)
{
	switch (A)
	{
	case HK_ACC_23_BIT: 
		{
			self.v[0] = ((a.m_quad.v[0] <= hkReal(0)) ? hkReal(0) : hkReal(hkMath::rcpF32Approx23Bit(hkMath::invSqrtF32Approx23Bit(hkFloat32(a.m_quad.v[0])))));
			self.v[1] = ((a.m_quad.v[1] <= hkReal(0)) ? hkReal(0) : hkReal(hkMath::rcpF32Approx23Bit(hkMath::invSqrtF32Approx23Bit(hkFloat32(a.m_quad.v[1])))));
			self.v[2] = ((a.m_quad.v[2] <= hkReal(0)) ? hkReal(0) : hkReal(hkMath::rcpF32Approx23Bit(hkMath::invSqrtF32Approx23Bit(hkFloat32(a.m_quad.v[2])))));
			self.v[3] = ((a.m_quad.v[3] <= hkReal(0)) ? hkReal(0) : hkReal(hkMath::rcpF32Approx23Bit(hkMath::invSqrtF32Approx23Bit(hkFloat32(a.m_quad.v[3])))));
		}
		break;
	case HK_ACC_12_BIT: 
		{
			self.v[0] = ((a.m_quad.v[0] <= hkReal(0)) ? hkReal(0) : hkReal(hkMath::rcpF32Approx12Bit(hkMath::invSqrtF32Approx12Bit(hkFloat32(a.m_quad.v[0])))));
			self.v[1] = ((a.m_quad.v[1] <= hkReal(0)) ? hkReal(0) : hkReal(hkMath::rcpF32Approx12Bit(hkMath::invSqrtF32Approx12Bit(hkFloat32(a.m_quad.v[1])))));
			self.v[2] = ((a.m_quad.v[2] <= hkReal(0)) ? hkReal(0) : hkReal(hkMath::rcpF32Approx12Bit(hkMath::invSqrtF32Approx12Bit(hkFloat32(a.m_quad.v[2])))));
			self.v[3] = ((a.m_quad.v[3] <= hkReal(0)) ? hkReal(0) : hkReal(hkMath::rcpF32Approx12Bit(hkMath::invSqrtF32Approx12Bit(hkFloat32(a.m_quad.v[3])))));
		}
		break;
	default:         
		{
			self.v[0] = ((a.m_quad.v[0] <= hkReal(0)) ? hkReal(0) : hkMath::sqrt(a.m_quad.v[0]));
			self.v[1] = ((a.m_quad.v[1] <= hkReal(0)) ? hkReal(0) : hkMath::sqrt(a.m_quad.v[1]));
			self.v[2] = ((a.m_quad.v[2] <= hkReal(0)) ? hkReal(0) : hkMath::sqrt(a.m_quad.v[2]));
			self.v[3] = ((a.m_quad.v[3] <= hkReal(0)) ? hkReal(0) : hkMath::sqrt(a.m_quad.v[3]));
		}
		break; // HK_ACC_FULL
	}
} };

} // namespace 

template <hkMathAccuracyMode A, hkMathNegSqrtMode S>
HK_FORCE_INLINE void hkVector4::setSqrt(hkVector4Parameter a)
{
	hkVector4_AdvancedInterface::unroll_setSqrt<A,S>::apply(m_quad, a);
}

HK_FORCE_INLINE void hkVector4::setSqrt(hkVector4Parameter a)
{
	hkVector4_AdvancedInterface::unroll_setSqrt<HK_ACC_23_BIT,HK_SQRT_SET_ZERO>::apply(m_quad, a);
}




namespace hkVector4_AdvancedInterface
{

template <hkMathAccuracyMode A, hkMathNegSqrtMode S>
struct unroll_setSqrtInverse { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a)
{
	HK_VECTOR4_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <hkMathAccuracyMode A>
struct unroll_setSqrtInverse<A, HK_SQRT_IGNORE> { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a)
{
	switch (A)
	{
	case HK_ACC_23_BIT: 
		{
			self.v[0] = hkReal(hkMath::invSqrtF32Approx23Bit(hkFloat32(a.m_quad.v[0])));
			self.v[1] = hkReal(hkMath::invSqrtF32Approx23Bit(hkFloat32(a.m_quad.v[1])));
			self.v[2] = hkReal(hkMath::invSqrtF32Approx23Bit(hkFloat32(a.m_quad.v[2])));
			self.v[3] = hkReal(hkMath::invSqrtF32Approx23Bit(hkFloat32(a.m_quad.v[3])));
		}
		break;
	case HK_ACC_12_BIT: 
		{
			self.v[0] = hkReal(hkMath::invSqrtF32Approx12Bit(hkFloat32(a.m_quad.v[0])));
			self.v[1] = hkReal(hkMath::invSqrtF32Approx12Bit(hkFloat32(a.m_quad.v[1])));
			self.v[2] = hkReal(hkMath::invSqrtF32Approx12Bit(hkFloat32(a.m_quad.v[2])));
			self.v[3] = hkReal(hkMath::invSqrtF32Approx12Bit(hkFloat32(a.m_quad.v[3])));
		}
		break;
	default:         
		{
			self.v[0] = hkMath::sqrtInverse(a.m_quad.v[0]);
			self.v[1] = hkMath::sqrtInverse(a.m_quad.v[1]);
			self.v[2] = hkMath::sqrtInverse(a.m_quad.v[2]);
			self.v[3] = hkMath::sqrtInverse(a.m_quad.v[3]);
		}
		break; // HK_ACC_FULL
	}
} };
template <hkMathAccuracyMode A>
struct unroll_setSqrtInverse<A, HK_SQRT_SET_ZERO> { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a)
{
	switch (A)
	{
	case HK_ACC_23_BIT: 
		{
			self.v[0] = ((a.m_quad.v[0] <= hkReal(0)) ? hkReal(0) : hkReal(hkMath::invSqrtF32Approx23Bit(hkFloat32(a.m_quad.v[0]))));
			self.v[1] = ((a.m_quad.v[1] <= hkReal(0)) ? hkReal(0) : hkReal(hkMath::invSqrtF32Approx23Bit(hkFloat32(a.m_quad.v[1]))));
			self.v[2] = ((a.m_quad.v[2] <= hkReal(0)) ? hkReal(0) : hkReal(hkMath::invSqrtF32Approx23Bit(hkFloat32(a.m_quad.v[2]))));
			self.v[3] = ((a.m_quad.v[3] <= hkReal(0)) ? hkReal(0) : hkReal(hkMath::invSqrtF32Approx23Bit(hkFloat32(a.m_quad.v[3]))));
		}
		break;
	case HK_ACC_12_BIT: 
		{
			self.v[0] = ((a.m_quad.v[0] <= hkReal(0)) ? hkReal(0) : hkReal(hkMath::invSqrtF32Approx12Bit(hkFloat32(a.m_quad.v[0]))));
			self.v[1] = ((a.m_quad.v[1] <= hkReal(0)) ? hkReal(0) : hkReal(hkMath::invSqrtF32Approx12Bit(hkFloat32(a.m_quad.v[1]))));
			self.v[2] = ((a.m_quad.v[2] <= hkReal(0)) ? hkReal(0) : hkReal(hkMath::invSqrtF32Approx12Bit(hkFloat32(a.m_quad.v[2]))));
			self.v[3] = ((a.m_quad.v[3] <= hkReal(0)) ? hkReal(0) : hkReal(hkMath::invSqrtF32Approx12Bit(hkFloat32(a.m_quad.v[3]))));
		}
		break;
	default:         
		{
			self.v[0] = ((a.m_quad.v[0] <= hkReal(0)) ? hkReal(0) : hkMath::sqrtInverse(a.m_quad.v[0]));
			self.v[1] = ((a.m_quad.v[1] <= hkReal(0)) ? hkReal(0) : hkMath::sqrtInverse(a.m_quad.v[1]));
			self.v[2] = ((a.m_quad.v[2] <= hkReal(0)) ? hkReal(0) : hkMath::sqrtInverse(a.m_quad.v[2]));
			self.v[3] = ((a.m_quad.v[3] <= hkReal(0)) ? hkReal(0) : hkMath::sqrtInverse(a.m_quad.v[3]));
		}
		break; // HK_ACC_FULL
	}
} };
} // namespace 

template <hkMathAccuracyMode A, hkMathNegSqrtMode S>
HK_FORCE_INLINE void hkVector4::setSqrtInverse(hkVector4Parameter a)
{
	hkVector4_AdvancedInterface::unroll_setSqrtInverse<A,S>::apply(m_quad, a);
}

HK_FORCE_INLINE void hkVector4::setSqrtInverse(hkVector4Parameter a)
{
	hkVector4_AdvancedInterface::unroll_setSqrtInverse<HK_ACC_23_BIT,HK_SQRT_SET_ZERO>::apply(m_quad, a);
}


namespace hkVector4_AdvancedInterface
{
template <int N, hkMathIoMode A>
struct unroll_load { HK_FORCE_INLINE static void apply(hkQuadReal& self, const hkReal* HK_RESTRICT p)
{
	HK_VECTOR4_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <int N>
struct unroll_load<N, HK_IO_BYTE_ALIGNED> { HK_FORCE_INLINE static void apply(hkQuadReal& self, const hkReal* HK_RESTRICT p)
{
	hkMemUtil::memCpy<1>(&self, p, N*sizeof(hkReal));
#if defined(HK_DEBUG)
#if defined(HK_REAL_IS_DOUBLE)
	for(int i=N; i<4; ++i) *((hkUint64*)&(self.v[i])) = 0xffffffffffffffffull;
#else
	for(int i=N; i<4; ++i) *((hkUint32*)&(self.v[i])) = 0xffffffff;
#endif
#endif
} };
template <int N>
struct unroll_load<N, HK_IO_NATIVE_ALIGNED> { HK_FORCE_INLINE static void apply(hkQuadReal& self, const hkReal* HK_RESTRICT p)
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkReal)-1) ) == 0, "pointer must be aligned to native size of hkReal.");
	hkMemUtil::memCpy<sizeof(hkReal)>(&self, p, N*sizeof(hkReal));
#if defined(HK_DEBUG)
#if defined(HK_REAL_IS_DOUBLE)
	for(int i=N; i<4; ++i) *((hkUint64*)&(self.v[i])) = 0xffffffffffffffffull;
#else
	for(int i=N; i<4; ++i) *((hkUint32*)&(self.v[i])) = 0xffffffff;
#endif
#endif
} };
template <int N>
struct unroll_load<N, HK_IO_SIMD_ALIGNED> { HK_FORCE_INLINE static void apply(hkQuadReal& self, const hkReal* HK_RESTRICT p)
{
#if !defined(HK_PLATFORM_IOS)
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & ((sizeof(hkReal)*(N!=3?N:4) )-1) ) == 0, "pointer must be aligned for SIMD.");
#endif
	switch (N)
	{
	case 4: 
		{
			hkMemUtil::memCpyOneAligned<4*sizeof(hkReal), 16>(&self, p);
		}
		break;
	default:
		{
			unroll_load<N, HK_IO_NATIVE_ALIGNED>::apply(self,p);
		}
		break;
	}
} };
template <int N>
struct unroll_load<N, HK_IO_NOT_CACHED> { HK_FORCE_INLINE static void apply(hkQuadReal& self, const hkReal* HK_RESTRICT p)
{
	unroll_load<N, HK_IO_SIMD_ALIGNED>::apply(self,p);
} };
} // namespace 

template <int N, hkMathIoMode A>
HK_FORCE_INLINE void hkVector4::load(const hkReal* p)
{
	HK_VECTOR4_UNSUPPORTED_LENGTH_CHECK;
	hkVector4_AdvancedInterface::unroll_load<N,A>::apply(m_quad, p);
}

template <int N>
HK_FORCE_INLINE void hkVector4::load(const hkReal* p)
{
	HK_VECTOR4_UNSUPPORTED_LENGTH_CHECK;
	hkVector4_AdvancedInterface::unroll_load<N,HK_IO_SIMD_ALIGNED>::apply(m_quad, p);
}



namespace hkVector4_AdvancedInterface
{
template <int N, hkMathIoMode A>
struct unroll_loadH { HK_FORCE_INLINE static void apply(hkQuadReal& self, const hkHalf* HK_RESTRICT p)
{
	HK_VECTOR4_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <int N>
struct unroll_loadH<N, HK_IO_BYTE_ALIGNED> { HK_FORCE_INLINE static void apply(hkQuadReal& self, const hkHalf* HK_RESTRICT p)
{
	switch (N)
	{
		case 4:  self.v[3] = hkReal(hkFloat32(p[3]));
		case 3:  self.v[2] = hkReal(hkFloat32(p[2]));
		case 2:  self.v[1] = hkReal(hkFloat32(p[1]));
		default: self.v[0] = hkReal(hkFloat32(p[0])); break;
	}
#if defined(HK_DEBUG)
#if defined(HK_REAL_IS_DOUBLE)
	for(int i=N; i<4; ++i) *((hkUint64*)&(self.v[i])) = 0xffffffffffffffffull;
#else
	for(int i=N; i<4; ++i) *((hkUint32*)&(self.v[i])) = 0xffffffff;
#endif
#endif
} };
template <int N>
struct unroll_loadH<N, HK_IO_NATIVE_ALIGNED> { HK_FORCE_INLINE static void apply(hkQuadReal& self, const hkHalf* HK_RESTRICT p)
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkHalf)-1) ) == 0, "pointer must be aligned to native size of hkHalf.");
	unroll_loadH<N, HK_IO_BYTE_ALIGNED>::apply(self,p);
} };
template <int N>
struct unroll_loadH<N, HK_IO_SIMD_ALIGNED> { HK_FORCE_INLINE static void apply(hkQuadReal& self, const hkHalf* HK_RESTRICT p)
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & ((sizeof(hkReal)*(N!=3?N:4) )-1) ) == 0, "pointer must be aligned for SIMD.");
	unroll_loadH<N, HK_IO_NATIVE_ALIGNED>::apply(self,p);
} };
template <int N>
struct unroll_loadH<N, HK_IO_NOT_CACHED> { HK_FORCE_INLINE static void apply(hkQuadReal& self, const hkHalf* HK_RESTRICT p)
{
	unroll_loadH<N, HK_IO_SIMD_ALIGNED>::apply(self,p);
} };
} // namespace 

template <int N, hkMathIoMode A>
HK_FORCE_INLINE void hkVector4::load(const hkHalf* p)
{
	HK_VECTOR4_UNSUPPORTED_LENGTH_CHECK;
	hkVector4_AdvancedInterface::unroll_loadH<N,A>::apply(m_quad, p);
}

template <int N>
HK_FORCE_INLINE void hkVector4::load(const hkHalf* p)
{
	HK_VECTOR4_UNSUPPORTED_LENGTH_CHECK;
	hkVector4_AdvancedInterface::unroll_loadH<N,HK_IO_SIMD_ALIGNED>::apply(m_quad, p);
}



namespace hkVector4_AdvancedInterface
{
template <int N, hkMathIoMode A>
struct unroll_loadF16 { HK_FORCE_INLINE static void apply(hkQuadReal& self, const hkFloat16* HK_RESTRICT p)
{
	HK_VECTOR4_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <int N>
struct unroll_loadF16<N, HK_IO_BYTE_ALIGNED> { HK_FORCE_INLINE static void apply(hkQuadReal& self, const hkFloat16* HK_RESTRICT p)
{
	switch (N)
	{
		case 4:  self.v[3] = p[3].getReal();
		case 3:  self.v[2] = p[2].getReal();
		case 2:  self.v[1] = p[1].getReal();
		default: self.v[0] = p[0].getReal(); break;
	}
#if defined(HK_DEBUG)
#if defined(HK_REAL_IS_DOUBLE)
	for(int i=N; i<4; ++i) *((hkUint64*)&(self.v[i])) = 0xffffffffffffffffull;
#else
	for(int i=N; i<4; ++i) *((hkUint32*)&(self.v[i])) = 0xffffffff;
#endif
#endif
} };
template <int N>
struct unroll_loadF16<N, HK_IO_NATIVE_ALIGNED> { HK_FORCE_INLINE static void apply(hkQuadReal& self, const hkFloat16* HK_RESTRICT p)
{
#if !defined(HK_PLATFORM_IOS)
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkFloat16)-1) ) == 0, "pointer must be aligned to native size of hkFloat16.");
#endif
	unroll_loadF16<N, HK_IO_BYTE_ALIGNED>::apply(self,p);
} };
template <int N>
struct unroll_loadF16<N, HK_IO_SIMD_ALIGNED> { HK_FORCE_INLINE static void apply(hkQuadReal& self, const hkFloat16* HK_RESTRICT p)
{
#if !defined(HK_PLATFORM_IOS)
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & ((sizeof(hkReal)*(N!=3?N:4) )-1) ) == 0, "pointer must be aligned for SIMD.");
#endif
	unroll_loadF16<N, HK_IO_NATIVE_ALIGNED>::apply(self,p);
} };
template <int N>
struct unroll_loadF16<N, HK_IO_NOT_CACHED> { HK_FORCE_INLINE static void apply(hkQuadReal& self, const hkFloat16* HK_RESTRICT p)
{
	unroll_loadF16<N, HK_IO_SIMD_ALIGNED>::apply(self,p);
} };
} // namespace 

template <int N, hkMathIoMode A>
HK_FORCE_INLINE void hkVector4::load(const hkFloat16* p)
{
	HK_VECTOR4_UNSUPPORTED_LENGTH_CHECK;
	hkVector4_AdvancedInterface::unroll_loadF16<N,A>::apply(m_quad, p);
}

template <int N>
HK_FORCE_INLINE void hkVector4::load(const hkFloat16* p)
{
	HK_VECTOR4_UNSUPPORTED_LENGTH_CHECK;
	hkVector4_AdvancedInterface::unroll_loadF16<N,HK_IO_SIMD_ALIGNED>::apply(m_quad, p);
}



namespace hkVector4_AdvancedInterface
{
template <int N, hkMathIoMode A>
struct unroll_store { HK_FORCE_INLINE static void apply(const hkQuadReal& self, hkReal* HK_RESTRICT p)
{
	HK_VECTOR4_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <int N>
struct unroll_store<N, HK_IO_BYTE_ALIGNED> { HK_FORCE_INLINE static void apply(const hkQuadReal& self, hkReal* HK_RESTRICT p)
{
	hkMemUtil::memCpy<1>(p, &self, N*sizeof(hkReal));
} };
template <int N>
struct unroll_store<N, HK_IO_NATIVE_ALIGNED> { HK_FORCE_INLINE static void apply(const hkQuadReal& self, hkReal* HK_RESTRICT p)
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkReal)-1) ) == 0, "pointer must be aligned to native size of hkReal.");
	hkMemUtil::memCpy<sizeof(hkReal)>(p, &self, N*sizeof(hkReal));
} };
template <int N>
struct unroll_store<N, HK_IO_SIMD_ALIGNED> { HK_FORCE_INLINE static void apply(const hkQuadReal& self, hkReal* HK_RESTRICT p)
{
#if !defined(HK_PLATFORM_IOS)
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & ((sizeof(hkReal)*(N!=3?N:4) )-1) ) == 0, "pointer must be aligned for SIMD.");
#endif
	switch (N)
	{
	case 4: 
		{
			hkMemUtil::memCpyOneAligned<4*sizeof(hkReal), 16>(p, &self);
		}
		break;
	default:
		{
			unroll_store<N, HK_IO_NATIVE_ALIGNED>::apply(self,p);
		}
		break;
	}
} };
template <int N>
struct unroll_store<N, HK_IO_NOT_CACHED> { HK_FORCE_INLINE static void apply(const hkQuadReal& self, hkReal* HK_RESTRICT p)
{
#if !defined(HK_PLATFORM_IOS)
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & ((sizeof(hkReal)*(N!=3?N:4) )-1) ) == 0, "pointer must be aligned for SIMD.");
#endif
	unroll_store<N, HK_IO_SIMD_ALIGNED>::apply(self,p);
} };
} // namespace 

template <int N, hkMathIoMode A> 
HK_FORCE_INLINE void hkVector4::store(hkReal* p) const
{
	HK_VECTOR4_UNSUPPORTED_LENGTH_CHECK;
	hkVector4_AdvancedInterface::unroll_store<N,A>::apply(m_quad, p);
}

template <int N> 
HK_FORCE_INLINE void hkVector4::store(hkReal* p) const
{
	HK_VECTOR4_UNSUPPORTED_LENGTH_CHECK;
	hkVector4_AdvancedInterface::unroll_store<N,HK_IO_SIMD_ALIGNED>::apply(m_quad, p);
}



namespace hkVector4_AdvancedInterface
{
template <int N, hkMathIoMode A, hkMathRoundingMode R>
struct unroll_storeH { HK_FORCE_INLINE static void apply(const hkQuadReal& self, hkHalf* HK_RESTRICT p)
{
	HK_VECTOR4_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <int N, hkMathRoundingMode R>
struct unroll_storeH<N, HK_IO_BYTE_ALIGNED, R> { HK_FORCE_INLINE static void apply(const hkQuadReal& self, hkHalf* HK_RESTRICT p)
{
	if (R == HK_ROUND_NEAREST)
	{
		const hkReal packHalf = g_vectorConstants[HK_QUADREAL_PACK_HALF].v[0];
		switch (N)
		{
			case 4:  p[3] = hkFloat32(self.v[3] * packHalf);
			case 3:  p[2] = hkFloat32(self.v[2] * packHalf);
			case 2:  p[1] = hkFloat32(self.v[1] * packHalf);
			default: p[0] = hkFloat32(self.v[0] * packHalf); break;
		}
	}
	else
	{
		switch (N)
		{
			case 4:  p[3] = hkFloat32(self.v[3]);
			case 3:  p[2] = hkFloat32(self.v[2]);
			case 2:  p[1] = hkFloat32(self.v[1]);
			default: p[0] = hkFloat32(self.v[0]); break;
		}
	}
} };
template <int N, hkMathRoundingMode R>
struct unroll_storeH<N, HK_IO_NATIVE_ALIGNED, R> { HK_FORCE_INLINE static void apply(const hkQuadReal& self, hkHalf* HK_RESTRICT p)
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkHalf)-1) ) == 0, "pointer must be aligned to native size of hkHalf.");
	unroll_storeH<N, HK_IO_BYTE_ALIGNED, R>::apply(self,p);
} };
template <int N, hkMathRoundingMode R>
struct unroll_storeH<N, HK_IO_SIMD_ALIGNED, R> { HK_FORCE_INLINE static void apply(const hkQuadReal& self, hkHalf* HK_RESTRICT p)
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & ((sizeof(hkReal)*(N!=3?N:4) )-1) ) == 0, "pointer must be aligned for SIMD.");
	unroll_storeH<N, HK_IO_NATIVE_ALIGNED, R>::apply(self,p);
} };
template <int N, hkMathRoundingMode R>
struct unroll_storeH<N, HK_IO_NOT_CACHED, R> { HK_FORCE_INLINE static void apply(const hkQuadReal& self, hkHalf* HK_RESTRICT p)
{
	unroll_storeH<N, HK_IO_SIMD_ALIGNED, R>::apply(self,p);
} };
} // namespace 

template <int N, hkMathIoMode A, hkMathRoundingMode R> 
HK_FORCE_INLINE void hkVector4::store(hkHalf* p) const
{
	HK_VECTOR4_UNSUPPORTED_LENGTH_CHECK;
	hkVector4_AdvancedInterface::unroll_storeH<N,A,R>::apply(m_quad, p);
}

template <int N> 
HK_FORCE_INLINE void hkVector4::store(hkHalf* p) const
{
	HK_VECTOR4_UNSUPPORTED_LENGTH_CHECK;
	hkVector4_AdvancedInterface::unroll_storeH<N,HK_IO_SIMD_ALIGNED,HK_ROUND_NEAREST>::apply(m_quad, p);
}






namespace hkVector4_AdvancedInterface
{
template <int N, hkMathIoMode A, hkMathRoundingMode R>
struct unroll_storeF16 { HK_FORCE_INLINE static void apply(const hkQuadReal& self, hkFloat16* HK_RESTRICT p)
{
	HK_VECTOR4_TEMPLATE_CONFIG_NOT_IMPLEMENTED;
} };
template <int N, hkMathRoundingMode R>
struct unroll_storeF16<N, HK_IO_BYTE_ALIGNED, R> { HK_FORCE_INLINE static void apply(const hkQuadReal& self, hkFloat16* HK_RESTRICT p)
{
	switch (N)
	{
		case 4:  p[3].setReal<(R == HK_ROUND_NEAREST)>(self.v[3]);
		case 3:  p[2].setReal<(R == HK_ROUND_NEAREST)>(self.v[2]);
		case 2:  p[1].setReal<(R == HK_ROUND_NEAREST)>(self.v[1]);
		default: p[0].setReal<(R == HK_ROUND_NEAREST)>(self.v[0]); break;
	}
} };
template <int N, hkMathRoundingMode R>
struct unroll_storeF16<N, HK_IO_NATIVE_ALIGNED, R> { HK_FORCE_INLINE static void apply(const hkQuadReal& self, hkFloat16* HK_RESTRICT p)
{
#if !defined(HK_PLATFORM_IOS)
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkFloat16)-1) ) == 0, "pointer must be aligned to native size of hkFloat16.");
#endif
	unroll_storeF16<N, HK_IO_BYTE_ALIGNED, R>::apply(self,p);
} };
template <int N, hkMathRoundingMode R>
struct unroll_storeF16<N, HK_IO_SIMD_ALIGNED, R> { HK_FORCE_INLINE static void apply(const hkQuadReal& self, hkFloat16* HK_RESTRICT p)
{
#if !defined(HK_PLATFORM_IOS)
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & ((sizeof(hkReal)*(N!=3?N:4) )-1) ) == 0, "pointer must be aligned for SIMD.");
#endif
	unroll_storeF16<N, HK_IO_NATIVE_ALIGNED, R>::apply(self,p);
} };
template <int N, hkMathRoundingMode R>
struct unroll_storeF16<N, HK_IO_NOT_CACHED, R> { HK_FORCE_INLINE static void apply(const hkQuadReal& self, hkFloat16* HK_RESTRICT p)
{
	unroll_storeF16<N, HK_IO_SIMD_ALIGNED, R>::apply(self,p);
} };
} // namespace 

template <int N, hkMathIoMode A, hkMathRoundingMode R> 
HK_FORCE_INLINE void hkVector4::store(hkFloat16* p) const
{
	HK_VECTOR4_UNSUPPORTED_LENGTH_CHECK;
	hkVector4_AdvancedInterface::unroll_storeF16<N,A,R>::apply(m_quad, p);
}

template <int N> 
HK_FORCE_INLINE void hkVector4::store(hkFloat16* p) const
{
	HK_VECTOR4_UNSUPPORTED_LENGTH_CHECK;
	hkVector4_AdvancedInterface::unroll_storeF16<N,HK_IO_SIMD_ALIGNED,HK_ROUND_NEAREST>::apply(m_quad, p);
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
