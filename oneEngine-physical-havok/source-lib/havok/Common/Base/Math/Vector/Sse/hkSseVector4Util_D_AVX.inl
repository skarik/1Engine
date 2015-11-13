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

#define HK_VECTOR4UTIL_exitMmx
HK_FORCE_INLINE void HK_CALL hkVector4Util::exitMmx()
{
#	ifndef HK_ARCH_X64
	_mm_empty();
#	endif
}


#define HK_VECTOR4UTIL_dot4_4vs4
HK_FORCE_INLINE void HK_CALL hkVector4Util::dot4_4vs4( hkVector4Parameter a0, hkVector4Parameter b0, hkVector4Parameter a1, hkVector4Parameter b1, hkVector4Parameter a2, hkVector4Parameter b2, hkVector4Parameter a3, hkVector4Parameter b3, hkVector4& dotsOut)
{
	const hkQuadReal m0 = _mm256_mul_pd(b0.m_quad, a0.m_quad);
	const hkQuadReal m1 = _mm256_mul_pd(b1.m_quad, a1.m_quad);
	const hkQuadReal m2 = _mm256_mul_pd(b2.m_quad, a2.m_quad);
	const hkQuadReal m3 = _mm256_mul_pd(b3.m_quad, a3.m_quad);

	// in-lane sum
	const hkQuadReal hsum0 = _mm256_hadd_pd(m0, m0);
	const hkQuadReal hsum1 = _mm256_hadd_pd(m1, m1);
	const hkQuadReal hsum2 = _mm256_hadd_pd(m2, m2);
	const hkQuadReal hsum3 = _mm256_hadd_pd(m3, m3);

	// cross-lane
	const hkQuadReal hsuminv0 = _mm256_permute2f128_pd(hsum0, hsum0, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH));
	const hkQuadReal hsuminv1 = _mm256_permute2f128_pd(hsum1, hsum1, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH));
	const hkQuadReal hsuminv2 = _mm256_permute2f128_pd(hsum2, hsum2, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH));
	const hkQuadReal hsuminv3 = _mm256_permute2f128_pd(hsum3, hsum3, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH));

	// grand sum
	const hkQuadReal dp0 = _mm256_add_pd(hsum0, hsuminv0);
	const hkQuadReal dp1 = _mm256_add_pd(hsum1, hsuminv1);
	const hkQuadReal dp2 = _mm256_add_pd(hsum2, hsuminv2);
	const hkQuadReal dp3 = _mm256_add_pd(hsum3, hsuminv3);

	const hkQuadReal dp0022 = _mm256_permute2f128_pd(dp0, dp2, 0x20);
	const hkQuadReal dp1133 = _mm256_permute2f128_pd(dp1, dp3, 0x20);

	dotsOut.m_quad = _mm256_shuffle_pd(dp0022, dp1133, _MM256_SHUFFLE(1, 0, 1, 0));
}

#define HK_VECTOR4UTIL_dot4_1vs4
HK_FORCE_INLINE void HK_CALL hkVector4Util::dot4_1vs4( hkVector4Parameter vectorIn, hkVector4Parameter a0, hkVector4Parameter a1, hkVector4Parameter a2, hkVector4Parameter a3, hkVector4& dotsOut)
{
	const hkQuadReal m0 = _mm256_mul_pd(vectorIn.m_quad, a0.m_quad);
	const hkQuadReal m1 = _mm256_mul_pd(vectorIn.m_quad, a1.m_quad);
	const hkQuadReal m2 = _mm256_mul_pd(vectorIn.m_quad, a2.m_quad);
	const hkQuadReal m3 = _mm256_mul_pd(vectorIn.m_quad, a3.m_quad);

	// in-lane sum
	const hkQuadReal hsum0 = _mm256_hadd_pd(m0, m0);
	const hkQuadReal hsum1 = _mm256_hadd_pd(m1, m1);
	const hkQuadReal hsum2 = _mm256_hadd_pd(m2, m2);
	const hkQuadReal hsum3 = _mm256_hadd_pd(m3, m3);

	// cross-lane
	const hkQuadReal hsuminv0 = _mm256_permute2f128_pd(hsum0, hsum0, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH));
	const hkQuadReal hsuminv1 = _mm256_permute2f128_pd(hsum1, hsum1, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH));
	const hkQuadReal hsuminv2 = _mm256_permute2f128_pd(hsum2, hsum2, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH));
	const hkQuadReal hsuminv3 = _mm256_permute2f128_pd(hsum3, hsum3, _MM256_PERMUTE2(_MM256_A_LOW, _MM256_A_HIGH));

	// grand sum
	const hkQuadReal dp0 = _mm256_add_pd(hsum0, hsuminv0);
	const hkQuadReal dp1 = _mm256_add_pd(hsum1, hsuminv1);
	const hkQuadReal dp2 = _mm256_add_pd(hsum2, hsuminv2);
	const hkQuadReal dp3 = _mm256_add_pd(hsum3, hsuminv3);

	const hkQuadReal dp0022 = _mm256_permute2f128_pd(dp0, dp2, 0x20);
	const hkQuadReal dp1133 = _mm256_permute2f128_pd(dp1, dp3, 0x20);

	dotsOut.m_quad = _mm256_shuffle_pd(dp0022, dp1133, _MM256_SHUFFLE(1, 0, 1, 0));
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
