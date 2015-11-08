/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HK_BASE_PERFORMANCE_TEST_H
#define HK_BASE_PERFORMANCE_TEST_H
//HK_REFLECTION_PARSER_EXCLUDE_FILE

#include <Common/Base/System/Stopwatch/hkStopwatch.h>


	#define HK_PERFORMANCE_TEST_REGISTER(func, menu, path) extern const hkPerformanceTestEntry register##func = { func, #func, menu path }




	struct hkPerformanceTestInput
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, hkPerformanceTestInput);

		hkPerformanceTestInput() : m_randomVectors11In(HK_NULL), m_userVectorBuffer(HK_NULL), m_userSimdRealBuffer(HK_NULL), m_numVectors(0) {}

		/// Random vectors between -1 and 1
		const hkVector4* HK_RESTRICT m_randomVectors11In;

		/// A buffer free for use by any of the performance tests, if necessary.
		hkVector4* HK_RESTRICT m_userVectorBuffer;
		hkSimdReal* HK_RESTRICT m_userSimdRealBuffer;

		/// Number of random vectors, simdreals and vectors in the user buffers
		int m_numVectors;

	};

	struct hkPerformanceTestOutput
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, hkPerformanceTestOutput);

		hkPerformanceTestOutput() : m_elapsedTicks(0), m_confident(false), m_vectorsOut(HK_NULL), m_simdrealsOut(HK_NULL), m_resultInt(0), m_numTests(0), m_referenceTicks(0) { m_testName[0] = 0; }

		/// The elapsed CPU ticks. This is considered to be for the function to test only, so
		/// you might want to subtract any overhead.
		hkUint64 m_elapsedTicks;

		/// If this is set to false, the m_elapsedTicks should be taken with care as the testing routine
		/// considers the timings to be inaccurate.
		hkBool   m_confident;

		/// A buffer of input.m_numVectors vectors initialized to 1, can be used to output data
		hkVector4* HK_RESTRICT m_vectorsOut;
		hkSimdReal* HK_RESTRICT m_simdrealsOut;

		/// A return value, useful for checksumming or similar
		hkUint32 m_resultInt;

		/// The number of tests performed
		hkUint32 m_numTests;

		/// The reference performance, checked for regressions
		hkUint64 m_referenceTicks;

		/// The name of the test for text output purposes
		char m_testName[256];

		/// Number of supported platforms
		enum { NUM_PLATFORMS = 18 };

		/// get index for reference ticks
		static int HK_CALL getReferenceTickIndex(int testIndex);

		/// save result as out-of-line methods
		template <class T> static void HK_CALL storeOutput(int index, T* HK_RESTRICT out, const T& v)
		{
			static volatile const T storage = v;
			const T* storagePtr = (const T*)&storage;
			out[index] = storagePtr[0];
		}
		template <class T> static void HK_CALL accOutput(int index, T* HK_RESTRICT out, const T& v)
		{
			static volatile const T storage = v;
			const T* storagePtr = (const T*)&storage;
			out[index].add(storagePtr[0]);
		}
	};



	struct hkPerformanceTestEntry
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BASE_CLASS, hkPerformanceTestEntry );

		/// This function is called with variant=0... until the function returns HK_FAILURE
		typedef hkResult (HK_CALL *hkPerformanceTestFunction)( const hkPerformanceTestInput& input, int variant, hkPerformanceTestOutput& output );

		static void HK_CALL runTests(hkPerformanceTestEntry const*const*const tests);

		hkPerformanceTestFunction m_func;
		const char* m_name;
		const char* m_path;
	};
	extern const hkPerformanceTestEntry* hkPerformanceTestDatabase[];



#endif // HK_BASE_PERFORMANCE_TEST_H

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
