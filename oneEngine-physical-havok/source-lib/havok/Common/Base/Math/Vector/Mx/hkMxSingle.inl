/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


#ifndef HK_DISABLE_MATH_CONSTRUCTORS

template <int M>
HK_FORCE_INLINE hkMxSingle<M>::hkMxSingle(hkSimdRealParameter v)
{
	m_single.s.setAll(v);
}

template <int M>
HK_FORCE_INLINE hkMxSingle<M>::hkMxSingle(hkVector4Parameter v)
{
	m_single.s = v;
}

#endif

template <int M>
HK_FORCE_INLINE void hkMxSingle<M>::operator= ( hkMxSingleParameter v )
{
	m_single.s = v.m_single.s;
}

template <int M>
HK_FORCE_INLINE const hkVector4& hkMxSingle<M>::getVector() const
{
	return m_single.s;
}

template <int M>
HK_FORCE_INLINE void hkMxSingle<M>::setVector(hkVector4Parameter r)
{
	m_single.s = r;
}

template <int M>
HK_FORCE_INLINE void hkMxSingle<M>::setZero()
{
	m_single.s.setZero();
}

template <int M>
template<int C>
HK_FORCE_INLINE void hkMxSingle<M>::setConstant()
{
	m_single.s = hkVector4::getConstant<C>();
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
