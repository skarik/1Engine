#ifndef RENDERER_STATE_RAII_HELPERS_2_H_
#define RENDERER_STATE_RAII_HELPERS_2_H_

#include "core/types.h"
#include "gpuw/Buffers.h"

/*class rrGroupedConstantView
{
public:
	uint64		offset = 0;
	uint64		size = 0;
};

class rrGroupedConstants
{
public:
	rrGroupedConstantView	Allocate ( uint64 size )
	{
		// Create new view
		rrGroupedConstantView view;
		view.offset = m_size;
		view.size = size;
		// Create new size, but align view so it's on 256-byte border
		uint64 m_size = (m_size + size + 0xFF) & ~0xFF;
		// Return the view we now have.
		return view;
	}

	void					Upload ( const rrGroupedConstantView& view, void* data, uint64 size )
	{
		//m_buffer.map
		void* buffer_subset = m_buffer.mapSubregion(view.offset, view.size);
		if (buffer_subset)
		{

		}
	}

private:
	uint64				m_size = 0;
	gpu::Buffer			m_buffer;

};*/

class rrSingleFrameConstantBufferPool
{
public:
	~rrSingleFrameConstantBufferPool ( void )
	{
		for ( Entry& entry : m_entries )
		{
			entry.buffer.free( NULL );
		}
	}

	gpu::Buffer				Allocate ( uint64 size )
	{
		// First search for available buffers.
		for ( Entry& entry : m_entries )
		{
			if ( entry.frame != m_currentFrame
				&& entry.size >= size )
			{
				// Update that entry with current frame and return it.
				entry.frame = m_currentFrame;
				return entry.buffer;
			}
		}

		// Nothing is available, allocate a new one
		const uint64 newSize = (size + 0xFF) & ~0xFF;
		gpu::Buffer newBuffer;
		newBuffer.initAsConstantBuffer( NULL, newSize, gpu::kMemoryTypeHeap );

		// Add it to the current list of entries
		m_entries.push_back( {m_currentFrame, newSize, newBuffer} );

		return newBuffer;
	}

	int					m_currentFrame = 0;

private:
	struct Entry
	{
		int				frame = 0;
		uint64			size = 0;
		gpu::Buffer		buffer;
	};
	std::vector<Entry>	m_entries;
};

#endif//RENDERER_STATE_RAII_HELPERS_2_H_
