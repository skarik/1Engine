
#ifndef _TERRAIN_BLOCK_TRACKER_STRUCT_H_
#define _TERRAIN_BLOCK_TRACKER_STRUCT_H_

//class CBoob;
//struct subblock16;
//struct subblock8;
//struct BlockInfo;
struct BlockTrackInfo;
//struct TrackInfo;
namespace Terrain
{
	class TPayload;
	class TSector;
};

#include "core/types/types.h"
#include "core/math/Vector3d.h"
#include "BlockData.h"
//#include "TerrainSector.h"
//#include "world/BlockTypes.h"

struct BlockTrackInfo
{
	// Tracking information
	Vector3d			center;
	uint32_t			subdivision;
	Terrain::TSector*	sector;
	char				bindex;
	// Indexing information
	uint16_t			pos_x;
	uint16_t			pos_y;
	uint16_t			pos_z;
	// If valid
	bool				valid;
	// Block information
	Terrain::terra_b*	pBlock;
	Terrain::terra_b	block;
	// Default set
	BlockTrackInfo ( void ) 
		: sector(NULL), valid(false), pBlock(NULL)
	{
		;
	}
};

/*
struct BlockInfo
{
	CBoob*		pBoob;
	char		b16index;
	subblock16*	pBlock16;
	char		b8index;
	subblock8*	pBlock8;
	short		b1index;
	Terrain::terra_t*	pBlock;
	Terrain::terra_t	block;
};
*/

/*struct BlockTrackInfo
{
	// Tracking information
	Vector3d			center;
	uint32_t			subdivision;
	Terrain::Sector*	sector;
	char				bindex;
	// Indexing information
	uint16_t			pos_x;
	uint16_t			pos_y;
	uint16_t			pos_z;
	// If valid
	bool				valid;
	// Block information
	Terrain::terra_b*	pBlock;
	Terrain::terra_b	block;
	// Default set
	BlockTrackInfo ( void ) 
		: sector(NULL), valid(false), pBlock(NULL)
	{
		;
	}
};*/
/*struct TrackInfo
{
	Vector3d			center;
	uint32_t			subdivision;
	Terrain::Sector*	sector;
	char				bindex;
};*/

#endif