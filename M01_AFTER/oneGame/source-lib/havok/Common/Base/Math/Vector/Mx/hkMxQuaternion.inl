/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

namespace hkMxQuaternion_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct gatherQH {
HK_FORCE_INLINE static void apply(hkVector4* v, const hkQuaternion* base)
{
	gatherQH<I-1, byteAddressIncrement>::apply(v, base);
	v[I-1] = hkAddByteOffsetConst( base, (I-1)*byteAddressIncrement )->m_vec;
} };
template <hkUint32 byteAddressIncrement> struct gatherQH<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(hkVector4* v, const hkQuaternion* base)
{
	v[0] = base->m_vec;
} };
}
template <int M>
template <hkUint32 byteAddressIncrement>
HK_FORCE_INLINE void hkMxQuaternion<M>::gather(const hkQuaternion* base)
{
	hkMxQuaternion_Implementation::gatherQH<M, byteAddressIncrement>::apply(this->m_vec.v, base);
}




namespace hkMxQuaternion_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct scatterQH {
HK_FORCE_INLINE static void apply(const hkVector4* v, hkQuaternion* base)
{
	scatterQH<I-1, byteAddressIncrement>::apply(v, base);
	hkAddByteOffset(base, (I-1)*byteAddressIncrement)->m_vec = v[I-1];
} };
template <hkUint32 byteAddressIncrement> struct scatterQH<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkVector4* v, hkQuaternion* base)
{
	base->m_vec = v[0];
} };
}
template <int M>
template <hkUint32 byteAddressIncrement>
HK_FORCE_INLINE void hkMxQuaternion<M>::scatter(hkQuaternion* base) const
{
	hkMxQuaternion_Implementation::scatterQH<M, byteAddressIncrement>::apply(this->m_vec.v, base);
}



namespace hkMxQuaternion_Implementation
{
template <int I>
HK_FORCE_INLINE void setMulQH(hkQuaternion* v, const hkQuaternion* p, const hkQuaternion* q)
{
	setMulQH<I-1>(v, p, q);
	v[I-1].setMul(p[I-1], q[I-1]);
}

template <>
HK_FORCE_INLINE void setMulQH<1>(hkQuaternion* v, const hkQuaternion* p, const hkQuaternion* q)
{
	v[0].setMul(p[0], q[0]);
}
}
template <int M>
HK_FORCE_INLINE void hkMxQuaternion<M>::setMulQ(hkMxQuaternionParameter p, hkMxQuaternionParameter q)
{
	hkMxQuaternion_Implementation::setMulQH<M>( (hkQuaternion*)this->m_vec.v, (const hkQuaternion*)p.m_vec.v, (const hkQuaternion*)q.m_vec.v);
}


namespace hkMxQuaternion_Implementation
{
template <int I>
HK_FORCE_INLINE void setMulInverseQH(hkQuaternion* v, const hkQuaternion* p, const hkQuaternion* q)
{
	setMulInverseQH<I-1>(v, p, q);
	v[I-1].setMulInverse(p[I-1], q[I-1]);
}

template <>
HK_FORCE_INLINE void setMulInverseQH<1>(hkQuaternion* v, const hkQuaternion* p, const hkQuaternion* q)
{
	v[0].setMulInverse(p[0], q[0]);
}
}
template <int M>
HK_FORCE_INLINE void hkMxQuaternion<M>::setMulInverseQ(hkMxQuaternionParameter p, hkMxQuaternionParameter q)
{
	hkMxQuaternion_Implementation::setMulInverseQH<M>( (hkQuaternion*)this->m_vec.v, (const hkQuaternion*)p.m_vec.v, (const hkQuaternion*)q.m_vec.v);
}




namespace hkMxQuaternion_Implementation
{
template <int I>
HK_FORCE_INLINE void setInverseQH(hkQuaternion* v, const hkQuaternion* p)
{
	setInverseQH<I-1>(v, p);
	v[I-1].setInverse(p[I-1]);
}

template <>
HK_FORCE_INLINE void setInverseQH<1>(hkQuaternion* v, const hkQuaternion* p)
{
	v[0].setInverse(p[0]);
}
}
template <int M>
HK_FORCE_INLINE void hkMxQuaternion<M>::setInverse(hkMxQuaternionParameter p)
{
	hkMxQuaternion_Implementation::setInverseQH<M>( (hkQuaternion*)this->m_vec.v, (const hkQuaternion*)p.m_vec.v);
}



template <int M>
template <int I> 
HK_FORCE_INLINE const hkQuaternion& hkMxQuaternion<M>::getQuaternion() const
{
	HK_MXVECTOR_MX_SUBINDEX_CHECK;
	return (const hkQuaternion&)this->m_vec.v[I];
}



namespace hkMxQuaternion_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct scatterQHR {
HK_FORCE_INLINE static void apply(const hkVector4* v, hkRotation* base)
{
	scatterQHR<I-1, byteAddressIncrement>::apply(v, base);
	hkAddByteOffset(base, (I-1)*byteAddressIncrement)->set( (const hkQuaternion&)v[I-1] );
} };
template <hkUint32 byteAddressIncrement> struct scatterQHR<4, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkVector4* v, hkRotation* base)
{
	hkVector4Util::convert4QuaternionsToRotations( (const hkQuaternion*)v,  // out of line call
		hkAddByteOffset(base, 0 * byteAddressIncrement),
		hkAddByteOffset(base, 1 * byteAddressIncrement),
		hkAddByteOffset(base, 2 * byteAddressIncrement),
		hkAddByteOffset(base, 3 * byteAddressIncrement)
		);
} };
template <hkUint32 byteAddressIncrement> struct scatterQHR<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkVector4* v, hkRotation* base)
{
	//hkVector4Util::convertQuaternionToRotation( (const hkQuaternion&)v[0], base );
	base->set( (const hkQuaternion&)v[0] );	// out of line
} };
}
template <int M>
template <hkUint32 byteAddressIncrement>
HK_FORCE_INLINE void hkMxQuaternion<M>::convertAndScatter(hkRotation* HK_RESTRICT base) const
{
	hkMxQuaternion_Implementation::scatterQHR<M, byteAddressIncrement>::apply(this->m_vec.v, base);
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
