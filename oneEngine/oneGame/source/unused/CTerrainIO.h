
#ifndef _C_TERRAIN_IO_H_
#define _C_TERRAIN_IO_H_

#include "../VoxelTerrain.h"
#include "../data/GamePayload.h"
#include "../data/Node.h"

/*
VERSION 7
unsigned short version
unsigned char terrain
unsigned char biome
uint32_t foliage_number
uint32_t component_number
uint32_t grass_number

terra_t terrain_data [32768]

data*{
	unsigned short type
	unsigned short index
	char data [60]
}
// end file

// Data overview (max size)
data*{
	unsigned char type (0 for foliage, 1 for component, 2 for grass)
								1
	unsigned short index		2
	float x,y,z					12
	float xrot,yrot,zrot		12
	float xscal,yscal,zscal		12
	uint64_t userdata			8
	char b16index				1
	char b8index				1
	short bindex				2
	char padding [13]
} //64 bytes


// Sub data types
data*{
	type = 0
	unsigned short foliage_index
	float x, y, z
	char branch_data [49]
}
data*{
	type = 1
	unsigned short component_index
	float x,y,z
	float xrot,yrot,zrot
	float xscal,yscal,zscal
	uint64_t userdata
	char b16index
	char b8index
	short bindex
	char padding [13]
}
data*{
	type = 2
	unsigned short type
	float x,y,z
	char padding [49]
}
*/


// IO Class for terrain file read/write
// In a separate class to easily change the format
class CTerrainIO
{

public:
	explicit CTerrainIO ( CVoxelTerrain* pTargetTerrain );

	//void GetBoobData ( CBoob * pBoob, RangeVector const& position, bool generateTerrain );
	//void SaveBoobData 
public:
	/*void WriteBoobToFile ( CBoob * pBoob, CBinaryFile * pFile );
	void WriteMetaboobToFile ( CMetaboob * pBoob, CBinaryFile * pFile );

	void ReadBoobFromFile ( CBoob * pBoob, CBinaryFile * pFile );*/

	struct filestruct_t {
		unsigned short	version;
		unsigned char	terra_type;
		unsigned char	biome_type;

		float	elevation;

		uint32_t foliage_number;
		uint32_t component_number;
		uint32_t grass_number;
		uint32_t npc_number;

		char padding [8];
	};
private:
	CVoxelTerrain* terrain;

	typedef union {
		struct {
			unsigned short i_type;
			unsigned short i_index;
			char raw_data [60];
		};
		struct foliage_t {
			short type;
			unsigned short foliage_index;
			float x, y, z;
			char branch_data [48];
		} foliage;
		struct component_t {
			short type;
			unsigned short component_index;
			float x, y, z;
			float xrot, yrot, zrot;
			float xscal, yscal, zscal;
			uint64_t userdata;
			char b16index;
			char b8index;
			short bindex;
			char padding [12];
		} component;
		struct grass_t {
			short type;
			unsigned short grass_type;
			float x, y, z;
			char padding [48];
		} grass;
	} propdata_t;

};


#endif