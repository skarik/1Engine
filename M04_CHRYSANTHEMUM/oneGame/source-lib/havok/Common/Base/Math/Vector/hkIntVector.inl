/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#if defined(HK_USING_GENERIC_INT_VECTOR_IMPLEMENTATION)

HK_FORCE_INLINE /*static*/ const hkIntVector HK_CALL hkIntVector::cast(const hkVector4& x)
{
	hkIntVector iv;
#if defined(HK_REAL_IS_DOUBLE)
	HK_WARN_ONCE_ON_DEBUG_IF(true, 0x9f75ba24, "casting double precision hkVector4 to hkIntVector does not work - enforcing a copy");
	iv.setConvertF32toS32(x);
#else
	iv.m_quad = *((hkQuadUint*)&x.m_quad);
#endif
	return iv;
}

HK_FORCE_INLINE /*static*/ const hkVector4 HK_CALL hkIntVector::cast(const hkIntVector& x)
{
	hkVector4 fv;
#if defined(HK_REAL_IS_DOUBLE)
	HK_WARN_ONCE_ON_DEBUG_IF(true, 0x9f75ba25, "casting hkIntVector to double precision hkVector4 does not work - enforcing a copy");
	x.convertS32ToF32(fv);
#else
	fv.m_quad = *((hkQuadReal*)&x.m_quad);
#endif
	return fv;
}

#if defined(HK_VECTOR_PARTS_MUST_BE_VALID)
HK_FORCE_INLINE /*static*/ const hkIntVector HK_CALL hkIntVector::castForInt24(const hkVector4& x)
{
	hkIntVector iv;
#if defined(HK_REAL_IS_DOUBLE)
	HK_WARN_ONCE_ON_DEBUG_IF(true, 0x9f75ba24, "casting double precision hkVector4 to hkIntVector does not work - enforcing a copy");
	iv.setConvertF32toS32(x);
#else
	iv.m_quad = *((hkQuadUint*)&x.m_quad);
	iv.m_quad.u32[0] &= ~0x3f000000;
	iv.m_quad.u32[1] &= ~0x3f000000;
	iv.m_quad.u32[2] &= ~0x3f000000;
	iv.m_quad.u32[3] &= ~0x3f000000;
#endif
	return iv;
}

HK_FORCE_INLINE /*static*/ const hkVector4 HK_CALL hkIntVector::castForInt24(const hkIntVector& x)
{
	hkVector4 fv;
	hkIntVector vTemp; vTemp = x;
	vTemp.m_quad.u32[0] |= 0x3f000000;
	vTemp.m_quad.u32[1] |= 0x3f000000;
	vTemp.m_quad.u32[2] |= 0x3f000000;
	vTemp.m_quad.u32[3] |= 0x3f000000;

#if defined(HK_REAL_IS_DOUBLE)
	HK_WARN_ONCE_ON_DEBUG_IF(true, 0x9f75ba25, "casting hkIntVector to double precision hkVector4 does not work - enforcing a copy");
	x.convertS32ToF32(fv);
#else
	fv.m_quad = *((hkQuadReal*)&vTemp.m_quad);
#endif
	return fv;
}
#endif

//
// Clearing, loading, and storing
//

HK_FORCE_INLINE void hkIntVector::setZero()
{
	hkUint32* HK_RESTRICT q = m_quad.u32;
	q[0] = 0;
	q[1] = 0;
	q[2] = 0;
	q[3] = 0;
}

HK_FORCE_INLINE void hkIntVector::setAll(int i)
{
	hkInt32* HK_RESTRICT q = reinterpret_cast<hkInt32*>(m_quad.u32);
	q[0] = i;
	q[1] = i;
	q[2] = i;
	q[3] = i;
}

HK_FORCE_INLINE void hkIntVector::setFirstComponent(int value)
{
	hkInt32* HK_RESTRICT q = reinterpret_cast<hkInt32*>(m_quad.u32);
	q[0] = value;
	q[1] = 0;
	q[2] = 0;
	q[3] = 0;
}

template <int I>
HK_FORCE_INLINE void hkIntVector::setComponent(int value)
{
	HK_INT_VECTOR_SUBINDEX_CHECK;
	hkInt32* HK_RESTRICT q = reinterpret_cast<hkInt32*>(m_quad.u32);
	q[I] = value;
}

HK_FORCE_INLINE void hkIntVector::set(int x, int y, int z, int w)
{
	hkInt32* HK_RESTRICT q = reinterpret_cast<hkInt32*>(m_quad.u32);
	q[0] = x;
	q[1] = y;
	q[2] = z;
	q[3] = w;
}

template <int I>
HK_FORCE_INLINE int hkIntVector::getComponent() const
{
	HK_INT_VECTOR_SUBINDEX_CHECK;
	const hkInt32* HK_RESTRICT q = reinterpret_cast<const hkInt32*>(m_quad.u32);
	return q[I];
}

template <int I>
HK_FORCE_INLINE hkUint8  hkIntVector::getU8 () const
{
	HK_COMPILE_TIME_ASSERT2((I>=0)&&(I<16), HK_INT_VECTOR_SUBVECTOR_INDEX_OUT_OF_RANGE);
	const hkUint8* HK_RESTRICT q = reinterpret_cast<const hkUint8*>(m_quad.u32);
	return q[I];
}

template <int I>
HK_FORCE_INLINE hkUint16 hkIntVector::getU16() const
{
	HK_COMPILE_TIME_ASSERT2((I>=0)&&(I<8), HK_INT_VECTOR_SUBVECTOR_INDEX_OUT_OF_RANGE);
	const hkUint16* HK_RESTRICT q = reinterpret_cast<const hkUint16*>(m_quad.u32);
	return q[I];
}

template <int I>
HK_FORCE_INLINE hkUint32 hkIntVector::getU32() const
{
	return getComponent<I>();
}

HK_FORCE_INLINE hkUint32 hkIntVector::getU32(int idx) const
{
	HK_ASSERT2(0xfabb2300, idx>=0 && idx<4, "int vector subindex out of range");
	const hkUint32* HK_RESTRICT q = m_quad.u32;
	return q[idx];
}

//
// Logical operations
//

HK_FORCE_INLINE void hkIntVector::setNot(hkIntVectorParameter a)
{
	const hkUint32* HK_RESTRICT qa = a.m_quad.u32;
	hkUint32* HK_RESTRICT qu = m_quad.u32;
	qu[0] = ~qa[0];
	qu[1] = ~qa[1];
	qu[2] = ~qa[2];
	qu[3] = ~qa[3];
}

HK_FORCE_INLINE hkBool32 hkIntVector::isNegativeAssumingAllValuesEqual(  ) const
{
	const hkUint32* HK_RESTRICT qu = m_quad.u32;
	HK_ASSERT2(0x252d00fa, qu[0] == qu[1] && qu[0] == qu[2] && qu[0] == qu[3], "assumption all values equal failed");
	return qu[0] & 0x80000000;
}

HK_FORCE_INLINE const hkVector4Comparison hkIntVector::compareLessThanS32(hkIntVectorParameter b ) const
{
	const hkInt32* HK_RESTRICT sa = reinterpret_cast<const hkInt32*>(m_quad.u32);
	const hkInt32* HK_RESTRICT sb = reinterpret_cast<const hkInt32*>(b.m_quad.u32);
	hkVector4Comparison	cmp;
	cmp.m_mask =(sa[0] < sb[0] ? hkVector4Comparison::MASK_X : hkVector4Comparison::MASK_NONE) |
				(sa[1] < sb[1] ? hkVector4Comparison::MASK_Y : hkVector4Comparison::MASK_NONE) |
				(sa[2] < sb[2] ? hkVector4Comparison::MASK_Z : hkVector4Comparison::MASK_NONE) |
				(sa[3] < sb[3] ? hkVector4Comparison::MASK_W : hkVector4Comparison::MASK_NONE);
	return cmp;
}

HK_FORCE_INLINE const hkVector4Comparison hkIntVector::compareEqualS32(hkIntVectorParameter b ) const
{
	const hkInt32* HK_RESTRICT sa = reinterpret_cast<const hkInt32*>(m_quad.u32);
	const hkInt32* HK_RESTRICT sb = reinterpret_cast<const hkInt32*>(b.m_quad.u32);
	hkVector4Comparison	cmp;
	cmp.m_mask =(sa[0] == sb[0] ? hkVector4Comparison::MASK_X : hkVector4Comparison::MASK_NONE) |
				(sa[1] == sb[1] ? hkVector4Comparison::MASK_Y : hkVector4Comparison::MASK_NONE) |
				(sa[2] == sb[2] ? hkVector4Comparison::MASK_Z : hkVector4Comparison::MASK_NONE) |
				(sa[3] == sb[3] ? hkVector4Comparison::MASK_W : hkVector4Comparison::MASK_NONE);
	return cmp;
}

HK_FORCE_INLINE void hkIntVector::setOr(hkIntVectorParameter a, hkIntVectorParameter b)
{
	const hkUint32* HK_RESTRICT qa = a.m_quad.u32;
	const hkUint32* HK_RESTRICT qb = b.m_quad.u32;
	hkUint32* HK_RESTRICT qu = m_quad.u32;
	qu[0] = qa[0] | qb[0];
	qu[1] = qa[1] | qb[1];
	qu[2] = qa[2] | qb[2];
	qu[3] = qa[3] | qb[3];
}

HK_FORCE_INLINE void hkIntVector::setAnd(hkIntVectorParameter a, hkIntVectorParameter b)
{
	const hkUint32* HK_RESTRICT qa = a.m_quad.u32;
	const hkUint32* HK_RESTRICT qb = b.m_quad.u32;
	hkUint32* HK_RESTRICT qu = m_quad.u32;
	qu[0] = qa[0] & qb[0];
	qu[1] = qa[1] & qb[1];
	qu[2] = qa[2] & qb[2];
	qu[3] = qa[3] & qb[3];
}

HK_FORCE_INLINE void hkIntVector::setXor(hkIntVectorParameter a, hkIntVectorParameter b)
{
	const hkUint32* HK_RESTRICT qa = a.m_quad.u32;
	const hkUint32* HK_RESTRICT qb = b.m_quad.u32;
	hkUint32* HK_RESTRICT qu = m_quad.u32;
	qu[0] = qa[0] ^ qb[0];
	qu[1] = qa[1] ^ qb[1];
	qu[2] = qa[2] ^ qb[2];
	qu[3] = qa[3] ^ qb[3];
}

HK_FORCE_INLINE void hkIntVector::setAndNot(hkIntVectorParameter a, hkIntVectorParameter b)
{
	const hkUint32* HK_RESTRICT qa = a.m_quad.u32;
	const hkUint32* HK_RESTRICT qb = b.m_quad.u32;
	hkUint32* HK_RESTRICT qu = m_quad.u32;
	qu[0] = qa[0] & ~qb[0];
	qu[1] = qa[1] & ~qb[1];
	qu[2] = qa[2] & ~qb[2];
	qu[3] = qa[3] & ~qb[3];
}

//
// Arithmetic operations
//

HK_FORCE_INLINE void hkIntVector::setSubU16( hkIntVectorParameter a, hkIntVectorParameter b )
{
	const hkUint16* HK_RESTRICT qa = reinterpret_cast<const hkUint16*>(a.m_quad.u32);
	const hkUint16* HK_RESTRICT qb = reinterpret_cast<const hkUint16*>(b.m_quad.u32);
	hkUint16* HK_RESTRICT qu = reinterpret_cast<hkUint16*>(m_quad.u32);
	qu[0] = qa[0] - qb[0];
	qu[1] = qa[1] - qb[1];
	qu[2] = qa[2] - qb[2];
	qu[3] = qa[3] - qb[3];
	qu[4] = qa[4] - qb[4];
	qu[5] = qa[5] - qb[5];
	qu[6] = qa[6] - qb[6];
	qu[7] = qa[7] - qb[7];
}

HK_FORCE_INLINE void hkIntVector::setAddU16( hkIntVectorParameter a, hkIntVectorParameter b )
{
	const hkUint16* HK_RESTRICT qa = reinterpret_cast<const hkUint16*>(a.m_quad.u32);
	const hkUint16* HK_RESTRICT qb = reinterpret_cast<const hkUint16*>(b.m_quad.u32);
	hkUint16* HK_RESTRICT qu = reinterpret_cast<hkUint16*>(m_quad.u32);
	qu[0] = qa[0] + qb[0];
	qu[1] = qa[1] + qb[1];
	qu[2] = qa[2] + qb[2];
	qu[3] = qa[3] + qb[3];
	qu[4] = qa[4] + qb[4];
	qu[5] = qa[5] + qb[5];
	qu[6] = qa[6] + qb[6];
	qu[7] = qa[7] + qb[7];
}

HK_FORCE_INLINE void hkIntVector::setSubU32( hkIntVectorParameter a, hkIntVectorParameter b )
{
	const hkUint32* HK_RESTRICT qa = a.m_quad.u32;
	const hkUint32* HK_RESTRICT qb = b.m_quad.u32;
	hkUint32* HK_RESTRICT qu = m_quad.u32;
	qu[0] = qa[0] - qb[0];
	qu[1] = qa[1] - qb[1];
	qu[2] = qa[2] - qb[2];
	qu[3] = qa[3] - qb[3];
}

HK_FORCE_INLINE void hkIntVector::setAddU32( hkIntVectorParameter a, hkIntVectorParameter b )
{
	const hkUint32* HK_RESTRICT qa = a.m_quad.u32;
	const hkUint32* HK_RESTRICT qb = b.m_quad.u32;
	hkUint32* HK_RESTRICT qu = m_quad.u32;
	qu[0] = qa[0] + qb[0];
	qu[1] = qa[1] + qb[1];
	qu[2] = qa[2] + qb[2];
	qu[3] = qa[3] + qb[3];
}

HK_FORCE_INLINE void hkIntVector::setAddSaturateU32( hkIntVectorParameter a, hkIntVectorParameter b )
{
	const hkUint32* HK_RESTRICT qa = a.m_quad.u32;
	const hkUint32* HK_RESTRICT qb = b.m_quad.u32;
	hkUint32* HK_RESTRICT qr = m_quad.u32;
	const hkUint32 max = 0xffffffff;

	qr[0] = (qa[0] < max - qb[0]) ? (qa[0] + qb[0]) : max;
	qr[1] = (qa[1] < max - qb[1]) ? (qa[1] + qb[1]) : max;
	qr[2] = (qa[2] < max - qb[2]) ? (qa[2] + qb[2]) : max;
	qr[3] = (qa[3] < max - qb[3]) ? (qa[3] + qb[3]) : max;
}

HK_FORCE_INLINE void hkIntVector::setAddSaturateS16( hkIntVectorParameter a, hkIntVectorParameter b )
{
	const hkInt16* HK_RESTRICT qa = reinterpret_cast<const hkInt16*>(a.m_quad.u32);
	const hkInt16* HK_RESTRICT qb = reinterpret_cast<const hkInt16*>(b.m_quad.u32);
	hkInt16* HK_RESTRICT qu = reinterpret_cast<hkInt16*>(m_quad.u32);

	const int max = +32767;
	const int min = -32768;

	for(int i = 0 ; i < 8 ; i++)
	{
		qu[i] = (hkInt16)hkMath::clamp<int>(int(qa[i]) + int(qb[i]), min, max);
	}
}

HK_FORCE_INLINE void hkIntVector::setAddSaturateU16( hkIntVectorParameter a, hkIntVectorParameter b )
{
	const hkUint16* HK_RESTRICT qa = reinterpret_cast<const hkUint16*>(a.m_quad.u32);
	const hkUint16* HK_RESTRICT qb = reinterpret_cast<const hkUint16*>(b.m_quad.u32);
	hkUint16* HK_RESTRICT qr = reinterpret_cast<hkUint16*>(m_quad.u32);
	const hkUint16 max = 0xffff;

	qr[0] = (qa[0] < max - qb[0]) ? (qa[0] + qb[0]) : max;
	qr[1] = (qa[1] < max - qb[1]) ? (qa[1] + qb[1]) : max;
	qr[2] = (qa[2] < max - qb[2]) ? (qa[2] + qb[2]) : max;
	qr[3] = (qa[3] < max - qb[3]) ? (qa[3] + qb[3]) : max;
	qr[4] = (qa[4] < max - qb[4]) ? (qa[4] + qb[4]) : max;
	qr[5] = (qa[5] < max - qb[5]) ? (qa[5] + qb[5]) : max;
	qr[6] = (qa[6] < max - qb[6]) ? (qa[6] + qb[6]) : max;
	qr[7] = (qa[7] < max - qb[7]) ? (qa[7] + qb[7]) : max;
}


HK_FORCE_INLINE void hkIntVector::setSubSaturateU32( hkIntVectorParameter a, hkIntVectorParameter b )
{
	const hkUint32* HK_RESTRICT qa = a.m_quad.u32;
	const hkUint32* HK_RESTRICT qb = b.m_quad.u32;
	hkUint32* HK_RESTRICT qr = m_quad.u32;

	qr[0] = (qa[0] > qb[0]) ? (qa[0] - qb[0]) : 0;
	qr[1] = (qa[1] > qb[1]) ? (qa[1] - qb[1]) : 0;
	qr[2] = (qa[2] > qb[2]) ? (qa[2] - qb[2]) : 0;
	qr[3] = (qa[3] > qb[3]) ? (qa[3] - qb[3]) : 0;
}

HK_FORCE_INLINE void hkIntVector::setSubSaturateS16( hkIntVectorParameter a, hkIntVectorParameter b )
{
	const hkInt16* HK_RESTRICT qa = reinterpret_cast<const hkInt16*>(a.m_quad.u32);
	const hkInt16* HK_RESTRICT qb = reinterpret_cast<const hkInt16*>(b.m_quad.u32);
	hkInt16* HK_RESTRICT qu = reinterpret_cast<hkInt16*>(m_quad.u32);

	const int min = -32768;
	const int max = +32767;

	for(int i = 0 ; i < 8 ; i++)
	{
		qu[i] = (hkInt16)hkMath::clamp<int>(int(qa[i]) - int(qb[i]), min, max); 
	}
}

HK_FORCE_INLINE void hkIntVector::setSubSaturateU16( hkIntVectorParameter a, hkIntVectorParameter b )
{
	const hkUint16* HK_RESTRICT qa = reinterpret_cast<const hkUint16*>(a.m_quad.u32);
	const hkUint16* HK_RESTRICT qb = reinterpret_cast<const hkUint16*>(b.m_quad.u32);
	hkUint16* HK_RESTRICT qr = reinterpret_cast<hkUint16*>(m_quad.u32);

	qr[0] = (qa[0] > qb[0]) ? (qa[0] - qb[0]) : 0; 
	qr[1] = (qa[1] > qb[1]) ? (qa[1] - qb[1]) : 0; 
	qr[2] = (qa[2] > qb[2]) ? (qa[2] - qb[2]) : 0; 
	qr[3] = (qa[3] > qb[3]) ? (qa[3] - qb[3]) : 0; 
	qr[4] = (qa[4] > qb[4]) ? (qa[4] - qb[4]) : 0; 
	qr[5] = (qa[5] > qb[5]) ? (qa[5] - qb[5]) : 0; 
	qr[6] = (qa[6] > qb[6]) ? (qa[6] - qb[6]) : 0; 
	qr[7] = (qa[7] > qb[7]) ? (qa[7] - qb[7]) : 0; 
}

HK_FORCE_INLINE void hkIntVector::setMinU8( hkIntVectorParameter a, hkIntVectorParameter b )
{
	const hkUint8* HK_RESTRICT qa = reinterpret_cast<const hkUint8*>(a.m_quad.u32);
	const hkUint8* HK_RESTRICT qb = reinterpret_cast<const hkUint8*>(b.m_quad.u32);
	hkUint8* HK_RESTRICT qu = reinterpret_cast<hkUint8*>(m_quad.u32);
	
	for(int i = 0; i < 16; i++)
	{
		qu[i] = hkMath::min2(qa[i], qb[i]);  
	}
}

HK_FORCE_INLINE void hkIntVector::setMinS32( hkIntVectorParameter a, hkIntVectorParameter b )
{
	const hkInt32* HK_RESTRICT qa = reinterpret_cast<const hkInt32*>(a.m_quad.u32);
	const hkInt32* HK_RESTRICT qb = reinterpret_cast<const hkInt32*>(b.m_quad.u32);
	hkInt32* HK_RESTRICT qr = reinterpret_cast<hkInt32*>(m_quad.u32);

	qr[0] = hkMath::min2(qa[0], qb[0]);
	qr[1] = hkMath::min2(qa[1], qb[1]);
	qr[2] = hkMath::min2(qa[2], qb[2]);
	qr[3] = hkMath::min2(qa[3], qb[3]);
}

HK_FORCE_INLINE void hkIntVector::setMaxS32( hkIntVectorParameter a, hkIntVectorParameter b )
{
	const hkInt32* HK_RESTRICT qa = reinterpret_cast<const hkInt32*>(a.m_quad.u32);
	const hkInt32* HK_RESTRICT qb = reinterpret_cast<const hkInt32*>(b.m_quad.u32);
	hkInt32* HK_RESTRICT qr = reinterpret_cast<hkInt32*>(m_quad.u32);

	qr[0] = hkMath::max2(qa[0], qb[0]);
	qr[1] = hkMath::max2(qa[1], qb[1]);
	qr[2] = hkMath::max2(qa[2], qb[2]);
	qr[3] = hkMath::max2(qa[3], qb[3]);
}

//
// Shift operations
//

template <int shift> 
HK_FORCE_INLINE void hkIntVector::setShiftLeft16( hkIntVectorParameter a)
{
	const hkUint16* HK_RESTRICT qa = reinterpret_cast<const hkUint16*>(a.m_quad.u32);
	hkUint16* HK_RESTRICT qu = reinterpret_cast<hkUint16*>(m_quad.u32);
	qu[0] = qa[0] << shift;
	qu[1] = qa[1] << shift;
	qu[2] = qa[2] << shift;
	qu[3] = qa[3] << shift;
	qu[4] = qa[4] << shift;
	qu[5] = qa[5] << shift;
	qu[6] = qa[6] << shift;
	qu[7] = qa[7] << shift;
}

template <int shift> 
HK_FORCE_INLINE void hkIntVector::setShiftRight16( hkIntVectorParameter a)
{
	const hkUint16* HK_RESTRICT qa = reinterpret_cast<const hkUint16*>(a.m_quad.u32);
	hkUint16* HK_RESTRICT qu = reinterpret_cast<hkUint16*>(m_quad.u32);
	qu[0] = qa[0] >> shift;
	qu[1] = qa[1] >> shift;
	qu[2] = qa[2] >> shift;
	qu[3] = qa[3] >> shift;
	qu[4] = qa[4] >> shift;
	qu[5] = qa[5] >> shift;
	qu[6] = qa[6] >> shift;
	qu[7] = qa[7] >> shift;
}

template <int shift> 
HK_FORCE_INLINE void hkIntVector::setShiftLeft32( hkIntVectorParameter a)
{
	const hkUint32* HK_RESTRICT qa = a.m_quad.u32;
	hkUint32* HK_RESTRICT qu = m_quad.u32;
	qu[0] = qa[0] << shift;
	qu[1] = qa[1] << shift;
	qu[2] = qa[2] << shift;
	qu[3] = qa[3] << shift;
}

template <int shift> 
HK_FORCE_INLINE void hkIntVector::setShiftRight32( hkIntVectorParameter a)
{
	const hkUint32* HK_RESTRICT qa = a.m_quad.u32;
	hkUint32* HK_RESTRICT qu = m_quad.u32;
	qu[0] = qa[0] >> shift;
	qu[1] = qa[1] >> shift;
	qu[2] = qa[2] >> shift;
	qu[3] = qa[3] >> shift;
}

template<int shift>
HK_FORCE_INLINE void hkIntVector::setShiftRightS32( hkIntVectorParameter a)
{
	const hkInt32* HK_RESTRICT qa = reinterpret_cast<const hkInt32*>(a.m_quad.u32);
	hkInt32* HK_RESTRICT qu = reinterpret_cast<hkInt32*>(m_quad.u32);
	qu[0] = qa[0] >> shift;
	qu[1] = qa[1] >> shift;
	qu[2] = qa[2] >> shift;
	qu[3] = qa[3] >> shift;
}

template<int shift>
HK_FORCE_INLINE void hkIntVector::setShiftRightS16( hkIntVectorParameter a)
{
	const hkInt16* HK_RESTRICT qa = reinterpret_cast<const hkInt16*>(a.m_quad.u32);
	hkInt16* HK_RESTRICT qu = reinterpret_cast<hkInt16*>(m_quad.u32);
	qu[0] = qa[0] >> shift;
	qu[1] = qa[1] >> shift;
	qu[2] = qa[2] >> shift;
	qu[3] = qa[3] >> shift;
	qu[4] = qa[4] >> shift;
	qu[5] = qa[5] >> shift;
	qu[6] = qa[6] >> shift;
	qu[7] = qa[7] >> shift;
}

template <int shift> 
HK_FORCE_INLINE void hkIntVector::setShiftLeft128( hkIntVectorParameter a)
{
	const hkUint8* HK_RESTRICT qa = reinterpret_cast<const hkUint8*>(a.m_quad.u32);
	hkQuadUint u;
	hkUint8* HK_RESTRICT qu = reinterpret_cast<hkUint8*>(u.u32);

	for(int j = 16 - shift ; j < 16 ; j++)
	{
		qu[j] = 0;
	}

	for(int i = 15 - shift ; i >= 0 ; i--)
	{
		qu[i] = qa[i + shift];
	}

	m_quad = u;
}

template <int shift> 
HK_FORCE_INLINE void hkIntVector::setShiftRight128( hkIntVectorParameter a)
{
	const hkUint8* HK_RESTRICT qa = reinterpret_cast<const hkUint8*>(a.m_quad.u32);
	hkQuadUint u;
	hkUint8* HK_RESTRICT qu = reinterpret_cast<hkUint8*>(u.u32);

	for(int i = 15, j = 15 - shift ; i >= shift ; i--, j--)
	{
		qu[i] = qa[j];
	}
	for(int j = shift - 1 ; j >= 0 ; j--)
	{
		qu[j] = 0;
	}

	m_quad = u;
}

//
// Merge operations
// Stick to the convention that "high" means x,y, "low" means z,w
//
HK_FORCE_INLINE void hkIntVector::setMergeHead32(hkIntVectorParameter a, hkIntVectorParameter b)
{
	const hkUint32* HK_RESTRICT qa = a.m_quad.u32;
	const hkUint32* HK_RESTRICT qb = b.m_quad.u32;
	hkQuadUint u;
	hkUint32* HK_RESTRICT qu = u.u32;

	qu[0] = qa[0]; 
	qu[1] = qb[0];
	qu[2] = qa[1];
	qu[3] = qb[1];

	m_quad = u;
}

HK_FORCE_INLINE void hkIntVector::setMergeTail32(hkIntVectorParameter a, hkIntVectorParameter b)
{
	const hkUint32* HK_RESTRICT qa = a.m_quad.u32;
	const hkUint32* HK_RESTRICT qb = b.m_quad.u32;
	hkQuadUint u;
	hkUint32* HK_RESTRICT qu = u.u32;

	qu[0] = qa[2];
	qu[1] = qb[2];
	qu[2] = qa[3];
	qu[3] = qb[3];

	m_quad = u;
}

HK_FORCE_INLINE void hkIntVector::setMergeHead16(hkIntVectorParameter a, hkIntVectorParameter b)
{
	const hkUint16* HK_RESTRICT qa = reinterpret_cast<const hkUint16*>(a.m_quad.u32);
	const hkUint16* HK_RESTRICT qb = reinterpret_cast<const hkUint16*>(b.m_quad.u32);
	hkQuadUint u;
	hkUint16* HK_RESTRICT qu = reinterpret_cast<hkUint16*>(u.u32);

	for (int i=0, j=0; i<8; i+=2, j++)
	{
		qu[i]   = qa[j];
		qu[i+1] = qb[j];
	}

	m_quad = u;
}

HK_FORCE_INLINE void hkIntVector::setMergeTail16(hkIntVectorParameter a, hkIntVectorParameter b)
{
	const hkUint16* HK_RESTRICT qa = reinterpret_cast<const hkUint16*>(a.m_quad.u32);
	const hkUint16* HK_RESTRICT qb = reinterpret_cast<const hkUint16*>(b.m_quad.u32);
	hkQuadUint u;
	hkUint16* HK_RESTRICT qu = reinterpret_cast<hkUint16*>(u.u32);

	for (int i=0, j=4; i<8; i+=2, j++)
	{
		qu[i]   = qa[j];
		qu[i+1] = qb[j];
	}

	m_quad = u;
}

HK_FORCE_INLINE void hkIntVector::setMergeHead8(hkIntVectorParameter a, hkIntVectorParameter b)
{
	const hkUint8* HK_RESTRICT qa = reinterpret_cast<const hkUint8*>(a.m_quad.u32);
	const hkUint8* HK_RESTRICT qb = reinterpret_cast<const hkUint8*>(b.m_quad.u32);
	hkQuadUint u;
	hkUint8* HK_RESTRICT qu = reinterpret_cast<hkUint8*>(u.u32);

	for (int i=0, j=0; i<16; i+=2, j++)
	{
		qu[i]   = qa[j];
		qu[i+1] = qb[j];
	}

	m_quad = u;
}

HK_FORCE_INLINE void hkIntVector::setMergeTail8(hkIntVectorParameter a, hkIntVectorParameter b)
{
	const hkUint8* HK_RESTRICT qa = reinterpret_cast<const hkUint8*>(a.m_quad.u32);
	const hkUint8* HK_RESTRICT qb = reinterpret_cast<const hkUint8*>(b.m_quad.u32);
	hkQuadUint u;
	hkUint8* HK_RESTRICT qu = reinterpret_cast<hkUint8*>(u.u32);

	for (int i=0, j=8; i<16; i+=2, j++)
	{
		qu[i]   = qa[j];
		qu[i+1] = qb[j];
	}

	m_quad = u;
}


//
// Pack operations
//
HK_FORCE_INLINE void hkIntVector::setConvertU32ToU16( hkIntVectorParameter a, hkIntVectorParameter b )
{
	const hkUint32* HK_RESTRICT qa = a.m_quad.u32;
	const hkUint32* HK_RESTRICT qb = b.m_quad.u32;
	hkQuadUint u;
	hkUint16* HK_RESTRICT qu = reinterpret_cast<hkUint16*>(u.u32);

	qu[0] = (hkUint16)qa[0];
	qu[1] = (hkUint16)qa[1];
	qu[2] = (hkUint16)qa[2];
	qu[3] = (hkUint16)qa[3];
	qu[4] = (hkUint16)qb[0];
	qu[5] = (hkUint16)qb[1];
	qu[6] = (hkUint16)qb[2];
	qu[7] = (hkUint16)qb[3];

	m_quad = u;
}

HK_FORCE_INLINE void hkIntVector::setConvertSaturateS32ToU16( hkIntVectorParameter a, hkIntVectorParameter b )
{
	const hkInt32* HK_RESTRICT qa = reinterpret_cast<const hkInt32*>(a.m_quad.u32);
	const hkInt32* HK_RESTRICT qb = reinterpret_cast<const hkInt32*>(b.m_quad.u32);
	hkQuadUint u;
	hkUint16* HK_RESTRICT qu = reinterpret_cast<hkUint16*>(u.u32);

	const hkInt32 min = 0;
	const hkInt32 max = 0x10000; // ? todo shouldnt that be 0xffff 

	for (int i=0; i<4; ++i)
	{
		qu[i]   = (hkUint16)hkMath::clamp(qa[i], min, max);
		qu[i+4] = (hkUint16)hkMath::clamp(qb[i], min, max);
	}

	m_quad = u;
}

HK_FORCE_INLINE void hkIntVector::setConvertSaturateS16ToU8( hkIntVectorParameter a, hkIntVectorParameter b )
{
	const hkInt16* HK_RESTRICT qa = reinterpret_cast<const hkInt16*>(a.m_quad.u32);
	const hkInt16* HK_RESTRICT qb = reinterpret_cast<const hkInt16*>(b.m_quad.u32);
	hkQuadUint u;
	hkUint8* HK_RESTRICT qu = reinterpret_cast<hkUint8*>(u.u32);

	const hkInt16 min = 0;
	const hkInt16 max = 255;

	for (int i=0; i<8; ++i)
	{
		qu[i]   = (hkUint8)hkMath::clamp(qa[i], min, max);
		qu[i+8] = (hkUint8)hkMath::clamp(qb[i], min, max);
	}

	m_quad = u;
}

//
//	Sets this.u32[i] = (unsigned int)a.u16[i]

HK_FORCE_INLINE void hkIntVector::setConvertLowerU16ToU32(hkIntVectorParameter a)
{
	const hkUint16* HK_RESTRICT qa = reinterpret_cast<const hkUint16*>(a.m_quad.u32);
	hkQuadUint u;
	hkUint32* HK_RESTRICT qu = u.u32;
	qu[0] = (hkUint32)qa[0];
	qu[1] = (hkUint32)qa[1];
	qu[2] = (hkUint32)qa[2];
	qu[3] = (hkUint32)qa[3];
	m_quad = u;
}

//
//	Sets this.u32[i] = (unsigned int)a.u16[i + 4]

HK_FORCE_INLINE void hkIntVector::setConvertUpperU16ToU32(hkIntVectorParameter a)
{
	const hkUint16* HK_RESTRICT qa = reinterpret_cast<const hkUint16*>(a.m_quad.u32);
	hkQuadUint u;
	hkUint32* HK_RESTRICT qu = u.u32;
	qu[0] = (hkUint32)qa[4];
	qu[1] = (hkUint32)qa[5];
	qu[2] = (hkUint32)qa[6];
	qu[3] = (hkUint32)qa[7];
	m_quad = u;
}

//
// Shuffle
//
template <hkVectorPermutation::Permutation P> 
HK_FORCE_INLINE void hkIntVector::setPermutation(hkIntVectorParameter v)
{
	const hkUint32* HK_RESTRICT qa = v.m_quad.u32;
	hkQuadUint u;
	hkUint32* HK_RESTRICT qu = u.u32;
	qu[0] = qa[(P & 0x3000) >> 12];
	qu[1] = qa[(P & 0x0300) >>  8];
	qu[2] = qa[(P & 0x0030) >>  4];
	qu[3] = qa[(P & 0x0003) >>  0];
	m_quad = u;
}

HK_FORCE_INLINE void hkIntVector::setPermuteU8(hkIntVectorParameter aIn, hkIntVectorParameter mask)
{
	const hkUint8* HK_RESTRICT m = reinterpret_cast<const hkUint8*>(mask.m_quad.u32);
	const hkUint8* HK_RESTRICT a = reinterpret_cast<const hkUint8*>(aIn.m_quad.u32);
	hkQuadUint u;
	hkUint8* HK_RESTRICT r = reinterpret_cast<hkUint8*>(u.u32);
	for (int i = 0; i < 16; i++)
	{
		HK_ASSERT2(0xf820d0c2, (m[i] & 0xF0)==0, "upper 4 bits not zero");
		r[i] = a[m[i]];
	}
	m_quad = u;
}

HK_FORCE_INLINE void hkIntVector::setPermuteU8(hkIntVectorParameter aIn, hkIntVectorParameter bIn, hkIntVectorParameter mask)
{
	const hkUint8* HK_RESTRICT m = reinterpret_cast<const hkUint8*>(mask.m_quad.u32);
	const hkUint8* HK_RESTRICT a = reinterpret_cast<const hkUint8*>(aIn.m_quad.u32);
	const hkUint8* HK_RESTRICT b = reinterpret_cast<const hkUint8*>(bIn.m_quad.u32);
	hkQuadUint u;
	hkUint8* HK_RESTRICT r = reinterpret_cast<hkUint8*>(u.u32);
	for (int i = 0; i < 16; i++)
	{
		if (m[i] & 0xF0)
		{
			r[i] = b[m[i] & 0x0F];
		}
		else
		{
			r[i] = a[m[i] & 0x0F];
		}
	}
	m_quad = u;
}

//
// Splat
//
template <int VALUE> 
HK_FORCE_INLINE void hkIntVector::splatImmediate32()
{
	HK_INT_VECTOR_IMM_SPLAT_VALUE_CHECK;
	hkInt32* HK_RESTRICT qu = reinterpret_cast<hkInt32*>(m_quad.u32);
	qu[0] = VALUE;
	qu[1] = VALUE;
	qu[2] = VALUE;
	qu[3] = VALUE;
}

template <int VALUE> 
HK_FORCE_INLINE void hkIntVector::splatImmediate16()
{
	HK_INT_VECTOR_IMM_SPLAT_VALUE_CHECK;
	hkUint16* HK_RESTRICT qu = reinterpret_cast<hkUint16*>(m_quad.u32);
	qu[0] = (hkUint16) VALUE;
	qu[1] = (hkUint16) VALUE;
	qu[2] = (hkUint16) VALUE;
	qu[3] = (hkUint16) VALUE;
	qu[4] = (hkUint16) VALUE;
	qu[5] = (hkUint16) VALUE;
	qu[6] = (hkUint16) VALUE;
	qu[7] = (hkUint16) VALUE;
}

template <int VALUE> 
HK_FORCE_INLINE void hkIntVector::splatImmediate8()
{
	HK_INT_VECTOR_IMM_SPLAT_VALUE_CHECK;
	hkUint8* HK_RESTRICT qu = reinterpret_cast<hkUint8*>(m_quad.u32);
	for (int i=0; i<16; i++)
	{
		qu[i] = (hkUint8) VALUE;
	}
}

template <int I> 
HK_FORCE_INLINE void hkIntVector::setBroadcast(hkIntVectorParameter a)
{
	HK_INT_VECTOR_SUBINDEX_CHECK;
	const hkUint32* HK_RESTRICT qa = a.m_quad.u32;
	hkUint32* HK_RESTRICT qu = m_quad.u32;
	hkUint32 value = qa[I];
	qu[0] = value;
	qu[1] = value;
	qu[2] = value;
	qu[3] = value;
}

template<int I>
HK_FORCE_INLINE void hkIntVector::setBroadcast8(hkIntVectorParameter a)
{
	const hkUint8* HK_RESTRICT qa = reinterpret_cast<const hkUint8*>(a.m_quad.u32);
	hkUint8* HK_RESTRICT qu = reinterpret_cast<hkUint8*>(m_quad.u32);
	hkUint8 value = qa[I];

	qu[0] = value;
	qu[1] = value;
	qu[2] = value;
	qu[3] = value;
	qu[4] = value;
	qu[5] = value;
	qu[6] = value;
	qu[7] = value;
	qu[8] = value;
	qu[9] = value;
	qu[10] = value;
	qu[11] = value;
	qu[12] = value;
	qu[13] = value;
	qu[14] = value;
	qu[15] = value;
}

template<int I>
HK_FORCE_INLINE void hkIntVector::setBroadcast16(hkIntVectorParameter a)
{
	const hkUint16* HK_RESTRICT qa = reinterpret_cast<const hkUint16*>(a.m_quad.u32);
	hkUint16* HK_RESTRICT qu = reinterpret_cast<hkUint16*>(m_quad.u32);
	hkUint16 value = qa[I];

	qu[0] = value;
	qu[1] = value;
	qu[2] = value;
	qu[3] = value;
	qu[4] = value;
	qu[5] = value;
	qu[6] = value;
	qu[7] = value;
}

//
// int <-> float conversion
//
HK_FORCE_INLINE void hkIntVector::convertU32ToF32(hkVector4& vOut) const
{
	const hkUint32* HK_RESTRICT qu = m_quad.u32;
	vOut.set( hkReal(qu[0]), hkReal(qu[1]), hkReal(qu[2]), hkReal(qu[3]));
}

HK_FORCE_INLINE void hkIntVector::convertS32ToF32(hkVector4& vOut) const
{
	const hkInt32* HK_RESTRICT qu = reinterpret_cast<const hkInt32*>(m_quad.u32);
	vOut.set( hkReal(qu[0]), hkReal(qu[1]), hkReal(qu[2]), hkReal(qu[3]));
}

HK_FORCE_INLINE hkUint32 __f32_u32(hkFloat32 a)
{
	union F32U32
	{
		hkFloat32 m_asF32;
		hkUint32 m_asU32;
	};

	F32U32 value; value.m_asF32 = a;
	const hkUint32 biasedExponent = (value.m_asU32 & 0x7F800000) >> 23; 
	const int exponent = (hkInt32)biasedExponent - 127;

	// Clip negative and fractional numbers
	if (exponent < 0 || (value.m_asU32 & 0x80000000)) 
	{
		return 0;
	}

	// Obtain mantissa and shift it according to the exponent
	hkUint32 mantissa = (value.m_asU32 & 0x007FFFFF) | 0x00800000;	
	if (exponent <= 23)
	{
		mantissa >>= (23 - exponent);
	}
	else
	{		
		if (exponent < 32)
		{
			mantissa <<= (exponent - 23);
		}

		// Saturate values over 2^32 - 1
		else
		{
			mantissa = 0xFFFFFFFF;
		}
	}

	return mantissa;
}

HK_FORCE_INLINE void hkIntVector::setConvertF32toU32(hkVector4Parameter vIn)
{
	hkUint32* HK_RESTRICT qu = m_quad.u32;
	qu[0] = __f32_u32( hkFloat32(vIn(0)) );
	qu[1] = __f32_u32( hkFloat32(vIn(1)) );
	qu[2] = __f32_u32( hkFloat32(vIn(2)) );
	qu[3] = __f32_u32( hkFloat32(vIn(3)) );
}

HK_FORCE_INLINE void hkIntVector::setConvertF32toS32(hkVector4Parameter vIn)
{
	hkInt32* HK_RESTRICT qu = reinterpret_cast<hkInt32*>(m_quad.u32);
	qu[0] = hkMath::hkToIntFast( vIn(0) );
	qu[1] = hkMath::hkToIntFast( vIn(1) );
	qu[2] = hkMath::hkToIntFast( vIn(2) );
	qu[3] = hkMath::hkToIntFast( vIn(3) );
}

template <int N>
HK_FORCE_INLINE void hkIntVector::loadNotAligned(const hkUint32* p)
{
	HK_INT_VECTOR_UNSUPPORTED_LENGTH_CHECK;
#if defined(HK_PLATFORM_CAFE)
	hkUint32* HK_RESTRICT d = reinterpret_cast<hkUint32*>(m_quad.u32);
	const hkUint32* HK_RESTRICT s = p;
	switch(N)
	{
	case 4:  d[3] = s[3];
	case 3:  d[2] = s[2];
	case 2:  d[1] = s[1];
	default: d[0] = s[0]; break;
	}
#else
	hkMemUtil::memCpy<sizeof(hkUint32)>(&m_quad, p, N*sizeof(hkUint32));
#endif
	HK_ON_DEBUG( { for(int i=N; i<4; ++i) m_quad.u32[i] = 0xffffffff; } );
}

template <int N>
HK_FORCE_INLINE void hkIntVector::storeNotAligned(hkUint32* p) const
{
	HK_INT_VECTOR_UNSUPPORTED_LENGTH_CHECK;
#if defined(HK_PLATFORM_CAFE)
	const hkUint32* HK_RESTRICT src = m_quad.u32;
	hkUint32* HK_RESTRICT d = p;
	switch(N)
	{
	case 4: d[3] = src[3];
	case 3: d[2] = src[2];
	case 2: d[1] = src[1];
	default: d[0] = src[0]; break;
	}
#else
	hkMemUtil::memCpy<sizeof(hkUint32)>(p, &m_quad, N*sizeof(hkUint32));
#endif
}

template <>
HK_FORCE_INLINE void hkIntVector::load<4>(const hkUint32* p)
{
#if !defined(HK_PLATFORM_CTR) && !defined(HK_PLATFORM_ANDROID)  && !(defined(HK_PLATFORM_IOS) && (HK_CONFIG_SIMD != HK_CONFIG_SIMD_ENABLED)) && !defined(HK_ALIGN_RELAX_CHECKS)
	HK_ASSERT2(0x70aae483, ((hkUlong)p & 0xf) == 0, "pointer for hkIntVector::load<4> must be 16-byte aligned");
#endif
	hkMemUtil::memCpyOneAligned<4*sizeof(hkUint32), 16>(&m_quad, p);
}

template <int N>
HK_FORCE_INLINE void hkIntVector::load(const hkUint32* p)
{
#if !defined(HK_PLATFORM_CTR) && !(defined(HK_PLATFORM_IOS) && HK_CONFIG_SIMD != HK_CONFIG_SIMD_ENABLED) && !defined(HK_ALIGN_RELAX_CHECKS)
	HK_ASSERT2(0x70aae483, ( ((hkUlong)p) & ((sizeof(hkUint32)*(N!=3?N:4))-1) ) == 0, "p must be aligned.");
#endif
	loadNotAligned<N>(p);
}

//
//	Store to an aligned address

template <>
HK_FORCE_INLINE void hkIntVector::store<4>(hkUint32* p) const
{
#if !defined(HK_PLATFORM_CTR) && !(defined(HK_PLATFORM_IOS) && (HK_CONFIG_SIMD != HK_CONFIG_SIMD_ENABLED)) && !defined(HK_ALIGN_RELAX_CHECKS)
	HK_ASSERT2(0x70aae483, ((hkUlong)p & 0xf) == 0, "pointer for hkIntVector::store<4> must be 16-byte aligned");
#endif
	hkMemUtil::memCpyOneAligned<4*sizeof(hkUint32), 16>(p, &m_quad);
}

template <int N>
HK_FORCE_INLINE void hkIntVector::store(hkUint32* p) const
{
#if !defined(HK_PLATFORM_CTR) && !(defined(HK_PLATFORM_IOS) && HK_CONFIG_SIMD != HK_CONFIG_SIMD_ENABLED) && !defined(HK_ALIGN_RELAX_CHECKS)
	HK_ASSERT2(0x70aae483, ( ((hkUlong)p) & ((sizeof(hkUint32)*(N!=3?N:4))-1) ) == 0, "p must be aligned.");
#endif
	storeNotAligned<N>(p);
}


//
//	Assuming that this = (i0, i1, i2, i3) and v = (v0, v1, v2, v3), the function will set this = (ik, ik, ik, ik)
//	where k ={0,..., 3} such that vk = max{v0, v1, v2, v3}.

HK_FORCE_INLINE void hkIntVector::broadcastComponentAtVectorMax(hkVector4Parameter v)
{
	setAll(getComponentAtVectorMax(v));
}

/// Assuming that this = (i0, i1, i2, i3) and v = (v0, v1, v2, v3), the function will return ik
/// where k in {0,..., 3} such that vk = max{v0, v1, v2, v3}.

HK_FORCE_INLINE int hkIntVector::getComponentAtVectorMax(hkVector4Parameter v) const
{
	const hkInt32* HK_RESTRICT qu = reinterpret_cast<const hkInt32*>(m_quad.u32);

	const int vmax = v.getIndexOfMaxComponent<4>();

	return qu[vmax];
}

HK_FORCE_INLINE void hkIntVector::setSelect(hkVector4ComparisonParameter selectMask, hkIntVectorParameter trueValue, hkIntVectorParameter falseValue)
{
	const hkUint32* HK_RESTRICT qt = trueValue.m_quad.u32;
	const hkUint32* HK_RESTRICT qf = falseValue.m_quad.u32;
	hkUint32* HK_RESTRICT qu = m_quad.u32;

	qu[0] = selectMask.anyIsSet(hkVector4Comparison::MASK_X) ? qt[0] : qf[0];
	qu[1] = selectMask.anyIsSet(hkVector4Comparison::MASK_Y) ? qt[1] : qf[1];
	qu[2] = selectMask.anyIsSet(hkVector4Comparison::MASK_Z) ? qt[2] : qf[2];
	qu[3] = selectMask.anyIsSet(hkVector4Comparison::MASK_W) ? qt[3] : qf[3];
}

template <hkIntVectorConstant vectorConstant>
HK_FORCE_INLINE /*static*/ const hkIntVector& HK_CALL hkIntVector::getConstant()
{
	return *(const hkIntVector*) (g_intVectorConstants + vectorConstant);
}

HK_FORCE_INLINE /*static*/ const hkIntVector& HK_CALL hkIntVector::getConstant(hkIntVectorConstant constant)
{
	return *(const hkIntVector*) (g_intVectorConstants + constant);
}

// result.u32[i] = highshorts.u16[i]<<16 + lowShorts.u16[i]. highShorts.u16/lowShorts.u16[4..7] are ignored
HK_FORCE_INLINE void hkIntVector::setCombineHead16To32( hkIntVectorParameter highShorts, hkIntVectorParameter lowShorts )
{
#if HK_ENDIAN_BIG
	setMergeHead16(highShorts, lowShorts);
#else
	setMergeHead16(lowShorts, highShorts);
#endif
}

// result.u32[i] = highshorts.u16[4+i]<<16 + lowShorts.u16[4+i]. highShorts.u16/lowShorts.u16[0..3] are ignored
HK_FORCE_INLINE void hkIntVector::setCombineTail16To32( hkIntVectorParameter highShorts, hkIntVectorParameter lowShorts )
{
#if HK_ENDIAN_BIG
	setMergeTail16(highShorts, lowShorts);
#else
	setMergeTail16(lowShorts, highShorts);
#endif
}

// result.u16[i] = highshorts.u8[i]<<16 + lowShorts.u8[i]. highShorts.u8/lowShorts.u8[8..15] are ignored
HK_FORCE_INLINE void hkIntVector::setCombineHead8To16( hkIntVectorParameter highShorts, hkIntVectorParameter lowShorts )
{
#if HK_ENDIAN_BIG
	setMergeHead8(highShorts, lowShorts);
#else
	setMergeHead8(lowShorts, highShorts);
#endif
}

// result.u16[i] = highshorts.u8[8+i]<<16 + lowShorts.u8[8+i]. highShorts.u8/lowShorts.u8[0..7] are ignored
HK_FORCE_INLINE void hkIntVector::setCombineTail8To16( hkIntVectorParameter highShorts, hkIntVectorParameter lowShorts )
{
#if HK_ENDIAN_BIG
	setMergeTail8(highShorts, lowShorts);
#else
	setMergeTail8(lowShorts, highShorts);
#endif
}

HK_FORCE_INLINE void hkIntVector::setSplit8To32( hkIntVectorParameter bytes )
{
	hkIntVector zero;zero.setZero();
	hkIntVector i16; i16.setCombineHead8To16( zero, bytes );
	setCombineHead16To32(zero, i16);
}

#endif // defined(HK_USING_GENERIC_INT_VECTOR_IMPLEMENTATION)

/////////////////////////////
// Variable shifts (shift all vector members by different amounts)
#if !defined(HK_INT_VECTOR_NATIVE_VARIABLESHIFT)

HK_FORCE_INLINE void hkIntVector::setShiftLeft32( hkIntVectorParameter a, hkIntVectorParameter shift )
{
	hkQuadUintUnion qu, qa, qshift;
	qa.q = a.m_quad;
	qshift.q = shift.m_quad;

	qu.u[0] = qa.u[0] << qshift.u[0];
	qu.u[1] = qa.u[1] << qshift.u[1];
	qu.u[2] = qa.u[2] << qshift.u[2];
	qu.u[3] = qa.u[3] << qshift.u[3];

	m_quad = qu.q;
}

HK_FORCE_INLINE void hkIntVector::setShiftRight32( hkIntVectorParameter a, hkIntVectorParameter shift )
{
	hkQuadUintUnion qu, qa, qshift;
	qa.q = a.m_quad;
	qshift.q = shift.m_quad;

	qu.u[0] = qa.u[0] >> qshift.u[0];
	qu.u[1] = qa.u[1] >> qshift.u[1];
	qu.u[2] = qa.u[2] >> qshift.u[2];
	qu.u[3] = qa.u[3] >> qshift.u[3];

	m_quad = qu.q;
}

HK_FORCE_INLINE void hkIntVector::setShiftLeft16( hkIntVectorParameter a, hkIntVectorParameter shift )
{
	hkQuadUshortUnion qu, qa, qshift;
	qa.q = a.m_quad;
	qshift.q = shift.m_quad;

	qu.u[0] = qa.u[0] << qshift.u[0];
	qu.u[1] = qa.u[1] << qshift.u[1];
	qu.u[2] = qa.u[2] << qshift.u[2];
	qu.u[3] = qa.u[3] << qshift.u[3];
	qu.u[4] = qa.u[4] << qshift.u[4];
	qu.u[5] = qa.u[5] << qshift.u[5];
	qu.u[6] = qa.u[6] << qshift.u[6];
	qu.u[7] = qa.u[7] << qshift.u[7];

	m_quad = qu.q;
}

HK_FORCE_INLINE void hkIntVector::setShiftRightS16( hkIntVectorParameter a, hkIntVectorParameter shift )
{
	hkQuadShortUnion qu, qa, qshift;
	qa.q = a.m_quad;
	qshift.q = shift.m_quad;

	qu.u[0] = qa.u[0] >> qshift.u[0];
	qu.u[1] = qa.u[1] >> qshift.u[1];
	qu.u[2] = qa.u[2] >> qshift.u[2];
	qu.u[3] = qa.u[3] >> qshift.u[3];
	qu.u[4] = qa.u[4] >> qshift.u[4];
	qu.u[5] = qa.u[5] >> qshift.u[5];
	qu.u[6] = qa.u[6] >> qshift.u[6];
	qu.u[7] = qa.u[7] >> qshift.u[7];

	m_quad = qu.q;
}

#endif // !defined(HK_INT_VECTOR_NATIVE_VARIABLESHIFT)

/////////////////////////////
// 128-bit bitwise shifts by a variable
#if !defined(HK_INT_VECTOR_NATIVE_BITSHIFT128)

HK_FORCE_INLINE void hkIntVector::setBitShiftLeft128(hkIntVectorParameter a, hkIntVectorParameter shiftSplat)
{
	int shift = shiftSplat.getU8<0>();
	int bitcount = shift % 8;
	int bytecount = shift / 8;
	hkQuadUcharUnion q;
	q.q = a.m_quad;
	int i;
	for (i = 0; i < 16 - (bytecount + 1); i++)
	{
		q.u[i] = (hkUint8)((q.u[i+bytecount] << bitcount) | (q.u[i+bytecount+1] >> (8 - bitcount)));
	}
	q.u[i++] = (hkUint8)(q.u[15] << bitcount);
	for (; i < 16; i++)
	{
		q.u[i] = 0;
	}
	m_quad = q.q;
}

HK_FORCE_INLINE void hkIntVector::setBitShiftRight128(hkIntVectorParameter a, hkIntVectorParameter shiftSplat)
{
	int shift = shiftSplat.getU8<0>();
	int bitcount = shift % 8;
	int bytecount = shift / 8;
	hkQuadUcharUnion q;
	q.q = a.m_quad;
	int i;
	for (i = 15; i >= bytecount + 1; i--)
	{
		q.u[i] = (hkUint8)((q.u[i-bytecount] >> bitcount) | (q.u[i-bytecount-1] << (8 - bitcount)));
	}
	q.u[i--] = (hkUint8)(q.u[0] >> bitcount);
	for (; i >= 0; i--)
	{
		q.u[i] = 0;
	}
	m_quad = q.q;
}

#endif // !defined(HK_INT_VECTOR_NATIVE_BITSHIFT128)

///////////////////////////////////////////////////////////
// everything below this is common code for all platforms
///////////////////////////////////////////////////////////

HK_FORCE_INLINE void hkIntVector::setSignExtendS16ToS32( hkIntVectorParameter a )
{
	setShiftLeft32<16>(*this);
	setShiftRightS32<16>(*this);
}

HK_FORCE_INLINE void hkIntVector::setCombine16To32( hkIntVectorParameter a )
{
	if (HK_ENDIAN_LITTLE)
	{
		// we need to swap shorts on little-endian machines
		hkIntVector zeros; zeros.setZero();
		hkIntVector ones; ones.setNot(zeros);

		hkIntVector leftmask; leftmask.setMergeHead16(ones, zeros);

		hkIntVector left; left.setAnd(a, leftmask);
		hkIntVector right; right.setAndNot(a, leftmask);
		left.setShiftLeft32<16>(left);
		right.setShiftRight32<16>(right);
		setOr(left, right);
	}
	else
	{
		m_quad = a.m_quad;
	}
}

HK_FORCE_INLINE void hkIntVector::setCombine8To16( hkIntVectorParameter a )
{
	if (HK_ENDIAN_LITTLE)
	{
		// we need to swap bytes on little-endian machines
		hkIntVector zeros; zeros.setZero();
		hkIntVector ones; ones.setNot(zeros);

		hkIntVector leftmask; leftmask.setMergeHead8(ones, zeros);

		hkIntVector left; left.setAnd(a, leftmask);
		hkIntVector right; right.setAndNot(a, leftmask);
		left.setShiftLeft16<8>(left);
		right.setShiftRight16<8>(right);
		setOr(left, right);
	}
	else
	{
		m_quad = a.m_quad;
	}
}

HK_FORCE_INLINE void hkIntVector::setConvertHeadU16ToU32( hkIntVectorParameter a )
{
	hkIntVector zero; zero.setZero();
	setCombineHead16To32(zero, a);
}

HK_FORCE_INLINE void hkIntVector::setConvertHeadS16ToS32( hkIntVectorParameter a )
{
	setConvertHeadU16ToU32(a);
	setSignExtendS16ToS32(*this);
}

HK_FORCE_INLINE void hkIntVector::setConvertTailU16ToU32( hkIntVectorParameter a )
{
	hkIntVector zero; zero.setZero();
	setCombineTail16To32(zero, a);
}

HK_FORCE_INLINE void hkIntVector::setConvertTailS16ToS32( hkIntVectorParameter a )
{
	setConvertTailU16ToU32(a);
	setSignExtendS16ToS32(*this);
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
