/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#if defined(HK_REAL_IS_DOUBLE) || (HK_SSE_VERSION < 0x50)
#error This implementation is for 32-Bit float with AVX SIMD instruction set
#endif

#include <Common/Base/Math/Vector/Mx/hkMxCommon_AVX.inl>

#define MXR_ONE_OPERAND(METHOD, OP) \
	namespace hkMxReal_Implementation { \
	template <int I> HK_FORCE_INLINE void METHOD##H1R(__m256* r, const __m256* r0) { METHOD##H1R<I-1>(r, r0); r[I-1] = OP(r[I-1], r0[I-1]); } \
	template <> HK_FORCE_INLINE void METHOD##H1R<1>(__m256* r, const __m256* r0) { r[0] = OP(r[0], r0[0]); } \
	} \
	template <int M> HK_FORCE_INLINE void hkMxReal<M>::METHOD(hkMxRealParameter r0) { hkMxReal_Implementation::METHOD##H1R<((M+1)>>1)>(m_real.r, r0.m_real.r); }

#define MXR_TWO_OPERANDS(METHOD, OP) \
	namespace hkMxReal_Implementation { \
	template <int I> HK_FORCE_INLINE void METHOD##HR(__m256* r, const __m256* r0, const __m256* r1) { METHOD##HR<I-1>(r, r0, r1); r[I-1] = OP(r0[I-1], r1[I-1]); } \
	template <> HK_FORCE_INLINE void METHOD##HR<1>(__m256* r, const __m256* r0, const __m256* r1) { r[0] = OP(r0[0], r1[0]); } \
	} \
	template <int M> HK_FORCE_INLINE void hkMxReal<M>::METHOD(hkMxRealParameter r0, hkMxRealParameter r1) { hkMxReal_Implementation::METHOD##HR<((M+1)>>1)>(m_real.r, r0.m_real.r, r1.m_real.r); }

#define MXR_CMP_MXVECTOR(METHOD, OP) \
	namespace hkMxReal_Implementation { \
	template <int I> HK_FORCE_INLINE void METHOD##HRC(const __m256* v0, const __m256* v1, __m256* m) { \
	METHOD##HRC<I-1>(v0, v1, m); \
    /*m[I-1] = _mm256_cmp_ps(v0[I-1], v1[I-1], OP);*/ \
	/* workaround VS2010 assembler bug */ \
	const __m256 masklow =  _mm256_cmp_ps(v0[I-1], v1[I-1], OP); \
	const __m256 maskhigh = _mm256_cmp_ps(_mm256_permute2f128_ps(v0[I-1],v0[I-1], _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_permute2f128_ps(v1[I-1],v1[I-1], _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), OP); \
	m[I-1] = _mm256_permute2f128_ps(masklow,maskhigh, 0x20); } \
	template <> HK_FORCE_INLINE void METHOD##HRC<1>(const __m256* v0, const __m256* v1, __m256* m) { \
	/*m[0] = _mm256_cmp_ps(v0[0], v1[0], OP);*/ \
	/* workaround VS2010 assembler bug */ \
	const __m256 masklow =  _mm256_cmp_ps(v0[0], v1[0], OP); \
	const __m256 maskhigh = _mm256_cmp_ps(_mm256_permute2f128_ps(v0[0],v0[0], _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_permute2f128_ps(v1[0],v1[0], _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), OP); \
	m[0] = _mm256_permute2f128_ps(masklow,maskhigh, 0x20); } \
	} \
	template <int M> HK_FORCE_INLINE void hkMxReal<M>::METHOD(hkMxVectorParameter v, hkMxMask<M>& mask) const { hkMxReal_Implementation::METHOD##HRC<((M+1)>>1)>(m_real.r, v.m_vec.v, mask.m_comp.c); }

#define MXR_CMP_MXREAL(METHOD, OP) \
	namespace hkMxReal_Implementation { \
	template <int I> HK_FORCE_INLINE void METHOD##HRCR(const __m256* v0, const __m256* v1, __m256* m) { \
	METHOD##HRCR<I-1>(v0, v1, m); \
	/*m[I-1] = _mm256_cmp_ps(v0[I-1], v1[I-1], OP);*/ \
	/* workaround VS2010 assembler bug */ \
	const __m256 masklow =  _mm256_cmp_ps(v0[I-1], v1[I-1], OP); \
	const __m256 maskhigh = _mm256_cmp_ps(_mm256_permute2f128_ps(v0[I-1],v0[I-1], _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_permute2f128_ps(v1[I-1],v1[I-1], _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), OP); \
	m[I-1] = _mm256_permute2f128_ps(masklow,maskhigh, 0x20); } \
	template <> HK_FORCE_INLINE void METHOD##HRCR<1>(const __m256* v0, const __m256* v1, __m256* m) { \
	/*m[0] = _mm256_cmp_ps(v0[0], v1[0], OP);*/ \
	/* workaround VS2010 assembler bug */ \
	const __m256 masklow =  _mm256_cmp_ps(v0[0], v1[0], OP); \
	const __m256 maskhigh = _mm256_cmp_ps(_mm256_permute2f128_ps(v0[0],v0[0], _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_permute2f128_ps(v1[0],v1[0], _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), OP); \
	m[0] = _mm256_permute2f128_ps(masklow,maskhigh, 0x20); } \
	} \
	template <int M> HK_FORCE_INLINE void hkMxReal<M>::METHOD(hkMxRealParameter v, hkMxMask<M>& mask) const { hkMxReal_Implementation::METHOD##HRCR<((M+1)>>1)>(m_real.r, v.m_real.r, mask.m_comp.c); }

#define MXR_CMP_MXSINGLE(METHOD, OP) \
	namespace hkMxReal_Implementation { \
	template <int I> HK_FORCE_INLINE void METHOD##HRS(const __m256* v0, const __m256& v1, __m256* m) { \
	METHOD##HRS<I-1>(v0, v1, m); \
	/*m[I-1] = _mm256_cmp_ps(v0[I-1], v1, OP);*/ \
	/* workaround VS2010 assembler bug */ \
	const __m256 masklow =  _mm256_cmp_ps(v0[I-1], v1, OP); \
	const __m256 maskhigh = _mm256_cmp_ps(_mm256_permute2f128_ps(v0[I-1],v0[I-1], _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_permute2f128_ps(v1,v1, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), OP); \
	m[I-1] = _mm256_permute2f128_ps(masklow,maskhigh, 0x20); } \
	template <> HK_FORCE_INLINE void METHOD##HRS<1>(const __m256* v0, const __m256& v1, __m256* m) { \
	/*m[0] = _mm256_cmp_ps(v0[0], v1, OP);*/ \
	/* workaround VS2010 assembler bug */ \
	const __m256 masklow =  _mm256_cmp_ps(v0[0], v1, OP); \
	const __m256 maskhigh = _mm256_cmp_ps(_mm256_permute2f128_ps(v0[0],v0[0], _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_permute2f128_ps(v1,v1, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), OP); \
	m[0] = _mm256_permute2f128_ps(masklow,maskhigh, 0x20); } \
	} \
	template <int M> HK_FORCE_INLINE void hkMxReal<M>::METHOD(hkMxSingleParameter v, hkMxMask<M>& mask) const { hkMxReal_Implementation::METHOD##HRS<((M+1)>>1)>(m_real.r, v.m_single.s, mask.m_comp.c); }



MXR_ONE_OPERAND( add, _mm256_add_ps )
MXR_ONE_OPERAND( sub, _mm256_sub_ps )
MXR_ONE_OPERAND( mul, _mm256_mul_ps )
MXR_ONE_OPERAND( div, _mm256_div_ps )

MXR_TWO_OPERANDS( setAdd, _mm256_add_ps )
MXR_TWO_OPERANDS( setSub, _mm256_sub_ps )
MXR_TWO_OPERANDS( setMul, _mm256_mul_ps )
MXR_TWO_OPERANDS( setDiv, _mm256_div_ps )
MXR_TWO_OPERANDS( setMin, _mm256_min_ps )
MXR_TWO_OPERANDS( setMax, _mm256_max_ps )

MXR_CMP_MXREAL( compareLessThan, _CMP_LT_OQ )
MXR_CMP_MXREAL( compareLessThanEqual, _CMP_LE_OQ )
MXR_CMP_MXREAL( compareGreaterThan, _CMP_GT_OQ )
MXR_CMP_MXREAL( compareGreaterThanEqual, _CMP_GE_OQ )

MXR_CMP_MXVECTOR( compareLessThan, _CMP_LT_OQ )
MXR_CMP_MXVECTOR( compareLessThanEqual, _CMP_LE_OQ )
MXR_CMP_MXVECTOR( compareGreaterThan, _CMP_GT_OQ )
MXR_CMP_MXVECTOR( compareGreaterThanEqual, _CMP_GE_OQ )

MXR_CMP_MXSINGLE( compareLessThan, _CMP_LT_OQ )
MXR_CMP_MXSINGLE( compareLessThanEqual, _CMP_LE_OQ )
MXR_CMP_MXSINGLE( compareGreaterThan, _CMP_GT_OQ )
MXR_CMP_MXSINGLE( compareGreaterThanEqual, _CMP_GE_OQ )



namespace hkMxReal_Implementation
{
template <int I, int N> struct setDotVHR { HK_FORCE_INLINE static void apply(__m256* r, const __m256* v0, const __m256* v1) 
{ 
	setDotVHR<I-1,N>::apply(r, v0, v1); 
	r[I-1] = hkMxCommon_Implementation::dotProdH<N>(v0[I-1], v1[I-1]); 
} };
template <int N> struct setDotVHR<1,N> { HK_FORCE_INLINE static void apply(__m256* r, const __m256* v0, const __m256* v1) 
{ 
	r[0] = hkMxCommon_Implementation::dotProdH<N>(v0[0], v1[0]); 
} };
}
template <int M> 
template <int N> 
HK_FORCE_INLINE void hkMxReal<M>::setDot(hkMxVectorParameter v0, hkMxVectorParameter v1) 
{ 
	hkMxReal_Implementation::setDotVHR<((M+1)>>1), N>::apply(m_real.r, v0.m_vec.v, v1.m_vec.v); 
}

namespace hkMxReal_Implementation
{
template <int I, int N> struct setDotSHR { HK_FORCE_INLINE static void apply(__m256* r, const __m256& r0, const __m256* r1) 
{ 
	setDotSHR<I-1,N>::apply(r, r0, r1); 
	r[I-1] = hkMxCommon_Implementation::dotProdH<N>(r0, r1[I-1]); 
} };
template <int N> struct setDotSHR<1,N> { HK_FORCE_INLINE static void apply(__m256* r, const __m256& r0, const __m256* r1) 
{ 
	r[0] = hkMxCommon_Implementation::dotProdH<N>(r0, r1[0]); 
} };
}
template <int M> 
template <int N> 
HK_FORCE_INLINE void hkMxReal<M>::setDot(hkMxSingleParameter r0, hkMxVectorParameter r1) 
{ 
	hkMxReal_Implementation::setDotSHR<((M+1)>>1), N>::apply(m_real.r, r0.m_single.s, r1.m_vec.v); 
}

template <int M> 
HK_FORCE_INLINE void hkMxReal<M>::setDot4xyz1(hkMxVectorParameter v0, hkMxVectorParameter v1) 
{ 
	hkMxReal<M> one; one.setConstant<HK_QUADREAL_1>();
	hkMxVector<M> v1w; v1w.setXYZ_W(v1, one);
	hkMxReal_Implementation::setDotVHR<((M+1)>>1), 4>::apply(m_real.r, v0.m_vec.v, v1w.m_vec.v); 
}

template <int M> 
HK_FORCE_INLINE void hkMxReal<M>::setDot4xyz1(hkMxSingleParameter r0, hkMxVectorParameter r1) 
{ 
	hkMxReal<M> one; one.setConstant<HK_QUADREAL_1>();
	hkMxVector<M> r1w; r1w.setXYZ_W(r1, one);
	hkMxReal_Implementation::setDotSHR<((M+1)>>1), 4>::apply(m_real.r, r0.m_single.s, r1w.m_vec.v); 
}

#ifndef HK_DISABLE_MATH_CONSTRUCTORS

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void constructHRS(__m256* r, hkReal& v) { constructHRS<I-1>(r, v); r[I-1] = _mm256_broadcast_ss(&v); }
template <> HK_FORCE_INLINE void constructHRS<1>(__m256* r, hkReal& v) { r[0] = _mm256_broadcast_ss(&v); }
}
template <int M>
HK_FORCE_INLINE hkMxReal<M>::hkMxReal(hkReal& v)
{
	hkMxReal_Implementation::constructHRS<((M+1)>>1)>(m_real.r, v);
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void constructHRSS(__m256* r, hkSimdRealParameter v) { constructHRSS<I-1>(r, v); r[I-1] = _mm256_broadcast_ps(&v.m_real); }
template <> HK_FORCE_INLINE void constructHRSS<1>(__m256* r, hkSimdRealParameter v) { r[0] = _mm256_broadcast_ps(&v.m_real); }
}
template <int M>
HK_FORCE_INLINE hkMxReal<M>::hkMxReal(hkSimdRealParameter v)
{
	hkMxReal_Implementation::constructHRSS<((M+1)>>1)>(m_real.r, v);
}

template <> HK_FORCE_INLINE hkMxReal<4>::hkMxReal(hkVector4Parameter v) { const hkReal x = v(0); const hkReal y = v(1); m_real.r[0] = _mm256_setr_ps(x,x,x,x,y,y,y,y); const hkReal z = v(2); const hkReal w = v(3); m_real.r[1] = _mm256_setr_ps(z,z,z,z,w,w,w,w); }
template <> HK_FORCE_INLINE hkMxReal<3>::hkMxReal(hkVector4Parameter v) { const hkReal x = v(0); const hkReal y = v(1); m_real.r[0] = _mm256_setr_ps(x,x,x,x,y,y,y,y); const hkReal z = v(2); m_real.r[1] = _mm256_broadcast_ss(&z); }
template <> HK_FORCE_INLINE hkMxReal<2>::hkMxReal(hkVector4Parameter v) { const hkReal x = v(0); const hkReal y = v(1); m_real.r[0] = _mm256_setr_ps(x,x,x,x,y,y,y,y); }
template <> HK_FORCE_INLINE hkMxReal<1>::hkMxReal(hkVector4Parameter v) { const hkReal x = v(0); m_real.r[0] = _mm256_broadcast_ss(&x); }
template <int M> HK_FORCE_INLINE hkMxReal<M>::hkMxReal(hkVector4Parameter v)
{
	HK_MXREAL_MX_NOT_IMPLEMENTED;
}

#endif

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void moveHR(__m256* HK_RESTRICT v, const __m256* HK_RESTRICT v0) { moveHR<I-1>(v, v0); v[I-1] = v0[I-1]; }
template <> HK_FORCE_INLINE void moveHR<1>(__m256* HK_RESTRICT v, const __m256* HK_RESTRICT v0) { v[0] = v0[0]; }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::operator= ( hkMxRealParameter v )
{
	const int N = (M+1)>>1;
	HK_ASSERT2(0x20baff20, ((hkUlong)m_real.r >= (hkUlong)(v.m_real.r+N)) || ((hkUlong)v.m_real.r >= (hkUlong)(m_real.r+N)), "unsafe move operation");
	hkMxReal_Implementation::moveHR<N>(m_real.r, v.m_real.r);
}


template <> HK_FORCE_INLINE void hkMxReal<4>::setBroadcast( hkVector4Parameter r ) 
{ 
	const hkReal x = r(0); const hkReal y = r(1); m_real.r[0] = _mm256_setr_ps(x,x,x,x,y,y,y,y); 
	const hkReal z = r(2); const hkReal w = r(3); m_real.r[1] = _mm256_setr_ps(z,z,z,z,w,w,w,w); 
}
template <> HK_FORCE_INLINE void hkMxReal<3>::setBroadcast( hkVector4Parameter r ) 
{ 
	const hkReal x = r(0); const hkReal y = r(1); m_real.r[0] = _mm256_setr_ps(x,x,x,x,y,y,y,y); 
	const hkReal z = r(2);                        m_real.r[1] = _mm256_broadcast_ss(&z); 
}
template <> HK_FORCE_INLINE void hkMxReal<2>::setBroadcast( hkVector4Parameter r ) 
{ 
	const hkReal x = r(0); const hkReal y = r(1); m_real.r[0] = _mm256_setr_ps(x,x,x,x,y,y,y,y); 
}
template <> HK_FORCE_INLINE void hkMxReal<1>::setBroadcast( hkVector4Parameter r ) 
{ 
	const hkReal x = r(0); m_real.r[0] = _mm256_broadcast_ss(&x); 
}
template <int M> HK_FORCE_INLINE void hkMxReal<M>::setBroadcast( hkVector4Parameter r )
{
	HK_MXREAL_MX_NOT_IMPLEMENTED;
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void setBCHRS(__m256* v, hkSimdRealParameter v0) { setBCHRS<I-1>(v, v0); v[I-1] = _mm256_broadcast_ps(&v0.m_real); }
template <> HK_FORCE_INLINE void setBCHRS<1>(__m256* v, hkSimdRealParameter v0) { v[0] = _mm256_broadcast_ps(&v0.m_real); }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::setBroadcast( hkSimdRealParameter r )
{
	hkMxReal_Implementation::setBCHRS<((M+1)>>1)>(m_real.r, r);
}

namespace hkMxReal_Implementation
{
template <int I, int N> struct loadHR { HK_FORCE_INLINE static void apply(__m256* v, const hkReal* r) 
{ 
	const hkReal r0 = r[N*2]; const hkReal r1 = r[N*2+1]; 
	v[N] = _mm256_setr_ps(r0,r0,r0,r0,r1,r1,r1,r1); 
	loadHR<I-2,N+1>::apply(v, r); 
} };
template <int N> struct loadHR<2,N> { HK_FORCE_INLINE static void apply(__m256* v, const hkReal* r) 
{ 
	const hkReal r0 = r[N*2]; const hkReal r1 = r[N*2+1]; 
	v[N] = _mm256_setr_ps(r0,r0,r0,r0,r1,r1,r1,r1); 
} };
template <int N> struct loadHR<1,N> { HK_FORCE_INLINE static void apply(__m256* v, const hkReal* r) 
{ 
	v[N] = _mm256_broadcast_ss(r+(N*2)); 
} };
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::load(const hkReal* r)
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)r) & 0x1f ) == 0, "r must be 32-byte aligned."); 
	hkMxReal_Implementation::loadHR<M,0>::apply(m_real.r, r);
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::loadNotAligned(const hkReal* r)
{
	hkMxReal_Implementation::loadHR<M,0>::apply(m_real.r, r);
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::loadNotCached(const hkReal* r)
{
	hkMxReal_Implementation::loadHR<M,0>::apply(m_real.r, r);
}

namespace hkMxReal_Implementation
{
template <int I, int N> struct storeHR { HK_FORCE_INLINE static void apply(const __m256* v, hkReal* r) 
{ 
// not supported by VS2010
//	_mm_extractmem_ps(r+(N*2), _mm256_extractf128_ps(v[N], 0x0), 0x0); 
//	_mm_extractmem_ps(r+(N*2)+1, _mm256_extractf128_ps(v[N], 0x1), 0x0); 
	const __m128 m0 = _mm256_extractf128_ps(v[N], 0x0);
	const __m128 m1 = _mm256_extractf128_ps(v[N], 0x1);
	storeHR<I-2,N+1>::apply(v, r); 
	_mm_store_ss(r+(N*2), m0);
	_mm_store_ss(r+(N*2)+1, m1);
} };
template <int N> struct storeHR<2,N> { HK_FORCE_INLINE static void apply(const __m256* v, hkReal* r) 
{ 
// not supported by VS2010
//	_mm_extractmem_ps(r+(N*2), _mm256_extractf128_ps(v[N], 0x0), 0x0); 
//	_mm_extractmem_ps(r+(N*2)+1, _mm256_extractf128_ps(v[N], 0x1), 0x0); 
	const __m128 m0 = _mm256_extractf128_ps(v[N], 0x0);
	const __m128 m1 = _mm256_extractf128_ps(v[N], 0x1);
	_mm_store_ss(r+(N*2), m0);
	_mm_store_ss(r+(N*2)+1, m1);
} };
template <int N> struct storeHR<1,N> { HK_FORCE_INLINE static void apply(const __m256* v, hkReal* r) 
{ 
// not supported by VS2010
//	_mm_extractmem_ps(r+(N*2), _mm256_extractf128_ps(v[N], 0x0), 0x0); 
	const __m128 m0 = _mm256_extractf128_ps(v[N], 0x0);
	_mm_store_ss(r+(N*2), m0);
} };
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::store(hkReal* rOut) const
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)rOut) & 0x1f ) == 0, "rOut must be 32-byte aligned."); 
	hkMxReal_Implementation::storeHR<M,0>::apply(m_real.r, rOut);
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::storeNotAligned(hkReal* rOut) const
{
	hkMxReal_Implementation::storeHR<M,0>::apply(m_real.r, rOut);
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::storeNotCached(hkReal* rOut) const
{
	hkMxReal_Implementation::storeHR<M,0>::apply(m_real.r, rOut);
}



namespace hkMxReal_Implementation
{
template <int I, int N, hkUint32 byteAddressIncrement> struct gatherHR {
HK_FORCE_INLINE static void apply(__m256* v, const hkReal* HK_RESTRICT base)
{
	const hkReal r0 = *hkAddByteOffsetConst( base, (N*2)*byteAddressIncrement );
	const hkReal r1 = *hkAddByteOffsetConst( base, (N*2+1)*byteAddressIncrement );
	v[N] = _mm256_setr_ps(r0,r0,r0,r0,r1,r1,r1,r1);
	gatherHR<I-2, N+1, byteAddressIncrement>::apply(v, base);
} };
template <int N, hkUint32 byteAddressIncrement> struct gatherHR<2, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(__m256* v, const hkReal* HK_RESTRICT base)
{
	const hkReal r0 = *hkAddByteOffsetConst( base, (N*2)*byteAddressIncrement );
	const hkReal r1 = *hkAddByteOffsetConst( base, (N*2+1)*byteAddressIncrement );
	v[N] = _mm256_setr_ps(r0,r0,r0,r0,r1,r1,r1,r1);
} };
template <int N, hkUint32 byteAddressIncrement> struct gatherHR<1, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(__m256* v, const hkReal* HK_RESTRICT base)
{
	v[N] = _mm256_broadcast_ss(hkAddByteOffsetConst( base, (N*2)*byteAddressIncrement ));
} };
}
template <int M>
template <hkUint32 byteAddressIncrement>
HK_FORCE_INLINE void hkMxReal<M>::gather(const hkReal* HK_RESTRICT base)
{
	hkMxReal_Implementation::gatherHR<M, 0, byteAddressIncrement>::apply(m_real.r, base);
}

namespace hkMxReal_Implementation
{
template <int I, int N, hkUint32 byteAddressIncrement> struct gatherUintHR {
HK_FORCE_INLINE static void apply(__m256* v, const hkReal* HK_RESTRICT base, const hkUint16* indices)
{
	const hkReal r0 = *hkAddByteOffsetConst( base, indices[N*2]*byteAddressIncrement );
	const hkReal r1 = *hkAddByteOffsetConst( base, indices[N*2+1]*byteAddressIncrement );
	v[N] = _mm256_setr_ps(r0,r0,r0,r0,r1,r1,r1,r1);
	gatherUintHR<I-2, N+1, byteAddressIncrement>::apply(v, base, indices);
} };
template <int N, hkUint32 byteAddressIncrement> struct gatherUintHR<2, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(__m256* v, const hkReal* HK_RESTRICT base, const hkUint16* indices)
{
	const hkReal r0 = *hkAddByteOffsetConst( base, indices[N*2]*byteAddressIncrement );
	const hkReal r1 = *hkAddByteOffsetConst( base, indices[N*2+1]*byteAddressIncrement );
	v[N] = _mm256_setr_ps(r0,r0,r0,r0,r1,r1,r1,r1);
} };
template <int N, hkUint32 byteAddressIncrement> struct gatherUintHR<1, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(__m256* v, const hkReal* HK_RESTRICT base, const hkUint16* indices)
{
	v[N] = _mm256_broadcast_ss(hkAddByteOffsetConst( base, indices[N*2]*byteAddressIncrement ));
} };
}
template <int M>
template <hkUint32 byteAddressIncrement>
HK_FORCE_INLINE void hkMxReal<M>::gather(const hkReal* HK_RESTRICT base, const hkUint16* indices)
{
	hkMxReal_Implementation::gatherUintHR<M, 0, byteAddressIncrement>::apply(m_real.r, base, indices);
}

namespace hkMxReal_Implementation
{
template <int I, int N, hkUint32 byteAddressIncrement> struct gatherIntHR {
HK_FORCE_INLINE static void apply(__m256* v, const hkReal* HK_RESTRICT base, const hkInt32* indices)
{
	const hkReal r0 = *hkAddByteOffsetConst( base, indices[N*2]*byteAddressIncrement );
	const hkReal r1 = *hkAddByteOffsetConst( base, indices[N*2+1]*byteAddressIncrement );
	v[N] = _mm256_setr_ps(r0,r0,r0,r0,r1,r1,r1,r1);
	gatherIntHR<I-2, N+1, byteAddressIncrement>::apply(v, base, indices);
} };
template <int N, hkUint32 byteAddressIncrement> struct gatherIntHR<2, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(__m256* v, const hkReal* HK_RESTRICT base, const hkInt32* indices)
{
	const hkReal r0 = *hkAddByteOffsetConst( base, indices[N*2]*byteAddressIncrement );
	const hkReal r1 = *hkAddByteOffsetConst( base, indices[N*2+1]*byteAddressIncrement );
	v[N] = _mm256_setr_ps(r0,r0,r0,r0,r1,r1,r1,r1);
} };
template <int N, hkUint32 byteAddressIncrement> struct gatherIntHR<1, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(__m256* v, const hkReal* HK_RESTRICT base, const hkInt32* indices)
{
	v[N] = _mm256_broadcast_ss(hkAddByteOffsetConst( base, indices[N*2]*byteAddressIncrement ));
} };
}
template <int M>
template <hkUint32 byteAddressIncrement>
HK_FORCE_INLINE void hkMxReal<M>::gather(const hkReal* HK_RESTRICT base, const hkInt32* indices)
{
	hkMxReal_Implementation::gatherIntHR<M, 0, byteAddressIncrement>::apply(m_real.r, base, indices);
}

namespace hkMxReal_Implementation
{
template <int I, int N, hkUint32 byteOffset> struct gatherWithOffsetHR {
HK_FORCE_INLINE static void apply(__m256* v, const void** base)
{
	const hkReal* HK_RESTRICT ptr0 = (const hkReal*)hkAddByteOffsetConst(base[N*2], byteOffset); 
	const hkReal* HK_RESTRICT ptr1 = (const hkReal*)hkAddByteOffsetConst(base[N*2+1], byteOffset); 
	gatherWithOffsetHR<I-2, N+1, byteOffset>::apply(v, base);
	const hkReal r0 = *ptr0;
	const hkReal r1 = *ptr1;
	v[N] = _mm256_setr_ps(r0,r0,r0,r0,r1,r1,r1,r1);
} };
template <int N, hkUint32 byteOffset> struct gatherWithOffsetHR<2, N, byteOffset> {
HK_FORCE_INLINE static void apply(__m256* v, const void** base)
{
	const hkReal* HK_RESTRICT ptr0 = (const hkReal*)hkAddByteOffsetConst(base[N*2], byteOffset); 
	const hkReal* HK_RESTRICT ptr1 = (const hkReal*)hkAddByteOffsetConst(base[N*2+1], byteOffset); 
	const hkReal r0 = *ptr0;
	const hkReal r1 = *ptr1;
	v[N] = _mm256_setr_ps(r0,r0,r0,r0,r1,r1,r1,r1);
} };
template <int N, hkUint32 byteOffset> struct gatherWithOffsetHR<1, N, byteOffset> {
HK_FORCE_INLINE static void apply(__m256* v, const void** base)
{
	const hkReal* HK_RESTRICT ptr = (const hkReal*)hkAddByteOffsetConst(base[N*2], byteOffset); 
	v[N] = _mm256_broadcast_ss(ptr);
} };
}
template <int M>
template <hkUint32 byteOffset> 
HK_FORCE_INLINE void hkMxReal<M>::gatherWithOffset(const void* base[M])
{
	hkMxReal_Implementation::gatherWithOffsetHR<M, 0, byteOffset>::apply(m_real.r, base);
}




namespace hkMxReal_Implementation
{
template <int I, int N, hkUint32 byteAddressIncrement> struct scatterHR {
HK_FORCE_INLINE static void apply(const __m256* v, hkReal* base)
{
	hkReal* HK_RESTRICT ptr0 = hkAddByteOffset(base, (N*2) * byteAddressIncrement);
	hkReal* HK_RESTRICT ptr1 = hkAddByteOffset(base, (N*2+1) * byteAddressIncrement);
// not supported by VS2010
//	_mm_extractmem_ps(ptr0, _mm256_extractf128_ps(v[N], 0x0), 0x0);
//	_mm_extractmem_ps(ptr1, _mm256_extractf128_ps(v[N], 0x1), 0x0);
	const __m128 m0 = _mm256_extractf128_ps(v[N], 0x0);
	const __m128 m1 = _mm256_extractf128_ps(v[N], 0x1);
	scatterHR<I-2, N+1, byteAddressIncrement>::apply(v, base);
	_mm_store_ss(ptr0, m0);
	_mm_store_ss(ptr1, m1);
} };
template <int N, hkUint32 byteAddressIncrement> struct scatterHR<2, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const __m256* v, hkReal* base)
{
	hkReal* HK_RESTRICT ptr0 = hkAddByteOffset(base, (N*2) * byteAddressIncrement);
	hkReal* HK_RESTRICT ptr1 = hkAddByteOffset(base, (N*2+1) * byteAddressIncrement);
// not supported by VS2010
//	_mm_extractmem_ps(ptr0, _mm256_extractf128_ps(v[N], 0x0), 0x0);
//	_mm_extractmem_ps(ptr1, _mm256_extractf128_ps(v[N], 0x1), 0x0);
	const __m128 m0 = _mm256_extractf128_ps(v[N], 0x0);
	const __m128 m1 = _mm256_extractf128_ps(v[N], 0x1);
	_mm_store_ss(ptr0, m0);
	_mm_store_ss(ptr1, m1);
} };
template <int N, hkUint32 byteAddressIncrement> struct scatterHR<1, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const __m256* v, hkReal* base)
{
	hkReal* HK_RESTRICT ptr = hkAddByteOffset(base, (N*2) * byteAddressIncrement);
// not supported by VS2010
//	_mm_extractmem_ps(ptr, _mm256_extractf128_ps(v[N], 0x0), 0x0);
	const __m128 m0 = _mm256_extractf128_ps(v[N], 0x0);
	_mm_store_ss(ptr, m0);
} };
}
template <int M>
template <hkUint32 byteAddressIncrement>
HK_FORCE_INLINE void hkMxReal<M>::scatter(hkReal* base) const
{
	hkMxReal_Implementation::scatterHR<M, 0, byteAddressIncrement>::apply(m_real.r, base);
}

namespace hkMxReal_Implementation
{
template <int I, int N, hkUint32 byteAddressIncrement> struct scatterUintHR {
HK_FORCE_INLINE static void apply(const __m256* v, hkReal* base, const hkUint16* indices)
{
	hkReal* HK_RESTRICT ptr0 = hkAddByteOffset(base, indices[N*2] * byteAddressIncrement);
	hkReal* HK_RESTRICT ptr1 = hkAddByteOffset(base, indices[N*2+1] * byteAddressIncrement);
// not supported by VS2010
//	_mm_extractmem_ps(ptr0, _mm256_extractf128_ps(v[N], 0x0), 0x0);
//	_mm_extractmem_ps(ptr1, _mm256_extractf128_ps(v[N], 0x1), 0x0);
	const __m128 m0 = _mm256_extractf128_ps(v[N], 0x0);
	const __m128 m1 = _mm256_extractf128_ps(v[N], 0x1);
	scatterUintHR<I-2, N+1, byteAddressIncrement>::apply(v, base, indices);
	_mm_store_ss(ptr0, m0);
	_mm_store_ss(ptr1, m1);
} };
template <int N, hkUint32 byteAddressIncrement> struct scatterUintHR<2, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const __m256* v, hkReal* base, const hkUint16* indices)
{
	hkReal* HK_RESTRICT ptr0 = hkAddByteOffset(base, indices[N*2] * byteAddressIncrement);
	hkReal* HK_RESTRICT ptr1 = hkAddByteOffset(base, indices[N*2+1] * byteAddressIncrement);
// not supported by VS2010
//	_mm_extractmem_ps(ptr0, _mm256_extractf128_ps(v[N], 0x0), 0x0);
//	_mm_extractmem_ps(ptr1, _mm256_extractf128_ps(v[N], 0x1), 0x0);
	const __m128 m0 = _mm256_extractf128_ps(v[N], 0x0);
	const __m128 m1 = _mm256_extractf128_ps(v[N], 0x1);
	_mm_store_ss(ptr0, m0);
	_mm_store_ss(ptr1, m1);
} };
template <int N, hkUint32 byteAddressIncrement> struct scatterUintHR<1, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const __m256* v, hkReal* base, const hkUint16* indices)
{
	hkReal* HK_RESTRICT ptr = hkAddByteOffset(base, indices[N*2] * byteAddressIncrement);
// not supported by VS2010
//	_mm_extractmem_ps(ptr, _mm256_extractf128_ps(v[N], 0x0), 0x0);
	const __m128 m0 = _mm256_extractf128_ps(v[N], 0x0);
	_mm_store_ss(ptr, m0);
} };
}
template <int M>
template <hkUint32 byteAddressIncrement>
HK_FORCE_INLINE void hkMxReal<M>::scatter(hkReal* base, const hkUint16* indices) const
{
	hkMxReal_Implementation::scatterUintHR<M, 0, byteAddressIncrement>::apply(m_real.r, base, indices);
}

namespace hkMxReal_Implementation
{
template <int I, int N, hkUint32 byteAddressIncrement> struct scatterIntHR {
HK_FORCE_INLINE static void apply(const __m256* v, hkReal* base, const hkInt32* indices)
{
	hkReal* HK_RESTRICT ptr0 = hkAddByteOffset(base, indices[N*2] * byteAddressIncrement);
	hkReal* HK_RESTRICT ptr1 = hkAddByteOffset(base, indices[N*2+1] * byteAddressIncrement);
// not supported by VS2010
//	_mm_extractmem_ps(ptr0, _mm256_extractf128_ps(v[N], 0x0), 0x0);
//	_mm_extractmem_ps(ptr1, _mm256_extractf128_ps(v[N], 0x1), 0x0);
	const __m128 m0 = _mm256_extractf128_ps(v[N], 0x0);
	const __m128 m1 = _mm256_extractf128_ps(v[N], 0x1);
	scatterIntHR<I-2, N+1, byteAddressIncrement>::apply(v, base, indices);
	_mm_store_ss(ptr0, m0);
	_mm_store_ss(ptr1, m1);
} };
template <int N, hkUint32 byteAddressIncrement> struct scatterIntHR<2, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const __m256* v, hkReal* base, const hkInt32* indices)
{
	hkReal* HK_RESTRICT ptr0 = hkAddByteOffset(base, indices[N*2] * byteAddressIncrement);
	hkReal* HK_RESTRICT ptr1 = hkAddByteOffset(base, indices[N*2+1] * byteAddressIncrement);
// not supported by VS2010
//	_mm_extractmem_ps(ptr0, _mm256_extractf128_ps(v[N], 0x0), 0x0);
//	_mm_extractmem_ps(ptr1, _mm256_extractf128_ps(v[N], 0x1), 0x0);
	const __m128 m0 = _mm256_extractf128_ps(v[N], 0x0);
	const __m128 m1 = _mm256_extractf128_ps(v[N], 0x1);
	_mm_store_ss(ptr0, m0);
	_mm_store_ss(ptr1, m1);
} };
template <int N, hkUint32 byteAddressIncrement> struct scatterIntHR<1, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const __m256* v, hkReal* base, const hkInt32* indices)
{
	hkReal* HK_RESTRICT ptr = hkAddByteOffset(base, indices[N*2] * byteAddressIncrement);
// not supported by VS2010
//	_mm_extractmem_ps(ptr, _mm256_extractf128_ps(v[N], 0x0), 0x0);
	const __m128 m0 = _mm256_extractf128_ps(v[N], 0x0);
	_mm_store_ss(ptr, m0);
} };
}
template <int M>
template <hkUint32 byteAddressIncrement>
HK_FORCE_INLINE void hkMxReal<M>::scatter(hkReal* base, const hkInt32* indices) const
{
	hkMxReal_Implementation::scatterIntHR<M, 0, byteAddressIncrement>::apply(m_real.r, base, indices);
}

namespace hkMxReal_Implementation
{
template <int I, int N, hkUint32 byteOffset> struct scatterWithOffsetHR {
HK_FORCE_INLINE static void apply(const __m256* v, void** base)
{
	hkReal* HK_RESTRICT ptr0 = (hkReal*)hkAddByteOffset(base[N*2], byteOffset); 
	hkReal* HK_RESTRICT ptr1 = (hkReal*)hkAddByteOffset(base[N*2+1], byteOffset); 
	const __m128 m0 = _mm256_extractf128_ps(v[N], 0x0);
	const __m128 m1 = _mm256_extractf128_ps(v[N], 0x1);
	scatterWithOffsetHR<I-2, N+1, byteOffset>::apply(v, base);
	_mm_store_ss(ptr0, m0);
	_mm_store_ss(ptr1, m1);
// not supported by VS2010
//	_mm_extractmem_ps(ptr0, _mm256_extractf128_ps(v[N], 0x0), 0x0);
//	_mm_extractmem_ps(ptr1, _mm256_extractf128_ps(v[N], 0x1), 0x0);
} };
template <int N, hkUint32 byteOffset> struct scatterWithOffsetHR<2, N, byteOffset> {
HK_FORCE_INLINE static void apply(const __m256* v, void** base)
{
	hkReal* HK_RESTRICT ptr0 = (hkReal*)hkAddByteOffset(base[N*2], byteOffset); 
	hkReal* HK_RESTRICT ptr1 = (hkReal*)hkAddByteOffset(base[N*2+1], byteOffset); 
// not supported by VS2010
//	_mm_extractmem_ps(ptr0, _mm256_extractf128_ps(v[N], 0x0), 0x0);
//	_mm_extractmem_ps(ptr1, _mm256_extractf128_ps(v[N], 0x1), 0x0);
	const __m128 m0 = _mm256_extractf128_ps(v[N], 0x0);
	const __m128 m1 = _mm256_extractf128_ps(v[N], 0x1);
	_mm_store_ss(ptr0, m0);
	_mm_store_ss(ptr1, m1);
} };
template <int N, hkUint32 byteOffset> struct scatterWithOffsetHR<1, N, byteOffset> {
HK_FORCE_INLINE static void apply(const __m256* v, void** base)
{
	hkReal* HK_RESTRICT ptr = (hkReal*)hkAddByteOffset(base[N*2], byteOffset); 
// not supported by VS2010
//	_mm_extractmem_ps(ptr, _mm256_extractf128_ps(v[N], 0x0), 0x0);
	const __m128 m0 = _mm256_extractf128_ps(v[N], 0x0);
	_mm_store_ss(ptr, m0);
} };
}
template <int M>
template <hkUint32 byteOffset> 
HK_FORCE_INLINE void hkMxReal<M>::scatterWithOffset(void* base[M]) const 
{
	hkMxReal_Implementation::scatterWithOffsetHR<M, 0, byteOffset>::apply(m_real.r, base);
}








template <int M>
template <int I>
HK_FORCE_INLINE void hkMxReal<M>::setReal(hkSimdRealParameter r)
{
	HK_MXVECTOR_MX_SUBINDEX_CHECK;
	const int N = (I+1)>>1;
	const int J = I>>1;
	m_real.r[J] = _mm256_insertf128_ps(m_real.r[J], r.m_real, N-J);
}

template <int M>
template <int I> 
HK_FORCE_INLINE void hkMxReal<M>::getAsBroadcast(hkVector4& vOut) const
{
	HK_MXVECTOR_MX_SUBINDEX_CHECK;
	const int N = (I+1)>>1;
	const int J = I>>1;
	vOut.m_quad = _mm256_extractf128_ps(m_real.r[J], N-J);
}

template <int M>
template <int I> 
HK_FORCE_INLINE const hkVector4 hkMxReal<M>::getAsBroadcast() const
{
	HK_MXVECTOR_MX_SUBINDEX_CHECK;
	hkVector4 h;
	const int N = (I+1)>>1;
	const int J = I>>1;
	h.m_quad = _mm256_extractf128_ps(m_real.r[J], N-J);
	return h;
}

template <int M>
template <int I> 
HK_FORCE_INLINE const hkSimdReal hkMxReal<M>::getReal() const
{
	HK_MXVECTOR_MX_SUBINDEX_CHECK;
	hkSimdReal h;
	const int N = (I+1)>>1;
	const int J = I>>1;
	h.m_real = _mm256_extractf128_ps(m_real.r[J], N-J);
	return h;
}


namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void setConstHR(__m256* v, const __m256& vC) { setConstHR<I-1>(v,vC); v[I-1] = vC; }
template <> HK_FORCE_INLINE void setConstHR<1>(__m256* v, const __m256& vC) { v[0] = vC; }
}
template <int M>
template<int vectorConstant> 
HK_FORCE_INLINE void hkMxReal<M>::setConstant()
{
	const __m256 vC = _mm256_broadcast_ps(g_vectorConstants+vectorConstant);
	hkMxReal_Implementation::setConstHR<((M+1)>>1)>(m_real.r, vC);
}


namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void div12HR(__m256* v, const __m256* v0) { div12HR<I-1>(v, v0); v[I-1] = _mm256_mul_ps(v[I-1], _mm256_rcp_ps(v0[I-1])); }
template <> HK_FORCE_INLINE void div12HR<1>(__m256* v, const __m256* v0) { v[0] = _mm256_mul_ps(v[0], _mm256_rcp_ps(v0[0])); }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::div_12BitAccurate(hkMxRealParameter r)
{
	hkMxReal_Implementation::div12HR<((M+1)>>1)>(m_real.r, r.m_real.r);
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void div23HR(__m256* v, const __m256* v0) { div23HR<I-1>(v, v0); v[I-1] = _mm256_mul_ps(v[I-1], hkMxCommon_Implementation::reciprocalH(v0[I-1])); }
template <> HK_FORCE_INLINE void div23HR<1>(__m256* v, const __m256* v0) { v[0] = _mm256_mul_ps(v[0], hkMxCommon_Implementation::reciprocalH(v0[0])); }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::div_23BitAccurate(hkMxRealParameter r)
{
	hkMxReal_Implementation::div23HR<((M+1)>>1)>(m_real.r, r.m_real.r);
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void setNegHR(__m256* v, const __m256* v0, const __m256& mask) { setNegHR<I-1>(v, v0, mask); v[I-1] = _mm256_xor_ps(v0[I-1],mask); }
template <> HK_FORCE_INLINE void setNegHR<1>(__m256* v, const __m256* v0, const __m256& mask) { v[0] = _mm256_xor_ps(v0[0],mask); }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::setNeg(hkMxRealParameter r)
{
	static HK_ALIGN32( const hkUint32 mask[8] ) = { 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000 };
	hkMxReal_Implementation::setNegHR<((M+1)>>1)>(m_real.r, r.m_real.r, *(__m256*)&mask);
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void setReciprocalHR(__m256* v, const __m256* v0, const __m256& one) { setReciprocalHR<I-1>(v, v0, one); v[I-1] = _mm256_div_ps(one, v0[I-1]); }
template <> HK_FORCE_INLINE void setReciprocalHR<1>(__m256* v, const __m256* v0, const __m256& one) { v[0] = _mm256_div_ps(one, v0[0]); }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::setReciprocal(hkMxRealParameter r)
{
	const __m256 one = _mm256_set1_ps(1.0f);
	hkMxReal_Implementation::setReciprocalHR<((M+1)>>1)>(m_real.r, r.m_real.r, one);
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void setReciprocal12HR(__m256* v, const __m256* v0) { setReciprocal12HR<I-1>(v, v0); v[I-1] = _mm256_rcp_ps(v0[I-1]); }
template <> HK_FORCE_INLINE void setReciprocal12HR<1>(__m256* v, const __m256* v0) { v[0] = _mm256_rcp_ps(v0[0]); }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::setReciprocal_12BitAccurate(hkMxRealParameter r)
{
	hkMxReal_Implementation::setReciprocal12HR<((M+1)>>1)>(m_real.r, r.m_real.r);
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void setReciprocal23HR(__m256* v, const __m256* v0) { setReciprocal23HR<I-1>(v, v0); v[I-1] = hkMxCommon_Implementation::reciprocalH(v0[I-1]); }
template <> HK_FORCE_INLINE void setReciprocal23HR<1>(__m256* v, const __m256* v0) { v[0] = hkMxCommon_Implementation::reciprocalH(v0[0]); }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::setReciprocal_23BitAccurate(hkMxRealParameter r)
{
	hkMxReal_Implementation::setReciprocal23HR<((M+1)>>1)>(m_real.r, r.m_real.r);
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void sqrt12HR(__m256* v) 
{ 
	sqrt12HR<I-1>(v); 
	// workaround VS2010 assembler bug
	const __m256 masklow =  _mm256_cmp_ps(v[I-1], _mm256_setzero_ps(), _CMP_EQ_OQ);
	const __m256 maskhigh = _mm256_cmp_ps(_mm256_permute2f128_ps(v[I-1],v[I-1], _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_ps(), _CMP_EQ_OQ);
	const __m256 equalsZero = _mm256_permute2f128_ps(masklow,maskhigh, 0x20);
	const __m256 e = _mm256_rcp_ps(_mm256_rsqrt_ps(v[I-1]));
	v[I-1] = _mm256_andnot_ps(equalsZero, e);
}
template <> HK_FORCE_INLINE void sqrt12HR<1>(__m256* v) 
{ 
	// workaround VS2010 assembler bug
	const __m256 masklow =  _mm256_cmp_ps(v[0], _mm256_setzero_ps(), _CMP_EQ_OQ);
	const __m256 maskhigh = _mm256_cmp_ps(_mm256_permute2f128_ps(v[0],v[0], _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_ps(), _CMP_EQ_OQ);
	const __m256 equalsZero = _mm256_permute2f128_ps(masklow,maskhigh, 0x20);
	const __m256 e = _mm256_rcp_ps(_mm256_rsqrt_ps(v[0]));
	v[0] = _mm256_andnot_ps(equalsZero, e);
}
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::sqrt_12BitAccurate()
{
	hkMxReal_Implementation::sqrt12HR<((M+1)>>1)>(m_real.r);
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void sqrt23HR(__m256* v) 
{ 
	sqrt23HR<I-1>(v); 
	// workaround VS2010 assembler bug
	const __m256 masklow =  _mm256_cmp_ps(v[I-1], _mm256_setzero_ps(), _CMP_EQ_OQ);
	const __m256 maskhigh = _mm256_cmp_ps(_mm256_permute2f128_ps(v[I-1],v[I-1], _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_ps(), _CMP_EQ_OQ);
	const __m256 equalsZero = _mm256_permute2f128_ps(masklow,maskhigh, 0x20);
	const __m256 e = hkMxCommon_Implementation::reciprocalH(hkMxCommon_Implementation::reciprocalSquareRootH(v[I-1]));
	v[I-1] = _mm256_andnot_ps(equalsZero, e);
}
template <> HK_FORCE_INLINE void sqrt23HR<1>(__m256* v) 
{ 
	// workaround VS2010 assembler bug
	const __m256 masklow =  _mm256_cmp_ps(v[0], _mm256_setzero_ps(), _CMP_EQ_OQ);
	const __m256 maskhigh = _mm256_cmp_ps(_mm256_permute2f128_ps(v[0],v[0], _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_ps(), _CMP_EQ_OQ);
	const __m256 equalsZero = _mm256_permute2f128_ps(masklow,maskhigh, 0x20);
	const __m256 e = hkMxCommon_Implementation::reciprocalH(hkMxCommon_Implementation::reciprocalSquareRootH(v[0]));
	v[0] = _mm256_andnot_ps(equalsZero, e);
}
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::sqrt_23BitAccurate()
{
	hkMxReal_Implementation::sqrt23HR<((M+1)>>1)>(m_real.r);
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void sqrtHR(__m256* v) { sqrtHR<I-1>(v); v[I-1] = _mm256_sqrt_ps(v[I-1]); }
template <> HK_FORCE_INLINE void sqrtHR<1>(__m256* v) { v[0] = _mm256_sqrt_ps(v[0]); }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::sqrt()
{
	hkMxReal_Implementation::sqrtHR<((M+1)>>1)>(m_real.r);
}



namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void selectRH(__m256* r, const __m256* mask, const __m256* trueValue, const __m256* falseValue)
{
	selectRH<I-1>(r, mask, trueValue, falseValue);
	r[I-1] = _mm256_blendv_ps(falseValue[I-1], trueValue[I-1], mask[I-1]);
}
template <> HK_FORCE_INLINE void selectRH<1>(__m256* r, const __m256* mask, const __m256* trueValue, const __m256* falseValue)
{
	r[0] = _mm256_blendv_ps(falseValue[0], trueValue[0], mask[0]);
}
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::setSelect(hkMxMaskParameter mask, hkMxRealParameter trueValue, hkMxRealParameter falseValue)
{
	hkMxReal_Implementation::selectRH<((M+1)>>1)>(m_real.r, mask.m_comp.c, trueValue.m_real.r, falseValue.m_real.r);
}



namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void selectRH(__m256* r, const __m256* mask, const __m256& trueValue, const __m256* falseValue)
{
	selectRH<I-1>(r, mask, trueValue, falseValue);
	r[I-1] = _mm256_blendv_ps(falseValue[I-1], trueValue, mask[I-1]);
}
template <> HK_FORCE_INLINE void selectRH<1>(__m256* r, const __m256* mask, const __m256& trueValue, const __m256* falseValue)
{
	r[0] = _mm256_blendv_ps(falseValue[0], trueValue, mask[0]);
}
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::setSelect(hkMxMaskParameter mask, hkMxSingleParameter trueValue, hkMxRealParameter falseValue)
{
	const __m256 tV = _mm256_permute_ps(trueValue.m_single.s, 0x00);
	hkMxReal_Implementation::selectRH<((M+1)>>1)>(m_real.r, mask.m_comp.c, tV, falseValue.m_real.r);
}



namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void selectRH(__m256* r, const __m256* mask, const __m256* trueValue, const __m256& falseValue)
{
	selectRH<I-1>(r, mask, trueValue, falseValue);
	r[I-1] = _mm256_blendv_ps(falseValue, trueValue[I-1], mask[I-1]);
}
template <> HK_FORCE_INLINE void selectRH<1>(__m256* r, const __m256* mask, const __m256* trueValue, const __m256& falseValue)
{
	r[0] = _mm256_blendv_ps(falseValue, trueValue[0], mask[0]);
}
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::setSelect(hkMxMaskParameter mask, hkMxRealParameter trueValue, hkMxSingleParameter falseValue)
{
	const __m256 fV = _mm256_permute_ps(falseValue.m_single.s, 0x00);
	hkMxReal_Implementation::selectRH<((M+1)>>1)>(m_real.r, mask.m_comp.c, trueValue.m_real.r, fV);
}


template <int M>
HK_FORCE_INLINE void hkMxReal<M>::storePacked(hkVector4& vOut) const
{
	HK_MXREAL_MX_NOT_IMPLEMENTED;
}

template <>
HK_FORCE_INLINE void hkMxReal<1>::storePacked(hkVector4& vOut) const
{
	vOut.m_quad = _mm256_extractf128_ps(m_real.r[0], 0x0);
	HK_ON_DEBUG(HK_M128(vOut.m_quad).m128_u32[1] = 0xffffffff; HK_M128(vOut.m_quad).m128_u32[2] = 0xffffffff; HK_M128(vOut.m_quad).m128_u32[3] = 0xffffffff;)
}

template <>
HK_FORCE_INLINE void hkMxReal<2>::storePacked(hkVector4& vOut) const
{
	hkSimdReal a; a.m_real = _mm256_extractf128_ps(m_real.r[0], 0x0);
	hkSimdReal b; b.m_real = _mm256_extractf128_ps(m_real.r[0], 0x1);

	vOut.set(a,b,b,b);
	HK_ON_DEBUG(HK_M128(vOut.m_quad).m128_u32[2] = 0xffffffff; HK_M128(vOut.m_quad).m128_u32[3] = 0xffffffff;)
}

template <>
HK_FORCE_INLINE void hkMxReal<3>::storePacked(hkVector4& vOut) const
{
	hkSimdReal a; a.m_real = _mm256_extractf128_ps(m_real.r[0], 0x0);
	hkSimdReal b; b.m_real = _mm256_extractf128_ps(m_real.r[0], 0x1);
	hkSimdReal c; c.m_real = _mm256_extractf128_ps(m_real.r[1], 0x0);

	vOut.set(a,b,c,c);
	HK_ON_DEBUG(HK_M128(vOut.m_quad).m128_u32[3] = 0xffffffff;)
}

template <>
HK_FORCE_INLINE void hkMxReal<4>::storePacked(hkVector4& vOut) const
{
	hkSimdReal a; a.m_real = _mm256_extractf128_ps(m_real.r[0], 0x0);
	hkSimdReal b; b.m_real = _mm256_extractf128_ps(m_real.r[0], 0x1);
	hkSimdReal c; c.m_real = _mm256_extractf128_ps(m_real.r[1], 0x0);
	hkSimdReal d; d.m_real = _mm256_extractf128_ps(m_real.r[1], 0x1);

	vOut.set(a,b,c,d);
}



namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void setDiv12HR(__m256* v, const __m256* v0, const __m256* v1) { setDiv12HR<I-1>(v, v0, v1); v[I-1] = _mm256_mul_ps(v0[I-1], _mm256_rcp_ps(v1[I-1])); }
template <> HK_FORCE_INLINE void setDiv12HR<1>(__m256* v, const __m256* v0, const __m256* v1) { v[0] = _mm256_mul_ps(v0[0], _mm256_rcp_ps(v1[0])); }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::setDiv_12BitAccurate(hkMxRealParameter v0, hkMxRealParameter v1)
{
	hkMxReal_Implementation::setDiv12HR<((M+1)>>1)>(m_real.r, v0.m_real.r, v1.m_real.r);
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void setDiv23HR(__m256* v, const __m256* v0, const __m256* v1) { setDiv23HR<I-1>(v, v0, v1); v[I-1] = _mm256_mul_ps(v0[I-1], hkMxCommon_Implementation::reciprocalH(v1[I-1])); }
template <> HK_FORCE_INLINE void setDiv23HR<1>(__m256* v, const __m256* v0, const __m256* v1) { v[0] = _mm256_mul_ps(v0[0], hkMxCommon_Implementation::reciprocalH(v1[0])); }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::setDiv_23BitAccurate(hkMxRealParameter v0, hkMxRealParameter v1)
{
	hkMxReal_Implementation::setDiv23HR<((M+1)>>1)>(m_real.r, v0.m_real.r, v1.m_real.r);
}


#undef MXR_ONE_OPERAND
#undef MXR_TWO_OPERANDS
#undef MXR_CMP_MXREAL
#undef MXR_CMP_MXVECTOR
#undef MXR_CMP_MXSINGLE

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
