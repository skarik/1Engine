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


#define MXV_NO_OPERANDS(METHOD, OP) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void METHOD##H(__m256* v) { METHOD##H<I-1>(v); v[I-1] = OP(); } \
	template <> HK_FORCE_INLINE void METHOD##H<1>(__m256* v) { v[0] = OP(); } \
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD() { hkMxVector_Implementation::METHOD##H<((M+1)>>1)>(m_vec.v); }

#define MXV_OP_MXVECTOR(METHOD, OP) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void METHOD##MXV_MXV_MXV(__m256* v, const __m256* v0) { METHOD##MXV_MXV_MXV<I-1>(v, v0); v[I-1] = OP(v[I-1],v0[I-1]); } \
	template <> HK_FORCE_INLINE void METHOD##MXV_MXV_MXV<1>(__m256* v, const __m256* v0) { v[0] = OP(v[0],v0[0]); }	\
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD(hkMxVectorParameter m) { hkMxVector_Implementation::METHOD##MXV_MXV_MXV<((M+1)>>1)>(m_vec.v, m.m_vec.v); }

#define MXV_OP_MXVECTOR_MXVECTOR(METHOD, OP) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void METHOD##H(__m256* v, const __m256* v0, const __m256* v1) { METHOD##H<I-1>(v, v0, v1); v[I-1] = OP(v0[I-1], v1[I-1]); } \
	template <> HK_FORCE_INLINE void METHOD##H<1>(__m256* v, const __m256* v0, const __m256* v1) { v[0] = OP(v0[0], v1[0]); } \
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD(hkMxVectorParameter v0, hkMxVectorParameter v1) { hkMxVector_Implementation::METHOD##H<((M+1)>>1)>(m_vec.v, v0.m_vec.v, v1.m_vec.v); }

#define MXV_OP_MXVECTOR_MXVECTOR_MXVECTOR(METHOD, OP1, OP2) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void METHOD##H(__m256* v, const __m256* v0, const __m256* v1, const __m256* v2) { METHOD##H<I-1>(v, v0, v1, v2); v[I-1] = OP1(v0[I-1], OP2(v1[I-1], v2[I-1])); } \
	template <> HK_FORCE_INLINE void METHOD##H<1>(__m256* v, const __m256* v0, const __m256* v1, const __m256* v2) { v[0] = OP1(v0[0], OP2(v1[0], v2[0])); } \
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD(hkMxVectorParameter v0, hkMxVectorParameter v1, hkMxVectorParameter v2) { hkMxVector_Implementation::METHOD##H<((M+1)>>1)>(m_vec.v, v0.m_vec.v, v1.m_vec.v, v2.m_vec.v); }

#define MXV_OP_MXVECTOR_MXVECTOR_MXSINGLE(METHOD, OP1, OP2) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void METHOD##S2H(__m256* v, const __m256* v0, const __m256* v1, const __m256& v2) { METHOD##S2H<I-1>(v, v0, v1, v2); v[I-1] = OP1(v0[I-1], OP2(v1[I-1], v2)); } \
	template <> HK_FORCE_INLINE void METHOD##S2H<1>(__m256* v, const __m256* v0, const __m256* v1, const __m256& v2) { v[0] = OP1(v0[0], OP2(v1[0], v2)); } \
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD(hkMxVectorParameter v0, hkMxVectorParameter v1, hkMxSingleParameter v2) { hkMxVector_Implementation::METHOD##S2H<((M+1)>>1)>(m_vec.v, v0.m_vec.v, v1.m_vec.v, v2.m_single.s); }

#define MXV_OP_MXVECTOR_MXVECTOR_MXREAL(METHOD, OP1, OP2) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void METHOD##HVR(__m256* v, const __m256* v0, const __m256* v1, const __m256* v2) { METHOD##HVR<I-1>(v, v0, v1, v2); v[I-1] = OP1(v0[I-1], OP2(v1[I-1], v2[I-1])); } \
	template <> HK_FORCE_INLINE void METHOD##HVR<1>(__m256* v, const __m256* v0, const __m256* v1, const __m256* v2) { v[0] = OP1(v0[0], OP2(v1[0], v2[0])); } \
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD(hkMxVectorParameter v0, hkMxVectorParameter v1, hkMxRealParameter v2) { hkMxVector_Implementation::METHOD##HVR<((M+1)>>1)>(m_vec.v, v0.m_vec.v, v1.m_vec.v, v2.m_real.r); }

#define MXV_OP_MXVECTOR_MXVECTOR_SELF(METHOD, OP1, OP2) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void METHOD##HSE(__m256* v, const __m256* v0, const __m256* v1) { METHOD##HSE<I-1>(v, v0, v1); v[I-1] = OP1(v[I-1], OP2(v0[I-1], v1[I-1])); } \
	template <> HK_FORCE_INLINE void METHOD##HSE<1>(__m256* v, const __m256* v0, const __m256* v1) { v[0] = OP1(v[0], OP2(v0[0], v1[0])); } \
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD(hkMxVectorParameter v0, hkMxVectorParameter v1) { hkMxVector_Implementation::METHOD##HSE<((M+1)>>1)>(m_vec.v, v0.m_vec.v, v1.m_vec.v); }

#define MXV_OP_MXVECTOR_MXSINGLE_SELF(METHOD, OP1, OP2) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void METHOD##HSES(__m256* v, const __m256* v0, const __m256& v1) { METHOD##HSES<I-1>(v, v0, v1); v[I-1] = OP1(v[I-1], OP2(v0[I-1], v1)); } \
	template <> HK_FORCE_INLINE void METHOD##HSES<1>(__m256* v, const __m256* v0, const __m256& v1) { v[0] = OP1(v[0], OP2(v0[0], v1)); } \
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD(hkMxVectorParameter v0, hkMxSingleParameter v1) { hkMxVector_Implementation::METHOD##HSES<((M+1)>>1)>(m_vec.v, v0.m_vec.v, v1.m_single.s); }

#define MXV_OP_MXREAL_MXVECTOR_SELF(METHOD, OP1, OP2) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void METHOD##HSE2(__m256* v, const __m256* v0, const __m256* v1) { METHOD##HSE2<I-1>(v, v0, v1); v[I-1] = OP1(v[I-1], OP2(v0[I-1], v1[I-1])); } \
	template <> HK_FORCE_INLINE void METHOD##HSE2<1>(__m256* v, const __m256* v0, const __m256* v1) { v[0] = OP1(v[0], OP2(v0[0], v1[0])); } \
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD(hkMxRealParameter v0, hkMxVectorParameter v1) { hkMxVector_Implementation::METHOD##HSE2<((M+1)>>1)>(m_vec.v, v0.m_real.r, v1.m_vec.v); }


#define MXV_OP_MXVECTOR_MXSINGLE_MXREAL(METHOD, OP1, OP2) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void METHOD##S2SR(__m256* v, const __m256* v0, const __m256& v1, const __m256* v2) { METHOD##S2SR<I-1>(v, v0, v1, v2); v[I-1] = OP1(v0[I-1], OP2(v1, v2[I-1])); } \
	template <> HK_FORCE_INLINE void METHOD##S2SR<1>(__m256* v, const __m256* v0, const __m256& v1,   const __m256* v2) { v[0] = OP1(v0[0], OP2(v1, v2[0])); } \
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD(hkMxVectorParameter v0, hkMxSingleParameter v1, hkMxRealParameter v2) { hkMxVector_Implementation::METHOD##S2SR<((M+1)>>1)>(m_vec.v, v0.m_vec.v, v1.m_single.s, v2.m_real.r); }


#define MXV_OP_MXREAL(METHOD, OP) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void METHOD##SHR(__m256* v, const __m256* v0) { METHOD##SHR<I-1>(v, v0); v[I-1] = OP(v[I-1],v0[I-1]); } \
	template <> HK_FORCE_INLINE void METHOD##SHR<1>(__m256* v, const __m256* v0) { v[0] = OP(v[0],v0[0]); } \
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD(hkMxRealParameter s) { hkMxVector_Implementation::METHOD##SHR<((M+1)>>1)>(m_vec.v, s.m_real.r); }


#define MXV_OP_MXSINGLE(METHOD, OP) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void METHOD##SH(__m256* v, const __m256& v0) {METHOD##SH<I-1>(v, v0); v[I-1] = OP(v[I-1],v0); } \
	template <> HK_FORCE_INLINE void METHOD##SH<1>(__m256* v, const __m256& v0) { v[0] = OP(v[0],v0); } \
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD(hkMxSingleParameter s) { hkMxVector_Implementation::METHOD##SH<((M+1)>>1)>(m_vec.v, s.m_single.s); }

#define MXV_OP_MXREAL_MXVECTOR(METHOD, OP) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void METHOD##RVH(__m256* v, const __m256* v0, const __m256* v1) { METHOD##RVH<I-1>(v, v0, v1); v[I-1] = OP(v0[I-1], v1[I-1]); } \
	template <> HK_FORCE_INLINE void METHOD##RVH<1>(__m256* v, const __m256* v0, const __m256* v1) { v[0] = OP(v0[0], v1[0]); } \
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD(hkMxRealParameter r, hkMxVectorParameter v0 ) { hkMxVector_Implementation::METHOD##RVH<((M+1)>>1)>(m_vec.v, r.m_real.r, v0.m_vec.v ); }


#define MXV_OP_MXVECTOR_MXSINGLE(METHOD, OP) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void METHOD##SH(__m256* v, const __m256* v0, const __m256& v1) { METHOD##SH<I-1>(v, v0, v1); v[I-1] = OP(v0[I-1], v1); } \
	template <> HK_FORCE_INLINE void METHOD##SH<1>(__m256* v, const __m256* v0, const __m256& v1) { v[0] = OP(v0[0], v1); }	\
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD(hkMxVectorParameter v, hkMxSingleParameter s) { hkMxVector_Implementation::METHOD##SH<((M+1)>>1)>(m_vec.v, v.m_vec.v, s.m_single.s); }

#define MXV_OP_MXSINGLE_MXSINGLE(METHOD, OP1, OP2) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void METHOD##MX_SS(__m256* v, const __m256& v0) { METHOD##MX_SS<I-1>(v, v0); v[I-1] = OP1(v[I-1], v0); } \
	template <> HK_FORCE_INLINE void METHOD##MX_SS<1>(  __m256* v, const __m256& v0) { v[0] = OP1(v[0], v0); }	\
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD(hkMxSingleParameter s0, hkMxSingleParameter s1) { const __m256 s = OP2(s0.m_single.s, s1.m_single.s); hkMxVector_Implementation::METHOD##MX_SS<((M+1)>>1)>(m_vec.v, s); }

#define MXV_OP_MXSINGLE_MXVECTOR(METHOD, OP) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void METHOD##RSH(__m256* v, const __m256& v1, const __m256* v0) { METHOD##RSH<I-1>(v, v1, v0); v[I-1] = OP(v1, v0[I-1]); } \
	template <> HK_FORCE_INLINE void METHOD##RSH<1>(__m256* v, const __m256& v1, const __m256* v0) { v[0] = OP(v1, v0[0]); } \
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD(hkMxSingleParameter s, hkMxVectorParameter v) { hkMxVector_Implementation::METHOD##RSH<((M+1)>>1)>(m_vec.v, s.m_single.s, v.m_vec.v); }

#define MXV_COMPARE(METHOD, OP) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void METHOD##OP##CH(const __m256* v0, const __m256* v1, __m256* mask) { \
	METHOD##OP##CH<I-1>(v0,v1,mask); \
	/*mask[I-1] = _mm256_cmp_ps(v0[I-1],v1[I-1],OP);*/ \
	/* workaround VS2010 assembler bug */ \
	const __m256 masklow =  _mm256_cmp_ps(v0[I-1], v1[I-1], OP); \
	const __m256 maskhigh = _mm256_cmp_ps(_mm256_permute2f128_ps(v0[I-1],v0[I-1], _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_permute2f128_ps(v1[I-1],v1[I-1], _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), OP); \
	mask[I-1] = _mm256_permute2f128_ps(masklow,maskhigh, 0x20); } \
	template <> HK_FORCE_INLINE void METHOD##OP##CH<1>(const __m256* v0, const __m256* v1, __m256* mask) { \
	/*mask[0] = _mm256_cmp_ps(v0[0],v1[0],OP);*/ \
	/* workaround VS2010 assembler bug */ \
	const __m256 masklow =  _mm256_cmp_ps(v0[0], v1[0], OP); \
	const __m256 maskhigh = _mm256_cmp_ps(_mm256_permute2f128_ps(v0[0],v0[0], _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_permute2f128_ps(v1[0],v1[0], _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), OP); \
	mask[0] = _mm256_permute2f128_ps(masklow,maskhigh, 0x20); } \
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD(hkMxVectorParameter v1, hkMxMask<M>& mask) const { hkMxVector_Implementation::METHOD##OP##CH<((M+1)>>1)>(m_vec.v, v1.m_vec.v, mask.m_comp.c); }

#define MXV_COMPARE_SINGLE(METHOD, OP) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void METHOD##OP##CHS(const __m256* v0, const __m256& v1, __m256* mask) { \
	METHOD##OP##CHS<I-1>(v0,v1,mask); \
	/*mask[I-1] = _mm256_cmp_ps(v0[I-1],v1,OP);*/ \
	/* workaround VS2010 assembler bug */ \
	const __m256 masklow =  _mm256_cmp_ps(v0[I-1], v1, OP); \
	const __m256 maskhigh = _mm256_cmp_ps(_mm256_permute2f128_ps(v0[I-1],v0[I-1], _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_permute2f128_ps(v1,v1, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), OP); \
	mask[I-1] = _mm256_permute2f128_ps(masklow,maskhigh, 0x20); } \
	template <> HK_FORCE_INLINE void METHOD##OP##CHS<1>(const __m256* v0, const __m256& v1, __m256* mask) { \
	/*mask[0] = _mm256_cmp_ps(v0[0],v1,OP);*/ \
	/* workaround VS2010 assembler bug */ \
	const __m256 masklow =  _mm256_cmp_ps(v0[0], v1, OP); \
	const __m256 maskhigh = _mm256_cmp_ps(_mm256_permute2f128_ps(v0[0],v0[0], _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_permute2f128_ps(v1,v1, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), OP); \
	mask[0] = _mm256_permute2f128_ps(masklow,maskhigh, 0x20); } \
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD(hkMxSingleParameter v1, hkMxMask<M>& mask) const { hkMxVector_Implementation::METHOD##OP##CHS<((M+1)>>1)>(m_vec.v, v1.m_single.s, mask.m_comp.c); }


#ifndef HK_DISABLE_MATH_CONSTRUCTORS

namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void constructH(__m256* v, const __m256& v0) { constructH<I-1>(v, v0); v[I-1] = v0; }
template <> HK_FORCE_INLINE void constructH<1>(__m256* v, const __m256& v0) { v[0] = v0; }
}
template <int M>
HK_FORCE_INLINE hkMxVector<M>::hkMxVector(hkVector4Parameter v)
{
	const __m256 vr = _mm256_broadcast_ps(&v.m_quad);
	hkMxVector_Implementation::constructH<((M+1)>>1)>(m_vec.v, vr);
}


namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void constructRealH(__m256* v, const __m256* v0) { constructRealH<I-1>(v, v0); v[I-1] = v0[I-1]; }
template <> HK_FORCE_INLINE void constructRealH<1>(__m256* v, const __m256* v0) { v[0] = v0[0]; }
}
template <int M>
HK_FORCE_INLINE hkMxVector<M>::hkMxVector(hkMxRealParameter v)
{
	hkMxVector_Implementation::constructRealH<((M+1)>>1)>(m_vec.v, v.m_real.r);
}


namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void constructSRealH(__m256* v, const __m256& r) { constructSRealH<I-1>(v, r); v[I-1] = r; }
template <> HK_FORCE_INLINE void constructSRealH<1>(__m256* v, const __m256& r) { v[0] = r; }
}
template <int M>
HK_FORCE_INLINE hkMxVector<M>::hkMxVector(hkSimdRealParameter r)
{
	const __m256 sr = _mm256_broadcast_ps(&r.m_real);
	hkMxVector_Implementation::constructSRealH<((M+1)>>1)>(m_vec.v, sr);
}

#endif

namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void setBCH(__m256* v, const __m256* v0) { setBCH<I-1>(v, v0); v[I-1] = v0[I-1]; }
template <> HK_FORCE_INLINE void setBCH<1>(__m256* v, const __m256* v0) { v[0] = v0[0]; }
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::setBroadcast( hkMxRealParameter r )
{
	hkMxVector_Implementation::setBCH<((M+1)>>1)>(m_vec.v, r.m_real.r);
}

namespace hkMxVector_Implementation
{
template <int I, int C> struct setScalarBroadcastH { HK_FORCE_INLINE static void apply(__m256* v, const __m256* v0) { setScalarBroadcastH<I-1,C>::apply(v, v0); v[I-1] = _mm256_permute_ps(v0[I-1], C); } };
template <int C> struct setScalarBroadcastH<1,C> { HK_FORCE_INLINE static void apply(__m256* v, const __m256* v0) { v[0] = _mm256_permute_ps(v0[0], C); } };
}
template <int M>
template <int I>
HK_FORCE_INLINE void hkMxVector<M>::setScalarBroadcast(hkMxVectorParameter v)
{
	HK_MXVECTOR_VECTOR4_SUBINDEX_CHECK;
	const int control = I | (I<<2) | (I<<4) | (I<<6);
	hkMxVector_Implementation::setScalarBroadcastH<((M+1)>>1),control>::apply(m_vec.v, v.m_vec.v);
}


template <int M>
template <int I>
HK_FORCE_INLINE void hkMxVector<M>::setVector(hkVector4Parameter v)
{
	HK_MXVECTOR_MX_SUBINDEX_CHECK;
	const int subvector = I>>1;
	const int subvectorsubindex = ((I+1)>>1) - subvector;
	m_vec.v[subvector] = _mm256_insertf128_ps(m_vec.v[subvector], v.m_quad, subvectorsubindex);
}

template <int M>
template <int I>
HK_FORCE_INLINE const hkVector4& hkMxVector<M>::getVector() const
{
	HK_MXVECTOR_MX_SUBINDEX_CHECK;
	const int subvector = I>>1;
	const int subvectorsubindex = ((I+1)>>1) - subvector;
	hkVector4 v;
	v.m_quad = _mm256_extractf128_ps(m_vec.v[subvector], subvectorsubindex);
	return v;
}

template <int M>
template <int I>
HK_FORCE_INLINE void hkMxVector<M>::getVector(hkVector4& vOut) const
{
	HK_MXVECTOR_MX_SUBINDEX_CHECK;
	const int subvector = I>>1;
	const int subvectorsubindex = ((I+1)>>1) - subvector;
	vOut.m_quad = _mm256_extractf128_ps(m_vec.v[subvector], subvectorsubindex);
}

namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void setConstH(__m256* v, const __m256& vC) { setConstH<I-1>(v,vC); v[I-1] = vC; }
template <> HK_FORCE_INLINE void setConstH<1>(__m256* v, const __m256& vC) { v[0] = vC; }
}
template <int M>
template<int vectorConstant> 
HK_FORCE_INLINE void hkMxVector<M>::setConstant()
{
	const __m256 vC = _mm256_broadcast_ps(g_vectorConstants + vectorConstant);
	hkMxVector_Implementation::setConstH<((M+1)>>1)>(m_vec.v, vC);
}


namespace hkMxVector_Implementation
{
template <int I, int N> struct isOkH { HK_FORCE_INLINE static __m256 apply(const __m256* v) 
{ 
	// workaround VS2010 assembler bug
	const __m256 masklow =  _mm256_cmp_ps(v[N], _mm256_setzero_ps(), _CMP_UNORD_Q);
	const __m256 maskhigh = _mm256_cmp_ps(_mm256_permute2f128_ps(v[N],v[N], _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_ps(), _CMP_UNORD_Q);
	const __m256 c = _mm256_permute2f128_ps(masklow,maskhigh, 0x20);
	return _mm256_or_ps(c, isOkH<I-2,N+1>::apply(v)); 
} };
template <int N> struct isOkH<2,N> { HK_FORCE_INLINE static __m256 apply(const __m256* v) 
{ 
	// workaround VS2010 assembler bug
	const __m256 masklow =  _mm256_cmp_ps(v[N], _mm256_setzero_ps(), _CMP_UNORD_Q);
	const __m256 maskhigh = _mm256_cmp_ps(_mm256_permute2f128_ps(v[N],v[N], _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_ps(), _CMP_UNORD_Q);
	return _mm256_permute2f128_ps(masklow,maskhigh, 0x20);
} };
template <int N> struct isOkH<1,N> { HK_FORCE_INLINE static __m256 apply(const __m256* v) 
{ 
	static HK_ALIGN32( const hkUint32 mask[8] ) = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
	// workaround VS2010 assembler bug
	const __m256 masklow =  _mm256_cmp_ps(v[N], _mm256_setzero_ps(), _CMP_UNORD_Q);
	const __m256 maskhigh = _mm256_cmp_ps(_mm256_permute2f128_ps(v[N],v[N], _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_ps(), _CMP_UNORD_Q);
	const __m256 c = _mm256_permute2f128_ps(masklow,maskhigh, 0x20);
	return _mm256_and_ps(c, *(__m256*)&mask);
} };
}
template <int M>
template <int N>
HK_FORCE_INLINE hkBool32 hkMxVector<M>::isOk() const
{
	HK_MXVECTOR_VECTOR4_UNSUPPORTED_LENGTH_CHECK;
#if !defined(HK_DEBUG)
	// pragma compiler perf warning
#endif
	const __m256 c = hkMxVector_Implementation::isOkH<M,0>::apply(m_vec.v);
	switch (N) // compiletime switch
	{
		case 2 : return !(_mm256_movemask_ps(c) & 0x33); break;
		case 3 : return !(_mm256_movemask_ps(c) & 0x77); break;
		case 4 : return !(_mm256_movemask_ps(c)); break;
		default: return !(_mm256_movemask_ps(c) & 0x11); break;
	}
}


namespace hkMxVector_Implementation
{
template <int I, int N> struct moveLoadH { HK_FORCE_INLINE static void apply(__m256* HK_RESTRICT v, const __m256* HK_RESTRICT v0) { v[N] = v0[N]; moveLoadH<I-2,N+1>::apply(v, v0); } };
template <int N> struct moveLoadH<2,N> { HK_FORCE_INLINE static void apply(__m256* HK_RESTRICT v, const __m256* HK_RESTRICT v0) { v[N] = v0[N]; } };
template <int N> struct moveLoadH<1,N> { HK_FORCE_INLINE static void apply(__m256* HK_RESTRICT v, const __m256* HK_RESTRICT v0) { v[N] = _mm256_insertf128_ps(_mm256_setzero_ps(), _mm256_castps256_ps128(v0[N]), 0x0); } };
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::moveLoad(const hkVector4* HK_RESTRICT v)
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)v) & 0x1f ) == 0, "v must be 32-byte aligned."); 
	HK_ASSERT2(0x20baff23, ((hkUlong)m_vec.v >= (hkUlong)(v+M)) || ((hkUlong)v >= (hkUlong)(m_vec.v+((M+1)>>1))), "unsafe move operation");
	hkMxVector_Implementation::moveLoadH<M,0>::apply(m_vec.v, (const __m256* HK_RESTRICT)v);
}
namespace hkMxVector_Implementation
{
template <int I, int N> struct moveStoreH { HK_FORCE_INLINE static void apply(hkVector4* HK_RESTRICT v, const __m256* HK_RESTRICT v0) { *((__m256*)(v+(2*N))) = v0[N]; moveStoreH<I-2,N+1>::apply(v, v0); } };
template <int N> struct moveStoreH<2,N> { HK_FORCE_INLINE static void apply(hkVector4* HK_RESTRICT v, const __m256* HK_RESTRICT v0) { *((__m256*)(v+(2*N))) = v0[N]; } };
template <int N> struct moveStoreH<1,N> { HK_FORCE_INLINE static void apply(hkVector4* HK_RESTRICT v, const __m256* HK_RESTRICT v0) { *((__m128*)(v+(2*N))) = _mm256_castps256_ps128(v0[N]); } };
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::moveStore(hkVector4* HK_RESTRICT v) const
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)v) & 0x1f ) == 0, "v must be 32-byte aligned."); 
	HK_ASSERT2(0x20baff22, ((hkUlong)m_vec.v >= (hkUlong)(v+M)) || ((hkUlong)v >= (hkUlong)(m_vec.v+((M+1)>>1))), "unsafe move operation");
	hkMxVector_Implementation::moveStoreH<M,0>::apply(v, m_vec.v);
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::operator= ( hkMxVectorParameter v )
{
	HK_ASSERT2(0x20baff21, ((hkUlong)m_vec.v >= (hkUlong)(v.m_vec.v+((M+1)>>1))) || ((hkUlong)v.m_vec.v >= (hkUlong)(m_vec.v+((M+1)>>1))), "unsafe move operation");
	hkMxVector_Implementation::moveLoadH<M,0>::apply(m_vec.v, v.m_vec.v);
}


namespace hkMxVector_Implementation
{
template <int I, int N> struct loadRH { HK_FORCE_INLINE static void apply(__m256* v, const hkReal* r) { v[N] = _mm256_load_ps(r+(N*8)); loadRH<I-2,N+1>::apply(v, r); } };
template <int N> struct loadRH<2,N> { HK_FORCE_INLINE static void apply(__m256* v, const hkReal* r) { v[N] = _mm256_load_ps(r+(N*8)); } };
template <int N> struct loadRH<1,N> { HK_FORCE_INLINE static void apply(__m256* v, const hkReal* r) { v[N] = _mm256_setr_ps(r[N*8], r[(N*8)+1], r[(N*8)+2], r[(N*8)+3], 0,0,0,0); } };
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::load(const hkReal* r)
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)r) & 0x1f ) == 0, "r must be 32-byte aligned."); 
	hkMxVector_Implementation::loadRH<M,0>::apply(m_vec.v, r);
}

namespace hkMxVector_Implementation
{
template <int I, int N> struct loadNRH { HK_FORCE_INLINE static void apply(__m256* v, const hkReal* r) { v[N] = _mm256_loadu_ps(r+(N*8)); loadNRH<I-2,N+1>::apply(v, r); } };
template <int N> struct loadNRH<2,N> { HK_FORCE_INLINE static void apply(__m256* v, const hkReal* r) { v[N] = _mm256_loadu_ps(r+(N*8)); } };
template <int N> struct loadNRH<1,N> { HK_FORCE_INLINE static void apply(__m256* v, const hkReal* r) { v[N] = _mm256_setr_ps(r[N*8], r[(N*8)+1], r[(N*8)+2], r[(N*8)+3], 0,0,0,0); } };
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::loadNotAligned(const hkReal* r)
{
	hkMxVector_Implementation::loadNRH<M,0>::apply(m_vec.v, r);
}

template <int M>
HK_FORCE_INLINE void hkMxVector<M>::loadNotCached(const hkReal* r)
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)r) & 0x1f ) == 0, "r must be 32-byte aligned."); 
	hkMxVector_Implementation::loadRH<M,0>::apply(m_vec.v, r);
}

namespace hkMxVector_Implementation
{
template <int I, int N> struct storeRH { HK_FORCE_INLINE static void apply(const __m256* v, hkReal* r) { _mm256_store_ps(r+(N*8), v[N]); storeRH<I-2,N+1>::apply(v, r); } };
template <int N> struct storeRH<2,N> { HK_FORCE_INLINE static void apply(const __m256* v, hkReal* r) { _mm256_store_ps(r+(N*8), v[N]); } };
template <int N> struct storeRH<1,N> { HK_FORCE_INLINE static void apply(const __m256* v, hkReal* r) 
{ 
	static HK_ALIGN32( const hkUint32 mask[8] ) = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
	_mm256_maskstore_ps(r+(N*8), *(__m256i*)&mask, v[N]); 
} };
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::store(hkReal* r) const
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)r) & 0x1f ) == 0, "r must be 32-byte aligned."); 
	hkMxVector_Implementation::storeRH<M,0>::apply(m_vec.v, r);
}

namespace hkMxVector_Implementation
{
template <int I, int N> struct storeNRH { HK_FORCE_INLINE static void apply(const __m256* v, hkReal* r) { _mm256_storeu_ps(r+(N*8), v[N]); storeNRH<I-2,N+1>::apply(v, r); } };
template <int N> struct storeNRH<2,N> { HK_FORCE_INLINE static void apply(const __m256* v, hkReal* r) { _mm256_storeu_ps(r+(N*8), v[N]); } };
template <int N> struct storeNRH<1,N> { HK_FORCE_INLINE static void apply(const __m256* v, hkReal* r) 
{ 
	static HK_ALIGN32( const hkUint32 mask[8] ) = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
	_mm256_maskstore_ps(r+(N*8), *(__m256i*)&mask, v[N]); 
} };
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::storeNotAligned(hkReal* r) const
{
	hkMxVector_Implementation::storeNRH<M,0>::apply(m_vec.v, r);
}

namespace hkMxVector_Implementation
{
template <int I, int N> struct storeCNRH { HK_FORCE_INLINE static void apply(const __m256* v, hkReal* r) { _mm256_stream_ps(r+(N*8), v[N]); storeCNRH<I-2,N+1>::apply(v, r); } };
template <int N> struct storeCNRH<2,N> { HK_FORCE_INLINE static void apply(const __m256* v, hkReal* r) { _mm256_stream_ps(r+(N*8), v[N]); } };
template <int N> struct storeCNRH<1,N> { HK_FORCE_INLINE static void apply(const __m256* v, hkReal* r) 
{ 
	// hm, no nt hint for mask
	static HK_ALIGN32( const hkUint32 mask[8] ) = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
	_mm256_maskstore_ps(r+(N*8), *(__m256i*)&mask, v[N]);
} };
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::storeNotCached(hkReal* r) const
{
	HK_ASSERT2(0x64211c2f, ( ((hkUlong)r) & 0x1f ) == 0, "r must be 32-byte aligned."); 
	hkMxVector_Implementation::storeCNRH<M,0>::apply(m_vec.v, r);
}



namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void setDiv12H(__m256* v, const __m256* v0, const __m256* v1) { setDiv12H<I-1>(v,v0,v1); v[I-1] = _mm256_mul_ps(v0[I-1], _mm256_rcp_ps(v1[I-1])); }
template <> HK_FORCE_INLINE void setDiv12H<1>(__m256* v, const __m256* v0, const __m256* v1) { v[0] = _mm256_mul_ps(v0[0], _mm256_rcp_ps(v1[0])); }
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::setDiv_12BitAccurate(hkMxVectorParameter v0, hkMxVectorParameter v1)
{
	hkMxVector_Implementation::setDiv12H<((M+1)>>1)>(m_vec.v, v0.m_vec.v, v1.m_vec.v);
}

namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void setDiv23H(__m256* v, const __m256* v0, const __m256* v1) { setDiv23H<I-1>(v,v0,v1); v[I-1] = _mm256_mul_ps(v0[I-1], hkMxCommon_Implementation::reciprocalH(v1[I-1])); }
template <> HK_FORCE_INLINE void setDiv23H<1>(__m256* v, const __m256* v0, const __m256* v1) { v[0] = _mm256_mul_ps(v0[0], hkMxCommon_Implementation::reciprocalH(v1[0])); }
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::setDiv_23BitAccurate(hkMxVectorParameter v0, hkMxVectorParameter v1)
{
	hkMxVector_Implementation::setDiv23H<((M+1)>>1)>(m_vec.v, v0.m_vec.v, v1.m_vec.v);
}

namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void setSqrtInverseH(__m256* v, const __m256* v0, const __m256& one) { setSqrtInverseH<I-1>(v,v0,one); v[I-1] = _mm256_div_ps(one, _mm256_sqrt_ps(v0[I-1])); }
template <> HK_FORCE_INLINE void setSqrtInverseH<1>(__m256* v, const __m256* v0, const __m256& one) { v[0] = _mm256_div_ps(one, _mm256_sqrt_ps(v0[0])); }
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::setSqrtInverse(hkMxVectorParameter v)
{
	const __m256 one = _mm256_set1_ps(1.0f);
	hkMxVector_Implementation::setSqrtInverseH<((M+1)>>1)>(m_vec.v, v.m_vec.v, one);
}

namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void setSqrtInverse12H(__m256* v, const __m256* v0) { setSqrtInverse12H<I-1>(v,v0); v[I-1] = _mm256_rsqrt_ps(v0[I-1]); }
template <> HK_FORCE_INLINE void setSqrtInverse12H<1>(__m256* v, const __m256* v0) { v[0] = _mm256_rsqrt_ps(v0[0]); }
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::setSqrtInverse_12BitAccurate(hkMxVectorParameter v)
{
	hkMxVector_Implementation::setSqrtInverse12H<((M+1)>>1)>(m_vec.v, v.m_vec.v);
}

namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void setSqrtInverse23H(__m256* v, const __m256* v0) { setSqrtInverse23H<I-1>(v,v0); v[I-1] = hkMxCommon_Implementation::reciprocalSquareRootH(v0[I-1]); }
template <> HK_FORCE_INLINE void setSqrtInverse23H<1>(__m256* v, const __m256* v0) { v[0] = hkMxCommon_Implementation::reciprocalSquareRootH(v0[0]); }
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::setSqrtInverse_23BitAccurate(hkMxVectorParameter v)
{
	hkMxVector_Implementation::setSqrtInverse23H<((M+1)>>1)>(m_vec.v, v.m_vec.v);
}

namespace hkMxVector_Implementation
{
template <int I, int N> struct normalizeH { HK_FORCE_INLINE static void apply(__m256* v, const __m256& one) 
{
	normalizeH<I-1,N>::apply(v,one);

	const __m256 len2 = hkMxCommon_Implementation::dotProdH<N>(v[I-1],v[I-1]);
	// workaround VS2010 assembler bug
	const __m256 masklow =  _mm256_cmp_ps(len2, _mm256_setzero_ps(), _CMP_EQ_OQ);
	const __m256 maskhigh = _mm256_cmp_ps(_mm256_permute2f128_ps(len2,len2, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_ps(), _CMP_EQ_OQ);
	const __m256 equalsZero = _mm256_permute2f128_ps(masklow,maskhigh, 0x20);
	const __m256 e = _mm256_div_ps(one, _mm256_sqrt_ps(len2));

	// Ensures that we return 0
	const __m256 result = _mm256_andnot_ps(equalsZero, e);
	v[I-1] = _mm256_mul_ps(result, v[I-1]);
} };
template <int N> struct normalizeH<1,N> { HK_FORCE_INLINE static void apply(__m256* v, const __m256& one) 
{ 
	const __m256 len2 = hkMxCommon_Implementation::dotProdH<N>(v[0],v[0]);
	// workaround VS2010 assembler bug
	const __m256 masklow =  _mm256_cmp_ps(len2, _mm256_setzero_ps(), _CMP_EQ_OQ);
	const __m256 maskhigh = _mm256_cmp_ps(_mm256_permute2f128_ps(len2,len2, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_ps(), _CMP_EQ_OQ);
	const __m256 equalsZero = _mm256_permute2f128_ps(masklow,maskhigh, 0x20);
	const __m256 e = _mm256_div_ps(one, _mm256_sqrt_ps(len2));

	// Ensures that we return 0
	const __m256 result = _mm256_andnot_ps(equalsZero, e);
	v[0] = _mm256_mul_ps(result, v[0]);
} };
}
template <int M> 
template <int N> 
HK_FORCE_INLINE void hkMxVector<M>::normalize() 
{
	HK_MXVECTOR_VECTOR4_UNSUPPORTED_LENGTH_CHECK; 
	const __m256 oneV = _mm256_set1_ps(1.0f);
	hkMxVector_Implementation::normalizeH<((M+1)>>1),N>::apply(m_vec.v, oneV);
}

namespace hkMxVector_Implementation
{
template <int I, int N> struct normalize12H { HK_FORCE_INLINE static void apply(__m256* v) 
{
	normalize12H<I-1,N>::apply(v);

	const __m256 len2 = hkMxCommon_Implementation::dotProdH<N>(v[I-1],v[I-1]);
	// workaround VS2010 assembler bug
	const __m256 masklow =  _mm256_cmp_ps(len2, _mm256_setzero_ps(), _CMP_EQ_OQ);
	const __m256 maskhigh = _mm256_cmp_ps(_mm256_permute2f128_ps(len2,len2, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_ps(), _CMP_EQ_OQ);
	const __m256 equalsZero = _mm256_permute2f128_ps(masklow,maskhigh, 0x20);
	const __m256 e = _mm256_rsqrt_ps(len2);

	// Ensures that we return 0
	const __m256 result = _mm256_andnot_ps(equalsZero, e);
	v[I-1] = _mm256_mul_ps(result, v[I-1]);
} };
template <int N> struct normalize12H<1,N> { HK_FORCE_INLINE static void apply(__m256* v) 
{ 
	const __m256 len2 = hkMxCommon_Implementation::dotProdH<N>(v[0],v[0]);
	// workaround VS2010 assembler bug
	const __m256 masklow =  _mm256_cmp_ps(len2, _mm256_setzero_ps(), _CMP_EQ_OQ);
	const __m256 maskhigh = _mm256_cmp_ps(_mm256_permute2f128_ps(len2,len2, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_ps(), _CMP_EQ_OQ);
	const __m256 equalsZero = _mm256_permute2f128_ps(masklow,maskhigh, 0x20);
	const __m256 e = _mm256_rsqrt_ps(len2);

	// Ensures that we return 0
	const __m256 result = _mm256_andnot_ps(equalsZero, e);
	v[0] = _mm256_mul_ps(result, v[0]);
} };
}
template <int M> 
template <int N> 
HK_FORCE_INLINE void hkMxVector<M>::normalize_12BitAccurate() 
{
	HK_MXVECTOR_VECTOR4_UNSUPPORTED_LENGTH_CHECK; 
	hkMxVector_Implementation::normalize12H<((M+1)>>1),N>::apply(m_vec.v);
}

namespace hkMxVector_Implementation
{
template <int I, int N> struct normalize23H { HK_FORCE_INLINE static void apply(__m256* v) 
{
	normalize23H<I-1,N>::apply(v);

	const __m256 len2 = hkMxCommon_Implementation::dotProdH<N>(v[I-1],v[I-1]);
	// workaround VS2010 assembler bug
	const __m256 masklow =  _mm256_cmp_ps(len2, _mm256_setzero_ps(), _CMP_EQ_OQ);
	const __m256 maskhigh = _mm256_cmp_ps(_mm256_permute2f128_ps(len2,len2, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_ps(), _CMP_EQ_OQ);
	const __m256 equalsZero = _mm256_permute2f128_ps(masklow,maskhigh, 0x20);
	const __m256 e = hkMxCommon_Implementation::reciprocalSquareRootH(len2);

	// Ensures that we return 0
	const __m256 result = _mm256_andnot_ps(equalsZero, e);
	v[I-1] = _mm256_mul_ps(result, v[I-1]);
} };
template <int N> struct normalize23H<1,N> { HK_FORCE_INLINE static void apply(__m256* v) 
{ 
	const __m256 len2 = hkMxCommon_Implementation::dotProdH<N>(v[0],v[0]);
	// workaround VS2010 assembler bug
	const __m256 masklow =  _mm256_cmp_ps(len2, _mm256_setzero_ps(), _CMP_EQ_OQ);
	const __m256 maskhigh = _mm256_cmp_ps(_mm256_permute2f128_ps(len2,len2, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_ps(), _CMP_EQ_OQ);
	const __m256 equalsZero = _mm256_permute2f128_ps(masklow,maskhigh, 0x20);
	const __m256 e = hkMxCommon_Implementation::reciprocalSquareRootH(len2);

	// Ensures that we return 0
	const __m256 result = _mm256_andnot_ps(equalsZero, e);
	v[0] = _mm256_mul_ps(result, v[0]);
} };
}
template <int M> 
template <int N> 
HK_FORCE_INLINE void hkMxVector<M>::normalize_23BitAccurate() 
{
	HK_MXVECTOR_VECTOR4_UNSUPPORTED_LENGTH_CHECK; 
	hkMxVector_Implementation::normalize23H<((M+1)>>1),N>::apply(m_vec.v);
}


namespace hkMxVector_Implementation
{
template <int N> HK_FORCE_INLINE __m256 getNegMaskH() { static HK_ALIGN32( const hkUint32 mask[8] ) = { 0x80000000, 0x00000000, 0x00000000, 0x00000000, 0x80000000, 0x00000000, 0x00000000, 0x00000000 }; return *(const __m256*)&mask; }
template <> HK_FORCE_INLINE __m256 getNegMaskH<2>()   { static HK_ALIGN32( const hkUint32 mask[8] ) = { 0x80000000, 0x80000000, 0x00000000, 0x00000000, 0x80000000, 0x80000000, 0x00000000, 0x00000000 }; return *(const __m256*)&mask; }
template <> HK_FORCE_INLINE __m256 getNegMaskH<3>()   { static HK_ALIGN32( const hkUint32 mask[8] ) = { 0x80000000, 0x80000000, 0x80000000, 0x00000000, 0x80000000, 0x80000000, 0x80000000, 0x00000000 }; return *(const __m256*)&mask; }
template <> HK_FORCE_INLINE __m256 getNegMaskH<4>()   { static HK_ALIGN32( const hkUint32 mask[8] ) = { 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000 }; return *(const __m256*)&mask; }


template <int I> HK_FORCE_INLINE void setNegH(__m256* v0, const __m256* v1, const __m256& mask) { setNegH<I-1>(v0,v1,mask); v0[I-1] = _mm256_xor_ps(v1[I-1],mask); }
template <> HK_FORCE_INLINE void setNegH<1>(__m256* v0, const __m256* v1, const __m256& mask) { v0[0] = _mm256_xor_ps(v1[0],mask); }
}
template <int M> 
template <int N> 
HK_FORCE_INLINE void hkMxVector<M>::setNeg(hkMxVectorParameter v0) 
{ 
	HK_MXVECTOR_VECTOR4_UNSUPPORTED_LENGTH_CHECK; 
	const __m256 maskV = hkMxVector_Implementation::getNegMaskH<N>();
	hkMxVector_Implementation::setNegH<((M+1)>>1)>(m_vec.v, v0.m_vec.v, maskV); 
}

namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void setAbsH(__m256* v0, const __m256* v1, const __m256& mask) { setAbsH<I-1>(v0,v1,mask); v0[I-1] = _mm256_and_ps(v1[I-1],mask); }
template <> HK_FORCE_INLINE void setAbsH<1>(__m256* v0, const __m256* v1, const __m256& mask) { v0[0] = _mm256_and_ps(v1[0],mask); }
}
template <int M> 
HK_FORCE_INLINE void hkMxVector<M>::setAbs(hkMxVectorParameter v0) 
{ 
	static HK_ALIGN32( const hkUint32 mask[8] ) = { 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff };
	hkMxVector_Implementation::setAbsH<((M+1)>>1)>(m_vec.v, v0.m_vec.v, *(const __m256*)&mask); 
}


namespace hkMxVector_Implementation
{
template <int N, int I> struct zeroCompH { HK_FORCE_INLINE static void apply(__m256* v) 
{ 
	zeroCompH<N-1,I>::apply(v); 
	v[N-1] = _mm256_blend_ps(v[N-1], _mm256_setzero_ps(), (0x01 << I) | (0x10 << I)); 
} };
template <int I> struct zeroCompH<1,I> { HK_FORCE_INLINE static void apply(__m256* v) 
{ 
	v[0] = _mm256_blend_ps(v[0], _mm256_setzero_ps(), (0x01 << I) | (0x10 << I)); 
} };
}
template <int M> 
template <int I> 
HK_FORCE_INLINE void hkMxVector<M>::zeroComponent() 
{ 
	HK_MXVECTOR_VECTOR4_SUBINDEX_CHECK;
	hkMxVector_Implementation::zeroCompH<((M+1)>>1),I>::apply(m_vec.v); 
}

MXV_NO_OPERANDS( setZero, _mm256_setzero_ps )

MXV_OP_MXVECTOR( add, _mm256_add_ps )
MXV_OP_MXVECTOR( sub, _mm256_sub_ps )
MXV_OP_MXVECTOR( mul, _mm256_mul_ps )
MXV_OP_MXREAL( mul, _mm256_mul_ps )

MXV_OP_MXVECTOR_MXVECTOR( setAdd, _mm256_add_ps )
MXV_OP_MXVECTOR_MXVECTOR( setSub, _mm256_sub_ps )
MXV_OP_MXVECTOR_MXVECTOR( setMul, _mm256_mul_ps )
MXV_OP_MXVECTOR_MXVECTOR( setDiv, _mm256_div_ps )
MXV_OP_MXVECTOR_MXVECTOR( setMin, _mm256_min_ps )
MXV_OP_MXVECTOR_MXVECTOR( setMax, _mm256_max_ps )
MXV_OP_MXREAL_MXVECTOR( setMul, _mm256_mul_ps )


MXV_OP_MXVECTOR_MXVECTOR_MXVECTOR( setAddMul, _mm256_add_ps, _mm256_mul_ps )
MXV_OP_MXVECTOR_MXVECTOR_MXVECTOR( setSubMul, _mm256_sub_ps, _mm256_mul_ps )
MXV_OP_MXVECTOR_MXVECTOR_MXSINGLE( setAddMul, _mm256_add_ps, _mm256_mul_ps )
MXV_OP_MXVECTOR_MXVECTOR_MXSINGLE( setSubMul, _mm256_sub_ps, _mm256_mul_ps )
MXV_OP_MXVECTOR_MXVECTOR_MXREAL( setSubMul, _mm256_sub_ps, _mm256_mul_ps )
MXV_OP_MXVECTOR_MXSINGLE_MXREAL( setSubMul, _mm256_sub_ps, _mm256_mul_ps )


MXV_OP_MXSINGLE( add, _mm256_add_ps )
MXV_OP_MXSINGLE( sub, _mm256_sub_ps )
MXV_OP_MXSINGLE( mul, _mm256_mul_ps )

MXV_OP_MXREAL_MXVECTOR_SELF( addMul, _mm256_add_ps, _mm256_mul_ps )
MXV_OP_MXVECTOR_MXVECTOR_SELF( addMul, _mm256_add_ps, _mm256_mul_ps )
MXV_OP_MXVECTOR_MXSINGLE_SELF( addMul, _mm256_add_ps, _mm256_mul_ps )

MXV_OP_MXVECTOR_MXSINGLE( setAdd, _mm256_add_ps )
MXV_OP_MXVECTOR_MXSINGLE( setSub, _mm256_sub_ps )
MXV_OP_MXVECTOR_MXSINGLE( setMul, _mm256_mul_ps )
MXV_OP_MXVECTOR_MXSINGLE( setMin, _mm256_min_ps )
MXV_OP_MXVECTOR_MXSINGLE( setMax, _mm256_max_ps )

MXV_OP_MXSINGLE_MXSINGLE( addMul, _mm256_add_ps, _mm256_mul_ps )
MXV_OP_MXSINGLE_MXVECTOR( setSub, _mm256_sub_ps )

MXV_COMPARE( compareEqual, _CMP_EQ_OQ )
MXV_COMPARE( compareNotEqual, _CMP_NEQ_OQ )
MXV_COMPARE( compareLessThan, _CMP_LT_OQ )
MXV_COMPARE( compareGreaterThan, _CMP_GT_OQ )
MXV_COMPARE( compareLessThanEqual, _CMP_LE_OQ )
MXV_COMPARE( compareGreaterThanEqual, _CMP_GE_OQ )

MXV_COMPARE_SINGLE( compareEqual, _CMP_EQ_OQ )
MXV_COMPARE_SINGLE( compareNotEqual, _CMP_NEQ_OQ )
MXV_COMPARE_SINGLE( compareLessThan, _CMP_LT_OQ )
MXV_COMPARE_SINGLE( compareGreaterThan, _CMP_GT_OQ )
MXV_COMPARE_SINGLE( compareLessThanEqual, _CMP_LE_OQ )
MXV_COMPARE_SINGLE( compareGreaterThanEqual, _CMP_GE_OQ )




namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void setCrossH(__m256* v, const __m256* v0, const __m256* v1) { setCrossH<I-1>(v,v0,v1); v[I-1] = hkMxCommon_Implementation::crossH( v0[I-1], v1[I-1]); } 
template <> HK_FORCE_INLINE void setCrossH<1>(__m256* v, const __m256* v0, const __m256* v1) { v[0] = hkMxCommon_Implementation::crossH( v0[0], v1[0]); } 
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::setCross(hkMxVectorParameter v0, hkMxVectorParameter v1)
{
	hkMxVector_Implementation::setCrossH<((M+1)>>1)>(m_vec.v, v0.m_vec.v, v1.m_vec.v);
}

namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void setCrossSH(__m256* v, const __m256& v0, const __m256* v1) { setCrossSH<I-1>(v,v0,v1); v[I-1] = hkMxCommon_Implementation::crossH( v0, v1[I-1]); } 
template <> HK_FORCE_INLINE void setCrossSH<1>(__m256* v, const __m256& v0, const __m256* v1) { v[0] = hkMxCommon_Implementation::crossH( v0, v1[0]); } 
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::setCross(hkMxSingleParameter v0, hkMxVectorParameter v1)
{
	hkMxVector_Implementation::setCrossSH<((M+1)>>1)>(m_vec.v, v0.m_single.s, v1.m_vec.v);
}

namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void setCrossHS(__m256* v, const __m256* v0, const __m256& v1) { setCrossHS<I-1>(v,v0,v1); v[I-1] = hkMxCommon_Implementation::crossH( v0[I-1], v1); } 
template <> HK_FORCE_INLINE void setCrossHS<1>(__m256* v, const __m256* v0, const __m256& v1) { v[0] = hkMxCommon_Implementation::crossH( v0[0], v1); } 
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::setCross(hkMxVectorParameter v0, hkMxSingleParameter v1)
{
	hkMxVector_Implementation::setCrossHS<((M+1)>>1)>(m_vec.v, v0.m_vec.v, v1.m_single.s);
}


namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void setInterpolateH(__m256* v, const __m256* v0, const __m256* v1, const __m256* t) 
{ 
	setInterpolateH<I-1>(v,v0,v1,t); 
	const __m256 d = _mm256_sub_ps(v1[I-1], v0[I-1]); 
	v[I-1] = _mm256_add_ps( v0[I-1], _mm256_mul_ps(d, t[I-1]) ); 
} 
template <> HK_FORCE_INLINE void setInterpolateH<1>(__m256* v, const __m256* v0, const __m256* v1, const __m256* t) 
{ 
	const __m256 d = _mm256_sub_ps(v1[0], v0[0]); 
	v[0] = _mm256_add_ps( v0[0], _mm256_mul_ps(d, t[0]) ); 
} 
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::setInterpolate(hkMxVectorParameter v0, hkMxVectorParameter v1, hkMxRealParameter t)
{
	hkMxVector_Implementation::setInterpolateH<((M+1)>>1)>(m_vec.v, v0.m_vec.v, v1.m_vec.v, t.m_real.r);
}


namespace hkMxVector_Implementation
{
template <int M> HK_FORCE_INLINE void setAddMulH(__m256* v, const __m256* v0, const __m256* v1, hkVector4Parameter v2) 
{ 
	HK_MXVECTOR_MX_NOT_IMPLEMENTED;
}
template <> HK_FORCE_INLINE void setAddMulH<1>(__m256* v, const __m256* v0, const __m256* v1, hkVector4Parameter v2) 
{
	const __m128 a = _mm_shuffle_ps(v2.m_quad, v2.m_quad, _MM_SHUFFLE(0,0,0,0));
	const __m128 b = _mm_shuffle_ps(v2.m_quad, v2.m_quad, _MM_SHUFFLE(1,1,1,1));
	__m256 c = _mm256_insertf128_ps(_mm256_undefined_ps(), a, 0x0);
	       c = _mm256_insertf128_ps(c, b, 0x1);
	v[0] = _mm256_add_ps(v0[0], _mm256_mul_ps(v1[0], c)); 
}
template <> HK_FORCE_INLINE void setAddMulH<2>(__m256* v, const __m256* v0, const __m256* v1, hkVector4Parameter v2) 
{
	const __m128 a = _mm_shuffle_ps(v2.m_quad, v2.m_quad, _MM_SHUFFLE(2,2,2,2));
	const __m128 b = _mm_shuffle_ps(v2.m_quad, v2.m_quad, _MM_SHUFFLE(3,3,3,3));

	setAddMulH<1>(v,v0,v1,v2);

	__m256 c = _mm256_insertf128_ps(_mm256_undefined_ps(), a, 0x0);
	       c = _mm256_insertf128_ps(c, b, 0x1);
	v[1] = _mm256_add_ps(v0[1], _mm256_mul_ps(v1[1], c)); 
}
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::setAddMul(hkMxVectorParameter v0, hkMxVectorParameter v1, hkVector4Parameter v2)
{
	hkMxVector_Implementation::setAddMulH<((M+1)>>1)>(m_vec.v, v0.m_vec.v, v1.m_vec.v, v2);
}

namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void setXYZ_W_H(__m256* v, const __m256* xyz, const __m256* w) { setXYZ_W_H<I-1>(v,xyz,w); v[I-1] = _mm256_blend_ps(xyz[I-1],w[I-1],0x88); }
template <> HK_FORCE_INLINE void setXYZ_W_H<1>(__m256* v, const __m256* xyz, const __m256* w) { v[0] = _mm256_blend_ps(xyz[0],w[0],0x88); }
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::setXYZ_W(hkMxVectorParameter v0, hkMxVectorParameter v1)
{
	hkMxVector_Implementation::setXYZ_W_H<((M+1)>>1)>(m_vec.v, v0.m_vec.v, v1.m_vec.v);
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::setXYZ_W(hkMxVectorParameter v0, hkMxRealParameter r1)
{
	hkMxVector_Implementation::setXYZ_W_H<((M+1)>>1)>(m_vec.v, v0.m_vec.v, r1.m_real.r);
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::setXYZ(hkMxVectorParameter v0)
{
	hkMxVector_Implementation::setXYZ_W_H<((M+1)>>1)>(m_vec.v, v0.m_vec.v, m_vec.v);
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::setW(hkMxVectorParameter v0)
{
	hkMxVector_Implementation::setXYZ_W_H<((M+1)>>1)>(m_vec.v, m_vec.v, v0.m_vec.v);
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::setW(hkMxRealParameter r0)
{
	hkMxVector_Implementation::setXYZ_W_H<((M+1)>>1)>(m_vec.v, m_vec.v, r0.m_real.r);
}

namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void setW_SH(__m256* v, const __m256& w) { setW_SH<I-1>(v,w); v[I-1] = _mm256_blend_ps(v[I-1],w,0x88); }
template <> HK_FORCE_INLINE void setW_SH<1>(__m256* v, const __m256& w) { v[0] = _mm256_blend_ps(v[0],w,0x88); }
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::setW(hkMxSingleParameter v0)
{
	hkMxVector_Implementation::setW_SH<((M+1)>>1)>(m_vec.v, v0.m_single.s);
}

template <int M>
HK_FORCE_INLINE void hkMxVector<M>::storeTransposed4(hkMatrix4& matrix4) const
{
	HK_MXVECTOR_MX_NOT_IMPLEMENTED;
}

template <>
HK_FORCE_INLINE void hkMxVector<1>::storeTransposed4(hkMatrix4& matrix4) const
{
	const __m128 s = _mm256_extractf128_ps(m_vec.v[0], 0x0);

	hkVector4 a; a.m_quad = _mm_shuffle_ps(s,s,_MM_SHUFFLE(0,0,0,0));
	hkVector4 b; b.m_quad = _mm_shuffle_ps(s,s,_MM_SHUFFLE(1,1,1,1));
	hkVector4 c; c.m_quad = _mm_shuffle_ps(s,s,_MM_SHUFFLE(2,2,2,2));
	hkVector4 d; d.m_quad = _mm_shuffle_ps(s,s,_MM_SHUFFLE(3,3,3,3));

	matrix4.setCols(a,b,c,d);
	// todo. for debug: set .yzw to 0x23456789
}

template <>
HK_FORCE_INLINE void hkMxVector<2>::storeTransposed4(hkMatrix4& matrix4) const
{
	hkVector4 a; a.m_quad = _mm256_extractf128_ps(m_vec.v[0], 0x0);
	hkVector4 b; b.m_quad = _mm256_extractf128_ps(m_vec.v[0], 0x1);
	matrix4.setRows(a,b,b,b);
}

template <>
HK_FORCE_INLINE void hkMxVector<3>::storeTransposed4(hkMatrix4& matrix4) const
{
	hkVector4 a; a.m_quad = _mm256_extractf128_ps(m_vec.v[0], 0x0);
	hkVector4 b; b.m_quad = _mm256_extractf128_ps(m_vec.v[0], 0x1);
	hkVector4 c; c.m_quad = _mm256_extractf128_ps(m_vec.v[1], 0x0);
	matrix4.setRows(a,b,c,c);
}

template <>
HK_FORCE_INLINE void hkMxVector<4>::storeTransposed4(hkMatrix4& matrix4) const
{
	hkVector4 a; a.m_quad = _mm256_extractf128_ps(m_vec.v[0], 0x0);
	hkVector4 b; b.m_quad = _mm256_extractf128_ps(m_vec.v[0], 0x1);
	hkVector4 c; c.m_quad = _mm256_extractf128_ps(m_vec.v[1], 0x0);
	hkVector4 d; d.m_quad = _mm256_extractf128_ps(m_vec.v[1], 0x1);
	matrix4.setRows(a,b,c,d);
}


namespace hkMxVector_Implementation
{
template <int M, int N> struct hAddH { HK_FORCE_INLINE static void apply(hkMxVectorParameter v, hkVector4& addsOut) 
{ 
	HK_MXVECTOR_MX_NOT_IMPLEMENTED; 
} };
template <> struct hAddH<4,4> { HK_FORCE_INLINE static void apply(const hkMxVector<4>& v, hkVector4& addsOut) 
{ 
	const __m256 a  = _mm256_hadd_ps(v.m_vec.v[0],v.m_vec.v[0]);
	const __m256 aa = _mm256_hadd_ps(a,a);
	const __m256 b  = _mm256_hadd_ps(v.m_vec.v[1],v.m_vec.v[1]);
	const __m256 bb = _mm256_hadd_ps(b,b);

	hkSimdReal sa; sa.m_real = _mm256_extractf128_ps(aa, 0x0);
	hkSimdReal sb; sb.m_real = _mm256_extractf128_ps(aa, 0x1);
	hkSimdReal sc; sc.m_real = _mm256_extractf128_ps(bb, 0x0);
	hkSimdReal sd; sd.m_real = _mm256_extractf128_ps(bb, 0x1);

	addsOut.set(sa,sb,sc,sd);
} };
template <> struct hAddH<2,3> { HK_FORCE_INLINE static void apply(const hkMxVector<2>& v, hkVector4& addsOut) 
{ 
	const __m256 a  = _mm256_hadd_ps(v.m_vec.v[0],v.m_vec.v[0]);
	const __m256 aa = _mm256_add_ps( _mm256_shuffle_ps(v.m_vec.v[0],v.m_vec.v[0],_MM256_PERMUTE(2,2,2,2)), _mm256_shuffle_ps(a,a,_MM256_PERMUTE(0,0,0,0)));

	hkSimdReal sa; sa.m_real = _mm256_extractf128_ps(aa, 0x0);
	hkSimdReal sb; sb.m_real = _mm256_extractf128_ps(aa, 0x1);

	addsOut.set(sa,sb,sb,sb);
} };
template <> struct hAddH<2,4> { HK_FORCE_INLINE static void apply(const hkMxVector<2>& v, hkVector4& addsOut) 
{ 
	const __m256 a  = _mm256_hadd_ps(v.m_vec.v[0],v.m_vec.v[0]);
	const __m256 aa = _mm256_hadd_ps(a,a);

	hkSimdReal sa; sa.m_real = _mm256_extractf128_ps(aa, 0x0);
	hkSimdReal sb; sb.m_real = _mm256_extractf128_ps(aa, 0x1);

	addsOut.set(sa,sb,sb,sb);
} };
template <> struct hAddH<4,3> { HK_FORCE_INLINE static void apply(const hkMxVector<4>& v, hkVector4& addsOut) 
{ 
	const __m256 a  = _mm256_hadd_ps(v.m_vec.v[0],v.m_vec.v[0]);
	const __m256 aa = _mm256_add_ps( _mm256_shuffle_ps(v.m_vec.v[0],v.m_vec.v[0],_MM256_PERMUTE(2,2,2,2)), _mm256_shuffle_ps(a,a,_MM256_PERMUTE(0,0,0,0)));
	const __m256 b  = _mm256_hadd_ps(v.m_vec.v[1],v.m_vec.v[1]);
	const __m256 bb = _mm256_add_ps( _mm256_shuffle_ps(v.m_vec.v[1],v.m_vec.v[1],_MM256_PERMUTE(2,2,2,2)), _mm256_shuffle_ps(b,b,_MM256_PERMUTE(0,0,0,0)));

	hkSimdReal sa; sa.m_real = _mm256_extractf128_ps(aa, 0x0);
	hkSimdReal sb; sb.m_real = _mm256_extractf128_ps(aa, 0x1);
	hkSimdReal sc; sc.m_real = _mm256_extractf128_ps(bb, 0x0);
	hkSimdReal sd; sd.m_real = _mm256_extractf128_ps(bb, 0x1);

	addsOut.set(sa,sb,sc,sd);
} };
template <> struct hAddH<3,4> { HK_FORCE_INLINE static void apply(const hkMxVector<3>& v, hkVector4& addsOut) 
{ 
	const __m256 a  = _mm256_hadd_ps(v.m_vec.v[0],v.m_vec.v[0]);
	const __m256 aa = _mm256_hadd_ps(a,a);
	const __m256 b  = _mm256_hadd_ps(v.m_vec.v[1],v.m_vec.v[1]);
	const __m256 bb = _mm256_hadd_ps(b,b);

	hkSimdReal sa; sa.m_real = _mm256_extractf128_ps(aa, 0x0);
	hkSimdReal sb; sb.m_real = _mm256_extractf128_ps(aa, 0x1);
	hkSimdReal sc; sc.m_real = _mm256_extractf128_ps(bb, 0x0);

	addsOut.set(sa,sb,sc,sc);
} };
template <> struct hAddH<3,3> { HK_FORCE_INLINE static void apply(const hkMxVector<3>& v, hkVector4& addsOut) 
{ 
	const __m256 a  = _mm256_hadd_ps(v.m_vec.v[0],v.m_vec.v[0]);
	const __m256 aa = _mm256_add_ps( _mm256_shuffle_ps(v.m_vec.v[0],v.m_vec.v[0],_MM256_PERMUTE(2,2,2,2)), _mm256_shuffle_ps(a,a,_MM256_PERMUTE(0,0,0,0)));
	const __m256 b  = _mm256_hadd_ps(v.m_vec.v[1],v.m_vec.v[1]);
	const __m256 bb = _mm256_add_ps( _mm256_shuffle_ps(v.m_vec.v[1],v.m_vec.v[1],_MM256_PERMUTE(2,2,2,2)), _mm256_shuffle_ps(b,b,_MM256_PERMUTE(0,0,0,0)));

	hkSimdReal sa; sa.m_real = _mm256_extractf128_ps(aa, 0x0);
	hkSimdReal sb; sb.m_real = _mm256_extractf128_ps(aa, 0x1);
	hkSimdReal sc; sc.m_real = _mm256_extractf128_ps(bb, 0x0);

	addsOut.set(sa,sb,sc,sc);
} };
template <int N> struct hAddH<1,N> { HK_FORCE_INLINE static void apply(const hkMxVector<1>& v, hkVector4& addsOut) 
{ 
	const hkVector4 a = v.getVector<0>();
	addsOut.setHorizontalAdd<N>(a);
} };
}
template <int M>
template <int N>
HK_FORCE_INLINE void hkMxVector<M>::horizontalAdd( hkVector4& addsOut ) const
{
	hkMxVector_Implementation::hAddH<M,N>::apply(*this, addsOut);
}



namespace hkMxVector_Implementation
{
template <int M, int N> struct hMinH { HK_FORCE_INLINE static void apply(hkMxVectorParameter v, hkVector4& minsOut) 
{ 
	HK_MXVECTOR_MX_NOT_IMPLEMENTED; 
} };
template <> struct hMinH<4,4> { HK_FORCE_INLINE static void apply(const hkMxVector<4>& v, hkVector4& minsOut) 
{ 
	const __m256 a0 = _mm256_min_ps( _mm256_shuffle_ps( v.m_vec.v[0], v.m_vec.v[0],_MM256_PERMUTE(1,0,3,2)), v.m_vec.v[0]); // yxwz+xyzw = xy xy zw zw
	const __m256 a1 = _mm256_shuffle_ps(a0,a0, _MM256_PERMUTE(2,3,0,1)); // = zw zw xy xy
	const __m256 aa = _mm256_min_ps( a0, a1 ); // xywz all 4 two times

	const __m256 b0 = _mm256_min_ps( _mm256_shuffle_ps( v.m_vec.v[1], v.m_vec.v[1],_MM256_PERMUTE(1,0,3,2)), v.m_vec.v[1]); // yxwz+xyzw = xy xy zw zw
	const __m256 b1 = _mm256_shuffle_ps(b0,b0, _MM256_PERMUTE(2,3,0,1)); // = zw zw xy xy
	const __m256 bb = _mm256_min_ps( b0, b1 ); // xywz all 4 two times

	hkSimdReal sa; sa.m_real = _mm256_extractf128_ps(aa, 0x0);
	hkSimdReal sb; sb.m_real = _mm256_extractf128_ps(aa, 0x1);
	hkSimdReal sc; sc.m_real = _mm256_extractf128_ps(bb, 0x0);
	hkSimdReal sd; sd.m_real = _mm256_extractf128_ps(bb, 0x1);

	minsOut.set(sa,sb,sc,sd);
} };
template <> struct hMinH<4,3> { HK_FORCE_INLINE static void apply(const hkMxVector<4>& v, hkVector4& minsOut) 
{ 
	const __m256 axy = _mm256_min_ps( _mm256_shuffle_ps(v.m_vec.v[0],v.m_vec.v[0],_MM256_PERMUTE(1,1,1,1)), _mm256_shuffle_ps(v.m_vec.v[0],v.m_vec.v[0],_MM256_PERMUTE(0,0,0,0)));
	const __m256 aa = _mm256_min_ps( _mm256_shuffle_ps(v.m_vec.v[0],v.m_vec.v[0],_MM256_PERMUTE(2,2,2,2)), axy);

	const __m256 bxy = _mm256_min_ps( _mm256_shuffle_ps(v.m_vec.v[1],v.m_vec.v[1],_MM256_PERMUTE(1,1,1,1)), _mm256_shuffle_ps(v.m_vec.v[1],v.m_vec.v[1],_MM256_PERMUTE(0,0,0,0)));
	const __m256 bb = _mm256_min_ps( _mm256_shuffle_ps(v.m_vec.v[1],v.m_vec.v[1],_MM256_PERMUTE(2,2,2,2)), bxy);

	hkSimdReal sa; sa.m_real = _mm256_extractf128_ps(aa, 0x0);
	hkSimdReal sb; sb.m_real = _mm256_extractf128_ps(aa, 0x1);
	hkSimdReal sc; sc.m_real = _mm256_extractf128_ps(bb, 0x0);
	hkSimdReal sd; sd.m_real = _mm256_extractf128_ps(bb, 0x1);

	minsOut.set(sa,sb,sc,sd);
} };
template <> struct hMinH<3,4> { HK_FORCE_INLINE static void apply(const hkMxVector<3>& v, hkVector4& minsOut) 
{ 
	const __m256 a0 = _mm256_min_ps( _mm256_shuffle_ps( v.m_vec.v[0], v.m_vec.v[0],_MM256_PERMUTE(1,0,3,2)), v.m_vec.v[0]); // yxwz+xyzw = xy xy zw zw
	const __m256 a1 = _mm256_shuffle_ps(a0,a0, _MM256_PERMUTE(2,3,0,1)); // = zw zw xy xy
	const __m256 aa = _mm256_min_ps( a0, a1 ); // xywz all 4 two times

	const __m256 b0 = _mm256_min_ps( _mm256_shuffle_ps( v.m_vec.v[1], v.m_vec.v[1],_MM256_PERMUTE(1,0,3,2)), v.m_vec.v[1]); // yxwz+xyzw = xy xy zw zw
	const __m256 b1 = _mm256_shuffle_ps(b0,b0, _MM256_PERMUTE(2,3,0,1)); // = zw zw xy xy
	const __m256 bb = _mm256_min_ps( b0, b1 ); // xywz all 4 two times

	hkSimdReal sa; sa.m_real = _mm256_extractf128_ps(aa, 0x0);
	hkSimdReal sb; sb.m_real = _mm256_extractf128_ps(aa, 0x1);
	hkSimdReal sc; sc.m_real = _mm256_extractf128_ps(bb, 0x0);

	minsOut.set(sa,sb,sc,sc);
} };
template <> struct hMinH<3,3> { HK_FORCE_INLINE static void apply(const hkMxVector<3>& v, hkVector4& minsOut) 
{ 
	const __m256 axy = _mm256_min_ps( _mm256_shuffle_ps(v.m_vec.v[0],v.m_vec.v[0],_MM256_PERMUTE(1,1,1,1)), _mm256_shuffle_ps(v.m_vec.v[0],v.m_vec.v[0],_MM256_PERMUTE(0,0,0,0)));
	const __m256 aa = _mm256_min_ps( _mm256_shuffle_ps(v.m_vec.v[0],v.m_vec.v[0],_MM256_PERMUTE(2,2,2,2)), axy);

	const __m256 bxy = _mm256_min_ps( _mm256_shuffle_ps(v.m_vec.v[1],v.m_vec.v[1],_MM256_PERMUTE(1,1,1,1)), _mm256_shuffle_ps(v.m_vec.v[1],v.m_vec.v[1],_MM256_PERMUTE(0,0,0,0)));
	const __m256 bb = _mm256_min_ps( _mm256_shuffle_ps(v.m_vec.v[1],v.m_vec.v[1],_MM256_PERMUTE(2,2,2,2)), bxy);

	hkSimdReal sa; sa.m_real = _mm256_extractf128_ps(aa, 0x0);
	hkSimdReal sb; sb.m_real = _mm256_extractf128_ps(aa, 0x1);
	hkSimdReal sc; sc.m_real = _mm256_extractf128_ps(bb, 0x0);

	minsOut.set(sa,sb,sc,sc);
} };
template <> struct hMinH<2,4> { HK_FORCE_INLINE static void apply(const hkMxVector<2>& v, hkVector4& minsOut) 
{ 
	const __m256 a0 = _mm256_min_ps( _mm256_shuffle_ps( v.m_vec.v[0], v.m_vec.v[0],_MM256_PERMUTE(1,0,3,2)), v.m_vec.v[0]); // yxwz+xyzw = xy xy zw zw
	const __m256 a1 = _mm256_shuffle_ps(a0,a0, _MM256_PERMUTE(2,3,0,1)); // = zw zw xy xy
	const __m256 aa = _mm256_min_ps( a0, a1 ); // xywz all 4 two times

	hkSimdReal sa; sa.m_real = _mm256_extractf128_ps(aa, 0x0);
	hkSimdReal sb; sb.m_real = _mm256_extractf128_ps(aa, 0x1);

	minsOut.set(sa,sb,sb,sb);
} };
template <> struct hMinH<2,3> { HK_FORCE_INLINE static void apply(const hkMxVector<2>& v, hkVector4& minsOut) 
{ 
	const __m256 axy = _mm256_min_ps( _mm256_shuffle_ps(v.m_vec.v[0],v.m_vec.v[0],_MM256_PERMUTE(1,1,1,1)), _mm256_shuffle_ps(v.m_vec.v[0],v.m_vec.v[0],_MM256_PERMUTE(0,0,0,0)));
	const __m256 aa = _mm256_min_ps( _mm256_shuffle_ps(v.m_vec.v[0],v.m_vec.v[0],_MM256_PERMUTE(2,2,2,2)), axy);

	hkSimdReal sa; sa.m_real = _mm256_extractf128_ps(aa, 0x0);
	hkSimdReal sb; sb.m_real = _mm256_extractf128_ps(aa, 0x1);

	minsOut.set(sa,sb,sb,sb);
} };
template <int N> struct hMinH<1,N> { HK_FORCE_INLINE static void apply(const hkMxVector<1>& v, hkVector4& minsOut) 
{ 
	const hkVector4 a = v.getVector<0>();
	minsOut.setHorizontalMin<N>(a);
} };
}
template <int M>
template <int N>
HK_FORCE_INLINE void hkMxVector<M>::horizontalMin( hkVector4& minsOut ) const
{
	hkMxVector_Implementation::hMinH<M,N>::apply(*this, minsOut);
}


namespace hkMxVector_Implementation
{
template <int M, int N> struct hMaxH { HK_FORCE_INLINE static void apply(hkMxVectorParameter v, hkVector4& maxsOut) 
{ 
	HK_MXVECTOR_MX_NOT_IMPLEMENTED; 
} };
template <> struct hMaxH<4,4> { HK_FORCE_INLINE static void apply(const hkMxVector<4>& v, hkVector4& maxsOut) 
{ 
	const __m256 a0 = _mm256_max_ps( _mm256_shuffle_ps( v.m_vec.v[0], v.m_vec.v[0],_MM256_PERMUTE(1,0,3,2)), v.m_vec.v[0]); // yxwz+xyzw = xy xy zw zw
	const __m256 a1 = _mm256_shuffle_ps(a0,a0, _MM256_PERMUTE(2,3,0,1)); // = zw zw xy xy
	const __m256 aa = _mm256_max_ps( a0, a1 ); // xywz all 4 two times

	const __m256 b0 = _mm256_max_ps( _mm256_shuffle_ps( v.m_vec.v[1], v.m_vec.v[1],_MM256_PERMUTE(1,0,3,2)), v.m_vec.v[1]); // yxwz+xyzw = xy xy zw zw
	const __m256 b1 = _mm256_shuffle_ps(b0,b0, _MM256_PERMUTE(2,3,0,1)); // = zw zw xy xy
	const __m256 bb = _mm256_max_ps( b0, b1 ); // xywz all 4 two times

	hkSimdReal sa; sa.m_real = _mm256_extractf128_ps(aa, 0x0);
	hkSimdReal sb; sb.m_real = _mm256_extractf128_ps(aa, 0x1);
	hkSimdReal sc; sc.m_real = _mm256_extractf128_ps(bb, 0x0);
	hkSimdReal sd; sd.m_real = _mm256_extractf128_ps(bb, 0x1);

	maxsOut.set(sa,sb,sc,sd);
} };
template <> struct hMaxH<4,3> { HK_FORCE_INLINE static void apply(const hkMxVector<4>& v, hkVector4& maxsOut) 
{ 
	const __m256 axy = _mm256_max_ps( _mm256_shuffle_ps(v.m_vec.v[0],v.m_vec.v[0],_MM256_PERMUTE(1,1,1,1)), _mm256_shuffle_ps(v.m_vec.v[0],v.m_vec.v[0],_MM256_PERMUTE(0,0,0,0)));
	const __m256 aa = _mm256_max_ps( _mm256_shuffle_ps(v.m_vec.v[0],v.m_vec.v[0],_MM256_PERMUTE(2,2,2,2)), axy);

	const __m256 bxy = _mm256_max_ps( _mm256_shuffle_ps(v.m_vec.v[1],v.m_vec.v[1],_MM256_PERMUTE(1,1,1,1)), _mm256_shuffle_ps(v.m_vec.v[1],v.m_vec.v[1],_MM256_PERMUTE(0,0,0,0)));
	const __m256 bb = _mm256_max_ps( _mm256_shuffle_ps(v.m_vec.v[1],v.m_vec.v[1],_MM256_PERMUTE(2,2,2,2)), bxy);

	hkSimdReal sa; sa.m_real = _mm256_extractf128_ps(aa, 0x0);
	hkSimdReal sb; sb.m_real = _mm256_extractf128_ps(aa, 0x1);
	hkSimdReal sc; sc.m_real = _mm256_extractf128_ps(bb, 0x0);
	hkSimdReal sd; sd.m_real = _mm256_extractf128_ps(bb, 0x1);

	maxsOut.set(sa,sb,sc,sd);
} };
template <> struct hMaxH<3,4> { HK_FORCE_INLINE static void apply(const hkMxVector<3>& v, hkVector4& maxsOut) 
{ 
	const __m256 a0 = _mm256_max_ps( _mm256_shuffle_ps( v.m_vec.v[0], v.m_vec.v[0],_MM256_PERMUTE(1,0,3,2)), v.m_vec.v[0]); // yxwz+xyzw = xy xy zw zw
	const __m256 a1 = _mm256_shuffle_ps(a0,a0, _MM256_PERMUTE(2,3,0,1)); // = zw zw xy xy
	const __m256 aa = _mm256_max_ps( a0, a1 ); // xywz all 4 two times

	const __m256 b0 = _mm256_max_ps( _mm256_shuffle_ps( v.m_vec.v[1], v.m_vec.v[1],_MM256_PERMUTE(1,0,3,2)), v.m_vec.v[1]); // yxwz+xyzw = xy xy zw zw
	const __m256 b1 = _mm256_shuffle_ps(b0,b0, _MM256_PERMUTE(2,3,0,1)); // = zw zw xy xy
	const __m256 bb = _mm256_max_ps( b0, b1 ); // xywz all 4 two times

	hkSimdReal sa; sa.m_real = _mm256_extractf128_ps(aa, 0x0);
	hkSimdReal sb; sb.m_real = _mm256_extractf128_ps(aa, 0x1);
	hkSimdReal sc; sc.m_real = _mm256_extractf128_ps(bb, 0x0);

	maxsOut.set(sa,sb,sc,sc);
} };
template <> struct hMaxH<3,3> { HK_FORCE_INLINE static void apply(const hkMxVector<3>& v, hkVector4& maxsOut) 
{ 
	const __m256 axy = _mm256_max_ps( _mm256_shuffle_ps(v.m_vec.v[0],v.m_vec.v[0],_MM256_PERMUTE(1,1,1,1)), _mm256_shuffle_ps(v.m_vec.v[0],v.m_vec.v[0],_MM256_PERMUTE(0,0,0,0)));
	const __m256 aa = _mm256_max_ps( _mm256_shuffle_ps(v.m_vec.v[0],v.m_vec.v[0],_MM256_PERMUTE(2,2,2,2)), axy);

	const __m256 bxy = _mm256_max_ps( _mm256_shuffle_ps(v.m_vec.v[1],v.m_vec.v[1],_MM256_PERMUTE(1,1,1,1)), _mm256_shuffle_ps(v.m_vec.v[1],v.m_vec.v[1],_MM256_PERMUTE(0,0,0,0)));
	const __m256 bb = _mm256_max_ps( _mm256_shuffle_ps(v.m_vec.v[1],v.m_vec.v[1],_MM256_PERMUTE(2,2,2,2)), bxy);

	hkSimdReal sa; sa.m_real = _mm256_extractf128_ps(aa, 0x0);
	hkSimdReal sb; sb.m_real = _mm256_extractf128_ps(aa, 0x1);
	hkSimdReal sc; sc.m_real = _mm256_extractf128_ps(bb, 0x0);

	maxsOut.set(sa,sb,sc,sc);
} };
template <> struct hMaxH<2,4> { HK_FORCE_INLINE static void apply(const hkMxVector<2>& v, hkVector4& maxsOut) 
{ 
	const __m256 a0 = _mm256_max_ps( _mm256_shuffle_ps( v.m_vec.v[0], v.m_vec.v[0],_MM256_PERMUTE(1,0,3,2)), v.m_vec.v[0]); // yxwz+xyzw = xy xy zw zw
	const __m256 a1 = _mm256_shuffle_ps(a0,a0, _MM256_PERMUTE(2,3,0,1)); // = zw zw xy xy
	const __m256 aa = _mm256_max_ps( a0, a1 ); // xywz all 4 two times

	hkSimdReal sa; sa.m_real = _mm256_extractf128_ps(aa, 0x0);
	hkSimdReal sb; sb.m_real = _mm256_extractf128_ps(aa, 0x1);

	maxsOut.set(sa,sb,sb,sb);
} };
template <> struct hMaxH<2,3> { HK_FORCE_INLINE static void apply(const hkMxVector<2>& v, hkVector4& maxsOut) 
{ 
	const __m256 axy = _mm256_max_ps( _mm256_shuffle_ps(v.m_vec.v[0],v.m_vec.v[0],_MM256_PERMUTE(1,1,1,1)), _mm256_shuffle_ps(v.m_vec.v[0],v.m_vec.v[0],_MM256_PERMUTE(0,0,0,0)));
	const __m256 aa = _mm256_max_ps( _mm256_shuffle_ps(v.m_vec.v[0],v.m_vec.v[0],_MM256_PERMUTE(2,2,2,2)), axy);

	hkSimdReal sa; sa.m_real = _mm256_extractf128_ps(aa, 0x0);
	hkSimdReal sb; sb.m_real = _mm256_extractf128_ps(aa, 0x1);

	maxsOut.set(sa,sb,sb,sb);
} };
template <int N> struct hMaxH<1,N> { HK_FORCE_INLINE static void apply(const hkMxVector<1>& v, hkVector4& maxsOut) 
{ 
	const hkVector4 a = v.getVector<0>();
	maxsOut.setHorizontalMax<N>(a);
} };
}
template <int M>
template <int N>
HK_FORCE_INLINE void hkMxVector<M>::horizontalMax( hkVector4& maxsOut ) const
{
	hkMxVector_Implementation::hMaxH<M,N>::apply(*this, maxsOut);
}


namespace hkMxVector_Implementation
{
template <int I, int N> struct reduceAddH { HK_FORCE_INLINE static __m256 apply(const __m256* v) 
{ 
	const __m256 r = reduceAddH<I-2,N+1>::apply(v); 
	return _mm256_add_ps(r, _mm256_add_ps(v[N], _mm256_permute2f128_ps(v[N], v[N], _MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_HIGH)))); 
} };
template <int N> struct reduceAddH<2,N> { HK_FORCE_INLINE static __m256 apply(const __m256* v) 
{ 
	return _mm256_add_ps(v[N], _mm256_permute2f128_ps(v[N], v[N], _MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_HIGH))); 
} };
template <int N> struct reduceAddH<1,N> { HK_FORCE_INLINE static __m256 apply(const __m256* v) 
{ 
	return v[N]; 
} };
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::reduceAdd(hkVector4& addOut) const
{
	const __m256 r = hkMxVector_Implementation::reduceAddH<M,0>::apply(m_vec.v);
	addOut.m_quad = _mm256_extractf128_ps(r, 0x0);
}


namespace hkMxVector_Implementation
{
template <int I, int N> struct dotH { HK_FORCE_INLINE static void apply(const __m256* v, const __m256* v0, __m256* dotsOut) 
{ 
	dotH<I-1,N>::apply(v, v0, dotsOut); 
	dotsOut[I-1] = hkMxCommon_Implementation::dotProdH<N>(v[I-1], v0[I-1]); 
} };
template <int N> struct dotH<1,N> { HK_FORCE_INLINE static void apply(const __m256* v, const __m256* v0, __m256* dotsOut) 
{ 
	dotsOut[0] = hkMxCommon_Implementation::dotProdH<N>(v[0], v0[0]); 
} };
}
template <int M> 
template <int N> 
HK_FORCE_INLINE void hkMxVector<M>::dot(hkMxVectorParameter v, hkMxReal<M>& dotsOut) const 
{ 
	hkMxVector_Implementation::dotH<((M+1)>>1),N>::apply(m_vec.v, v.m_vec.v, dotsOut.m_real.r); 
}


namespace hkMxVector_Implementation
{
template <int I, int N> struct dotHSingle { HK_FORCE_INLINE static void apply(const __m256* v, const __m256& v0, __m256* dotsOut) 
{ 
	dotHSingle<I-1,N>::apply(v, v0, dotsOut); 
	dotsOut[I-1] = hkMxCommon_Implementation::dotProdH<N>(v[I-1],v0); 
} };
template <int N> struct dotHSingle<1,N> { HK_FORCE_INLINE static void apply(const __m256* v, const __m256& v0, __m256* dotsOut) 
{ 
	dotsOut[0] = hkMxCommon_Implementation::dotProdH<N>(v[0],v0); 
} };
}
template <int M> 
template <int N> 
HK_FORCE_INLINE void hkMxVector<M>::dot(hkMxSingleParameter v, hkMxReal<M>& dotsOut) const 
{ 
	hkMxVector_Implementation::dotHSingle<((M+1)>>1),N>::apply(m_vec.v, v.m_single.s, dotsOut.m_real.r); 
}


namespace hkMxVector_Implementation
{
template <int I, int N, hkUint32 byteAddressIncrement> struct gatherH {
HK_FORCE_INLINE static void apply(__m256* v, const __m128* HK_RESTRICT base)
{
	v[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), *hkAddByteOffsetConst( base, (N*2)*byteAddressIncrement ), 0x0);
	v[N] = _mm256_insertf128_ps(v[N], *hkAddByteOffsetConst( base, (N*2+1)*byteAddressIncrement ), 0x1);
	gatherH<I-2, N+1, byteAddressIncrement>::apply(v, base);
} };
template <int N, hkUint32 byteAddressIncrement> struct gatherH<2, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(__m256* v, const __m128* HK_RESTRICT base)
{
	v[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), *hkAddByteOffsetConst( base, (N*2)*byteAddressIncrement ), 0x0);
	v[N] = _mm256_insertf128_ps(v[N], *hkAddByteOffsetConst( base, (N*2+1)*byteAddressIncrement ), 0x1);
} };
template <int N, hkUint32 byteAddressIncrement> struct gatherH<1, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(__m256* v, const __m128* HK_RESTRICT base)
{
	v[N] = _mm256_broadcast_ps(hkAddByteOffsetConst( base, (N*2)*byteAddressIncrement ));
} };
}
template <int M>
template <hkUint32 byteAddressIncrement>
HK_FORCE_INLINE void hkMxVector<M>::gather(const hkVector4* HK_RESTRICT base)
{
	hkMxVector_Implementation::gatherH<M, 0, byteAddressIncrement>::apply(m_vec.v, &(base->m_quad));
}


namespace hkMxVector_Implementation
{
template <int I, int N, hkUint32 byteAddressIncrement> struct gatherUintH {
HK_FORCE_INLINE static void apply(__m256* v, const __m128* HK_RESTRICT base, const hkUint16* indices)
{
	v[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), *hkAddByteOffsetConst( base, indices[N*2]*byteAddressIncrement ), 0x0);
	v[N] = _mm256_insertf128_ps(v[N], *hkAddByteOffsetConst( base, indices[N*2+1]*byteAddressIncrement ), 0x1);
	gatherUintH<I-2, N+1, byteAddressIncrement>::apply(v, base, indices);
} };
template <int N, hkUint32 byteAddressIncrement> struct gatherUintH<2, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(__m256* v, const __m128* HK_RESTRICT base, const hkUint16* indices)
{
	v[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), *hkAddByteOffsetConst( base, indices[N*2]*byteAddressIncrement ), 0x0);
	v[N] = _mm256_insertf128_ps(v[N], *hkAddByteOffsetConst( base, indices[N*2+1]*byteAddressIncrement ), 0x1);
} };
template <int N, hkUint32 byteAddressIncrement> struct gatherUintH<1, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(__m256* v, const __m128* HK_RESTRICT base, const hkUint16* indices)
{
	v[N] = _mm256_broadcast_ps(hkAddByteOffsetConst( base, indices[N*2]*byteAddressIncrement ));
} };
}
template <int M>
template <hkUint32 byteAddressIncrement>
HK_FORCE_INLINE void hkMxVector<M>::gather(const hkVector4* HK_RESTRICT base, const hkUint16* indices)
{
	hkMxVector_Implementation::gatherUintH<M, 0, byteAddressIncrement>::apply(m_vec.v, &(base->m_quad), indices);
}


namespace hkMxVector_Implementation
{
template <int I, int N, hkUint32 byteAddressIncrement> struct gatherIntH {
HK_FORCE_INLINE static void apply(__m256* v, const __m128* HK_RESTRICT base, const hkInt32* indices)
{
	v[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), *hkAddByteOffsetConst( base, indices[N*2]*byteAddressIncrement ), 0x0);
	v[N] = _mm256_insertf128_ps(v[N], *hkAddByteOffsetConst( base, indices[N*2+1]*byteAddressIncrement ), 0x1);
	gatherIntH<I-2, N+1, byteAddressIncrement>::apply(v, base, indices);
} };
template <int N, hkUint32 byteAddressIncrement> struct gatherIntH<2, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(__m256* v, const __m128* HK_RESTRICT base, const hkInt32* indices)
{
	v[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), *hkAddByteOffsetConst( base, indices[N*2]*byteAddressIncrement ), 0x0);
	v[N] = _mm256_insertf128_ps(v[N], *hkAddByteOffsetConst( base, indices[N*2+1]*byteAddressIncrement ), 0x1);
} };
template <int N, hkUint32 byteAddressIncrement> struct gatherIntH<1, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(__m256* v, const __m128* HK_RESTRICT base, const hkInt32* indices)
{
	v[N] = _mm256_broadcast_ps(hkAddByteOffsetConst( base, indices[N*2]*byteAddressIncrement ));
} };
}
template <int M>
template <hkUint32 byteAddressIncrement>
HK_FORCE_INLINE void hkMxVector<M>::gather(const hkVector4* HK_RESTRICT base, const hkInt32* indices)
{
	hkMxVector_Implementation::gatherIntH<M, 0, byteAddressIncrement>::apply(m_vec.v, &(base->m_quad), indices);
}


namespace hkMxVector_Implementation
{
template <int I, int N, hkUint32 byteOffset> struct gatherWithOffsetH {
HK_FORCE_INLINE static void apply(__m256* v, const void** base)
{
	const __m128* HK_RESTRICT ptr0 = (const __m128*)hkAddByteOffsetConst(base[N*2], byteOffset); 
	const __m128* HK_RESTRICT ptr1 = (const __m128*)hkAddByteOffsetConst(base[N*2+1], byteOffset); 
	gatherWithOffsetH<I-2, N+1, byteOffset>::apply(v, base);
	v[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), *ptr0, 0x0);
	v[N] = _mm256_insertf128_ps(v[N], *ptr1, 0x1);
} };
template <int N, hkUint32 byteOffset> struct gatherWithOffsetH<2, N, byteOffset> {
HK_FORCE_INLINE static void apply(__m256* v, const void** base)
{
	const __m128* HK_RESTRICT ptr0 = (const __m128*)hkAddByteOffsetConst(base[N*2], byteOffset); 
	const __m128* HK_RESTRICT ptr1 = (const __m128*)hkAddByteOffsetConst(base[N*2+1], byteOffset); 
	v[N] = _mm256_insertf128_ps(_mm256_undefined_ps(), *ptr0, 0x0);
	v[N] = _mm256_insertf128_ps(v[N], *ptr1, 0x1);
} };
template <int N, hkUint32 byteOffset> struct gatherWithOffsetH<1, N, byteOffset> {
HK_FORCE_INLINE static void apply(__m256* v, const void** base)
{
	const __m128* HK_RESTRICT ptr = (const __m128*)hkAddByteOffsetConst(base[N*2], byteOffset); 
	v[N] = _mm256_broadcast_ps(ptr);
} };
}
template <int M>
template <hkUint32 byteOffset> 
HK_FORCE_INLINE void hkMxVector<M>::gatherWithOffset(const void* base[M])
{
	hkMxVector_Implementation::gatherWithOffsetH<M, 0, byteOffset>::apply(m_vec.v, base);
}





namespace hkMxVector_Implementation
{
template <int I, int N, hkUint32 byteAddressIncrement> struct scatterUintH {
HK_FORCE_INLINE static void apply(const __m256* v, __m128* base, const hkUint16* indices)
{
	__m128* HK_RESTRICT ptr0 = hkAddByteOffset(base, indices[N*2] * byteAddressIncrement);
	__m128* HK_RESTRICT ptr1 = hkAddByteOffset(base, indices[N*2+1] * byteAddressIncrement);
	scatterUintH<I-2, N+1, byteAddressIncrement>::apply(v, base, indices);
	*ptr0 = _mm256_extractf128_ps(v[N], 0x0);
	*ptr1 = _mm256_extractf128_ps(v[N], 0x1);
} };
template <int N, hkUint32 byteAddressIncrement> struct scatterUintH<2, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const __m256* v, __m128* base, const hkUint16* indices)
{
	__m128* HK_RESTRICT ptr0 = hkAddByteOffset(base, indices[N*2] * byteAddressIncrement);
	__m128* HK_RESTRICT ptr1 = hkAddByteOffset(base, indices[N*2+1] * byteAddressIncrement);
	*ptr0 = _mm256_extractf128_ps(v[N], 0x0);
	*ptr1 = _mm256_extractf128_ps(v[N], 0x1);
} };
template <int N, hkUint32 byteAddressIncrement> struct scatterUintH<1, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const __m256* v, __m128* base, const hkUint16* indices)
{
	__m128* HK_RESTRICT ptr = hkAddByteOffset(base, indices[N*2] * byteAddressIncrement);
	*ptr = _mm256_extractf128_ps(v[N], 0x0);
} };
}
template <int M>
template <hkUint32 byteAddressIncrement>
HK_FORCE_INLINE void hkMxVector<M>::scatter(hkVector4* base, const hkUint16* indices) const
{
	hkMxVector_Implementation::scatterUintH<M, 0, byteAddressIncrement>::apply(m_vec.v, &(base->m_quad), indices);
}



namespace hkMxVector_Implementation
{
template <int I, int N, hkUint32 byteAddressIncrement> struct scatterIntH {
HK_FORCE_INLINE static void apply(const __m256* v, __m128* base, const hkInt32* indices)
{
	__m128* HK_RESTRICT ptr0 = hkAddByteOffset(base, indices[N*2] * byteAddressIncrement);
	__m128* HK_RESTRICT ptr1 = hkAddByteOffset(base, indices[N*2+1] * byteAddressIncrement);
	scatterIntH<I-2, N+1, byteAddressIncrement>::apply(v, base, indices);
	*ptr0 = _mm256_extractf128_ps(v[N], 0x0);
	*ptr1 = _mm256_extractf128_ps(v[N], 0x1);
} };
template <int N, hkUint32 byteAddressIncrement> struct scatterIntH<2, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const __m256* v, __m128* base, const hkInt32* indices)
{
	__m128* HK_RESTRICT ptr0 = hkAddByteOffset(base, indices[N*2] * byteAddressIncrement);
	__m128* HK_RESTRICT ptr1 = hkAddByteOffset(base, indices[N*2+1] * byteAddressIncrement);
	*ptr0 = _mm256_extractf128_ps(v[N], 0x0);
	*ptr1 = _mm256_extractf128_ps(v[N], 0x1);
} };
template <int N, hkUint32 byteAddressIncrement> struct scatterIntH<1, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const __m256* v, __m128* base, const hkInt32* indices)
{
	__m128* HK_RESTRICT ptr = hkAddByteOffset(base, indices[N*2] * byteAddressIncrement);
	*ptr = _mm256_extractf128_ps(v[N], 0x0);
} };
}
template <int M>
template <hkUint32 byteAddressIncrement>
HK_FORCE_INLINE void hkMxVector<M>::scatter(hkVector4* base, const hkInt32* indices) const
{
	hkMxVector_Implementation::scatterIntH<M, 0, byteAddressIncrement>::apply(m_vec.v, &(base->m_quad), indices);
}



namespace hkMxVector_Implementation
{
template <int I, int N, hkUint32 byteAddressIncrement> struct scatterH {
HK_FORCE_INLINE static void apply(const __m256* v, __m128* base)
{
	__m128* HK_RESTRICT ptr0 = hkAddByteOffset(base, (N*2) * byteAddressIncrement);
	__m128* HK_RESTRICT ptr1 = hkAddByteOffset(base, (N*2+1) * byteAddressIncrement);
	scatterH<I-2, N+1, byteAddressIncrement>::apply(v, base);
	*ptr0 = _mm256_extractf128_ps(v[N], 0x0);
	*ptr1 = _mm256_extractf128_ps(v[N], 0x1);
} };
template <int N, hkUint32 byteAddressIncrement> struct scatterH<2, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const __m256* v, __m128* base)
{
	__m128* HK_RESTRICT ptr0 = hkAddByteOffset(base, (N*2) * byteAddressIncrement);
	__m128* HK_RESTRICT ptr1 = hkAddByteOffset(base, (N*2+1) * byteAddressIncrement);
	*ptr0 = _mm256_extractf128_ps(v[N], 0x0);
	*ptr1 = _mm256_extractf128_ps(v[N], 0x1);
} };
template <int N, hkUint32 byteAddressIncrement> struct scatterH<1, N, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const __m256* v, __m128* base)
{
	__m128* HK_RESTRICT ptr = hkAddByteOffset(base, (N*2) * byteAddressIncrement);
	*ptr = _mm256_extractf128_ps(v[N], 0x0);
} };
}
template <int M>
template <hkUint32 byteAddressIncrement>
HK_FORCE_INLINE void hkMxVector<M>::scatter(hkVector4* base) const
{
	hkMxVector_Implementation::scatterH<M, 0, byteAddressIncrement>::apply(m_vec.v, &(base->m_quad));
}



namespace hkMxVector_Implementation
{
template <int I, int N, hkUint32 byteOffset> struct scatterWithOffsetH {
HK_FORCE_INLINE static void apply(const __m256* v, void** base)
{
	__m128* HK_RESTRICT ptr0 = (__m128*)hkAddByteOffsetConst(base[N*2], byteOffset); 
	__m128* HK_RESTRICT ptr1 = (__m128*)hkAddByteOffsetConst(base[N*2+1], byteOffset); 
	scatterWithOffsetH<I-2, N+1, byteOffset>::apply(v, base);
	*ptr0 = _mm256_extractf128_ps(v[N], 0x0);
	*ptr1 = _mm256_extractf128_ps(v[N], 0x1);
} };
template <int N, hkUint32 byteOffset> struct scatterWithOffsetH<2, N, byteOffset> {
HK_FORCE_INLINE static void apply(const __m256* v, void** base)
{
	__m128* HK_RESTRICT ptr0 = (__m128*)hkAddByteOffsetConst(base[N*2], byteOffset); 
	__m128* HK_RESTRICT ptr1 = (__m128*)hkAddByteOffsetConst(base[N*2+1], byteOffset); 
	*ptr0 = _mm256_extractf128_ps(v[N], 0x0);
	*ptr1 = _mm256_extractf128_ps(v[N], 0x1);
} };
template <int N, hkUint32 byteOffset> struct scatterWithOffsetH<1, N, byteOffset> {
HK_FORCE_INLINE static void apply(const __m256* v, void** base)
{
	__m128* HK_RESTRICT ptr = (__m128*)hkAddByteOffsetConst(base[N*2], byteOffset); 
	*ptr = _mm256_extractf128_ps(v[N], 0x0);
} };
}
template <int M>
template <hkUint32 byteOffset> 
HK_FORCE_INLINE void hkMxVector<M>::scatterWithOffset(void* base[M]) const 
{
	hkMxVector_Implementation::scatterWithOffsetH<M, 0, byteOffset>::apply(m_vec.v, base);
}


namespace hkMxVector_Implementation
{
template <int I, int N> struct lengthH { 
HK_FORCE_INLINE static void apply(const __m256* v, __m256* lensOut) 
{ 
	lengthH<I-1, N>::apply(v, lensOut); 
	lensOut[I-1] = _mm256_sqrt_ps(hkMxCommon_Implementation::dotProdH<N>(v[I-1], v[I-1]));
} };
template <int N> struct lengthH<1, N> { 
HK_FORCE_INLINE static void apply(const __m256* v, __m256* lensOut) 
{ 
	lensOut[0] = _mm256_sqrt_ps(hkMxCommon_Implementation::dotProdH<N>(v[0], v[0]));
} };
}
template <int M> template <int N> 
HK_FORCE_INLINE void hkMxVector<M>::length(hkMxReal<M>& lensOut) const 
{ 
	hkMxVector_Implementation::lengthH<((M+1)>>1), N>::apply(m_vec.v, lensOut.m_real.r); 
}

namespace hkMxVector_Implementation
{
template <int I, int N> struct lengthSqrH { 
HK_FORCE_INLINE static void apply(const __m256* v, __m256* lensOut) 
{ 
	lengthSqrH<I-1, N>::apply(v, lensOut); 
	lensOut[I-1] = hkMxCommon_Implementation::dotProdH<N>(v[I-1], v[I-1]); 
} };
template <int N> struct lengthSqrH<1, N> { 
HK_FORCE_INLINE static void apply(const __m256* v, __m256* lensOut) 
{ 
	lensOut[0] = hkMxCommon_Implementation::dotProdH<N>(v[0], v[0]); 
} };
}
template <int M> template <int N> 
HK_FORCE_INLINE void hkMxVector<M>::lengthSquared(hkMxReal<M>& lensOut) const 
{ 
	hkMxVector_Implementation::lengthSqrH<((M+1)>>1), N>::apply(m_vec.v, lensOut.m_real.r); 
}

namespace hkMxVector_Implementation
{
template <int I, int N> struct lengthInvH { 
HK_FORCE_INLINE static void apply(const __m256* v, __m256* lensOut, const __m256& one) 
{ 
	lengthInvH<I-1, N>::apply(v, lensOut, one); 
	const __m256 len2 = hkMxCommon_Implementation::dotProdH<N>(v[I-1], v[I-1]);
	// workaround VS2010 assembler bug
	const __m256 masklow =  _mm256_cmp_ps(len2, _mm256_setzero_ps(), _CMP_EQ_OQ);
	const __m256 maskhigh = _mm256_cmp_ps(_mm256_permute2f128_ps(len2,len2, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_ps(), _CMP_EQ_OQ);
	const __m256 equalsZero = _mm256_permute2f128_ps(masklow,maskhigh, 0x20);
	const __m256 e = _mm256_div_ps(one, _mm256_sqrt_ps(len2));
	lensOut[I-1] = _mm256_andnot_ps(equalsZero, e);
} };
template <int N> struct lengthInvH<1, N> { 
HK_FORCE_INLINE static void apply(const __m256* v, __m256* lensOut, const __m256& one) 
{ 
	const __m256 len2 = hkMxCommon_Implementation::dotProdH<N>(v[0], v[0]);
	// workaround VS2010 assembler bug
	const __m256 masklow =  _mm256_cmp_ps(len2, _mm256_setzero_ps(), _CMP_EQ_OQ);
	const __m256 maskhigh = _mm256_cmp_ps(_mm256_permute2f128_ps(len2,len2, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_ps(), _CMP_EQ_OQ);
	const __m256 equalsZero = _mm256_permute2f128_ps(masklow,maskhigh, 0x20);
	const __m256 e = _mm256_div_ps(one, _mm256_sqrt_ps(len2));
	lensOut[0] = _mm256_andnot_ps(equalsZero, e);
} };
}
template <int M> template <int N> 
HK_FORCE_INLINE void hkMxVector<M>::lengthInverse(hkMxReal<M>& lensOut) const 
{ 
	__m256 one = _mm256_set1_ps(1.0f);
	hkMxVector_Implementation::lengthInvH<((M+1)>>1), N>::apply(m_vec.v, lensOut.m_real.r, one); 
}

namespace hkMxVector_Implementation
{
template <int I, int N> struct lengthInv12H { 
HK_FORCE_INLINE static void apply(const __m256* v, __m256* lensOut) 
{ 
	lengthInv12H<I-1, N>::apply(v, lensOut); 
	const __m256 len2 = hkMxCommon_Implementation::dotProdH<N>(v[I-1], v[I-1]);
	// workaround VS2010 assembler bug
	const __m256 masklow =  _mm256_cmp_ps(len2, _mm256_setzero_ps(), _CMP_EQ_OQ);
	const __m256 maskhigh = _mm256_cmp_ps(_mm256_permute2f128_ps(len2,len2, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_ps(), _CMP_EQ_OQ);
	const __m256 equalsZero = _mm256_permute2f128_ps(masklow,maskhigh, 0x20);
	const __m256 e = _mm256_rsqrt_ps(len2);
	lensOut[I-1] = _mm256_andnot_ps(equalsZero, e);
} };
template <int N> struct lengthInv12H<1, N> { 
HK_FORCE_INLINE static void apply(const __m256* v, __m256* lensOut) 
{ 
	const __m256 len2 = hkMxCommon_Implementation::dotProdH<N>(v[0], v[0]);
	// workaround VS2010 assembler bug
	const __m256 masklow =  _mm256_cmp_ps(len2, _mm256_setzero_ps(), _CMP_EQ_OQ);
	const __m256 maskhigh = _mm256_cmp_ps(_mm256_permute2f128_ps(len2,len2, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_ps(), _CMP_EQ_OQ);
	const __m256 equalsZero = _mm256_permute2f128_ps(masklow,maskhigh, 0x20);
	const __m256 e = _mm256_rsqrt_ps(len2);
	lensOut[0] = _mm256_andnot_ps(equalsZero, e);
} };
}
template <int M> template <int N> 
HK_FORCE_INLINE void hkMxVector<M>::lengthInverse_12BitAccurate(hkMxReal<M>& lensOut) const 
{ 
	hkMxVector_Implementation::lengthInv12H<((M+1)>>1), N>::apply(m_vec.v, lensOut.m_real.r); 
}

namespace hkMxVector_Implementation
{
template <int I, int N> struct lengthInv23H { 
HK_FORCE_INLINE static void apply(const __m256* v, __m256* lensOut) 
{ 
	lengthInv23H<I-1,N>::apply(v, lensOut); 
	const __m256 len2 = hkMxCommon_Implementation::dotProdH<N>(v[I-1], v[I-1]);
	// workaround VS2010 assembler bug
	const __m256 masklow =  _mm256_cmp_ps(len2, _mm256_setzero_ps(), _CMP_EQ_OQ);
	const __m256 maskhigh = _mm256_cmp_ps(_mm256_permute2f128_ps(len2,len2, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_ps(), _CMP_EQ_OQ);
	const __m256 equalsZero = _mm256_permute2f128_ps(masklow,maskhigh, 0x20);
	const __m256 e = hkMxCommon_Implementation::reciprocalSquareRootH(len2);
	lensOut[I-1] = _mm256_andnot_ps(equalsZero, e);
} };
template <int N> struct lengthInv23H<1, N> { 
HK_FORCE_INLINE static void apply(const __m256* v, __m256* lensOut) 
{ 
	const __m256 len2 = hkMxCommon_Implementation::dotProdH<N>(v[0], v[0]);
	// workaround VS2010 assembler bug
	const __m256 masklow =  _mm256_cmp_ps(len2, _mm256_setzero_ps(), _CMP_EQ_OQ);
	const __m256 maskhigh = _mm256_cmp_ps(_mm256_permute2f128_ps(len2,len2, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH)), _mm256_setzero_ps(), _CMP_EQ_OQ);
	const __m256 equalsZero = _mm256_permute2f128_ps(masklow,maskhigh, 0x20);
	const __m256 e = hkMxCommon_Implementation::reciprocalSquareRootH(len2);
	lensOut[0] = _mm256_andnot_ps(equalsZero, e);
} };
}
template <int M> template <int N> 
HK_FORCE_INLINE void hkMxVector<M>::lengthInverse_23BitAccurate(hkMxReal<M>& lensOut) const 
{ 
	hkMxVector_Implementation::lengthInv23H<((M+1)>>1), N>::apply(m_vec.v, lensOut.m_real.r); 
}



namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void selectH(__m256* v, const __m256* mask, const __m256* tVal, const __m256* fVal) { selectH<I-1>(v, mask, tVal, fVal); v[I-1] = _mm256_blendv_ps(fVal[I-1], tVal[I-1], mask[I-1]); }
template <> HK_FORCE_INLINE void selectH<1>(__m256* v, const __m256* mask, const __m256* tVal, const __m256* fVal) { v[0] = _mm256_blendv_ps(fVal[0], tVal[0], mask[0]); }
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::setSelect(hkMxMaskParameter mask, hkMxVectorParameter trueValue, hkMxVectorParameter falseValue )
{
	hkMxVector_Implementation::selectH<((M+1)>>1)>(m_vec.v, mask.m_comp.c, trueValue.m_vec.v, falseValue.m_vec.v);
}
namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void selectSH(__m256* v, const __m256* mask, const __m256& tVal, const __m256* fVal) { selectSH<I-1>(v, mask, tVal, fVal); v[I-1] = _mm256_blendv_ps(fVal[I-1], tVal, mask[I-1]); }
template <> HK_FORCE_INLINE void selectSH<1>(__m256* v, const __m256* mask, const __m256& tVal, const __m256* fVal) { v[0] = _mm256_blendv_ps(fVal[0], tVal, mask[0]); }
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::setSelect(hkMxMaskParameter mask, hkMxSingleParameter trueValue, hkMxVectorParameter falseValue )
{
	hkMxVector_Implementation::selectSH<((M+1)>>1)>(m_vec.v, mask.m_comp.c, trueValue.m_single.s, falseValue.m_vec.v);
}
namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void selectHS(__m256* v, const __m256* mask, const __m256* tVal, const __m256& fVal) { selectHS<I-1>(v, mask, tVal, fVal); v[I-1] = _mm256_blendv_ps(fVal, tVal[I-1], mask[I-1]); }
template <> HK_FORCE_INLINE void selectHS<1>(__m256* v, const __m256* mask, const __m256* tVal, const __m256& fVal) { v[0] = _mm256_blendv_ps(fVal, tVal[0], mask[0]); }
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::setSelect(hkMxMaskParameter mask, hkMxVectorParameter trueValue, hkMxSingleParameter falseValue )
{
	hkMxVector_Implementation::selectHS<((M+1)>>1)>(m_vec.v, mask.m_comp.c, trueValue.m_vec.v, falseValue.m_single.s);
}



namespace hkMxVector_Implementation
{
template <int I, int N> struct leftCH { HK_FORCE_INLINE static void apply(__m256* v, const __m256* v0)
{
	leftCH<I-1,N>::apply(v,v0);
	v[I-1] = _mm256_permute2f128_ps(v0[I-1], v0[I%N], _MM256_PERMUTE2(_MM256_B_LOW,_MM256_A_HIGH));
} };
template <int N> struct leftCH<1,N> { HK_FORCE_INLINE static void apply(__m256* v, const __m256* v0)
{
	v[0] = _mm256_permute2f128_ps(v0[0], v0[1%N], _MM256_PERMUTE2(_MM256_B_LOW,_MM256_A_HIGH));
} };

template <int I, int N> struct leftCUH { HK_FORCE_INLINE static void apply(__m256* v, const __m256* v0)
{
	v[N] = _mm256_permute2f128_ps(v0[N], v0[N+1], _MM256_PERMUTE2(_MM256_B_LOW,_MM256_A_HIGH));
	leftCUH<I-1,N+1>::apply(v,v0);
} };
template <int N> struct leftCUH<0,N> { HK_FORCE_INLINE static void apply(__m256* v, const __m256* v0)
{
	v[N] = v0[0];
} };

template <int I, int N> struct rightCH { HK_FORCE_INLINE static void apply(__m256* v, const __m256* v0)
{
	rightCH<I-1,N>::apply(v,v0);
	v[I-1] = _mm256_permute2f128_ps(v0[I-1], v0[(I-2)%N], _MM256_PERMUTE2(_MM256_A_LOW,_MM256_B_HIGH));
} };
template <int N> struct rightCH<1,N> { HK_FORCE_INLINE static void apply(__m256* v, const __m256* v0)
{
	v[0] = _mm256_permute2f128_ps(v0[0], v0[(N-1)%N], _MM256_PERMUTE2(_MM256_A_LOW,_MM256_B_HIGH));
} };

template <int I, int N> struct rightCUH { HK_FORCE_INLINE static void apply(__m256* v, const __m256* v0)
{
	v[N] = _mm256_permute2f128_ps(v0[I], v0[N], _MM256_PERMUTE2(_MM256_B_LOW,_MM256_A_LOW));
	rightCUH<I-1,N+1>::apply(v,v0);
} };
template <int N> struct rightCUH<0,N> { HK_FORCE_INLINE static void apply(__m256* v, const __m256* v0)
{
	v[N] = _mm256_permute2f128_ps(v0[0], _mm256_undefined_ps(), _MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_HIGH));
} };
template <> struct rightCUH<0,0> { HK_FORCE_INLINE static void apply(__m256* v, const __m256* v0)
{
	v[0] = v0[0];
} };

template <int I, int N> struct revertH { HK_FORCE_INLINE static void apply(__m256* v, const __m256* v0)
{
	revertH<I-1,N+1>::apply(v,v0);
	v[I-1] = _mm256_permute2f128_ps(v0[N], v0[N], _MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_HIGH));
} };
template <int N> struct revertH<1,N> { HK_FORCE_INLINE static void apply(__m256* v, const __m256* v0)
{
	v[0] = _mm256_permute2f128_ps(v0[N], v0[N], _MM256_PERMUTE2(_MM256_A_LOW,_MM256_A_HIGH));
} };

template <int I, int N> struct revertUH { HK_FORCE_INLINE static void apply(__m256* v, const __m256* v0)
{
	v[N] = _mm256_permute2f128_ps(v0[I], v0[I-1], _MM256_PERMUTE2(_MM256_B_HIGH,_MM256_A_LOW));
	revertUH<I-1,N+1>::apply(v,v0);
} };
template <int N> struct revertUH<0,N> { HK_FORCE_INLINE static void apply(__m256* v, const __m256* v0)
{
	v[N] = v0[0];
} };
}
template <int M>
template <hkMxVectorPermutation::Permutation P>
HK_FORCE_INLINE void hkMxVector<M>::setPermutation(hkMxVectorParameter m)
{
	HK_COMPILE_TIME_ASSERT2((P==hkMxVectorPermutation::SHIFT_LEFT_CYCLIC)||(P==hkMxVectorPermutation::SHIFT_RIGHT_CYCLIC)||(P==hkMxVectorPermutation::REVERSE),MX_UNKNOWN_PERMUTATION);
	const int N = (M+1)>>1;
	HK_ASSERT2(0xf03454f5, ((hkUlong)m_vec.v >= (hkUlong)(m.m_vec.v+N)) || ((hkUlong)m.m_vec.v >= (hkUlong)(m_vec.v+N)), "unsafe permutation");

	if ( ((2*N)-M) == 0 ) // compiletime check for uneven number of vectors
	{

		if (P == hkMxVectorPermutation::SHIFT_LEFT_CYCLIC )
		{
			hkMxVector_Implementation::leftCH<N,N>::apply(m_vec.v, m.m_vec.v);
		}
		else if ( P == hkMxVectorPermutation::SHIFT_RIGHT_CYCLIC )
		{
			hkMxVector_Implementation::rightCH<N,N>::apply(m_vec.v,m.m_vec.v);
		}
		else /*if ( P == hkMxVectorPermutation::REVERSE )*/
		{
			hkMxVector_Implementation::revertH<N,0>::apply(m_vec.v, m.m_vec.v);
		}

	}
	else
	{

		if (P == hkMxVectorPermutation::SHIFT_LEFT_CYCLIC )
		{
			hkMxVector_Implementation::leftCUH<N-1,0>::apply(m_vec.v, m.m_vec.v);
		}
		else if ( P == hkMxVectorPermutation::SHIFT_RIGHT_CYCLIC )
		{
			hkMxVector_Implementation::rightCUH<N-1,0>::apply(m_vec.v,m.m_vec.v);
		}
		else /*if ( P == hkMxVectorPermutation::REVERSE )*/
		{
			hkMxVector_Implementation::revertUH<N-1,0>::apply(m_vec.v, m.m_vec.v);
		}

	}
}


template <int M> HK_FORCE_INLINE void hkMxVector<M>::setAsBroadcast(hkVector4Parameter v)
{
	HK_MXVECTOR_MX_NOT_IMPLEMENTED;
}
template <> HK_FORCE_INLINE void hkMxVector<4>::setAsBroadcast(hkVector4Parameter v) 
{ 
	hkReal x = v(0); hkReal y = v(1); m_vec.v[0] = _mm256_setr_ps(x,x,x,x,y,y,y,y); 
	hkReal z = v(2); hkReal w = v(3); m_vec.v[1] = _mm256_setr_ps(z,z,z,z,w,w,w,w); 
}
template <> HK_FORCE_INLINE void hkMxVector<3>::setAsBroadcast(hkVector4Parameter v) 
{ 
	hkReal x = v(0); hkReal y = v(1); m_vec.v[0] = _mm256_setr_ps(x,x,x,x,y,y,y,y); 
	hkReal z = v(2);                  m_vec.v[1] = _mm256_broadcast_ss(&z); 
}
template <> HK_FORCE_INLINE void hkMxVector<2>::setAsBroadcast(hkVector4Parameter v) 
{ 
	hkReal x = v(0); hkReal y = v(1); m_vec.v[0] = _mm256_setr_ps(x,x,x,x,y,y,y,y); 
}
template <> HK_FORCE_INLINE void hkMxVector<1>::setAsBroadcast(hkVector4Parameter v) 
{ 
	hkReal x = v(0); m_vec.v[0] = _mm256_broadcast_ss(&x); 
}

#undef MXV_NO_OPERANDS
#undef MXV_OP_MXVECTOR
#undef MXV_OP_MXVECTOR_MXVECTOR
#undef MXV_OP_MXVECTOR_MXVECTOR_MXVECTOR
#undef MXV_OP_MXVECTOR_MXVECTOR_MXSINGLE
#undef MXV_OP_MXVECTOR_MXVECTOR_MXREAL
#undef MXV_OP_MXVECTOR_MXVECTOR_SELF
#undef MXV_OP_MXVECTOR_MXSINGLE_SELF
#undef MXV_OP_MXREAL_MXVECTOR_SELF
#undef MXV_OP_MXVECTOR_MXSINGLE_MXREAL
#undef MXV_OP_MXREAL
#undef MXV_OP_MXSINGLE
#undef MXV_OP_MXREAL_MXVECTOR
#undef MXV_OP_MXVECTOR_MXSINGLE
#undef MXV_OP_MXSINGLE_MXSINGLE
#undef MXV_OP_MXSINGLE_MXVECTOR
#undef MXV_COMPARE
#undef MXV_COMPARE_SINGLE

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
