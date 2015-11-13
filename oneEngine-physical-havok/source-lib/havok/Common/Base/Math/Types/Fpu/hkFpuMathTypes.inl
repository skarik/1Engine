/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// no special includes necessary

// transpose 4 hkVector4
#define HK_TRANSPOSE4_SWAP(a,b) t = a; a = b; b = t
#define HK_TRANSPOSE4(v0,v1,v2,v3) { hkReal t; \
	HK_TRANSPOSE4_SWAP( v0(1), v1(0) ); \
	HK_TRANSPOSE4_SWAP( v0(2), v2(0) ); \
	HK_TRANSPOSE4_SWAP( v0(3), v3(0) ); \
	HK_TRANSPOSE4_SWAP( v1(2), v2(1) ); \
	HK_TRANSPOSE4_SWAP( v1(3), v3(1) ); \
	HK_TRANSPOSE4_SWAP( v2(3), v3(2) ); }

// transpose 3 hkVector4: w component is undefined
#define HK_TRANSPOSE3_SWAP(a,b) t = a; a = b; b = t
#define HK_TRANSPOSE3(v0,v1,v2) { hkReal t; \
	HK_TRANSPOSE3_SWAP( v0(1), v1(0) ); \
	HK_TRANSPOSE3_SWAP( v0(2), v2(0) ); \
	HK_TRANSPOSE3_SWAP( v1(2), v2(1) ); }



// storage type for hkVector4 (and thus hkQuaternion)
struct hkQuadReal
{
	HK_ALIGN_REAL( hkReal v[4] );
};

// storage type for hkSimdReal
typedef hkReal hkSingleReal;

// storage type for hkVector4Comparison
typedef hkInt32 hkVector4Mask;

// storage  type for hkIntVector
struct hkQuadUint
{
	//+hk.MemoryTracker(ignore=True)
	HK_ALIGN_REAL(hkUint32 u32[4]);
};

// argument types
class hkVector4;
typedef const hkVector4& hkVector4Parameter;

class hkSimdReal;
typedef const hkSimdReal& hkSimdRealParameter;

class hkVector4Comparison;
typedef const hkVector4Comparison& hkVector4ComparisonParameter;

class hkQuaternion;
typedef const hkQuaternion& hkQuaternionParameter;

class hkIntVector;
typedef const hkIntVector& hkIntVectorParameter;

// this causes problems for the optimizer, use for debug checks only
#define HK_QUADREAL_CONSTANT(a, b, c, d)	{{a,b,c,d}}
#define HK_QUADINT_CONSTANT(a, b, c, d)		{(a), (b), (c), (d)}

union hkQuadRealUnion
{
	hkReal r[4];
	hkQuadReal q;
};

union hkQuadUintUnion
{
	hkUint32 u[4];
	hkQuadUint q;
};

union hkQuadShortUnion
{
	hkInt16 u[8];
	hkQuadUint q;
};

union hkQuadUshortUnion
{
	hkUint16 u[8];
	hkQuadUint q;
};

union hkQuadCharUnion
{
	hkUint8 u[16];
	hkQuadUint q;
};

union hkQuadUcharUnion
{
	hkUint8 u[16];
	hkQuadUint q;
};

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
