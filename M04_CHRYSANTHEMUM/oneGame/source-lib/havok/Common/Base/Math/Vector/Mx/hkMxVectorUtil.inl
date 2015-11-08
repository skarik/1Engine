/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


namespace hkMxVectorUtil_Implementation
{
template <int I> struct loadUnpack2H {
	HK_FORCE_INLINE static void apply(const hkHalf* base, hkVector4* vOut0)
	{
		HK_COMPILE_TIME_ASSERT( (I&1) == 0);
		hkVector4 h0,h1; 
		h0.load<4,HK_IO_NATIVE_ALIGNED>(base+((I-2)*4));
		h1.load<4,HK_IO_NATIVE_ALIGNED>(base+((I-1)*4));
		loadUnpack2H<I-2>::apply(base, vOut0);
		vOut0[I-2] = h0;
		vOut0[I-1] = h1;
	} };

	template <> struct loadUnpack2H<2> {
		HK_FORCE_INLINE static void apply(const hkHalf* base, hkVector4* vOut0)
		{
			hkVector4 h0,h1;
			h0.load<4,HK_IO_NATIVE_ALIGNED>(base);
			h1.load<4,HK_IO_NATIVE_ALIGNED>(base+4);
			vOut0[0] = h0;
			vOut0[1] = h1;
		} };
}

template <int M>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::loadUnpack(const hkHalf* base, hkMxVector<M>& vOut0)
{
	hkMxVectorUtil_Implementation::loadUnpack2H<M>::apply(base, vOut0.m_vec.v);
}





namespace hkMxVectorUtil_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct gatherUnpackH {
HK_FORCE_INLINE static void apply(const hkHalf* base, hkVector4* vOut0, hkVector4* vOut1)
{
	hkVector4 h0,h1;
	const hkHalf* ptr = hkAddByteOffsetConst( base, (I-1) * byteAddressIncrement ); 
	h0.load<4,HK_IO_NATIVE_ALIGNED>(ptr);
	h1.load<4,HK_IO_NATIVE_ALIGNED>(ptr+4);
	gatherUnpackH<I-1, byteAddressIncrement>::apply(base, vOut0, vOut1);
	vOut0[I-1] = h0;
	vOut1[I-1] = h1;
} };
template <hkUint32 byteAddressIncrement> struct gatherUnpackH<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkHalf* base, hkVector4* vOut0, hkVector4* vOut1)
{
	hkVector4 h0,h1;
	h0.load<4,HK_IO_NATIVE_ALIGNED>(base);
	h1.load<4,HK_IO_NATIVE_ALIGNED>(base+4);
	vOut0[0] = h0;
	vOut1[0] = h1;
} };
}
template <int M, hkUint32 byteAddressIncrement>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::gatherUnpack(const hkHalf* base, hkMxVector<M>& vOut0, hkMxVector<M>& vOut1)
{
	hkMxVectorUtil_Implementation::gatherUnpackH<M, byteAddressIncrement>::apply(base, vOut0.m_vec.v, vOut1.m_vec.v);
}


namespace hkMxVectorUtil_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct gatherUnpack2Hu {
HK_FORCE_INLINE static void apply(const hkHalf* base, const hkUint16* indices, hkVector4* vOut0, hkVector4* vOut1)
{
	hkVector4 h0,h1;
	const hkHalf* ptr = hkAddByteOffsetConst( base, indices[I-1] * byteAddressIncrement ); 
	h0.load<4,HK_IO_NATIVE_ALIGNED>(ptr);
	h1.load<4,HK_IO_NATIVE_ALIGNED>(ptr+4);
	gatherUnpack2Hu<I-1, byteAddressIncrement>::apply(base, indices, vOut0, vOut1);
	vOut0[I-1] = h0;
	vOut1[I-1] = h1;
} };
template <hkUint32 byteAddressIncrement> struct gatherUnpack2Hu<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkHalf* base, const hkUint16* indices, hkVector4* vOut0, hkVector4* vOut1)
{
	hkVector4 h0,h1;
	const hkHalf* ptr = hkAddByteOffsetConst( base, indices[0] * byteAddressIncrement ); 
	h0.load<4,HK_IO_NATIVE_ALIGNED>(ptr);
	h1.load<4,HK_IO_NATIVE_ALIGNED>(ptr+4);
	vOut0[0] = h0;
	vOut1[0] = h1;
} };
}
template <int M, hkUint32 byteAddressIncrement>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::gatherUnpack(const hkHalf* base, const hkUint16* indices, hkMxVector<M>& vOut0, hkMxVector<M>& vOut1)
{
	hkMxVectorUtil_Implementation::gatherUnpack2Hu<M, byteAddressIncrement>::apply(base, indices, vOut0.m_vec.v, vOut1.m_vec.v);
}


namespace hkMxVectorUtil_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct gatherUnpack2H {
HK_FORCE_INLINE static void apply(const hkHalf* base, const hkInt32* indices, hkVector4* vOut0, hkVector4* vOut1)
{
	hkVector4 h0,h1;
	const hkHalf* ptr = hkAddByteOffsetConst( base, indices[I-1] * byteAddressIncrement ); 
	h0.load<4,HK_IO_NATIVE_ALIGNED>(ptr);
	h1.load<4,HK_IO_NATIVE_ALIGNED>(ptr+4);
	gatherUnpack2H<I-1, byteAddressIncrement>::apply(base, indices, vOut0, vOut1);
	vOut0[I-1] = h0;
	vOut1[I-1] = h1;
} };
template <hkUint32 byteAddressIncrement> struct gatherUnpack2H<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkHalf* base, const hkInt32* indices, hkVector4* vOut0, hkVector4* vOut1)
{
	hkVector4 h0,h1;
	const hkHalf* ptr = hkAddByteOffsetConst( base, indices[0] * byteAddressIncrement ); 
	h0.load<4,HK_IO_NATIVE_ALIGNED>(ptr);
	h1.load<4,HK_IO_NATIVE_ALIGNED>(ptr+4);
	vOut0[0] = h0;
	vOut1[0] = h1;
} };
}
template <int M, hkUint32 byteAddressIncrement>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::gatherUnpack(const hkHalf* base, const hkInt32* indices, hkMxVector<M>& vOut0, hkMxVector<M>& vOut1)
{
	hkMxVectorUtil_Implementation::gatherUnpack2H<M, byteAddressIncrement>::apply(base, indices, vOut0.m_vec.v, vOut1.m_vec.v);
}





namespace hkMxVectorUtil_Implementation
{
template <int I, hkUint32 byteOffset> struct gatherUnpackWithOffsetH {
HK_FORCE_INLINE static void apply(hkVector4* v0, hkVector4* v1, const void** base)
{
	const hkHalf* ptr = (const hkHalf*)hkAddByteOffsetConst( base[I-1], byteOffset ); 
	gatherUnpackWithOffsetH<I-1, byteOffset>::apply(v0, v1, base);
	v0[I-1].load<4,HK_IO_NATIVE_ALIGNED>(ptr);
	v1[I-1].load<4,HK_IO_NATIVE_ALIGNED>(ptr+4);
} };
template <hkUint32 byteOffset> struct gatherUnpackWithOffsetH<1, byteOffset> {
HK_FORCE_INLINE static void apply(hkVector4* v0, hkVector4* v1, const void** base)
{
	const hkHalf* ptr = (const hkHalf*)hkAddByteOffsetConst( base[0], byteOffset ); 
	v0[0].load<4,HK_IO_NATIVE_ALIGNED>(ptr);
	v1[0].load<4,HK_IO_NATIVE_ALIGNED>(ptr+4);
} };
}
template <int M, hkUint32 byteAddressOffset>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::gatherUnpackHalf8WithOffset(const void* base[M], hkMxVector<M>& vOut0, hkMxVector<M>& vOut1)
{
	hkMxVectorUtil_Implementation::gatherUnpackWithOffsetH<M, byteAddressOffset>::apply(vOut0.m_vec.v, vOut1.m_vec.v, base);
}





namespace hkMxVectorUtil_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct gatherUnpackFirstH {
HK_FORCE_INLINE static void apply(const hkHalf* base, hkVector4* vOut)
{
	const hkHalf* ptr = hkAddByteOffsetConst( base, (I-1) * byteAddressIncrement ); 
	gatherUnpackFirstH<I-1, byteAddressIncrement>::apply(base, vOut);
	vOut[I-1].load<4,HK_IO_NATIVE_ALIGNED>(ptr);
} };
template <hkUint32 byteAddressIncrement> struct gatherUnpackFirstH<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkHalf* base, hkVector4* vOut)
{
	vOut[0].load<4,HK_IO_NATIVE_ALIGNED>(base);
} };
}
template <int M, hkUint32 byteAddressIncrement>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::gatherUnpackFirst(const hkHalf* base, hkMxVector<M>& vOut)
{
	hkMxVectorUtil_Implementation::gatherUnpackFirstH<M, byteAddressIncrement>::apply(base, vOut.m_vec.v);
}


namespace hkMxVectorUtil_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct gatherUnpackFirst2Hu {
HK_FORCE_INLINE static void apply(const hkHalf* base, const hkUint16* indices, hkVector4* vOut)
{
	const hkHalf* ptr = hkAddByteOffsetConst( base, indices[I-1] * byteAddressIncrement ); 
	gatherUnpackFirst2Hu<I-1, byteAddressIncrement>::apply(base, indices, vOut);
	vOut[I-1].load<4,HK_IO_NATIVE_ALIGNED>(ptr);
} };
template <hkUint32 byteAddressIncrement> struct gatherUnpackFirst2Hu<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkHalf* base, const hkUint16* indices, hkVector4* vOut)
{
	const hkHalf* ptr = hkAddByteOffsetConst( base, indices[0] * byteAddressIncrement ); 
	vOut[0].load<4,HK_IO_NATIVE_ALIGNED>(ptr);
} };
}
template <int M, hkUint32 byteAddressIncrement>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::gatherUnpackFirst(const hkHalf* base, const hkUint16* indices, hkMxVector<M>& vOut)
{
	hkMxVectorUtil_Implementation::gatherUnpackFirst2Hu<M, byteAddressIncrement>::apply(base, indices, vOut.m_vec.v);
}


namespace hkMxVectorUtil_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct gatherUnpackFirst2H {
HK_FORCE_INLINE static void apply(const hkHalf* base, const hkInt32* indices, hkVector4* vOut)
{
	const hkHalf* ptr = hkAddByteOffsetConst( base, indices[I-1] * byteAddressIncrement ); 
	gatherUnpackFirst2H<I-1, byteAddressIncrement>::apply(base, indices, vOut);
	vOut[I-1].load<4,HK_IO_NATIVE_ALIGNED>(ptr);
} };

template <hkUint32 byteAddressIncrement> struct gatherUnpackFirst2H<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkHalf* base, const hkInt32* indices, hkVector4* vOut)
{
	const hkHalf* ptr = hkAddByteOffsetConst( base, indices[0] * byteAddressIncrement ); 
	vOut[0].load<4,HK_IO_NATIVE_ALIGNED>(ptr);
} };
}
template <int M, hkUint32 byteAddressIncrement>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::gatherUnpackFirst(const hkHalf* base, const hkInt32* indices, hkMxVector<M>& vOut)
{
	hkMxVectorUtil_Implementation::gatherUnpackFirst2H<M, byteAddressIncrement>::apply(base, indices, vOut.m_vec.v);
}


namespace hkMxVectorUtil_Implementation
{
template <int I, hkUint32 byteOffset> struct gatherUnpackFirstWithOffsetH {
HK_FORCE_INLINE static void apply(hkVector4* v, const void** base)
{
	const hkHalf* ptr = (const hkHalf*)hkAddByteOffsetConst( base[I-1], byteOffset ); 
	gatherUnpackFirstWithOffsetH<I-1, byteOffset>::apply(v, base);
	v[I-1].load<4,HK_IO_NATIVE_ALIGNED>(ptr);
} };
template <hkUint32 byteOffset> struct gatherUnpackFirstWithOffsetH<1, byteOffset> {
HK_FORCE_INLINE static void apply(hkVector4* v, const void** base)
{
	const hkHalf* ptr = (const hkHalf*)hkAddByteOffsetConst( base[0], byteOffset ); 
	v[0].load<4,HK_IO_NATIVE_ALIGNED>(ptr);
} };
}
template <int M, hkUint32 byteAddressOffset>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::gatherUnpackFirstHalf8WithOffset(const void* base[M], hkMxVector<M>& vOut)
{
	hkMxVectorUtil_Implementation::gatherUnpackFirstWithOffsetH<M, byteAddressOffset>::apply(vOut.m_vec.v, base);
}



namespace hkMxVectorUtil_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct gatherUnpackSecondH {
HK_FORCE_INLINE static void apply(const hkHalf* base, hkVector4* vOut)
{
	const hkHalf* ptr = hkAddByteOffsetConst( base, (I-1) * byteAddressIncrement +4); 
	gatherUnpackSecondH<I-1, byteAddressIncrement>::apply(base, vOut);
	vOut[I-1].load<4,HK_IO_NATIVE_ALIGNED>(ptr);
} };
template <hkUint32 byteAddressIncrement> struct gatherUnpackSecondH<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkHalf* base, hkVector4* vOut)
{
	vOut[0].load<4,HK_IO_NATIVE_ALIGNED>(base+4);
} };
}
template <int M, hkUint32 byteAddressIncrement>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::gatherUnpackSecond(const hkHalf* base, hkMxVector<M>& vOut)
{
	hkMxVectorUtil_Implementation::gatherUnpackSecondH<M, byteAddressIncrement>::apply(base, vOut.m_vec.v);
}


namespace hkMxVectorUtil_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct gatherUnpackSecond2H {
HK_FORCE_INLINE static void apply(const hkHalf* base, const hkInt32* indices, hkVector4* vOut)
{
	const hkHalf* ptr = hkAddByteOffsetConst( base, indices[I-1] * byteAddressIncrement +4); 
	gatherUnpackSecond2H<I-1, byteAddressIncrement>::apply(base, indices, vOut);
	vOut[I-1].load<4,HK_IO_NATIVE_ALIGNED>(ptr);
} };
template <hkUint32 byteAddressIncrement> struct gatherUnpackSecond2H<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkHalf* base, const hkInt32* indices, hkVector4* vOut)
{
	const hkHalf* ptr = hkAddByteOffsetConst( base, indices[0] * byteAddressIncrement +4); 
	vOut[0].load<4,HK_IO_NATIVE_ALIGNED>(ptr);
} };
}
template <int M, hkUint32 byteAddressIncrement>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::gatherUnpackSecond(const hkHalf* base, const hkInt32* indices, hkMxVector<M>& vOut)
{
	hkMxVectorUtil_Implementation::gatherUnpackSecond2H<M, byteAddressIncrement>::apply(base, indices, vOut.m_vec.v);
}


namespace hkMxVectorUtil_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct gatherUnpackSecond2Hu {
HK_FORCE_INLINE static void apply(const hkHalf* base, const hkUint16* indices, hkVector4* vOut)
{
	const hkHalf* ptr = hkAddByteOffsetConst( base, indices[I-1] * byteAddressIncrement +4); 
	gatherUnpackSecond2Hu<I-1, byteAddressIncrement>::apply(base, indices, vOut);
	vOut[I-1].load<4,HK_IO_NATIVE_ALIGNED>(ptr);
} };
template <hkUint32 byteAddressIncrement> struct gatherUnpackSecond2Hu<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkHalf* base, const hkUint16* indices, hkVector4* vOut)
{
	const hkHalf* ptr = hkAddByteOffsetConst( base, indices[0] * byteAddressIncrement +4); 
	vOut[0].load<4,HK_IO_NATIVE_ALIGNED>(ptr);
} };
}
template <int M, hkUint32 byteAddressIncrement>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::gatherUnpackSecond(const hkHalf* base, const hkUint16* indices, hkMxVector<M>& vOut)
{
	hkMxVectorUtil_Implementation::gatherUnpackSecond2Hu<M, byteAddressIncrement>::apply(base, indices, vOut.m_vec.v);
}


namespace hkMxVectorUtil_Implementation
{
template <int I, hkUint32 byteOffset> struct gatherUnpackSecondWithOffsetH {
HK_FORCE_INLINE static void apply(hkVector4* v, const void** base)
{
	const hkHalf* ptr = (const hkHalf*)hkAddByteOffsetConst( base[I-1], byteOffset+4 ); 
	gatherUnpackSecondWithOffsetH<I-1, byteOffset>::apply(v, base);
	v[I-1].load<4,HK_IO_NATIVE_ALIGNED>(ptr);
} };
template <hkUint32 byteOffset> struct gatherUnpackSecondWithOffsetH<1, byteOffset> {
HK_FORCE_INLINE static void apply(hkVector4* v, const void** base)
{
	const hkHalf* ptr = (const hkHalf*)hkAddByteOffsetConst( base[0], byteOffset+4 ); 
	v[0].load<4,HK_IO_NATIVE_ALIGNED>(ptr);
} };
}
template <int M, hkUint32 byteAddressOffset>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::gatherUnpackSecondHalf8WithOffset(const void* base[M], hkMxVector<M>& vOut)
{
	hkMxVectorUtil_Implementation::gatherUnpackSecondWithOffsetH<M, byteAddressOffset>::apply(vOut.m_vec.v, base);
}







namespace hkMxVectorUtil_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct scatterPackH {
HK_FORCE_INLINE static void apply(const hkVector4* v0, const hkVector4* v1, hkHalf* HK_RESTRICT base)
{
	hkHalf* base0 = hkAddByteOffset( base, (I-1) * byteAddressIncrement ); 
	scatterPackH<I-1, byteAddressIncrement>::apply(v0, v1, base);
	v0[I-1].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base0);
	v1[I-1].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base0+4);
} };
template <hkUint32 byteAddressIncrement> struct scatterPackH<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkVector4* v0, const hkVector4* v1, hkHalf* HK_RESTRICT base)
{
	v0[0].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base);
	v1[0].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base+4);
} };
}
template <int M, hkUint32 byteAddressIncrement>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::scatterPack(hkMxVectorParameter v0, hkMxVectorParameter v1, hkHalf* HK_RESTRICT base)
{
	hkMxVectorUtil_Implementation::scatterPackH<M, byteAddressIncrement>::apply(v0.m_vec.v, v1.m_vec.v, base);
}


namespace hkMxVectorUtil_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct scatterPackHu {
HK_FORCE_INLINE static void apply(const hkVector4* v0, const hkVector4* v1, hkHalf* HK_RESTRICT base, const hkUint16* indices)
{
	hkHalf* base0 = hkAddByteOffset( base, indices[I-1] * byteAddressIncrement ); 
	scatterPackHu<I-1, byteAddressIncrement>::apply(v0, v1, base, indices);
	v0[I-1].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base0);
	v1[I-1].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base0+4);
} };
template <hkUint32 byteAddressIncrement> struct scatterPackHu<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkVector4* v0, const hkVector4* v1, hkHalf* HK_RESTRICT base, const hkUint16* indices)
{
	hkHalf* base0 = hkAddByteOffset( base, indices[0] * byteAddressIncrement ); 
	v0[0].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base0);
	v1[0].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base0+4);
} };
}
template <int M, hkUint32 byteAddressIncrement> 
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::scatterPack(hkMxVectorParameter v0, hkMxVectorParameter v1, hkHalf* base, const hkUint16* indices)
{
	hkMxVectorUtil_Implementation::scatterPackHu<M, byteAddressIncrement>::apply(v0.m_vec.v, v1.m_vec.v, base, indices);
}

namespace hkMxVectorUtil_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct scatterPackHui {
HK_FORCE_INLINE static void apply(const hkVector4* v0, const hkVector4* v1, hkHalf* HK_RESTRICT base, const hkInt32* indices)
{
	hkHalf* base0 = hkAddByteOffset( base, indices[I-1] * byteAddressIncrement ); 
	scatterPackHui<I-1, byteAddressIncrement>::apply(v0, v1, base, indices);
	v0[I-1].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base0);
	v1[I-1].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base0+4);
} };
template <hkUint32 byteAddressIncrement> struct scatterPackHui<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkVector4* v0, const hkVector4* v1, hkHalf* HK_RESTRICT base, const hkInt32* indices)
{
	hkHalf* base0 = hkAddByteOffset( base, indices[0] * byteAddressIncrement ); 
	v0[0].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base0);
	v1[0].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base0+4);
} };
}
template <int M, hkUint32 byteAddressIncrement> 
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::scatterPack(hkMxVectorParameter v0, hkMxVectorParameter v1, hkHalf* base, const hkInt32* indices)
{
	hkMxVectorUtil_Implementation::scatterPackHui<M, byteAddressIncrement>::apply(v0.m_vec.v, v1.m_vec.v, base, indices);
}



namespace hkMxVectorUtil_Implementation
{
template <int I, hkUint32 byteOffset> struct scatterPackWithOffsetH {
HK_FORCE_INLINE static void apply(const hkVector4* v0, const hkVector4* v1, void** base)
{
	hkHalf* base0 = (hkHalf*)hkAddByteOffset( base[I-1], byteOffset ); 
	scatterPackWithOffsetH<I-1, byteOffset>::apply(v0, v1, base);
	v0[I-1].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base0);
	v1[I-1].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base0+4);
} };
template <hkUint32 byteOffset> struct scatterPackWithOffsetH<1, byteOffset> {
HK_FORCE_INLINE static void apply(const hkVector4* v0, const hkVector4* v1, void** base)
{
	hkHalf* base0 = (hkHalf*)hkAddByteOffset( base[0], byteOffset ); 
	v0[0].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base0);
	v1[0].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base0+4);
} };
}
template <int M, hkUint32 byteOffset>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::scatterPackHalf8WithOffset(hkMxVectorParameter v0, hkMxVectorParameter v1, void* base[M])
{
	hkMxVectorUtil_Implementation::scatterPackWithOffsetH<M, byteOffset>::apply(v0.m_vec.v, v1.m_vec.v, base);
}




//
//
//
namespace hkMxVectorUtil_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct scatterPackFirstH {
HK_FORCE_INLINE static void apply(const hkVector4* v, hkHalf* HK_RESTRICT base)
{
	hkHalf* base0 = hkAddByteOffset( base, (I-1) * byteAddressIncrement ); 
	scatterPackFirstH<I-1, byteAddressIncrement>::apply(v, base);
	v[I-1].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base0);
} };
template <hkUint32 byteAddressIncrement> struct scatterPackFirstH<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkVector4* v, hkHalf* HK_RESTRICT base)
{
	v[0].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base);
} };
}
template <int M, hkUint32 byteAddressIncrement>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::scatterPackFirst(hkMxVectorParameter v, hkHalf* HK_RESTRICT base)
{
	hkMxVectorUtil_Implementation::scatterPackFirstH<M, byteAddressIncrement>::apply(v.m_vec.v, base);
}



//
//
//
namespace hkMxVectorUtil_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct scatterPackOneH {
	HK_FORCE_INLINE static void apply(const hkVector4* v, hkHalf* HK_RESTRICT base)
	{
		hkHalf* base0 = hkAddByteOffset( base, (I-1) * byteAddressIncrement ); 
		scatterPackOneH<I-1, byteAddressIncrement>::apply(v, base);
		v[I-1].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base0);
	} };
template <hkUint32 byteAddressIncrement> struct scatterPackOneH<1, byteAddressIncrement> {
	HK_FORCE_INLINE static void apply(const hkVector4* v, hkHalf* HK_RESTRICT base)
	{
		v[0].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base);
	} };
}
template <int M, hkUint32 byteAddressIncrement>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::scatterPack(hkMxVectorParameter v, hkHalf* HK_RESTRICT base)
{
	hkMxVectorUtil_Implementation::scatterPackOneH<M, byteAddressIncrement>::apply(v.m_vec.v, base);
}




//
//
//

namespace hkMxVectorUtil_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct scatterPackFirstHu {
HK_FORCE_INLINE static void apply(const hkVector4* v, hkHalf* HK_RESTRICT base, const hkUint16* indices)
{
	hkHalf* base0 = hkAddByteOffset( base, indices[I-1] * byteAddressIncrement ); 
	scatterPackFirstHu<I-1, byteAddressIncrement>::apply(v, base, indices);
	v[I-1].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base0);
} };
template <hkUint32 byteAddressIncrement> struct scatterPackFirstHu<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkVector4* v, hkHalf* HK_RESTRICT base, const hkUint16* indices)
{
	hkHalf* base0 = hkAddByteOffset( base, indices[0] * byteAddressIncrement ); 
	v[0].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base0);
} };
}
template <int M, hkUint32 byteAddressIncrement> 
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::scatterPackFirst(hkMxVectorParameter v, hkHalf* base, const hkUint16* indices)
{
	hkMxVectorUtil_Implementation::scatterPackFirstHu<M, byteAddressIncrement>::apply(v.m_vec.v, base, indices);
}

namespace hkMxVectorUtil_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct scatterPackFirstHui {
HK_FORCE_INLINE static void apply(const hkVector4* v, hkHalf* HK_RESTRICT base, const hkInt32* indices)
{
	hkHalf* base0 = hkAddByteOffset( base, indices[I-1] * byteAddressIncrement ); 
	scatterPackFirstHui<I-1, byteAddressIncrement>::apply(v, base, indices);
	v[I-1].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base0);
} };
template <hkUint32 byteAddressIncrement> struct scatterPackFirstHui<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkVector4* v, hkHalf* HK_RESTRICT base, const hkInt32* indices)
{
	hkHalf* base0 = hkAddByteOffset( base, indices[0] * byteAddressIncrement ); 
	v[0].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base0);
} };
}
template <int M, hkUint32 byteAddressIncrement> 
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::scatterPackFirst(hkMxVectorParameter v, hkHalf* base, const hkInt32* indices)
{
	hkMxVectorUtil_Implementation::scatterPackFirstHui<M, byteAddressIncrement>::apply(v.m_vec.v, base, indices);
}



namespace hkMxVectorUtil_Implementation
{
template <int I, hkUint32 byteOffset> struct scatterPackFirstWithOffsetH {
HK_FORCE_INLINE static void apply(const hkVector4* v, void** base)
{
	hkHalf* base0 = (hkHalf*)hkAddByteOffset( base[I-1], byteOffset );
	scatterPackFirstWithOffsetH<I-1, byteOffset>::apply(v, base);
	v[I-1].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base0);
} };
template <hkUint32 byteOffset> struct scatterPackFirstWithOffsetH<1, byteOffset> {
HK_FORCE_INLINE static void apply(const hkVector4* v, void** base)
{
	hkHalf* base0 = (hkHalf*)hkAddByteOffset( base[0], byteOffset ); 
	v[0].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base0);
} };
}
template <int M, hkUint32 byteOffset>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::scatterPackFirstHalf8WithOffset(hkMxVectorParameter v, void* base[M])
{
	hkMxVectorUtil_Implementation::scatterPackFirstWithOffsetH<M, byteOffset>::apply(v.m_vec.v, base);
}


namespace hkMxVectorUtil_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct scatterPackSecondH {
HK_FORCE_INLINE static void apply(const hkVector4* v, hkHalf* HK_RESTRICT base)
{
	hkHalf* base0 = hkAddByteOffset( base, (I-1) * byteAddressIncrement +4 ); 
	scatterPackSecondH<I-1, byteAddressIncrement>::apply(v, base);
	v[I-1].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base0);
} };
template <hkUint32 byteAddressIncrement> struct scatterPackSecondH<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkVector4* v, hkHalf* HK_RESTRICT base)
{
	v[0].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base+4);
} };
}
template <int M, hkUint32 byteAddressIncrement>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::scatterPackSecond(hkMxVectorParameter v, hkHalf* HK_RESTRICT base)
{
	hkMxVectorUtil_Implementation::scatterPackSecondH<M, byteAddressIncrement>::apply(v.m_vec.v, base);
}


namespace hkMxVectorUtil_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct scatterPackSecondHu {
HK_FORCE_INLINE static void apply(const hkVector4* v, hkHalf* HK_RESTRICT base, const hkUint16* indices)
{
	hkHalf* base0 = hkAddByteOffset( base, indices[I-1] * byteAddressIncrement +4); 
	scatterPackSecondHu<I-1, byteAddressIncrement>::apply(v, base, indices);
	v[I-1].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base0);
} };
template <hkUint32 byteAddressIncrement> struct scatterPackSecondHu<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkVector4* v, hkHalf* HK_RESTRICT base, const hkUint16* indices)
{
	hkHalf* base0 = hkAddByteOffset( base, indices[0] * byteAddressIncrement +4); 
	v[0].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base0);
} };
}
template <int M, hkUint32 byteAddressIncrement> 
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::scatterPackSecond(hkMxVectorParameter v, hkHalf* base, const hkUint16* indices)
{
	hkMxVectorUtil_Implementation::scatterPackSecondHu<M, byteAddressIncrement>::apply(v.m_vec.v, base, indices);
}

namespace hkMxVectorUtil_Implementation
{
template <int I, hkUint32 byteAddressIncrement> struct scatterPackSecondHui {
HK_FORCE_INLINE static void apply(const hkVector4* v, hkHalf* HK_RESTRICT base, const hkInt32* indices)
{
	hkHalf* base0 = hkAddByteOffset( base, indices[I-1] * byteAddressIncrement +4); 
	scatterPackSecondHui<I-1, byteAddressIncrement>::apply(v, base, indices);
	v[I-1].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base0);
} };
template <hkUint32 byteAddressIncrement> struct scatterPackSecondHui<1, byteAddressIncrement> {
HK_FORCE_INLINE static void apply(const hkVector4* v, hkHalf* HK_RESTRICT base, const hkInt32* indices)
{
	hkHalf* base0 = hkAddByteOffset( base, indices[0] * byteAddressIncrement +4); 
	v[0].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base0);
} };
}
template <int M, hkUint32 byteAddressIncrement> 
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::scatterPackSecond(hkMxVectorParameter v, hkHalf* base, const hkInt32* indices)
{
	hkMxVectorUtil_Implementation::scatterPackSecondHui<M, byteAddressIncrement>::apply(v.m_vec.v, base, indices);
}



namespace hkMxVectorUtil_Implementation
{
template <int I, hkUint32 byteOffset> struct scatterPackSecondWithOffsetH {
HK_FORCE_INLINE static void apply(const hkVector4* v, void** base)
{
	hkHalf* base0 = (hkHalf*)hkAddByteOffset( base[I-1], byteOffset ); 
	scatterPackSecondWithOffsetH<I-1, byteOffset>::apply(v, base);
	v[I-1].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base0);
} };
template <hkUint32 byteOffset> struct scatterPackSecondWithOffsetH<1, byteOffset> {
HK_FORCE_INLINE static void apply(const hkVector4* v, void** base)
{
	hkHalf* base0 = (hkHalf*)hkAddByteOffset( base[0], byteOffset ); 
	v[0].store<4,HK_IO_NATIVE_ALIGNED,HK_ROUND_TRUNCATE>(base0);
} };
}
template <int M, hkUint32 byteOffset>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::scatterPackSecondHalf8WithOffset(hkMxVectorParameter v, void* base[M])
{
	hkMxVectorUtil_Implementation::scatterPackSecondWithOffsetH<M, byteOffset>::apply(v.m_vec.v, base);
}










namespace hkMxVectorUtil_Implementation
{
	template <int I, hkUint32 byteOffset> struct gatherUnpackPackedVectorWithOffsetH {
		HK_FORCE_INLINE static void apply(hkVector4* v, const void** base)
		{
			const hkPackedVector3* HK_RESTRICT ptr0 = (const hkPackedVector3*)hkAddByteOffsetConst(base[I-1], byteOffset); 
			gatherUnpackPackedVectorWithOffsetH<I-1, byteOffset>::apply(v, base);
			ptr0->unpack(v[I-1]);
} };
template <hkUint32 byteOffset> struct gatherUnpackPackedVectorWithOffsetH<1, byteOffset> {
HK_FORCE_INLINE static void apply(hkVector4* v, const void** base)
{
	const hkPackedVector3* HK_RESTRICT ptr = (const hkPackedVector3*)hkAddByteOffsetConst(base[0], byteOffset); 
	ptr->unpack(v[0]);
} };
		}
template <int M, hkUint32 byteAddressOffset>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::gatherUnpackPackedVectorWithOffset(const void* base[M], hkMxVector<M>& vOut)
{
	hkMxVectorUtil_Implementation::gatherUnpackPackedVectorWithOffsetH<M, byteAddressOffset>::apply(vOut.m_vec.v, base);
}


namespace hkMxVectorUtil_Implementation
{
template <int I, hkUint32 byteOffset> struct gatherUnpackPackedUnitVectorWithOffsetH {
HK_FORCE_INLINE static void apply(hkVector4* v, const void** base)
{
	const hkPackedUnitVector<4>* HK_RESTRICT ptr0 = (const hkPackedUnitVector<4>*)hkAddByteOffsetConst(base[I-1], byteOffset); 
	gatherUnpackPackedUnitVectorWithOffsetH<I-1, byteOffset>::apply(v, base);
	ptr0->unpack(&v[I-1]);
} };
template <hkUint32 byteOffset> struct gatherUnpackPackedUnitVectorWithOffsetH<1, byteOffset> {
HK_FORCE_INLINE static void apply(hkVector4* v, const void** base)
{
	const hkPackedUnitVector<4>* HK_RESTRICT ptr = (const hkPackedUnitVector<4>*)hkAddByteOffsetConst(base[0], byteOffset); 
	ptr->unpack(&v[0]);
} };
}
template <int M, hkUint32 byteAddressOffset>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::gatherUnpackPackedUnitVectorWithOffset(const void* base[M], hkMxVector<M>& vOut)
{
	hkMxVectorUtil_Implementation::gatherUnpackPackedUnitVectorWithOffsetH<M, byteAddressOffset>::apply(vOut.m_vec.v, base);
}






















namespace hkMxVectorUtil_Implementation
{

HK_FORCE_INLINE static void transformPositionH(hkMxTransformParameter mat, const hkMxVector<4>& vIn, hkMxVector<4>& vOut)
{
	// the vectors in mat are the columns of a transform matrix
	hkVector4 m0,m1,m2,m3;
	{
		mat.getVector<0>(m0);
		mat.getVector<1>(m1);
		mat.getVector<2>(m2);
		mat.getVector<3>(m3);
	}

	hkVector4 in0,in1,in2,in3;
	{
		vIn.getVector<0>(in0);
		vIn.getVector<1>(in1);
		vIn.getVector<2>(in2);
		vIn.getVector<3>(in3);
	}

	hkVector4 xb,yb,zb;
	hkVector4 out;
	{
		xb.setBroadcast<0>(in0);
		yb.setBroadcast<1>(in0);
		zb.setBroadcast<2>(in0);

		out.setAddMul( m3, xb, m0 );
		out.addMul( yb, m1 );
		out.addMul( zb, m2 );
		vOut.setVector<0>(out);
	}
	{
		xb.setBroadcast<0>(in1);
		yb.setBroadcast<1>(in1);
		zb.setBroadcast<2>(in1);

		out.setAddMul( m3, xb, m0 );
		out.addMul( yb, m1 );
		out.addMul( zb, m2 );
		vOut.setVector<1>(out);
	}
	{
		xb.setBroadcast<0>(in2);
		yb.setBroadcast<1>(in2);
		zb.setBroadcast<2>(in2);

		out.setAddMul( m3, xb, m0 );
		out.addMul( yb, m1 );
		out.addMul( zb, m2 );
		vOut.setVector<2>(out);
	}
	{
		xb.setBroadcast<0>(in3);
		yb.setBroadcast<1>(in3);
		zb.setBroadcast<2>(in3);

		out.setAddMul( m3, xb, m0 );
		out.addMul( yb, m1 );
		out.addMul( zb, m2 );
		vOut.setVector<3>(out);
	}
}

HK_FORCE_INLINE static void transformPositionH(hkMxTransformParameter mat, const hkMxVector<3>& vIn, hkMxVector<3>& vOut)
{
	// the vectors in mat are the columns of a transform matrix
	hkVector4 m0,m1,m2,m3;
	{
		mat.getVector<0>(m0);
		mat.getVector<1>(m1);
		mat.getVector<2>(m2);
		mat.getVector<3>(m3);
	}

	hkVector4 in0,in1,in2;
	{
		vIn.getVector<0>(in0);
		vIn.getVector<1>(in1);
		vIn.getVector<2>(in2);
	}

	hkVector4 xb,yb,zb;
	hkVector4 out;
	{
		xb.setBroadcast<0>(in0);
		yb.setBroadcast<1>(in0);
		zb.setBroadcast<2>(in0);

		out.setAddMul( m3, xb, m0 );
		out.addMul( yb, m1 );
		out.addMul( zb, m2 );
		vOut.setVector<0>(out);
	}
	{
		xb.setBroadcast<0>(in1);
		yb.setBroadcast<1>(in1);
		zb.setBroadcast<2>(in1);

		out.setAddMul( m3, xb, m0 );
		out.addMul( yb, m1 );
		out.addMul( zb, m2 );
		vOut.setVector<1>(out);
	}
	{
		xb.setBroadcast<0>(in2);
		yb.setBroadcast<1>(in2);
		zb.setBroadcast<2>(in2);

		out.setAddMul( m3, xb, m0 );
		out.addMul( yb, m1 );
		out.addMul( zb, m2 );
		vOut.setVector<2>(out);
	}
}

HK_FORCE_INLINE static void transformPositionH(hkMxTransformParameter mat, const hkMxVector<2>& vIn, hkMxVector<2>& vOut)
{
	// the vectors in mat are the columns of a transform matrix
	hkVector4 m0,m1,m2,m3;
	{
		mat.getVector<0>(m0);
		mat.getVector<1>(m1);
		mat.getVector<2>(m2);
		mat.getVector<3>(m3);
	}

	hkVector4 in0,in1;
	{
		vIn.getVector<0>(in0);
		vIn.getVector<1>(in1);
	}

	hkVector4 xb,yb,zb;
	hkVector4 out;
	{
		xb.setBroadcast<0>(in0);
		yb.setBroadcast<1>(in0);
		zb.setBroadcast<2>(in0);

		out.setAddMul( m3, xb, m0 );
		out.addMul( yb, m1 );
		out.addMul( zb, m2 );
		vOut.setVector<0>(out);
	}
	{
		xb.setBroadcast<0>(in1);
		yb.setBroadcast<1>(in1);
		zb.setBroadcast<2>(in1);

		out.setAddMul( m3, xb, m0 );
		out.addMul( yb, m1 );
		out.addMul( zb, m2 );
		vOut.setVector<1>(out);
	}
}

HK_FORCE_INLINE static void transformPositionH(hkMxTransformParameter mat, const hkMxVector<1>& vIn, hkMxVector<1>& vOut)
{
	vOut.m_vec.v[0]._setTransformedPos((const hkTransform&)mat.m_vec.v[0], vIn.m_vec.v[0]);
}
}
template <int M>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::transformPosition(hkMxTransformParameter mat, hkMxVectorParameter vIn, hkMxVector<M>& vOut)
{
	hkMxVectorUtil_Implementation::transformPositionH(mat, vIn, vOut);
}



namespace hkMxVectorUtil_Implementation
{

HK_FORCE_INLINE static void transformTransposePositionH(hkMxTransformParameter mat, const hkMxVector<4>& vIn, hkMxVector<4>& vOut)
{
	// the vectors in mat are the columns of a transform matrix
	hkVector4 m0,m1,m2,m3;
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	mat.getVector<0>(m0);
	mat.getVector<1>(m1);
	mat.getVector<2>(m2);
	mat.getVector<3>(m3);
	HK_TRANSPOSE4( m0, m1, m2, m3 );
#else
	m0.set( mat.m_vec.v[0](0), mat.m_vec.v[1](0), mat.m_vec.v[2](0), mat.m_vec.v[3](0) );
	m1.set( mat.m_vec.v[0](1), mat.m_vec.v[1](1), mat.m_vec.v[2](1), mat.m_vec.v[3](1) );
	m2.set( mat.m_vec.v[0](2), mat.m_vec.v[1](2), mat.m_vec.v[2](2), mat.m_vec.v[3](2) );
	m3.set( mat.m_vec.v[0](3), mat.m_vec.v[1](3), mat.m_vec.v[2](3), mat.m_vec.v[3](3) );
#endif

	hkVector4 in0,in1,in2,in3;
	{
		vIn.getVector<0>(in0);
		vIn.getVector<1>(in1);
		vIn.getVector<2>(in2);
		vIn.getVector<3>(in3);
	}

	hkVector4 xb,yb,zb;
	hkVector4 out;
	{
		xb.setBroadcast<0>(in0);
		yb.setBroadcast<1>(in0);
		zb.setBroadcast<2>(in0);

		out.setAddMul( m3, xb, m0 );
		out.addMul( yb, m1 );
		out.addMul( zb, m2 );
		vOut.setVector<0>(out);
	}
	{
		xb.setBroadcast<0>(in1);
		yb.setBroadcast<1>(in1);
		zb.setBroadcast<2>(in1);

		out.setAddMul( m3, xb, m0 );
		out.addMul( yb, m1 );
		out.addMul( zb, m2 );
		vOut.setVector<1>(out);
	}
	{
		xb.setBroadcast<0>(in2);
		yb.setBroadcast<1>(in2);
		zb.setBroadcast<2>(in2);

		out.setAddMul( m3, xb, m0 );
		out.addMul( yb, m1 );
		out.addMul( zb, m2 );
		vOut.setVector<2>(out);
	}
	{
		xb.setBroadcast<0>(in3);
		yb.setBroadcast<1>(in3);
		zb.setBroadcast<2>(in3);

		out.setAddMul( m3, xb, m0 );
		out.addMul( yb, m1 );
		out.addMul( zb, m2 );
		vOut.setVector<3>(out);
	}
}

HK_FORCE_INLINE static void transformTransposePositionH(hkMxTransformParameter mat, const hkMxVector<3>& vIn, hkMxVector<3>& vOut)
{
	// the vectors in mat are the columns of a transform matrix
	hkVector4 m0,m1,m2,m3;
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	mat.getVector<0>(m0);
	mat.getVector<1>(m1);
	mat.getVector<2>(m2);
	mat.getVector<3>(m3);
	HK_TRANSPOSE4( m0, m1, m2, m3 );
#else
	m0.set( mat.m_vec.v[0](0), mat.m_vec.v[1](0), mat.m_vec.v[2](0), mat.m_vec.v[3](0) );
	m1.set( mat.m_vec.v[0](1), mat.m_vec.v[1](1), mat.m_vec.v[2](1), mat.m_vec.v[3](1) );
	m2.set( mat.m_vec.v[0](2), mat.m_vec.v[1](2), mat.m_vec.v[2](2), mat.m_vec.v[3](2) );
	m3.set( mat.m_vec.v[0](3), mat.m_vec.v[1](3), mat.m_vec.v[2](3), mat.m_vec.v[3](3) );
#endif

	hkVector4 in0,in1,in2;
	{
		vIn.getVector<0>(in0);
		vIn.getVector<1>(in1);
		vIn.getVector<2>(in2);
	}

	hkVector4 xb,yb,zb;
	hkVector4 out;
	{
		xb.setBroadcast<0>(in0);
		yb.setBroadcast<1>(in0);
		zb.setBroadcast<2>(in0);

		out.setAddMul( m3, xb, m0 );
		out.addMul( yb, m1 );
		out.addMul( zb, m2 );
		vOut.setVector<0>(out);
	}
	{
		xb.setBroadcast<0>(in1);
		yb.setBroadcast<1>(in1);
		zb.setBroadcast<2>(in1);

		out.setAddMul( m3, xb, m0 );
		out.addMul( yb, m1 );
		out.addMul( zb, m2 );
		vOut.setVector<1>(out);
	}
	{
		xb.setBroadcast<0>(in2);
		yb.setBroadcast<1>(in2);
		zb.setBroadcast<2>(in2);

		out.setAddMul( m3, xb, m0 );
		out.addMul( yb, m1 );
		out.addMul( zb, m2 );
		vOut.setVector<2>(out);
	}
}

HK_FORCE_INLINE static void transformTransposePositionH(hkMxTransformParameter mat, const hkMxVector<2>& vIn, hkMxVector<2>& vOut)
{
	// the vectors in mat are the columns of a transform matrix
	hkVector4 m0,m1,m2,m3;
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	mat.getVector<0>(m0);
	mat.getVector<1>(m1);
	mat.getVector<2>(m2);
	mat.getVector<3>(m3);
	HK_TRANSPOSE4( m0, m1, m2, m3 );
#else
	m0.set( mat.m_vec.v[0](0), mat.m_vec.v[1](0), mat.m_vec.v[2](0), mat.m_vec.v[3](0) );
	m1.set( mat.m_vec.v[0](1), mat.m_vec.v[1](1), mat.m_vec.v[2](1), mat.m_vec.v[3](1) );
	m2.set( mat.m_vec.v[0](2), mat.m_vec.v[1](2), mat.m_vec.v[2](2), mat.m_vec.v[3](2) );
	m3.set( mat.m_vec.v[0](3), mat.m_vec.v[1](3), mat.m_vec.v[2](3), mat.m_vec.v[3](3) );
#endif

	hkVector4 in0,in1;
	{
		vIn.getVector<0>(in0);
		vIn.getVector<1>(in1);
	}

	hkVector4 xb,yb,zb;
	hkVector4 out;
	{
		xb.setBroadcast<0>(in0);
		yb.setBroadcast<1>(in0);
		zb.setBroadcast<2>(in0);

		out.setAddMul( m3, xb, m0 );
		out.addMul( yb, m1 );
		out.addMul( zb, m2 );
		vOut.setVector<0>(out);
	}
	{
		xb.setBroadcast<0>(in1);
		yb.setBroadcast<1>(in1);
		zb.setBroadcast<2>(in1);

		out.setAddMul( m3, xb, m0 );
		out.addMul( yb, m1 );
		out.addMul( zb, m2 );
		vOut.setVector<1>(out);
	}
}

HK_FORCE_INLINE static void transformTransposePositionH(hkMxTransformParameter mat, const hkMxVector<1>& vIn, hkMxVector<1>& vOut)
{
	vOut.m_vec.v[0]._setTransformedInversePos((const hkTransform&)mat.m_vec.v[0], vIn.m_vec.v[0]);
}
}

template <int M>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::transformTransposePosition(hkMxTransformParameter mat, hkMxVectorParameter vIn, hkMxVector<M>& vOut)
{
	hkMxVectorUtil_Implementation::transformTransposePositionH(mat, vIn, vOut);
}





namespace hkMxVectorUtil_Implementation
{


	HK_FORCE_INLINE static void rotateDirectionH( hkMxTransformParameter mat, const hkMxVector<4>& vIn, hkMxVector<4>& vOut)
	{
		// the vectors in mat are the columns of a transform matrix
		hkVector4 m0,m1,m2; 
		{
			mat.getVector<0>(m0);
			mat.getVector<1>(m1);
			mat.getVector<2>(m2);
		}

		hkVector4 in0,in1,in2,in3;
		{
			vIn.getVector<0>(in0);
			vIn.getVector<1>(in1);
			vIn.getVector<2>(in2);
			vIn.getVector<3>(in3);
		}

		hkVector4 xb,yb,zb;
		hkVector4 out;
		{
			xb.setBroadcast<0>(in0);
			yb.setBroadcast<1>(in0);
			zb.setBroadcast<2>(in0);

			out.setMul( xb, m0 );
			out.addMul( yb, m1 );
			out.addMul( zb, m2 );
			vOut.setVector<0>(out);
		}
		{
			xb.setBroadcast<0>(in1);
			yb.setBroadcast<1>(in1);
			zb.setBroadcast<2>(in1);

			out.setMul( xb, m0 );
			out.addMul( yb, m1 );
			out.addMul( zb, m2 );
			vOut.setVector<1>(out);
		}
		{
			xb.setBroadcast<0>(in2);
			yb.setBroadcast<1>(in2);
			zb.setBroadcast<2>(in2);

			out.setMul( xb, m0 );
			out.addMul( yb, m1 );
			out.addMul( zb, m2 );
			vOut.setVector<2>(out);
		}
		{
			xb.setBroadcast<0>(in3);
			yb.setBroadcast<1>(in3);
			zb.setBroadcast<2>(in3);

			out.setMul( xb, m0 );
			out.addMul( yb, m1 );
			out.addMul( zb, m2 );
			vOut.setVector<3>(out);
		}
	}

	HK_FORCE_INLINE static void rotateDirectionH( hkMxTransformParameter mat, const hkMxVector<3>& vIn, hkMxVector<3>& vOut)
	{
		// the vectors in mat are the columns of a transform matrix
		hkVector4 m0,m1,m2; 
		{
			mat.getVector<0>(m0);
			mat.getVector<1>(m1);
			mat.getVector<2>(m2);
		}

		hkVector4 in0,in1,in2;
		{
			vIn.getVector<0>(in0);
			vIn.getVector<1>(in1);
			vIn.getVector<2>(in2);
		}

		hkVector4 xb,yb,zb;
		hkVector4 out;
		{
			xb.setBroadcast<0>(in0);
			yb.setBroadcast<1>(in0);
			zb.setBroadcast<2>(in0);

			out.setMul( xb, m0 );
			out.addMul( yb, m1 );
			out.addMul( zb, m2 );
			vOut.setVector<0>(out);
		}
		{
			xb.setBroadcast<0>(in1);
			yb.setBroadcast<1>(in1);
			zb.setBroadcast<2>(in1);

			out.setMul( xb, m0 );
			out.addMul( yb, m1 );
			out.addMul( zb, m2 );
			vOut.setVector<1>(out);
		}
		{
			xb.setBroadcast<0>(in2);
			yb.setBroadcast<1>(in2);
			zb.setBroadcast<2>(in2);

			out.setMul( xb, m0 );
			out.addMul( yb, m1 );
			out.addMul( zb, m2 );
			vOut.setVector<2>(out);
		}
	}

	HK_FORCE_INLINE static void rotateDirectionH( hkMxTransformParameter mat, const hkMxVector<2>& vIn, hkMxVector<2>& vOut)
	{
		// the vectors in mat are the columns of a transform matrix
		hkVector4 m0,m1,m2; 
		{
			mat.getVector<0>(m0);
			mat.getVector<1>(m1);
			mat.getVector<2>(m2);
		}

		hkVector4 in0,in1;
		{
			vIn.getVector<0>(in0);
			vIn.getVector<1>(in1);
		}

		hkVector4 xb,yb,zb;
		hkVector4 out;
		{
			xb.setBroadcast<0>(in0);
			yb.setBroadcast<1>(in0);
			zb.setBroadcast<2>(in0);

			out.setMul( xb, m0 );
			out.addMul( yb, m1 );
			out.addMul( zb, m2 );
			vOut.setVector<0>(out);
		}
		{
			xb.setBroadcast<0>(in1);
			yb.setBroadcast<1>(in1);
			zb.setBroadcast<2>(in1);

			out.setMul( xb, m0 );
			out.addMul( yb, m1 );
			out.addMul( zb, m2 );
			vOut.setVector<1>(out);
		}
	}

	HK_FORCE_INLINE static void rotateDirectionH( hkMxTransformParameter mat, const hkMxVector<1>& vIn, hkMxVector<1>& vOut)
	{
		vOut.m_vec.v[0]._setRotatedDir((const hkRotation&)mat.m_vec.v[0], vIn.m_vec.v[0]);
	}
}

template <int M>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::rotateDirection( hkMxTransformParameter mat, hkMxVectorParameter vIn, hkMxVector<M>& vOut)
{
	hkMxVectorUtil_Implementation::rotateDirectionH(mat, vIn, vOut);
}


namespace hkMxVectorUtil_Implementation
{
HK_FORCE_INLINE static void rotateInverseDirectionH( hkMxTransformParameter mat, const hkMxVector<4>& vIn, hkMxVector<4>& vOut)
{
	// the vectors in mat are the columns of a transform matrix
	hkVector4 m0,m1,m2; 
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	mat.getVector<0>(m0);
	mat.getVector<1>(m1);
	mat.getVector<2>(m2);
	HK_TRANSPOSE3( m0, m1, m2 );
#else
	m0.set( mat.m_vec.v[0](0), mat.m_vec.v[1](0), mat.m_vec.v[2](0), mat.m_vec.v[3](0) );
	m1.set( mat.m_vec.v[0](1), mat.m_vec.v[1](1), mat.m_vec.v[2](1), mat.m_vec.v[3](1) );
	m2.set( mat.m_vec.v[0](2), mat.m_vec.v[1](2), mat.m_vec.v[2](2), mat.m_vec.v[3](2) );
#endif

	hkVector4 in0,in1,in2,in3;
	{
		vIn.getVector<0>(in0);
		vIn.getVector<1>(in1);
		vIn.getVector<2>(in2);
		vIn.getVector<3>(in3);
	}

	hkVector4 xb,yb,zb;
	hkVector4 out;
	{
		xb.setBroadcast<0>(in0);
		yb.setBroadcast<1>(in0);
		zb.setBroadcast<2>(in0);

		out.setMul( xb, m0 );
		out.addMul( yb, m1 );
		out.addMul( zb, m2 );
		vOut.setVector<0>(out);
	}
	{
		xb.setBroadcast<0>(in1);
		yb.setBroadcast<1>(in1);
		zb.setBroadcast<2>(in1);

		out.setMul( xb, m0 );
		out.addMul( yb, m1 );
		out.addMul( zb, m2 );
		vOut.setVector<1>(out);
	}
	{
		xb.setBroadcast<0>(in2);
		yb.setBroadcast<1>(in2);
		zb.setBroadcast<2>(in2);

		out.setMul( xb, m0 );
		out.addMul( yb, m1 );
		out.addMul( zb, m2 );
		vOut.setVector<2>(out);
	}
	{
		xb.setBroadcast<0>(in3);
		yb.setBroadcast<1>(in3);
		zb.setBroadcast<2>(in3);

		out.setMul( xb, m0 );
		out.addMul( yb, m1 );
		out.addMul( zb, m2 );
		vOut.setVector<3>(out);
	}
}


HK_FORCE_INLINE static void rotateInverseDirectionH( hkMxTransformParameter mat, const hkMxVector<3>& vIn, hkMxVector<3>& vOut)
{
	// the vectors in mat are the columns of a transform matrix
	hkVector4 m0,m1,m2; 
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	mat.getVector<0>(m0);
	mat.getVector<1>(m1);
	mat.getVector<2>(m2);
	HK_TRANSPOSE3( m0, m1, m2 );
#else
	m0.set( mat.m_vec.v[0](0), mat.m_vec.v[1](0), mat.m_vec.v[2](0), mat.m_vec.v[3](0) );
	m1.set( mat.m_vec.v[0](1), mat.m_vec.v[1](1), mat.m_vec.v[2](1), mat.m_vec.v[3](1) );
	m2.set( mat.m_vec.v[0](2), mat.m_vec.v[1](2), mat.m_vec.v[2](2), mat.m_vec.v[3](2) );
#endif

	hkVector4 in0,in1,in2;
	{
		vIn.getVector<0>(in0);
		vIn.getVector<1>(in1);
		vIn.getVector<2>(in2);
	}

	hkVector4 xb,yb,zb;
	hkVector4 out;
	{
		xb.setBroadcast<0>(in0);
		yb.setBroadcast<1>(in0);
		zb.setBroadcast<2>(in0);

		out.setMul( xb, m0 );
		out.addMul( yb, m1 );
		out.addMul( zb, m2 );
		vOut.setVector<0>(out);
	}
	{
		xb.setBroadcast<0>(in1);
		yb.setBroadcast<1>(in1);
		zb.setBroadcast<2>(in1);

		out.setMul( xb, m0 );
		out.addMul( yb, m1 );
		out.addMul( zb, m2 );
		vOut.setVector<1>(out);
	}
	{
		xb.setBroadcast<0>(in2);
		yb.setBroadcast<1>(in2);
		zb.setBroadcast<2>(in2);

		out.setMul( xb, m0 );
		out.addMul( yb, m1 );
		out.addMul( zb, m2 );
		vOut.setVector<2>(out);
	}
}

HK_FORCE_INLINE static void rotateInverseDirectionH( hkMxTransformParameter mat, const hkMxVector<2>& vIn, hkMxVector<2>& vOut)
{
	// the vectors in mat are the columns of a transform matrix
	hkVector4 m0,m1,m2; 
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	mat.getVector<0>(m0);
	mat.getVector<1>(m1);
	mat.getVector<2>(m2);
	HK_TRANSPOSE3( m0, m1, m2 );
#else
	m0.set( mat.m_vec.v[0](0), mat.m_vec.v[1](0), mat.m_vec.v[2](0), mat.m_vec.v[3](0) );
	m1.set( mat.m_vec.v[0](1), mat.m_vec.v[1](1), mat.m_vec.v[2](1), mat.m_vec.v[3](1) );
	m2.set( mat.m_vec.v[0](2), mat.m_vec.v[1](2), mat.m_vec.v[2](2), mat.m_vec.v[3](2) );
#endif

	hkVector4 in0,in1;
	{
		vIn.getVector<0>(in0);
		vIn.getVector<1>(in1);
	}

	hkVector4 xb,yb,zb;
	hkVector4 out;
	{
		xb.setBroadcast<0>(in0);
		yb.setBroadcast<1>(in0);
		zb.setBroadcast<2>(in0);

		out.setMul( xb, m0 );
		out.addMul( yb, m1 );
		out.addMul( zb, m2 );
		vOut.setVector<0>(out);
	}
	{
		xb.setBroadcast<0>(in1);
		yb.setBroadcast<1>(in1);
		zb.setBroadcast<2>(in1);

		out.setMul( xb, m0 );
		out.addMul( yb, m1 );
		out.addMul( zb, m2 );
		vOut.setVector<1>(out);
	}
}
HK_FORCE_INLINE static void rotateInverseDirectionH( hkMxTransformParameter mat, const hkMxVector<1>& vIn, hkMxVector<1>& vOut)
{
	vOut.m_vec.v[0]._setRotatedInverseDir((const hkRotation&)mat.m_vec.v[0], vIn.m_vec.v[0]); 
}
}

template <int M>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::rotateInverseDirection( hkMxTransformParameter mat, hkMxVectorParameter vIn, hkMxVector<M>& vOut)
{
	hkMxVectorUtil_Implementation::rotateInverseDirectionH(mat, vIn, vOut);
}




namespace hkMxVectorUtil_Implementation
{
template <int I>
HK_FORCE_INLINE void rotateDirH(const hkQuaternion* mat, const hkVector4* vIn, hkVector4* vOut)
{
	rotateDirH<I-1>(mat, vIn, vOut);
	vOut[I-1]._setRotatedDir( mat[I-1], vIn[I-1] );
}
template <>
HK_FORCE_INLINE void rotateDirH<1>(const hkQuaternion* mat, const hkVector4* vIn, hkVector4* vOut)
{
	vOut[0]._setRotatedDir( mat[0], vIn[0] );
}
}
template <int M>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::rotateDirection( hkMxQuaternionParameter mat, hkMxVectorParameter vIn, hkMxVector<M>& vOut)
{
	hkMxVectorUtil_Implementation::rotateDirH<M>((const hkQuaternion*)mat.m_vec.v, vIn.m_vec.v, vOut.m_vec.v);
}


namespace hkMxVectorUtil_Implementation
{
template <int I>
HK_FORCE_INLINE void rotateDirH(const hkTransform* mat, const hkVector4* vIn, hkVector4* vOut)
{
	rotateDirH<I-1>(mat, vIn, vOut);
	vOut[I-1]._setRotatedDir( mat[I-1].getRotation(), vIn[I-1] );
}
template <>
HK_FORCE_INLINE void rotateDirH<1>(const hkTransform* mat, const hkVector4* vIn, hkVector4* vOut)
{
	vOut[0]._setRotatedDir( mat[0].getRotation(), vIn[0] );
}
}
template <int M>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::rotateDirection( const hkTransform* mat, hkMxVectorParameter vIn, hkMxVector<M>& vOut)
{
	hkMxVectorUtil_Implementation::rotateDirH<M>(mat, vIn.m_vec.v, vOut.m_vec.v);
}


namespace hkMxVectorUtil_Implementation
{
template <int I>
HK_FORCE_INLINE void transformPosH(const hkTransform* mat, const hkVector4* vIn, hkVector4* vOut)
{
	transformPosH<I-1>(mat, vIn, vOut);
	vOut[I-1]._setTransformedPos( mat[I-1], vIn[I-1] );
}
template <>
HK_FORCE_INLINE void transformPosH<1>(const hkTransform* mat, const hkVector4* vIn, hkVector4* vOut)
{
	vOut[0]._setTransformedPos( mat[0], vIn[0] );
}
}
template <int M>
HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::transformPosition( const hkTransform* mat, hkMxVectorParameter vIn, hkMxVector<M>& vOut)
{
	hkMxVectorUtil_Implementation::transformPosH<M>(mat, vIn.m_vec.v, vOut.m_vec.v);
}


HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::transform4Positions(hkMxTransformParameter mat0, hkMxTransformParameter mat1, hkMxTransformParameter mat2, hkMxTransformParameter mat3, const hkMxVector<4>& vIn, hkMxVector<4>& vOut)
{
	hkVector4 in0,in1,in2,in3;
	{
		vIn.getVector<0>(in0);
		vIn.getVector<1>(in1);
		vIn.getVector<2>(in2);
		vIn.getVector<3>(in3);
	}

	//	creates better assembly
	//	mat0 x in0
	hkVector4 m00,m10,m20,m30;
	hkVector4 xb0,yb0,zb0;
	hkVector4 out0;

	//	mat1 x in1
	hkVector4 m01,m11,m21,m31;
	hkVector4 xb1,yb1,zb1;
	hkVector4 out1;

	//	mat2 x in2
	hkVector4 m02,m12,m22,m32;
	hkVector4 xb2,yb2,zb2;
	hkVector4 out2;

	//	mat3 x in3
	hkVector4 m03,m13,m23,m33;
	hkVector4 xb3,yb3,zb3;
	hkVector4 out3;
	{
		// get matrix
		mat0.getVector<0>(m00);
		mat0.getVector<1>(m10);
		mat0.getVector<2>(m20);
		mat0.getVector<3>(m30);

		mat1.getVector<0>(m01);
		mat1.getVector<1>(m11);
		mat1.getVector<2>(m21);
		mat1.getVector<3>(m31);

		mat2.getVector<0>(m02);
		mat2.getVector<1>(m12);
		mat2.getVector<2>(m22);
		mat2.getVector<3>(m32);

		mat3.getVector<0>(m03);
		mat3.getVector<1>(m13);
		mat3.getVector<2>(m23);
		mat3.getVector<3>(m33);

		//	set vector
		xb0.setBroadcast<0>(in0);
		yb0.setBroadcast<1>(in0);
		zb0.setBroadcast<2>(in0);

		xb1.setBroadcast<0>(in1);
		yb1.setBroadcast<1>(in1);
		zb1.setBroadcast<2>(in1);

		xb2.setBroadcast<0>(in2);
		yb2.setBroadcast<1>(in2);
		zb2.setBroadcast<2>(in2);

		xb3.setBroadcast<0>(in3);
		yb3.setBroadcast<1>(in3);
		zb3.setBroadcast<2>(in3);

		//	execute
		out0.setAddMul( m30, xb0, m00 );
		out0.addMul( yb0, m10 );
		out0.addMul( zb0, m20 );
		vOut.setVector<0>(out0);

		out1.setAddMul( m31, xb1, m01 );
		out1.addMul( yb1, m11 );
		out1.addMul( zb1, m21 );
		vOut.setVector<1>(out1);

		out2.setAddMul( m32, xb2, m02 );
		out2.addMul( yb2, m12 );
		out2.addMul( zb2, m22 );
		vOut.setVector<2>(out2);

		out3.setAddMul( m33, xb3, m03 );
		out3.addMul( yb3, m13 );
		out3.addMul( zb3, m23 );
		vOut.setVector<3>(out3);
	}
}

HK_FORCE_INLINE /*static*/ void HK_CALL hkMxVectorUtil::rotate4Directions(hkMxTransformParameter mat0, hkMxTransformParameter mat1, hkMxTransformParameter mat2, hkMxTransformParameter mat3, const hkMxVector<4>& vIn, hkMxVector<4>& vOut)
{
	hkVector4 in0,in1,in2,in3;
	{
		vIn.getVector<0>(in0);
		vIn.getVector<1>(in1);
		vIn.getVector<2>(in2);
		vIn.getVector<3>(in3);
	}

	//	creates better assembly
	//	mat0 x in0
	hkVector4 m00,m10,m20;
	hkVector4 xb0,yb0,zb0;
	hkVector4 out0;

	//	mat1 x in1
	hkVector4 m01,m11,m21;
	hkVector4 xb1,yb1,zb1;
	hkVector4 out1;

	//	mat2 x in2
	hkVector4 m02,m12,m22;
	hkVector4 xb2,yb2,zb2;
	hkVector4 out2;

	//	mat3 x in3
	hkVector4 m03,m13,m23;
	hkVector4 xb3,yb3,zb3;
	hkVector4 out3;
	{
		// get matrix
		mat0.getVector<0>(m00);
		mat0.getVector<1>(m10);
		mat0.getVector<2>(m20);

		mat1.getVector<0>(m01);
		mat1.getVector<1>(m11);
		mat1.getVector<2>(m21);

		mat2.getVector<0>(m02);
		mat2.getVector<1>(m12);
		mat2.getVector<2>(m22);

		mat3.getVector<0>(m03);
		mat3.getVector<1>(m13);
		mat3.getVector<2>(m23);

		//	set vector
		xb0.setBroadcast<0>(in0);
		yb0.setBroadcast<1>(in0);
		zb0.setBroadcast<2>(in0);

		xb1.setBroadcast<0>(in1);
		yb1.setBroadcast<1>(in1);
		zb1.setBroadcast<2>(in1);

		xb2.setBroadcast<0>(in2);
		yb2.setBroadcast<1>(in2);
		zb2.setBroadcast<2>(in2);

		xb3.setBroadcast<0>(in3);
		yb3.setBroadcast<1>(in3);
		zb3.setBroadcast<2>(in3);

		//	execute
		out0.setMul( xb0, m00 );
		out0.addMul( yb0, m10 );
		out0.addMul( zb0, m20 );
		vOut.setVector<0>(out0);

		out1.setMul( xb1, m01 );
		out1.addMul( yb1, m11 );
		out1.addMul( zb1, m21 );
		vOut.setVector<1>(out1);

		out2.setMul( xb2, m02 );
		out2.addMul( yb2, m12 );
		out2.addMul( zb2, m22 );
		vOut.setVector<2>(out2);

		out3.setMul( xb3, m03 );
		out3.addMul( yb3, m13 );
		out3.addMul( zb3, m23 );
		vOut.setVector<3>(out3);
	}
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
