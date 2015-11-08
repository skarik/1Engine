
#include "CVoxelTerrain.h"
#include "CGameSettings.h"
#include "CTreeBase.h"
#include "CTerrainIO.h"
#include <sstream>
using std::stringstream;
using std::cout;
using std::endl;

/*
filenames:
x_y_z
binary:
VERSION 0
unsigned short version
data*{
	unsigned char [512] terrain_data
	unsigned char [512] water_data
}
unsigned char terrain
unsigned char biome
bufferbufferbuffer

VERSION 1 (THIS IS NOT EVEN FUCKING CORRECT)
unsigned short version
data*{
	unsigned char terrain_data
	unsigned char water_data
}
unsigned char terrain
unsigned char biome
data*{
	unsigned short big_objects
}

VERSION 2
unsigned short version
data*{
	unsigned char terrain_data
}
data*{
	unsigned char water_data
}
unsigned char terrain
unsigned char biome
data*{
	unsigned short big_objects
	unsigned long little_objects
	unsigned int medium_objects
}

VERSION 3
unsigned short version
data*{
	unsigned char terrain_data
}
data*{
	unsigned char water_data
}
unsigned char terrain
unsigned char biome
uint32_t foliage_number
data*{
	unsigned short foliage_index
	float x, y, z
	uint64_t userbitmask
}


VERSION 4
unsigned short version
data*{
	unsigned char terrain_data
}
data*{
	unsigned char water_data
}
unsigned char terrain
unsigned char biome
uint32_t foliage_number
data*{
	unsigned short foliage_index
	float x, y, z
	uint64_t userbitmask
}
uint32_t component_number
data*{
	unsigned short component_index
	float x,y,z
	float xrot,yrot,zrot
	float xscal,yscal,zscal
	uint64_t userdata
	char b16index
	char b8index
	short bindex
}

VERSION 5
unsigned short version
data*{
	unsigned char terrain_data
}
data*{
	unsigned char water_data
}
unsigned char terrain
unsigned char biome
uint32_t foliage_number
data*{
	unsigned short foliage_index
	float x, y, z
	uint64_t userbitmask
}
uint32_t component_number
data*{
	unsigned short component_index
	float x,y,z
	float xrot,yrot,zrot
	float xscal,yscal,zscal
	uint64_t userdata
	char b16index
	char b8index
	short bindex
}
uint32_t grass_number
data*{
	unsigned short type
	float x,y,z
}

VERSION 6
unsigned short version
data*{
	unsigned char terrain_data
}
data*{
	unsigned char water_data
}
unsigned char terrain
unsigned char biome
uint32_t foliage_number
data*{
	unsigned short foliage_index
	float x, y, z
	string userdata
}
uint32_t component_number
data*{
	unsigned short component_index
	float x,y,z
	float xrot,yrot,zrot
	float xscal,yscal,zscal
	uint64_t userdata
	char b16index
	char b8index
	short bindex
}
uint32_t grass_number
data*{
	unsigned short type
	float x,y,z
}

*/

void CVoxelTerrain::GetBoobData ( CBoob * pBoob, RangeVector const& position, bool generateTerrain )
{
	if (( pBoob->hasBlockData )&&( pBoob->hasPropData ))
	{
		//cout << "Warning: Attempting to get data on a boob that already has data." << endl;
		Debug::Console->PrintWarning( "Warning: Attempting to get data on a boob that already has data.\n" );
		//return;
	}
	CBinaryFile file;
	/*stringstream tempStream;
	string sFilename;
	tempStream << CGameSettings::Active()->GetTerrainSaveDir() << "\\";
	tempStream << position.x << "_" << position.y << "_" << position.z;
	sFilename = tempStream.str();*/
	char filename [256];
	sprintf( filename, "%s\\%d_%d_%d\0", CGameSettings::Active()->GetTerrainSaveDir().c_str(), position.x, position.y, position.z );

	if ( file.Exists( filename ) )	
	{
		if ( false && (file.GetFilesize() < 64000) )	// Check for proper size
		{
			cout << " Area " << filename << " is corrupted! Regenerating." << endl;
			if ( generateTerrain )
			{
				remove( filename );
				AddGenerationRequest( position );
			}
		}
		else
		{
			file.Open( filename, CBinaryFile::IO_READ );

			if ( file.IsOpen() )
			{
				// Lock the terrain while updating
				mutex::scoped_lock local_lock( mtGaurd );

				if ( pBoob->data == NULL ) {
					Debug::Console->PrintError( "Null data block found! This should NEVER happen...\n" );
				}
				else {
					io->ReadBoobFromFile( pBoob, &file );
					if (( !pBoob->hasBlockData )||( pBoob->loadPropData && !pBoob->hasPropData ))
					{
						Debug::Console->PrintError( "Did not load valid values!" );
					}
				}
				file.Close();
			}
			else
			{
				Debug::Console->PrintError( "GetBoobData: Could not load block! (block is already open?)\n" );
			}
		}
		
	}
	else if ( generateTerrain ) //for some reason doesn't run correctly if hasData is true.
	{
		// Lock the terrain while updating
		//mutex::scoped_lock local_lock( mtGaurd );

		//GenerateSingleTerrain( pBoob, position );
		AddGenerationRequest( position, true );
	}
	// Set position
	pBoob->position = Vector3d( ftype(position.x*64+32),ftype(position.y*64+32),ftype(position.z*64+32) );
	pBoob->li_position = position;
}
void CVoxelTerrain::SaveBoobData ( CBoob * pBoob, RangeVector const& position )
{
	if (( !pBoob->hasBlockData )||( !pBoob->hasPropData ))
	{
		Debug::Console->PrintWarning( "Warning: Attempting to save a boob that has no data!\n" );
		return;
	}
	/*stringstream tempStream;
	string sFilename;
	tempStream << CGameSettings::Active()->GetTerrainSaveDir() << "\\";
	tempStream << position.x << "_" << position.y << "_" << position.z;
	sFilename = tempStream.str();*/
	char filename [256];
	sprintf( filename, "%s\\%d_%d_%d\0", CGameSettings::Active()->GetTerrainSaveDir().c_str(), position.x, position.y, position.z );

	CBinaryFile file;
	//if ( file.Open( sFilename.c_str(), CBinaryFile::IO_WRITE ) )
	if ( file.Open( filename, CBinaryFile::IO_WRITE ) )
	{
		io->WriteBoobToFile( pBoob, &file );
		file.Close();
	}
	else
	{
		//cout << "error saving...likely lack of permissions or bad lock?" << endl;
		Debug::Console->PrintError( "SaveBoobData: Could not load block! (block is already open?)\n" );
	}
}

void CVoxelTerrain::SaveMetaboobData ( CMetaboob * pBoob, RangeVector const& position )
{
	if (( !pBoob->hasBlockData )||( !pBoob->hasPropData ))
	{
		Debug::Console->PrintWarning( "Warning: Attempting to save a boob that has no data!\n" );
		return;
	}
	/*stringstream tempStream;
	string sFilename;
	tempStream << CGameSettings::Active()->GetTerrainSaveDir() << "\\";
	tempStream << position.x << "_" << position.y << "_" << position.z;
	sFilename = tempStream.str();*/
	char filename [256];
	sprintf( filename, "%s\\%d_%d_%d\0", CGameSettings::Active()->GetTerrainSaveDir().c_str(), position.x, position.y, position.z );

	CBinaryFile file;
	if ( file.Exists( filename ) )
	{
		cout << "Attempting to regen existing area ("
			<< position.x << ","
			<< position.y << ","
			<< position.z << ")....ignoring." << endl;
	}
	else if ( file.Open( filename, CBinaryFile::IO_WRITE ) )
	{
		io->WriteMetaboobToFile( pBoob, &file );
		file.Close();
	}
	else
	{
		//cout << "error saving...likely lack of permissions or bad lock?" << endl;
		Debug::Console->PrintError( "SaveMetaboobData: Could not load block! (block is already open?)\n" );
	}
}

void CVoxelTerrain::SaveActiveTerrain ( CBoob * pBoob, RangeVector const& position )
{
	// Recursively check the outliers first, then work way in, because it likely will be unneeded on the inlyers.
	if ( pBoob != NULL )
	{
		if ( pBoob->bitinfo[7] == false )
		{
			pBoob->bitinfo[7] = true;

			// Move to the edges FIRST.
			if ( position.x >= root_position.x )
				SaveActiveTerrain( pBoob->front, RangeVector( position.x+1, position.y, position.z ) );
			if ( position.y >= root_position.y )
				SaveActiveTerrain( pBoob->left, RangeVector( position.x, position.y+1, position.z ) );
			if ( position.z >= root_position.z )
				SaveActiveTerrain( pBoob->top, RangeVector( position.x, position.y, position.z+1 ) );
			if ( position.x <= root_position.x )
				SaveActiveTerrain( pBoob->back, RangeVector( position.x-1, position.y, position.z ) );
			if ( position.y <= root_position.y )
				SaveActiveTerrain( pBoob->right, RangeVector( position.x, position.y-1, position.z ) );
			if ( position.z <= root_position.z )
				SaveActiveTerrain( pBoob->bottom, RangeVector( position.x, position.y, position.z-1 ) );

			// Only save valid data
			if ( pBoob->hasBlockData && pBoob->hasPropData )
			{
				SaveBoobData( pBoob, position );
			}
		}
	}
}
void CVoxelTerrain::LoadTerrain ( CBoob * pBoob, RangeVector const& position )
{
	// Recursively check the outliers first, then work way in, because it likely will be unneeded on the inlyers.
	if ( pBoob != NULL )
	{
		if ( pBoob->bitinfo[7] == false )
		{
			pBoob->bitinfo[7] = true;

			// Move to the edges FIRST.
			if ( position.x >= root_position.x )
				LoadTerrain( pBoob->front, RangeVector( position.x+1, position.y, position.z ) );
			if ( position.y >= root_position.y )
				LoadTerrain( pBoob->left, RangeVector( position.x, position.y+1, position.z ) );
			if ( position.z >= root_position.z )
				LoadTerrain( pBoob->top, RangeVector( position.x, position.y, position.z+1 ) );
			if ( position.x <= root_position.x )
				LoadTerrain( pBoob->back, RangeVector( position.x-1, position.y, position.z ) );
			if ( position.y <= root_position.y )
				LoadTerrain( pBoob->right, RangeVector( position.x, position.y-1, position.z ) );
			if ( position.z <= root_position.z )
				LoadTerrain( pBoob->bottom, RangeVector( position.x, position.y, position.z-1 ) );

			//cout << "saving " << position.x << "_" << position.y << "_" << position.z << endl;
			GetBoobData( pBoob, position, true );
		}
	}
}

bool CVoxelTerrain::TerraFileExists ( void )
{
	CBinaryFile file;
	//stringstream tempStream;
	//string sFilename;
	//tempStream << CGameSettings::Active()->GetTerrainSaveDir() << ".info";
	char sFilename [256];
	sprintf( sFilename, "%s.info", CGameSettings::Active()->GetTerrainSaveDir().c_str() );
	//sFilename = tempStream.str();
	if ( file.Exists( sFilename ) )
	{
		return true;
	}
	else
	{
		file.Open( sFilename, CBinaryFile::IO_WRITE );
		file.WriteUChar( 0 );
		file.Close();
		return false;
	}
}

// Load the target block on the LOD
bool CVoxelTerrain::LOD_Level1_LoadTarget ( bool& changeOccurred )
{
	RangeVector position = LODIndexToRangeVector( iCurrentLoadTarget );
	char filename [256];
	sprintf( filename, "%s/%d_%d_%d\0", CGameSettings::Active()->GetTerrainSaveDir().c_str(), position.x, position.y, position.z );

	CBinaryFile file;
	changeOccurred = false;
	if ( file.Exists( filename ) )
	{
		if ( file.Open( filename, CBinaryFile::IO_READ ) )
		{
			FILE* cfile = file.GetFILE();

			terra_t* dataBlock = (terra_t*)GetLODDataBlock(0);
			terra_t_lod* lodBlock = GetLODDataBlock(1);

			// Load in the primary structure
			CTerrainIO::filestruct_t fs;
			fread( &fs, sizeof( CTerrainIO::filestruct_t ), 1, cfile );
			if ( fs.version != TERRA_SYSTEM_VERSION )
			{
				cout << "Attempted to load outdated terrain format (cannot convert!)" << endl;
				return false;
			}

			// Load the terrain data
			fread( dataBlock, 4*32768, 1, cfile );
			// Close the file NOW
			file.Close();

			// Set the solidity of the terrain based on the percentage of solid blocks
			int solidCount = 0;
			// Also count the block types at the same time
			int dirtCount = 0;
			int grassCount = 0;
			int rockCount = 0;
			int sandCount = 0;
			int snowCount = 0;
			for ( int i = 0; i < 32768; ++i ) {
				if (( dataBlock[i].block == EB_NONE )||( dataBlock[i].block == EB_WATER )) {
					continue;
				}
				else if ( dataBlock[i].block == EB_DIRT ) {
					dirtCount += 2;
				}
				else if ( dataBlock[i].block == EB_GRASS ) {
					grassCount += 26;
				}
				else if ( dataBlock[i].block == EB_STONE || dataBlock[i].block == EB_DEADSTONE ) {
					rockCount += 1;
				}
				else if ( dataBlock[i].block == EB_SAND ) {
					sandCount += 3;
				}
				else if ( dataBlock[i].block == EB_SNOW || dataBlock[i].block == EB_TOP_SNOW ) {
					snowCount += 23;
				}
				solidCount++;
			}
			//lodBlock[iCurrentLoadTarget].solid = (solidCount > 19660) ? 1 : 0;
			//lodBlock[iCurrentLoadTarget].solid = (solidCount > 13107) ? 1 : 0;
			uint prevSolid = lodBlock[iCurrentLoadTarget].solid;
			uint prevLoaded = lodBlock[iCurrentLoadTarget].loaded;
			uint prevR = lodBlock[iCurrentLoadTarget].r;
			uint prevG = lodBlock[iCurrentLoadTarget].r;
			uint prevB = lodBlock[iCurrentLoadTarget].r;

			lodBlock[iCurrentLoadTarget].solid = (solidCount > 16384) ? 1 : 0;
			lodBlock[iCurrentLoadTarget].loaded = 1;

			if ( snowCount > rockCount || snowCount > dirtCount ) { // Snow color
				lodBlock[iCurrentLoadTarget].r = 207;
				lodBlock[iCurrentLoadTarget].g = 215;
				lodBlock[iCurrentLoadTarget].b = 220;
			}
			if ( rockCount > dirtCount && rockCount > sandCount ) { // Stone color
				lodBlock[iCurrentLoadTarget].r = 117;
				lodBlock[iCurrentLoadTarget].g = 124;
				lodBlock[iCurrentLoadTarget].b = 132;
			}
			else if ( sandCount > dirtCount ) { // Sand color
				lodBlock[iCurrentLoadTarget].r = 223;
				lodBlock[iCurrentLoadTarget].g = 180;
				lodBlock[iCurrentLoadTarget].b = 105;
			}
			else if ( dirtCount > grassCount ) { // Dirt color
				lodBlock[iCurrentLoadTarget].r = 148;
				lodBlock[iCurrentLoadTarget].g = 100;
				lodBlock[iCurrentLoadTarget].b = 52;
			}
			else { // Grass color
				lodBlock[iCurrentLoadTarget].r = 80;
				lodBlock[iCurrentLoadTarget].g = 136;
				lodBlock[iCurrentLoadTarget].b = 37;
			}
			
			if ( (prevLoaded && (prevSolid != lodBlock[iCurrentLoadTarget].solid
									|| prevR != lodBlock[iCurrentLoadTarget].r
									|| prevG != lodBlock[iCurrentLoadTarget].g
									|| prevB != lodBlock[iCurrentLoadTarget].b) ) ||
				(!prevLoaded && lodBlock[iCurrentLoadTarget].solid) )
			{
				changeOccurred = true;
			}

			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}