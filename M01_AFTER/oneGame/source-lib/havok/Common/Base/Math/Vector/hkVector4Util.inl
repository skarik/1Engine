/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


#ifndef HK_VECTOR4UTIL_setNormalOfTriangle
HK_FORCE_INLINE void HK_CALL hkVector4Util::setNormalOfTriangle(hkVector4 &result, hkVector4Parameter a, hkVector4Parameter b, hkVector4Parameter c)
{
	hkVector4 ba,ca;
	ba.setSub( b, a );
	ca.setSub( c, a );
	result.setCross( ba , ca);
}
#endif

#ifndef HK_VECTOR4UTIL_atan2Approximation
HK_FORCE_INLINE void HK_CALL hkVector4Util::atan2Approximation(hkVector4Parameter y, hkVector4Parameter x, hkVector4& result)
{
	result.set(hkMath::atan2Approximation(y(0),x(0)), hkMath::atan2Approximation(y(1),x(1)), hkMath::atan2Approximation(y(2),x(2)), hkMath::atan2Approximation(y(3),x(3)));
}
#endif

#ifndef HK_VECTOR4UTIL_logApproximation
HK_FORCE_INLINE void HK_CALL hkVector4Util::logApproximation(hkVector4Parameter v, hkVector4& result)
{
	result.set(hkMath::logApproximation(v(0)), hkMath::logApproximation(v(1)), hkMath::logApproximation(v(2)), hkMath::logApproximation(v(3)));
}
#endif

#ifndef HK_VECTOR4UTIL_convertFromHalf
HK_FORCE_INLINE void HK_CALL hkVector4Util::convertFromHalf( const hkHalf& a, const hkHalf& b, const hkHalf& c, const hkHalf& d, hkVector4& out )
{
	hkFloat32 fa = a;
	hkFloat32 fb = b;
	hkFloat32 fc = c;
	hkFloat32 fd = d;
	out.set(hkReal(fa),hkReal(fb),hkReal(fc),hkReal(fd));
}
#endif

#ifndef HK_VECTOR4UTIL_convertToUint16
HK_FORCE_INLINE void HK_CALL hkVector4Util::convertToUint16( hkVector4Parameter in, hkVector4Parameter offset, hkVector4Parameter scale, hkIntUnion64& out)
{
	hkVector4 x;
	x.setAdd(in,offset);
	x.mul(scale);

	hkIntVector i;
	i.setConvertF32toS32(x);
	out.i16[0] = (hkInt16)i.getU32<0>();
	out.i16[1] = (hkInt16)i.getU32<1>();
	out.i16[2] = (hkInt16)i.getU32<2>();
	out.i16[3] = (hkInt16)i.getU32<3>();
}
#endif

#ifndef HK_VECTOR4UTIL_convertToUint16WithClip
// can't be inside convert to clip as gcc will not inline funcs with statics in them
static hkQuadReal convertToUint16WithClipMagicQuad = HK_QUADREAL_CONSTANT( 65536.0f, 65536.0f, 65536.0f, 65536.0f );

HK_FORCE_INLINE void HK_CALL hkVector4Util::convertToUint16WithClip( hkVector4Parameter in, hkVector4Parameter offset, hkVector4Parameter scale, hkVector4Parameter min, hkVector4Parameter max, hkIntUnion64& out)
{
	hkVector4 x;
	x.setAdd(in,offset);
	x.mul(scale);
	x.setMin(x,max);
	x.setMax(x,min);

#if !defined(HK_REAL_IS_DOUBLE)
	x.add( (const hkVector4&)convertToUint16WithClipMagicQuad );
	
	union 
	{
		hkQuadReal q;
		hkUint32 i[4];
	} u;

	u.q = x.m_quad;
	// note implicitly throw away top 16 bits
	out.i16[0] = hkInt16(u.i[0] >> 7);
	out.i16[1] = hkInt16(u.i[1] >> 7);
	out.i16[2] = hkInt16(u.i[2] >> 7);
	out.i16[3] = hkInt16(u.i[3] >> 7);
#else
	hkIntVector i;
	i.setConvertF32toU32(x);
	out.i16[0] = (hkInt16)i.getU32<0>();
	out.i16[1] = (hkInt16)i.getU32<1>();
	out.i16[2] = (hkInt16)i.getU32<2>();
	out.i16[3] = (hkInt16)i.getU32<3>();
#endif
}
#endif

#ifndef HK_VECTOR4UTIL_convertToUint32
HK_FORCE_INLINE void HK_CALL hkVector4Util::convertToUint32( hkVector4Parameter in, hkVector4Parameter offset, hkVector4Parameter scale, hkUint32* out)
{
	hkVector4 x;
	x.setAdd(in,offset);
	x.mul(scale);
	hkIntVector i;
	i.setConvertF32toU32(x);
	i.storeNotAligned<4>(out);
}
#endif

#ifndef HK_VECTOR4UTIL_convertToUint32WithClip
HK_FORCE_INLINE void HK_CALL hkVector4Util::convertToUint32WithClip( hkVector4Parameter in, hkVector4Parameter offset, hkVector4Parameter scale, hkVector4Parameter min, hkVector4Parameter max, hkUint32* out)
{
	hkVector4 x;
	x.setAdd(in,offset);
	x.mul(scale);
	x.setMin(x,max);
	x.setMax(x,min);
	hkIntVector i;
	i.setConvertF32toU32(x);
	i.storeNotAligned<4>(out);
}
#endif

#if !defined(HK_VECTOR4UTIL_convertAabbToUint32)
HK_FORCE_INLINE void HK_CALL hkVector4Util::convertAabbToUint32( const hkAabb& aabb, hkVector4Parameter offsetLow, hkVector4Parameter offsetHigh, hkVector4Parameter scale, hkAabbUint32& aabbOut )
{
	HK_ASSERT3(0x4868f301,  aabb.isValid(), "AABB at " << &aabb << " was invalid. (Contains a NaN or min > max)");

	hkVector4 maxVal; maxVal.m_quad = hkVector4Util::hkAabbUint32MaxVal;
	hkVector4 zero; zero.setZero();
	hkVector4Util::convertToUint32WithClip( aabb.m_min, offsetLow,  scale, zero, maxVal, aabbOut.m_min );
	hkVector4Util::convertToUint32WithClip( aabb.m_max, offsetHigh, scale, zero, maxVal, aabbOut.m_max );
}
#endif

#ifndef HK_VECTOR4UTIL_convertAabbFromUint32
//Based on void hkp3AxisSweep::getAabbFromNode(const hkpBpNode& node, hkAabb & aabb) const
HK_FORCE_INLINE void HK_CALL hkVector4Util::convertAabbFromUint32( const hkAabbUint32& aabbIn, hkVector4Parameter offsetLow, hkVector4Parameter scale, hkAabb& aabbOut)
{
	aabbOut.m_min.set( hkReal(aabbIn.m_min[0]), hkReal(aabbIn.m_min[1]), hkReal(aabbIn.m_min[2]) );
	aabbOut.m_max.set( hkReal(aabbIn.m_max[0]), hkReal(aabbIn.m_max[1]), hkReal(aabbIn.m_max[2]) );

	hkVector4 invScale;
	invScale.setReciprocal(scale); 
	invScale.setComponent<3>(hkSimdReal_1);

	aabbOut.m_min.mul(invScale);
	aabbOut.m_max.mul(invScale);

	aabbOut.m_min.sub(offsetLow);
	aabbOut.m_max.sub(offsetLow);
}
#endif


#ifndef HK_VECTOR4UTIL_calculatePerpendicularVector
HK_FORCE_INLINE void HK_CALL hkVector4Util::calculatePerpendicularVector(hkVector4Parameter vectorIn, hkVector4& biVectorOut)
{
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	hkVector4 d0; d0.setCross( vectorIn, hkVector4::getConstant( HK_QUADREAL_1) );
	hkVector4 d1; d1.setCross( vectorIn, hkVector4::getConstant( HK_QUADREAL_1000) );
	const hkSimdReal l0 = d0.lengthSquared<3>();
	const hkSimdReal l1 = d1.lengthSquared<3>();
	const hkVector4Comparison c = l0.less( l1 );
	biVectorOut.setSelect(c,d1,d0);
#else
	// find the indices of (one of) the smallest component(s) and the indices of
	// the remaining two components in the vector
	int min = 0;
	int ok1 = 1;
	int ok2 = 2;

	hkReal a0 = hkMath::fabs(vectorIn(0));
	hkReal a1 = hkMath::fabs(vectorIn(1));
	hkReal a2 = hkMath::fabs(vectorIn(2));

	// sort the indices to make min index point to the smallest
	if( a1 < a0 )
	{
		ok1 = 0;
		min = 1;
		a0 = a1;
	}

	if(a2 < a0)
	{
		ok2 = min;
		min = 2;
	}

	biVectorOut.setZero();
	biVectorOut(ok1) =  vectorIn(ok2);
	biVectorOut(ok2) = -vectorIn(ok1);
#endif
}
#endif

#ifndef HK_VECTOR4UTIL_transformPoints
HK_FORCE_INLINE void HK_CALL hkVector4Util::transformPoints( const hkTransform& t, const hkVector4* vectorsIn, int numVectors, hkVector4* vectorsOut )
{
	HK_ASSERT2( 0xf0237abd, numVectors > 0, "At least one element required");
	hkTransform unaliased = t;
	int i = numVectors - 1;
	do
	{
		vectorsOut[i]._setTransformedPos( unaliased, vectorsIn[i] );
	} while( --i >= 0 );
}
#endif

#ifndef HK_VECTOR4UTIL_qtransformPoints
HK_FORCE_INLINE void HK_CALL hkVector4Util::transformPoints( const hkQTransform& t, const hkVector4* vectorsIn, int numVectors, hkVector4* vectorsOut )
{
	HK_ASSERT2( 0xf0237abd, numVectors > 0, "At least one element required");
	hkQTransform unaliased = t;
	int i = numVectors - 1;
	do
	{
		vectorsOut[i]._setTransformedPos( unaliased, vectorsIn[i] );
	} while( --i >= 0 );
}
#endif

#ifndef HK_VECTOR4UTIL_qstransformPoints
HK_FORCE_INLINE void HK_CALL hkVector4Util::transformPoints( const hkQsTransform& t, const hkVector4* vectorsIn, int numVectors, hkVector4* vectorsOut )
{
	HK_ASSERT2( 0xf0237abd, numVectors > 0, "At least one element required");
	hkQsTransform unaliased = t;
	int i = numVectors - 1;
	do
	{
		vectorsOut[i]._setTransformedPos( unaliased, vectorsIn[i] );
	} while( --i >= 0 );
}
#endif


#ifndef HK_VECTOR4UTIL_transformPlaneEquations
HK_FORCE_INLINE void HK_CALL hkVector4Util::transformPlaneEquations( const hkTransform& t, const hkVector4* HK_RESTRICT planes, int numPlanes, hkVector4* HK_RESTRICT planesOut )
{
	const hkRotation rotation = t.getRotation();	// we are making a copy here to allow for better compiler support

	hkVector4 tr;	tr.setNeg<4>(t.getTranslation()); 
	hkVector4 pivotShift; pivotShift._setRotatedInverseDir( rotation, tr );

	int p = numPlanes - 1;
	do
	{
		const hkSimdReal newDist = planes->dot4xyz1(pivotShift);
		planesOut->setRotatedDir(rotation, planes[0] );

		planes++;

		planesOut->setW( newDist );
		planesOut++;
	}
	while ( --p >=0 );
}
#endif

#ifndef HK_VECTOR4UTIL_mul4xyz1Points
HK_FORCE_INLINE void HK_CALL hkVector4Util::mul4xyz1Points( const hkTransform& t, const hkVector4* vectorsIn, int numVectors, hkVector4* vectorsOut )
{
	HK_ASSERT2( 0xf0237abe, numVectors > 0, "At least one element required");
	hkTransform unaliased = t;
	int i = numVectors - 1;
	do
	{
		vectorsOut[i]._setTransformedPos( unaliased, vectorsIn[i] );
	} while( --i >= 0 );
}
#endif

#ifndef HK_VECTOR4UTIL_transformSpheres
HK_FORCE_INLINE void HK_CALL hkVector4Util::transformSpheres( const hkTransform& t, const hkVector4* vectorsIn, int numVectors, hkVector4* vectorsOut )
{
	HK_ASSERT2( 0xf0237abf, numVectors > 0, "At least one element required");
	hkTransform unaliased = t;
	int i = numVectors - 1;
	do
	{
		hkVector4 v;
		v._setTransformedPos( unaliased, vectorsIn[i] ); // position
		v.setW(vectorsIn[i]);			// radius
		vectorsOut[i] = v;
	} while( --i >= 0 );
}
#endif

#ifndef HK_VECTOR4UTIL_qstransformSpheres
HK_FORCE_INLINE void HK_CALL hkVector4Util::transformSpheres( const hkQsTransform& t, const hkVector4* vectorsIn, int numVectors, hkVector4* vectorsOut )
{
	HK_ASSERT2( 0xf0237abf, numVectors > 0, "At least one element required");
	hkQsTransform unaliased = t;
	int i = numVectors - 1;
	do
	{
		hkVector4 v;
		v._setTransformedPos( unaliased, vectorsIn[i] ); // position
		v.setW(vectorsIn[i]);			// radius
		vectorsOut[i] = v;
	} while( --i >= 0 );
}
#endif

#ifndef HK_VECTOR4UTIL_rotatePoints
HK_FORCE_INLINE void HK_CALL hkVector4Util::rotatePoints( const hkMatrix3& t, const hkVector4* vectorsIn, int numVectors, hkVector4* vectorsOut )
{
	HK_ASSERT2( 0xf0237ac0, numVectors > 0, "At least one element required");
	hkMatrix3 unaliased = t;
	int i = numVectors - 1;
	do
	{
		vectorsOut[i]._setRotatedDir( unaliased, vectorsIn[i] );
	} while( --i >= 0 );
}
#endif

#ifndef HK_VECTOR4UTIL_rotateInversePoints
HK_FORCE_INLINE void HK_CALL hkVector4Util::rotateInversePoints( const hkRotation& t, const hkVector4* vectorsIn, int numVectors, hkVector4* vectorsOut )
{
	HK_ASSERT2( 0xf0237ac1, numVectors > 0, "At least one element required");

#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	hkVector4 c0 = t.getColumn<0>();
	hkVector4 c1 = t.getColumn<1>();
	hkVector4 c2 = t.getColumn<2>();
	HK_TRANSPOSE3(c0,c1,c2);

	for( int i = 0; i < numVectors; ++i )
	{
		const hkVector4 b = vectorsIn[i];
		const hkSimdReal b0 = b.getComponent<0>();
		hkVector4 r0; r0.setMul( c0, b0 );
		const hkSimdReal b1 = b.getComponent<1>();
		hkVector4 r1; r1.setMul( c1, b1 );
		const hkSimdReal b2 = b.getComponent<2>();
		hkVector4 r2; r2.setMul( c2, b2 );

		hkVector4 out; 
		out.setAdd(r0,r1);
		out.add(r2);

		vectorsOut[i] = out;
	}
#else
	hkRotation unaliased = t;
	int i = numVectors - 1;
	do
	{
		vectorsOut[i]._setRotatedInverseDir( unaliased, vectorsIn[i] );
	} while( --i >= 0 );
#endif
}
#endif

#ifndef HK_VECTOR4UTIL_setMul
HK_FORCE_INLINE void HK_CALL hkVector4Util::setMul( const hkMatrix3& aTb, const hkMatrix3& bTc, hkMatrix3& aTcOut )
{
	HK_ASSERT(0x6d9d1d43,  &aTcOut != &aTb );
	HK_ASSERT(0x64a8df81,  &aTcOut != &bTc );
	hkVector4Util::rotatePoints( aTb, &bTc.getColumn(0), 3, &aTcOut.getColumn(0) );
}
#endif

#ifndef HK_VECTOR4UTIL_setInverseMul
HK_FORCE_INLINE void HK_CALL hkVector4Util::setInverseMul( const hkRotation& bTa, const hkMatrix3& bTc, hkMatrix3& aTcOut )
{
	hkVector4Util::rotateInversePoints( bTa, &bTc.getColumn(0), 3, &aTcOut.getColumn(0) );
}
#endif


#ifndef HK_VECTOR4UTIL_buildOrthonormal
HK_FORCE_INLINE void HK_CALL hkVector4Util::buildOrthonormal( hkVector4Parameter dir, hkMatrix3& out )
{
	hkVector4 perp;
	calculatePerpendicularVector( dir, perp );
	perp.normalize<3>();

	hkVector4 cross;
	cross.setCross( dir, perp );

	out.setColumn<0>(dir);
	out.setColumn<1>(perp);
	out.setColumn<2>(cross);
}
#endif

#ifndef HK_VECTOR4UTIL_buildOrthonormal
HK_FORCE_INLINE void HK_CALL hkVector4Util::buildOrthonormal( hkVector4Parameter dir, hkVector4Parameter dir2, hkMatrix3& out )
{
	hkVector4 cross;
	cross.setCross( dir, dir2 );
	if ( cross.lengthSquared<3>() < hkSimdReal::getConstant<HK_QUADREAL_EPS_SQRD>() )
	{
		buildOrthonormal( dir, out );
		return;
	}
	cross.normalize<3>();

	hkVector4 cross1;
	cross1.setCross( cross, dir );

	out.setColumn<0>(dir);
	out.setColumn<1>(cross1);
	out.setColumn<2>(cross);
}
#endif

#ifndef HK_VECTOR4UTIL_dot3_3vs3
HK_FORCE_INLINE void HK_CALL hkVector4Util::dot3_3vs3(hkVector4Parameter a0, hkVector4Parameter b0, hkVector4Parameter a1, hkVector4Parameter b1, hkVector4Parameter a2, hkVector4Parameter b2, hkVector4& dotsOut)
{
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	hkVector4 c0; c0.setMul(a0,b0);
	hkVector4 c1; c1.setMul(a1,b1);
	hkVector4 c2; c2.setMul(a2,b2);

	HK_TRANSPOSE3(c0,c1,c2);

	dotsOut.setAdd(c0,c1);
	dotsOut.add(c2);
#else
	dotsOut.setComponent<0>( a0.dot<3>( b0 ) );
	dotsOut.setComponent<1>( a1.dot<3>( b1 ) );
	dotsOut.setComponent<2>( a2.dot<3>( b2 ) );
#if defined(HK_PLATFORM_RVL) || defined(HK_PLATFORM_CAFE)
	dotsOut.zeroComponent<3>();
#elif defined(HK_REAL_IS_DOUBLE)
	HK_ON_DEBUG( *((hkUint64*)&(dotsOut.m_quad.v[3])) = 0xffffffffffffffffull; )
#else
	HK_ON_DEBUG( *((hkUint32*)&(dotsOut.m_quad.v[3])) = 0xffffffff; )
#endif
#endif
}
#endif

#ifndef HK_VECTOR4UTIL_dot3_4vs4
HK_FORCE_INLINE void HK_CALL hkVector4Util::dot3_4vs4( hkVector4Parameter a0, hkVector4Parameter b0, hkVector4Parameter a1, hkVector4Parameter b1, hkVector4Parameter a2, hkVector4Parameter b2, hkVector4Parameter a3, hkVector4Parameter b3, hkVector4& dotsOut)
{
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	hkVector4 c0; c0.setMul(a0,b0);
	hkVector4 c1; c1.setMul(a1,b1);
	hkVector4 c2; c2.setMul(a2,b2);
	hkVector4 c3; c3.setMul(a3,b3);

	HK_TRANSPOSE4(c0,c1,c2,c3);

	dotsOut.setAdd(c0,c1);
	dotsOut.add(c2);
#else
	dotsOut.set( a0.dot<3>( b0 ),
				 a1.dot<3>( b1 ),
				 a2.dot<3>( b2 ),
				 a3.dot<3>( b3 ) );
#endif
}
#endif

#ifndef HK_VECTOR4UTIL_dot4_4vs4
HK_FORCE_INLINE void HK_CALL hkVector4Util::dot4_4vs4( hkVector4Parameter a0, hkVector4Parameter b0, hkVector4Parameter a1, hkVector4Parameter b1, hkVector4Parameter a2, hkVector4Parameter b2, hkVector4Parameter a3, hkVector4Parameter b3, hkVector4& dotsOut)
{
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	hkVector4 c0; c0.setMul(a0,b0);
	hkVector4 c1; c1.setMul(a1,b1);
	hkVector4 c2; c2.setMul(a2,b2);
	hkVector4 c3; c3.setMul(a3,b3);

	HK_TRANSPOSE4(c0,c1,c2,c3);

	hkVector4 s; s.setAdd(c2, c3);
	dotsOut.setAdd(c0,c1);
	dotsOut.add(s);
#else
	dotsOut.set( a0.dot<4>( b0 ),
				 a1.dot<4>( b1 ),
				 a2.dot<4>( b2 ),
				 a3.dot<4>( b3 ) );
#endif
}
#endif

#ifndef HK_VECTOR4UTIL_dot3_1vs4
HK_FORCE_INLINE void HK_CALL hkVector4Util::dot3_1vs4( hkVector4Parameter vectorIn, hkVector4Parameter a0, hkVector4Parameter a1, hkVector4Parameter a2, hkVector4Parameter a3, hkVector4& dotsOut)
{
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	hkVector4 c0; c0.setMul(a0, vectorIn);
	hkVector4 c1; c1.setMul(a1, vectorIn);
	hkVector4 c2; c2.setMul(a2, vectorIn);
	hkVector4 c3; c3.setMul(a3, vectorIn);

	HK_TRANSPOSE4(c0, c1, c2, c3);

	dotsOut.setAdd(c0, c1);
	dotsOut.add(c2);
#else
	dotsOut.set( a0.dot<3>( vectorIn ),
				 a1.dot<3>( vectorIn ),
				 a2.dot<3>( vectorIn ),
				 a3.dot<3>( vectorIn ) );
#endif
}
#endif

#ifndef HK_VECTOR4UTIL_dot4_1vs4
HK_FORCE_INLINE void HK_CALL hkVector4Util::dot4_1vs4( hkVector4Parameter vectorIn, hkVector4Parameter a0, hkVector4Parameter a1, hkVector4Parameter a2, hkVector4Parameter a3, hkVector4& dotsOut)
{
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	hkVector4 c0; c0.setMul(a0, vectorIn);
	hkVector4 c1; c1.setMul(a1, vectorIn);
	hkVector4 c2; c2.setMul(a2, vectorIn);
	hkVector4 c3; c3.setMul(a3, vectorIn);

	HK_TRANSPOSE4(c0, c1, c2, c3);

	hkVector4 s; s.setAdd(c2, c3);
	dotsOut.setAdd(c0, c1);
	dotsOut.add(s);
#else
	dotsOut.set( a0.dot<4>( vectorIn ),
				 a1.dot<4>( vectorIn ),
				 a2.dot<4>( vectorIn ),
				 a3.dot<4>( vectorIn ) );
#endif
}
#endif

#ifndef HK_VECTOR4UTIL_dot3_2vs2
HK_FORCE_INLINE void HK_CALL hkVector4Util::dot3_2vs2( hkVector4Parameter a0, hkVector4Parameter a2, hkVector4Parameter b0, hkVector4Parameter b1, hkVector4& dotsOut)
{
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	hkVector4 c0; c0.setMul(a0,b0);
	hkVector4 c1; c1.setMul(a0,b1);
	hkVector4 c2; c2.setMul(a2,b0);
	hkVector4 c3; c3.setMul(a2,b1);

	HK_TRANSPOSE4(c0,c1,c2,c3);

	dotsOut.setAdd(c0,c1);
	dotsOut.add(c2);
#else
	dotsOut.set( a0.dot<3>( b0 ),
				 a0.dot<3>( b1 ),
				 a2.dot<3>( b0 ),
				 a2.dot<3>( b1 ) );
#endif
}
#endif

//
//	Computes an = (a x n), bn = (b x n), cn = (c x n)

#ifndef HK_VECTOR4UTIL_cross_3vs1
HK_FORCE_INLINE void HK_CALL hkVector4Util::cross_3vs1(hkVector4Parameter a, hkVector4Parameter b, hkVector4Parameter c, hkVector4Parameter n,
												hkVector4& an, hkVector4& bn, hkVector4& cn)
{
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	hkVector4 tmp_n; tmp_n.setPermutation<hkVectorPermutation::ZXYW>(n);
	hkVector4 tmp_a; tmp_a.setPermutation<hkVectorPermutation::YZXW>(a);
	hkVector4 tmp_b; tmp_b.setPermutation<hkVectorPermutation::YZXW>(b);
	hkVector4 tmp_c; tmp_c.setPermutation<hkVectorPermutation::YZXW>(c);

	tmp_a.mul(tmp_n);
	tmp_b.mul(tmp_n);
	tmp_c.mul(tmp_n);

	hkVector4 p_n; p_n.setPermutation<hkVectorPermutation::YZXW>(n);
	hkVector4 p_a; p_a.setPermutation<hkVectorPermutation::ZXYW>(a);
	hkVector4 p_b; p_b.setPermutation<hkVectorPermutation::ZXYW>(b);
	hkVector4 p_c; p_c.setPermutation<hkVectorPermutation::ZXYW>(c);

	an.setSubMul(tmp_a, p_n, p_a);
	bn.setSubMul(tmp_b, p_n, p_b);
	cn.setSubMul(tmp_c, p_n, p_c);
#else
	an.setCross(a, n);
	bn.setCross(b, n);
	cn.setCross(c, n);
#endif
}
#endif

//
//	Computes an = (a x n), bn = (b x n), cn = (c x n), dn = (d x n)

#ifndef HK_VECTOR4UTIL_cross_4vs1
HK_FORCE_INLINE void HK_CALL hkVector4Util::cross_4vs1(	hkVector4Parameter a, hkVector4Parameter b, hkVector4Parameter c, hkVector4Parameter d, hkVector4Parameter n,
														hkVector4& an, hkVector4& bn, hkVector4& cn, hkVector4& dn)
{
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	hkVector4 tmp4; tmp4.setPermutation<hkVectorPermutation::ZXYW>(n);

	hkVector4 t_a; t_a.setPermutation<hkVectorPermutation::YZXW>(a);
	hkVector4 t_b; t_b.setPermutation<hkVectorPermutation::YZXW>(b);
	hkVector4 t_c; t_c.setPermutation<hkVectorPermutation::YZXW>(c);
	hkVector4 t_d; t_d.setPermutation<hkVectorPermutation::YZXW>(d);

	t_a.mul(tmp4);
	t_b.mul(tmp4);
	t_c.mul(tmp4);
	t_d.mul(tmp4);

	hkVector4 ntmp4; ntmp4.setPermutation<hkVectorPermutation::YZXW>(n);

	hkVector4 p_a; p_a.setPermutation<hkVectorPermutation::ZXYW>(a);
	hkVector4 p_b; p_b.setPermutation<hkVectorPermutation::ZXYW>(b);
	hkVector4 p_c; p_c.setPermutation<hkVectorPermutation::ZXYW>(c);
	hkVector4 p_d; p_d.setPermutation<hkVectorPermutation::ZXYW>(d);

	an.setSubMul(t_a, ntmp4, p_a);
	bn.setSubMul(t_b, ntmp4, p_b);
	cn.setSubMul(t_c, ntmp4, p_c);
	dn.setSubMul(t_d, ntmp4, p_d);
#else
	an.setCross(a, n);
	bn.setCross(b, n);
	cn.setCross(c, n);
	dn.setCross(d, n);
#endif
}
#endif

//
//	Computes the cross products: (vA, vB), (vB, vC), (vC, vA)

#ifndef HK_VECTOR4UTIL_computeCyclicCrossProducts
HK_FORCE_INLINE void hkVector4Util::computeCyclicCrossProducts(	hkVector4Parameter vA, hkVector4Parameter vB, hkVector4Parameter vC,
																hkVector4& vAB, hkVector4& vBC, hkVector4& vCA)
{
#if (HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED)
	hkVector4 tmpa; tmpa.setPermutation<hkVectorPermutation::YZXW>(vA);	// tmpa = (ay, az, ax, *)
	hkVector4 tmpb; tmpb.setPermutation<hkVectorPermutation::ZXYW>(vB);	// tmpb = (bz, bx, by, *)
	hkVector4 tmpc; tmpc.setPermutation<hkVectorPermutation::YZXW>(vC);	// tmpc = (cy, cz, cx, *)
	hkVector4 tmpd; tmpd.setPermutation<hkVectorPermutation::ZXYW>(vA);	// tmpd = (az, ax, ay, *)

	hkVector4 ab; ab.setMul(tmpa, tmpb);	// (ay * bz, az * bx, ax * by, *)
	hkVector4 bc; bc.setMul(tmpb, tmpc);	// (bz * cy, bx * cz, by * cx, *)
	hkVector4 ca; ca.setMul(tmpc, tmpd);	// (cy * az, cz * ax, cx * ay, *)

	hkVector4 pb; pb.setPermutation<hkVectorPermutation::YZXW>(vB);	// tmpb = (by, bz, bx, *)
	hkVector4 pc; pc.setPermutation<hkVectorPermutation::ZXYW>(vC);	// tmpc = (cz, cx, cy, *)
	vAB.setSubMul(ab, tmpd, pb);					// (ay * bz, az * bx, ax * by, *) - (az * by, ax * bz, ay * bx, *) = (vA x vB)
	hkVector4 sb; sb.setMul(pb, pc);											// tmpb = (by * cz, bz * cx, bx * cy, *)
	vCA.setSubMul(ca, pc, tmpa);					// (cy * az, cz * ax, cx * ay, *) - (cz * ay, cx * az, cy * ax, *) = (vC x vA)
	vBC.setSub(sb, bc);										// (by * cz, bz * cx, bx * cy, *) - (bz * cy, bx * cz, by * cx, *) = (vB x vC)*/
#else
	vAB.setCross(vA, vB);
	vBC.setCross(vB, vC);
	vCA.setCross(vC, vA);
#endif
}
#endif



#ifndef HK_VECTOR4UTIL_exitMmx
HK_FORCE_INLINE void HK_CALL hkVector4Util::exitMmx()
{
}
#endif

#ifndef HK_VECTOR4UTIL_distToLineSquared
HK_FORCE_INLINE const hkSimdReal HK_CALL hkVector4Util::distToLineSquared( hkVector4Parameter a, hkVector4Parameter b, hkVector4Parameter p )
{
	hkVector4 ab, ap;
	ab.setSub( b, a );
	ap.setSub( p, a );

	hkSimdReal projectionAsPercentOfSegment; 
	projectionAsPercentOfSegment.setDiv( ap.dot<3>(ab), ab.lengthSquared<3>() );
	ap.subMul( projectionAsPercentOfSegment, ab );
	return ap.lengthSquared<3>();
}
#endif

HK_FORCE_INLINE void HK_CALL hkVector4Util::setMax44(hkVector4Parameter a, hkVector4Parameter b, hkVector4Parameter c, hkVector4Parameter d, hkVector4& out)
{
	hkVector4	t0; t0.setMax(a,b);
	hkVector4	t1; t1.setMax(c,d);
	out.setMax(t0,t1);
}

HK_FORCE_INLINE void HK_CALL hkVector4Util::setMin44(hkVector4Parameter a, hkVector4Parameter b, hkVector4Parameter c, hkVector4Parameter d, hkVector4& out)
{
	hkVector4	t0; t0.setMin(a,b);
	hkVector4	t1; t1.setMin(c,d);
	out.setMin(t0,t1);
}


#ifndef HK_VECTOR4UTIL_unPackInt32IntoQuaternion

// we need to convert the quaternion to int. To avoid problems
// at -1 and 1, we encode [-1.1 .. 1.1]
#define PACK_RANGE 1.1f

HK_FORCE_INLINE void HK_CALL hkVector4Util::unPackInt32IntoQuaternion( hkUint32 ivalue, hkVector4& qout )
{
	hkInt32 a = ivalue;
	hkInt32 b = ivalue >> 8;
	hkInt32 c = ivalue >> 16;
	hkInt32 d = ivalue >> 24;
	a = (a & 0xff) - 0x80;
	b = (b & 0xff) - 0x80;
	c = (c & 0xff) - 0x80;
	d = (d & 0xff) - 0x80;
	hkIntVector i; i.set(a,b,c,d);
	i.convertS32ToF32(qout);
	hkVector4 scale; scale.setAll(hkReal(PACK_RANGE/128.0f));
	qout.mul(scale);
}

#undef PACK_RANGE
#endif

#ifndef HK_VECTOR4UTIL_compressExpandedAabbUint32
HK_FORCE_INLINE void HK_CALL hkVector4Util::compressExpandedAabbUint32(const hkAabbUint32& expandedAabbInt, hkAabbUint32& unexpandedAabbInt_InOut)
{
	hkInt32 mix = unexpandedAabbInt_InOut.m_min[0] - expandedAabbInt.m_min[0];
	hkInt32 miy = unexpandedAabbInt_InOut.m_min[1] - expandedAabbInt.m_min[1];
	hkInt32 miz = unexpandedAabbInt_InOut.m_min[2] - expandedAabbInt.m_min[2];
	hkInt32 maxx= expandedAabbInt.m_max[0] - unexpandedAabbInt_InOut.m_max[0];
	hkInt32 may = expandedAabbInt.m_max[1] - unexpandedAabbInt_InOut.m_max[1];
	hkInt32 maz = expandedAabbInt.m_max[2] - unexpandedAabbInt_InOut.m_max[2];

	hkInt32 m = (mix | miy) | (miz | maxx) | (may | maz);
	HK_ASSERT2( 0xf0ed3454, m >= 0, "Your expanded AABB is smaller than the unexpanded AABB" );
	int shift = 0;
	while ( m >=  0x800){ shift += 4; m = unsigned(m) >> 4; };
	while ( m >=  0x100){ shift += 1; m = unsigned(m) >> 1; };

	unexpandedAabbInt_InOut.m_expansionShift = hkUchar(shift);
	unexpandedAabbInt_InOut.m_expansionMin[0] = hkUchar(mix >> shift);
	unexpandedAabbInt_InOut.m_expansionMin[1] = hkUchar(miy >> shift);
	unexpandedAabbInt_InOut.m_expansionMin[2] = hkUchar(miz >> shift);
	unexpandedAabbInt_InOut.m_expansionMax[0] = hkUchar(maxx >> shift);
	unexpandedAabbInt_InOut.m_expansionMax[1] = hkUchar(may >> shift);
	unexpandedAabbInt_InOut.m_expansionMax[2] = hkUchar(maz >> shift);

}
#endif


#ifndef HK_VECTOR4UTIL_uncompressExpandedAabbUint32
HK_FORCE_INLINE void HK_CALL hkVector4Util::uncompressExpandedAabbUint32(const hkAabbUint32& unexpandedAabbInt, hkAabbUint32& expandedAabbOut)
{
	int shift = unexpandedAabbInt.m_expansionShift;
	hkUint32 mix = unexpandedAabbInt.m_min[0] - (unexpandedAabbInt.m_expansionMin[0] << shift);
	hkUint32 miy = unexpandedAabbInt.m_min[1] - (unexpandedAabbInt.m_expansionMin[1] << shift);
	hkUint32 miz = unexpandedAabbInt.m_min[2] - (unexpandedAabbInt.m_expansionMin[2] << shift);
	expandedAabbOut.m_min[0]= mix;
	expandedAabbOut.m_min[1]= miy;
	expandedAabbOut.m_min[2]= miz;
	hkUint32 maxx = unexpandedAabbInt.m_max[0] + (unexpandedAabbInt.m_expansionMax[0] << shift);
	hkUint32 may  = unexpandedAabbInt.m_max[1] + (unexpandedAabbInt.m_expansionMax[1] << shift);
	hkUint32 maz  = unexpandedAabbInt.m_max[2] + (unexpandedAabbInt.m_expansionMax[2] << shift);
	expandedAabbOut.m_max[0]= maxx;
	expandedAabbOut.m_max[1]= may;
	expandedAabbOut.m_max[2]= maz;
}
#endif

#ifndef HK_VECTOR4UTIL_convertQuaternionToRotation
HK_FORCE_INLINE void HK_CALL hkVector4Util::convertQuaternionToRotation( hkQuaternionParameter qi, hkRotation& rotationOut )
{
	
	HK_ASSERT2(0x1ff88f0e, qi.isOk(), "hkQuaternion not normalized/invalid!");
	const hkVector4 q = qi.m_vec;

	hkVector4 q2; q2.setAdd(q,q);
	hkVector4 xq2; xq2.setMul(q2, q.getComponent<0>());
	hkVector4 yq2; yq2.setMul(q2, q.getComponent<1>());
	hkVector4 zq2; zq2.setMul(q2, q.getComponent<2>());
	hkVector4 wq2; wq2.setMul(q2, q.getComponent<3>());

	const hkSimdReal one = hkSimdReal::getConstant<HK_QUADREAL_1>();
	hkSimdReal zero; zero.setZero();

	// 	1-(yy+zz),	xy+wz,		xz-wy
	// 	xy-wz,		1-(xx+zz),	yz+wx
	// 	xz+wy,		yz-wx,		1-(xx+yy)
	hkVector4 c0; c0.set( one-(yq2.getComponent<1>()+zq2.getComponent<2>()), xq2.getComponent<1>()+wq2.getComponent<2>(), xq2.getComponent<2>()-wq2.getComponent<1>(), zero );
	hkVector4 c1; c1.set( xq2.getComponent<1>()-wq2.getComponent<2>(), one-(xq2.getComponent<0>()+zq2.getComponent<2>()), yq2.getComponent<2>()+wq2.getComponent<0>(), zero );
	hkVector4 c2; c2.set( xq2.getComponent<2>()+wq2.getComponent<1>(), yq2.getComponent<2>()-wq2.getComponent<0>(), one-(xq2.getComponent<0>()+yq2.getComponent<1>()), zero );

	rotationOut.setColumn<0>(c0);
	rotationOut.setColumn<1>(c1);
	rotationOut.setColumn<2>(c2);
}
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
