/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKBASE_HK_SEMAPHORE_BUSY_WAIT__H
#define HKBASE_HK_SEMAPHORE_BUSY_WAIT__H

#include <Common/Base/Config/hkConfigThread.h>

#if defined(HK_PLATFORM_CAFE) && (HK_CONFIG_THREAD == HK_CONFIG_MULTI_THREADED)

/// A wrapper class for a semaphore that can work on both SPU and PPU
class hkSemaphoreBusyWait
{
public:

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, hkSemaphoreBusyWait);

	/// Create a semaphore with an initial count and a maximum count. Max count currently ignored.
	hkSemaphoreBusyWait( int initialCount = 0, int maxCount = 1000);

	hkSemaphoreBusyWait( hkFinishLoadedObjectFlag f) {}

	/// Destruct the Semaphore
	~hkSemaphoreBusyWait();

	/// This call will block until the semaphore is released.
	void acquire();

	/// Release the semaphore. Releases a thread blocked by acquire().
	void release(int count = 1);

	// static functions (for SPU compat)
	static void HK_CALL acquire(hkSemaphoreBusyWait* semaphoreOnPpu);
	static void HK_CALL release(hkSemaphoreBusyWait* semaphoreOnPpu, int count = 1);

public:

	HK_ALIGN( hkUint32 m_value, 32 );
	// pad out rest of cache line 
	hkUint8 m_pad[32 - sizeof(hkUint32)]; // 32 byte cache line
};

#elif defined(HK_PLATFORM_NGP) || defined(HK_PLATFORM_ANDROID) || defined(HK_PLATFORM_IOS) 

class hkSemaphoreBusyWait
{
public:

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, hkSemaphoreBusyWait);

	/// Create a semaphore with an initial count and a maximum count. Max count currently ignored.
	hkSemaphoreBusyWait( int initialCount = 0, int maxCount = 1000);

	hkSemaphoreBusyWait( hkFinishLoadedObjectFlag f) {}

	/// Destruct the Semaphore
	~hkSemaphoreBusyWait();

	/// This call will block until the semaphore is released.
	void acquire();

	/// Release the semaphore. Releases a thread blocked by acquire().
	void release(int count = 1);

	// These static functions used for PlayStation(R)3 compat
	static void HK_CALL acquire(hkSemaphoreBusyWait* semaphore);
	static void HK_CALL release(hkSemaphoreBusyWait* semaphoreu, int count = 1);

protected:

	volatile hkUint32 m_value;	
};

#else

#include <Common/Base/Thread/Semaphore/hkSemaphore.h>

// fallback for non Cell platforms, or non threaded Cell builds
typedef hkSemaphore hkSemaphoreBusyWait;

#endif

#endif // HKBASE_HK_SEMAPHORE_BUSY_WAIT__H

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
