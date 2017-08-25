
#include "TerrainRenderer.h"
#include "after/terrain/VoxelTerrain.h"

Terrain::TerrainRenderer::TerrainRenderer ( CVoxelTerrain* n_terrain )
	: CRenderableObject(), m_terrain(n_terrain)
{
	;
}
Terrain::TerrainRenderer::~TerrainRenderer ( void )
{
	;
}


//=========================================//
// Terrain-related systems

// clears mesh and collision data for all current sectors/areas
void Terrain::TerrainRenderer::ClearAll ( void )
{
	throw Core::NotYetImplementedException();
}
// clears mesh and collision data at the given sector
void Terrain::TerrainRenderer::ClearAreaAt ( const WorldVector& n_sector )
{
	throw Core::NotYetImplementedException();
}
void Terrain::TerrainRenderer::ClearAreaAt ( const Vector3d& n_position )
{
	ClearAreaAt( m_terrain->GetCenterSector() + WorldVector(
		(rangeint)std::floor(n_position.x/64.0F),
		(rangeint)std::floor(n_position.y/64.0F),
		(rangeint)std::floor(n_position.z/64.0F) )
		);
}
// marks all sectors/areas as out-of-date
void Terrain::TerrainRenderer::InvalidateAll ( void )
{
	throw Core::NotYetImplementedException();
}
// marks out-of-date flag at the given sector
void Terrain::TerrainRenderer::InvalidateAreaAt ( const WorldVector& n_sector )
{
	throw Core::NotYetImplementedException();
}
void Terrain::TerrainRenderer::InvalidateAreaAt ( const Vector3d& n_position )
{
	InvalidateAreaAt( m_terrain->GetCenterSector() + WorldVector(
		(rangeint)std::floor(n_position.x/64.0F),
		(rangeint)std::floor(n_position.y/64.0F),
		(rangeint)std::floor(n_position.z/64.0F) )
		);
}

// Grabs an area's meshstate from the given terrain, given the input gameplay position.
// This is safe without locks because all mesh and collision updates must happen in the main thread.
Terrain::AreaMeshState* Terrain::TerrainRenderer::GetAreaMeshstate ( const Vector3d& n_approximatePosition )
{
	throw Core::NotYetImplementedException();
}

//=========================================//
// Render step
bool Terrain::TerrainRenderer::Render ( const char pass )
{
	return false;
}
