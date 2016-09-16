
#include "core/math/Math.h"
#include "core/math/Math3d.h"
#include "core/math/noise/SimplexNoise.h"
#include "core/math/random/Random.h"

#include "CWorldGen_Terran.h"

#include "after/entities/foliage/CTerraFoliageFactory.h"
#include "after/entities/props/CTerrainPropFactory.h"

#include "after/terrain/edit/SidebufferAccessor.h"
#include "after/terrain/edit/csg/SidebufferVolumeEditor.h"
#include "after/terrain/VoxelTerrain.h"

using namespace Terrain;

// =============================
// == World layout + creation ==
// =============================
void CWorldGen_Terran::Init_CreateContinents ( void )
{
	// Place the continents
	for ( int i = 0; i < m_continentCount; ++i ) {
		m_continentPositions.push_back( Vector2d_d( (noise->Get( i, 2*i )*0.5+0.5) * m_worldSize.x, (noise->Get( i+10, 1-i )*0.5+0.5) * m_worldSize.y ) );
		// Generate sizes for them
		m_continentSizes.push_back( Vector2d_d(
			noise->Unnormalize(noise->Get( -i-1, -1+2*i ))+1.2,
			noise->Unnormalize(noise->Get( -i-1,  1-2*i ))+1.2
			) );
		if ( m_continentSizes[i].x/m_continentSizes[i].y > 5 ) {
			m_continentSizes[i].y = m_continentSizes[i].x/5;
		}
		if ( m_continentSizes[i].y/m_continentSizes[i].x > 5 ) {
			m_continentSizes[i].x = m_continentSizes[i].y/5;
		}
		if ( m_continentSizes[i].magnitude() < 1.2 ) {
			m_continentSizes[i] = m_continentSizes[i].normal() * 1.2;
		}
	}
	// Space the continents better
	for ( int iteration = 0; iteration < 15; ++iteration ) {
		for ( int i = 0; i < m_continentCount; ++i ) {
			// Provide repulsion among continents
			Vector2d_d& currentCenter = m_continentPositions[i];
			for ( int j = 0; j < m_continentCount; ++j ) {
				if ( i == j ) continue;
				Vector2d_d& opposingCenter = m_continentPositions[j];
				Vector2d_d opposingTowardsThis = (currentCenter-opposingCenter);
				// Repel when too close
				if ( opposingTowardsThis.magnitude() < 250000 ) {
					opposingCenter -= (opposingTowardsThis.normal()) * 40000;
					currentCenter += (opposingTowardsThis.normal()) * 40000;
				}
				else {
					opposingCenter -= (opposingTowardsThis.normal()) * 2000;
					currentCenter += (opposingTowardsThis.normal()) * 2000;
				}
			}
			// Add some noise for good measure
			currentCenter += Vector2d_d( noise->Get3D(i,1-i+iteration,iteration)*40000, noise->Get3D(i,0-i-iteration,iteration)*40000 );
			// Limit center to disc from world center
			Vector2d_d centerToThis = currentCenter-(m_worldSize/2);
			if ( centerToThis.magnitude() > m_worldSize.x/2 ) {
				currentCenter = (m_worldSize/2) + (centerToThis.normal() * (m_worldSize.x/2));
			}
			//
		}
		// End repulsion loop
	}

	// Generate some stickers for now
	{
		if ( m_areaStickers.empty() ) 
		{
			areaSticker_t area;

			area.biome = BIO_DEFAULT;
			area.terrain = TER_FLATLANDS;
			area.position = Vector2d_d(0,0);
			area.size = Vector2d_d( 150,150 );
			m_areaStickers.push_back( area );

			area.biome = BIO_DESERT;
			area.terrain = TER_DESERT;
			area.position = Vector2d_d(200,0);
			area.size = Vector2d_d( 150,150 );
			m_areaStickers.push_back( area );
		}
	}
}


// GetLandmassity( ) : Returns parametric distance from the coast. Positive for land, negative for water.
// firstPass will abort the checks on the first land hit.
Real CWorldGen_Terran::GetLandmassity ( const Vector2d_d& worldposition, const bool firstPass )
{
	return 24.0f;
	/*
	Real landmassity = -3;
	for ( int i = 0; i < m_continentCount; ++i ) {
		for ( int offs = 0; offs < 4; ++offs )
		{
			Vector2d_d sampleWorldPosition = worldposition;
			// Perform edge offsets
			if ( worldposition.x < m_worldSize.x/2 ) {
				if ( offs%2 == 1 ) {
					sampleWorldPosition.x += m_worldSize.x;
				}
			}
			else {
				if ( offs%2 == 1 ) {
					sampleWorldPosition.x -= m_worldSize.x;
				}
			}
			if ( worldposition.y < m_worldSize.y/2 ) {
				if ( offs/2 == 1 ) {
					sampleWorldPosition.y += m_worldSize.y;
				}
			}
			else {
				if ( offs/2 == 1 ) {
					sampleWorldPosition.y -= m_worldSize.y;
				}
			}
			// Create distance to target
			Vector2d_d delta = sampleWorldPosition - m_continentPositions[i];

			// Rotate the input for an offset
			Real_d offsetStrength = (std::max<Real_d>( m_continentSizes[i].x/m_continentSizes[i].y, m_continentSizes[i].y/m_continentSizes[i].x ) - 1.0f)*2;
			Matrix2x2 rotation;
			rotation.setRotation( offsetStrength * delta.magnitude() * noise->Get3D(sampleWorldPosition.x/500000, sampleWorldPosition.y/500000, 2*i + 4 ) / 12000 );
			//delta = rotation * delta;
			delta = Vector2d_d(
				rotation.pData[0]*delta.x+rotation.pData[1]*delta.y,
				rotation.pData[2]*delta.x+rotation.pData[3]*delta.y
				);

			// Perform size checks
			delta.x /= m_continentSizes[i].x;
			delta.y /= m_continentSizes[i].y;

			// Perform delta check
			Vector2d_d samplePosition = sampleWorldPosition/200000;
			Real_d radius = 150000 + 130000*noise->Get3D( samplePosition.x, samplePosition.y, i+1 );
			landmassity = (Real)( std::max<Real_d>( landmassity, (radius-delta.magnitude())/150000 ) ); // Double precision needed during calculation, but not for final value
			if ( firstPass && landmassity > 0 ) {
				return landmassity * 4000;
			}
		}
	}
	return landmassity * 4000;*/
}


uint8_t CWorldGen_Terran::GetTerrainAt ( Vector3d_d& worldPosition )
{
	ETerrainType terrain = TER_DEFAULT;

	for ( auto sticker = m_areaStickers.begin(); sticker != m_areaStickers.end(); ++sticker )
	{
		// Create distance to target
		Vector2d_d delta = Vector2d_d(worldPosition.x,worldPosition.y) - sticker->position;

		// Perform size checks
		delta.x /= sticker->size.x;
		delta.y /= sticker->size.y;

		// Perform delta check
		if ( delta.sqrMagnitude() < 1.0 )
		{
			terrain = sticker->terrain;
		}
	}

	return terrain;
}
uint8_t CWorldGen_Terran::GetBiomeAt ( Vector3d_d& worldPosition )
{
	EBiomeType biome = BIO_DEFAULT;

	return biome;
}


// =============================
//     == Sector Creation ==
// =============================
void CWorldGen_Terran::GenerateBlockAt ( inputTerrain_t& n_terrain, Payload* n_buffer, const Vector3d_d& worldPosition, terra_b& o_block ) 
{
	throw std::exception( "Don't use." );
}

void CWorldGen_Terran::GenerateSector  ( inputTerrain_t& n_terrain, Payload* n_buffer, GamePayload* n_gamedata, const Vector3d_d& nMin, const Vector3d_d& nMax, const RangeVector& nIndexer ) 
{
	// Create terrain accessor
	Terrain::SidebufferAccessor accessor (n_terrain.terrain,n_buffer->data,Terrain::SectorDim);
	// Create volume editor for 3D changes
	Terrain::SidebufferVolumeEditor editor (&accessor,nMin);

	// Create quick sending struct (smallerize code)
	quickAccessData	generationData;
	generationData.terrain		= n_terrain.terrain;
	generationData.buffer		= n_buffer;
	generationData.gamedata		= n_gamedata;
	generationData.pMin			= nMin;
	generationData.pMax			= nMax;
	generationData.indexer		= nIndexer;
	generationData.accessor = &accessor;
	generationData.editor	= &editor;
	// Grab the initial sample
	Generate_MacroSample( generationData );

	// Generate the terrain
	Generate_Terrain( generationData );
	// Generate the cave/patterns
	//Generate_Caves( generationData );
	// Generate the rivers
	//Generate_Rivers( generationData );
	// Generate the patterns
	Generate_Patterns( generationData, n_terrain );
	// Generate the biomes
	Generate_Biomes( generationData );
	//Generate_Architecture_Test( generationData );
	Generate_Lighting_Temporary( generationData );
}
