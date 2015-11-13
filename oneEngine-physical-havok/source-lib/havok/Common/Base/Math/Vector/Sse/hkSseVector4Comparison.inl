/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#if defined(HK_REAL_IS_DOUBLE)
#error This implementation is for 32-Bit float
#endif

static HK_ALIGN16 (const hkInt32 hkVector4Comparison_maskToComparison[16*4]) = 
{
	0,  0, 0, 0,
	~0,  0, 0, 0,
	0, ~0, 0, 0,
	~0, ~0, 0, 0,
	0,  0,~0, 0,
	~0,  0,~0, 0,
	0, ~0,~0, 0,
	~0, ~0,~0, 0,
	0,  0, 0,~0,
	~0,  0, 0,~0,
	0, ~0, 0,~0,
	~0, ~0, 0,~0,
	0,  0,~0,~0,
	~0,  0,~0,~0,
	0, ~0,~0,~0,
	~0, ~0,~0,~0,
};

HK_FORCE_INLINE /*static*/ const hkVector4Comparison HK_CALL hkVector4Comparison::convert(const hkVector4Mask& x)
{
	hkVector4Comparison c;
	c.m_mask = x;
	return c;
}

HK_FORCE_INLINE void hkVector4Comparison::setAnd( hkVector4ComparisonParameter a, hkVector4ComparisonParameter b )
{ 
	m_mask = _mm_and_ps( a.m_mask,b.m_mask ); 
}

HK_FORCE_INLINE void hkVector4Comparison::setAndNot( hkVector4ComparisonParameter a, hkVector4ComparisonParameter b )
{
	m_mask = _mm_andnot_ps(b.m_mask, a.m_mask);
}

HK_FORCE_INLINE void hkVector4Comparison::setNot( hkVector4ComparisonParameter a )
{ 
	m_mask = _mm_castsi128_ps(_mm_cmpeq_epi32(_mm_setzero_si128(), _mm_castps_si128(a.m_mask)));
}

HK_FORCE_INLINE void hkVector4Comparison::setOr( hkVector4ComparisonParameter a, hkVector4ComparisonParameter b ) 
{ 
	m_mask = _mm_or_ps( a.m_mask,b.m_mask ); 
}

HK_FORCE_INLINE void hkVector4Comparison::setSelect( hkVector4ComparisonParameter comp, hkVector4ComparisonParameter trueValue, hkVector4ComparisonParameter falseValue ) 
{
#if HK_SSE_VERSION >= 0x41
	m_mask = _mm_blendv_ps(falseValue.m_mask, trueValue.m_mask, comp.m_mask);
#else
	m_mask = _mm_or_ps( _mm_and_ps(comp.m_mask, trueValue.m_mask), _mm_andnot_ps(comp.m_mask, falseValue.m_mask) );
#endif
}

HK_FORCE_INLINE void hkVector4Comparison::set( Mask m ) 
{ 
	HK_COMPILE_TIME_ASSERT( hkVector4Comparison::INDEX_X == 0);
	HK_COMPILE_TIME_ASSERT( hkVector4Comparison::MASK_X == 1);
	HK_COMPILE_TIME_ASSERT( hkVector4Comparison::MASK_Y == 2);
	HK_COMPILE_TIME_ASSERT( hkVector4Comparison::MASK_XYZW == 15);

	HK_ASSERT2(0x557dac2a, (m&0xf)==m, "illegal mask value handed in");

	m_mask = *(hkVector4Mask*)&(hkVector4Comparison_maskToComparison[m*4]);
}

template <hkVector4Comparison::Mask M>
HK_FORCE_INLINE void hkVector4Comparison::set()
{ 
	m_mask = *(hkVector4Mask*)&(hkVector4Comparison_maskToComparison[M*4]);
}

HK_FORCE_INLINE hkBool32 hkVector4Comparison::allAreSet( Mask m ) const 
{ 
	HK_ASSERT2(0x71b4006b, (m&0xf)==m, "illegal mask value handed in");
#if HK_SSE_VERSION >= 0x41
	return _mm_testc_si128(_mm_castps_si128(m_mask), *(__m128i*)&(hkVector4Comparison_maskToComparison[m*4]));
#else
	return (_mm_movemask_ps(m_mask) & m) == m; 
#endif
}

HK_FORCE_INLINE hkBool32 hkVector4Comparison::anyIsSet( Mask m ) const 
{ 
	HK_ASSERT2(0x68b44aee, (m&0xf)==m, "illegal mask value handed in");
	return _mm_movemask_ps(m_mask) & m; 
}

HK_FORCE_INLINE hkBool32 hkVector4Comparison::allAreSet() const 
{ 
#if HK_SSE_VERSION >= 0x41
	return _mm_testc_si128(_mm_castps_si128(m_mask), *(__m128i*)&(hkVector4Comparison_maskToComparison[15*4]));
#else
	return _mm_movemask_ps(m_mask) == MASK_XYZW; 
#endif
}

HK_FORCE_INLINE hkBool32 hkVector4Comparison::anyIsSet() const 
{ 
	return _mm_movemask_ps(m_mask); 
}

HK_FORCE_INLINE hkVector4Comparison::Mask hkVector4Comparison::getMask() const 
{ 
	return (hkVector4Comparison::Mask)_mm_movemask_ps(m_mask); 
}

HK_FORCE_INLINE hkVector4Comparison::Mask hkVector4Comparison::getMask(Mask m) const 
{ 
	HK_ASSERT2(0x3d2fea61, (m&0xf)==m, "illegal mask value handed in");
	return (hkVector4Comparison::Mask)(_mm_movemask_ps(m_mask) & m); 
}

HK_FORCE_INLINE /*static*/ hkUint32 HK_CALL hkVector4Comparison::getCombinedMask(hkVector4ComparisonParameter ca, hkVector4ComparisonParameter cb, hkVector4ComparisonParameter cc )
{
	return _mm_movemask_ps(ca.m_mask) | (_mm_movemask_ps(cb.m_mask)<<4 ) | (_mm_movemask_ps(cc.m_mask)<<8);
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
