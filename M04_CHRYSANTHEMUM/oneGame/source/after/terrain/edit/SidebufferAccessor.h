
#ifndef _AFTER_TERRAIN_SIDEBUFFER_ACCESSOR_
#define _AFTER_TERRAIN_SIDEBUFFER_ACCESSOR_

#include "../VoxelTerrain.h"

#include "after/terrain/VoxelTerrain.h"
#include "after/terrain/data/Node.h"
#include "after/types/WorldVector.h"

// Sidebuffer accessor
namespace Terrain
{
	/*class SidebufferAccessor
	{
	public:
		explicit	SidebufferAccessor ( CVoxelTerrain* n_terrain, Terrain::Payload* n_sideBuffer )
			: m_terrain( n_terrain ), m_sideBuffer( n_sideBuffer )
		{
			;
		}

		// Sets data at side-buffer position
		void		SetDataAtSidebufferPosition ( const uint64_t newData, const RangeVector& absoluteIndex ) {
			m_terrain->SetDataAtSidebufferPosition( m_sideBuffer, newData, absoluteIndex );
		}
		// Gets data at side-buffer position
		void		GetDataAtSidebufferPosition ( uint64_t& o_data, const RangeVector& absoluteIndex ) {
			m_terrain->GetDataAtSidebufferPosition( m_sideBuffer, o_data, absoluteIndex );
		}

	private:
		CVoxelTerrain*		m_terrain;
		Terrain::Payload*	m_sideBuffer;
	};*/

	class SidebufferAccessor
	{
	public:
		explicit	SidebufferAccessor ( VoxelTerrain* n_terrain, Terrain::terra_b* n_sideBuffer, const WorldVector& n_buffersize )
			: m_terrain( n_terrain ), m_sideBuffer( n_sideBuffer ),
			m_size( n_buffersize )
		{
			;
		}

		// Sets data at given side-buffer position
		void		SetData ( const uint64_t i_data, const WorldVector& index ) {
			m_sideBuffer[index.x + index.y*m_size.x + index.z*m_size.x*m_size.y].raw = i_data;
		}

		// Gets data at given side-buffer position
		void		GetData ( uint64_t& o_data, const WorldVector& index ) {
			o_data = m_sideBuffer[index.x + index.y*m_size.x + index.z*m_size.x*m_size.y].raw;
		}

		// Returns the size of the buffer
		const WorldVector&	GetSize ( void ) {
			return m_size;
		}

	private:
		CVoxelTerrain*		m_terrain;
		Terrain::terra_b*	m_sideBuffer;
		WorldVector			m_size;
	};
};

#endif//_AFTER_TERRAIN_SIDEBUFFER_ACCESSOR_