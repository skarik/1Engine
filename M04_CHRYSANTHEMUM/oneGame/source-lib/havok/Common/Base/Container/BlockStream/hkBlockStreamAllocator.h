/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_BLOCK_STREAM_ALLOCATOR_H
#define HK_BLOCK_STREAM_ALLOCATOR_H

#include <Common/Base/Container/BlockStream/hkBlockStream.h>
#include <Common/Base/Thread/CriticalSection/hkCriticalSection.h>

class hkBlockStreamTlAllocator;

class hkBlockStreamAllocator
{
	public:
		typedef hkBlockStreamBase::Block Block;

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_COLLIDE, hkBlockStreamAllocator );

		void init( int byteSize );

		void init( void* buffer, int byteSize );

		void clear();

		hkBlockStreamAllocator();

		~hkBlockStreamAllocator();

		void addMemory( void* buffer, int byteSize );

		/// Allocate blocks.
		/// Note: 'this' is a pointer to ppu
		void blockAllocBatch( Block** blocksOut, int nblocks );

		/// Free blocks.
		/// Note: 'this' is a pointer to ppu
		void blockFreeBatch( Block** blocks, int nblocks );

		/// get the current bytes used. Note that data in the thread local allocator will
		/// count as bytes used (as they are not available to other threads in a worst case
		/// scenario).
		int getBytesUsed();

		/// the peak usage
		int getMaxBytesUsed();

		HK_FORCE_INLINE int getCapacity() const { return m_capacity * sizeof(hkBlockStreamBase::Block); }

	private:

		void setFreeList();

	public:


		Block* m_storage;

		hkArray< Block* > m_freeList;

		int		m_capacity;			// number of total blocks available
		int     m_minFreeListSize;	// for peak memory usage
		hkBool	m_dontDeallocate;
		hkBool  m_enableAllocationTimerInfo;	///< add extra used memory timer infos into the timer stream, debug only
		HK_ALIGN16(mutable hkCriticalSection m_criticalSection);	///< the critical section, must be the last element
};


/// Thread local allocator, which caches a number of blocks 
class hkBlockStreamTlAllocator
{
	public:
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_COLLIDE, hkBlockStreamTlAllocator );

		/// Constructor.
		/// Note that on spu bsAllocator must be a pointer to the ppu
		hkBlockStreamTlAllocator(hkBlockStreamAllocator* bsAllocator): m_blockStreamAllocator( bsAllocator ), m_numFreeBlocks(0){ }

		~hkBlockStreamTlAllocator(){ clear(); }

		/// Allocate a block
		hkBlockStreamAllocator::Block* blockAlloc();

		/// Deallocate a block
		void blockFree(hkBlockStreamAllocator::Block* blockPpu);

		/// Free a batch
		void blockFreeBatch( hkBlockStreamAllocator::Block** blocks, int nblocks );

		/// Clear all locally cached blocks
		void clear(); 

		enum
		{
			BATCH_ALLOC_SIZE		= 6,	///< The number of blocks freed/allocated in one go in the m_blockStreamManager
			MAX_FREE_LIST_SIZE		= 8,	///< The maximum number of elements cached locally
		};

	protected:
		void checkConsistency( );

	public:

		hkPadSpu<hkBlockStreamAllocator*> m_blockStreamAllocator;

		hkPadSpu<int>	m_numFreeBlocks;
		hkBlockStreamAllocator::Block* m_freeBlocks[MAX_FREE_LIST_SIZE];
};





#endif // HK_BLOCK_STREAM_ALLOCATOR_H

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
