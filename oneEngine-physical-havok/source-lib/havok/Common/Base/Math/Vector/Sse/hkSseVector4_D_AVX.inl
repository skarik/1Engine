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

#define HK_VECTOR4_COMBINE_XYZ_W(xyz, w) _mm256_blend_pd( xyz, w, 0x8)

// helper funcs to avoid SSE code
template <int I>
HK_FORCE_INLINE static hkQuadReal broadcastScalar(hkVector4Parameter v)
{
	HK_VECTOR4_NOT_IMPLEMENTED;
	return g_vectorConstants[HK_QUADREAL_0];
}

template <>
HK_FORCE_INLINE static hkQuadReal broadcastScalar<0>(hkVector4Parameter v)
{
	const hkQuadReal xz = _mm256_permute_pd(v.m_quad, 0x0);
	return _mm256_permute2f128_pd(xz,xz,0x00);
}

template <>
HK_FORCE_INLINE static hkQuadReal broadcastScalar<1>(hkVector4Parameter v)
{
	const hkQuadReal yw = _mm256_permute_pd(v.m_quad, 0xF);
	return _mm256_permute2f128_pd(yw,yw,0x00);
}

template <>
HK_FORCE_INLINE static hkQuadReal broadcastScalar<2>(hkVector4Parameter v)
{
	const hkQuadReal xz = _mm256_permute_pd(v.m_quad, 0x0);
	return _mm256_permute2f128_pd(xz,xz,0x11);
}

template <>
HK_FORCE_INLINE static hkQuadReal broadcastScalar<3>(hkVector4Parameter v)
{
	const hkQuadReal yw = _mm256_permute_pd(v.m_quad, 0xF);
	return _mm256_permute2f128_pd(yw,yw,0x11);
}


/* quad, here for inlining */

#ifndef HK_DISABLE_MATH_CONSTRUCTORS
/* construct, assign, zero */
HK_FORCE_INLINE hkVector4::hkVector4(hkReal a, hkReal b, hkReal c, hkReal d)
{
	m_quad = _mm256_setr_pd(a,b,c,d);
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
	m_quad = _mm256_setr_pd(a,b,c,d);
}

HK_FORCE_INLINE void hkVector4::set( hkSimdRealParameter a, hkSimdRealParameter b, hkSimdRealParameter c, hkSimdRealParameter d )
{
	const hkQuadReal ab = _mm256_castpd128_pd256(_mm_move_sd(b.m_real,a.m_real)); // reinterpret lower register half
	const hkQuadReal cd = _mm256_castpd128_pd256(_mm_move_sd(d.m_real,c.m_real));
	m_quad = _mm256_permute2f128_pd(ab, cd, 0x20);
}

HK_FORCE_INLINE void hkVector4::setAll(const hkReal& a)
{
	m_quad = _mm256_broadcast_sd(&a);
}

HK_FORCE_INLINE void hkVector4::setAll(hkSimdRealParameter a)
{
	const hkQuadReal sr = _mm256_castpd128_pd256(a.m_real); // reinterpret lower register half
	m_quad = _mm256_permute2f128_pd(sr, sr, 0x00);
}

HK_FORCE_INLINE void hkVector4::setZero()
{
	m_quad = _mm256_setzero_pd();
}

template <int I> 
HK_FORCE_INLINE void hkVector4::zeroComponent()
{
	HK_VECTOR4_SUBINDEX_CHECK;
	m_quad = _mm256_blend_pd(m_quad, _mm256_setzero_pd(), 1<<I);
}

HK_FORCE_INLINE void hkVector4::zeroComponent(const int i)
{
	HK_ASSERT2(0x3bc36625, (i>=0) && (i<4), "Component index out of range");
	static HK_ALIGN32( const hkUint64 cx[4*4] ) ={ 
		 0x0000000000000000, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff ,
		 0xffffffffffffffff, 0x0000000000000000, 0xffffffffffffffff, 0xffffffffffffffff ,
		 0xffffffffffffffff, 0xffffffffffffffff, 0x0000000000000000, 0xffffffffffffffff ,
		 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0x0000000000000000  };

	m_quad = _mm256_and_pd( m_quad, *(const hkQuadReal*)(cx+(i*4)) );
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::getComponent<0>() const
{
	return hkSimdReal::convert(_mm256_castpd256_pd128(_mm256_permute_pd(m_quad, 0x0)));
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::getComponent<1>() const
{
	return hkSimdReal::convert(_mm256_castpd256_pd128(_mm256_permute_pd(m_quad, 0x3)));
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::getComponent<2>() const
{
	return hkSimdReal::convert(_mm256_extractf128_pd(_mm256_permute_pd(m_quad, 0x0),1));
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::getComponent<3>() const
{
	return hkSimdReal::convert(_mm256_extractf128_pd(_mm256_permute_pd(m_quad, 0xC),1));
}

template <int I>
HK_FORCE_INLINE const hkSimdReal hkVector4::getComponent() const
{
	HK_VECTOR4_NOT_IMPLEMENTED;
	return hkSimdReal::getConstant<HK_QUADREAL_0>();
}

HK_FORCE_INLINE const hkSimdReal hkVector4::getComponent(const int i) const
{
	HK_ASSERT2(0x6d0c31d7, i>=0 && i<4, "index out of bounds for component access");

	__m128d selected;

	switch(i)
	{
		case 1:  { selected = _mm256_extractf128_pd(_mm256_permute_pd(m_quad, 0xF),0); } break;
		case 2:  { selected = _mm256_extractf128_pd(_mm256_permute_pd(m_quad, 0x0),1); } break;
		case 3:  { selected = _mm256_extractf128_pd(_mm256_permute_pd(m_quad, 0xF),1); } break;
		default: { selected = _mm256_extractf128_pd(_mm256_permute_pd(m_quad, 0x0),0); } break;
	}

	return hkSimdReal::convert(selected);
}

HK_FORCE_INLINE void hkVector4::setAdd(hkVector4Parameter v0, hkVector4Parameter v1)
{
	m_quad = _mm256_add_pd(v0.m_quad, v1.m_quad);
}

HK_FORCE_INLINE void hkVector4::setSub(hkVector4Parameter v0, hkVector4Parameter v1)
{
	m_quad = _mm256_sub_pd(v0.m_quad, v1.m_quad);
}

HK_FORCE_INLINE void hkVector4::setMul(hkVector4Parameter v0, hkVector4Parameter v1)
{
	m_quad = _mm256_mul_pd(v0.m_quad, v1.m_quad);
}

HK_FORCE_INLINE void hkVector4::setMul(hkVector4Parameter v1, hkSimdRealParameter r)
{
	const hkQuadReal sr = _mm256_castpd128_pd256(r.m_real); // reinterpret lower register half
	const hkQuadReal s = _mm256_permute2f128_pd(sr, sr, 0x00);
	m_quad = _mm256_mul_pd( s, v1.m_quad);
}

HK_FORCE_INLINE void hkVector4::setSubMul(hkVector4Parameter a, hkVector4Parameter b, hkSimdRealParameter r)
{
	const hkQuadReal sr = _mm256_castpd128_pd256(r.m_real); // reinterpret lower register half
	const hkQuadReal s = _mm256_permute2f128_pd(sr, sr, 0x00);
#if HK_SSE_VERSION >= 0x51
	m_quad = _mm256_fnmadd_pd(s, b.m_quad, a.m_quad);
#else
	m_quad = _mm256_sub_pd( a.m_quad, _mm256_mul_pd( s, b.m_quad) );
#endif
}

HK_FORCE_INLINE void hkVector4::setAddMul(hkVector4Parameter a, hkVector4Parameter b, hkSimdRealParameter r)
{
	const hkQuadReal sr = _mm256_castpd128_pd256(r.m_real); // reinterpret lower register half
	const hkQuadReal s = _mm256_permute2f128_pd(sr, sr, 0x00);
#if HK_SSE_VERSION >= 0x51
	m_quad = _mm256_fmadd_pd(s, b.m_quad, a.m_quad);
#else
	m_quad = _mm256_add_pd( a.m_quad, _mm256_mul_pd( s, b.m_quad) );
#endif
}

HK_FORCE_INLINE void hkVector4::setAddMul(hkVector4Parameter a, hkVector4Parameter x, hkVector4Parameter y)
{
#if HK_SSE_VERSION >= 0x51
	m_quad = _mm256_fmadd_pd(y.m_quad, x.m_quad, a.m_quad);
#else
	m_quad = _mm256_add_pd( a.m_quad, _mm256_mul_pd( x.m_quad, y.m_quad) );
#endif
}

HK_FORCE_INLINE void hkVector4::setSubMul(hkVector4Parameter a, hkVector4Parameter x, hkVector4Parameter y)
{
#if HK_SSE_VERSION >= 0x51
	m_quad = _mm256_fnmadd_pd(y.m_quad, x.m_quad, a.m_quad);
#else
	m_quad = _mm256_sub_pd( a.m_quad, _mm256_mul_pd( x.m_quad, y.m_quad) );
#endif
}

HK_FORCE_INLINE void hkVector4::setCross( hkVector4Parameter v0, hkVector4Parameter v1 )
{
	// x = y0z1 - z0y1
	const hkQuadReal v0yxzw = _mm256_permute_pd(v0.m_quad, 0x9);
	const hkQuadReal v1yxzw = _mm256_permute_pd(v1.m_quad, 0x9);
	const hkQuadReal v1zwyx = _mm256_permute2f128_pd(v1yxzw, v1yxzw, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH));

	const hkQuadReal x2 = _mm256_mul_pd(v0yxzw, v1zwyx);
	const hkQuadReal x  = _mm256_sub_pd(x2, _mm256_permute2f128_pd(x2, x2, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)));

	// y = z0x1 - x0z1
	const hkQuadReal v0zwxy = _mm256_permute2f128_pd(v0.m_quad, v0.m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH));

	const hkQuadReal y2 = _mm256_mul_pd(v0zwxy, v1.m_quad);
	const hkQuadReal y  = _mm256_sub_pd(y2, _mm256_permute2f128_pd(y2, y2, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)));

	// z = x0y1 - y0x1
	const hkQuadReal z2 = _mm256_mul_pd(v0.m_quad, v1yxzw);
	const hkQuadReal z  = _mm256_sub_pd(z2, _mm256_permute_pd(z2, 0x1));

	m_quad = _mm256_permute2f128_pd(x, z, 0x20);
	m_quad = _mm256_blend_pd(m_quad, y, 0x2);
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::equal(hkVector4Parameter a) const
{
	// workaround VS2010 assembler bug
	const hkQuadReal maskxy =  _mm256_cmp_pd(m_quad, a.m_quad, _CMP_EQ_OQ);
	const hkQuadReal maskzw = _mm256_cmp_pd(_mm256_permute2f128_pd(m_quad,m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_permute2f128_pd(a.m_quad,a.m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _CMP_EQ_OQ);
	const hkQuadReal mask = _mm256_permute2f128_pd(maskxy,maskzw, 0x20);
	return hkVector4Comparison::convert(mask);
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::notEqual(hkVector4Parameter a) const
{
	// workaround VS2010 assembler bug
	const hkQuadReal maskxy =  _mm256_cmp_pd(m_quad, a.m_quad, _CMP_NEQ_OQ);
	const hkQuadReal maskzw = _mm256_cmp_pd(_mm256_permute2f128_pd(m_quad,m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_permute2f128_pd(a.m_quad,a.m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _CMP_NEQ_OQ);
	const hkQuadReal mask = _mm256_permute2f128_pd(maskxy,maskzw, 0x20);
	return hkVector4Comparison::convert(mask);
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::less(hkVector4Parameter a) const
{
	// workaround VS2010 assembler bug
	const hkQuadReal maskxy =  _mm256_cmp_pd(m_quad, a.m_quad, _CMP_LT_OQ);
	const hkQuadReal maskzw = _mm256_cmp_pd(_mm256_permute2f128_pd(m_quad,m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_permute2f128_pd(a.m_quad,a.m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _CMP_LT_OQ);
	const hkQuadReal mask = _mm256_permute2f128_pd(maskxy,maskzw, 0x20);
	return hkVector4Comparison::convert(mask);
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::lessEqual(hkVector4Parameter a) const
{
	// workaround VS2010 assembler bug
	const hkQuadReal maskxy =  _mm256_cmp_pd(m_quad, a.m_quad, _CMP_LE_OQ);
	const hkQuadReal maskzw = _mm256_cmp_pd(_mm256_permute2f128_pd(m_quad,m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_permute2f128_pd(a.m_quad,a.m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _CMP_LE_OQ);
	const hkQuadReal mask = _mm256_permute2f128_pd(maskxy,maskzw, 0x20);
	return hkVector4Comparison::convert(mask);
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::greater(hkVector4Parameter a) const
{
	// workaround VS2010 assembler bug
	const hkQuadReal maskxy =  _mm256_cmp_pd(m_quad, a.m_quad, _CMP_GT_OQ);
	const hkQuadReal maskzw = _mm256_cmp_pd(_mm256_permute2f128_pd(m_quad,m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_permute2f128_pd(a.m_quad,a.m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _CMP_GT_OQ);
	const hkQuadReal mask = _mm256_permute2f128_pd(maskxy,maskzw, 0x20);
	return hkVector4Comparison::convert(mask);
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::greaterEqual(hkVector4Parameter a) const
{
	// workaround VS2010 assembler bug
	const hkQuadReal maskxy =  _mm256_cmp_pd(m_quad, a.m_quad, _CMP_GE_OQ);
	const hkQuadReal maskzw = _mm256_cmp_pd(_mm256_permute2f128_pd(m_quad,m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_permute2f128_pd(a.m_quad,a.m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _CMP_GE_OQ);
	const hkQuadReal mask = _mm256_permute2f128_pd(maskxy,maskzw, 0x20);
	return hkVector4Comparison::convert(mask);
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::lessZero() const
{
	// workaround VS2010 assembler bug
	const hkQuadReal maskxy =  _mm256_cmp_pd(m_quad, _mm256_setzero_pd(), _CMP_LT_OQ);
	const hkQuadReal maskzw = _mm256_cmp_pd(_mm256_permute2f128_pd(m_quad,m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_pd(), _CMP_LT_OQ);
	const hkQuadReal mask = _mm256_permute2f128_pd(maskxy,maskzw, 0x20);
	return hkVector4Comparison::convert(mask);
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::lessEqualZero() const
{
	// workaround VS2010 assembler bug
	const hkQuadReal maskxy =  _mm256_cmp_pd(m_quad, _mm256_setzero_pd(), _CMP_LE_OQ);
	const hkQuadReal maskzw = _mm256_cmp_pd(_mm256_permute2f128_pd(m_quad,m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_pd(), _CMP_LE_OQ);
	const hkQuadReal mask = _mm256_permute2f128_pd(maskxy,maskzw, 0x20);
	return hkVector4Comparison::convert(mask);
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::greaterZero() const
{
	// workaround VS2010 assembler bug
	const hkQuadReal maskxy =  _mm256_cmp_pd(m_quad, _mm256_setzero_pd(), _CMP_GT_OQ);
	const hkQuadReal maskzw = _mm256_cmp_pd(_mm256_permute2f128_pd(m_quad,m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_pd(), _CMP_GT_OQ);
	const hkQuadReal mask = _mm256_permute2f128_pd(maskxy,maskzw, 0x20);
	return hkVector4Comparison::convert(mask);
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::greaterEqualZero() const
{
	// workaround VS2010 assembler bug
	const hkQuadReal maskxy =  _mm256_cmp_pd(m_quad, _mm256_setzero_pd(), _CMP_GE_OQ);
	const hkQuadReal maskzw = _mm256_cmp_pd(_mm256_permute2f128_pd(m_quad,m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_pd(), _CMP_GE_OQ);
	const hkQuadReal mask = _mm256_permute2f128_pd(maskxy,maskzw, 0x20);
	return hkVector4Comparison::convert(mask);
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::equalZero() const
{
	// workaround VS2010 assembler bug
	const hkQuadReal maskxy =  _mm256_cmp_pd(m_quad, _mm256_setzero_pd(), _CMP_EQ_OQ);
	const hkQuadReal maskzw = _mm256_cmp_pd(_mm256_permute2f128_pd(m_quad,m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_pd(), _CMP_EQ_OQ);
	const hkQuadReal mask = _mm256_permute2f128_pd(maskxy,maskzw, 0x20);
	return hkVector4Comparison::convert(mask);
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::notEqualZero() const
{
	// workaround VS2010 assembler bug
	const hkQuadReal maskxy =  _mm256_cmp_pd(m_quad, _mm256_setzero_pd(), _CMP_NEQ_OQ);
	const hkQuadReal maskzw = _mm256_cmp_pd(_mm256_permute2f128_pd(m_quad,m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_pd(), _CMP_NEQ_OQ);
	const hkQuadReal mask = _mm256_permute2f128_pd(maskxy,maskzw, 0x20);
	return hkVector4Comparison::convert(mask);
}

HK_FORCE_INLINE void hkVector4::setSelect( hkVector4ComparisonParameter comp, hkVector4Parameter trueValue, hkVector4Parameter falseValue )
{
	m_quad = _mm256_blendv_pd(falseValue.m_quad, trueValue.m_quad, comp.m_mask);
}

template <>
HK_FORCE_INLINE void hkVector4::setNeg<2>(hkVector4Parameter v)
{
	static HK_ALIGN32( const hkUint64 negateMask[4] ) = { 0x8000000000000000, 0x8000000000000000, 0x0000000000000000, 0x0000000000000000 }; // todo constant
	m_quad = _mm256_xor_pd(v.m_quad, *(const hkQuadReal*)&negateMask);
}

template <>
HK_FORCE_INLINE void hkVector4::setNeg<3>(hkVector4Parameter v)
{
	static HK_ALIGN32( const hkUint64 negateMask[4] ) = { 0x8000000000000000, 0x8000000000000000, 0x8000000000000000, 0x0000000000000000 }; // todo constant
	m_quad = _mm256_xor_pd(v.m_quad, *(const hkQuadReal*)&negateMask);
}

template <>
HK_FORCE_INLINE void hkVector4::setNeg<4>(hkVector4Parameter v)
{
	static HK_ALIGN32( const hkUint64 negateMask[4] ) = { 0x8000000000000000, 0x8000000000000000, 0x8000000000000000, 0x8000000000000000 }; // todo constant
	m_quad = _mm256_xor_pd(v.m_quad, *(const hkQuadReal*)&negateMask);
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
	m_quad = _mm256_min_pd(a.m_quad, b.m_quad);
}

HK_FORCE_INLINE void hkVector4::setMax(hkVector4Parameter a, hkVector4Parameter b)
{
	m_quad = _mm256_max_pd(a.m_quad, b.m_quad);
}

/* matrix3, rotation, quaternion, transform */

HK_FORCE_INLINE void hkVector4::_setRotatedDir(const hkMatrix3& r, hkVector4Parameter b )
{
	const hkQuadReal c0 = r.getColumn<0>().m_quad;
	const hkQuadReal c1 = r.getColumn<1>().m_quad;
	const hkQuadReal c2 = r.getColumn<2>().m_quad;

	const hkQuadReal b_xxzz = _mm256_permute_pd( b.m_quad, 0x0);
	const hkQuadReal b_yyww = _mm256_permute_pd( b.m_quad, 0xF);

	const hkQuadReal b0 = _mm256_permute2f128_pd(b_xxzz, b_xxzz, 0x00);
	const hkQuadReal b1 = _mm256_permute2f128_pd(b_yyww, b_yyww, 0x00);
	const hkQuadReal b2 = _mm256_permute2f128_pd(b_xxzz, b_xxzz, 0x11);	

	const hkQuadReal r0 = _mm256_mul_pd( c0, b0 );
	const hkQuadReal r1 = _mm256_mul_pd( c1, b1 );
	const hkQuadReal r2 = _mm256_mul_pd( c2, b2 );

	m_quad = _mm256_add_pd( _mm256_add_pd(r0, r1), r2 );
}

HK_FORCE_INLINE void hkVector4::_setRotatedInverseDir(const hkMatrix3& r, hkVector4Parameter b )
{
	hkVector4 c0 = r.getColumn<0>();
	hkVector4 c1 = r.getColumn<1>();
	hkVector4 c2 = r.getColumn<2>();

	HK_TRANSPOSE3(c0,c1,c2);

	const hkQuadReal b_xxzz = _mm256_permute_pd( b.m_quad, 0x0);
	const hkQuadReal b_yyww = _mm256_permute_pd( b.m_quad, 0xF);

	const hkQuadReal b0 = _mm256_permute2f128_pd(b_xxzz, b_xxzz, 0x00);
	const hkQuadReal b1 = _mm256_permute2f128_pd(b_yyww, b_yyww, 0x00);
	const hkQuadReal b2 = _mm256_permute2f128_pd(b_xxzz, b_xxzz, 0x11);	

	const hkQuadReal r0 = _mm256_mul_pd( c0.m_quad, b0 );
	const hkQuadReal r1 = _mm256_mul_pd( c1.m_quad, b1 );
	const hkQuadReal r2 = _mm256_mul_pd( c2.m_quad, b2 );

	m_quad = _mm256_add_pd( _mm256_add_pd(r0, r1), r2 );
}


template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::dot<2>(hkVector4Parameter a) const
{
	const hkQuadReal x2 = _mm256_mul_pd(m_quad,a.m_quad);
	const hkQuadReal hsum = _mm256_hadd_pd(x2,x2); // xy xy zw zw
	return hkSimdReal::convert(_mm256_castpd256_pd128(hsum));
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::dot<3>(hkVector4Parameter a) const
{
	const hkQuadReal x2 = _mm256_mul_pd(m_quad,a.m_quad);
	const hkQuadReal hsum = _mm256_hadd_pd(x2,x2); // xy xy zw zw
	const hkQuadReal z = broadcastScalar<2>(*this); // z z z z
	const hkQuadReal hsumz = _mm256_add_pd(hsum, z); // xyz xyz zzw zzw
	return hkSimdReal::convert(_mm256_castpd256_pd128(hsumz));
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::dot<4>(hkVector4Parameter a) const
{
	const hkQuadReal x2 = _mm256_mul_pd(m_quad,a.m_quad);
	const hkQuadReal hsum = _mm256_hadd_pd(x2,x2); // xy xy zw zw
	const hkQuadReal hsuminv = _mm256_permute2f128_pd(hsum, hsum, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)); // zw zw xy xy
	return hkSimdReal::convert(_mm256_castpd256_pd128(_mm256_add_pd(hsum,hsuminv))); // xyzw xyzw xyzw xyzw
}

template <int N>
HK_FORCE_INLINE const hkSimdReal hkVector4::dot(hkVector4Parameter a) const
{
	HK_VECTOR4_NOT_IMPLEMENTED;
	return hkSimdReal::getConstant<HK_QUADREAL_0>();
}


HK_FORCE_INLINE const hkSimdReal hkVector4::dot4xyz1(hkVector4Parameter a) const
{
	const hkQuadReal xx2 = _mm256_mul_pd(m_quad,a.m_quad);
	const hkQuadReal x2 = HK_VECTOR4_COMBINE_XYZ_W(xx2, m_quad);	// replace w by this.w
	const hkQuadReal hsum = _mm256_hadd_pd(x2,x2); // xy xy zw zw
	const hkQuadReal hsuminv = _mm256_permute2f128_pd(hsum, hsum, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)); // zw zw xy xy
	return hkSimdReal::convert(_mm256_castpd256_pd128(_mm256_add_pd(hsum,hsuminv))); // xyzw xyzw xyzw xyzw
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::horizontalAdd<2>() const
{
	const hkQuadReal x2 = _mm256_hadd_pd(m_quad, m_quad); // xy xy zw zw
	return hkSimdReal::convert(_mm256_castpd256_pd128(x2));
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::horizontalAdd<3>() const
{
	const hkQuadReal x2 = _mm256_hadd_pd(m_quad, m_quad); // xy xy zw zw
	const hkQuadReal z = broadcastScalar<2>(*this); // z z z z
	const hkQuadReal hsumz = _mm256_add_pd(x2, z); // xyz xyz zzw zzw
	return hkSimdReal::convert(_mm256_castpd256_pd128(hsumz));
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::horizontalAdd<4>() const
{
	const hkQuadReal hsum = _mm256_hadd_pd(m_quad, m_quad); // xy xy zw zw
	const hkQuadReal hsuminv = _mm256_permute2f128_pd(hsum, hsum, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)); // zw zw xy xy
	return hkSimdReal::convert(_mm256_castpd256_pd128(_mm256_add_pd(hsum,hsuminv))); // xyzw xyzw xyzw xyzw
}

template <int N>
HK_FORCE_INLINE const hkSimdReal hkVector4::horizontalAdd() const
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
	const hkQuadReal yx = _mm256_permute_pd(m_quad, 0x1);
	return hkSimdReal::convert(_mm256_castpd256_pd128(_mm256_min_pd( m_quad, yx )));
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::horizontalMin<3>() const
{
	const hkQuadReal xyzz = _mm256_permute_pd(m_quad, 0x2);
	const hkQuadReal yxzz = _mm256_permute_pd(m_quad, 0x1);
	const hkQuadReal m = _mm256_min_pd( xyzz, yxzz );           // min (x,y) (y,x) (z,z) (z,z)
	const hkQuadReal minv = _mm256_permute2f128_pd(m, m, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)); // min (z,z) (z,z) (x,y) (y,x)
	const hkQuadReal mall = _mm256_min_pd( m, minv );           // min (x,y,z,z) all 4
	return hkSimdReal::convert(_mm256_castpd256_pd128(mall));
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::horizontalMin<4>() const
{
	const hkQuadReal yxwz = _mm256_permute_pd(m_quad, 0x5);
	const hkQuadReal m = _mm256_min_pd( m_quad, yxwz );         // min (x,y) (y,x) (z,w) (w,z)
	const hkQuadReal minv = _mm256_permute2f128_pd(m, m, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)); // min (z,w) (w,z) (x,y) (y,x)
	const hkQuadReal mall = _mm256_min_pd( m, minv );           // min (x,y,z,w) all 4
	return hkSimdReal::convert(_mm256_castpd256_pd128(mall));
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
	return m_quad.m256d_f64[a];
}

HK_FORCE_INLINE const hkReal& hkVector4::operator() (int a) const
{
	HK_ASSERT2(0x6d0c31d7, a>=0 && a<4, "index out of bounds for component access");
	return m_quad.m256d_f64[a];
}

HK_FORCE_INLINE void hkVector4::setXYZ_W(hkVector4Parameter xyz, hkVector4Parameter w)
{
	m_quad = HK_VECTOR4_COMBINE_XYZ_W(xyz.m_quad, w.m_quad);
}

HK_FORCE_INLINE void hkVector4::setXYZ_W(hkVector4Parameter xyz, hkSimdRealParameter w)
{
	const hkQuadReal sr = _mm256_castpd128_pd256(w.m_real); // reinterpret lower register half
	const hkQuadReal s = _mm256_permute2f128_pd(sr, sr, 0x00);
	m_quad = HK_VECTOR4_COMBINE_XYZ_W(xyz.m_quad, s);
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
	m_quad = _mm256_add_pd(m_quad, xyz.m_quad);
	HK_ON_DEBUG( *(hkUint64*)&(m_quad.m256d_f64[3]) = 0xffffffffffffffff; )
}

HK_FORCE_INLINE void hkVector4::subXYZ(hkVector4Parameter xyz)
{
	m_quad = _mm256_sub_pd(m_quad, xyz.m_quad);
	HK_ON_DEBUG( *(hkUint64*)&(m_quad.m256d_f64[3]) = 0xffffffffffffffff; )
}

HK_FORCE_INLINE void hkVector4::setXYZ(hkReal v)
{
	const hkQuadReal q = _mm256_set1_pd(v);
	m_quad = HK_VECTOR4_COMBINE_XYZ_W( q, m_quad );
}

HK_FORCE_INLINE void hkVector4::setXYZ(hkSimdRealParameter v)
{
	const hkQuadReal sr = _mm256_castpd128_pd256(v.m_real); // reinterpret lower register half
	const hkQuadReal s = _mm256_permute2f128_pd(sr, sr, 0x00);
	m_quad = HK_VECTOR4_COMBINE_XYZ_W( s, m_quad );
}

HK_FORCE_INLINE void hkVector4::setXYZ_0(hkVector4Parameter xyz)
{
	static HK_ALIGN32( const hkUint64 cw[4] ) = { 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0x0000000000000000 };
	m_quad = _mm256_and_pd( xyz.m_quad, *(const hkQuadReal*)&cw );
}

HK_FORCE_INLINE void hkVector4::setBroadcastXYZ(const int i, hkVector4Parameter v)
{
	setBroadcast(i,v);
	HK_ON_DEBUG( *(hkUint64*)&(m_quad.m256d_f64[3]) = 0xffffffffffffffff; )
}



HK_FORCE_INLINE void hkVector4::setComponent(const int i, hkSimdRealParameter val)
{
	static HK_ALIGN32 (const hkUint64 indexToMask[4*4]) = 
	{
		0xffffffffffffffff, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
		0x0000000000000000, 0xffffffffffffffff, 0x0000000000000000, 0x0000000000000000,
		0x0000000000000000, 0x0000000000000000, 0xffffffffffffffff, 0x0000000000000000,
		0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0xffffffffffffffff
	};

	HK_ASSERT2(0x6d0c31d7, i>=0 && i<4, "index out of bounds for component access");

	const hkQuadReal mask = *(const hkQuadReal*)&indexToMask[ i * 4 ];
	const hkQuadReal sr = _mm256_castpd128_pd256(val.m_real); // reinterpret lower register half
	const hkQuadReal s = _mm256_permute2f128_pd(sr, sr, 0x00);

	m_quad = _mm256_blendv_pd(m_quad, s, mask);
}

template <int I>
HK_FORCE_INLINE void hkVector4::setComponent(hkSimdRealParameter val)
{
	HK_VECTOR4_SUBINDEX_CHECK;
	const hkQuadReal sr = _mm256_castpd128_pd256(val.m_real); // reinterpret lower register half
	const hkQuadReal s = _mm256_permute2f128_pd(sr, sr, 0x00);
	m_quad = _mm256_blend_pd(m_quad, s, 0x1 << I);
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::horizontalMax<1>() const
{
	return getComponent<0>();
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::horizontalMax<2>() const
{
	const hkQuadReal yx = _mm256_permute_pd(m_quad, 0x1);
	return hkSimdReal::convert(_mm256_castpd256_pd128(_mm256_max_pd( m_quad, yx )));
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::horizontalMax<3>() const
{
	const hkQuadReal xyzz = _mm256_permute_pd(m_quad, 0x2);
	const hkQuadReal yxzz = _mm256_permute_pd(m_quad, 0x1);
	const hkQuadReal m = _mm256_max_pd( xyzz, yxzz );           // max (x,y) (y,x) (z,z) (z,z)
	const hkQuadReal minv = _mm256_permute2f128_pd(m, m, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)); // max (z,z) (z,z) (x,y) (y,x)
	const hkQuadReal mall = _mm256_max_pd( m, minv );           // max (x,y,z,z) all 4
	return hkSimdReal::convert(_mm256_castpd256_pd128(mall));
}

template <>
HK_FORCE_INLINE const hkSimdReal hkVector4::horizontalMax<4>() const
{
	const hkQuadReal yxwz = _mm256_permute_pd(m_quad, 0x5);
	const hkQuadReal m = _mm256_max_pd( m_quad, yxwz );         // max (x,y) (y,x) (z,w) (w,z)
	const hkQuadReal minv = _mm256_permute2f128_pd(m, m, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)); // max (z,w) (w,z) (x,y) (y,x)
	const hkQuadReal mall = _mm256_max_pd( m, minv );           // max (x,y,z,w) all 4
	return hkSimdReal::convert(_mm256_castpd256_pd128(mall));
}

template <int N>
HK_FORCE_INLINE const hkSimdReal hkVector4::horizontalMax() const
{
	HK_VECTOR4_NOT_IMPLEMENTED;
	return hkSimdReal::getConstant<HK_QUADREAL_0>();
}


template <>
HK_FORCE_INLINE void hkVector4::loadNotAligned<4>(const hkReal* p)
{
	m_quad = _mm256_loadu_pd(p);
}

template <>
HK_FORCE_INLINE void hkVector4::loadNotAligned<3>(const hkReal* p)
{
	m_quad = _mm256_setr_pd(p[0], p[1], p[2], 0);
	HK_ON_DEBUG( *(hkUint64*)&(m_quad.m256d_f64[3]) = 0xffffffffffffffff; )
}

template <>
HK_FORCE_INLINE void hkVector4::loadNotAligned<2>(const hkReal* p)
{
	m_quad = _mm256_setr_pd(p[0], p[1], 0, 0);
	HK_ON_DEBUG( *(hkUint64*)&(m_quad.m256d_f64[2]) = 0xffffffffffffffff; *(hkUint64*)&(m_quad.m256d_f64[3]) = 0xffffffffffffffff; )
}

template <>
HK_FORCE_INLINE void hkVector4::loadNotAligned<1>(const hkReal* p)
{
	m_quad = _mm256_set1_pd(p[0]);
	HK_ON_DEBUG( *(hkUint64*)&(m_quad.m256d_f64[1]) = 0xffffffffffffffff; )
	HK_ON_DEBUG( *(hkUint64*)&(m_quad.m256d_f64[2]) = 0xffffffffffffffff; *(hkUint64*)&(m_quad.m256d_f64[3]) = 0xffffffffffffffff; )
}

template <int N>
HK_FORCE_INLINE void hkVector4::loadNotAligned(const hkReal* p)
{
	HK_VECTOR4_NOT_IMPLEMENTED;
}

template <>
HK_FORCE_INLINE void hkVector4::loadNotAligned<4>(const hkHalf* p)
{
	__m128i fourhalfs = _mm_loadl_epi64((const __m128i*)p);
	__m128  fourfloats = _mm_castsi128_ps( _mm_unpacklo_epi16(_mm_setzero_si128(), fourhalfs) );
	m_quad = _mm256_cvtps_pd(fourfloats);
}

template <>
HK_FORCE_INLINE void hkVector4::loadNotAligned<2>(const hkHalf* p)
{
	__m128i twohalfs = _mm_castps_si128( _mm_load_ss((const float*)p) );
	__m128  twofloats = _mm_castsi128_ps( _mm_unpacklo_epi16(_mm_setzero_si128(), twohalfs) );
	m_quad = _mm256_cvtps_pd(twofloats);
	HK_ON_DEBUG( *(hkUint64*)&(m_quad.m256d_f64[2]) = 0xffffffffffffffff; *(hkUint64*)&(m_quad.m256d_f64[3]) = 0xffffffffffffffff; )
}

template <int N>
HK_FORCE_INLINE void hkVector4::loadNotAligned(const hkHalf* p)
{
	HK_VECTOR4_NOT_IMPLEMENTED;
}

template <>
HK_FORCE_INLINE void hkVector4::storeNotAligned<4>(hkReal* p) const
{
	_mm256_storeu_pd(p, m_quad);
}

template <>
HK_FORCE_INLINE void hkVector4::storeNotAligned<3>(hkReal* p) const
{
	static HK_ALIGN32( const hkUint64 mask[4] ) = { 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0x0000000000000000 };
	_mm256_maskstore_pd(p, *(__m256i*)&mask, m_quad);
}

template <>
HK_FORCE_INLINE void hkVector4::storeNotAligned<2>(hkReal* p) const
{
	static HK_ALIGN32( const hkUint64 mask[4] ) = { 0xffffffffffffffff, 0xffffffffffffffff, 0x0000000000000000, 0x0000000000000000 };
	_mm256_maskstore_pd(p, *(__m256i*)&mask, m_quad);
}

template <>
HK_FORCE_INLINE void hkVector4::storeNotAligned<1>(hkReal* p) const
{
	static HK_ALIGN32( const hkUint64 mask[4] ) = { 0xffffffffffffffff, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000 };
	_mm256_maskstore_pd(p, *(__m256i*)&mask, m_quad);
}

template <int N>
HK_FORCE_INLINE void hkVector4::storeNotAligned(hkReal* p) const
{
	HK_VECTOR4_NOT_IMPLEMENTED;
}

template <>
HK_FORCE_INLINE void hkVector4::storeNotAligned<4>(hkHalf* p) const
{
	__m128 xyzw = _mm256_cvtpd_ps(_mm256_mul_pd(m_quad,g_vectorConstants[HK_QUADREAL_PACK_HALF]));
	__m128i tmp0 = _mm_srai_epi32( _mm_castps_si128(xyzw), 16 );
	__m128i tmp1 = _mm_packs_epi32(tmp0, tmp0);
	_mm_storel_epi64((__m128i*) p, tmp1);
}

template <>
HK_FORCE_INLINE void hkVector4::storeNotAligned<2>(hkHalf* p) const
{
	__m128 xy = _mm256_cvtpd_ps(_mm256_mul_pd(m_quad,g_vectorConstants[HK_QUADREAL_PACK_HALF]));
	__m128i tmp0 = _mm_srai_epi32( _mm_castps_si128(xy), 16 );
	__m128i tmp1 = _mm_packs_epi32(tmp0, tmp0);
	_mm_store_ss((float*)p, _mm_castsi128_ps(tmp1));
}

template <int N>
HK_FORCE_INLINE void hkVector4::storeNotAligned(hkHalf* p) const
{
	HK_VECTOR4_NOT_IMPLEMENTED;
}

template <>
HK_FORCE_INLINE void hkVector4::storeNotAlignedNotRounded<4>(hkHalf* p) const
{
	__m128 xyzw = _mm256_cvtpd_ps(m_quad);
	__m128i tmp0 = _mm_srai_epi32( _mm_castps_si128(xyzw), 16 );
	__m128i tmp1 = _mm_packs_epi32(tmp0, tmp0);
	_mm_storel_epi64((__m128i*) p, tmp1);
}

template <>
HK_FORCE_INLINE void hkVector4::storeNotAlignedNotRounded<2>(hkHalf* p) const
{
	__m128 xy = _mm256_cvtpd_ps(m_quad);
	__m128i tmp0 = _mm_srai_epi32( _mm_castps_si128(xy), 16 );
	__m128i tmp1 = _mm_packs_epi32(tmp0, tmp0);
	_mm_store_ss((float*)p, _mm_castsi128_ps(tmp1));
}

template <int N>
HK_FORCE_INLINE void hkVector4::storeNotAlignedNotRounded(hkHalf* p) const
{
	HK_VECTOR4_NOT_IMPLEMENTED;
}

HK_FORCE_INLINE void hkVector4::reduceToHalfPrecision()
{
	__m128i precisionMask = _mm_set1_epi32(0xffff0000);
	__m128 xyzw = _mm256_cvtpd_ps(m_quad);
	xyzw = _mm_and_ps( xyzw, _mm_castsi128_ps(precisionMask) );
	m_quad = _mm256_cvtps_pd(xyzw);
}

template <>
HK_FORCE_INLINE void hkVector4::load<4>(const hkReal* p)
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & 0x1f ) == 0, "p must be 32-byte aligned.");
	m_quad = _mm256_load_pd(p);
}

template <int N>
HK_FORCE_INLINE void hkVector4::load(const hkReal* p)
{
	HK_ON_DEBUG(const hkUint32 dataSize = sizeof(hkReal)*(N!=3?N:4));
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & (dataSize-1) ) == 0, "p must be aligned.");
	loadNotAligned<N>(p);
}

template <>
HK_FORCE_INLINE void hkVector4::store<4>(hkReal* p) const
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & 0x1f ) == 0, "p must be 32-byte aligned.");
	_mm256_store_pd(p,   m_quad);
}

template <int N>
HK_FORCE_INLINE void hkVector4::store(hkReal* p) const
{
	HK_ON_DEBUG(const hkUint32 dataSize = sizeof(hkReal)*(N!=3?N:4));
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & (dataSize-1) ) == 0, "p must be aligned.");
	storeNotAligned<N>(p);
}


template <> 
HK_FORCE_INLINE void hkVector4::storeNotCached<4>(hkReal* p) const
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & 0x1f ) == 0, "p must be 32-byte aligned.");
	_mm256_stream_pd(p, m_quad);
}

template <int N> 
HK_FORCE_INLINE void hkVector4::storeNotCached(hkReal* p) const
{
	HK_ON_DEBUG(const hkUint32 dataSize = sizeof(hkReal)*(N!=3?N:4));
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & (dataSize-1) ) == 0, "p must be aligned.");
	storeNotAligned<N>(p);
}


template <int N> 
HK_FORCE_INLINE void hkVector4::loadNotCached(const hkReal* p)
{
	HK_ON_DEBUG(const hkUint32 dataSize = sizeof(hkReal)*(N!=3?N:4));
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)p) & (dataSize-1) ) == 0, "p must be aligned.");
	loadNotAligned<N>(p);
}


template <> 
HK_FORCE_INLINE hkBool32 hkVector4::isOk<1>() const
{
	// workaround VS2010 assembler bug
	const hkQuadReal maskxy = _mm256_cmp_pd(m_quad, _mm256_setzero_pd(), _CMP_UNORD_Q);
	const hkQuadReal maskzw = _mm256_cmp_pd(_mm256_permute2f128_pd(m_quad,m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_pd(), _CMP_UNORD_Q);
	const hkQuadReal nanMask = _mm256_permute2f128_pd(maskxy,maskzw, 0x20);
	return !(_mm256_movemask_pd(nanMask) & 0x1);
}

template <> 
HK_FORCE_INLINE hkBool32 hkVector4::isOk<2>() const
{
	// workaround VS2010 assembler bug
	const hkQuadReal maskxy = _mm256_cmp_pd(m_quad, _mm256_setzero_pd(), _CMP_UNORD_Q);
	const hkQuadReal maskzw = _mm256_cmp_pd(_mm256_permute2f128_pd(m_quad,m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_pd(), _CMP_UNORD_Q);
	const hkQuadReal nanMask = _mm256_permute2f128_pd(maskxy,maskzw, 0x20);
	return !(_mm256_movemask_pd(nanMask) & 0x3);
}

template <> 
HK_FORCE_INLINE hkBool32 hkVector4::isOk<3>() const
{
	// workaround VS2010 assembler bug
	const hkQuadReal maskxy = _mm256_cmp_pd(m_quad, _mm256_setzero_pd(), _CMP_UNORD_Q);
	const hkQuadReal maskzw = _mm256_cmp_pd(_mm256_permute2f128_pd(m_quad,m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_pd(), _CMP_UNORD_Q);
	const hkQuadReal nanMask = _mm256_permute2f128_pd(maskxy,maskzw, 0x20);
	return !(_mm256_movemask_pd(nanMask) & 0x7);
}

template <> 
HK_FORCE_INLINE hkBool32 hkVector4::isOk<4>() const
{
	// workaround VS2010 assembler bug
	const hkQuadReal maskxy = _mm256_cmp_pd(m_quad, _mm256_setzero_pd(), _CMP_UNORD_Q);
	const hkQuadReal maskzw = _mm256_cmp_pd(_mm256_permute2f128_pd(m_quad,m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_pd(), _CMP_UNORD_Q);
	const hkQuadReal nanMask = _mm256_permute2f128_pd(maskxy,maskzw, 0x20);
	return !(_mm256_movemask_pd(nanMask));
}

template <int N> 
HK_FORCE_INLINE hkBool32 hkVector4::isOk() const
{
	HK_VECTOR4_NOT_IMPLEMENTED;
	return false;
}


template <> 
HK_FORCE_INLINE void hkVector4::setPermutation<hkVectorPermutation::XYXY>(hkVector4Parameter v)
{
	m_quad = _mm256_permute2f128_pd(v.m_quad, v.m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_LOW));
}

template <> 
HK_FORCE_INLINE void hkVector4::setPermutation<hkVectorPermutation::ZWXY>(hkVector4Parameter v)
{
	m_quad = _mm256_permute2f128_pd(v.m_quad, v.m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH));
}

template <> 
HK_FORCE_INLINE void hkVector4::setPermutation<hkVectorPermutation::ZWZW>(hkVector4Parameter v)
{
	m_quad = _mm256_permute2f128_pd(v.m_quad, v.m_quad, _MM256_PERMUTE2(_MM256_A_HIGH, _MM256_A_HIGH));
}

template <> 
HK_FORCE_INLINE void hkVector4::setPermutation<hkVectorPermutation::XXZZ>(hkVector4Parameter v)
{
	m_quad = _mm256_permute_pd(v.m_quad, 0x0);
}

//template <> 
//HK_FORCE_INLINE void hkVector4::setPermutation<hkVectorPermutation::YXZZ>(hkVector4Parameter v)
//{
//	m_quad = _mm256_permute_pd(v.m_quad, 0x1);
//}

template <> 
HK_FORCE_INLINE void hkVector4::setPermutation<hkVectorPermutation::XYZZ>(hkVector4Parameter v)
{
	m_quad = _mm256_permute_pd(v.m_quad, 0x2);
}

//template <> 
//HK_FORCE_INLINE void hkVector4::setPermutation<hkVectorPermutation::YYZZ>(hkVector4Parameter v)
//{
//	m_quad = _mm256_permute_pd(v.m_quad, 0x3);
//}

//template <> 
//HK_FORCE_INLINE void hkVector4::setPermutation<hkVectorPermutation::XXWZ>(hkVector4Parameter v)
//{
//	m_quad = _mm256_permute_pd(v.m_quad, 0x4);
//}

template <> 
HK_FORCE_INLINE void hkVector4::setPermutation<hkVectorPermutation::YXWZ>(hkVector4Parameter v)
{
	m_quad = _mm256_permute_pd(v.m_quad, 0x5);
}

template <> 
HK_FORCE_INLINE void hkVector4::setPermutation<hkVectorPermutation::XYWZ>(hkVector4Parameter v)
{
	m_quad = _mm256_permute_pd(v.m_quad, 0x6);
}

//template <> 
//HK_FORCE_INLINE void hkVector4::setPermutation<hkVectorPermutation::YYWZ>(hkVector4Parameter v)
//{
//	m_quad = _mm256_permute_pd(v.m_quad, 0x7);
//}

//template <> 
//HK_FORCE_INLINE void hkVector4::setPermutation<hkVectorPermutation::XXZW>(hkVector4Parameter v)
//{
//	m_quad = _mm256_permute_pd(v.m_quad, 0x8);
//}

template <> 
HK_FORCE_INLINE void hkVector4::setPermutation<hkVectorPermutation::YXZW>(hkVector4Parameter v)
{
	m_quad = _mm256_permute_pd(v.m_quad, 0x9);
}

template <> 
HK_FORCE_INLINE void hkVector4::setPermutation<hkVectorPermutation::XYZW>(hkVector4Parameter v)
{
	m_quad = v.m_quad; // _mm256_permute_pd(v.m_quad, 0xA);
}

//template <> 
//HK_FORCE_INLINE void hkVector4::setPermutation<hkVectorPermutation::YYZW>(hkVector4Parameter v)
//{
//	m_quad = _mm256_permute_pd(v.m_quad, 0xB);
//}

//template <> 
//HK_FORCE_INLINE void hkVector4::setPermutation<hkVectorPermutation::XXWW>(hkVector4Parameter v)
//{
//	m_quad = _mm256_permute_pd(v.m_quad, 0xC);
//}

template <> 
HK_FORCE_INLINE void hkVector4::setPermutation<hkVectorPermutation::YXWW>(hkVector4Parameter v)
{
	m_quad = _mm256_permute_pd(v.m_quad, 0xD);
}

template <> 
HK_FORCE_INLINE void hkVector4::setPermutation<hkVectorPermutation::XYWW>(hkVector4Parameter v)
{
	m_quad = _mm256_permute_pd(v.m_quad, 0xE);
}

template <> 
HK_FORCE_INLINE void hkVector4::setPermutation<hkVectorPermutation::YYWW>(hkVector4Parameter v)
{
	m_quad = _mm256_permute_pd(v.m_quad, 0xF);
}

template <hkVectorPermutation::Permutation P> 
HK_FORCE_INLINE void hkVector4::setPermutation(hkVector4Parameter v)
{
	const hkQuadReal X = broadcastScalar<(P>>12)&0x3>(v);
	const hkQuadReal Y = broadcastScalar<(P>>8)&0x3>(v);
	const hkQuadReal Z = broadcastScalar<(P>>4)&0x3>(v);
	const hkQuadReal W = broadcastScalar<(P)&0x3>(v);

	const hkQuadReal xxzz = _mm256_permute2f128_pd(X, Z, 0x20);
	const hkQuadReal yyww = _mm256_permute2f128_pd(Y, W, 0x20);
	m_quad = _mm256_shuffle_pd(xxzz, yyww, _MM256_SHUFFLE(1,0,1,0));
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::signBitSet() const
{
	static HK_ALIGN16( const hkUint64 signmask[2] ) = { 0x8000000000000000, 0x8000000000000000 };
	const __m128d xy = _mm256_extractf128_pd(m_quad, 0);
	const __m128d zw = _mm256_extractf128_pd(m_quad, 1);
	const __m128d signsSetXY = _mm_and_pd(xy, *(const __m128d*)&signmask);
	const __m128d signsSetZW = _mm_and_pd(zw, *(const __m128d*)&signmask);

	const __m128d sxy = _mm_castsi128_pd(_mm_cmpeq_epi64(_mm_castpd_si128(signsSetXY), *(const __m128i*)&signmask));
	const __m128d szw = _mm_castsi128_pd(_mm_cmpeq_epi64(_mm_castpd_si128(signsSetZW), *(const __m128i*)&signmask));

	hkQuadReal q = _mm256_castpd128_pd256(sxy); // reinterpret lower register half
	q = _mm256_insertf128_pd(q, szw, 1);

	return hkVector4Comparison::convert(q);
}

HK_FORCE_INLINE const hkVector4Comparison hkVector4::signBitClear() const
{
	static HK_ALIGN16( const hkUint64 signmask[2] ) = { 0x7fffffffffffffff, 0x7fffffffffffffff };
	const __m128d xy = _mm256_extractf128_pd(m_quad, 0);
	const __m128d zw = _mm256_extractf128_pd(m_quad, 1);
	const __m128d signsSetXY = _mm_and_pd(xy, *(const __m128d*)&signmask);
	const __m128d signsSetZW = _mm_and_pd(zw, *(const __m128d*)&signmask);

	const __m128d sxy = _mm_castsi128_pd(_mm_cmpeq_epi64(_mm_castpd_si128(signsSetXY), _mm_castpd_si128(xy)));
	const __m128d szw = _mm_castsi128_pd(_mm_cmpeq_epi64(_mm_castpd_si128(signsSetZW), _mm_castpd_si128(zw)));

	hkQuadReal q = _mm256_castpd128_pd256(sxy); // reinterpret lower register half
	q = _mm256_insertf128_pd(q, szw, 1);

	return hkVector4Comparison::convert(q);
}

HK_FORCE_INLINE void hkVector4::setFlipSign(hkVector4Parameter v, hkVector4ComparisonParameter mask)
{
	static HK_ALIGN32( const hkUint64 signmask[4] ) = { 0x8000000000000000, 0x8000000000000000, 0x8000000000000000, 0x8000000000000000 };
	m_quad = _mm256_xor_pd(v.m_quad, _mm256_and_pd(mask.m_mask, *(const __m256d*)&signmask));
}

HK_FORCE_INLINE void hkVector4::setFlipSign(hkVector4Parameter v, hkVector4Parameter vSign)
{
	static HK_ALIGN32( const hkUint64 signmask[4] ) = { 0x8000000000000000, 0x8000000000000000, 0x8000000000000000, 0x8000000000000000 };
	m_quad = _mm256_xor_pd(v.m_quad, _mm256_and_pd(vSign.m_quad, *(const __m256d*)&signmask));
}

HK_FORCE_INLINE void hkVector4::setFlipSign(hkVector4Parameter v, hkSimdRealParameter sSign)
{
	static HK_ALIGN32( const hkUint64 signmask[4] ) = { 0x8000000000000000, 0x8000000000000000, 0x8000000000000000, 0x8000000000000000 };
	const hkQuadReal sr = _mm256_castpd128_pd256(sSign.m_real); // reinterpret lower register half
	const hkQuadReal s = _mm256_permute2f128_pd(sr, sr, 0x00);
	m_quad = _mm256_xor_pd(v.m_quad, _mm256_and_pd(s, *(const __m256d*)&signmask));
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
				self = hkMath::quadReciprocal(a.m_quad); 
			}
			break;
		case HK_ACC_12_BIT: 
			{
				const __m256 xyzw = _mm256_cvtpd_ps(a.m_quad);
				const __m256 re = _mm256_rcp_ps(xyzw);
				self = _mm256_cvtps_pd(_mm256_castps256_ps128(re));
			}
			break;
		default:		 
			{
				self = _mm256_div_pd(g_vectorConstants[HK_QUADREAL_1], a.m_quad); 
			}
			break; // HK_ACC_FULL
	}
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_SET_ZERO> { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a)
{
	// workaround VS2010 assembler bug
	const hkQuadReal masklow =  _mm256_cmp_pd(a.m_quad, _mm256_setzero_pd(), _CMP_EQ_OQ);
	const hkQuadReal maskhigh = _mm256_cmp_pd(_mm256_permute2f128_pd(a.m_quad,a.m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_pd(), _CMP_EQ_OQ);
	const hkQuadReal equalsZero = _mm256_permute2f128_pd(masklow,maskhigh, 0x20);
	hkQuadReal e; unroll_setReciprocal<A, HK_DIV_IGNORE>::apply(e, a);
	self = _mm256_andnot_pd(equalsZero, e);
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_SET_HIGH> { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a)
{
	// workaround VS2010 assembler bug
	const hkQuadReal masklow =  _mm256_cmp_pd(a.m_quad, _mm256_setzero_pd(), _CMP_EQ_OQ);
	const hkQuadReal maskhigh = _mm256_cmp_pd(_mm256_permute2f128_pd(a.m_quad,a.m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_pd(), _CMP_EQ_OQ);
	const hkQuadReal equalsZero = _mm256_permute2f128_pd(masklow,maskhigh, 0x20);
	hkQuadReal e; unroll_setReciprocal<A, HK_DIV_IGNORE>::apply(e, a);
	const hkQuadReal huge = _mm256_set1_pd(HK_REAL_HIGH);
	self = _mm256_blendv_pd(e, huge, equalsZero);
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_SET_MAX> { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a)
{
	// workaround VS2010 assembler bug
	const hkQuadReal masklow =  _mm256_cmp_pd(a.m_quad, _mm256_setzero_pd(), _CMP_EQ_OQ);
	const hkQuadReal maskhigh = _mm256_cmp_pd(_mm256_permute2f128_pd(a.m_quad,a.m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_pd(), _CMP_EQ_OQ);
	const hkQuadReal equalsZero = _mm256_permute2f128_pd(masklow,maskhigh, 0x20);
	hkQuadReal e; unroll_setReciprocal<A, HK_DIV_IGNORE>::apply(e, a);
	const hkQuadReal huge = _mm256_set1_pd(HK_REAL_MAX);
	self = _mm256_blendv_pd(e, huge, equalsZero);
} };
template <hkMathAccuracyMode A>
struct unroll_setReciprocal<A, HK_DIV_SET_ZERO_AND_ONE> { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a)
{
	unroll_setReciprocal<A, HK_DIV_SET_ZERO>::apply(self, a);
	const hkQuadReal one = g_vectorConstants[HK_QUADREAL_1];
	const hkQuadReal absVal = hkMath::quadFabs(self);
	const hkQuadReal absValLessOne = _mm256_sub_pd(absVal, one);
	// workaround VS2010 assembler bug
	const hkQuadReal masklow =  _mm256_cmp_pd(absValLessOne, g_vectorConstants[HK_QUADREAL_EPS], _CMP_LE_OQ);
	const hkQuadReal maskhigh = _mm256_cmp_pd(_mm256_permute2f128_pd(absValLessOne,absValLessOne, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), g_vectorConstants[HK_QUADREAL_EPS], _CMP_LE_OQ);
	const hkQuadReal lessEqualEps = _mm256_permute2f128_pd(masklow,maskhigh, 0x20);
	self = _mm256_blendv_pd(self, one, lessEqualEps);
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
		case HK_ACC_23_BIT: 
			{
				const hkQuadReal re = hkMath::quadReciprocal(b.m_quad); 
				self = _mm256_mul_pd(a.m_quad,re); 
			}
			break;
		case HK_ACC_12_BIT: 
			{
				const __m256 xyzw = _mm256_cvtpd_ps(a.m_quad);
				const __m256 re = _mm256_rcp_ps(xyzw);
				self = _mm256_mul_pd(a.m_quad,_mm256_cvtps_pd(_mm256_castps256_ps128(re)));
			}
			break;
		default:         
			{
				self = _mm256_div_pd(a.m_quad, b.m_quad); 
			}
			break; // HK_ACC_FULL
	}
} };
template <hkMathAccuracyMode A>
struct unroll_setDiv<A, HK_DIV_SET_ZERO> { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a, hkVector4Parameter b)
{
	// workaround VS2010 assembler bug
	const hkQuadReal masklow =  _mm256_cmp_pd(b.m_quad, _mm256_setzero_pd(), _CMP_EQ_OQ);
	const hkQuadReal maskhigh = _mm256_cmp_pd(_mm256_permute2f128_pd(b.m_quad,b.m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_pd(), _CMP_EQ_OQ);
	const hkQuadReal equalsZero = _mm256_permute2f128_pd(masklow,maskhigh, 0x20);
	hkQuadReal e; unroll_setReciprocal<A, HK_DIV_IGNORE>::apply(e, b);
	self = _mm256_mul_pd(a.m_quad,_mm256_andnot_pd(equalsZero, e));
} };
template <hkMathAccuracyMode A>
struct unroll_setDiv<A, HK_DIV_SET_HIGH> { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a, hkVector4Parameter b)
{
	// workaround VS2010 assembler bug
	const hkQuadReal masklow =  _mm256_cmp_pd(b.m_quad, _mm256_setzero_pd(), _CMP_EQ_OQ);
	const hkQuadReal maskhigh = _mm256_cmp_pd(_mm256_permute2f128_pd(b.m_quad,b.m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_pd(), _CMP_EQ_OQ);
	const hkQuadReal equalsZero = _mm256_permute2f128_pd(masklow,maskhigh, 0x20);
	hkQuadReal e; unroll_setReciprocal<A, HK_DIV_IGNORE>::apply(e, b);
	const hkQuadReal huge = _mm256_set1_pd(HK_REAL_HIGH);
	const hkQuadReal val = _mm256_mul_pd(a.m_quad, e);
	self = _mm256_blendv_pd(val, huge, equalsZero);
} };
template <hkMathAccuracyMode A>
struct unroll_setDiv<A, HK_DIV_SET_MAX> { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a, hkVector4Parameter b)
{
	// workaround VS2010 assembler bug
	const hkQuadReal masklow =  _mm256_cmp_pd(b.m_quad, _mm256_setzero_pd(), _CMP_EQ_OQ);
	const hkQuadReal maskhigh = _mm256_cmp_pd(_mm256_permute2f128_pd(b.m_quad,b.m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_pd(), _CMP_EQ_OQ);
	const hkQuadReal equalsZero = _mm256_permute2f128_pd(masklow,maskhigh, 0x20);
	hkQuadReal e; unroll_setReciprocal<A, HK_DIV_IGNORE>::apply(e, b);
	const hkQuadReal huge = _mm256_set1_pd(HK_REAL_MAX);
	const hkQuadReal val = _mm256_mul_pd(a.m_quad, e);
	self = _mm256_blendv_pd(val, huge, equalsZero);
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
				const hkQuadReal re = hkMath::quadReciprocalSquareRoot(a.m_quad); 
				self = _mm256_mul_pd(a.m_quad,re); 
			}
			break;
		case HK_ACC_12_BIT: 
			{
				const __m256 xyzw = _mm256_cvtpd_ps(a.m_quad);
				const __m256 re = _mm256_rsqrt_ps(xyzw);
				self = _mm256_mul_pd(a.m_quad,_mm256_cvtps_pd(_mm256_castps256_ps128(re)));
			}
			break;
		default:         
			{
				self = _mm256_sqrt_pd(a.m_quad); 
			}
			break; // HK_ACC_FULL
	}
} };
template <hkMathAccuracyMode A>
struct unroll_setSqrt<A, HK_SQRT_SET_ZERO> { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a)
{
	// workaround VS2010 assembler bug
	const hkQuadReal masklow =  _mm256_cmp_pd(a.m_quad, _mm256_setzero_pd(), _CMP_LE_OQ);
	const hkQuadReal maskhigh = _mm256_cmp_pd(_mm256_permute2f128_pd(a.m_quad,a.m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_pd(), _CMP_LE_OQ);
	const hkQuadReal equalsZero = _mm256_permute2f128_pd(masklow,maskhigh, 0x20);
	hkQuadReal e; unroll_setSqrt<A, HK_SQRT_IGNORE>::apply(e,a);
	self = _mm256_andnot_pd(equalsZero, e);
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
			self = hkMath::quadReciprocalSquareRoot(a.m_quad);
		}
		break;
	case HK_ACC_12_BIT: 
		{
			const __m256 xyzw = _mm256_cvtpd_ps(a.m_quad);
			const __m256 re = _mm256_rsqrt_ps(xyzw);
			self = _mm256_cvtps_pd(_mm256_castps256_ps128(re));
		}
		break;
	default:         
		{
			self = _mm256_div_pd(g_vectorConstants[HK_QUADREAL_1], _mm256_sqrt_pd(a.m_quad));
		}
		break; // HK_ACC_FULL
	}
} };
template <hkMathAccuracyMode A>
struct unroll_setSqrtInverse<A, HK_SQRT_SET_ZERO> { HK_FORCE_INLINE static void apply(hkQuadReal& self, hkVector4Parameter a)
{
	// workaround VS2010 assembler bug
	const hkQuadReal masklow =  _mm256_cmp_pd(a.m_quad, _mm256_setzero_pd(), _CMP_LE_OQ);
	const hkQuadReal maskhigh = _mm256_cmp_pd(_mm256_permute2f128_pd(a.m_quad,a.m_quad, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_pd(), _CMP_LE_OQ);
	const hkQuadReal equalsZero = _mm256_permute2f128_pd(masklow,maskhigh, 0x20);
	hkQuadReal e; unroll_setSqrtInverse<A, HK_SQRT_IGNORE>::apply(e,a);
	self = _mm256_andnot_pd(equalsZero, e);
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
