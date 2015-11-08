/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/*static*/ HK_FORCE_INLINE hkSimdReal HK_CALL hkSimdReal::fromFloat(const hkReal& x)
{ 
	hkSimdReal r; 
	r.setFromFloat(x); 
	return r; 
}

/*static*/ HK_FORCE_INLINE hkSimdReal HK_CALL hkSimdReal::fromHalf(const hkHalf& x)
{ 
	hkSimdReal r; 
	r.setFromHalf(x); 
	return r; 
}

/*static*/ HK_FORCE_INLINE hkSimdReal HK_CALL hkSimdReal::fromUint16(const hkUint16& x)
{ 
	hkSimdReal r; 
	r.setFromUint16(x); 
	return r; 
}

/*static*/ HK_FORCE_INLINE hkSimdReal HK_CALL hkSimdReal::fromUint8(const hkUint8& x)
{ 
	hkSimdReal r; 
	r.setFromUint8(x); 
	return r; 
}

/*static*/ HK_FORCE_INLINE hkSimdReal HK_CALL hkSimdReal::fromInt32(const hkInt32& x)
{ 
	hkSimdReal r; 
	r.setFromInt32(x); 
	return r; 
}

HK_FORCE_INLINE void hkSimdReal::operator= (hkSimdRealParameter v)
{
	m_real = v.m_real;
}

HK_FORCE_INLINE void hkSimdReal::setAdd(hkSimdRealParameter a, hkSimdRealParameter b)
{
	*this = a + b;
}
HK_FORCE_INLINE void hkSimdReal::setSub(hkSimdRealParameter a, hkSimdRealParameter b)
{
	*this = a - b;
}
HK_FORCE_INLINE void hkSimdReal::setMul(hkSimdRealParameter a, hkSimdRealParameter b)
{
	*this = a * b;
}

HK_FORCE_INLINE void hkSimdReal::setAddMul(hkSimdRealParameter a, hkSimdRealParameter b, hkSimdRealParameter c)
{
	*this = a + (b * c);
}
HK_FORCE_INLINE void hkSimdReal::setSubMul(hkSimdRealParameter a, hkSimdRealParameter b, hkSimdRealParameter c)
{
	*this = a - (b * c);
}

HK_FORCE_INLINE void hkSimdReal::add(hkSimdRealParameter a)
{
	setAdd( *this, a );
}
HK_FORCE_INLINE void hkSimdReal::sub(hkSimdRealParameter a)
{
	setSub( *this, a );
}
HK_FORCE_INLINE void hkSimdReal::mul(hkSimdRealParameter a)
{
	setMul( *this, a );
}

HK_FORCE_INLINE void hkSimdReal::addMul(hkSimdRealParameter a, hkSimdRealParameter b)
{
	setAddMul( *this, a, b );
}
HK_FORCE_INLINE void hkSimdReal::subMul(hkSimdRealParameter a, hkSimdRealParameter b)
{
	setSubMul( *this, a, b );
}

HK_FORCE_INLINE void hkSimdReal::setInterpolate( hkSimdRealParameter a, hkSimdRealParameter b, hkSimdRealParameter t )
{
	const hkSimdReal bminusA = b-a;
	setAddMul( a, t, bminusA );
}

HK_FORCE_INLINE bool hkSimdReal::approxEqual(hkSimdRealParameter v, hkSimdRealParameter epsilon) const
{
	hkSimdReal t; t.setAbs(*this - v);
	return t.isLess(epsilon);
}

HK_FORCE_INLINE void hkSimdReal::setClamped( hkSimdRealParameter a, hkSimdRealParameter minVal, hkSimdRealParameter maxVal )
{
	// This ensures that if a is NAN, clamped will be maxVal afterwards	
	const hkVector4Comparison maxValGtA = maxVal.greater(a);
	hkSimdReal clamped; clamped.setSelect(maxValGtA, a, maxVal);
	setMax(minVal, clamped);
}

#ifndef HK_DISABLE_OLD_VECTOR4_INTERFACE

//
// old interface
//
HK_FORCE_INLINE hkVector4Comparison hkSimdReal::isNegative() const { return signBitSet(); }
HK_FORCE_INLINE hkVector4Comparison hkSimdReal::isPositive() const { return signBitClear(); }
HK_FORCE_INLINE hkBool32 hkSimdReal::isZero() const { return isEqualZero(); }
HK_FORCE_INLINE hkVector4Comparison hkSimdReal::compareLessThanZero() const { return lessZero(); }
HK_FORCE_INLINE hkVector4Comparison hkSimdReal::compareGreaterThanZero() const { return greaterZero(); }
HK_FORCE_INLINE hkVector4Comparison hkSimdReal::compareLessThan(hkSimdRealParameter a) const { return less(a); }
HK_FORCE_INLINE hkVector4Comparison hkSimdReal::compareLessThanEqual(hkSimdRealParameter a) const { return lessEqual(a); }
HK_FORCE_INLINE hkVector4Comparison hkSimdReal::compareGreaterThan(hkSimdRealParameter a) const { return greater(a); }
HK_FORCE_INLINE hkVector4Comparison hkSimdReal::compareGreaterThanEqual(hkSimdRealParameter a) const { return greaterEqual(a); }

#endif

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
