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

#if HK_SSE_VERSION >= 0x41
#define HK_VECTOR4_COMBINE_XYZ_W(xyz, w) _mm_blend_ps( xyz, w, 0x8)
#else
#define HK_VECTOR4_COMBINE_XYZ_W(xyz, w) _mm_shuffle_ps( xyz, _mm_unpackhi_ps(xyz, w), _MM_SHUFFLE(3,0,1,0))
#endif

/* quad, here for inlining */

#ifndef HK_DISABLE_MATH_CONSTRUCTORS
/* construct, assign, zero */
HK_FORCE_INLINE hkVector4::hkVector4(hkReal a, hkReal b, hkReal c, hkReal d)
{
	m_quad = _mm_setr_ps(a,b,c,d);
}

HK_FORCE_INLINE hkVector4::hkVector4(const hkQuadReal& q)
{
	m_quad = q;
}

HK_FORCE_INLINE hkVector4::hkVector4(const hkVector4& v)
{
	m_quad = v.m_quad;
}
#endif

HK_FORCE_INLINE void hkVector4::set(hkReal a, hkReal b, hkReal c, hkReal d)
{
	m_quad = _mm_setr_ps(a,b,c,d);
}

HK_FORCE_INLINE void hkVector4::set( hkSimdRealParameter a, hkSimdRealParameter b, hkSimdRealParameter c, hkSimdRealParameter d )
{
	const hkQuadReal ab = _mm_unpacklo_ps(a.m_real, b.m_real);
	const hkQuadReal cd = _mm_unpacklo_ps(c.m_real, d.m_real);
	m_quad = _mm_movelh_ps(ab,cd);
}

HK_FORCE_INLINE void hkVector4::setAll(const hkReal& a)
{
	m_quad = _mm_set1_ps(a);
}

HK_FORCE_INLINE void hkVector4::setAll(hkSimdRealParameter a)
{
	m_quad = a.m_real;
}

HK_FORCE_INLINE void hkVector4::setZero()
{
	m_quad = _mm_setzero_ps();
}

template <int I> 
HK_FORCE_INLINE void hkVector4::zeroComponent()
{
	HK_VECTOR4_SUBINDEX_CHECK;
#if HK_SSE_VERSION >= 0x41
	m_quad = _mm_blend_ps(m_quad, _mm_setzero_ps(), 1 << I);
#else
	static HK_ALIGN16( const hkUint32 cx[4][4] ) ={ 
		{ 0x00000000, 0xffffffff, 0xffffffff, 0xffffffff },
		{ 0xffffffff, 0x00000000, 0xffffffff, 0xffffffff },
		{ 0xffffffff, 0xffffffff, 0x00000000, 0xffffffff },
		{ 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000 } };

	m_quad = _mm_and_ps( m_quad, reinterpret_cast<const hkQuadReal*>(cx)[I] );
#endif
}

HK_FORCE_INLINE void hkVector4::zeroComponent(const int i)
{
	HK_ASSERT2(0x3bc36625, (i>=0) && (i<4), "Component index out of range");
	static HK_ALIGN16( const hkUint32 cx[4][4] ) ={ 
		{ 0x00000000, 0xffffffff, 0xffffffff, 0xffffffff },
		{ 0xffffffff, 0x00000000, 0xffffffff, 0xffffffff },
		{ 0xffffffff, 0xffffffff, 0x00000000, 0xffffffff },
		{ 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000 } };

	m_quad = _mm_and_ps( m_quad, reinterpret_cast<const hkQuadReal*>(cx)[i] );
}

HK_FORCE_INLINE void hkVector4::setAdd(hkVector4Parameter v0, hkVector4Parameter v1)
{
	m_quad = _mm_add_ps(v0.m_quad, v1.m_quad);
}

HK_FORCE_INLINE void hkVector4::setSub(hkVector4Parameter v0, hkVector4Parameter v1)
{
	m_quad = _mm_sub_ps(v0.m_quad, v1.m_quad);
}

HK_FORCE_INLINE void hkVector4::setMul(hkVector4Parameter v0, hkVector4Parameter v1)
{
	m_quad = _mm_mul_ps(v0.m_quad, v1.m_quad);
}

HK_FORCE_INLINE void hkVector4::setMul(hkVector4Parameter v1, hkSimdRealParameter r)
{
	m_quad = _mm_mul_ps( r.m_real, v1.m_quad);
}

HK_FORCE_INLINE void hkVector4::setSubMul(hkVector4Parameter a, hkVector4Parameter b, hkSimdRealParameter r)
{
	m_quad = _mm_sub_ps( a.m_quad, _mm_mul_ps( r.m_real, b.m_quad) );
}

HK_FORCE_INLINE void hkVector4::setAddMul(hkVector4Parameter a, hkVector4Parameter b, hkSimdRealParameter r)
{
	m_quad = _mm_add_ps( a.m_quad, _mm_mul_ps( r.m_real, b.m_quad) );
}

HK_FORCE_INLINE void hkVector4::setAddMul(hkVector4Parameter a, hkVector4Parameter x, hkVector4Parameter y)
{
	m_quad = _mm_add_ps( a.m_quad, _mm_mul_ps( x.m_quad, y.m_quad) );
}

HK_FORCE_INLINE void hkVector4::setSubMul(hkVector4Parameter a, hkVector4Parameter x, hkVector4Parameter y)
{
	m_quad = _mm_sub_ps( a.m_quad, _mm_mul_ps( x.m_quad, y.m_quad) );
}

HK_FORCE_INLINE void hkVector4::setCross( hkVector4Parameter v0, hkVector4Parameter v1 )
{
	const hkQuadReal cross0 = _mm_mul_ps(
		_mm_shuffle_ps(v0.m_quad, v0.m_quad, _MM_SHUFFLE(3,0,2,1)),
		_mm_shuffle_ps(v1.m_quad, v1.m_quad, _MM_SHUFFLE(3,1,0,2)) );
	const hkQuadReal cross1 = _mm_mul_ps(
		_mm_shuffle_ps(v0.m_quad, v0.m_quad, _MM_SHUFFLE(3,1,0,2)),
		_mm_shuffle_ps(v1.m_quad, v1.m_quad, _MM_SHUFFLE(3,0,2,1)) );

	m_quad = _mm_sub_ps(cross0, cross1);
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::equal(hkVector4Parameter a) const
{
	return hkVector4Comparison::convert(_mm_cmpeq_ps(m_quad, a.m_quad));
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::notEqual(hkVector4Parameter a) const
{
	return hkVector4Comparison::convert(_mm_cmpneq_ps(m_quad, a.m_quad));
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::less(hkVector4Parameter a) const
{
	return hkVector4Comparison::convert(_mm_cmplt_ps(m_quad, a.m_quad));
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::lessEqual(hkVector4Parameter a) const
{
	return hkVector4Comparison::convert(_mm_cmple_ps(m_quad, a.m_quad));
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::greater(hkVector4Parameter a) const
{
	return hkVector4Comparison::convert(_mm_cmpgt_ps(m_quad, a.m_quad));
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::greaterEqual(hkVector4Parameter a) const
{
	return hkVector4Comparison::convert(_mm_cmpge_ps(m_quad, a.m_quad));
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::lessZero() const
{
	return hkVector4Comparison::convert(_mm_cmplt_ps(m_quad, _mm_setzero_ps()));
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::lessEqualZero() const
{
	return hkVector4Comparison::convert(_mm_cmple_ps(m_quad, _mm_setzero_ps()));
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::greaterZero() const
{
	return hkVector4Comparison::convert(_mm_cmpgt_ps(m_quad, _mm_setzero_ps()));
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::greaterEqualZero() const
{
	return hkVector4Comparison::convert(_mm_cmpge_ps(m_quad, _mm_setzero_ps()));
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::equalZero() const
{
	return hkVector4Comparison::convert(_mm_cmpeq_ps(m_quad, _mm_setzero_ps()));
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::notEqualZero() const
{
	return hkVector4Comparison::convert(_mm_cmpneq_ps(m_quad, _mm_setzero_ps()));
}

HK_FORCE_INLINE void hkVector4::setSelect( hkVector4ComparisonParameter comp, hkVector4Parameter trueValue, hkVector4Parameter falseValue )
{
#if HK_SSE_VERSION >= 0x41
	m_quad = _mm_blendv_ps(falseValue.m_quad, trueValue.m_quad, comp.m_mask);
#else
	m_quad = _mm_or_ps( _mm_and_ps(comp.m_mask, trueValue.m_quad), _mm_andnot_ps(comp.m_mask, falseValue.m_quad) );
#endif
}

template <>
HK_FORCE_INLINE void hkVector4::setNeg<2>(hkVector4Parameter v)
{
	static HK_ALIGN16( const hkUint32 negateMask[4] ) = { 0x80000000, 0x80000000, 0x00000000, 0x00000000 }; // todo constant
	m_quad = _mm_xor_ps(v.m_quad, *(const hkQuadReal*)&negateMask);
}

template <>
HK_FORCE_INLINE void hkVector4::setNeg<3>(hkVector4Parameter v)
{
	static HK_ALIGN16( const hkUint32 negateMask[4] ) = { 0x80000000, 0x80000000, 0x80000000, 0x00000000 }; // todo constant
	m_quad = _mm_xor_ps(v.m_quad, *(const hkQuadReal*)&negateMask);
}

template <>
HK_FORCE_INLINE void hkVector4::setNeg<4>(hkVector4Parameter v)
{
	static HK_ALIGN16( const hkUint32 negateMask[4] ) = { 0x80000000, 0x80000000, 0x80000000, 0x80000000 }; // todo constant
	m_quad = _mm_xor_ps(v.m_quad, *(const hkQuadReal*)&negateMask);
}

template <int N>
HK_FORCE_INLINE void hkVector4::setNeg(hkVector4Parameter v)
{
	HK_VECTOR4_NOT_IMPLEMENTED;
}

HK_FORCE_INLINE void hkVector4::setAbs(hkVector4Parameter v)
{
	m_quad = hkMath::quadFabs(v.m_quad); 
}

HK_FORCE_INLINE void hkVector4::setMin(hkVector4Parameter a, hkVector4Parameter b)
{
	m_quad = _mm_min_ps(a.m_quad, b.m_quad);
}

HK_FORCE_INLINE void hkVector4::setMax(hkVector4Parameter a, hkVector4Parameter b)
{
	m_quad = _mm_max_ps(a.m_quad, b.m_quad);
}

/* matrix3, rotation, quaternion, transform */

HK_FORCE_INLINE void hkVector4::_setRotatedDir(const hkMatrix3& r, hkVector4Parameter b )
{
	const hkQuadReal c0 = r.getColumn<0>().m_quad;
	const hkQuadReal c1 = r.getColumn<1>().m_quad;
	const hkQuadReal c2 = r.getColumn<2>().m_quad;

	const hkQuadReal b0 = _mm_shuffle_ps( b.m_quad, b.m_quad, _MM_SHUFFLE(0,0,0,0));
	const hkQuadReal b1 = _mm_shuffle_ps( b.m_quad, b.m_quad, _MM_SHUFFLE(1,1,1,1));
	const hkQuadReal b2 = _mm_shuffle_ps( b.m_quad, b.m_quad, _MM_SHUFFLE(2,2,2,2));	

	const hkQuadReal r0 = _mm_mul_ps( c0, b0 );
	const hkQuadReal r1 = _mm_mul_ps( c1, b1 );
	const hkQuadReal r2 = _mm_mul_ps( c2, b2 );

	m_quad = _mm_add_ps( _mm_add_ps(r0, r1), r2 );
}

HK_FORCE_INLINE void hkVector4::_setRotatedInverseDir(const hkMatrix3& r, hkVector4Parameter b )
{
#if HK_SSE_VERSION >= 0x41
	const hkQuadReal c0 = r.getColumn<0>().m_quad;
	const hkQuadReal c1 = r.getColumn<1>().m_quad;
	const hkQuadReal c2 = r.getColumn<2>().m_quad;

	const hkQuadReal r0 = _mm_dp_ps( c0, b.m_quad, 0x71 );
	const hkQuadReal r1 = _mm_dp_ps( c1, b.m_quad, 0x72 );
	const hkQuadReal r2 = _mm_dp_ps( c2, b.m_quad, 0x74 );

	m_quad = _mm_or_ps( _mm_or_ps(r0, r1), r2 );
#else
	hkVector4 c0 = r.getColumn<0>();
	hkVector4 c1 = r.getColumn<1>();
	hkVector4 c2 = r.getColumn<2>();

	HK_TRANSPOSE3(c0,c1,c2);

	const hkQuadReal b0 = _mm_shuffle_ps( b.m_quad, b.m_quad, _MM_SHUFFLE(0,0,0,0));
	const hkQuadReal b1 = _mm_shuffle_ps( b.m_quad, b.m_quad, _MM_SHUFFLE(1,1,1,1));
	const hkQuadReal b2 = _mm_shuffle_ps( b.m_quad, b.m_quad, _MM_SHUFFLE(2,2,2,2));	

	const hkQuadReal r0 = _mm_mul_ps( c0.m_quad, b0 );
	const hkQuadReal r1 = _mm_mul_ps( c1.m_quad, b1 );
	const hkQuadReal r2 = _mm_mul_ps( c2.m_quad, b2 );

	m_quad = _mm_add_ps( _mm_add_ps(r0, r1), r2 );
#endif
}


template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::dot<2>(hkVector4Parameter a) const
{
#if HK_SSE_VERSION >= 0x41
	return hkSimdReal::convert(_mm_dp_ps(m_quad, a.m_quad, 0x3F));
#elif HK_SSE_VERSION >= 0x30
	const hkQuadReal x2 = _mm_mul_ps(m_quad,a.m_quad);
	const hkQuadReal hsum = _mm_hadd_ps(x2,x2); // xy zw xy zw
	return hkSimdReal::convert(_mm_shuffle_ps(hsum,hsum,_MM_SHUFFLE(0,0,0,0)));
#else
	const hkQuadReal x2 = _mm_mul_ps(m_quad,a.m_quad);
	const hkQuadReal result = _mm_add_ps( _mm_shuffle_ps(x2,x2,_MM_SHUFFLE(1,1,1,1)), _mm_shuffle_ps(x2,x2,_MM_SHUFFLE(0,0,0,0))); // xy xy xy xy
	return hkSimdReal::convert(result);
#endif
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::dot<3>(hkVector4Parameter a) const
{
#if HK_SSE_VERSION >= 0x41
	return hkSimdReal::convert(_mm_dp_ps(m_quad, a.m_quad, 0x7F));
#elif HK_SSE_VERSION >= 0x30
	const hkQuadReal x2 = _mm_mul_ps(m_quad,a.m_quad);
	const hkQuadReal hsum = _mm_hadd_ps(x2,x2); // xy zw xy zw
	const hkQuadReal z = _mm_shuffle_ps(x2,x2,_MM_SHUFFLE(2,2,2,2)); // zzzz
	const hkQuadReal hsumz = _mm_add_ps(hsum, z); // xyz zzw xyz zzw
	return hkSimdReal::convert(_mm_shuffle_ps(hsumz,hsumz,_MM_SHUFFLE(0,0,0,0)));
#else
	const hkQuadReal x2 = _mm_mul_ps(m_quad,a.m_quad);
	const hkQuadReal xySum = _mm_add_ps( _mm_shuffle_ps(x2,x2,_MM_SHUFFLE(1,1,1,1)), _mm_shuffle_ps(x2,x2,_MM_SHUFFLE(0,0,0,0))); // xy xy xy xy
	const hkQuadReal z = _mm_shuffle_ps(x2,x2,_MM_SHUFFLE(2,2,2,2)); // zzzz
	const hkQuadReal result = _mm_add_ps( z, xySum); // xyz xyz xyz xyz
	return hkSimdReal::convert(result);
#endif
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::dot<4>(hkVector4Parameter a) const
{
#if HK_SSE_VERSION >= 0x41
	return hkSimdReal::convert(_mm_dp_ps(m_quad, a.m_quad, 0xFF));
#elif HK_SSE_VERSION >= 0x30
	const hkQuadReal x2 = _mm_mul_ps(m_quad,a.m_quad);
	const hkQuadReal hsum = _mm_hadd_ps(x2,x2); // xy zw xy zw
	return hkSimdReal::convert(_mm_hadd_ps(hsum,hsum)); // xyzw all 4
#else
	const hkQuadReal x2 = _mm_mul_ps(m_quad,a.m_quad);
	const hkQuadReal sum0 = _mm_add_ps( _mm_shuffle_ps(x2,x2,_MM_SHUFFLE(1,0,3,2)), x2); // yxwz+xyzw = xy xy zw zw
	const hkQuadReal sum1 = _mm_shuffle_ps(sum0,sum0, _MM_SHUFFLE(2,3,0,1)); // = zw zw xy xy
	const hkQuadReal result = _mm_add_ps( sum0, sum1 ); // = xyzw xyzw xyzw xyzw
	return hkSimdReal::convert(result);
#endif
}

template <int N>
HK_FORCE_INLINE const hkSimdReal hkVector4::dot(hkVector4Parameter a) const
{
	HK_VECTOR4_NOT_IMPLEMENTED;
	return hkSimdReal::getConstant<HK_QUADREAL_0>();
}


HK_FORCE_INLINE const hkSimdReal hkVector4::dot4xyz1(hkVector4Parameter a) const
{
#if HK_SSE_VERSION >= 0x41
	const hkQuadReal xyz = _mm_dp_ps(m_quad, a.m_quad, 0x7F);
	return hkSimdReal::convert(_mm_add_ps(xyz, _mm_shuffle_ps(m_quad, m_quad, _MM_SHUFFLE(3,3,3,3))));
#elif HK_SSE_VERSION >= 0x30
	const hkQuadReal xx2 = _mm_mul_ps(m_quad,a.m_quad);
	const hkQuadReal x2 = HK_VECTOR4_COMBINE_XYZ_W(xx2, m_quad);	// replace w by this.w
	const hkQuadReal hsum = _mm_hadd_ps(x2,x2); // xy zw xy zw
	return hkSimdReal::convert(_mm_hadd_ps(hsum,hsum)); // xyzw all 4
#else
	const hkQuadReal xx2 = _mm_mul_ps(m_quad,a.m_quad);
	const hkQuadReal x2 = HK_VECTOR4_COMBINE_XYZ_W(xx2, m_quad);	// replace w by this.w
	const hkQuadReal sum0 = _mm_add_ps( _mm_shuffle_ps(x2,x2,_MM_SHUFFLE(1,0,3,2)), x2); // yxwz+xyzw = xy xy zw zw
	const hkQuadReal sum1 = _mm_shuffle_ps(sum0,sum0, _MM_SHUFFLE(2,3,0,1)); // = zw zw xy xy
	const hkQuadReal result = _mm_add_ps( sum0, sum1 ); // = xyzw xyzw xyzw xyzw
	return hkSimdReal::convert(result);
#endif
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::horizontalAdd<2>() const
{
#if HK_SSE_VERSION >= 0x30
	const hkQuadReal x2 = _mm_hadd_ps(m_quad, m_quad);
	return hkSimdReal::convert(_mm_shuffle_ps(x2,x2,_MM_SHUFFLE(0,0,0,0)));
#else
	return hkSimdReal::convert(_mm_add_ps( _mm_shuffle_ps(m_quad,m_quad,_MM_SHUFFLE(1,1,1,1)), _mm_shuffle_ps(m_quad,m_quad,_MM_SHUFFLE(0,0,0,0))));
#endif
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::horizontalAdd<3>() const
{
#if HK_SSE_VERSION >= 0x30
	const hkQuadReal x2 = _mm_hadd_ps(m_quad, m_quad);
	return hkSimdReal::convert(_mm_add_ps( _mm_shuffle_ps(m_quad,m_quad,_MM_SHUFFLE(2,2,2,2)), _mm_shuffle_ps(x2,x2,_MM_SHUFFLE(0,0,0,0))));
#else
	const hkQuadReal xySum = _mm_add_ps( _mm_shuffle_ps(m_quad,m_quad,_MM_SHUFFLE(1,1,1,1)), _mm_shuffle_ps(m_quad,m_quad,_MM_SHUFFLE(0,0,0,0)));
	return hkSimdReal::convert(_mm_add_ps( _mm_shuffle_ps(m_quad,m_quad,_MM_SHUFFLE(2,2,2,2)), xySum));
#endif
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::horizontalAdd<4>() const
{
#if HK_SSE_VERSION >= 0x30
	const hkQuadReal x2 = _mm_hadd_ps(m_quad, m_quad);
	return hkSimdReal::convert(_mm_hadd_ps(x2, x2));
#else
	const hkQuadReal sum0 = _mm_add_ps( _mm_shuffle_ps(m_quad,m_quad,_MM_SHUFFLE(1,0,3,2)), m_quad); // yxwz+xyzw = xy xy zw zw
	const hkQuadReal sum1 = _mm_shuffle_ps(sum0,sum0, _MM_SHUFFLE(2,3,0,1)); // = zw zw xy xy
	return hkSimdReal::convert(_mm_add_ps( sum0, sum1 )); // xywz all 4
#endif
}

template <int N>
HK_FORCE_INLINE const hkSimdReal hkVector4::horizontalAdd() const
{
	HK_VECTOR4_NOT_IMPLEMENTED;
	return hkSimdReal::getConstant<HK_QUADREAL_0>();
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::horizontalMax<1>() const
{
	return getComponent<0>();
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::horizontalMax<2>() const
{
	return hkSimdReal::convert(_mm_max_ps( _mm_shuffle_ps(m_quad,m_quad,_MM_SHUFFLE(1,1,1,1)), _mm_shuffle_ps(m_quad,m_quad,_MM_SHUFFLE(0,0,0,0))));
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::horizontalMax<3>() const
{
	const hkQuadReal xy = _mm_max_ps( _mm_shuffle_ps(m_quad,m_quad,_MM_SHUFFLE(1,1,1,1)), _mm_shuffle_ps(m_quad,m_quad,_MM_SHUFFLE(0,0,0,0)));
	return hkSimdReal::convert(_mm_max_ps( _mm_shuffle_ps(m_quad,m_quad,_MM_SHUFFLE(2,2,2,2)), xy));
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::horizontalMax<4>() const
{
	const hkQuadReal sum0 = _mm_max_ps( _mm_shuffle_ps( m_quad, m_quad,_MM_SHUFFLE(1,0,3,2)), m_quad); // yxwz+xyzw = xy xy zw zw
	const hkQuadReal sum1 = _mm_shuffle_ps(sum0,sum0, _MM_SHUFFLE(2,3,0,1)); // = zw zw xy xy
	return hkSimdReal::convert(_mm_max_ps( sum0, sum1 )); // xywz all 4
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
	return hkSimdReal::convert(_mm_min_ps( _mm_shuffle_ps(m_quad,m_quad,_MM_SHUFFLE(1,1,1,1)), _mm_shuffle_ps(m_quad,m_quad,_MM_SHUFFLE(0,0,0,0))));
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::horizontalMin<3>() const
{
	const hkQuadReal xy = _mm_min_ps( _mm_shuffle_ps(m_quad,m_quad,_MM_SHUFFLE(1,1,1,1)), _mm_shuffle_ps(m_quad,m_quad,_MM_SHUFFLE(0,0,0,0)));
	return hkSimdReal::convert(_mm_min_ps( _mm_shuffle_ps(m_quad,m_quad,_MM_SHUFFLE(2,2,2,2)), xy));
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::horizontalMin<4>() const
{
	const hkQuadReal sum0 = _mm_min_ps( _mm_shuffle_ps( m_quad, m_quad,_MM_SHUFFLE(1,0,3,2)), m_quad); // yxwz+xyzw = xy xy zw zw
	const hkQuadReal sum1 = _mm_shuffle_ps(sum0,sum0, _MM_SHUFFLE(2,3,0,1)); // = zw zw xy xy
	return hkSimdReal::convert(_mm_min_ps( sum0, sum1 )); // xywz all 4
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
	return HK_M128(m_quad).m128_f32[a];
}

HK_FORCE_INLINE const hkReal& hkVector4::operator() (int a) const
{
	HK_ASSERT2(0x6d0c31d7, a>=0 && a<4, "index out of bounds for component access");
	return HK_M128(m_quad).m128_f32[a];
}

HK_FORCE_INLINE void hkVector4::setXYZ_W(hkVector4Parameter xyz, hkVector4Parameter w)
{
	m_quad = HK_VECTOR4_COMBINE_XYZ_W(xyz.m_quad, w.m_quad);
}

HK_FORCE_INLINE void hkVector4::setXYZ_W(hkVector4Parameter xyz, hkSimdRealParameter w)
{
	m_quad = HK_VECTOR4_COMBINE_XYZ_W(xyz.m_quad, w.m_real);
}

HK_FORCE_INLINE void hkVector4::setW(hkVector4Parameter w)
{
	m_quad = HK_VECTOR4_COMBINE_XYZ_W(m_quad, w.m_quad);
}

HK_FORCE_INLINE void hkVector4::setXYZ(hkVector4Parameter xyz)
{
	m_quad = HK_VECTOR4_COMBINE_XYZ_W(xyz.m_quad, m_quad);
}

HK_FORCE_INLINE void hkVector4::addXYZ(hkVector4Parameter xyz)
{
	m_quad = _mm_add_ps(m_quad, xyz.m_quad);
	HK_ON_DEBUG( HK_M128(m_quad).m128_u32[3] = 0xffffffff; )
}

HK_FORCE_INLINE void hkVector4::subXYZ(hkVector4Parameter xyz)
{
	m_quad = _mm_sub_ps(m_quad, xyz.m_quad);
	HK_ON_DEBUG( HK_M128(m_quad).m128_u32[3] = 0xffffffff; )
}

HK_FORCE_INLINE void hkVector4::setXYZ(hkReal v)
{
	const hkQuadReal q = _mm_set1_ps(v);
	m_quad = HK_VECTOR4_COMBINE_XYZ_W( q, m_quad );
}

HK_FORCE_INLINE void hkVector4::setXYZ(hkSimdRealParameter v)
{
	m_quad = HK_VECTOR4_COMBINE_XYZ_W( v.m_real, m_quad );
}

HK_FORCE_INLINE void hkVector4::setXYZ_0(hkVector4Parameter xyz)
{
	static HK_ALIGN16( const hkUint32 cw[4] ) = { 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000 };
	m_quad = _mm_and_ps( xyz.m_quad, *(const hkQuadReal*)&cw );
}

HK_FORCE_INLINE void hkVector4::setBroadcastXYZ(const int i, hkVector4Parameter v)
{
	setBroadcast(i,v);
	HK_ON_DEBUG( HK_M128(m_quad).m128_u32[3] = 0xffffffff; )
}

HK_FORCE_INLINE const hkSimdReal hkVector4::getComponent(const int i) const
{
	static HK_ALIGN16 (const hkUint32 indexToMask[16]) = 
	{
		0xffffffff, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0xffffffff, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0xffffffff, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0xffffffff
	};

	HK_ASSERT2(0x6d0c31d7, i>=0 && i<4, "index out of bounds for component access");

	const hkQuadReal mask = *(const hkQuadReal*)&indexToMask[ i * 4 ];
	hkQuadReal selected = _mm_and_ps(mask, m_quad); 

	const hkQuadReal zwxy = _mm_shuffle_ps( selected, selected, _MM_SHUFFLE(1,0,3,2));
	selected = _mm_or_ps( selected, zwxy );
	const hkQuadReal yxwz = _mm_shuffle_ps( selected, selected, _MM_SHUFFLE(2,3,0,1));
	selected = _mm_or_ps( selected, yxwz );
	return hkSimdReal::convert(selected);
}

template <int I>
HK_FORCE_INLINE const hkSimdReal hkVector4::getComponent() const
{
	HK_VECTOR4_SUBINDEX_CHECK;
	return hkSimdReal::convert(_mm_shuffle_ps(m_quad, m_quad, _MM_SHUFFLE(I,I,I,I)));
}


HK_FORCE_INLINE void hkVector4::setComponent(const int i, hkSimdRealParameter val)
{
	static HK_ALIGN16 (const hkUint32 indexToMask[16]) = 
	{
		0xffffffff, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0xffffffff, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0xffffffff, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0xffffffff
	};

	HK_ASSERT2(0x6d0c31d7, i>=0 && i<4, "index out of bounds for component access");

	const hkQuadReal mask = *(const hkQuadReal*)&indexToMask[ i * 4 ];

#if HK_SSE_VERSION >= 0x41
	m_quad = _mm_blendv_ps(m_quad, val.m_real, mask);
#else
	m_quad = _mm_or_ps( _mm_and_ps(mask, val.m_real), _mm_andnot_ps(mask, m_quad) );
#endif
}

#if HK_SSE_VERSION >= 0x41
template <int I>
HK_FORCE_INLINE void hkVector4::setComponent(hkSimdRealParameter val)
{
	HK_VECTOR4_SUBINDEX_CHECK;
	m_quad = _mm_blend_ps(m_quad, val.m_real, 0x1 << I);
	// todo check if _mm_insert_ps is faster
}
#else
template <>
HK_FORCE_INLINE void hkVector4::setComponent<0>(hkSimdRealParameter val)
{
	m_quad = _mm_move_ss( m_quad, val.m_real );
}
template <>
HK_FORCE_INLINE void hkVector4::setComponent<1>(hkSimdRealParameter val)
{
	m_quad = _mm_shuffle_ps( _mm_unpacklo_ps(m_quad, val.m_real), m_quad, _MM_SHUFFLE(3,2,1,0));
}
template <>
HK_FORCE_INLINE void hkVector4::setComponent<2>(hkSimdRealParameter val)
{
	m_quad = _mm_shuffle_ps( m_quad, _mm_unpackhi_ps(m_quad, val.m_real), _MM_SHUFFLE(2,3,1,0));
}
template <>
HK_FORCE_INLINE void hkVector4::setComponent<3>(hkSimdRealParameter val)
{
	m_quad = _mm_shuffle_ps( m_quad, _mm_unpackhi_ps(m_quad, val.m_real), _MM_SHUFFLE(3,0,1,0));
}
template <int N>
HK_FORCE_INLINE void hkVector4::setComponent(hkSimdRealParameter val)
{
	HK_VECTOR4_NOT_IMPLEMENTED;
}
#endif


HK_FORCE_INLINE void hkVector4::reduceToHalfPrecision()
{
	__m128i precisionMask = _mm_set1_epi32(0xffff0000);
	m_quad = _mm_and_ps( m_quad, _mm_castsi128_ps(precisionMask) );
}


template <> 
HK_FORCE_INLINE hkBool32 hkVector4::isOk<1>() const
{
	const hkQuadReal nanMask = _mm_cmpunord_ps(m_quad, _mm_setzero_ps());
	return !(_mm_movemask_ps(nanMask) & 0x1);
}

template <> 
HK_FORCE_INLINE hkBool32 hkVector4::isOk<2>() const
{
	const hkQuadReal nanMask = _mm_cmpunord_ps(m_quad, _mm_setzero_ps());
	return !(_mm_movemask_ps(nanMask) & 0x3);
}

template <> 
HK_FORCE_INLINE hkBool32 hkVector4::isOk<3>() const
{
	const hkQuadReal nanMask = _mm_cmpunord_ps(m_quad, _mm_setzero_ps());
	return !(_mm_movemask_ps(nanMask) & 0x7);
}

template <> 
HK_FORCE_INLINE hkBool32 hkVector4::isOk<4>() const
{
	const hkQuadReal nanMask = _mm_cmpunord_ps(m_quad, _mm_setzero_ps());
	return !_mm_movemask_ps(nanMask);
}

template <int N> 
HK_FORCE_INLINE hkBool32 hkVector4::isOk() const
{
	HK_VECTOR4_NOT_IMPLEMENTED;
	return false;
}


template <> 
HK_FORCE_INLINE void hkVector4::setPermutation<hkVectorPermutation::XYZW>(hkVector4Parameter v)
{
	m_quad = v.m_quad;
}

#if HK_SSE_VERSION >= 0x30
template <> 
HK_FORCE_INLINE void hkVector4::setPermutation<hkVectorPermutation::XXZZ>(hkVector4Parameter v)
{
	m_quad = _mm_moveldup_ps(v.m_quad);
}

template <> 
HK_FORCE_INLINE void hkVector4::setPermutation<hkVectorPermutation::YYWW>(hkVector4Parameter v)
{
	m_quad = _mm_movehdup_ps(v.m_quad);
}
#endif

template <> 
HK_FORCE_INLINE void hkVector4::setPermutation<hkVectorPermutation::XXYY>(hkVector4Parameter v)
{
	m_quad = _mm_unpacklo_ps(v.m_quad,v.m_quad);
}

template <hkVectorPermutation::Permutation P> 
HK_FORCE_INLINE void hkVector4::setPermutation(hkVector4Parameter v)
{
	const int shuf = ((P >> (12 - 0)) & 0x03) |
		((P >> ( 8 - 2)) & 0x0c) |
		((P >> ( 4 - 4)) & 0x30) |
		((P << ( 0 + 6)) & 0xc0);

	m_quad = _mm_shuffle_ps(v.m_quad, v.m_quad, shuf); 
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::signBitSet() const
{
	static HK_ALIGN16( const hkUint32 signmask[4] ) = { 0x80000000, 0x80000000, 0x80000000, 0x80000000 };
	const hkQuadReal signsSet = _mm_and_ps(m_quad, *(const hkQuadReal*)&signmask);
	return hkVector4Comparison::convert( _mm_castsi128_ps(_mm_cmpeq_epi32(_mm_castps_si128(signsSet), *(const __m128i*)&signmask)) );
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::signBitClear() const
{
	static HK_ALIGN16( const hkUint32 signmask[4] ) = { 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff };
	const hkQuadReal signsSet = _mm_and_ps(m_quad, *(const hkQuadReal*)&signmask);
	return hkVector4Comparison::convert( _mm_castsi128_ps(_mm_cmpeq_epi32(_mm_castps_si128(signsSet), *(const __m128i*)&m_quad)) );
}

HK_FORCE_INLINE void hkVector4::setFlipSign(hkVector4Parameter v, hkVector4ComparisonParameter mask)
{
	static HK_ALIGN16( const hkUint32 signmask[4] ) = { 0x80000000, 0x80000000, 0x80000000, 0x80000000 };
	m_quad = _mm_xor_ps(v.m_quad, _mm_and_ps(mask.m_mask, *(const hkQuadReal*)&signmask));
}

HK_FORCE_INLINE void hkVector4::setFlipSign(hkVector4Parameter v, hkVector4Parameter vSign)
{
	static HK_ALIGN16( const hkUint32 signmask[4] ) = { 0x80000000, 0x80000000, 0x80000000, 0x80000000 };
	m_quad = _mm_xor_ps(v.m_quad, _mm_and_ps(vSign.m_quad, *(const hkQuadReal*)&signmask));
}

HK_FORCE_INLINE void hkVector4::setFlipSign(hkVector4Parameter v, hkSimdRealParameter sSign)
{
	static HK_ALIGN16( const hkUint32 signmask[4] ) = { 0x80000000, 0x80000000, 0x80000000, 0x80000000 };
	m_quad = _mm_xor_ps(v.m_quad, _mm_and_ps(sSign.m_real, *(const hkQuadReal*)&signmask));
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
		case HK_ACC_23_BIT: self = hkMath::quadReciprocal(a.m_quad); break;
		case HK_ACC_12_BIT: self = _mm_rcp_ps(a.m_quad); break;
		default:         self = _mm_div_ps(g_vectorConstants[HK_QUADREAL_1],a.m_quad); break; // HK_ACC_FULL
	}
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_SET_ZERO> { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a)
{
	const hkQuadReal equalsZero = _mm_cmpeq_ps(a.m_quad, _mm_setzero_ps());
	hkQuadReal e; unroll_setReciprocal<A, HK_DIV_IGNORE>::apply(e, a);
	self = _mm_andnot_ps(equalsZero, e);
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_SET_HIGH> { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a)
{
	const hkQuadReal equalsZero = _mm_cmpeq_ps(a.m_quad, _mm_setzero_ps());
	hkQuadReal e; unroll_setReciprocal<A, HK_DIV_IGNORE>::apply(e, a);
	const hkQuadReal huge = _mm_set1_ps(HK_REAL_HIGH);
#if HK_SSE_VERSION >= 0x41
	self = _mm_blendv_ps(e, huge, equalsZero);
#else
	self = _mm_or_ps( _mm_and_ps(equalsZero, huge), _mm_andnot_ps(equalsZero, e) );
#endif
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_SET_MAX> { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a)
{
	const hkQuadReal equalsZero = _mm_cmpeq_ps(a.m_quad, _mm_setzero_ps());
	hkQuadReal e; unroll_setReciprocal<A, HK_DIV_IGNORE>::apply(e, a);
	const hkQuadReal huge = _mm_set1_ps(HK_REAL_MAX);
#if HK_SSE_VERSION >= 0x41
	self = _mm_blendv_ps(e, huge, equalsZero);
#else
	self = _mm_or_ps( _mm_and_ps(equalsZero, huge), _mm_andnot_ps(equalsZero, e) );
#endif
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_SET_ZERO_AND_ONE> { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a)
{
	unroll_setReciprocal<A, HK_DIV_SET_ZERO>::apply(self, a);
	const hkQuadReal one = g_vectorConstants[HK_QUADREAL_1];
	const hkQuadReal absVal = hkMath::quadFabs(self);
	const hkQuadReal absValLessOne = _mm_sub_ps(absVal, one);
	const hkQuadReal lessEqualEps = _mm_cmple_ps(absValLessOne, g_vectorConstants[HK_QUADREAL_EPS]);
#if HK_SSE_VERSION >= 0x41
	self = _mm_blendv_ps(self, one, lessEqualEps);
#else
	self = _mm_or_ps( _mm_and_ps(lessEqualEps, one), _mm_andnot_ps(lessEqualEps, self) );
#endif
} };

} // namespace 

template <hkMathAccuracyMode A, hkMathDivByZeroMode D>
HK_FORCE_INLINE void hkVector4::setReciprocal(hkVector4Parameter a)
{
	hkVector4_AdvancedInterface::unroll_setReciprocal<A,D>::apply(m_quad,a);
}

HK_FORCE_INLINE void hkVector4::setReciprocal(hkVector4Parameter a)
{
	hkVector4_AdvancedInterface::unroll_setReciprocal<HK_ACC_23_BIT,HK_DIV_IGNORE>::apply(m_quad,a);
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
		case HK_ACC_23_BIT: self = _mm_mul_ps(a.m_quad,hkMath::quadReciprocal(b.m_quad)); break;
		case HK_ACC_12_BIT: self = _mm_mul_ps(a.m_quad,_mm_rcp_ps(b.m_quad)); break;
		default:         self = _mm_div_ps(a.m_quad,b.m_quad); break; // HK_ACC_FULL
	}
} };
template <hkMathAccuracyMode A>
struct unroll_setDiv<A, HK_DIV_SET_ZERO> { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a, hkVector4Parameter b)
{
	const hkQuadReal equalsZero = _mm_cmpeq_ps(b.m_quad, _mm_setzero_ps());
	hkQuadReal e; unroll_setReciprocal<A, HK_DIV_IGNORE>::apply(e, b);
	self = _mm_mul_ps(a.m_quad,_mm_andnot_ps(equalsZero, e));
} };
template <hkMathAccuracyMode A>
struct unroll_setDiv<A, HK_DIV_SET_HIGH> { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a, hkVector4Parameter b)
{
	const hkQuadReal equalsZero = _mm_cmpeq_ps(b.m_quad, _mm_setzero_ps());
	hkQuadReal e; unroll_setReciprocal<A, HK_DIV_IGNORE>::apply(e, b);
	const hkQuadReal huge = _mm_set1_ps(HK_REAL_HIGH);
	const hkQuadReal val = _mm_mul_ps(a.m_quad, e);
#if HK_SSE_VERSION >= 0x41
	self = _mm_blendv_ps(val, huge, equalsZero);
#else
	self = _mm_or_ps( _mm_and_ps(equalsZero, huge), _mm_andnot_ps(equalsZero, val) );
#endif
} };
template <hkMathAccuracyMode A>
struct unroll_setDiv<A, HK_DIV_SET_MAX> { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a, hkVector4Parameter b)
{
	const hkQuadReal equalsZero = _mm_cmpeq_ps(b.m_quad, _mm_setzero_ps());
	hkQuadReal e; unroll_setReciprocal<A, HK_DIV_IGNORE>::apply(e, b);
	const hkQuadReal huge = _mm_set1_ps(HK_REAL_MAX);
	const hkQuadReal val = _mm_mul_ps(a.m_quad, e);
#if HK_SSE_VERSION >= 0x41
	self = _mm_blendv_ps(val, huge, equalsZero);
#else
	self = _mm_or_ps( _mm_and_ps(equalsZero, huge), _mm_andnot_ps(equalsZero, val) );
#endif
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
		case HK_ACC_23_BIT: self = _mm_mul_ps(a.m_quad,hkMath::quadReciprocalSquareRoot(a.m_quad)); break;
		case HK_ACC_12_BIT: self = _mm_mul_ps(a.m_quad,_mm_rsqrt_ps(a.m_quad)); break;
		default:         self = _mm_sqrt_ps(a.m_quad); break; // HK_ACC_FULL
	}
} };
template <hkMathAccuracyMode A>
struct unroll_setSqrt<A, HK_SQRT_SET_ZERO> { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a)
{
	const hkQuadReal equalsZero = _mm_cmple_ps(a.m_quad, _mm_setzero_ps());
	hkQuadReal e; unroll_setSqrt<A, HK_SQRT_IGNORE>::apply(e,a);
	self = _mm_andnot_ps(equalsZero, e);
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
		case HK_ACC_23_BIT: self = hkMath::quadReciprocalSquareRoot(a.m_quad); break;
		case HK_ACC_12_BIT: self = _mm_rsqrt_ps(a.m_quad); break;
		default:			self = _mm_div_ps(g_vectorConstants[HK_QUADREAL_1], _mm_sqrt_ps(a.m_quad)); break; // HK_ACC_FULL
	}
} };
template <hkMathAccuracyMode A>
struct unroll_setSqrtInverse<A, HK_SQRT_SET_ZERO> { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a)
{
	const hkQuadReal equalsZero = _mm_cmple_ps(a.m_quad, _mm_setzero_ps());
	hkQuadReal e; unroll_setSqrtInverse<A, HK_SQRT_IGNORE>::apply(e,a);
	self = _mm_andnot_ps(equalsZero, e);
} };
} // namespace 

template <hkMathAccuracyMode A, hkMathNegSqrtMode S>
HK_FORCE_INLINE void hkVector4::setSqrtInverse(hkVector4Parameter a)
{
	hkVector4_AdvancedInterface::unroll_setSqrtInverse<A,S>::apply(m_quad,a);
}

HK_FORCE_INLINE void hkVector4::setSqrtInverse(hkVector4Parameter a)
{
	hkVector4_AdvancedInterface::unroll_setSqrtInverse<HK_ACC_23_BIT,HK_SQRT_SET_ZERO>::apply(m_quad,a);
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
	switch (N)
	{
		case 1:
			{
				self = _mm_load_ss(p);
				HK_ON_DEBUG(HK_M128(self).m128_u32[1] = 0xffffffff; HK_M128(self).m128_u32[2] = 0xffffffff; HK_M128(self).m128_u32[3] = 0xffffffff;)
			}
			break;
		case 2:
			{
				self = _mm_castpd_ps(_mm_load_sd((const double*)p));
				HK_ON_DEBUG(HK_M128(self).m128_u32[2] = 0xffffffff; HK_M128(self).m128_u32[3] = 0xffffffff;)
			}
			break;
		case 3:
			{
				__m128 xy = _mm_castpd_ps(_mm_load_sd((const double*)p));
				__m128 z = _mm_load_ss(p+2);
				self = _mm_movelh_ps(xy,z);
				HK_ON_DEBUG(HK_M128(self).m128_u32[3] = 0xffffffff;)
			}
			break;
		default:
			{
#if HK_SSE_VERSION >= 0x30
				self = _mm_castsi128_ps(_mm_lddqu_si128((const __m128i*)p));
#else
				self = _mm_loadu_ps(p);
#endif
			}
			break;
	}
} };
template <int N>
struct unroll_load<N, HK_IO_NATIVE_ALIGNED> { HK_FORCE_INLINE static void apply(hkQuadReal& self, const hkReal* HK_RESTRICT p)
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkReal)-1) ) == 0, "pointer must be aligned to native size of hkReal.");
	unroll_load<N, HK_IO_BYTE_ALIGNED>::apply(self,p);
} };
template <int N>
struct unroll_load<N, HK_IO_SIMD_ALIGNED> { HK_FORCE_INLINE static void apply(hkQuadReal& self, const hkReal* HK_RESTRICT p)
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & ((sizeof(hkReal)*(N!=3?N:4) )-1) ) == 0, "pointer must be aligned for SIMD.");
	switch (N)
	{
		case 4: 
			{
				self = _mm_load_ps(p);
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
#if HK_SSE_VERSION >= 0x41
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & ((sizeof(hkReal)*(N!=3?N:4) )-1) ) == 0, "pointer must be aligned for SIMD.");
	switch (N)
	{
	case 4: 
		{
			self = _mm_castsi128_ps(_mm_stream_load_si128((__m128i*) p));
		}
		break;
	default:
		{
			unroll_load<N, HK_IO_SIMD_ALIGNED>::apply(self,p);
		}
		break;
	}
#else
	unroll_load<N, HK_IO_SIMD_ALIGNED>::apply(self,p);
#endif
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
	case 1:
		{
			self = _mm_set_ss(hkReal(p[0]));
			HK_ON_DEBUG(HK_M128(self).m128_u32[1] = 0xffffffff; HK_M128(self).m128_u32[2] = 0xffffffff; HK_M128(self).m128_u32[3] = 0xffffffff;)
		}
		break;
	case 2:
		{
			__m128i twohalfs = _mm_castps_si128( _mm_load_ss((const float*)p) );
			self = _mm_castsi128_ps( _mm_unpacklo_epi16(_mm_setzero_si128(), twohalfs) );
			HK_ON_DEBUG(HK_M128(self).m128_u32[2] = 0xffffffff; HK_M128(self).m128_u32[3] = 0xffffffff;)
		}
		break;
	case 3:
		{
			__m128i h = _mm_castps_si128( _mm_load_ss((const float*)p) );
			__m128 twohalfs = _mm_castsi128_ps( _mm_unpacklo_epi16(_mm_setzero_si128(), h) );
			__m128 val = _mm_set1_ps(hkReal(p[2]));
			self = _mm_movelh_ps(twohalfs,val);
			HK_ON_DEBUG(HK_M128(self).m128_u32[3] = 0xffffffff;)
		}
		break;
	default:
		{
			__m128i fourhalfs = _mm_castpd_si128(_mm_load_sd((const double*)p));
			self = _mm_castsi128_ps( _mm_unpacklo_epi16(_mm_setzero_si128(), fourhalfs) );
		}
		break;
	}
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
	static HK_ALIGN16(const unsigned int half_sign[4])		  = {0x00008000, 0x00008000, 0x00008000, 0x00008000};
	static HK_ALIGN16(const unsigned int half_exponent[4])	  = {0x00007C00, 0x00007C00, 0x00007C00, 0x00007C00};
	static HK_ALIGN16(const unsigned int half_mantissa[4])	  = {0x000003FF, 0x000003FF, 0x000003FF, 0x000003FF};
	static HK_ALIGN16(const unsigned int half_bias_offset[4]) = {0x0001C000, 0x0001C000, 0x0001C000, 0x0001C000};

	__m128i r;
	switch (N)
	{
	case 1:
		{
			hkFloat16 tmp[2];
			tmp[0] = p[0];
			tmp[1].setZero();
			r = _mm_castps_si128(_mm_load_ss((const float*)tmp));
		}
		break;
	case 2:
		{
			r = _mm_castps_si128(_mm_load_ss((const float*)p));
		}
		break;
	case 3:
		{
			hkFloat16 tmp[4];
			tmp[0] = p[0];
			tmp[1] = p[1];
			tmp[2] = p[2];
			tmp[3].setZero();
			r = _mm_castpd_si128(_mm_load_sd((const double*)tmp));
		}
		break;
	default:
		{
			r = _mm_castpd_si128(_mm_load_sd((const double*)p));
		}
		break;
	}

	__m128i unpacked = _mm_unpacklo_epi16(r, _mm_setzero_si128());

	__m128i sign = _mm_and_si128(unpacked, *(__m128i*)half_sign);
	__m128i exponent = _mm_and_si128(unpacked, *(__m128i*)half_exponent);
	__m128i exp_zero = _mm_cmpeq_epi32(exponent, _mm_setzero_si128());
	__m128i mantissa = _mm_and_si128(unpacked, *(__m128i*)half_mantissa);
	__m128i exp_offset = _mm_andnot_si128(exp_zero, _mm_add_epi32(exponent, *(__m128i*)half_bias_offset));

	__m128i sign_shift = _mm_slli_epi32(sign, 16);
	__m128i exp_mantissa = _mm_slli_epi32(_mm_or_si128(exp_offset,mantissa), 13);

	self = _mm_castsi128_ps(_mm_or_si128(sign_shift, exp_mantissa));

#if defined(HK_DEBUG)
	for(int i=N; i<4; ++i) HK_M128(self).m128_u32[i] = 0xffffffff;
#endif
} };
template <int N>
struct unroll_loadF16<N, HK_IO_NATIVE_ALIGNED> { HK_FORCE_INLINE static void apply(hkQuadReal& self, const hkFloat16* HK_RESTRICT p)
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkFloat16)-1) ) == 0, "pointer must be aligned to native size of hkFloat16.");
	unroll_loadF16<N, HK_IO_BYTE_ALIGNED>::apply(self,p);
} };
template <int N>
struct unroll_loadF16<N, HK_IO_SIMD_ALIGNED> { HK_FORCE_INLINE static void apply(hkQuadReal& self, const hkFloat16* HK_RESTRICT p)
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & ((sizeof(hkReal)*(N!=3?N:4) )-1) ) == 0, "pointer must be aligned for SIMD.");
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
	switch (N)
	{
	case 1:
		{
			_mm_store_ss(p, self);
		}
		break;
	case 2:
		{
			_mm_store_sd((double*) p, _mm_castps_pd(self));
		}
		break;
	case 3:
		{
			_mm_store_sd((double*) p, _mm_castps_pd(self));
			const hkQuadReal p2 = _mm_shuffle_ps(self,self,_MM_SHUFFLE(2,2,2,2));
			_mm_store_ss(p+2, p2);
			
			
			
			
			
			
		}
		break;
	default:
		{
			_mm_storeu_ps(p, self);
		}
		break;
	}
} };
template <int N>
struct unroll_store<N, HK_IO_NATIVE_ALIGNED> { HK_FORCE_INLINE static void apply(const hkQuadReal& self, hkReal* HK_RESTRICT p)
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkReal)-1) ) == 0, "pointer must be aligned to native size of hkReal.");
	unroll_store<N, HK_IO_BYTE_ALIGNED>::apply(self,p);
} };
template <int N>
struct unroll_store<N, HK_IO_SIMD_ALIGNED> { HK_FORCE_INLINE static void apply(const hkQuadReal& self, hkReal* HK_RESTRICT p)
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & ((sizeof(hkReal)*(N!=3?N:4) )-1) ) == 0, "pointer must be aligned for SIMD.");
	switch (N)
	{
	case 4: 
		{
			_mm_store_ps(p, self);
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
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & ((sizeof(hkReal)*(N!=3?N:4) )-1) ) == 0, "pointer must be aligned for SIMD.");
	switch (N)
	{
	case 4: 
		{
			_mm_stream_ps(p, self);
		}
		break;
	default:
		{
			unroll_store<N, HK_IO_SIMD_ALIGNED>::apply(self,p);
		}
		break;
	}
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
	__m128  vs0;
	if (R == HK_ROUND_NEAREST)
		vs0 = _mm_mul_ps( self, g_vectorConstants[HK_QUADREAL_PACK_HALF] );
	else
		vs0 = self;
	__m128i tmp0 = _mm_srai_epi32( _mm_castps_si128(vs0), 16 );
	__m128i tmp1 = _mm_packs_epi32(tmp0, tmp0);

	switch (N)
	{
	case 1:
		{
			float ftmp1; _mm_store_ss(&ftmp1, _mm_castsi128_ps(tmp1));
			const hkHalf* HK_RESTRICT htmp1 = (const hkHalf* HK_RESTRICT)&ftmp1;
			p[0] = htmp1[0];
		}
		break;
	case 2:
		{
			_mm_store_ss((float*)p, _mm_castsi128_ps(tmp1));
		}
		break;
	case 3:
		{
			double dtmp1; _mm_store_sd(&dtmp1, _mm_castsi128_pd(tmp1));
			const hkHalf* HK_RESTRICT htmp1 = (const hkHalf* HK_RESTRICT)&dtmp1;
			p[0] = htmp1[0];
			p[1] = htmp1[1];
			p[2] = htmp1[2];
		}
		break;
	default:
		{
			_mm_store_sd((double*) p, _mm_castsi128_pd(tmp1));
		}
		break;
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
	static HK_ALIGN16(const unsigned int abs[4])      = {0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF};
	static HK_ALIGN16(const unsigned int infinity[4]) = {0x47FFE000, 0x47FFE000, 0x47FFE000, 0x47FFE000};
	static HK_ALIGN16(const unsigned int denormal[4]) = {0x38800000, 0x38800000, 0x38800000, 0x38800000};
	static HK_ALIGN16(const unsigned int fixup[4])    = {0x48000000, 0x48000000, 0x48000000, 0x48000000};
	static HK_ALIGN16(const unsigned int round1[4])   = {0x00000001, 0x00000001, 0x00000001, 0x00000001};
	static HK_ALIGN16(const unsigned int round2[4])   = {0x00000FFF, 0x00000FFF, 0x00000FFF, 0x00000FFF};
	static HK_ALIGN16(const unsigned int sign[4])     = {0x80000000, 0x80000000, 0x80000000, 0x80000000};
	static HK_ALIGN16(const unsigned int base[4])     = {0x00007FFF, 0x00007FFF, 0x00007FFF, 0x00007FFF};
	static HK_ALIGN16(const unsigned int integer[4])  = {0x52000000, 0x52000000, 0x52000000, 0x52000000};

	// Compute masks
	__m128 r_abs = _mm_and_ps(self, *(__m128*)abs);
	__m128 r_inf = _mm_cmpnlt_ps(r_abs, *(__m128*)infinity);
	__m128 r_den = _mm_cmplt_ps(r_abs, *(__m128*)denormal);

	// Denormal case
	__m128i r_int = _mm_cvttps_epi32( _mm_mul_ps(r_abs, *(__m128*)integer) );

	// Normal case and combine
	__m128i fix = _mm_add_epi32(_mm_castps_si128(r_abs), *(__m128i*)fixup);
	__m128i select_den = _mm_and_si128(r_int, _mm_castps_si128(r_den));
	__m128i select_fix = _mm_andnot_si128(_mm_castps_si128(r_den), fix);
	__m128i all = _mm_or_si128(select_den, select_fix);

	__m128i all_rounded;
	if (R == HK_ROUND_NEAREST)
	{
		// Correct rounding
		__m128i rounded = _mm_add_epi32(_mm_and_si128(_mm_srli_epi32(all, 13), *(__m128i*)round1), *(__m128i*)round2);
		all_rounded = _mm_add_epi32(rounded, all);
	}
	else
	{
		all_rounded = all;
	}

	// Combine with sign and infinity
	__m128i extract_sign = _mm_srai_epi32(_mm_and_si128(_mm_castps_si128(self), *(__m128i*)sign), 16);
	__m128i mantissa = _mm_and_si128(_mm_or_si128(_mm_srli_epi32(all_rounded, 13), _mm_castps_si128(r_inf)), *(__m128i*)base);
	__m128i assembled = _mm_or_si128(mantissa, extract_sign); // Result in lower words of each element

	// Pack
	__m128i packed = _mm_packs_epi32(assembled, assembled); // result in lower elements

	switch (N)
	{
	case 1:
		{
			float ftmp1; _mm_store_ss(&ftmp1, _mm_castsi128_ps(packed));
			const hkFloat16* HK_RESTRICT htmp1 = (const hkFloat16* HK_RESTRICT)&ftmp1;
			p[0] = htmp1[0];
		}
		break;
	case 2:
		{
			_mm_store_ss((float*)p, _mm_castsi128_ps(packed));
		}
		break;
	case 3:
		{
			double dtmp1; _mm_store_sd(&dtmp1, _mm_castsi128_pd(packed));
			const hkFloat16* HK_RESTRICT htmp1 = (const hkFloat16* HK_RESTRICT)&dtmp1;
			p[0] = htmp1[0];
			p[1] = htmp1[1];
			p[2] = htmp1[2];
		}
		break;
	default:
		{
			_mm_store_sd((double*) p, _mm_castsi128_pd(packed));
		}
		break;
	}
} };
template <int N, hkMathRoundingMode R>
struct unroll_storeF16<N, HK_IO_NATIVE_ALIGNED, R> { HK_FORCE_INLINE static void apply(const hkQuadReal& self, hkFloat16* HK_RESTRICT p)
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & (sizeof(hkFloat16)-1) ) == 0, "pointer must be aligned to native size of hkFloat16.");
	unroll_storeF16<N, HK_IO_BYTE_ALIGNED, R>::apply(self,p);
} };
template <int N, hkMathRoundingMode R>
struct unroll_storeF16<N, HK_IO_SIMD_ALIGNED, R> { HK_FORCE_INLINE static void apply(const hkQuadReal& self, hkFloat16* HK_RESTRICT p)
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & ((sizeof(hkReal)*(N!=3?N:4) )-1) ) == 0, "pointer must be aligned for SIMD.");
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


#undef HK_VECTOR4_COMBINE_XYZ_W

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
