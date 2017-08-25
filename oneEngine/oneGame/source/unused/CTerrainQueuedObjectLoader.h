
#ifndef _C_TERRAIN_QUEUED_OBJECT_LOADER_H_
#define _C_TERRAIN_QUEUED_OBJECT_LOADER_H_

#include <vector>
#include "TerrainGameSector.h"
#include "RangeVector.h"
#include "boost/thread.hpp"

namespace Terrain
{
	struct queuerequest_Grass
	{
		RangeVector		index;
		Terrain::SectorGameData*	target;
		unsigned short grass_type;
		float x, y, z;
	};
	struct queuerequest_Foliage
	{
		RangeVector		index;
		Terrain::SectorGameData*	target;
		unsigned short	foliage_index;
		Vector3d		position;
		char			userdata [48];
	};
	struct queuerequest_Component
	{
		RangeVector		index;
		Terrain::SectorGameData*	target;
		unsigned short	component_index;
		Vector3d		position;
		Vector3d		rotation;
		Vector3d		scale;
		uint64_t		userdata;
		uint8_t		block_x;
		uint8_t		block_y;
		uint8_t		block_z;
		uint8_t		block_w;
	};

	class CQueuedObjectLoader
	{

	public:
		void LoadGrass ( const queuerequest_Grass& n_request ) {
			boost::mutex::scoped_lock addLock( m_lock );
			bool valid = true;
#ifdef _ENGINE_DEBUG
			for ( uint i = 0; i < m_grassqueue.size(); ++i ) {
				if ( m_grassqueue[i].target == n_request.target ) {
					if ( Vector3d( m_grassqueue[i].x, m_grassqueue[i].y, m_grassqueue[i].z ) == Vector3d( n_request.x, n_request.y, n_request.z ) ) {
						valid = false;
						break;
					}
				}
			}
#endif//_ENGIN_DEBUG

			if ( !valid ) {
				Debug::Console->PrintError( "BAD INFO\n" );
			}
			else {
				m_grassqueue.push_back( n_request );
			}
		}
		void LoadFoliage ( const queuerequest_Foliage& n_request ) {
			boost::mutex::scoped_lock addLock( m_lock );
			m_foliagequeue.push_back( n_request );
		}
		void LoadComponent ( const queuerequest_Component& n_request ) {
			boost::mutex::scoped_lock addLock( m_lock );
			m_componentqueue.push_back( n_request );
		}

		boost::mutex m_lock;
	public:
		std::vector<queuerequest_Grass>		m_grassqueue;
		std::vector<queuerequest_Foliage>	m_foliagequeue;
		std::vector<queuerequest_Component>	m_componentqueue;
	};
};

#endif//_C_TERRAIN_QUEUED_OBJECT_LOADER_H_