/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


#define MXV_NO_OPERANDS(METHOD, OP) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void METHOD##H(hkVector4* v) { METHOD##H<I-1>(v); v[I-1].OP(); } \
	template <> HK_FORCE_INLINE void METHOD##H<1>(hkVector4* v) { v[0].OP(); } \
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD() { hkMxVector_Implementation::METHOD##H<M>(m_vec.v); }

#define MXV_OP_MXVECTOR(METHOD, OP) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void METHOD##MXV_MXV_MXV(hkVector4* v, const hkVector4* v0) { METHOD##MXV_MXV_MXV<I-1>(v, v0); v[I-1].OP(v0[I-1]); } \
	template <> HK_FORCE_INLINE void METHOD##MXV_MXV_MXV<1>(hkVector4* v, const hkVector4* v0) { v[0].OP(v0[0]); }	\
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD(hkMxVectorParameter m) { hkMxVector_Implementation::METHOD##MXV_MXV_MXV<M>(m_vec.v, m.m_vec.v); }

#define MXV_OP_MXVECTOR_MXVECTOR(METHOD, OP) namespace hkMxVector_Implementation { template <int I> HK_FORCE_INLINE void OP##H(hkVector4* v, const hkVector4* v0, const hkVector4* v1) { OP##H<I-1>(v, v0, v1); v[I-1].OP(v0[I-1], v1[I-1]); } template <> HK_FORCE_INLINE void OP##H<1>(hkVector4* v, const hkVector4* v0, const hkVector4* v1) { v[0].OP(v0[0], v1[0]); } } template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD(hkMxVectorParameter v0, hkMxVectorParameter v1) { hkMxVector_Implementation::OP##H<M>(m_vec.v, v0.m_vec.v, v1.m_vec.v); }

#define MXV_OP_MXVECTOR_MXVECTOR_MXVECTOR(METHOD, OP) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void OP##H(hkVector4* v, const hkVector4* v0, const hkVector4* v1, const hkVector4* v2) { OP##H<I-1>(v, v0, v1, v2); v[I-1].OP(v0[I-1], v1[I-1], v2[I-1]); } \
	template <> HK_FORCE_INLINE void OP##H<1>(hkVector4* v, const hkVector4* v0, const hkVector4* v1, const hkVector4* v2) { v[0].OP(v0[0], v1[0], v2[0]); } \
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD(hkMxVectorParameter v0, hkMxVectorParameter v1, hkMxVectorParameter v2) { hkMxVector_Implementation::OP##H<M>(m_vec.v, v0.m_vec.v, v1.m_vec.v, v2.m_vec.v); }

#define MXV_OP_MXVECTOR_MXVECTOR_MXSINGLE(METHOD, OP) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void OP##S2H(hkVector4* v, const hkVector4* v0, const hkVector4* v1, hkVector4Parameter v2) { OP##S2H<I-1>(v, v0, v1, v2); v[I-1].OP(v0[I-1], v1[I-1], v2); }\
	template <> HK_FORCE_INLINE void OP##S2H<1>(hkVector4* v, const hkVector4* v0, const hkVector4* v1, hkVector4Parameter v2) { v[0].OP(v0[0], v1[0], v2); }\
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD(hkMxVectorParameter v0, hkMxVectorParameter v1, hkMxSingleParameter v2) { hkMxVector_Implementation::OP##S2H<M>(m_vec.v, v0.m_vec.v, v1.m_vec.v, v2.m_single.s); }

#define MXV_OP_MXVECTOR_MXVECTOR_MXREAL(METHOD, OP) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void OP##S2R(hkVector4* v, const hkVector4* v0, const hkVector4* v1, const hkSimdReal* v2) { OP##S2R<I-1>(v, v0, v1, v2); v[I-1].OP(v0[I-1], v1[I-1], v2[I-1]); }\
	template <> HK_FORCE_INLINE void OP##S2R<1>(hkVector4* v, const hkVector4* v0, const hkVector4* v1, const hkSimdReal* v2) { v[0].OP(v0[0], v1[0], v2[0]); }\
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD(hkMxVectorParameter v0, hkMxVectorParameter v1, hkMxRealParameter v2) { hkMxVector_Implementation::OP##S2R<M>(m_vec.v, v0.m_vec.v, v1.m_vec.v, v2.m_real.r); }

#define MXV_OP_MXVECTOR_MXSINGLE_MXREAL(METHOD, OP) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void OP##S2SR(hkVector4* v, const hkVector4* v0, hkVector4Parameter v1, const hkSimdReal* v2) { OP##S2SR<I-1>(v, v0, v1, v2); v[I-1].OP(v0[I-1], v1, v2[I-1]); }\
	template <> HK_FORCE_INLINE void OP##S2SR<1>(hkVector4* v, const hkVector4* v0, hkVector4Parameter v1,   const hkSimdReal* v2) { v[0].OP(v0[0], v1, v2[0]); }\
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD(hkMxVectorParameter v0, hkMxSingleParameter v1, hkMxRealParameter v2) { hkMxVector_Implementation::OP##S2SR<M>(m_vec.v, v0.m_vec.v, v1.m_single.s, v2.m_real.r); }


#define MXV_OP_MXREAL(METHOD, OP) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void OP##SHR(hkVector4* v, const hkSimdReal* v0) { OP##SHR<I-1>(v, v0); v[I-1].OP(v0[I-1]); }\
	template <> HK_FORCE_INLINE void OP##SHR<1>(hkVector4* v, const hkSimdReal* v0) { v[0].OP(v0[0]); } \
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD(hkMxRealParameter s) { hkMxVector_Implementation::OP##SHR<M>(m_vec.v, s.m_real.r); }


#define MXV_OP_MXSINGLE(METHOD, OP) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void OP##SH(hkVector4* v, hkVector4Parameter v0) { OP##SH<I-1>(v, v0); v[I-1].OP(v0); }\
	template <> HK_FORCE_INLINE void OP##SH<1>(hkVector4* v, hkVector4Parameter v0) { v[0].OP(v0); } \
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD(hkMxSingleParameter s) { hkMxVector_Implementation::OP##SH<M>(m_vec.v, s.m_single.s); }


#define MXV_OP_MXVECTOR_MXREAL(METHOD, OP) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void OP##SH2R(hkVector4* v, const hkVector4* v0, const hkSimdReal* v1) { OP##SH2R<I-1>(v, v0, v1); v[I-1].OP(v0[I-1], v1[I-1]); }	\
	template <>   HK_FORCE_INLINE void OP##SH2R<1>(hkVector4* v, const hkVector4* v0, const hkSimdReal* v1) { v[0].OP(v0[0],v1[0]); }							\
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD(hkMxVectorParameter v0, hkMxRealParameter r) { hkMxVector_Implementation::OP##SH2R<M>(m_vec.v, v0.m_vec.v, r.m_real.r); }

#define MXV_OP_MXREAL_MXVECTOR(METHOD, OP) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void OP##SHR2(hkVector4* v, const hkSimdReal* s0, const hkVector4* v1) { OP##SHR2<I-1>(v, s0, v1); v[I-1].OP(s0[I-1], v1[I-1]); }	\
	template <>   HK_FORCE_INLINE void OP##SHR2<1>(hkVector4* v, const hkSimdReal* s0, const hkVector4* v1 ) { v[0].OP(s0[0],v1[0]); }							\
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD(hkMxRealParameter r, hkMxVectorParameter v0 ) { hkMxVector_Implementation::OP##SHR2<M>(m_vec.v, r.m_real.r, v0.m_vec.v ); }


#define MXV_OP_MXVECTOR_MXSINGLE(METHOD, OP) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void OP##SH(hkVector4* v, const hkVector4* v0, hkVector4Parameter v1) { OP##SH<I-1>(v, v0, v1); v[I-1].OP(v0[I-1], v1); } \
	template <> HK_FORCE_INLINE void OP##SH<1>(hkVector4* v, const hkVector4* v0, hkVector4Parameter v1) { v[0].OP(v0[0], v1); }	\
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD(hkMxVectorParameter v, hkMxSingleParameter s) { hkMxVector_Implementation::OP##SH<M>(m_vec.v, v.m_vec.v, s.m_single.s); }

#define MXV_OP_MXSINGLE_MXSINGLE(METHOD, OP) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void OP##MX_SS(hkVector4* v, hkVector4Parameter v0, hkVector4Parameter v1) { OP##MX_SS<I-1>(v, v0, v1); v[I-1].OP(v0, v1); } \
	template <> HK_FORCE_INLINE void OP##MX_SS<1>(  hkVector4* v, hkVector4Parameter v0, hkVector4Parameter v1) { v[0].OP(v0, v1); }	\
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD(hkMxSingleParameter s0, hkMxSingleParameter s1) { hkMxVector_Implementation::OP##MX_SS<M>(m_vec.v, s0.m_single.s, s1.m_single.s); }

#define MXV_OP_MXSINGLE_MXVECTOR(METHOD, OP) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void OP##RSH(hkVector4* v, hkVector4Parameter v1, const hkVector4* v0) { OP##RSH<I-1>(v, v1, v0); v[I-1].OP(v1, v0[I-1]); }\
	template <> HK_FORCE_INLINE void OP##RSH<1>(hkVector4* v, hkVector4Parameter v1, const hkVector4* v0) { v[0].OP(v1, v0[0]); } \
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD(hkMxSingleParameter s, hkMxVectorParameter v) { hkMxVector_Implementation::OP##RSH<M>(m_vec.v, s.m_single.s, v.m_vec.v); }

#define MXV_COMPARE(METHOD, OP) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void OP##CH(const hkVector4* v0, const hkVector4* v1, hkVector4Comparison* mask) { OP##CH<I-1>(v0,v1,mask); mask[I-1] = v0[I-1].OP(v1[I-1]); } \
	template <> HK_FORCE_INLINE void OP##CH<1>(const hkVector4* v0, const hkVector4* v1, hkVector4Comparison* mask) { mask[0] = v0[0].OP(v1[0]); } \
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD(hkMxVectorParameter v1, hkMxMask<M>& mask) const { hkMxVector_Implementation::OP##CH<M>(m_vec.v, v1.m_vec.v, mask.m_comp.c); }

#define MXV_COMPARE_SINGLE(METHOD, OP) \
	namespace hkMxVector_Implementation { \
	template <int I> HK_FORCE_INLINE void OP##CHS(const hkVector4* v0, hkVector4Parameter v1, hkVector4Comparison* mask) { OP##CHS<I-1>(v0,v1,mask); mask[I-1] = v0[I-1].OP(v1); }\
	template <> HK_FORCE_INLINE void OP##CHS<1>(const hkVector4* v0, hkVector4Parameter v1, hkVector4Comparison* mask) { mask[0] = v0[0].OP(v1); } \
	} \
	template <int M> HK_FORCE_INLINE void hkMxVector<M>::METHOD(hkMxSingleParameter v1, hkMxMask<M>& mask) const { hkMxVector_Implementation::OP##CHS<M>(m_vec.v, v1.m_single.s, mask.m_comp.c); }


#ifndef HK_DISABLE_MATH_CONSTRUCTORS

namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void constructH(hkVector4* v, hkVector4Parameter v0) { constructH<I-1>(v, v0); v[I-1] = v0; }
template <> HK_FORCE_INLINE void constructH<1>(hkVector4* v, hkVector4Parameter v0) { v[0] = v0; }
}
template <int M>
HK_FORCE_INLINE hkMxVector<M>::hkMxVector(hkVector4Parameter v)
{
	hkMxVector_Implementation::constructH<M>(m_vec.v, v);
}


namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void constructRealH(hkVector4* v, const hkSimdReal* v0) { constructRealH<I-1>(v, v0); v[I-1].setAll(v0[I-1]); }
template <> HK_FORCE_INLINE void constructRealH<1>(hkVector4* v, const hkSimdReal* v0) { v[0].setAll(v0[0]); }
}
template <int M>
HK_FORCE_INLINE hkMxVector<M>::hkMxVector(hkMxRealParameter v)
{
	hkMxVector_Implementation::constructRealH<M>(m_vec.v, v.m_real.r);
}


namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void constructSRealH(hkVector4* v, hkSimdRealParameter r) { constructSRealH<I-1>(v, r); v[I-1].setAll(r); }
template <> HK_FORCE_INLINE void constructSRealH<1>(hkVector4* v, hkSimdRealParameter r) { v[0].setAll(r); }
}
template <int M>
HK_FORCE_INLINE hkMxVector<M>::hkMxVector(hkSimdRealParameter r)
{
	hkMxVector_Implementation::constructSRealH<M>(m_vec.v, r);
}

#endif

namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void setBCH(hkVector4* v, const hkSimdReal* v0) { setBCH<I-1>(v, v0); v[I-1].setAll(v0[I-1]); }
template <> HK_FORCE_INLINE void setBCH<1>(hkVector4* v, const hkSimdReal* v0) { v[0].setAll(v0[0]); }
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::setBroadcast( hkMxRealParameter r )
{
	hkMxVector_Implementation::setBCH<M>(m_vec.v, r.m_real.r);
}

namespace hkMxVector_Implementation
{
template <int I, int idx> struct setScalarBroadcastH { HK_FORCE_INLINE static void apply(hkVector4* v, const hkVector4* v0) { setScalarBroadcastH<I-1,idx>::apply(v, v0); v[I-1].setBroadcast<idx>(v0[I-1]); } };
template <int idx> struct setScalarBroadcastH<1,idx> { HK_FORCE_INLINE static void apply(hkVector4* v, const hkVector4* v0) { v[0].setBroadcast<idx>(v0[0]); } };
}
template <int M>
template <int I>
HK_FORCE_INLINE void hkMxVector<M>::setScalarBroadcast(hkMxVectorParameter v)
{
	HK_VECTOR4_SUBINDEX_CHECK;
	hkMxVector_Implementation::setScalarBroadcastH<M,I>::apply(m_vec.v, v.m_vec.v);
}


template <int M>
template <int I>
HK_FORCE_INLINE void hkMxVector<M>::setVector(hkVector4Parameter v)
{
	HK_MXVECTOR_MX_SUBINDEX_CHECK;
	m_vec.v[I] = v;
}

template <int M>
template <int I>
HK_FORCE_INLINE const hkVector4& hkMxVector<M>::getVector() const
{
	HK_MXVECTOR_MX_SUBINDEX_CHECK;
	return m_vec.v[I];
}

template <int M>
template <int I>
HK_FORCE_INLINE void hkMxVector<M>::getVector(hkVector4& vOut) const
{
	HK_MXVECTOR_MX_SUBINDEX_CHECK;
	vOut = m_vec.v[I];
}

namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void setConstH(hkVector4* v, hkVector4Parameter vC) { setConstH<I-1>(v,vC); v[I-1] = vC; }
template <> HK_FORCE_INLINE void setConstH<1>(hkVector4* v, hkVector4Parameter vC) { v[0] = vC; }
}
template <int M>
template<int vectorConstant> 
HK_FORCE_INLINE void hkMxVector<M>::setConstant()
{
	const hkVector4 vC = hkVector4::getConstant<vectorConstant>();
	hkMxVector_Implementation::setConstH<M>(m_vec.v, vC);
}


namespace hkMxVector_Implementation
{
template <int I, int N> struct isOkH { HK_FORCE_INLINE static hkBool32 apply(const hkVector4* v) { if (v[I-1].isOk<N>()) return isOkH<I-1,N>::apply(v); else return 0; } };
template <int N> struct isOkH<1,N> { HK_FORCE_INLINE static hkBool32 apply(const hkVector4* v) { return v[0].isOk<N>(); } };
}
template <int M>
template <int N>
HK_FORCE_INLINE hkBool32 hkMxVector<M>::isOk() const
{
#if !defined(HK_DEBUG)
	// pragma compiler perf warning
#endif
	return hkMxVector_Implementation::isOkH<M,N>::apply(m_vec.v);
}


namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void moveH(hkVector4* HK_RESTRICT v, const hkVector4* HK_RESTRICT v0) { moveH<I-1>(v, v0); v[I-1] = v0[I-1]; }
template <> HK_FORCE_INLINE void moveH<1>(hkVector4* HK_RESTRICT v, const hkVector4* HK_RESTRICT v0) { v[0] = v0[0]; }
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::moveLoad(const hkVector4* HK_RESTRICT v)
{
	HK_ASSERT2(0x20baff23, ((hkUlong)m_vec.v >= (hkUlong)(v+M)) || ((hkUlong)v >= (hkUlong)(m_vec.v+M)), "unsafe move operation");
	hkMxVector_Implementation::moveH<M>(m_vec.v, v);
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::moveStore(hkVector4* HK_RESTRICT v) const
{
	HK_ASSERT2(0x20baff22, ((hkUlong)m_vec.v >= (hkUlong)(v+M)) || ((hkUlong)v >= (hkUlong)(m_vec.v+M)), "unsafe move operation");
	hkMxVector_Implementation::moveH<M>(v, m_vec.v);
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::operator= ( hkMxVectorParameter v )
{
	HK_ASSERT2(0x20baff21, ((hkUlong)m_vec.v >= (hkUlong)(v.m_vec.v+M)) || ((hkUlong)v.m_vec.v >= (hkUlong)(m_vec.v+M)), "unsafe move operation");
	hkMxVector_Implementation::moveH<M>(m_vec.v, v.m_vec.v);
}


namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void loadRH(hkVector4* v, const hkReal* r) { loadRH<I-1>(v, r); v[I-1].load<4>(r+((I-1)*4)); }
template <> HK_FORCE_INLINE void loadRH<1>(hkVector4* v, const hkReal* r) { v[0].load<4>(r); }
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::load(const hkReal* r)
{
	hkMxVector_Implementation::loadRH<M>(m_vec.v, r);
}

namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void loadNRH(hkVector4* v, const hkReal* r) { loadNRH<I-1>(v, r); v[I-1].load<4,HK_IO_NATIVE_ALIGNED>(r+((I-1)*4)); }
template <> HK_FORCE_INLINE void loadNRH<1>(hkVector4* v, const hkReal* r) { v[0].load<4,HK_IO_NATIVE_ALIGNED>(r); }
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::loadNotAligned(const hkReal* r)
{
	hkMxVector_Implementation::loadNRH<M>(m_vec.v, r);
}

namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void loadCNRH(hkVector4* v, const hkReal* r) { loadCNRH<I-1>(v, r); v[I-1].load<4,HK_IO_NOT_CACHED>(r+((I-1)*4)); }
template <> HK_FORCE_INLINE void loadCNRH<1>(hkVector4* v, const hkReal* r) { v[0].load<4,HK_IO_NOT_CACHED>(r); }
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::loadNotCached(const hkReal* r)
{
	hkMxVector_Implementation::loadCNRH<M>(m_vec.v, r);
}

namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void storeRH(const hkVector4* v, hkReal* r) { storeRH<I-1>(v, r); v[I-1].store<4>(r+((I-1)*4)); }
template <> HK_FORCE_INLINE void storeRH<1>(const hkVector4* v, hkReal* r) { v[0].store<4>(r); }
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::store(hkReal* r) const
{
	hkMxVector_Implementation::storeRH<M>(m_vec.v, r);
}

namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void storeNRH(const hkVector4* v, hkReal* r) { storeNRH<I-1>(v, r); v[I-1].store<4,HK_IO_NATIVE_ALIGNED>(r+((I-1)*4)); }
template <> HK_FORCE_INLINE void storeNRH<1>(const hkVector4* v, hkReal* r) { v[0].store<4,HK_IO_NATIVE_ALIGNED>(r); }
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::storeNotAligned(hkReal* r) const
{
	hkMxVector_Implementation::storeNRH<M>(m_vec.v, r);
}

namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void storeCNRH(const hkVector4* v, hkReal* r) { storeCNRH<I-1>(v, r); v[I-1].store<4,HK_IO_NOT_CACHED>(r+((I-1)*4)); }
template <> HK_FORCE_INLINE void storeCNRH<1>(const hkVector4* v, hkReal* r) { v[0].store<4,HK_IO_NOT_CACHED>(r); }
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::storeNotCached(hkReal* r) const
{
	hkMxVector_Implementation::storeCNRH<M>(m_vec.v, r);
}



namespace hkMxVector_Implementation
{
template <int I, int N, hkMathAccuracyMode A, hkMathNegSqrtMode S> struct normalizeH { HK_FORCE_INLINE static void apply(hkVector4* v) { normalizeH<I-1,N,A,S>::apply(v); v[I-1].normalize<N,A,S>(); } };
template <int N, hkMathAccuracyMode A, hkMathNegSqrtMode S> struct normalizeH<1,N,A,S> { HK_FORCE_INLINE static void apply(hkVector4* v) { v[0].normalize<N,A,S>(); } };
}
template <int M> template <int N, hkMathAccuracyMode A, hkMathNegSqrtMode S> HK_FORCE_INLINE void hkMxVector<M>::normalize() { hkMxVector_Implementation::normalizeH<M,N,A,S>::apply(m_vec.v); }

template <int M> template <int N> HK_FORCE_INLINE void hkMxVector<M>::normalize() { hkMxVector_Implementation::normalizeH<M,N,HK_ACC_23_BIT,HK_SQRT_SET_ZERO>::apply(m_vec.v); }



namespace hkMxVector_Implementation
{
template <int I, int N> struct setNegH { HK_FORCE_INLINE static void apply(hkVector4* v0, const hkVector4* v1) { setNegH<I-1,N>::apply(v0,v1); v0[I-1].setNeg<N>(v1[I-1]); } };
template <int N> struct setNegH<1,N> { HK_FORCE_INLINE static void apply(hkVector4* v0, const hkVector4* v1) { v0[0].setNeg<N>(v1[0]); } };
}
template <int M> template <int N> HK_FORCE_INLINE void hkMxVector<M>::setNeg(hkMxVectorParameter v0) { hkMxVector_Implementation::setNegH<M,N>::apply(m_vec.v, v0.m_vec.v); }

namespace hkMxVector_Implementation
{
template <int I, hkMathAccuracyMode A, hkMathDivByZeroMode D> struct setDivH { HK_FORCE_INLINE static void apply(hkVector4* v0, const hkVector4* v1, const hkVector4* v2) { setDivH<I-1,A,D>::apply(v0,v1,v2); v0[I-1].setDiv<A,D>(v1[I-1],v2[I-1]); } };
template <hkMathAccuracyMode A, hkMathDivByZeroMode D> struct setDivH<1,A,D> { HK_FORCE_INLINE static void apply(hkVector4* v0, const hkVector4* v1, const hkVector4* v2) { v0[0].setDiv<A,D>(v1[0],v2[0]); } };
}
template <int M> template <hkMathAccuracyMode A, hkMathDivByZeroMode D> HK_FORCE_INLINE void hkMxVector<M>::setDiv(hkMxVectorParameter v0, hkMxVectorParameter v1) { hkMxVector_Implementation::setDivH<M,A,D>::apply(m_vec.v, v0.m_vec.v, v1.m_vec.v); }

template <int M> HK_FORCE_INLINE void hkMxVector<M>::setDiv(hkMxVectorParameter v0, hkMxVectorParameter v1) { hkMxVector_Implementation::setDivH<M,HK_ACC_23_BIT,HK_DIV_IGNORE>::apply(m_vec.v, v0.m_vec.v, v1.m_vec.v); }

namespace hkMxVector_Implementation
{
template <int I, hkMathAccuracyMode A, hkMathNegSqrtMode S> struct setSqrtInverseH { HK_FORCE_INLINE static void apply(hkVector4* v0, const hkVector4* v1) { setSqrtInverseH<I-1,A,S>::apply(v0,v1); v0[I-1].setSqrtInverse<A,S>(v1[I-1]); } };
template <hkMathAccuracyMode A, hkMathNegSqrtMode S> struct setSqrtInverseH<1,A,S> { HK_FORCE_INLINE static void apply(hkVector4* v0, const hkVector4* v1) { v0[0].setSqrtInverse<A,S>(v1[0]); } };
}
template <int M> template <hkMathAccuracyMode A, hkMathNegSqrtMode S> HK_FORCE_INLINE void hkMxVector<M>::setSqrtInverse(hkMxVectorParameter v0) { hkMxVector_Implementation::setSqrtInverseH<M,A,S>::apply(m_vec.v, v0.m_vec.v); }

template <int M> HK_FORCE_INLINE void hkMxVector<M>::setSqrtInverse(hkMxVectorParameter v0) { hkMxVector_Implementation::setSqrtInverseH<M,HK_ACC_23_BIT,HK_SQRT_SET_ZERO>::apply(m_vec.v, v0.m_vec.v); }

MXV_NO_OPERANDS( setZero, setZero )

namespace hkMxVector_Implementation
{
template <int I, int N> struct zeroCompH { HK_FORCE_INLINE static void apply(hkVector4* v) { zeroCompH<I-1,N>::apply(v); v[I-1].zeroComponent<N>(); } };
template <int N> struct zeroCompH<1,N> { HK_FORCE_INLINE static void apply(hkVector4* v) { v[0].zeroComponent<N>(); } };
}
template <int M> 
template <int N> 
HK_FORCE_INLINE void hkMxVector<M>::zeroComponent() 
{ 
	hkMxVector_Implementation::zeroCompH<M,N>::apply(m_vec.v); 
}

MXV_OP_MXVECTOR( setAbs, setAbs )
MXV_OP_MXVECTOR( add, add )
MXV_OP_MXVECTOR( sub, sub )
MXV_OP_MXVECTOR( mul, mul )
MXV_OP_MXREAL( mul, mul )

MXV_OP_MXVECTOR( setXYZ, setXYZ )
MXV_OP_MXVECTOR( setW, setW )

MXV_OP_MXVECTOR_MXVECTOR( setAdd, setAdd )
MXV_OP_MXVECTOR_MXVECTOR( setSub, setSub )
MXV_OP_MXVECTOR_MXVECTOR( setMul, setMul )
MXV_OP_MXVECTOR_MXVECTOR( setCross, setCross )
MXV_OP_MXVECTOR_MXVECTOR( addMul, addMul )
MXV_OP_MXVECTOR_MXVECTOR( setMin, setMin )
MXV_OP_MXVECTOR_MXVECTOR( setMax, setMax )
MXV_OP_MXREAL_MXVECTOR( addMul, addMul )
MXV_OP_MXREAL_MXVECTOR( setMul, setMul )


MXV_OP_MXVECTOR_MXVECTOR_MXVECTOR( setAddMul, setAddMul )
MXV_OP_MXVECTOR_MXVECTOR_MXVECTOR( setSubMul, setSubMul )
MXV_OP_MXVECTOR_MXVECTOR_MXSINGLE( setAddMul, setAddMul )
MXV_OP_MXVECTOR_MXVECTOR_MXSINGLE( setSubMul, setSubMul )
MXV_OP_MXVECTOR_MXVECTOR_MXREAL( setSubMul, setSubMul )
MXV_OP_MXVECTOR_MXSINGLE_MXREAL( setSubMul, setSubMul )

MXV_OP_MXVECTOR_MXVECTOR_MXREAL( setInterpolate, setInterpolate )

MXV_OP_MXSINGLE( add, add )
MXV_OP_MXSINGLE( sub, sub )
MXV_OP_MXSINGLE( mul, mul )
MXV_OP_MXSINGLE( setW, setW )
MXV_OP_MXREAL( setW, setW )
MXV_OP_MXVECTOR_MXREAL( setXYZ_W, setXYZ_W )


MXV_OP_MXVECTOR_MXSINGLE( addMul, addMul )
MXV_OP_MXSINGLE_MXSINGLE( addMul, addMul )
MXV_OP_MXVECTOR_MXSINGLE( setAdd, setAdd )
MXV_OP_MXVECTOR_MXSINGLE( setSub, setSub )
MXV_OP_MXSINGLE_MXVECTOR( setSub, setSub )
MXV_OP_MXVECTOR_MXSINGLE( setMul, setMul )
MXV_OP_MXVECTOR_MXSINGLE( setCross, setCross )
MXV_OP_MXSINGLE_MXVECTOR( setCross, setCross )
MXV_OP_MXVECTOR_MXSINGLE( setMin, setMin )
MXV_OP_MXVECTOR_MXSINGLE( setMax, setMax )

MXV_COMPARE( equal, equal )
MXV_COMPARE( notEqual, notEqual )
MXV_COMPARE( less, less )
MXV_COMPARE( greater, greater )
MXV_COMPARE( lessEqual, lessEqual )
MXV_COMPARE( greaterEqual, greaterEqual )

MXV_COMPARE_SINGLE( equal, equal )
MXV_COMPARE_SINGLE( notEqual, notEqual )
MXV_COMPARE_SINGLE( less, less )
MXV_COMPARE_SINGLE( greater, greater )
MXV_COMPARE_SINGLE( lessEqual, lessEqual )
MXV_COMPARE_SINGLE( greaterEqual, greaterEqual )





namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void setAddMulH(hkVector4* v, const hkVector4* v0, const hkVector4* v1, hkVector4Parameter v2) { setAddMulH<I-1>(v, v0, v1, v2); v[I-1].setAddMul(v0[I-1], v1[I-1], v2.getComponent<I-1>()); }
template <> HK_FORCE_INLINE void setAddMulH<1>(hkVector4* v, const hkVector4* v0, const hkVector4* v1, hkVector4Parameter v2) { v[0].setAddMul(v0[0], v1[0], v2.getComponent<0>()); }
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::setAddMul(hkMxVectorParameter v0, hkMxVectorParameter v1, hkVector4Parameter v2)
{
	hkMxVector_Implementation::setAddMulH<M>(m_vec.v, v0.m_vec.v, v1.m_vec.v, v2); // this is safe because v2.getComponent<> asserts M<=4
}


template <int M>
HK_FORCE_INLINE void hkMxVector<M>::setXYZ_W(hkMxVectorParameter v0, hkMxVectorParameter v1)
{
	setXYZ(v0);
	setW(v1);
}


template <int M>
HK_FORCE_INLINE void hkMxVector<M>::storeTransposed4(hkMatrix4& matrix4) const
{
	HK_MXVECTOR_MX_NOT_IMPLEMENTED;
}

template <>
HK_FORCE_INLINE void hkMxVector<1>::storeTransposed4(hkMatrix4& matrix4) const
{
	hkVector4 a; a.setBroadcast<0>(m_vec.v[0]);
	hkVector4 b; b.setBroadcast<1>(m_vec.v[0]);
	hkVector4 c; c.setBroadcast<2>(m_vec.v[0]);
	hkVector4 d; d.setBroadcast<3>(m_vec.v[0]);

	matrix4.setCols(a,b,c,d);
	// todo. for debug: set .yzw to 0x23456789
}

template <>
HK_FORCE_INLINE void hkMxVector<2>::storeTransposed4(hkMatrix4& matrix4) const
{
	matrix4.setRows(m_vec.v[0], m_vec.v[1], m_vec.v[1], m_vec.v[1]);
}

template <>
HK_FORCE_INLINE void hkMxVector<3>::storeTransposed4(hkMatrix4& matrix4) const
{
	matrix4.setRows(m_vec.v[0], m_vec.v[1], m_vec.v[2], m_vec.v[2]);
}

template <>
HK_FORCE_INLINE void hkMxVector<4>::storeTransposed4(hkMatrix4& matrix4) const
{
	matrix4.setRows(m_vec.v[0], m_vec.v[1], m_vec.v[2], m_vec.v[3]);
}


namespace hkMxVector_Implementation
{
template <int M, int N> struct hAddH { HK_FORCE_INLINE static void apply(hkMxVectorParameter v, hkVector4& addsOut) { HK_MXVECTOR_MX_NOT_IMPLEMENTED; } };
template <> struct hAddH<4,4> { HK_FORCE_INLINE static void apply(const hkMxVector<4>& v, hkVector4& addsOut) 
{ 
	hkMatrix4 f; v.storeTransposed4(f);
	hkVector4 t0; t0.setAdd( f.getColumn<0>(), f.getColumn<1>() );
	hkVector4 t1; t1.setAdd( f.getColumn<2>(), f.getColumn<3>() );
	addsOut.setAdd( t0, t1 );
} };
template <> struct hAddH<4,3> { HK_FORCE_INLINE static void apply(const hkMxVector<4>& v, hkVector4& addsOut) 
{ 
	hkMatrix4 f; v.storeTransposed4(f);
	hkVector4 t0; t0.setAdd( f.getColumn<0>(), f.getColumn<1>() );
	addsOut.setAdd( t0, f.getColumn<2>() );
} };
template <> struct hAddH<3,4> { HK_FORCE_INLINE static void apply(const hkMxVector<3>& v, hkVector4& addsOut) 
{ 
	// optimize!
	hkMatrix4 f; v.storeTransposed4(f);
	hkVector4 t0; t0.setAdd( f.getColumn<0>(), f.getColumn<1>() );
	hkVector4 t1; t1.setAdd( f.getColumn<2>(), f.getColumn<3>() );
	addsOut.setAdd( t0, t1 );
} };
template <> struct hAddH<3,3> { HK_FORCE_INLINE static void apply(const hkMxVector<3>& v, hkVector4& addsOut) 
{ 
	hkMatrix4 f; v.storeTransposed4(f);
	hkVector4 t0; t0.setAdd( f.getColumn<0>(), f.getColumn<1>() );
	addsOut.setAdd( t0, f.getColumn<2>() );
} };
template <> struct hAddH<2,4> { HK_FORCE_INLINE static void apply(const hkMxVector<2>& v, hkVector4& addsOut) 
{ 
	hkMatrix4 f; v.storeTransposed4(f);
	hkVector4 t0; t0.setAdd( f.getColumn<0>(), f.getColumn<1>() );
	hkVector4 t1; t1.setAdd( f.getColumn<2>(), f.getColumn<3>() );
	addsOut.setAdd( t0, t1 );
} };
template <> struct hAddH<2,3> { HK_FORCE_INLINE static void apply(const hkMxVector<2>& v, hkVector4& addsOut) 
{ 
	hkMatrix4 f; v.storeTransposed4(f);
	hkVector4 t0; t0.setAdd( f.getColumn<0>(), f.getColumn<1>() );
	addsOut.setAdd( t0, f.getColumn<2>() );
} };

template <int N> struct hAddH<1,N> { HK_FORCE_INLINE static void apply(const hkMxVector<1>& v, hkVector4& addsOut) 
{ 
	addsOut.setHorizontalAdd<N>(v.m_vec.v[0]);
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
template <int M, int N> struct hMinH { HK_FORCE_INLINE static void apply(hkMxVectorParameter v, hkVector4& minsOut) { HK_MXVECTOR_MX_NOT_IMPLEMENTED; } };
template <> struct hMinH<4,4> { HK_FORCE_INLINE static void apply(const hkMxVector<4>& v, hkVector4& minsOut) 
{ 
	hkMatrix4 f; v.storeTransposed4(f);
	hkVector4 t0; t0.setMin( f.getColumn<0>(), f.getColumn<1>() );
	hkVector4 t1; t1.setMin( f.getColumn<2>(), f.getColumn<3>() );
	minsOut.setMin( t0, t1 );
} };
template <> struct hMinH<4,3> { HK_FORCE_INLINE static void apply(const hkMxVector<4>& v, hkVector4& minsOut) 
{ 
	hkMatrix4 f; v.storeTransposed4(f);
	hkVector4 t0; t0.setMin( f.getColumn<0>(), f.getColumn<1>() );
	minsOut.setMin( t0, f.getColumn<2>() );
} };
template <> struct hMinH<3,4> { HK_FORCE_INLINE static void apply(const hkMxVector<3>& v, hkVector4& minsOut) 
{ 
	hkMatrix4 f; v.storeTransposed4(f);
	hkVector4 t0; t0.setMin( f.getColumn<0>(), f.getColumn<1>() );
	hkVector4 t1; t1.setMin( f.getColumn<2>(), f.getColumn<3>() );
	minsOut.setMin( t0, t1 );
} };
template <> struct hMinH<3,3> { HK_FORCE_INLINE static void apply(const hkMxVector<3>& v, hkVector4& minsOut) 
{ 
	hkMatrix4 f; v.storeTransposed4(f);
	hkVector4 t0; t0.setMin( f.getColumn<0>(), f.getColumn<1>() );
	minsOut.setMin( t0, f.getColumn<2>() );
} };
template <> struct hMinH<2,4> { HK_FORCE_INLINE static void apply(const hkMxVector<2>& v, hkVector4& minsOut) 
{ 
	hkMatrix4 f; v.storeTransposed4(f);
	hkVector4 t0; t0.setMin( f.getColumn<0>(), f.getColumn<1>() );
	hkVector4 t1; t1.setMin( f.getColumn<2>(), f.getColumn<3>() );
	minsOut.setMin( t0, t1 );
} };
template <> struct hMinH<2,3> { HK_FORCE_INLINE static void apply(const hkMxVector<2>& v, hkVector4& minsOut) 
{ 
	hkMatrix4 f; v.storeTransposed4(f);
	hkVector4 t0; t0.setMin( f.getColumn<0>(), f.getColumn<1>() );
	minsOut.setMin( t0, f.getColumn<2>() );
} };
template <int N> struct hMinH<1,N> { HK_FORCE_INLINE static void apply(const hkMxVector<1>& v, hkVector4& minsOut) 
{ 
	minsOut.setHorizontalMin<N>(v.m_vec.v[0]);
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
template <int M, int N> struct hMaxH { HK_FORCE_INLINE static void apply(hkMxVectorParameter v, hkVector4& maxsOut) { HK_MXVECTOR_MX_NOT_IMPLEMENTED; } };
template <> struct hMaxH<4,4> { HK_FORCE_INLINE static void apply(const hkMxVector<4>& v, hkVector4& maxsOut) 
{ 
	hkMatrix4 f; v.storeTransposed4(f);
	hkVector4 t0; t0.setMax( f.getColumn<0>(), f.getColumn<1>() );
	hkVector4 t1; t1.setMax( f.getColumn<2>(), f.getColumn<3>() );
	maxsOut.setMax( t0, t1 );
} };
template <> struct hMaxH<4,3> { HK_FORCE_INLINE static void apply(const hkMxVector<4>& v, hkVector4& maxsOut) 
{ 
	hkMatrix4 f; v.storeTransposed4(f);
	hkVector4 t0; t0.setMax( f.getColumn<0>(), f.getColumn<1>() );
	maxsOut.setMax( t0, f.getColumn<2>() );
} };
template <> struct hMaxH<3,4> { HK_FORCE_INLINE static void apply(const hkMxVector<3>& v, hkVector4& maxsOut) 
{ 
	hkMatrix4 f; v.storeTransposed4(f);
	hkVector4 t0; t0.setMax( f.getColumn<0>(), f.getColumn<1>() );
	hkVector4 t1; t1.setMax( f.getColumn<2>(), f.getColumn<3>() );
	maxsOut.setMax( t0, t1 );
} };
template <> struct hMaxH<3,3> { HK_FORCE_INLINE static void apply(const hkMxVector<3>& v, hkVector4& maxsOut) 
{ 
	hkMatrix4 f; v.storeTransposed4(f);
	hkVector4 t0; t0.setMax( f.getColumn<0>(), f.getColumn<1>() );
	maxsOut.setMax( t0, f.getColumn<2>() );
} };
template <> struct hMaxH<2,4> { HK_FORCE_INLINE static void apply(const hkMxVector<2>& v, hkVector4& maxsOut) 
{ 
	hkMatrix4 f; v.storeTransposed4(f);
	hkVector4 t0; t0.setMax( f.getColumn<0>(), f.getColumn<1>() );
	hkVector4 t1; t1.setMax( f.getColumn<2>(), f.getColumn<3>() );
	maxsOut.setMax( t0, t1 );
} };
template <> struct hMaxH<2,3> { HK_FORCE_INLINE static void apply(const hkMxVector<2>& v, hkVector4& maxsOut) 
{ 
	hkMatrix4 f; v.storeTransposed4(f);
	hkVector4 t0; t0.setMax( f.getColumn<0>(), f.getColumn<1>() );
	maxsOut.setMax( t0, f.getColumn<2>() );
} };
template <int N> struct hMaxH<1,N> { HK_FORCE_INLINE static void apply(const hkMxVector<1>& v, hkVector4& maxsOut) 
{ 
	maxsOut.setHorizontalMax<N>(v.m_vec.v[0]);
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
template <int I> HK_FORCE_INLINE void reduceAddH(const hkVector4* v, hkVector4& addOut) { reduceAddH<I-1>(v, addOut); addOut.add(v[I-1]); }
template <> HK_FORCE_INLINE void reduceAddH<1>(const hkVector4* v, hkVector4& addOut) { addOut = v[0]; }
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::reduceAdd(hkVector4& addOut) const
{
	hkMxVector_Implementation::reduceAddH<M>(m_vec.v, addOut);
}


namespace hkMxVector_Implementation
{
template <int I, int N> struct dotH { HK_FORCE_INLINE static void apply(const hkVector4* v, const hkVector4* v0, hkSimdReal* dotsOut) { dotH<I-1,N>::apply(v, v0, dotsOut); dotsOut[I-1] = v[I-1].dot<N>(v0[I-1]); } };
template <int N> struct dotH<1,N> { HK_FORCE_INLINE static void apply(const hkVector4* v, const hkVector4* v0, hkSimdReal* dotsOut) { dotsOut[0] = v[0].dot<N>(v0[0]); } };
}
template <int M> 
template <int N> 
HK_FORCE_INLINE void hkMxVector<M>::dot(hkMxVectorParameter v, hkMxReal<M>& dotsOut) const 
{ 
	hkMxVector_Implementation::dotH<M,N>::apply(m_vec.v, v.m_vec.v, dotsOut.m_real.r); 
}


namespace hkMxVector_Implementation
{
template <int I, int N> struct dotHSingle { HK_FORCE_INLINE static void apply(const hkVector4* v, const hkVector4& v0, hkSimdReal* dotsOut) { dotHSingle<I-1,N>::apply(v, v0, dotsOut); dotsOut[I-1] = v[I-1].dot<N>(v0); } };
template <int N> struct dotHSingle<1,N> {   HK_FORCE_INLINE static void apply(const hkVector4* v, const hkVector4& v0, hkSimdReal* dotsOut) { dotsOut[0] = v[0].dot<N>(v0); } };
}
template <int M> 
template <int N> 
HK_FORCE_INLINE void hkMxVector<M>::dot(hkMxSingleParameter v, hkMxReal<M>& dotsOut) const 
{ 
	hkMxVector_Implementation::dotHSingle<M,N>::apply(m_vec.v, v.m_single.s, dotsOut.m_real.r); 
}


namespace hkMxVector_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct gatherH {
HK_FORCE_INLINE static void apply(hkVector4* v, const hkVector4* HK_RESTRICT base)
{
	gatherH<I-1, byteAddressIncrement>::apply(v, base);
	v[I-1] = *hkAddByteOffsetConst( base, (I-1)*byteAddressIncrement );
} };
template <hkUint32 byteAddressIncrement> struct gatherH<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(hkVector4* v, const hkVector4* HK_RESTRICT base)
{
	v[0] = *base;
} };
}
template <int M>
template <hkUint32 byteAddressIncrement>
HK_FORCE_INLINE void hkMxVector<M>::gather(const hkVector4* HK_RESTRICT base)
{
	hkMxVector_Implementation::gatherH<M, byteAddressIncrement>::apply(m_vec.v, base);
}


namespace hkMxVector_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct gatherUintH {
HK_FORCE_INLINE static void apply(hkVector4* v, const hkVector4* HK_RESTRICT base, const hkUint16* indices)
{
	gatherUintH<I-1, byteAddressIncrement>::apply(v, base, indices);
	v[I-1] = *hkAddByteOffsetConst( base, indices[I-1] * byteAddressIncrement );
} };
template <hkUint32 byteAddressIncrement> struct gatherUintH<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(hkVector4* v, const hkVector4* HK_RESTRICT base, const hkUint16* indices)
{
	v[0] = *hkAddByteOffsetConst( base, indices[0] * byteAddressIncrement );
} };
}
template <int M>
template <hkUint32 byteAddressIncrement>
HK_FORCE_INLINE void hkMxVector<M>::gather(const hkVector4* HK_RESTRICT base, const hkUint16* indices)
{
	hkMxVector_Implementation::gatherUintH<M, byteAddressIncrement>::apply(m_vec.v, base, indices);
}


namespace hkMxVector_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct gatherIntH {
HK_FORCE_INLINE static void apply(hkVector4* v, const hkVector4* HK_RESTRICT base, const hkInt32* indices)
{
	gatherIntH<I-1, byteAddressIncrement>::apply(v, base, indices);
	v[I-1] = *hkAddByteOffsetConst( base, indices[I-1] * byteAddressIncrement );
} };
template <hkUint32 byteAddressIncrement> struct gatherIntH<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(hkVector4* v, const hkVector4* HK_RESTRICT base, const hkInt32* indices)
{
	v[0] = *hkAddByteOffsetConst( base, indices[0] * byteAddressIncrement );
} };
}
template <int M>
template <hkUint32 byteAddressIncrement>
HK_FORCE_INLINE void hkMxVector<M>::gather(const hkVector4* HK_RESTRICT base, const hkInt32* indices)
{
	hkMxVector_Implementation::gatherIntH<M, byteAddressIncrement>::apply(m_vec.v, base, indices);
}


namespace hkMxVector_Implementation
{
	template <int I, hkUint32 byteOffset> struct gatherWithOffsetH {
	HK_FORCE_INLINE static void apply(hkVector4* v, const void** base)
	{
		const hkVector4* HK_RESTRICT ptr0 = (const hkVector4*)hkAddByteOffsetConst(base[I-1], byteOffset); 
		gatherWithOffsetH<I-1, byteOffset>::apply(v, base);
		v[I-1] = *ptr0;
	} };
template <hkUint32 byteOffset> struct gatherWithOffsetH<1, byteOffset> {
		HK_FORCE_INLINE static void apply(hkVector4* v, const void** base)
		{
	const hkVector4* HK_RESTRICT ptr = (const hkVector4*)hkAddByteOffsetConst(base[0], byteOffset); 
	v[0] = *ptr;
} };
}
template <int M>
template <hkUint32 byteOffset> 
HK_FORCE_INLINE void hkMxVector<M>::gatherWithOffset(const void* base[M])
{
	hkMxVector_Implementation::gatherWithOffsetH<M, byteOffset>::apply(m_vec.v, base);
}





namespace hkMxVector_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct scatterUintH {
HK_FORCE_INLINE static void apply(const hkVector4* v, hkVector4* base, const hkUint16* indices)
{
	hkVector4* HK_RESTRICT ptr = hkAddByteOffset(base, indices[I-1] * byteAddressIncrement);
	scatterUintH<I-1, byteAddressIncrement>::apply(v, base, indices);
	*ptr = v[I-1];
} };
template <hkUint32 byteAddressIncrement> struct scatterUintH<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkVector4* v, hkVector4* base, const hkUint16* indices)
{
	hkVector4* HK_RESTRICT ptr = hkAddByteOffset(base, indices[0] * byteAddressIncrement);
	*ptr = v[0];
} };
}
template <int M>
template <hkUint32 byteAddressIncrement>
HK_FORCE_INLINE void hkMxVector<M>::scatter(hkVector4* base, const hkUint16* indices) const
{
	hkMxVector_Implementation::scatterUintH<M, byteAddressIncrement>::apply(m_vec.v, base, indices);
}



namespace hkMxVector_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct scatterIntH {
HK_FORCE_INLINE static void apply(const hkVector4* v, hkVector4* base, const hkInt32* indices)
{
	hkVector4* HK_RESTRICT ptr = hkAddByteOffset(base, indices[I-1] * byteAddressIncrement);
	scatterIntH<I-1, byteAddressIncrement>::apply(v, base, indices);
	*ptr = v[I-1];
} };
template <hkUint32 byteAddressIncrement> struct scatterIntH<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkVector4* v, hkVector4* base, const hkInt32* indices)
{
	hkVector4* HK_RESTRICT ptr = hkAddByteOffset(base, indices[0] * byteAddressIncrement);
	*ptr = v[0];
} };
}
template <int M>
template <hkUint32 byteAddressIncrement>
HK_FORCE_INLINE void hkMxVector<M>::scatter(hkVector4* base, const hkInt32* indices) const
{
	hkMxVector_Implementation::scatterIntH<M, byteAddressIncrement>::apply(m_vec.v, base, indices);
}



namespace hkMxVector_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct scatterH {
HK_FORCE_INLINE static void apply(const hkVector4* v, hkVector4* base)
{
	hkVector4* HK_RESTRICT ptr0 = hkAddByteOffset(base, (I-1) * byteAddressIncrement); 
	scatterH<I-1, byteAddressIncrement>::apply(v, base);
	*ptr0 = v[I-1];
} };
template <hkUint32 byteAddressIncrement> struct scatterH<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkVector4* v, hkVector4* base)
{
	*base = v[0];
} };
}
template <int M>
template <hkUint32 byteAddressIncrement>
HK_FORCE_INLINE void hkMxVector<M>::scatter(hkVector4* base) const
{
	hkMxVector_Implementation::scatterH<M, byteAddressIncrement>::apply(m_vec.v, base);
}



namespace hkMxVector_Implementation
{
template <int I, hkUint32 byteOffset> struct scatterWithOffsetH {
HK_FORCE_INLINE static void apply(const hkVector4* v, void** base)
{
	hkVector4* HK_RESTRICT ptr0 = (hkVector4*)hkAddByteOffset(base[I-1], byteOffset); 
	scatterWithOffsetH<I-1, byteOffset>::apply(v, base);
	*ptr0 = v[I-1];
} };
template <hkUint32 byteOffset> struct scatterWithOffsetH<1, byteOffset> {
HK_FORCE_INLINE static void apply(const hkVector4* v, void** base)
{
	hkVector4* HK_RESTRICT ptr0 = (hkVector4*)hkAddByteOffset(base[0], byteOffset); 
	*ptr0 = v[0];
} };
}
template <int M>
template <hkUint32 byteOffset> 
HK_FORCE_INLINE void hkMxVector<M>::scatterWithOffset(void* base[M]) const 
{
	hkMxVector_Implementation::scatterWithOffsetH<M, byteOffset>::apply(m_vec.v, base);
}


namespace hkMxVector_Implementation
{
template <int I, int N, hkMathAccuracyMode A, hkMathNegSqrtMode S> struct lengthH { HK_FORCE_INLINE static void apply(const hkVector4* v, hkSimdReal* lensOut) { lengthH<I-1,N,A,S>::apply(v, lensOut); lensOut[I-1] = v[I-1].length<N,A,S>(); } };
template <int N, hkMathAccuracyMode A, hkMathNegSqrtMode S> struct lengthH<1,N,A,S> { HK_FORCE_INLINE static void apply(const hkVector4* v, hkSimdReal* lensOut) { lensOut[0] = v[0].length<N,A,S>(); } };
}
template <int M> template <int N, hkMathAccuracyMode A, hkMathNegSqrtMode S> HK_FORCE_INLINE void hkMxVector<M>::length(hkMxReal<M>& lensOut) const { hkMxVector_Implementation::lengthH<M,N,A,S>::apply(m_vec.v, lensOut.m_real.r); }

template <int M> template <int N> HK_FORCE_INLINE void hkMxVector<M>::length(hkMxReal<M>& lensOut) const { hkMxVector_Implementation::lengthH<M,N,HK_ACC_23_BIT,HK_SQRT_SET_ZERO>::apply(m_vec.v, lensOut.m_real.r); }

namespace hkMxVector_Implementation
{
template <int I, int N> struct lengthSqrH { HK_FORCE_INLINE static void apply(const hkVector4* v, hkSimdReal* lensOut) { lengthSqrH<I-1,N>::apply(v, lensOut); lensOut[I-1] = v[I-1].lengthSquared<N>(); } };
template <int N> struct lengthSqrH<1,N> { HK_FORCE_INLINE static void apply(const hkVector4* v, hkSimdReal* lensOut) { lensOut[0] = v[0].lengthSquared<N>(); } };
}
template <int M> template <int N> HK_FORCE_INLINE void hkMxVector<M>::lengthSquared(hkMxReal<M>& lensOut) const { hkMxVector_Implementation::lengthSqrH<M,N>::apply(m_vec.v, lensOut.m_real.r); }

namespace hkMxVector_Implementation
{
template <int I, int N, hkMathAccuracyMode A, hkMathNegSqrtMode S> struct lengthInvH { HK_FORCE_INLINE static void apply(const hkVector4* v, hkSimdReal* lensOut) { lengthInvH<I-1,N,A,S>::apply(v, lensOut); lensOut[I-1] = v[I-1].lengthInverse<N,A,S>(); } };
template <int N, hkMathAccuracyMode A, hkMathNegSqrtMode S> struct lengthInvH<1,N,A,S> { HK_FORCE_INLINE static void apply(const hkVector4* v, hkSimdReal* lensOut) { lensOut[0] = v[0].lengthInverse<N,A,S>(); } };
}
template <int M> template <int N, hkMathAccuracyMode A, hkMathNegSqrtMode S> HK_FORCE_INLINE void hkMxVector<M>::lengthInverse(hkMxReal<M>& lensOut) const { hkMxVector_Implementation::lengthInvH<M,N,A,S>::apply(m_vec.v, lensOut.m_real.r); }

template <int M> template <int N> HK_FORCE_INLINE void hkMxVector<M>::lengthInverse(hkMxReal<M>& lensOut) const { hkMxVector_Implementation::lengthInvH<M,N,HK_ACC_23_BIT,HK_SQRT_SET_ZERO>::apply(m_vec.v, lensOut.m_real.r); }



namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void selectH(hkVector4* v, const hkVector4Comparison* mask, const hkVector4* tVal, const hkVector4* fVal) { selectH<I-1>(v, mask, tVal, fVal); v[I-1].setSelect(mask[I-1], tVal[I-1], fVal[I-1]); }
template <> HK_FORCE_INLINE void selectH<1>(hkVector4* v, const hkVector4Comparison* mask, const hkVector4* tVal, const hkVector4* fVal) { v[0].setSelect(mask[0], tVal[0], fVal[0]); }
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::setSelect(hkMxMaskParameter mask, hkMxVectorParameter trueValue, hkMxVectorParameter falseValue )
{
	hkMxVector_Implementation::selectH<M>(m_vec.v, mask.m_comp.c, trueValue.m_vec.v, falseValue.m_vec.v);
}
namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void selectSH(hkVector4* v, const hkVector4Comparison* mask, hkVector4Parameter tVal, const hkVector4* fVal) { selectSH<I-1>(v, mask, tVal, fVal); v[I-1].setSelect(mask[I-1], tVal, fVal[I-1]); }
template <> HK_FORCE_INLINE void selectSH<1>(hkVector4* v, const hkVector4Comparison* mask, hkVector4Parameter tVal, const hkVector4* fVal) { v[0].setSelect(mask[0], tVal, fVal[0]); }
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::setSelect(hkMxMaskParameter mask, hkMxSingleParameter trueValue, hkMxVectorParameter falseValue )
{
	hkMxVector_Implementation::selectSH<M>(m_vec.v, mask.m_comp.c, trueValue.m_single.s, falseValue.m_vec.v);
}
namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void selectHS(hkVector4* v, const hkVector4Comparison* mask, const hkVector4* tVal, hkVector4Parameter fVal) { selectHS<I-1>(v, mask, tVal, fVal); v[I-1].setSelect(mask[I-1], tVal[I-1], fVal); }
template <> HK_FORCE_INLINE void selectHS<1>(hkVector4* v, const hkVector4Comparison* mask, const hkVector4* tVal, hkVector4Parameter fVal) { v[0].setSelect(mask[0], tVal[0], fVal); }
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::setSelect(hkMxMaskParameter mask, hkMxVectorParameter trueValue, hkMxSingleParameter falseValue )
{
	hkMxVector_Implementation::selectHS<M>(m_vec.v, mask.m_comp.c, trueValue.m_vec.v, falseValue.m_single.s);
}


namespace hkMxVector_Implementation
{
	template <int I, int idx> struct revertH {
	HK_FORCE_INLINE static void apply(hkVector4* v, const hkVector4* v0)
	{ 
		revertH<I-1,idx+1>::apply(v, v0); 
		v[I-1] = v0[idx]; 
	} };
	template <int idx> struct revertH<1,idx> {
	HK_FORCE_INLINE static void apply(hkVector4* v, const hkVector4* v0)
	{
		v[0] = v0[idx]; 
	} };
	template <int I> 
	HK_FORCE_INLINE void leftCH(hkVector4* v, const hkVector4* v0)
	{ 
		leftCH<I-1>(v, v0); 
		v[I-1] = v0[I]; 
	}
	template <> 
	HK_FORCE_INLINE void leftCH<1>(hkVector4* v, const hkVector4* v0)
	{
		v[0] = v0[1]; 
	}
	template <int I> 
	HK_FORCE_INLINE void rightCH(hkVector4* v, const hkVector4* v0)
	{ 
		rightCH<I-1>(v, v0); 
		v[I] = v0[I-1]; 
	}
	template <> 
	HK_FORCE_INLINE void rightCH<1>(hkVector4* v, const hkVector4* v0)
	{
		v[1] = v0[0]; 
	}
}

template <int M>
template <hkMxVectorPermutation::Permutation P>
HK_FORCE_INLINE void hkMxVector<M>::setPermutation(hkMxVectorParameter m)
{
	HK_COMPILE_TIME_ASSERT2((P==hkMxVectorPermutation::SHIFT_LEFT_CYCLIC)||(P==hkMxVectorPermutation::SHIFT_RIGHT_CYCLIC)||(P==hkMxVectorPermutation::REVERSE),MX_UNKNOWN_PERMUTATION);
	HK_ASSERT2(0xf03454f5, ((hkUlong)m_vec.v >= (hkUlong)(m.m_vec.v+M)) || ((hkUlong)m.m_vec.v >= (hkUlong)(m_vec.v+M)), "unsafe permutation");

	if (P == hkMxVectorPermutation::SHIFT_LEFT_CYCLIC )
	{
		hkMxVector_Implementation::leftCH<M-1>(m_vec.v, m.m_vec.v);
		m_vec.v[M-1] = m.m_vec.v[0];
	}
	else if ( P == hkMxVectorPermutation::SHIFT_RIGHT_CYCLIC )
	{
		m_vec.v[0] = m.m_vec.v[M-1];
		hkMxVector_Implementation::rightCH<M-1>(m_vec.v, m.m_vec.v);
	}
	else /*if ( P == hkMxVectorPermutation::REVERSE )*/
	{
		hkMxVector_Implementation::revertH<M,0>::apply(m_vec.v, m.m_vec.v);
	}
}


namespace hkMxVector_Implementation
{
template <int I> HK_FORCE_INLINE void setAsBroadcastH(hkVector4* v, hkVector4Parameter v0) { setAsBroadcastH<I-1>(v, v0); v[I-1].setAll(v0.getComponent<I-1>()); }
template <> HK_FORCE_INLINE void setAsBroadcastH<1>(hkVector4* v, hkVector4Parameter v0) { v[0].setAll(v0.getComponent<0>()); }
}
template <int M>
HK_FORCE_INLINE void hkMxVector<M>::setAsBroadcast(hkVector4Parameter v)
{
	hkMxVector_Implementation::setAsBroadcastH<M>(m_vec.v, v); // safe, v.getComponent will assert M<=4
}

#undef MXV_NO_OPERANDS
#undef MXV_OP_MXVECTOR
#undef MXV_OP_MXVECTOR_MXVECTOR
#undef MXV_OP_MXVECTOR_MXVECTOR_MXVECTOR
#undef MXV_OP_MXVECTOR_MXVECTOR_MXSINGLE
#undef MXV_OP_MXVECTOR_MXVECTOR_MXREAL
#undef MXV_OP_MXVECTOR_MXSINGLE_MXREAL
#undef MXV_OP_MXREAL
#undef MXV_OP_MXSINGLE
#undef MXV_OP_MXVECTOR_MXREAL
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
