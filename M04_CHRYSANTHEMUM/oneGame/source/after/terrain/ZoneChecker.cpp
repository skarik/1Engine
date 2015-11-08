
#include "Zones.h"
#include "after/terrain/VoxelTerrain.h"
#include "after/terrain/system/TerrainRenderer.h"
#include "after/terrain/system/DataSampler.h"
#include "after/terrain/generation/CWorldGenerator.h"

#include "after/types/terrain/TerrainTypes.h"
#include "after/types/terrain/BiomeTypes.h"

//
// Returns:
//		RangeVector - the input vector converted to index coordinates.
//		For the MCC, this takes into account the active terrain, and will offset based on its root.
//		Note that this function will still work even if there is no active terrain.
//
RangeVector CZoneChecker::PositionToRV	( const Vector3d & vInPos )
{
	Vector3d pos = vInPos;
	pos.x /= (64.0f);
	pos.y /= (64.0f);
	pos.z /= (64.0f);
	return RangeVector( (int32_t)floorf(pos.x),(int32_t)floorf(pos.y),(int32_t)floorf(pos.z) );
}
RangeVector	CZoneChecker::PositionToRV	( const Vector3d_d & vInPos )
{
	Vector3d_d pos = vInPos;
	pos.x /= (64.0);
	pos.y /= (64.0);
	pos.z /= (64.0);
	return RangeVector( (int32_t)floor(pos.x),(int32_t)floor(pos.y),(int32_t)floor(pos.z) );
}

//
// Returns:
//		bool - indicative if the given position is in an 'active' zone.
//		For the MCC, this checks mainly the current active terrain.
//		Active terrain areas are where objects should be allowed to be active.
//		The active terrain area is where the physics engine is gaurenteed to be active, and is where
//		CCharacter instances should be limited to.
//		
bool	CZoneChecker::IsActiveArea		( const Vector3d & vInPos )
{
	if ( !CVoxelTerrain::TerrainList.empty() )
	{
		//return CVoxelTerrain::TerrainList[0]->GetActiveCollision( vInPos );
		Terrain::AreaMeshState* meshstate = CVoxelTerrain::TerrainList[0]->Renderer->GetAreaMeshstate( vInPos );
		if ( meshstate )
			return meshstate->has_collision;
		else
			return false;
	}
	return true;
}

//
// Returns:
//		bool - indicative if the given position is in an 'super' zone.
//		For the MCC, this checks mainly the current active terrain.
//		Super terrain areas are very very very much last place that objects should be allowed to be active.
//		CCharacter instances should be limited to this area at the least.
//		Totally good for flying enemies though.
//		
bool	CZoneChecker::IsSuperArea			( const Vector3d & vInPos )
{
	if ( !CVoxelTerrain::TerrainList.empty() )
	{
		RangeVector charpos = CVoxelTerrain::TerrainList[0]->GetCenterSector();
		Vector3d difpos = vInPos-Vector3d( (ftype)charpos.x*64.0f, (ftype)charpos.y*64.0f, (ftype)charpos.z*64.0f );
		//if ( difpos.magnitude() < (CVoxelTerrain::TerrainList[0]->GetTerrainRange()*64.0f*1.1f) )
		if ( difpos.magnitude() < (7*64.0f*1.1f) )
		{
			return true;
		}
		return false;
	}
	return true;
}


//
// Returns:
//		char - corresponding to an enumeration in EBiomeType defined in CBoob.h
//		For the MCC, this checks the current active terrain and recreates the terrain generation algorithm.
//		
char	CZoneChecker::GetTerrainBiomeAt	( const Vector3d & vInPos )
{
	if ( !CVoxelTerrain::TerrainList.empty() )
	{
		//return CVoxelTerrain::TerrainList[0]->TerraGen_pub_GetBiome( vInPos / 64.0f );
		Vector3d_d checkPosition = CVoxelTerrain::GetActive()->GetCenterPosition();
		checkPosition.x += vInPos.x;
		checkPosition.y += vInPos.y;
		checkPosition.z += vInPos.z;
		return CVoxelTerrain::GetActive()->GetGenerator()->GetBiomeAt( checkPosition );
	}
	else
	{
		//return BIO_DEFAULT;
		return Terrain::BIO_DESERT;
	}
}

//
// Returns:
//		char - corresponding to an enumeration in ETerrainType defined in CBoob.h
//		For the MCC, this checks the current active terrain and recreates the terrain generation algorithm.
//		
char	CZoneChecker::GetTerrainTypeAt	( const Vector3d & vInPos )
{
	if ( !CVoxelTerrain::TerrainList.empty() )
	{
		//return CVoxelTerrain::TerrainList[0]->TerraGen_pub_GetType( vInPos / 64.0f );
		Vector3d_d checkPosition = CVoxelTerrain::GetActive()->GetCenterPosition();
		checkPosition.x += vInPos.x;
		checkPosition.y += vInPos.y;
		checkPosition.z += vInPos.z;
		return CVoxelTerrain::GetActive()->GetGenerator()->GetTerrainAt( checkPosition );
	}
	else
	{
		//return TER_DEFAULT;
		return Terrain::TER_DESERT;
	}
}

//
// Returns:
//		ftype - giving the elevation in sector coordinates. Multiply by 64 to get the elevation in feet.
//		For the MCC, this checks the current active terrain and recreates the terrain generation algorithm.
//
/*ftype	CZoneChecker::GetTerrainElevationAt	( const Vector3d & vInPos )
{
	if ( !CVoxelTerrain::TerrainList.empty() )
	{
		return CVoxelTerrain::TerrainList[0]->TerraGen_pub_GetElevation( vInPos / 64.0f );
	}
	else
	{
		return 0;
	}
}*/

//
// Returns:
//		bool - indicating that the given terrain area has data loaded and has a non-negative collision reference.
//		This is rather specific to AFTER. It will always return true if there is no terrain to check.
//
bool	CZoneChecker::IsCollidableArea	( const Vector3d & vInPos )
{
	/*if ( COctreeTerrain::GetActive() )
	{
		// Terrain has a routine specifically for checking this
		if ( COctreeTerrain::GetActive()->State_SectorHasMesh( vInPos ) ) {
			return true;
		}
		return false;
	}
	else if (( !CVoxelTerrain::terrainList.empty() )&&( CVoxelTerrain::terrainList[0] != NULL ))
	{
		// First, get the boob reference at the given position
		CBoob* target = CVoxelTerrain::terrainList[0]->GetBoobAtPosition( vInPos );
		if ( target ) {
			if ( target->hasBlockData ) {
				return true;
			}
		}
		return false;
	}
	else
	{
		return true;
	}*/
	if ( !CVoxelTerrain::TerrainList.empty() )
	{
		// Terrain has a routine specifically for checking this
		/*if ( CVoxelTerrain::TerrainList[0]->Renderer->GetAreaMeshstate( vInPos )->has_collision ) {
			return true;
		}
		return false;*/
		Terrain::AreaMeshState* meshstate = CVoxelTerrain::TerrainList[0]->Renderer->GetAreaMeshstate( vInPos );
		if ( meshstate )
			return meshstate->has_collision;
		else
			return false;
	}
	return true;
}

//
// Returns:
//		bool - Returns true if supplied pointer is voxel terrain
//
bool	 CZoneChecker::IsActiveTerrain		( const CVoxelTerrain* terra )
{
	if (( !CVoxelTerrain::TerrainList.empty() )&&( terra == CVoxelTerrain::TerrainList[0] )) {
		return true;
	}
	return false;
}
// Returns:
//		pointer - Returns pointer to active terrain or NULL.
CVoxelTerrain* CZoneChecker::GetActiveTerrain	( void )
{
	if ( !CVoxelTerrain::TerrainList.empty() ) {
		return CVoxelTerrain::TerrainList[0];
	}
	return NULL;
}

//	GetCurrentRoot ( )
// Returns:
//		double Vector3d - current XYZ position of the world that the root represents right now.
//		This is specific to AFTER.
Vector3d_d	CZoneChecker::GetCurrentRoot ( void )
{
	/*if ( COctreeTerrain::GetActive() )
	{
		return COctreeTerrain::GetActive()->GetStateWorldCenterPosition();
	}*/
	if ( !CVoxelTerrain::TerrainList.empty() ) {
		return CVoxelTerrain::TerrainList[0]->GetCenterPosition();
	}
	return Vector3d_d( 0,0,4 );
}

/*
Terrain::terra_t	CZoneChecker::GetBlockAtPosition ( const Vector3d & position )
{
	if ( CVoxelTerrain::GetActive() ) {
		return CVoxelTerrain::GetActive()->GetBlockAtPosition( position );
	}
	else if ( COctreeTerrain::GetActive() && (!COctreeTerrain::GetActive()->GetSystemPaused()) ) {
		Terrain::terra_t value;
		Terrain::terra_b result;
		COctreeTerrain::GetActive()->SampleBlock( position, result.raw );
		value.block = result.block;
		return value;
	}
	Terrain::terra_t empty;
	empty.raw = 0;
	return empty;
}
*/
Terrain::terra_b	CZoneChecker::GetBlockAtPosition ( const Vector3d &position )
{
	//if ( COctreeTerrain::GetActive() && (!COctreeTerrain::GetActive()->GetSystemPaused()) ) {
	if ( !CVoxelTerrain::TerrainList.empty() )
	{
		Terrain::terra_b result;
		//COctreeTerrain::GetActive()->SampleBlock( position, result.raw );
		CVoxelTerrain::TerrainList[0]->Sampler->BlockAt( position, result );
		return result;
	}
	Terrain::terra_b empty;
	empty.raw = 0;
	return empty;
}
