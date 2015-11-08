
// Includes
#include "CTerrainIO.h"
#include "CGameSettings.h"
#include "CTreeBase.h"

#include <sstream>
using std::stringstream;

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "zlib.h"
#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif
#define CHUNK 131072

using std::cout;
using std::endl;
using std::max;
using std::min;

// Constructor
CTerrainIO::CTerrainIO( CVoxelTerrain * pTargetTerrain )
{
	terrain = pTargetTerrain;
}

// IO
void CTerrainIO::WriteBoobToFile ( CBoob * pBoob, CBinaryFile * pFile )
{
	static char compress_buf [144192];
	unsigned long compress_size = 144192;

	FILE* cfile = pFile->GetFILE();

	// First create the primary structure
	filestruct_t fs;
	fs.version	= pBoob->version;
	fs.terra_type	= pBoob->terrain;
	fs.biome_type	= pBoob->biome;
	fs.elevation	= pBoob->elevation;
	fs.foliage_number	= (uint32_t)pBoob->v_foliage.size();
	fs.component_number	= (uint32_t)pBoob->v_component.size();
	fs.grass_number		= (uint32_t)pBoob->v_grass.size();
	fs.npc_number		= 0;//(uint32_t)pBoob->v_npc.size();

	// Create the prop data to save
	uint32_t datacount = fs.foliage_number + fs.grass_number + fs.component_number;
	propdata_t* propdata = new propdata_t [datacount];
	memset( propdata, 0, sizeof(propdata_t)*datacount );

	uint32_t k = 0;
	for ( unsigned int i = 0; i < fs.foliage_number; i++ )
	{
		propdata[k].foliage.type = 0;
		propdata[k].foliage.foliage_index = terrain->GetFoliageType( pBoob->v_foliage[i] );
		propdata[k].foliage.x = pBoob->v_foliage[i]->transform.position.x;
		propdata[k].foliage.y = pBoob->v_foliage[i]->transform.position.y;
		propdata[k].foliage.z = pBoob->v_foliage[i]->transform.position.z;
		char data [48];
		((CTreeBase*)(pBoob->v_foliage[i]))->GetToggle( data );
		memcpy( propdata[k].foliage.branch_data, data, 48 );

		++k;
	}
	for ( unsigned int i = 0; i < fs.component_number; i++ )
	{
		propdata[k].component.type = 1;
		propdata[k].component.component_index = terrain->GetComponentType( pBoob->v_component[i] );
		propdata[k].component.x = pBoob->v_component[i]->transform.position.x;
		propdata[k].component.y = pBoob->v_component[i]->transform.position.y;
		propdata[k].component.z = pBoob->v_component[i]->transform.position.z;
		Vector3d angles = pBoob->v_component[i]->transform.rotation.getEulerAngles();
		propdata[k].component.xrot = angles.x;
		propdata[k].component.yrot = angles.y;
		propdata[k].component.zrot = angles.z;
		propdata[k].component.xscal = pBoob->v_component[i]->transform.scale.x;
		propdata[k].component.yscal = pBoob->v_component[i]->transform.scale.y;
		propdata[k].component.zscal = pBoob->v_component[i]->transform.scale.z;
		propdata[k].component.userdata = pBoob->v_component[i]->GetUserdata();
		/*BlockInfo tempBlockInfo = pBoob->v_component[i]->GetBlockInfo();
		propdata[k].component.b16index = tempBlockInfo.b16index;
		propdata[k].component.b8index = tempBlockInfo.b8index;
		propdata[k].component.bindex = tempBlockInfo.b1index;*/
		throw std::exception();

		++k;
	}
	for ( unsigned int i = 0; i < fs.grass_number; i++ )
	{
		propdata[k].grass.type = 2;
		propdata[k].grass.grass_type = pBoob->v_grass[i].type;
		propdata[k].grass.x = pBoob->v_grass[i].position.x;
		propdata[k].grass.y = pBoob->v_grass[i].position.y;
		propdata[k].grass.z = pBoob->v_grass[i].position.z;

		++k;
	}

	// == Begin writing data ==
	fwrite( &fs, sizeof( filestruct_t ), 1, cfile );
	// Write out in the terrain data
	{
		// COMPRESS THE DATA
		int z_result = compress( (uchar*)compress_buf, &compress_size, (uchar*)pBoob->data, 4*32768 );
		switch( z_result )
		{
		case Z_OK:
			//printf("***** SUCCESS! *****\n");
			break;
		case Z_MEM_ERROR:
			printf("out of memory\n");
			exit(1);    // quit.
			break;
		case Z_BUF_ERROR:
			printf("output buffer wasn't large enough!\n");
			exit(1);    // quit.
			break;
		}
		fwrite( &compress_size, sizeof( unsigned long ), 1, cfile );
		fwrite( compress_buf, compress_size, 1, cfile );
	}
	// Write out the component data
	fwrite( propdata, sizeof( propdata_t ), datacount, cfile );
	// Write out the NPC data
	fwrite( &(pBoob->v_npc[0]), sizeof(uint64_t), fs.npc_number, cfile );

	// Free prop data
	delete [] propdata;
	propdata = NULL;
}
void CTerrainIO::WriteMetaboobToFile ( CMetaboob * pBoob, CBinaryFile * pFile )
{
	static char compress_buf [144192];
	unsigned long compress_size = 144192;

	FILE* cfile = pFile->GetFILE();

	// First create the primary structure
	filestruct_t fs;
	fs.version	= pBoob->version;
	fs.terra_type	= pBoob->terrain;
	fs.biome_type	= pBoob->biome;
	fs.elevation	= pBoob->elevation;
	fs.foliage_number	= (uint32_t)pBoob->v_foliageIO.size();
	fs.component_number	= (uint32_t)pBoob->v_componentIO.size();
	fs.grass_number		= (uint32_t)pBoob->v_grass.size();
	fs.npc_number		= 0;//(uint32_t)pBoob->v_npc.size();

	// Create the prop data to save
	uint32_t datacount = fs.foliage_number + fs.grass_number + fs.component_number;
	propdata_t* propdata = new propdata_t [datacount];
	memset( propdata, 0, sizeof(propdata_t)*datacount );

	uint32_t k = 0;
	for ( unsigned int i = 0; i < fs.foliage_number; i++ )
	{
		propdata[k].foliage.type = 0;
		propdata[k].foliage.foliage_index = pBoob->v_foliageIO[i].foliage_index;
		propdata[k].foliage.x = pBoob->v_foliageIO[i].position.x;
		propdata[k].foliage.y = pBoob->v_foliageIO[i].position.y;
		propdata[k].foliage.z = pBoob->v_foliageIO[i].position.z;
		char* data = pBoob->v_foliageIO[i].userdata;
		memcpy( propdata[k].foliage.branch_data, data, 48  );
		++k;
	}
	for ( unsigned int i = 0; i < fs.component_number; i++ )
	{
		propdata[k].component.type = 1;
		propdata[k].component.component_index = pBoob->v_componentIO[i].component_index;
		propdata[k].component.x = pBoob->v_componentIO[i].position.x;
		propdata[k].component.y = pBoob->v_componentIO[i].position.y;
		propdata[k].component.z = pBoob->v_componentIO[i].position.z;
		propdata[k].component.xrot = pBoob->v_componentIO[i].rotation.x;
		propdata[k].component.yrot = pBoob->v_componentIO[i].rotation.y;
		propdata[k].component.zrot = pBoob->v_componentIO[i].rotation.z;
		propdata[k].component.xscal = pBoob->v_componentIO[i].scaling.x;
		propdata[k].component.yscal = pBoob->v_componentIO[i].scaling.y;
		propdata[k].component.zscal = pBoob->v_componentIO[i].scaling.z;
		propdata[k].component.userdata = pBoob->v_componentIO[i].data;
		propdata[k].component.b16index = pBoob->v_componentIO[i].b16index;
		propdata[k].component.b8index = pBoob->v_componentIO[i].b8index;
		propdata[k].component.bindex = pBoob->v_componentIO[i].bindex;

		++k;
	}
	for ( unsigned int i = 0; i < fs.grass_number; i++ )
	{
		propdata[k].grass.type = 2;
		propdata[k].grass.grass_type = pBoob->v_grass[i].type;
		propdata[k].grass.x = pBoob->v_grass[i].position.x;
		propdata[k].grass.y = pBoob->v_grass[i].position.y;
		propdata[k].grass.z = pBoob->v_grass[i].position.z;

		++k;
	}

	// == Begin writing data ==
	fwrite( &fs, sizeof( filestruct_t ), 1, cfile );
	// Write out in the terrain data
	{
		// COMPRESS THE DATA
		int z_result = compress( (uchar*)compress_buf, &compress_size, (uchar*)pBoob->data, 4*32768 );
		switch( z_result )
		{
		case Z_OK:
			//printf("***** SUCCESS! *****\n");
			break;
		case Z_MEM_ERROR:
			printf("out of memory\n");
			exit(1);    // quit.
			break;
		case Z_BUF_ERROR:
			printf("output buffer wasn't large enough!\n");
			exit(1);    // quit.
			break;
		}
		fwrite( &compress_size, sizeof( unsigned long ), 1, cfile );
		fwrite( compress_buf, compress_size, 1, cfile );
	}
	// Write out the component data
	fwrite( propdata, sizeof( propdata_t ), datacount, cfile );
	// Write out the NPC data
	fwrite( &(pBoob->v_npc[0]), sizeof(uint64_t), fs.npc_number, cfile );

	// Free prop data
	delete [] propdata;
	propdata = NULL;
}

void CTerrainIO::ReadBoobFromFile ( CBoob * pBoob, CBinaryFile * pFile )
{
	static char compress_buf [144192];
	unsigned long compress_size;

	FILE* cfile = pFile->GetFILE();

	// Load in the primary structure
	filestruct_t	fs;
	fread( &fs, sizeof( filestruct_t ), 1, cfile );
	if ( fs.version < TERRA_SYSTEM_VERSION-1 )
	{
		cout << "Attempted to load outdated terrain format (cannot convert!)" << endl;
		return;
	}

	// Set terra and biome data
	pBoob->biome	= fs.biome_type;
	pBoob->terrain	= fs.terra_type;
	pBoob->elevation= fs.elevation;

	// Load in the terrain data
	if ( fs.version >= 8 )
	{
		// decompress the terrain data
		fread( &compress_size, sizeof( unsigned long ), 1, cfile );
		fread( compress_buf, compress_size, 1, cfile );
		// DECOMPRESS THE DATA
		unsigned long finalDataSize = 4*32768;
		int z_result = uncompress( (uchar*)pBoob->data, &finalDataSize, (uchar*)compress_buf, compress_size );
		switch( z_result )
		{
		case Z_OK:
			//printf("***** SUCCESS! *****\n");
			break;
		case Z_MEM_ERROR:
			printf("out of memory\n");
			exit(1);    // quit.
			break;
		case Z_BUF_ERROR:
			printf("output buffer wasn't large enough!\n");
			exit(1);    // quit.
			break;
		}
		if ( finalDataSize != 4*32768 )
		{
			exit(1);
		}
	}
	else if ( fs.version == 7 )
	{
		fread( pBoob->data, 4*32768, 1, cfile );
	}
	// Boob now has blocks
	pBoob->hasBlockData = true;
	pBoob->bitinfo[3] = true;

	if ( pBoob->loadPropData )
	{
		// Load in the component data
		uint32_t datacount = fs.foliage_number + fs.grass_number + fs.component_number;
		propdata_t* propdata = new propdata_t [datacount];
		fread( propdata, sizeof( propdata_t ), datacount, cfile );

		// Loop through all the components and load their objects
		for ( uint32_t i = 0; i < datacount; ++i )
		{
			if ( propdata[i].i_type == 0 )
			{
				// Foliage load
				CVoxelTerrain::TerraFoiliage	newFoliageRequest;
				newFoliageRequest.foliage_index	= propdata[i].foliage.foliage_index;
				newFoliageRequest.position	= Vector3d( propdata[i].foliage.x, propdata[i].foliage.y, propdata[i].foliage.z );
				memcpy( newFoliageRequest.userdata, propdata[i].foliage.branch_data, 48 );
				newFoliageRequest.myBoob	= pBoob;
				terrain->vFoliageQueue.push_back( newFoliageRequest );
			}
			else if ( propdata[i].i_type == 1 )
			{
				// Component load
				CVoxelTerrain::TerraProp	newComponentRequest;
				newComponentRequest.component_index	= propdata[i].component.component_index;
				newComponentRequest.position = Vector3d( propdata[i].component.x,propdata[i].component.y,propdata[i].component.z );
				newComponentRequest.rotation = Vector3d( propdata[i].component.xrot,propdata[i].component.yrot,propdata[i].component.zrot );
				newComponentRequest.scaling  = Vector3d( propdata[i].component.xscal,propdata[i].component.yscal,propdata[i].component.zscal );
				newComponentRequest.m_block.pBoob = pBoob;
				newComponentRequest.m_block.b16index	= propdata[i].component.b16index;
				newComponentRequest.m_block.b8index		= propdata[i].component.b8index;
				newComponentRequest.m_block.b1index		= propdata[i].component.bindex;
				newComponentRequest.userdata = propdata[i].component.userdata;
				terrain->vComponentQueue.push_back( newComponentRequest );
			}
			else if ( propdata[i].i_type == 2 )
			{
				// Grass load
				sTerraGrass newGrass;
				newGrass.type = propdata[i].grass.grass_type;
				newGrass.position.x = propdata[i].grass.x;
				newGrass.position.y = propdata[i].grass.y;
				newGrass.position.z = propdata[i].grass.z;
				pBoob->v_grass.push_back( newGrass );
			}
			else
			{
				cout << "Invalid terrain prop type " << propdata[i].i_type << " loaded. Data corruption." << endl;
			}
		}

		// Delete prop data
		delete [] propdata;
		propdata = NULL;

		// Load in the NPCs
		pBoob->v_npc.resize( fs.npc_number );
		fread( &(pBoob->v_npc[0]), sizeof(uint64_t),fs.npc_number, cfile );

		// Boob now has props (and NPCs too)
		pBoob->hasPropData = true;
	}
}
