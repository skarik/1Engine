#include "CTerrainAccessor.h"

#include "after/terrain/VoxelTerrain.h"
#include "after/terrain/system/TerrainRenderer.h"

#include "after/terrain/data/GameState.h"
#include "after/types/terrain/BlockType.h"
#include "after/entities/item/system/ItemTerraBlok.h"

#include "after/renderer/objects/grass/CTerraGrass.h"

using namespace Terrain;

CTerrainAccessor::CTerrainAccessor ( void )
{
	m_terrain = CVoxelTerrain::GetActive();
	if ( m_terrain == NULL ) {
		//throw Core::NullReferenceException();
	}
}
CTerrainAccessor::~CTerrainAccessor ( void )
{

}

//=========================================//
// Block Grabbers
//=========================================//

void CTerrainAccessor::GetBlockAtPosition( Vector3d const&, BlockTrackInfo & o_blockInfo )
{
	if ( m_terrain ) {
		// use terrain->Sampler for the block grabbing
		throw Core::NotYetImplementedException();
	}
	else {
		o_blockInfo.block.raw = 0;
		o_blockInfo.block.block = EB_STONE;
		o_blockInfo.valid = false;
	}
}
void CTerrainAccessor::GetBlockAtPosition( RaycastHit const&, BlockTrackInfo & o_blockInfo )
{
	if ( m_terrain ) {
		// use terrain->Sampler for the block grabbing
		throw Core::NotYetImplementedException();
	}
	else {
		o_blockInfo.block.raw = 0;
		o_blockInfo.block.block = EB_STONE;
		o_blockInfo.valid = false;
	}
}
Terrain::terra_b CTerrainAccessor::GetBlockAtPosition ( Vector3d const& )
{
	if ( m_terrain ) {
		// use terrain->Sampler for the block grabbing
		throw Core::NotYetImplementedException();
	}
	else {
		Terrain::terra_b block;
		block.raw = 0;
		block.block = EB_STONE;
		return block;
	}
}
Terrain::terra_b CTerrainAccessor::GetBlockAtPosition ( RaycastHit const& )
{
	if ( m_terrain ) {
		// use terrain->Sampler for the block grabbing
		throw Core::NotYetImplementedException();
	}
	else {
		Terrain::terra_b block;
		block.raw = 0;
		block.block = EB_STONE;
		return block;
	}
}

//=========================================//
// Block Setters
//=========================================//

bool CTerrainAccessor::SetBlockAtPosition ( Vector3d const&, const ushort )
{
	// use terrain->Sampler for the block grabbing
	throw Core::NotYetImplementedException();
}
bool CTerrainAccessor::SetBlockAtPosition ( Vector3d const&, const Terrain::terra_b )
{
	// use terrain->Sampler for the block grabbing
	throw Core::NotYetImplementedException();
}
bool CTerrainAccessor::SetBlockAtPosition ( BlockTrackInfo const&, const ushort )
{
	// use terrain->Sampler for the block grabbing
	throw Core::NotYetImplementedException();
}
bool CTerrainAccessor::SetBlockAtPosition ( BlockTrackInfo const&, const Terrain::terra_b )
{
	// use terrain->Sampler for the block grabbing
	throw Core::NotYetImplementedException();
}

//=========================================//
// Query
//=========================================//

bool CTerrainAccessor::BlockHasComponent ( const BlockTrackInfo& targetBlock )
{
	return false;
}
bool CTerrainAccessor::BlockHasFoliage ( const BlockTrackInfo& targetBlock )
{
	return false;
}
bool CTerrainAccessor::BlockHasGrass ( const BlockTrackInfo& targetBlock )
{
	if ( !targetBlock.valid ) {
		throw std::invalid_argument( "Invalid block passed in!" );
	}
	//
	// Take block position, request a sector
	uint32_t t_index;
	Terrain::AreaGameState* t_data = NULL;
	while ( t_data == NULL )
	{
		// Keep trying to grab the area for edit
		try {
			t_data = m_terrain->AquireAreaGamestate( targetBlock.center, t_index );
		}
		catch ( const std::exception& ) {
			t_data = NULL;
		}
	}
	// Break grass
	bool result = t_data->grass_renderer->HasGrass( targetBlock.pBlock );
	// Release working reference
	m_terrain->ReleaseAreaGamestate( t_data, t_index );
	// Return result
	return result;
}

//=========================================//
// Editors
//=========================================//

void CTerrainAccessor::CompressBlock ( const BlockTrackInfo& targetBlock )
{
	if ( !targetBlock.valid ) {
		throw std::invalid_argument( "Invalid block passed in!" );
	}
	//
	switch ( targetBlock.block.block ) {
		case EB_SAND:
			targetBlock.pBlock->block = EB_SANDSTONE;
			break;
		case EB_SNOW:
			targetBlock.pBlock->block = EB_ICE;
			break;
		case EB_ASH:
			targetBlock.pBlock->block = EB_CLAY;
			break;
		case EB_STONE:
			targetBlock.pBlock->block = EB_GRAVEL;
			break;
		case EB_DEADSTONE:
			targetBlock.pBlock->block = EB_CURSED_DEADSTONE; // nice try assholes :D
			break;
	}

	m_terrain->Renderer->InvalidateAreaAt( targetBlock.center );
}
void CTerrainAccessor::DestroyBlock ( const BlockTrackInfo& targetBlock )
{
	if ( !targetBlock.valid ) {
		throw std::invalid_argument( "Invalid block passed in!" );
	}
	//

	ItemTerraBlok* newBlok = NULL;

	// Check for certain blocks that need to be changed
	if ( targetBlock.pBlock->block == EB_GRASS )
		targetBlock.pBlock->block = EB_DIRT;

	// Now itemize it
	if ( targetBlock.pBlock->block != EB_NONE )
		newBlok = new ItemTerraBlok ( NULL, targetBlock.pBlock->block );

	if ( newBlok )
	{
		newBlok->transform.position = targetBlock.center + Vector3d(1,1,1);
			/*= (infoes.pBoob->position) - Vector3d(31,31,31) + (Vector3d(
			ftype( ((infoes.b16index%2)*16)+((infoes.b8index%2)*8)+(infoes.b1index%8) ),
			ftype( (((infoes.b16index/2)%2)*16)+(((infoes.b8index/2)%2)*8)+((infoes.b1index/8)%8) ),
			ftype( ((infoes.b16index/4)*16)+((infoes.b8index/4)*8)+(infoes.b1index/64) ) ) * 2.0f );*/
		newBlok->transform.SetDirty();
	}

	// And then turn the block to air
	//SetBlock( infoes, EB_NONE );
	//return (CWeaponItem*)(newBlok);
	targetBlock.pBlock->block = EB_NONE;

	m_terrain->Renderer->InvalidateAreaAt( targetBlock.center );
}
void CTerrainAccessor::TillBlock ( const BlockTrackInfo& targetBlock )
{
	if ( !targetBlock.valid ) {
		throw std::invalid_argument( "Invalid block passed in!" );
	}
	//
	switch ( targetBlock.block.block ) {
		case EB_DIRT:
			targetBlock.pBlock->block = EB_TILLED_DIRT;
			targetBlock.pBlock->normal_z_x = Terrain::_normal_unbias(0);
			targetBlock.pBlock->normal_z_y = Terrain::_normal_unbias(0);
			break;
		case EB_GRASS:
			targetBlock.pBlock->block = EB_DIRT;
			break;
	}

	m_terrain->Renderer->InvalidateAreaAt( targetBlock.center );
}

void CTerrainAccessor::DestroyGrass ( const BlockTrackInfo& targetBlock )
{
	if ( !targetBlock.valid ) {
		throw std::invalid_argument( "Invalid block passed in!" );
	}
	//
	// Take block position, request a sector
	uint32_t t_index;
	Terrain::AreaGameState* t_data = NULL;
	while ( t_data == NULL )
	{
		// Keep trying to grab the area for edit
		try {
			t_data = m_terrain->AquireAreaGamestate( targetBlock.center, t_index );
		}
		catch ( const std::exception& ) {
			t_data = NULL;
		}
	}
	// Break grass
	t_data->grass_renderer->BreakGrass( targetBlock.pBlock );
	// Release working reference
	m_terrain->ReleaseAreaGamestate( t_data, t_index );
}

