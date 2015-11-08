
/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */



HK_FORCE_INLINE void hkBlockStreamBase::Block::setHeaderToZero()
{
	hkString::memClear16( this, BLOCK_HEADER_SIZE>>4 );	// shorter code on spu
	HK_ON_DEBUG( hkString::memSet(m_data, 0xcf, BLOCK_DATA_SIZE) );
}


bool hkBlockStreamBase::Stream::isEmpty() const
{
	HK_ASSERT2(0xf034dfdf, !m_isLocked, "You cannot use isEmpty() while writing to it (call writer::finalize() first.");
	return m_numTotalElements== 0;
}




int hkBlockStreamBase::Stream::getTotalNumElems() const
{
	HK_ASSERT2( 0xf032defd, m_isLocked == false, "You cannot call this function, when non-finalized write itrators are "
												 "linked to the stream.");
	HK_ASSERT2( 0xf032defd, m_partiallyFreed == false, "You cannot call this function when some blocks are freed "
													   "by a hkBlockStream::Consumer" );
	return m_numTotalElements;
}


hkBlockStreamBase::Stream::~Stream()
{
	HK_ASSERT2( 0xf03454ed, m_blocks.getSize() == 0, "You must call clear before the destructor" );
}


//	write iterator

hkBlockStreamBase::Writer::Writer()
{
	HK_ON_DEBUG( m_finalized    = true    );
	HK_ON_DEBUG( m_accessSize   = 0       );
	HK_ON_DEBUG( m_currentBlock = HK_NULL );

	m_blockStream = HK_NULL;
}

hkBlockStreamBase::Writer::~Writer()
{
	HK_ASSERT(0xf034dedf, m_finalized);
}

HK_FORCE_INLINE int hkBlockStreamBase::Writer::getTotalNumElems()
{
	HK_ON_DEBUG(m_finalized = false);
	return m_blockStream->m_numTotalElements + m_currentBlockNumElems;
}



void* hkBlockStreamBase::Writer::reserveBytes( int numBytes )
{
	HK_ASSERT( 0xf03d3401, m_currentBlock != HK_NULL && numBytes <= Block::BLOCK_DATA_SIZE );
	HK_ON_DEBUG( m_accessSize = numBytes );
	HK_ON_DEBUG( m_finalized  = false    );

	// Make sure numBytes fit into the remaining storage space in the current block. Otherwise move to next
	// block in stream.
	int nextLocation = m_currentByteLocation + numBytes;
	if( nextLocation <= Block::BLOCK_DATA_SIZE )
	{
		return hkAddByteOffset( m_currentBlock->begin(), m_currentByteLocation );
	}
	else
	{
		return allocateAndAccessNewBlock();
	}
}


void hkBlockStreamBase::Writer::advance( int numBytes )
{
	// Make sure we are exactly advancing by the size of the previous (write)access.
	HK_ASSERT( 0xf03454df, numBytes <= m_accessSize );
	HK_ON_DEBUG( m_finalized  = false );
	HK_ON_DEBUG( m_accessSize = 0     );

	int nextLocation = m_currentByteLocation + numBytes;
	int numElements  = m_currentBlockNumElems + 1;

	m_currentByteLocation  = nextLocation;
	m_currentBlockNumElems = numElements;
}


void* hkBlockStreamBase::Writer::advanceAndReserveNext( int currentNumBytes, int reservedNumBytes )
{
	advance( currentNumBytes );
	return reserveBytes( reservedNumBytes );
}



HK_FORCE_INLINE void hkBlockStreamBase::Writer::setCurrentBlockFull()
{
	if (m_currentByteLocation!=0)
	{
		m_currentByteLocation = Block::BLOCK_DATA_SIZE;
		allocateAndAccessNewBlock();
	}
}


// template<int SIZE>
// void hkBlockStreamBase::hkBlockStreamWriterBase::prefetch(int offset) const
// {
// 	hkMath::forcePrefetch<SIZE>( hkAddByteOffset(m_currentBlock->begin(), m_currentByteLocation+offset) );
// }

//////////////////////////////////////////////////////////////////////////
//
// Read Iterator
//
//////////////////////////////////////////////////////////////////////////

hkBlockStreamBase::Reader::Reader()
{
}

hkBlockStreamBase::Reader::~Reader()
{

}

void hkBlockStreamBase::Reader::setEmpty()
{
	m_currentByteLocation = HK_NULL;
	HK_ON_SPU( m_nextBuffer = -1);
}


const void* hkBlockStreamBase::Reader::access()
{
	return (const void*)m_currentByteLocation;
}


const void* hkBlockStreamBase::Reader::advanceAndAccessNext(int thisElemSize)
{
	HK_ASSERT( 0xf034df43, m_currentBlock );
	HK_ASSERT( 0xf0347687, m_numElementsToReadInThisBlock > 0 );

	m_numElementsToReadInThisBlock		= m_numElementsToReadInThisBlock-1;

	if( m_numElementsToReadInThisBlock > 0 )
	{
		const char* nextLocation = hkAddByteOffsetConst( (const char*)m_currentByteLocation, thisElemSize );
		HK_ASSERT( 0xf06576df, nextLocation <= m_currentBlock->end() );
		m_currentByteLocation = nextLocation;
		return (const void*)nextLocation;
	}
	else
	{
		const void* data = advanceToNewBlock();
		HK_ASSERT( 0xf06576df, (const char*)m_currentByteLocation + thisElemSize <= m_currentBlock->end() );
		return data;
	}
}

template<int SIZE>
void hkBlockStreamBase::Reader::prefetch(int offset) const
{
	hkMath::forcePrefetch<SIZE>( m_currentByteLocation + offset );
}

//////////////////////////////////////////////////////////////////////////
//
// Consumer Iterator
//
//////////////////////////////////////////////////////////////////////////



const void* hkBlockStreamBase::Consumer::consumeAndAccessNext(int thisElemSize)
{
	HK_ASSERT( 0xf034df43, m_currentBlock );
	HK_ASSERT( 0xf0347687, m_numElementsToReadInThisBlock > 0 );

	m_numElementsToReadInThisBlock		= m_numElementsToReadInThisBlock-1;
	const char* nextLocation = hkAddByteOffsetConst( (const char*)m_currentByteLocation, thisElemSize );	// get this here as this is the most common code path and allows the compiler to optimize better

	if( m_numElementsToReadInThisBlock > 0 )
	{
		HK_ASSERT( 0xf06576df, nextLocation <= m_currentBlock->end() );
		m_currentByteLocation = nextLocation;
		return (const void*)nextLocation;
	}
	else
	{
		const void* data = freeAndAdvanceToNewBlock();
		HK_ASSERT( 0xf06576df, (const char*)m_currentByteLocation + thisElemSize <= m_currentBlock->end() );
		return data;
	}
}


//////////////////////////////////////////////////////////////////////////
//
// Modify Iterator
//
//////////////////////////////////////////////////////////////////////////

void hkBlockStreamBase::Modifier::setToRange( const Range* HK_RESTRICT range )
{
	Reader::setToRange(range);
	HK_ON_SPU( m_writeBackBlockStartOffset = range->m_startByteLocation );
}

void hkBlockStreamBase::Modifier::setToStartOfStream( hkBlockStreamBase::Stream::Allocator* HK_RESTRICT allocator, hkBlockStreamBase::Stream* HK_RESTRICT stream, hkBlockStreamBase::Stream* streamPpu )
{
	// 	HK_ASSERT( 0xf0345dfe, !blockStream->m_isLocked );	// enable this, but it requires a finalize()
	// 	HK_ON_DEBUG( blockStream->m_isLocked = true );
	Reader::setToStartOfStream(stream);
	HK_ON_SPU(m_writeBackBlockStartOffset = 0);
}

void* hkBlockStreamBase::Modifier::advanceAndAccessNext(int thisElemSize)
{
	HK_ASSERT( 0xf034df43, m_currentBlock );
	HK_ASSERT( 0xf0347687, m_numElementsToReadInThisBlock > 0 );

	const char* nextLocation = hkAddByteOffsetConst( (const char*)m_currentByteLocation, thisElemSize );	// get this here as this is the most common code path and allows the compiler to optimize better
	m_numElementsToReadInThisBlock		= m_numElementsToReadInThisBlock-1;

	if( m_numElementsToReadInThisBlock > 0 )
	{
		HK_ASSERT( 0xf06576df, nextLocation <= m_currentBlock->end() );
		m_currentByteLocation = nextLocation;
		return const_cast<void*>((void*)nextLocation);
	}
	else
	{
		const void* data = advanceToNewBlock(  );	// there is a special spu implementation
		HK_ASSERT( 0xf06576df, (const char*)m_currentByteLocation + thisElemSize <= m_currentBlock->end() );
		return const_cast<void*>(data);
	}
}


//////////////////////////////////////////////////////////////////////////
//
// Marker
//
//////////////////////////////////////////////////////////////////////////

HK_FORCE_INLINE void hkBlockStreamBase::Range::setStartPoint(const hkBlockStreamBase::Writer* HK_RESTRICT it)
{
	Block* currentBlock						= it->m_currentBlock;
	Block::CountType startByteLocation		= Block::CountType(it->m_currentByteLocation);
	Block::CountType startBlockNumElements	= Block::CountType(it->m_currentBlockNumElems);
	int numElements							= -(it->m_blockStream->m_numTotalElements + it->m_currentBlockNumElems);

	HK_ASSERT(0x5a000196, startBlockNumElements < 0x1000 );

	m_startBlock			= it->spuToPpu(currentBlock);
	m_startByteLocation		= startByteLocation;
	m_startBlockNumElements	= startBlockNumElements;
	m_numElements			= numElements;
	m_next					= HK_NULL;
}

HK_FORCE_INLINE void hkBlockStreamBase::Range::setStartPointExclusive(hkBlockStreamBase::Writer* HK_RESTRICT it)
{
	it->setCurrentBlockFull();
	setStartPoint(it);
}

HK_FORCE_INLINE void hkBlockStreamBase::Range::setEndPointExclusive(hkBlockStreamBase::Writer* HK_RESTRICT it)
{
	it->setCurrentBlockFull();
	setEndPoint(it);
}

HK_FORCE_INLINE void hkBlockStreamBase::Range::setEntireStream(hkBlockStreamBase::Stream* HK_RESTRICT stream )
{
	bool empty = stream->isEmpty();
	m_startBlock = (!empty)? stream->beginRw(): HK_NULL;
	m_startByteLocation = 0;
	m_startBlockNumElements = Block::CountType(m_startBlock->m_numElements);
	m_numElements = stream->getTotalNumElems();
	m_next = HK_NULL;
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
