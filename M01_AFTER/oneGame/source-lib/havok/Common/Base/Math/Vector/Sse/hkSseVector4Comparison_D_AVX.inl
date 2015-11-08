/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#if !defined(HK_REAL_IS_DOUBLE) || HK_SSE_VERSION < 0x50
#error This implementation is for 64-Bit double with AVX SIMD instruction set
#endif

HK_FORCE_INLINE /*static*/ const hkVector4Comparison HK_CALL hkVector4Comparison::convert(const hkVector4Mask& x)
{
	hkVector4Comparison c;
	c.m_mask = x;
	return c;
}

HK_FORCE_INLINE void hkVector4Comparison::setAnd( hkVector4ComparisonParameter a, hkVector4ComparisonParameter b )
{ 
	m_mask = _mm256_and_pd( a.m_mask, b.m_mask ); 
}

HK_FORCE_INLINE void hkVector4Comparison::setAndNot( hkVector4ComparisonParameter a, hkVector4ComparisonParameter b )
{
	m_mask = _mm256_andnot_pd(b.m_mask, a.m_mask);
}

HK_FORCE_INLINE void hkVector4Comparison::setNot( hkVector4ComparisonParameter a )
{ 
	static HK_ALIGN32( const hkUint64 allone[4] ) = { 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff };
	m_mask = _mm256_andnot_pd(a.m_mask, *(const __m256d*)&allone);
}

HK_FORCE_INLINE void hkVector4Comparison::setOr( hkVector4ComparisonParameter a, hkVector4ComparisonParameter b ) 
{ 
	m_mask = _mm256_or_pd( a.m_mask, b.m_mask ); 
}

HK_FORCE_INLINE void hkVector4Comparison::setSelect( hkVector4ComparisonParameter comp, hkVector4ComparisonParameter trueValue, hkVector4ComparisonParameter falseValue ) 
{
	m_mask = _mm256_blendv_pd(falseValue.m_mask, trueValue.m_mask, comp.m_mask);
}

HK_FORCE_INLINE void hkVector4Comparison::set( Mask m ) 
{ 
	HK_ALIGN32(const hkUint64 maskbits[4 * 4]) = 
	{
		0xffffffffffffffff, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 
		0x0000000000000000, 0xffffffffffffffff, 0x0000000000000000, 0x0000000000000000, 
		0x0000000000000000, 0x0000000000000000, 0xffffffffffffffff, 0x0000000000000000, 
		0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0xffffffffffffffff, 
	};
	m_mask = _mm256_setzero_pd();
	if (m&MASK_X) m_mask = _mm256_or_pd(m_mask, *(const __m256d*)&(maskbits[0*4]));
	if (m&MASK_Y) m_mask = _mm256_or_pd(m_mask, *(const __m256d*)&(maskbits[1*4]));
	if (m&MASK_Z) m_mask = _mm256_or_pd(m_mask, *(const __m256d*)&(maskbits[2*4]));
	if (m&MASK_W) m_mask = _mm256_or_pd(m_mask, *(const __m256d*)&(maskbits[3*4]));
}

template <hkVector4Comparison::Mask M>
HK_FORCE_INLINE void hkVector4Comparison::set() 
{ 
	HK_ALIGN32(const hkUint64 maskbits[4 * 4]) = 
	{
		0xffffffffffffffff, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 
		0x0000000000000000, 0xffffffffffffffff, 0x0000000000000000, 0x0000000000000000, 
		0x0000000000000000, 0x0000000000000000, 0xffffffffffffffff, 0x0000000000000000, 
		0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0xffffffffffffffff, 
	};
	m_mask = _mm256_setzero_pd();
	if (M&MASK_X) m_mask = _mm256_or_pd(m_mask, *(const __m256d*)&(maskbits[0*4]));
	if (M&MASK_Y) m_mask = _mm256_or_pd(m_mask, *(const __m256d*)&(maskbits[1*4]));
	if (M&MASK_Z) m_mask = _mm256_or_pd(m_mask, *(const __m256d*)&(maskbits[2*4]));
	if (M&MASK_W) m_mask = _mm256_or_pd(m_mask, *(const __m256d*)&(maskbits[3*4]));
}

HK_FORCE_INLINE hkBool32 hkVector4Comparison::allAreSet( Mask m ) const 
{ 
	HK_ASSERT2(0x71b4006b, (m&0xf)==m, "illegal mask value handed in");
	int xyzw = _mm256_movemask_pd(m_mask);
	return (xyzw & m) == m; 
}

HK_FORCE_INLINE hkBool32 hkVector4Comparison::anyIsSet( Mask m ) const 
{ 
	HK_ASSERT2(0x68b44aee, (m&0xf)==m, "illegal mask value handed in");
	int xyzw = _mm256_movemask_pd(m_mask); 
	return xyzw & m;
}

HK_FORCE_INLINE hkBool32 hkVector4Comparison::allAreSet() const 
{ 
	int xyzw = _mm256_movemask_pd(m_mask); 
	return (xyzw == MASK_XYZW);
}

HK_FORCE_INLINE hkBool32 hkVector4Comparison::anyIsSet() const 
{ 
	return _mm256_movemask_pd(m_mask); 
}

HK_FORCE_INLINE hkVector4Comparison::Mask hkVector4Comparison::getMask() const 
{ 
	int xyzw = _mm256_movemask_pd(m_mask); 
	return (hkVector4Comparison::Mask)xyzw;
}

HK_FORCE_INLINE hkVector4Comparison::Mask hkVector4Comparison::getMask(Mask m) const 
{ 
	HK_ASSERT2(0x3d2fea61, (m&0xf)==m, "illegal mask value handed in");
	int xyzw = _mm256_movemask_pd(m_mask); 
	return (hkVector4Comparison::Mask)( xyzw & m );
}

HK_FORCE_INLINE /*static*/ hkUint32 HK_CALL hkVector4Comparison::getCombinedMask(hkVector4ComparisonParameter ca, hkVector4ComparisonParameter cb, hkVector4ComparisonParameter cc )
{
	return ca.getMask() | (cb.getMask()<<4) | (cc.getMask()<<8);
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
