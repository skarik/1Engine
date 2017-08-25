
#ifndef _AFTER_TERRAIN_DATA_NODE_DATA_PAYLOAD_H_
#define _AFTER_TERRAIN_DATA_NODE_DATA_PAYLOAD_H_

#include <vector>
#include <atomic>

#include "core/types/types.h"
#include "after/types/terrain/BlockData.h"
#include "after/types/terrain/Grass.h"
#include "after/terrain/data/GamePayload.h"

// Define macros
#define BLOCK_COUNT 32768

// Class Prototypes
class CFoliage;
class CTerrainProp;

namespace Terrain
{
	//=========================================//
	//	class CTtPayload - See the Titty Payload
	//
	// Container for actual terrain data
	// the key is that there can be overlap with the payloads. the depth is prioritized using LoD
	class CTtPayload
	{
	public:
		//	LOD of the payload data.
		// 0 for highest detail, log2(size) for lowest
		uint8_t lod;
		//	Payload block data
		terra_b* data;
		
		// following are only valid if lod is 0
		GamePayload		*gamedata_0;
		// following is valid regardless of the lod
		GamePayload		*gamedata_f;
	};
	typedef CTtPayload Payload;

	//	PayloadConstruct ( new payload )
	// Initializes the values in a new payload
	FORCE_INLINE void PayloadConstruct ( CTtPayload& payload )
	{
		payload.lod = 0;
		payload.data	= NULL;
		payload.gamedata_0	= NULL;
		payload.gamedata_f	= NULL;
	}

	//=========================================//
	//	class CTtNode
	//
	// Container for the terrain Octree layout.
	// A CTtNode doesn't need to have data, but it usually does.
	class CTtNode
	{
	private:
		std::atomic_flag	m_ready;
		std::atomic<uchar>	m_readers;
	public:
		CTtNode* children [8];
		CTtNode* parent;
		CTtPayload* payload;
		bool	linkstyle;
	};
	typedef CTtNode Node;

	//	NodeConstruct ( new node )
	// Initializes the values in a new node to meaninful null values.
	FORCE_INLINE void NodeConstruct ( CTtNode& node )
	{
		for ( uint i = 0; i < 8; ++i ) node.children[i] = NULL;
		node.parent = NULL;
		node.payload = NULL;
		node.linkstyle = false;
	}
}

#endif//_AFTER_TERRAIN_DATA_NODE_DATA_PAYLOAD_H_