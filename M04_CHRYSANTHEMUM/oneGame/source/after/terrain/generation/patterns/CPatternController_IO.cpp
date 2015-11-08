
#include "CPatternController.h"

#include "core/system/io/FileUtils.h"
#include "core/system/io/CBinaryFile.h"

#include "after/terrain/generation/regions/CRegionGenerator.h"
#include "after/terrain/VoxelTerrain.h"
#include "after/terrain/generation/CWorldGen_Terran.h"
#include "after/terrain/io/COctreeIO.h"

using namespace Terrain;
using namespace World;

bool CPatternController::Pattern_ExistsAt ( const RangeVector& n_index )
{
	char stemp_fn[256];
	sprintf( stemp_fn, "%s.towns/t%d_%d_%d", m_terrain->IO->GetSavefile(), n_index.x, n_index.y, n_index.z ); 
	return IO::FileExists( stemp_fn );
}


void CPatternController::IO_SavePattern (
	const RangeVector& n_index,
	const patternData_t& n_pattern,
	const patternQuickInfo_t& n_quickinfo,
	const patternGameInfo_t& n_gameinfo )
{
	char stemp_fn[256];
	sprintf( stemp_fn, "%s.towns/t%d_%d_%d", m_terrain->IO->GetSavefile(), n_index.x, n_index.y, n_index.z ); 
	
	CBinaryFile file;
	file.OpenWait( stemp_fn, CBinaryFile::IO_WRITE|CBinaryFile::IO_READ );
	// First place empty information
	for ( uint i = 0; i < 1024; ++i ) {
		fputc( 0, file.GetFILE() );
	}

	// Write out the information structs
	fseek( file.GetFILE(), 0, SEEK_SET );
	fwrite( &n_quickinfo, sizeof( patternQuickInfo_t ), 1, file.GetFILE() );
	fseek( file.GetFILE(), 512, SEEK_SET );
	fwrite( &n_gameinfo, sizeof( patternGameInfo_t ), 1, file.GetFILE() );
	fseek( file.GetFILE(), 1024, SEEK_SET );

	// Write out the pattern data
	file.WriteData( (char*)&n_pattern.properties, sizeof(patternProperties_t) );
	file.WriteUInt32( n_pattern.roads.size() );
	for ( uint i = 0; i < n_pattern.roads.size(); ++i )
	{
		file.WriteData( (char*)&(n_pattern.roads[i]), sizeof(patternRoad_t) );
	}
	file.WriteUInt32( n_pattern.landmarks.size() );
	for ( uint i = 0; i < n_pattern.landmarks.size(); ++i )
	{
		file.WriteData( (char*)&(n_pattern.landmarks[i]), sizeof(patternLandmark_t) );
	}
	file.WriteUInt32( n_pattern.instances.size() );
	for ( uint i = 0; i < n_pattern.instances.size(); ++i )
	{
		file.WriteData( (char*)&(n_pattern.instances[i]), sizeof(patternInstance_t) );
	}
	file.WriteUInt32( n_pattern.buildings.size() );
	for ( uint i = 0; i < n_pattern.buildings.size(); ++i )
	{
		file.WriteData( (char*)&(n_pattern.buildings[i]), sizeof(patternBuilding_t) );
	}
}
void CPatternController::IO_LoadPattern (
	const RangeVector& n_index,
	patternData_t& o_pattern,
	patternQuickInfo_t& o_quickinfo,
	patternGameInfo_t& o_gameinfo )
{
	char stemp_fn[256];
	sprintf( stemp_fn, "%s.towns/t%d_%d_%d", m_terrain->IO->GetSavefile(), n_index.x, n_index.y, n_index.z ); 

	CBinaryFile file;
	file.OpenWait( stemp_fn, CBinaryFile::IO_READ );

	// Read in the information structs
	fread( &o_quickinfo, sizeof( patternQuickInfo_t ), 1, file.GetFILE() );
	fseek( file.GetFILE(), 512, SEEK_SET );
	fread( &o_gameinfo, sizeof( patternGameInfo_t ), 1, file.GetFILE() );
	fseek( file.GetFILE(), 1024, SEEK_SET );

	// Read in the pattern data
	file.ReadData( (char*)&o_pattern.properties, sizeof(patternProperties_t) );
	o_pattern.roads.resize( file.ReadUInt32() );
	for ( uint i = 0; i < o_pattern.roads.size(); ++i )
	{
		file.ReadData( (char*)&(o_pattern.roads[i]), sizeof(patternRoad_t) );
	}
	o_pattern.landmarks.resize( file.ReadUInt32() );
	for ( uint i = 0; i < o_pattern.landmarks.size(); ++i )
	{
		file.ReadData( (char*)&(o_pattern.landmarks[i]), sizeof(patternLandmark_t) );
	}
	o_pattern.instances.resize( file.ReadUInt32() );
	for ( uint i = 0; i < o_pattern.instances.size(); ++i )
	{
		file.ReadData( (char*)&(o_pattern.instances[i]), sizeof(patternInstance_t) );
	}
	o_pattern.buildings.resize( file.ReadUInt32() );
	for ( uint i = 0; i < o_pattern.buildings.size(); ++i )
	{
		file.ReadData( (char*)&(o_pattern.buildings[i]), sizeof(patternBuilding_t) );
	}
}