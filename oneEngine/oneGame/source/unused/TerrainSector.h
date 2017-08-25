
#ifndef _C_TERRAIN_SECTOR_H_
#define _C_TERRAIN_SECTOR_H_

#include "boost/thread.hpp"
#include "world/BlockTypes.h"

#include <atomic>

namespace Terrain
{
	typedef boost::unique_lock<boost::shared_mutex> write_lock;
	typedef boost::shared_lock<boost::shared_mutex> read_lock;

	struct Sector
	{
	private:
		std::atomic_flag	m_ready;
		std::atomic<uchar>	m_readers;
	public:
		//boost::mutex	lock;
		//boost::mutex	write_lock;
		//boost::shared_mutex	lock;

		// 32-bit data. 8 parts. 9th is the parent.
		//uint32_t	links [9];
		Sector*		leafs [9];
		terra_b		info [8];

		// Flags. If true, corresponding index is data. If false, corresponding index is a pointer to another sector.
		bool		data [8];

		// Link style. Defaults to false.
		// False indicates normal occtree linking.
		// True indicates "lesbian pair" linking: four sectors all on the same plane.
		bool			linkstyle;

		//char __padding [3];

		// Constructor
		explicit Sector ( void ) :
			linkstyle(false)
		{
			for ( int i = 0; i < 8; ++i ) {
				//links[i]	= NULL;
				leafs[i]	= NULL;
				data[i]		= true;
				info[i].raw = 0;
			}
			//links[8] = NULL;
			leafs[8] = NULL;

			m_ready.clear();
			m_readers.store(0);
		}

		void BeginWrite ( void )
		{
			// while( m_ready == false ) {}
			//while ( m_ready.test_and_set() ) {}
			uchar x = 255;
			do {
				if ( !m_ready.test_and_set() ) {
					x = m_readers.load( std::memory_order_relaxed );
					if ( x > 0 ) {
						m_ready.clear();
					}
				}
			} while ( x > 0 );
		}
		void EndWrite ( void )
		{
			// m_ready = true;
			m_ready.clear();
		}

		void BeginRead ( void )
		{
			// while( m_ready == false ) {}
			//while ( m_ready.test_and_set() ) {}
			m_readers++;
			//m_ready.clear();
		}
		void EndRead ( void )
		{
#ifdef _ENGINE_DEBUG
			/*if ( m_readers.load() == 0 ) {
				throw std::exception();
			}*/
#endif
			m_readers--;
		}
	};
}

#endif//_C_TERRAIN_SECTOR_H_