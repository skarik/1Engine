
#include "CRegionGenerator.h"

#include "core/system/io/CBinaryFile.h"

#include "after/terrain/VoxelTerrain.h"
#include "after/terrain/generation/CWorldGen_Terran.h"
#include "after/terrain/Zones.h"

#include "after/utils/NamingUtils.h"

using namespace Terrain;
using namespace World;

CRegionGenerator::CRegionGenerator ( CVoxelTerrain* n_terra )
	: m_terrain( n_terra ), CGameBehavior()
{
	IO_Start();
}

CRegionGenerator::~CRegionGenerator ( void )
{
	IO_End();
}



void CRegionGenerator::Update ( void )
{

}


void CRegionGenerator::Generate ( const RangeVector& n_index )
{
	// Find current region.
	bool t_regionExists = false;

	// Ensure the file is open
	IO_Open();

	// Read region map
	if ( m_io && m_io->IsOpen() )
	{
		t_regionExists = ( Region_ClosestValidToIndex(n_index) != REGION_NONE );
	}
	else
	{
		throw Core::NullReferenceException();
	}

	// If cannot find region for this area
	if ( !t_regionExists )
	{
		std::cout << "Adding region (" << n_index.x << "," << n_index.y << "," << n_index.z << ") to region map. Commencing floodfill." << std::endl;

		// Begin floodfill creation
		m_gen_floodfill_lock.lock();
		// Floodfill for the area
		std::vector<RangeVector>	t_sectorList;
		bool						t_generateProperties = false;
		uint32_t resultRegion = Region_GenerateFloodfill( n_index, t_generateProperties, t_sectorList );

		// Save state information
		IO_UpdateNext();
		// Done with floodfill
		m_gen_floodfill_lock.unlock();

		// If valid region created, then give it properties
		if ( t_generateProperties && ( resultRegion != REGION_NONE )&&( resultRegion != REGION_UNUSED ))
		{
			Region_GenerateProperties( resultRegion, n_index, t_sectorList );
		}
		else if (( resultRegion != REGION_NONE )&&( resultRegion != REGION_UNUSED ))
		{
			Region_AppendSectors( resultRegion, t_sectorList );
		}
	}

	// After X amount of generates, save the region file.
	if ( --m_io_usecount <= 0 )
	{
		m_io_usecount = 57;
		IO_Save();
	}
	// End find current region.
}

// Region_GenerateProperties ( ) : Generates properties for the given region.
// Will overwrite old properties.
void CRegionGenerator::Region_GenerateProperties ( const uint32_t n_region, const RangeVector& n_origin, const std::vector<RangeVector>& n_sectors )
{
	// Generate region properties
	{
		regionproperties_t region;
		memset( &region, 0, sizeof(regionproperties_t) );
		// First get list of areas
		/*vector<sProvinceAreaEntry> areaList;
		GetProvinceArea( region_id, areaList );
		if ( areaList.empty() ) {
			cout << "Empty area list" << endl;
			return;
		}*/

		//sProvinceInfo provinceInfo;
		auto randomSampler = ((CWorldGen_Terran*)m_terrain->GetGenerator())->m_buf_general;

		
		// Generate culture values
		region.culture[World::CULTURE_EUROPEAN]	=
			randomSampler.sampleBufferMacro( m_terrain->GetGenerator()->GetSeed()*0.22 - n_region * 0.70, (n_origin.x+n_origin.y+n_origin.z)*0.1 + 127.32 )*0.004 - 0.1;
			//randomSampler.sampleBufferMacro( m_terrain->GetGenerator()->GetSeed()*0.22 - n_region * 0.70, (areaList[0].x+areaList[0].y)*0.1f, 127.32f ))+0.5;
		region.culture[World::CULTURE_ASIAN]		=
			randomSampler.sampleBufferMacro( m_terrain->GetGenerator()->GetSeed()*0.12 - n_region * 0.71, (n_origin.x+n_origin.y+n_origin.z)*0.1 + 265.43 )*0.004 - 0.1;
			//randomSampler.sampleBufferMacro( m_terrain->GetGenerator()->GetSeed()*0.12 - n_region * 0.71, (areaList[0].x+areaList[0].y)*0.1f, 265.43f ))+0.5;
		region.culture[World::CULTURE_ARCANA]		= // Arcana is more rare
			randomSampler.sampleBufferMacro( m_terrain->GetGenerator()->GetSeed()*0.32 - n_region * 0.72, (n_origin.x+n_origin.y+n_origin.z)*0.1 + 378.23 )*0.004 - 0.1;
		region.culture[World::CULTURE_ARCANA] *= 0.27;
			//randomSampler.sampleBufferMacro( m_terrain->GetGenerator()->GetSeed()*0.32 - n_region * 0.72, (areaList[0].x+areaList[0].y)*0.1f, 378.23f ))+0.5f)*0.27;

		// Normalize the culture values to 1
		ftype maxValue = 0;
		for ( uint i = 0; i < 3; ++i ) {
			maxValue += region.culture[i];
		}
		// Possible culture could be lower than zero, arbitrarily make them above zero
		if ( maxValue < 0 ) {
			for ( uint i = 0; i < 3; ++i ) {
				region.culture[i] += maxValue / 2;
			}
			maxValue = 0;
			for ( uint i = 0; i < 3; ++i ) {
				maxValue += region.culture[i];
			}
		}
		// Do inv mul to normalize to 1
		maxValue = 1/maxValue;
		for ( uint i = 0; i < 3; ++i ) {
			region.culture[i] *= maxValue;
		}

		// Create general opnions
		//region.magi_opinion	= generator->noise_biome->Get3D( (areaList[0].x+areaList[0].y)*0.1f, region_id * -0.50f, 154.76f ) * 0.2f;
		region.magi_opinion	= randomSampler.sampleBufferMacro( (n_origin.x+n_origin.y+n_origin.z)*0.1, n_region * -0.50f + 154.76 )/256.0 - 0.5;
		//region.tech_opinion	= generator->noise_biome->Get3D( (areaList[0].x+areaList[0].y)*0.1f, region_id * -0.51f, 223.64f ) * 0.2f;
		region.tech_opinion	= randomSampler.sampleBufferMacro( (n_origin.x+n_origin.y+n_origin.z)*0.1, n_region * -0.51f + 223.64 )/256.0 - 0.5;

		// Generate region name (TODO: look at surrounding area to get name!)
		region.name = NamingUtils::CreateBaseTownName();

		// Save region
		IO_RegionSaveInfo( n_region, &region );
		// Save region sectors
		IO_RegionSaveSectors( n_region, n_sectors );

		// Output debug info
		std::cout << "Saved region " << n_region << " with info: " << std::endl;
		std::cout << "  CULTURE_EURO:   " << region.culture[World::CULTURE_EUROPEAN] << std::endl;
		std::cout << "  CULTURE_ASIAN:  " << region.culture[World::CULTURE_ASIAN] << std::endl;
		std::cout << "  CULTURE_ARCANA: " << region.culture[World::CULTURE_ARCANA] << std::endl;
		std::cout << "  MAGI_OPINION: " << region.magi_opinion << std::endl;
		std::cout << "  TECH_OPINION: " << region.tech_opinion << std::endl;
	}
}


// Region_GenerateFloodfill ( ) : Generates a region using a floodfill algorithm.
// Will swallow existing regions.
uint32_t CRegionGenerator::Region_GenerateFloodfill ( const RangeVector& n_index, bool& o_isNewArea, std::vector<RangeVector>& o_sectorList )
{
	// So, here's how it goes. We floodfill.
	uint32_t regionSize = 0;

	//Vector3d_d t_sourceCenter = (qd.pMin + qd.pMax)/2;
	Vector3d_d t_sourceCenter ( n_index.x * 64 + 32, n_index.y * 64 + 32, n_index.z * 64 + 32 );

	// First, need the source color.
	uint8_t sourceBiome = m_terrain->GetGenerator()->GetBiomeAt( t_sourceCenter );
	uint8_t sourceTerra = m_terrain->GetGenerator()->GetTerrainAt( t_sourceCenter );

	if ( sourceTerra == TER_OCEAN || sourceTerra == TER_THE_EDGE )
	{
		// Add position to regions as unused.
		regioninfo_t set;
		set.position = n_index;
		set.region = REGION_UNUSED;
		IO_SetRegion( set );
		return REGION_UNUSED;
	}

	// Now, floodfill using a queue
	//o_sectorList.clear();
	std::list<RangeVector>		floodQueue;
	floodQueue.push_back( n_index );
	while ( !floodQueue.empty() )
	{
		// Get the current value on the queue
		RangeVector currentPosition = floodQueue.front();
		floodQueue.pop_front();
		// Check target color
		Vector3d_d t_worldPosition ( currentPosition.x*64 + 32, currentPosition.y*64 + 32, currentPosition.z*64 + 32 );
		uint8_t targetBiome = m_terrain->GetGenerator()->GetBiomeAt( t_worldPosition );
		uint8_t targetTerra = m_terrain->GetGenerator()->GetTerrainAt( t_worldPosition );

		// Generate differences
		int majorDif = Zones.TerraCompare( sourceBiome, targetBiome );
		int minorDif = Zones.TerraCompare( sourceTerra, targetTerra );
		// Distance adds 1 to majorDif. Regions shouldn't be too large.
		//int distDif = (int)( Vector2d_d( n_index.x-currentPosition.x,n_index.y-currentPosition.y ).magnitude() * 0.03f );
		int distDif = (int)( Vector2d_d( n_index.x-currentPosition.x,n_index.y-currentPosition.y ).magnitude() * 0.08 );
		majorDif += distDif;
		
		// On majorDif, 4 is should stop. 5 is should definitely stop.
		// Same for minorDif is majorDif is 2 or smaller.
		if ( majorDif <= 2 ) {
			majorDif = minorDif+distDif;
		}

		bool addRegion = true;
		if ( majorDif == 4 )
		{
			// Chance to stop with this region.
			//if ( generator->Trd_Noise_Sample(currentPosition.x*3.3f,currentPosition.y*4.4f,majorDif*9.1f+minorDif*1.7f) > 0.0f )
			if ( ((CWorldGen_Terran*)m_terrain->GetGenerator())->m_buf_general_noise.sampleBufferMicro(
				currentPosition.x*3.3 + majorDif*9.1 + currentPosition.z*2.2,
				currentPosition.y*4.4 + minorDif*1.7 - currentPosition.z*1.1 ) > 128 )
			{
				addRegion = false;
			}
		}
		else if ( majorDif >= 5 )
		{
			// Always stop with this region
			addRegion = false;
		}
		// Check that the area is not already in the regions
		if ( addRegion )
		{
			if ( Region_ClosestValidToIndex(currentPosition) != REGION_NONE )
			{
				addRegion = false;
			}
		}

		// Add the region
		if ( addRegion )
		{
			// Increment size of the region
			regionSize += 1;

			// Add this region to the map
			regioninfo_t set;
			set.position = currentPosition;
			set.region = m_gen_nextregion;
			IO_SetRegion( set );

			// Add this region to the list for if we need to rework the target position
			o_sectorList.push_back( currentPosition );

			// Add its neighbors to the floodfill algorithm.
			RangeVector addPosition;
			addPosition = RangeVector(currentPosition.x-1,currentPosition.y,currentPosition.z);
				floodQueue.push_back( addPosition );
			addPosition = RangeVector(currentPosition.x+1,currentPosition.y,currentPosition.z);
				floodQueue.push_back( addPosition );
			addPosition = RangeVector(currentPosition.x,currentPosition.y-1,currentPosition.z);
				floodQueue.push_back( addPosition );
			addPosition = RangeVector(currentPosition.x,currentPosition.y+1,currentPosition.z);
				floodQueue.push_back( addPosition );
		}
	}
	
	// If the region is too small, it needs to take the neighbor regions.
	//if ( regionSize <= 5 ) // a kingdom should be at LEAST 20,000 sq feet
	// FUQ THAT. A KINGDOM SHOULD BE BIGGER
	o_isNewArea = true;
	if ( regionSize <= 13 )
	{
		// Look for nearby region.
		RangeVector newPosition = RangeVector(n_index.x-1,n_index.y,n_index.z);
		uint32_t nearRegion = Region_ClosestValidToIndex( newPosition );
		if ( (nearRegion == REGION_NONE)||(nearRegion == REGION_UNUSED||(nearRegion == m_gen_nextregion)) ) {
			newPosition = RangeVector(n_index.x+1,n_index.y,n_index.z);
			nearRegion = Region_ClosestValidToIndex( newPosition );
		}
		if ( (nearRegion == REGION_NONE)||(nearRegion == REGION_UNUSED||(nearRegion == m_gen_nextregion)) ) {
			newPosition = RangeVector(n_index.x,n_index.y-1,n_index.z);
			nearRegion = Region_ClosestValidToIndex( newPosition );
		}
		if ( (nearRegion == REGION_NONE)||(nearRegion == REGION_UNUSED||(nearRegion == m_gen_nextregion)) ) {
			newPosition = RangeVector(n_index.x,n_index.y+1,n_index.z);
			nearRegion = Region_ClosestValidToIndex( newPosition );
		}
		// Select region
		if ( (nearRegion != REGION_NONE)&&(nearRegion != REGION_UNUSED&&(nearRegion != m_gen_nextregion)) ) {
			// Make it the region
			//Region_ChangeRegionTo( m_gen_nextregion, nearRegion );
			for ( uint i = 0; i < o_sectorList.size(); ++i )
			{
				// Add this region to the map
				regioninfo_t set;
				set.position = o_sectorList[i];
				set.region = nearRegion;
				IO_SetRegion( set );
			}
			std::cout << "  Setting " << m_gen_nextregion << " to " << nearRegion << std::endl;
			o_isNewArea = false;
			// Return that region
			return nearRegion;
		}
		else
		{
			if ( nearRegion == m_gen_nextregion )
			{
				// Why are we here?
			}
			else if ( nearRegion == REGION_NONE )
			{
				// Generate that region
				//RegionsFloodfill( newPosition );
				// Get the id of the region generated.
				//nearRegion = RegionsSearch( newPosition );
				nearRegion = Region_GenerateFloodfill( newPosition, o_isNewArea, o_sectorList );
				// Use the new region as the value to set
				//Region_ChangeRegionTo( m_gen_nextregion, nearRegion );
				for ( uint i = 0; i < o_sectorList.size(); ++i )
				{
					// Add this region to the map
					regioninfo_t set;
					set.position = o_sectorList[i];
					set.region = nearRegion;
					IO_SetRegion( set );
				}
				std::cout << "  Setting " << m_gen_nextregion << " to " << nearRegion << std::endl;
				// Return that region
				return nearRegion;
			}
			else 
			{
				// Set to invalid region
				//Region_ChangeRegionTo( m_gen_nextregion, REGION_UNUSED );
				for ( uint i = 0; i < o_sectorList.size(); ++i )
				{
					// Add this region to the map
					regioninfo_t set;
					set.position = o_sectorList[i];
					set.region = REGION_UNUSED;
					IO_SetRegion( set );
				}
				// Return invalid area.
				return REGION_UNUSED;
			}
		}
	}

	// Increment region index
	m_gen_nextregion += 1;

	// Return region index-1
	return m_gen_nextregion-1;
}


// Region_ClosestValidToIndex ( ) : Finds the closest valid region to the index.
// Validitiy is based on the XY coordinates. The Z coordinate comes into play when there are overlapping regions.
uint32_t CRegionGenerator::Region_ClosestValidToIndex ( const RangeVector& n_index )
{
	// For now, we only check for a matching XY coordinate. Z coordinate adds another level of complexity.
	io_intermediate_t result;
	IO_FindIndex( n_index, result );
	return result.data.region;
}


// Region_ChangeRegionTo ( ) : Changes all positions with the given region into another region.
void CRegionGenerator::Region_ChangeRegionTo ( const uint32_t n_source_region, const uint32_t n_target_region )
{
	throw std::exception();
}

// Region_AppendSectors ( ) : Appends the list of sectors to the existing region
void CRegionGenerator::Region_AppendSectors ( const uint32_t n_region, const std::vector<RangeVector>& n_sectors )
{
	std::vector<RangeVector> t_sectorList;
	IO_RegionLoadSectors( n_region, t_sectorList );
	for ( auto sector = n_sectors.begin(); sector != n_sectors.end(); ++sector )
	{
		if ( std::find( t_sectorList.begin(), t_sectorList.end(), *sector ) == t_sectorList.end() )
		{
			t_sectorList.push_back( *sector );
		}
	}
	IO_RegionSaveSectors( n_region, t_sectorList );
}