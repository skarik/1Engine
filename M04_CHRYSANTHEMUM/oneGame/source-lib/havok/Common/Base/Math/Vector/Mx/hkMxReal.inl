/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#define MXR_TWO_OPERANDS(METHOD, OP) namespace hkMxReal_Implementation { template <int I> HK_FORCE_INLINE void OP##HR(hkSimdReal* r, const hkSimdReal* r0, const hkSimdReal* r1) { OP##HR<I-1>(r, r0, r1); r[I-1].OP(r0[I-1], r1[I-1]); } template <> HK_FORCE_INLINE void OP##HR<1>(hkSimdReal* r, const hkSimdReal* r0, const hkSimdReal* r1) { r[0].OP(r0[0], r1[0]); } } template <int M> HK_FORCE_INLINE void hkMxReal<M>::METHOD(hkMxRealParameter r0, hkMxRealParameter r1) { hkMxReal_Implementation::OP##HR<M>(m_real.r, r0.m_real.r, r1.m_real.r); }
#define MXR_MIXED_OPERANDS(METHOD, OP) namespace hkMxReal_Implementation { template <int I> HK_FORCE_INLINE void OP##SHR(hkSimdReal* r, hkVector4Parameter r0, const hkVector4* r1) { OP##SHR<I-1>(r, r0, r1); r[I-1] = r0.OP(r1[I-1]); } template <> HK_FORCE_INLINE void OP##SHR<1>(hkSimdReal* r, hkVector4Parameter r0, const hkVector4* r1) { r[0] = r0.OP(r1[0]); } } template <int M> HK_FORCE_INLINE void hkMxReal<M>::METHOD(hkMxSingleParameter r0, hkMxVectorParameter r1) { hkMxReal_Implementation::OP##SHR<M>(m_real.r, r0.m_single.s, r1.m_vec.v); }
#define MXR_TWO_V_OPERANDS(METHOD, OP) namespace hkMxReal_Implementation { template <int I> HK_FORCE_INLINE void OP##VHR(hkSimdReal* r, const hkVector4* v0, const hkVector4* v1) { OP##VHR<I-1>(r, v0, v1); r[I-1] = v0[I-1].OP(v1[I-1]); } template <> HK_FORCE_INLINE void OP##VHR<1>(hkSimdReal* r, const hkVector4* v0, const hkVector4* v1) { r[0] = v0[0].OP(v1[0]); } } template <int M> HK_FORCE_INLINE void hkMxReal<M>::METHOD(hkMxVectorParameter v0, hkMxVectorParameter v1) { hkMxReal_Implementation::OP##VHR<M>(m_real.r, v0.m_vec.v, v1.m_vec.v); }



MXR_TWO_OPERANDS( setMin, setMin )
MXR_TWO_OPERANDS( setMax, setMax )

namespace hkMxReal_Implementation
{
template <int I, int N> struct setDotVHR { HK_FORCE_INLINE static void apply(hkSimdReal* r, const hkVector4* v0, const hkVector4* v1) { setDotVHR<I-1,N>::apply(r, v0, v1); r[I-1] = v0[I-1].dot<N>(v1[I-1]); } };
template <int N> struct setDotVHR<1,N> { HK_FORCE_INLINE static void apply(hkSimdReal* r, const hkVector4* v0, const hkVector4* v1) { r[0] = v0[0].dot<N>(v1[0]); } };
}
template <int M> template <int N> HK_FORCE_INLINE void hkMxReal<M>::setDot(hkMxVectorParameter v0, hkMxVectorParameter v1) 
{ 
	hkMxReal_Implementation::setDotVHR<M,N>::apply(m_real.r, v0.m_vec.v, v1.m_vec.v); 
}

MXR_TWO_V_OPERANDS( setDot4xyz1, dot4xyz1 )


namespace hkMxReal_Implementation
{
template <int I, int N> struct setDotSHR { HK_FORCE_INLINE static void apply(hkSimdReal* r, hkVector4Parameter r0, const hkVector4* r1) { setDotSHR<I-1,N>::apply(r, r0, r1); r[I-1] = r0.dot<N>(r1[I-1]); } };
template <int N> struct setDotSHR<1,N> { HK_FORCE_INLINE static void apply(hkSimdReal* r, hkVector4Parameter r0, const hkVector4* r1) { r[0] = r0.dot<N>(r1[0]); } };
}
template <int M> template <int N> HK_FORCE_INLINE void hkMxReal<M>::setDot(hkMxSingleParameter r0, hkMxVectorParameter r1) 
{ 
	hkMxReal_Implementation::setDotSHR<M,N>::apply(m_real.r, r0.m_single.s, r1.m_vec.v); 
}

MXR_MIXED_OPERANDS( setDot4xyz1, dot4xyz1 )



#undef MXR_TWO_OPERANDS
#undef MXR_TWO_V_OPERANDS
#undef MXR_MIXED_OPERANDS

#ifndef HK_DISABLE_MATH_CONSTRUCTORS

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void constructHRS(hkSimdReal* r, hkReal& v) { constructHRS<I-1>(r, v); r[I-1].setFromFloat(v); }
template <> HK_FORCE_INLINE void constructHRS<1>(hkSimdReal* r, hkReal& v) { r[0].setFromFloat(v); }
}
template <int M>
HK_FORCE_INLINE hkMxReal<M>::hkMxReal(hkReal& v)
{
	hkMxReal_Implementation::constructHRS<M>(m_real.r, v);
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void constructHRSS(hkSimdReal* r, hkSimdRealParameter v) { constructHRSS<I-1>(r, v); r[I-1] = v; }
template <> HK_FORCE_INLINE void constructHRSS<1>(hkSimdReal* r, hkSimdRealParameter v) { r[0] = v; }
}
template <int M>
HK_FORCE_INLINE hkMxReal<M>::hkMxReal(hkSimdRealParameter v)
{
	hkMxReal_Implementation::constructHRSS<M>(m_real.r, v);
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void constructHR(hkSimdReal* v, hkVector4Parameter v0) { constructHR<I-1>(v, v0); v[I-1] = v0.getComponent<I-1>(); }
template <> HK_FORCE_INLINE void constructHR<1>(hkSimdReal* v, hkVector4Parameter v0) { v[0] = v0.getComponent<0>(); }
}
template <int M>
HK_FORCE_INLINE hkMxReal<M>::hkMxReal(hkVector4Parameter v)
{
	hkMxReal_Implementation::constructHR<M>(m_real.r, v); // safe, v.getComponent<> asserts M<4
}

#endif

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void moveHR(hkSimdReal* HK_RESTRICT v, const hkSimdReal* HK_RESTRICT v0) { moveHR<I-1>(v, v0); v[I-1] = v0[I-1]; }
template <> HK_FORCE_INLINE void moveHR<1>(hkSimdReal* HK_RESTRICT v, const hkSimdReal* HK_RESTRICT v0) { v[0] = v0[0]; }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::operator= ( hkMxRealParameter v )
{
	HK_ASSERT2(0x20baff20, ((hkUlong)m_real.r >= (hkUlong)(v.m_real.r+M)) || ((hkUlong)v.m_real.r >= (hkUlong)(m_real.r+M)), "unsafe move operation");
	hkMxReal_Implementation::moveHR<M>(m_real.r, v.m_real.r);
}


namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void setBCHR(hkSimdReal* v, hkVector4Parameter v0) { setBCHR<I-1>(v, v0); v[I-1] = v0.getComponent<I-1>(); }
template <> HK_FORCE_INLINE void setBCHR<1>(hkSimdReal* v, hkVector4Parameter v0) { v[0] = v0.getComponent<0>(); }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::setBroadcast( hkVector4Parameter r )
{
	hkMxReal_Implementation::setBCHR<M>(m_real.r, r); // safe, v.getComponent<> asserts M<4
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void setBCHRS(hkSimdReal* v, hkSimdRealParameter v0) { setBCHRS<I-1>(v, v0); v[I-1] = v0; }
template <> HK_FORCE_INLINE void setBCHRS<1>(hkSimdReal* v, hkSimdRealParameter v0) { v[0] = v0; }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::setBroadcast( hkSimdRealParameter r )
{
	hkMxReal_Implementation::setBCHRS<M>(m_real.r, r);
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void loadHR(hkSimdReal* v, const hkReal* r) { loadHR<I-1>(v, r); v[I-1].load<1>(r + (I-1)); }
template <> HK_FORCE_INLINE void loadHR<1>(hkSimdReal* v, const hkReal* r) { v[0].load<1>(r); }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::load(const hkReal* r)
{
	hkMxReal_Implementation::loadHR<M>(m_real.r, r);
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void loadNHR(hkSimdReal* v, const hkReal* r) { loadNHR<I-1>(v, r); v[I-1].load<1,HK_IO_NATIVE_ALIGNED>(r + (I-1)); }
template <> HK_FORCE_INLINE void loadNHR<1>(hkSimdReal* v, const hkReal* r) { v[0].load<1,HK_IO_NATIVE_ALIGNED>(r); }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::loadNotAligned(const hkReal* r)
{
	hkMxReal_Implementation::loadNHR<M>(m_real.r, r);
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void loadCHR(hkSimdReal* v, const hkReal* r) { loadCHR<I-1>(v, r); v[I-1].load<1,HK_IO_NOT_CACHED>(r + (I-1)); }
template <> HK_FORCE_INLINE void loadCHR<1>(hkSimdReal* v, const hkReal* r) { v[0].load<1,HK_IO_NOT_CACHED>(r); }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::loadNotCached(const hkReal* r)
{
	hkMxReal_Implementation::loadCHR<M>(m_real.r, r);
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void storeHR(const hkSimdReal* v, hkReal* r) { storeHR<I-1>(v, r); v[I-1].store<1>(r + (I-1)); }
template <> HK_FORCE_INLINE void storeHR<1>(const hkSimdReal* v, hkReal* r) { v[0].store<1>(r); }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::store(hkReal* rOut) const
{
	hkMxReal_Implementation::storeHR<M>(m_real.r, rOut);
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void storeNHR(const hkSimdReal* v, hkReal* r) { storeNHR<I-1>(v, r); v[I-1].store<1,HK_IO_NATIVE_ALIGNED>(r + (I-1)); }
template <> HK_FORCE_INLINE void storeNHR<1>(const hkSimdReal* v, hkReal* r) { v[0].store<1,HK_IO_NATIVE_ALIGNED>(r); }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::storeNotAligned(hkReal* rOut) const
{
	hkMxReal_Implementation::storeNHR<M>(m_real.r, rOut);
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void storeCHR(const hkSimdReal* v, hkReal* r) { storeCHR<I-1>(v, r); v[I-1].store<1,HK_IO_NOT_CACHED>(r + (I-1)); }
template <> HK_FORCE_INLINE void storeCHR<1>(const hkSimdReal* v, hkReal* r) { v[0].store<1,HK_IO_NOT_CACHED>(r); }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::storeNotCached(hkReal* rOut) const
{
	hkMxReal_Implementation::storeCHR<M>(m_real.r, rOut);
}



namespace hkMxReal_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct gatherHR {
HK_FORCE_INLINE static void apply(hkSimdReal* v, const hkReal* HK_RESTRICT base)
{
	gatherHR<I-1, byteAddressIncrement>::apply(v, base);
	v[I-1].setFromFloat( *hkAddByteOffsetConst( base, (I-1)*byteAddressIncrement ) );
} };
template <hkUint32 byteAddressIncrement> struct gatherHR<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(hkSimdReal* v, const hkReal* HK_RESTRICT base)
{
	v[0].setFromFloat(*base);
} };
}
template <int M>
template <hkUint32 byteAddressIncrement>
HK_FORCE_INLINE void hkMxReal<M>::gather(const hkReal* HK_RESTRICT base)
{
	hkMxReal_Implementation::gatherHR<M, byteAddressIncrement>::apply(m_real.r, base);
}

namespace hkMxReal_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct gatherUintHR {
HK_FORCE_INLINE static void apply(hkSimdReal* v, const hkReal* HK_RESTRICT base, const hkUint16* indices)
{
	gatherUintHR<I-1, byteAddressIncrement>::apply(v, base, indices);
	v[I-1].setFromFloat( *hkAddByteOffsetConst( base, indices[I-1] * byteAddressIncrement ) );
} };
template <hkUint32 byteAddressIncrement> struct gatherUintHR<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(hkSimdReal* v, const hkReal* HK_RESTRICT base, const hkUint16* indices)
{
	v[0].setFromFloat(*hkAddByteOffsetConst( base, indices[0] * byteAddressIncrement ));
} };
}
template <int M>
template <hkUint32 byteAddressIncrement>
HK_FORCE_INLINE void hkMxReal<M>::gather(const hkReal* HK_RESTRICT base, const hkUint16* indices)
{
	hkMxReal_Implementation::gatherUintHR<M, byteAddressIncrement>::apply(m_real.r, base, indices);
}

namespace hkMxReal_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct gatherIntHR {
HK_FORCE_INLINE static void apply(hkSimdReal* v, const hkReal* HK_RESTRICT base, const hkInt32* indices)
{
	gatherIntHR<I-1, byteAddressIncrement>::apply(v, base, indices);
	v[I-1].setFromFloat( *hkAddByteOffsetConst( base, indices[I-1] * byteAddressIncrement ) );
} };
template <hkUint32 byteAddressIncrement> struct gatherIntHR<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(hkSimdReal* v, const hkReal* HK_RESTRICT base, const hkInt32* indices)
{
	v[0].setFromFloat( *hkAddByteOffsetConst( base, indices[0] * byteAddressIncrement ));
} };
}
template <int M>
template <hkUint32 byteAddressIncrement>
HK_FORCE_INLINE void hkMxReal<M>::gather(const hkReal* HK_RESTRICT base, const hkInt32* indices)
{
	hkMxReal_Implementation::gatherIntHR<M, byteAddressIncrement>::apply(m_real.r, base, indices);
}

namespace hkMxReal_Implementation
{
template <int I, hkUint32 byteOffset> struct gatherWithOffsetHR {
HK_FORCE_INLINE static void apply(hkSimdReal* v, const void** base)
{
	const hkReal* HK_RESTRICT ptr0 = (const hkReal*)hkAddByteOffsetConst(base[I-1], byteOffset); 
	gatherWithOffsetHR<I-1, byteOffset>::apply(v, base);
	v[I-1].setFromFloat( *ptr0 );
} };
template <hkUint32 byteOffset> struct gatherWithOffsetHR<1, byteOffset> {
HK_FORCE_INLINE static void apply(hkSimdReal* v, const void** base)
{
	const hkReal* HK_RESTRICT ptr = (const hkReal*)hkAddByteOffsetConst(base[0], byteOffset); 
	v[0].setFromFloat(*ptr);
} };
}
template <int M>
template <hkUint32 byteOffset> 
HK_FORCE_INLINE void hkMxReal<M>::gatherWithOffset(const void* base[M])
{
	hkMxReal_Implementation::gatherWithOffsetHR<M, byteOffset>::apply(m_real.r, base);
}




namespace hkMxReal_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct scatterHR {
HK_FORCE_INLINE static void apply(const hkSimdReal* v, hkReal* base)
{
	scatterHR<I-1, byteAddressIncrement>::apply(v, base);
	v[I-1].store<1>(hkAddByteOffset(base, (I-1)*byteAddressIncrement));
} };
template <hkUint32 byteAddressIncrement> struct scatterHR<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkSimdReal* v, hkReal* base)
{
	v[0].store<1>(base);
} };
}
template <int M>
template <hkUint32 byteAddressIncrement>
HK_FORCE_INLINE void hkMxReal<M>::scatter(hkReal* base) const
{
	hkMxReal_Implementation::scatterHR<M, byteAddressIncrement>::apply(m_real.r, base);
}

namespace hkMxReal_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct scatterUintHR {
HK_FORCE_INLINE static void apply(const hkSimdReal* v, hkReal* base, const hkUint16* indices)
{
	hkReal* HK_RESTRICT ptr = hkAddByteOffset(base, indices[I-1] * byteAddressIncrement);
	scatterUintHR<I-1, byteAddressIncrement>::apply(v, base, indices);
	v[I-1].store<1>(ptr);
} };
template <hkUint32 byteAddressIncrement> struct scatterUintHR<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkSimdReal* v, hkReal* base, const hkUint16* indices)
{
	hkReal* HK_RESTRICT ptr = hkAddByteOffset(base, indices[0] * byteAddressIncrement);
	v[0].store<1>(ptr);
} };
}
template <int M>
template <hkUint32 byteAddressIncrement>
HK_FORCE_INLINE void hkMxReal<M>::scatter(hkReal* base, const hkUint16* indices) const
{
	hkMxReal_Implementation::scatterUintHR<M, byteAddressIncrement>::apply(m_real.r, base, indices);
}

namespace hkMxReal_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct scatterIntHR {
HK_FORCE_INLINE static void apply(const hkSimdReal* v, hkReal* base, const hkInt32* indices)
{
	hkReal* HK_RESTRICT ptr = hkAddByteOffset(base, indices[I-1] * byteAddressIncrement);
	scatterIntHR<I-1, byteAddressIncrement>::apply(v, base, indices);
	v[I-1].store<1>(ptr);
} };
template <hkUint32 byteAddressIncrement> struct scatterIntHR<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkSimdReal* v, hkReal* base, const hkInt32* indices)
{
	hkReal* HK_RESTRICT ptr = hkAddByteOffset(base, indices[0] * byteAddressIncrement);
	v[0].store<1>(ptr);
} };
}
template <int M>
template <hkUint32 byteAddressIncrement>
HK_FORCE_INLINE void hkMxReal<M>::scatter(hkReal* base, const hkInt32* indices) const
{
	hkMxReal_Implementation::scatterIntHR<M, byteAddressIncrement>::apply(m_real.r, base, indices);
}

namespace hkMxReal_Implementation
{
template <int I, hkUint32 byteOffset> struct scatterWithOffsetHR {
HK_FORCE_INLINE static void apply(const hkSimdReal* v, void** base)
{
	hkReal* HK_RESTRICT ptr0 = (hkReal*)hkAddByteOffset(base[I-1], byteOffset); 
	scatterWithOffsetHR<I-1, byteOffset>::apply(v, base);
	v[I-1].store<1>(ptr0);
} };
template <hkUint32 byteOffset> struct scatterWithOffsetHR<1, byteOffset> {
HK_FORCE_INLINE static void apply(const hkSimdReal* v, void** base)
{
	hkReal* HK_RESTRICT ptr0 = (hkReal*)hkAddByteOffset(base[0], byteOffset); 
	v[0].store<1>(ptr0);
} };
}
template <int M>
template <hkUint32 byteOffset> 
HK_FORCE_INLINE void hkMxReal<M>::scatterWithOffset(void* base[M]) const 
{
	hkMxReal_Implementation::scatterWithOffsetHR<M, byteOffset>::apply(m_real.r, base);
}








template <int M>
template <int I>
HK_FORCE_INLINE void hkMxReal<M>::setReal(hkSimdRealParameter r)
{
	HK_MXVECTOR_MX_SUBINDEX_CHECK;
	m_real.r[I] = r;
}

template <int M>
template <int I> 
HK_FORCE_INLINE void hkMxReal<M>::getAsBroadcast(hkVector4& vOut) const
{
	HK_MXVECTOR_MX_SUBINDEX_CHECK;
	vOut.setAll(m_real.r[I]);
}

template <int M>
template <int I> 
HK_FORCE_INLINE const hkVector4 hkMxReal<M>::getAsBroadcast() const
{
	HK_MXVECTOR_MX_SUBINDEX_CHECK;
	hkVector4 h; h.setAll(m_real.r[I]);
	return h;
}

template <int M>
template <int I> 
HK_FORCE_INLINE const hkSimdReal hkMxReal<M>::getReal() const
{
	HK_MXVECTOR_MX_SUBINDEX_CHECK;
	return m_real.r[I];
}


namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void setConstHR(hkSimdReal* v, hkSimdRealParameter vC) { setConstHR<I-1>(v,vC); v[I-1] = vC; }
template <> HK_FORCE_INLINE void setConstHR<1>(hkSimdReal* v, hkSimdRealParameter vC) { v[0] = vC; }
}
template <int M>
template<int vectorConstant> 
HK_FORCE_INLINE void hkMxReal<M>::setConstant()
{
	const hkSimdReal vC = hkSimdReal::getConstant<vectorConstant>();
	hkMxReal_Implementation::setConstHR<M>(m_real.r, vC);
}


namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void addHR(hkSimdReal* v, const hkSimdReal* v0) { addHR<I-1>(v, v0); v[I-1] = v[I-1] + v0[I-1]; }
template <> HK_FORCE_INLINE void addHR<1>(hkSimdReal* v, const hkSimdReal* v0) { v[0] = v[0] + v0[0]; }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::add(hkMxRealParameter r)
{
	hkMxReal_Implementation::addHR<M>(m_real.r, r.m_real.r);
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void subHR(hkSimdReal* v, const hkSimdReal* v0) { subHR<I-1>(v, v0); v[I-1] = v[I-1] - v0[I-1]; }
template <> HK_FORCE_INLINE void subHR<1>(hkSimdReal* v, const hkSimdReal* v0) { v[0] = v[0] - v0[0]; }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::sub(hkMxRealParameter r)
{
	hkMxReal_Implementation::subHR<M>(m_real.r, r.m_real.r);
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void mulHR(hkSimdReal* v, const hkSimdReal* v0) { mulHR<I-1>(v, v0); v[I-1] = v[I-1] * v0[I-1]; }
template <> HK_FORCE_INLINE void mulHR<1>(hkSimdReal* v, const hkSimdReal* v0) { v[0] = v[0] * v0[0]; }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::mul(hkMxRealParameter r)
{
	hkMxReal_Implementation::mulHR<M>(m_real.r, r.m_real.r);
}

namespace hkMxReal_Implementation
{
template <int I, hkMathAccuracyMode A, hkMathDivByZeroMode D> struct divHR { HK_FORCE_INLINE static void apply(hkSimdReal* v, const hkSimdReal* v0) { divHR<I-1,A,D>::apply(v, v0); v[I-1].div<A,D>(v0[I-1]); } };
template <hkMathAccuracyMode A, hkMathDivByZeroMode D> struct divHR<1,A,D> { HK_FORCE_INLINE static void apply(hkSimdReal* v, const hkSimdReal* v0) { v[0].div<A,D>(v0[0]); } };
}
template <int M>
template <hkMathAccuracyMode A, hkMathDivByZeroMode D>
HK_FORCE_INLINE void hkMxReal<M>::div(hkMxRealParameter r)
{
	hkMxReal_Implementation::divHR<M,A,D>::apply(m_real.r, r.m_real.r);
}

template <int M>
HK_FORCE_INLINE void hkMxReal<M>::div(hkMxRealParameter r)
{
	hkMxReal_Implementation::divHR<M,HK_ACC_23_BIT,HK_DIV_IGNORE>::apply(m_real.r, r.m_real.r);
}



namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void setNegHR(hkSimdReal* v, const hkSimdReal* v0) { setNegHR<I-1>(v, v0); v[I-1] = -v0[I-1]; }
template <> HK_FORCE_INLINE void setNegHR<1>(hkSimdReal* v, const hkSimdReal* v0) { v[0] = -v0[0]; }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::setNeg(hkMxRealParameter r)
{
	hkMxReal_Implementation::setNegHR<M>(m_real.r, r.m_real.r);
}

namespace hkMxReal_Implementation
{
template <int I, hkMathAccuracyMode A, hkMathDivByZeroMode D> struct setReciprocalHR { HK_FORCE_INLINE static void apply(hkSimdReal* v, const hkSimdReal* v0) { setReciprocalHR<I-1,A,D>::apply(v, v0); v[I-1].setReciprocal<A,D>(v0[I-1]); } };
template <hkMathAccuracyMode A, hkMathDivByZeroMode D> struct setReciprocalHR<1,A,D> { HK_FORCE_INLINE static void apply(hkSimdReal* v, const hkSimdReal* v0) { v[0].setReciprocal<A,D>(v0[0]); } };
}
template <int M>
template <hkMathAccuracyMode A, hkMathDivByZeroMode D> 
HK_FORCE_INLINE void hkMxReal<M>::setReciprocal(hkMxRealParameter r)
{
	hkMxReal_Implementation::setReciprocalHR<M,A,D>::apply(m_real.r, r.m_real.r);
}

template <int M>
HK_FORCE_INLINE void hkMxReal<M>::setReciprocal(hkMxRealParameter r)
{
	hkMxReal_Implementation::setReciprocalHR<M,HK_ACC_23_BIT,HK_DIV_IGNORE>::apply(m_real.r, r.m_real.r);
}


namespace hkMxReal_Implementation
{
template <int I, hkMathAccuracyMode A, hkMathNegSqrtMode S> struct sqrtHR { HK_FORCE_INLINE static void apply(hkSimdReal* v) { sqrtHR<I-1,A,S>::apply(v); v[I-1] = v[I-1].sqrt<A,S>(); } };
template <hkMathAccuracyMode A, hkMathNegSqrtMode S> struct sqrtHR<1,A,S> { HK_FORCE_INLINE static void apply(hkSimdReal* v) { v[0] = v[0].sqrt<A,S>(); } };
}
template <int M>
template <hkMathAccuracyMode A, hkMathNegSqrtMode S> 
HK_FORCE_INLINE void hkMxReal<M>::sqrt()
{
	hkMxReal_Implementation::sqrtHR<M,A,S>::apply(m_real.r);
}

template <int M>
HK_FORCE_INLINE void hkMxReal<M>::sqrt()
{
	hkMxReal_Implementation::sqrtHR<M,HK_ACC_23_BIT,HK_SQRT_SET_ZERO>::apply(m_real.r);
}



namespace hkMxReal_Implementation
{
template <int I>
HK_FORCE_INLINE void selectRH(hkSimdReal* r, const hkVector4Comparison* mask, const hkSimdReal* trueValue, const hkSimdReal* falseValue)
{
	selectRH<I-1>(r, mask, trueValue, falseValue);
	r[I-1].setSelect(mask[I-1], trueValue[I-1], falseValue[I-1]);
}

template <>
HK_FORCE_INLINE void selectRH<1>(hkSimdReal* r, const hkVector4Comparison* mask, const hkSimdReal* trueValue, const hkSimdReal* falseValue)
{
	r[0].setSelect(mask[0], trueValue[0], falseValue[0]);
}
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::setSelect(hkMxMaskParameter mask, hkMxRealParameter trueValue, hkMxRealParameter falseValue)
{
	hkMxReal_Implementation::selectRH<M>(m_real.r, mask.m_comp.c, trueValue.m_real.r, falseValue.m_real.r);
}



namespace hkMxReal_Implementation
{
template <int I>
HK_FORCE_INLINE void selectRH(hkSimdReal* r, const hkVector4Comparison* mask, hkSimdRealParameter trueValue, const hkSimdReal* falseValue)
{
	selectRH<I-1>(r, mask, trueValue, falseValue);
	r[I-1].setSelect(mask[I-1], trueValue, falseValue[I-1]);
}

template <>
HK_FORCE_INLINE void selectRH<1>(hkSimdReal* r, const hkVector4Comparison* mask, hkSimdRealParameter trueValue, const hkSimdReal* falseValue)
{
	r[0].setSelect(mask[0], trueValue, falseValue[0]);
}
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::setSelect(hkMxMaskParameter mask, hkMxSingleParameter trueValue, hkMxRealParameter falseValue)
{
	hkMxReal_Implementation::selectRH<M>(m_real.r, mask.m_comp.c, trueValue.m_single.s.template getComponent<0>(), falseValue.m_real.r);
}



namespace hkMxReal_Implementation
{
template <int I>
HK_FORCE_INLINE void selectRH(hkSimdReal* r, const hkVector4Comparison* mask, const hkSimdReal* trueValue, hkSimdRealParameter falseValue)
{
	selectRH<I-1>(r, mask, trueValue, falseValue);
	r[I-1].setSelect(mask[I-1], trueValue[I-1], falseValue);
}

template <>
HK_FORCE_INLINE void selectRH<1>(hkSimdReal* r, const hkVector4Comparison* mask, const hkSimdReal* trueValue, hkSimdRealParameter falseValue)
{
	r[0].setSelect(mask[0], trueValue[0], falseValue);
}
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::setSelect(hkMxMaskParameter mask, hkMxRealParameter trueValue, hkMxSingleParameter falseValue)
{
	hkMxReal_Implementation::selectRH<M>(m_real.r, mask.m_comp.c, trueValue.m_real.r, falseValue.m_single.s.template getComponent<0>());
}



namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void compareLessThanHR(const hkSimdReal* v0, const hkVector4* v1, hkVector4Comparison* m) { hkVector4 v; v.setAll(v0[I-1]); compareLessThanHR<I-1>(v0, v1, m); m[I-1] = v.less(v1[I-1]); }
template <> HK_FORCE_INLINE void compareLessThanHR<1>(const hkSimdReal* v0, const hkVector4* v1, hkVector4Comparison* m) { hkVector4 v; v.setAll(v0[0]); m[0] = v.less(v1[0]); }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::less(hkMxVectorParameter v, hkMxMask<M>& mask) const
{
	hkMxReal_Implementation::compareLessThanHR<M>(m_real.r, v.m_vec.v, mask.m_comp.c);
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void compareLessThanEqualHR(const hkSimdReal* v0, const hkVector4* v1, hkVector4Comparison* m) { hkVector4 v; v.setAll(v0[I-1]); compareLessThanEqualHR<I-1>(v0, v1, m); m[I-1] = v.lessEqual(v1[I-1]); }
template <> HK_FORCE_INLINE void compareLessThanEqualHR<1>(const hkSimdReal* v0, const hkVector4* v1, hkVector4Comparison* m) { hkVector4 v; v.setAll(v0[0]); m[0] = v.lessEqual(v1[0]); }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::lessEqual(hkMxVectorParameter v, hkMxMask<M>& mask) const
{
	hkMxReal_Implementation::compareLessThanEqualHR<M>(m_real.r, v.m_vec.v, mask.m_comp.c);
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void compareGreaterThanHR(const hkSimdReal* v0, const hkVector4* v1, hkVector4Comparison* m) { hkVector4 v; v.setAll(v0[I-1]); compareGreaterThanHR<I-1>(v0, v1, m); m[I-1] = v.greater(v1[I-1]); }
template <> HK_FORCE_INLINE void compareGreaterThanHR<1>(const hkSimdReal* v0, const hkVector4* v1, hkVector4Comparison* m) { hkVector4 v; v.setAll(v0[0]); m[0] = v.greater(v1[0]); }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::greater(hkMxVectorParameter v, hkMxMask<M>& mask) const
{
	hkMxReal_Implementation::compareGreaterThanHR<M>(m_real.r, v.m_vec.v, mask.m_comp.c);
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void compareGreaterThanEqualHR(const hkSimdReal* v0, const hkVector4* v1, hkVector4Comparison* m) { hkVector4 v; v.setAll(v0[I-1]); compareGreaterThanEqualHR<I-1>(v0, v1, m); m[I-1] = v.greaterEqual(v1[I-1]); }
template <> HK_FORCE_INLINE void compareGreaterThanEqualHR<1>(const hkSimdReal* v0, const hkVector4* v1, hkVector4Comparison* m) { hkVector4 v; v.setAll(v0[0]); m[0] = v.greaterEqual(v1[0]); }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::greaterEqual(hkMxVectorParameter v, hkMxMask<M>& mask) const
{
	hkMxReal_Implementation::compareGreaterThanEqualHR<M>(m_real.r, v.m_vec.v, mask.m_comp.c);
}


namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void compareLessThanHRS(const hkSimdReal* v0, hkVector4Parameter v1, hkVector4Comparison* m) { hkVector4 v; v.setAll(v0[I-1]); compareLessThanHRS<I-1>(v0, v1, m); m[I-1] = v.less(v1); }
template <> HK_FORCE_INLINE void compareLessThanHRS<1>(const hkSimdReal* v0, hkVector4Parameter v1, hkVector4Comparison* m) { hkVector4 v; v.setAll(v0[0]); m[0] = v.less(v1); }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::less(hkMxSingleParameter v, hkMxMask<M>& mask) const
{
	hkMxReal_Implementation::compareLessThanHRS<M>(m_real.r, v.m_single.s, mask.m_comp.c);
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void compareLessThanEqualHRS(const hkSimdReal* v0, hkVector4Parameter v1, hkVector4Comparison* m) { hkVector4 v; v.setAll(v0[I-1]); compareLessThanEqualHRS<I-1>(v0, v1, m); m[I-1] = v.lessEqual(v1); }
template <> HK_FORCE_INLINE void compareLessThanEqualHRS<1>(const hkSimdReal* v0, hkVector4Parameter v1, hkVector4Comparison* m) { hkVector4 v; v.setAll(v0[0]); m[0] = v.lessEqual(v1); }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::lessEqual(hkMxSingleParameter v, hkMxMask<M>& mask) const
{
	hkMxReal_Implementation::compareLessThanEqualHRS<M>(m_real.r, v.m_single.s, mask.m_comp.c);
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void compareGreaterThanHRS(const hkSimdReal* v0, hkVector4Parameter v1, hkVector4Comparison* m) { hkVector4 v; v.setAll(v0[I-1]); compareGreaterThanHRS<I-1>(v0, v1, m); m[I-1] = v.greater(v1); }
template <> HK_FORCE_INLINE void compareGreaterThanHRS<1>(const hkSimdReal* v0, hkVector4Parameter v1, hkVector4Comparison* m) { hkVector4 v; v.setAll(v0[0]); m[0] = v.greater(v1); }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::greater(hkMxSingleParameter v, hkMxMask<M>& mask) const
{
	hkMxReal_Implementation::compareGreaterThanHRS<M>(m_real.r, v.m_single.s, mask.m_comp.c);
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void compareGreaterThanEqualHRS(const hkSimdReal* v0, hkVector4Parameter v1, hkVector4Comparison* m) { hkVector4 v; v.setAll(v0[I-1]); compareGreaterThanEqualHRS<I-1>(v0, v1, m); m[I-1] = v.greaterEqual(v1); }
template <> HK_FORCE_INLINE void compareGreaterThanEqualHRS<1>(const hkSimdReal* v0, hkVector4Parameter v1, hkVector4Comparison* m) { hkVector4 v; v.setAll(v0[0]); m[0] = v.greaterEqual(v1); }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::greaterEqual(hkMxSingleParameter v, hkMxMask<M>& mask) const
{
	hkMxReal_Implementation::compareGreaterThanEqualHRS<M>(m_real.r, v.m_single.s, mask.m_comp.c);
}


namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void compareLessThanHR(const hkSimdReal* v0, const hkSimdReal* v1, hkVector4Comparison* m) { compareLessThanHR<I-1>(v0, v1, m); m[I-1] = v0[I-1].less(v1[I-1]); }
template <> HK_FORCE_INLINE void compareLessThanHR<1>(const hkSimdReal* v0, const hkSimdReal* v1, hkVector4Comparison* m) { m[0] = v0[0].less(v1[0]); }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::less(hkMxRealParameter v, hkMxMask<M>& mask) const
{
	hkMxReal_Implementation::compareLessThanHR<M>(m_real.r, v.m_real.r, mask.m_comp.c);
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void compareLessThanEqualHR(const hkSimdReal* v0, const hkSimdReal* v1, hkVector4Comparison* m) { compareLessThanEqualHR<I-1>(v0, v1, m); m[I-1] = v0[I-1].lessEqual(v1[I-1]); }
template <> HK_FORCE_INLINE void compareLessThanEqualHR<1>(const hkSimdReal* v0, const hkSimdReal* v1, hkVector4Comparison* m) { m[0] = v0[0].lessEqual(v1[0]); }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::lessEqual(hkMxRealParameter v, hkMxMask<M>& mask) const
{
	hkMxReal_Implementation::compareLessThanEqualHR<M>(m_real.r, v.m_real.r, mask.m_comp.c);
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void compareGreaterThanHR(const hkSimdReal* v0, const hkSimdReal* v1, hkVector4Comparison* m) { compareGreaterThanHR<I-1>(v0, v1, m); m[I-1] = v0[I-1].greater(v1[I-1]); }
template <> HK_FORCE_INLINE void compareGreaterThanHR<1>(const hkSimdReal* v0, const hkSimdReal* v1, hkVector4Comparison* m) { m[0] = v0[0].greater(v1[0]); }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::greater(hkMxRealParameter v, hkMxMask<M>& mask) const
{
	hkMxReal_Implementation::compareGreaterThanHR<M>(m_real.r, v.m_real.r, mask.m_comp.c);
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void compareGreaterThanEqualHR(const hkSimdReal* v0, const hkSimdReal* v1, hkVector4Comparison* m) { compareGreaterThanEqualHR<I-1>(v0, v1, m); m[I-1] = v0[I-1].greaterEqual(v1[I-1]); }
template <> HK_FORCE_INLINE void compareGreaterThanEqualHR<1>(const hkSimdReal* v0, const hkSimdReal* v1, hkVector4Comparison* m) { m[0] = v0[0].greaterEqual(v1[0]); }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::greaterEqual(hkMxRealParameter v, hkMxMask<M>& mask) const
{
	hkMxReal_Implementation::compareGreaterThanEqualHR<M>(m_real.r, v.m_real.r, mask.m_comp.c);
}


template <int M>
HK_FORCE_INLINE void hkMxReal<M>::storePacked(hkVector4& vOut) const
{
	HK_MXREAL_MX_NOT_IMPLEMENTED;
}

template <>
HK_FORCE_INLINE void hkMxReal<1>::storePacked(hkVector4& vOut) const
{
	vOut.setComponent<0>(m_real.r[0]);
	//HK_ON_DEBUG(HK_M128(vOut.m_quad).m128_u32[1] = 0xffffffff; HK_M128(vOut.m_quad).m128_u32[2] = 0xffffffff; HK_M128(vOut.m_quad).m128_u32[3] = 0xffffffff;)
}

template <>
HK_FORCE_INLINE void hkMxReal<2>::storePacked(hkVector4& vOut) const
{
	vOut.set(m_real.r[0], m_real.r[1], m_real.r[1], m_real.r[1]);
	//HK_ON_DEBUG(HK_M128(vOut.m_quad).m128_u32[2] = 0xffffffff; HK_M128(vOut.m_quad).m128_u32[3] = 0xffffffff;)
}

template <>
HK_FORCE_INLINE void hkMxReal<3>::storePacked(hkVector4& vOut) const
{
	vOut.set(m_real.r[0], m_real.r[1], m_real.r[2], m_real.r[2]);
	//HK_ON_DEBUG(HK_M128(vOut.m_quad).m128_u32[3] = 0xffffffff;)
}

template <>
HK_FORCE_INLINE void hkMxReal<4>::storePacked(hkVector4& vOut) const
{
	vOut.set(m_real.r[0], m_real.r[1], m_real.r[2], m_real.r[3]);
}




namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void setAddHR(hkSimdReal* v, const hkSimdReal* v0, const hkSimdReal* v1) { setAddHR<I-1>(v, v0, v1); v[I-1] = v0[I-1] + v1[I-1]; }
template <> HK_FORCE_INLINE void setAddHR<1>(hkSimdReal* v, const hkSimdReal* v0, const hkSimdReal* v1) { v[0] = v0[0] + v1[0]; }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::setAdd(hkMxRealParameter v0, hkMxRealParameter v1)
{
	hkMxReal_Implementation::setAddHR<M>(m_real.r, v0.m_real.r, v1.m_real.r);
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void setSubHR(hkSimdReal* v, const hkSimdReal* v0, const hkSimdReal* v1) { setSubHR<I-1>(v, v0, v1); v[I-1] = v0[I-1] - v1[I-1]; }
template <> HK_FORCE_INLINE void setSubHR<1>(hkSimdReal* v, const hkSimdReal* v0, const hkSimdReal* v1) { v[0] = v0[0] - v1[0]; }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::setSub(hkMxRealParameter v0, hkMxRealParameter v1)
{
	hkMxReal_Implementation::setSubHR<M>(m_real.r, v0.m_real.r, v1.m_real.r);
}

namespace hkMxReal_Implementation
{
template <int I> HK_FORCE_INLINE void setMulHR(hkSimdReal* v, const hkSimdReal* v0, const hkSimdReal* v1) { setMulHR<I-1>(v, v0, v1); v[I-1] = v0[I-1] * v1[I-1]; }
template <> HK_FORCE_INLINE void setMulHR<1>(hkSimdReal* v, const hkSimdReal* v0, const hkSimdReal* v1) { v[0] = v0[0] * v1[0]; }
}
template <int M>
HK_FORCE_INLINE void hkMxReal<M>::setMul(hkMxRealParameter v0, hkMxRealParameter v1)
{
	hkMxReal_Implementation::setMulHR<M>(m_real.r, v0.m_real.r, v1.m_real.r);
}

namespace hkMxReal_Implementation
{
template <int I, hkMathAccuracyMode A, hkMathDivByZeroMode D> struct setDivHR { HK_FORCE_INLINE static void apply(hkSimdReal* v, const hkSimdReal* v0, const hkSimdReal* v1) { setDivHR<I-1,A,D>::apply(v, v0, v1); v[I-1].setDiv<A,D>(v0[I-1], v1[I-1]); } };
template <hkMathAccuracyMode A, hkMathDivByZeroMode D> struct setDivHR<1,A,D> { HK_FORCE_INLINE static void apply(hkSimdReal* v, const hkSimdReal* v0, const hkSimdReal* v1) { v[0].setDiv<A,D>(v0[0], v1[0]); } };
}
template <int M>
template <hkMathAccuracyMode A, hkMathDivByZeroMode D>
HK_FORCE_INLINE void hkMxReal<M>::setDiv(hkMxRealParameter v0, hkMxRealParameter v1)
{
	hkMxReal_Implementation::setDivHR<M,A,D>::apply(m_real.r, v0.m_real.r, v1.m_real.r);
}

template <int M>
HK_FORCE_INLINE void hkMxReal<M>::setDiv(hkMxRealParameter v0, hkMxRealParameter v1)
{
	hkMxReal_Implementation::setDivHR<M,HK_ACC_23_BIT,HK_DIV_IGNORE>::apply(m_real.r, v0.m_real.r, v1.m_real.r);
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
