/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

HK_FORCE_INLINE hkVector4& hkTransform::getColumn(int x)
{
	return m_rotation.getColumn(x);
}

HK_FORCE_INLINE const hkVector4& hkTransform::getColumn(int x) const
{
	return m_rotation.getColumn(x);
}

template <int I>
HK_FORCE_INLINE const hkVector4& hkTransform::getColumn() const
{
	return m_rotation.getColumn<I>();
}

template <int I>
HK_FORCE_INLINE void hkTransform::setColumn(hkVector4Parameter v)
{
	m_rotation.setColumn<I>(v);
}

HK_FORCE_INLINE void hkTransform::setTranslation(hkVector4Parameter t)
{
	m_translation = t;
}

HK_FORCE_INLINE hkRotation& hkTransform::getRotation()
{
	return m_rotation;
}

HK_FORCE_INLINE const hkRotation& hkTransform::getRotation() const
{
	return m_rotation;
}

HK_FORCE_INLINE void hkTransform::setRotation(const hkRotation& rotation)
{	
	m_rotation = rotation;
}

HK_FORCE_INLINE void hkTransform::setRotation(hkQuaternionParameter quatRotation)
{
	m_rotation.set(quatRotation);
}

#ifndef HK_DISABLE_MATH_CONSTRUCTORS
HK_FORCE_INLINE hkTransform::hkTransform(const hkTransform& t)
{
	const hkVector4 r0 = t.getColumn<0>();
	const hkVector4 r1 = t.getColumn<1>();
	const hkVector4 r2 = t.getColumn<2>();
	const hkVector4 r3 = t.getColumn<3>();
	setColumn<0>(r0);
	setColumn<1>(r1);
	setColumn<2>(r2);
	setColumn<3>(r3);
}

HK_FORCE_INLINE hkTransform::hkTransform(hkQuaternionParameter q, hkVector4Parameter t)
	:	m_translation(t)
{
	m_rotation.set(q);
}


HK_FORCE_INLINE hkTransform::hkTransform(const hkRotation& r, hkVector4Parameter t)
	:	m_rotation(r),
		m_translation(t)
{
}
#endif

HK_FORCE_INLINE hkReal& hkTransform::operator() (int r, int c)
{
	return m_rotation(r,c);
}

HK_FORCE_INLINE const hkReal& hkTransform::operator() (int r, int c) const
{
	return m_rotation(r,c);
}

template <int ROW, int COL>
HK_FORCE_INLINE const hkSimdReal hkTransform::getElement() const
{
	return m_rotation.getElement<ROW,COL>();
}

template <int ROW, int COL>
HK_FORCE_INLINE void hkTransform::setElement(hkSimdRealParameter s)
{
	m_rotation.setElement<ROW,COL>(s);
}

HK_FORCE_INLINE void hkTransform::set(hkQuaternionParameter q, hkVector4Parameter t)
{
	m_rotation.set(q);
	m_translation = t;
}

HK_FORCE_INLINE void hkTransform::set(const hkRotation& r, hkVector4Parameter t)
{
	m_rotation = r;
	m_translation = t;
}

HK_FORCE_INLINE /*static*/ const hkTransform& HK_CALL hkTransform::getIdentity()
{
	union { const hkQuadReal* r; const hkTransform* t; } r2t;
	r2t.r = g_vectorConstants + HK_QUADREAL_1000;
	return *r2t.t;
}

HK_FORCE_INLINE void hkTransform::setIdentity()
{
	m_rotation.setIdentity();
	m_translation.setZero();
}

HK_FORCE_INLINE hkVector4& hkTransform::getTranslation()
{ 
	return m_translation; 
}

HK_FORCE_INLINE const hkVector4& hkTransform::getTranslation() const 
{ 
	return m_translation; 
}

HK_FORCE_INLINE void hkTransform::_setInverse( const hkTransform &t )
{
	m_rotation._setTranspose( t.getRotation() ); 

	hkVector4 tr;	tr.setNeg<4>(t.getTranslation()); 
	m_translation._setRotatedDir( m_rotation, tr);
}


HK_FORCE_INLINE void hkTransform::_setMulInverseMul( const hkTransform& bTa, const hkTransform &bTc )
{
	int i = 3;
	do
	{
		getColumn(i)._setRotatedInverseDir( bTa.getRotation(), bTc.getColumn(i) );
	} while( --i >= 0 );
	hkVector4 h; h.setSub(bTc.m_translation, bTa.m_translation );
	m_translation._setRotatedInverseDir( bTa.m_rotation, h);
}


HK_FORCE_INLINE void hkTransform::setRows4( hkVector4Parameter r0, hkVector4Parameter r1, hkVector4Parameter r2, hkVector4Parameter r3)
{
	hkVector4 c0 = r0;
	hkVector4 c1 = r1;
	hkVector4 c2 = r2;
	hkVector4 c3 = r3;

	HK_TRANSPOSE4(c0, c1, c2, c3);

	setColumn<0>(c0);
	setColumn<1>(c1);
	setColumn<2>(c2);
	setColumn<3>(c3);
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
