
#include "core/math/Math.h"
#include "core/math/Math3d.h"
#include "core/math/noise/SimplexNoise.h"
#include "core/math/random/Random.h"

#include "CWorldGen_Terran.h"

#include "after/entities/foliage/CTerraFoliageFactory.h"
#include "after/entities/props/CTerrainPropFactory.h"

#include "after/terrain/edit/csg/SidebufferVolumeEditor.h"
#include "after/terrain/edit/SidebufferAccessor.h"
#include "after/terrain/VoxelTerrain.h"
#include "after/types/terrain/BlockType.h"
#include "after/types/terrain/GrassTypes.h"

//#include "../../../COctreeRenderer.h"
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
}


// GetLandmassity( ) : Returns parametric distance from the coast. Positive for land, negative for water.
// firstPass will abort the checks on the first land hit.
Real CWorldGen_Terran::GetLandmassity ( const Vector2d_d& worldposition, const bool firstPass )
{
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
			//if ( delta.magnitude() < 150000 + 130000*noise->Get3D( samplePosition.x, samplePosition.y, i+1 ) ) {
			//	return true;
			//}
			Real_d radius = 150000 + 130000*noise->Get3D( samplePosition.x, samplePosition.y, i+1 );
			//if ( delta.magnitude() < radius ) {
			landmassity = (Real)( std::max<Real_d>( landmassity, (radius-delta.magnitude())/150000 ) ); // Double precision needed during calculation, but not for final value
			/*}
			else {
				landmassity = std::max<Real>( landmassity, (radius-delta.magnitude())/150000 );
			}*/
			if ( firstPass && landmassity > 0 ) {
				return landmassity;
			}
		}
	}
	return landmassity;
}

// ===================
// == Biome utility ==
// ===================
bool TerraGen_3p_CanMakeTree ( const Vector3d_d & blockPosition, ftype density )
{
	/*ftype divSize = (1/32.0f)/density;
	Vector3d gridOffset (
		fmodf( fabs(blockPosition.x), divSize ) - divSize/2,
		fmodf( fabs(blockPosition.y), divSize ) - divSize/2,
		fmodf( fabs(blockPosition.z*2.0f), divSize ) - divSize/2
		);*/
	ftype divSize = (0.5)/density;
	Vector3d_d gridOffset (
		fmod( abs(blockPosition.x*0.5), divSize ) - divSize/2,
		fmod( abs(blockPosition.y*0.5), divSize ) - divSize/2,
		fmod( abs(blockPosition.z), divSize ) - divSize/2
		);
	/*gridOffset.x += noise_hhf->Get3D( gridOffset.x,blockPosition.x*4.2f,blockPosition.z*3.4f )*divSize*0.632f*0.5f;
	gridOffset.y += noise_hhf->Get3D( blockPosition.y*4.3f,gridOffset.y,blockPosition.z*3.3f )*divSize*0.632f*0.5f;
	gridOffset.z += noise_hhf->Get3D( blockPosition.z*4.4f,blockPosition.y*3.2f,gridOffset.z )*divSize*0.632f*0.5f;*/
	//gridOffset *= 32;
	if ( gridOffset.sqrMagnitude() < sqr(0.632) ) {
		return true;
	}
	return false;
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

	// Generate the terrain
	Generate_Terrain( generationData );
	// Generate the cave/patterns
	//Generate_Caves( generationData );
	// Generate the patterns
	/*Generate_Patterns( generationData );
	// Generate the biomes
	Generate_Biomes( generationData );*/
	Generate_Architecture_Test( generationData );
}


struct _block_passinfo {
	Real_d x, y, z;
	int32_t subX, subY, subZ;
	Real stepX, stepY, stepZ;
};
struct landmassity_baseline {
	virtual Real operator() ( Real* landmassity_sample, Real subX, Real subY, Real_d x, Real_d y, CWorldGen_Terran* target ) =0;
};
struct density_baseline {
	landmassity_baseline* hm_gen;
	virtual Real operator() ( Real* landmassity_sample, Real subX, Real subY, Real_d x, Real_d y, Real_d z, CWorldGen_Terran* target ) =0;
};
struct block_baseline {
	virtual void operator() ( Terrain::terra_b& block, Real z_difference, Real density, const _block_passinfo& bp, const quickAccessData& qd, CWorldGen_Terran* gen ) =0;
};


void CWorldGen_Terran::Generate_Terrain ( const quickAccessData& qd )
{
	// Create all the temporary variables
	Terrain::terra_b block;
	Real_d x,y,z;
	int32_t subX, subY, subZ;
	Real stepX, stepY, stepZ;
	// Calculate step size
	stepX = (Real)((qd.pMax.x - qd.pMin.x)/32);
	stepY = (Real)((qd.pMax.y - qd.pMin.y)/32);
	stepZ = (Real)((qd.pMax.z - qd.pMin.z)/32);

	// Calculate the landmassisity at each corner. (This is the approximate elevation)
	Real landmassity_sample [4];

	landmassity_sample[0] = GetLandmassity( Vector2d_d(qd.pMin.x,qd.pMin.y), false );
	landmassity_sample[1] = GetLandmassity( Vector2d_d(qd.pMax.x,qd.pMin.y), false );
	landmassity_sample[2] = GetLandmassity( Vector2d_d(qd.pMin.x,qd.pMax.y), false );
	landmassity_sample[3] = GetLandmassity( Vector2d_d(qd.pMax.x,qd.pMax.y), false );

	landmassity_sample[0] = 24.0f / 4000;
	landmassity_sample[1] = 24.0f / 4000;
	landmassity_sample[2] = 24.0f / 4000;
	landmassity_sample[3] = 24.0f / 4000;

	struct : public landmassity_baseline { // Noisy ground
		Real operator() ( Real* landmassity_sample, Real subX, Real subY, Real_d x, Real_d y, CWorldGen_Terran* target ) override
		{
			Real landmassity = Math.lerp( (subY/32.0),
				Math.lerp( (subX/32.0), landmassity_sample[0], landmassity_sample[1] ),
				Math.lerp( (subX/32.0), landmassity_sample[2], landmassity_sample[3] )
				);
			landmassity += (target->m_buf_general.sampleBufferMacro( x*0.2,y*0.2 ) - 128.0) / 50000.0;
			return landmassity;
		}
	} lg_noisyGround; // Creates the real elevation.
	struct : public landmassity_baseline { // Tall hills
		Real operator() ( Real* landmassity_sample, Real subX, Real subY, Real_d x, Real_d y, CWorldGen_Terran* target )
		{
			Real landmassity = Math.lerp( (subY/32.0),
				Math.lerp( (subX/32.0), landmassity_sample[0], landmassity_sample[1] ),
				Math.lerp( (subX/32.0), landmassity_sample[2], landmassity_sample[3] )
				);
			landmassity += (target->m_buf_general.sampleBufferMacro( x*0.2,y*0.2 ) - 128.0) / 50000.0;
			landmassity += (target->m_buf_general.sampleBufferMacro( x*0.017,y*0.017 ) - 128.0) / 6000.0;
			return landmassity;
		}
	} lg_tallHills;
	struct : public landmassity_baseline { // Hills variation
		Real operator() ( Real* landmassity_sample, Real subX, Real subY, Real_d x, Real_d y, CWorldGen_Terran* target )
		{
			Real landmassity = Math.lerp( (subY/32.0),
				Math.lerp( (subX/32.0), landmassity_sample[0], landmassity_sample[1] ),
				Math.lerp( (subX/32.0), landmassity_sample[2], landmassity_sample[3] )
				);
			landmassity += (target->m_buf_general.sampleBufferMacro( x*0.08,y*0.08 ) - 128.0) / 50000.0;
			landmassity += (target->m_buf_general.sampleBufferMacro( x*0.03,y*0.03 ) - 128.0) / 6000.0;
			return landmassity;
		}
	} lg_tallHills2;
	struct : public landmassity_baseline { // Chasm
		Real operator() ( Real* landmassity_sample, Real subX, Real subY, Real_d x, Real_d y, CWorldGen_Terran* target )
		{
			Real landmassity = Math.lerp( (subY/32.0),
				Math.lerp( (subX/32.0), landmassity_sample[0], landmassity_sample[1] ),
				Math.lerp( (subX/32.0), landmassity_sample[2], landmassity_sample[3] )
				);
			landmassity += (target->m_buf_general.sampleBufferMacro( x*0.2,y*0.2 ) - 128.0) / 50000.0;
			landmassity += floorf((target->m_buf_general.sampleBufferMacro( x*0.02,y*0.02 ) - 128.0) / 5.0)*5.0 / 1000.0; // Biiig mountains.
			return landmassity;
		}
	} lg_testChasm;
	struct : public landmassity_baseline { // Mountainous
		Real operator() ( Real* landmassity_sample, Real subX, Real subY, Real_d x, Real_d y, CWorldGen_Terran* target )
		{
			Real landmassity = Math.lerp( (subY/32.0),
				Math.lerp( (subX/32.0), landmassity_sample[0], landmassity_sample[1] ),
				Math.lerp( (subX/32.0), landmassity_sample[2], landmassity_sample[3] )
				);
			landmassity += (target->m_buf_general.sampleBufferMacro( x*0.2,y*0.2 ) - 128.0) / 50000.0;
			landmassity += (target->m_buf_general.sampleBufferMacro( x*0.02,y*0.02 ) - 128.0) / 300.0;
			return landmassity;
		}
	} lg_testMountainous;
	struct : public landmassity_baseline { // Chillaxed and duneish
		Real operator() ( Real* landmassity_sample, Real subX, Real subY, Real_d x, Real_d y, CWorldGen_Terran* target )
		{
			Real landmassity = Math.lerp( (subY/32.0),
				Math.lerp( (subX/32.0), landmassity_sample[0], landmassity_sample[1] ),
				Math.lerp( (subX/32.0), landmassity_sample[2], landmassity_sample[3] )
				);
			landmassity += (target->m_buf_general.sampleBufferMacro( x*0.1,y*0.1 ) - 128.0) / 50000.0;
			landmassity += (target->m_buf_general.sampleBufferMacro( x*0.02,y*0.02 ) - 128.0) / 20000.0;
			return landmassity;
		}
	} lg_dunes;
	struct : public landmassity_baseline { // Steppish
		Real operator() ( Real* landmassity_sample, Real subX, Real subY, Real_d x, Real_d y, CWorldGen_Terran* target )
		{
			Real landmassity = Math.lerp( (subY/32.0),
				Math.lerp( (subX/32.0), landmassity_sample[0], landmassity_sample[1] ),
				Math.lerp( (subX/32.0), landmassity_sample[2], landmassity_sample[3] )
				);
			landmassity += (target->m_buf_general.sampleBufferMacro( x*0.1,y*0.1 ) - 128.0) / 50000.0;
			landmassity += floorf((target->m_buf_general.sampleBufferMacro( x*0.03,y*0.03 ) - 128.0) / 9.0)*9.0 / 2000.0;
			return landmassity;
		}
	} lg_test_steppes;
	struct : public landmassity_baseline { // New outlands.
		Real operator() ( Real* landmassity_sample, Real subX, Real subY, Real_d x, Real_d y, CWorldGen_Terran* target )
		{
			Real landmassity = Math.lerp( (subY/32.0),
				Math.lerp( (subX/32.0), landmassity_sample[0], landmassity_sample[1] ),
				Math.lerp( (subX/32.0), landmassity_sample[2], landmassity_sample[3] )
				);
			landmassity += (target->m_buf_general.sampleBufferMacro( x*0.05,y*0.05 ) - 128.0) / 50000.0;
			landmassity += (target->m_buf_general.sampleBufferMacro( x*0.02,y*0.02 ) - 128.0) / 10000.0;

			landmassity += std::max<Real>( (target->m_buf_general.sampleBufferMacro( x*0.3,y*0.3 ) - 170.0) / 4000.0, 0 );
			landmassity -= std::max<Real>( (target->m_buf_general.sampleBufferMacro( x*0.27,y*0.27 ) - 180.0) / 4000.0, 0 );
			return landmassity;
		}
	} lg_test_outlands;
	struct : public landmassity_baseline {
		Real operator() ( Real* landmassity_sample, Real subX, Real subY, Real_d x, Real_d y, CWorldGen_Terran* target )
		{
			return 24.0f / 4000;
		}
	} lg_test_flat;
	
	//landmassity_baseline* landmassityGenerator = &lg_test_flat;//&lg_dunes;//lg_noisyGround;
	landmassity_baseline* landmassityGenerator = &lg_dunes;

	struct : public density_baseline { // Perturb tall hills
		Real operator() ( Real* landmassity_sample, Real subX, Real subY, Real_d x, Real_d y, Real_d z, CWorldGen_Terran* target ) override
		{
			// Sample the heightmap first
			Real landmassity = (*hm_gen)( landmassity_sample, subX, subY, x,y, target );
			//return landmassity; // Below above zero means solid.
			// All blocks on the surface should be in the density range of 0-2

			Real density = landmassity*4000 - z; // When z is smaller than landmassity*4000, Z will be 0-2 for surface, >2 for underground, <0 above ground

			Real peturbation;
			Real_d tx, ty, tz;
			tx = x; 
			ty = y;
			tz = z;

			// Add some warp to the position
			{
				Real pet_x = -256.0f
					+ target->m_buf_general.sampleBufferMacro( tx*0.0076,tz*0.0076 )
					+ target->m_buf_general.sampleBufferMacro( ty*0.0076,tx*0.0076 );
				Real pet_y = -256.0f
					+ target->m_buf_general.sampleBufferMacro( tz*0.0076,ty*0.0076 )
					+ target->m_buf_general.sampleBufferMacro( ty*0.0076,tx*0.0076 );
				Real pet_z = -256.0f
					+ target->m_buf_general.sampleBufferMacro( tx*0.0076,tz*0.0076 )
					+ target->m_buf_general.sampleBufferMacro( tz*0.0076,ty*0.0076 );
				tx += pet_x * 0.5f;
				ty += pet_y * 0.5f;
				tz += pet_z * 0.5f;
			}

			// Add some hi-res noise
			//density += (target->m_buf_general.sampleBufferMacro( tx*0.278,ty*0.278 ) - 128.0) * 0.1f;

			// Add some med-res noise
			density += (target->m_buf_general.sampleBufferMacro( tx*0.077,ty*0.077 ) - 128.0) * 0.15f;

			// Add some Z peturbation
			/*peturbation = -384.0f
				+ target->m_buf_general.sampleBufferMacro( tx*0.103,tz*0.103 )
				+ target->m_buf_general.sampleBufferMacro( tz*0.103,ty*0.103 )
				+ target->m_buf_general.sampleBufferMacro( ty*0.103,tx*0.103 );
			density += peturbation * 0.17f;
			peturbation = -384.0f
				+ target->m_buf_general.sampleBufferMacro( tx*0.067,ty*0.067 )
				+ target->m_buf_general.sampleBufferMacro( tz*0.067,tx*0.067 )
				+ target->m_buf_general.sampleBufferMacro( tz*0.067,ty*0.067 );
			density += peturbation * 0.37f;*/

			return density;
		}
	} dg_Peturb; // Creates the real elevation.
	struct : public density_baseline { // Perturb to cliffs
		Real operator() ( Real* landmassity_sample, Real subX, Real subY, Real_d x, Real_d y, Real_d z, CWorldGen_Terran* target ) override
		{
			// Sample the heightmap first
			Real landmassity = (*hm_gen)( landmassity_sample, subX, subY, x,y, target );
			//return landmassity; // Below above zero means solid.
			// All blocks on the surface should be in the density range of 0-2

			Real density = landmassity*4000 - z; // When z is smaller than landmassity*4000, Z will be 0-2 for surface, >2 for underground, <0 above ground

			Real peturbation;
			Real_d tx, ty, tz;
			// Set initial positions
			tx = x; 
			ty = y;
			tz = z;

			// Add medium res noise
			peturbation = -384.0f
				+ target->m_buf_general.sampleBufferMacro( tx*0.103*0.8f,tz*0.043*0.8f )
				+ target->m_buf_general.sampleBufferMacro( tz*0.043*0.8f,ty*0.103*0.8f )
				+ target->m_buf_general.sampleBufferMacro( ty*0.103*0.8f,tx*0.103*0.8f );
			density += peturbation * 0.47f;
			peturbation = -384.0f
				+ target->m_buf_general.sampleBufferMacro( tx*0.027*0.7f,ty*0.027*0.7f )
				+ target->m_buf_general.sampleBufferMacro( tz*0.013*0.7f,tx*0.027*0.7f )
				+ target->m_buf_general.sampleBufferMacro( tz*0.013*0.7f,ty*0.027*0.7f );
			density += peturbation * 0.67f;

			// Add a step
			density = std::min<ftype>( landmassity*4000 - z + 14, density );
			density = std::max<ftype>( landmassity*4000 - z - 30, density );

			// Add super low res noise
			peturbation = -384.0f
				+ target->m_buf_general.sampleBufferMacro( tx*0.039,ty*0.039 )
				+ target->m_buf_general.sampleBufferMacro( tz*0.039,tx*0.039 )
				+ target->m_buf_general.sampleBufferMacro( tz*0.039,ty*0.039 );
			density += peturbation * 0.52f;
			// Add medium-high res noise
			peturbation = -384.0f
				+ target->m_buf_general.sampleBufferMacro( tx*0.173,tz*0.173 )
				+ target->m_buf_general.sampleBufferMacro( tz*0.173,ty*0.173 )
				+ target->m_buf_general.sampleBufferMacro( ty*0.173,tx*0.173 );
			density += peturbation * 0.28f;

			// Add a step
			density = std::min<ftype>( landmassity*4000 - z, density );

			// Add some warp to the position
			{
				Real pet_x = -256.0f
					+ target->m_buf_general.sampleBufferMacro( tx*0.0076,tz*0.0076 )
					+ target->m_buf_general.sampleBufferMacro( ty*0.0076,tx*0.0076 );
				Real pet_y = -256.0f
					+ target->m_buf_general.sampleBufferMacro( tz*0.0076,ty*0.0076 )
					+ target->m_buf_general.sampleBufferMacro( ty*0.0076,tx*0.0076 );
				Real pet_z = -256.0f
					+ target->m_buf_general.sampleBufferMacro( tx*0.0076,tz*0.0076 )
					+ target->m_buf_general.sampleBufferMacro( tz*0.0076,ty*0.0076 );
				tx += pet_x * 0.5f;
				ty += pet_y * 0.5f;
				tz += pet_z * 0.5f;
			}

			// Add some med-res noise
			density += (target->m_buf_general.sampleBufferMacro( tx*0.077,ty*0.077 ) - 128.0) * 0.08f;

			// Add some Z peturbation
			peturbation = -384.0f
				+ target->m_buf_general.sampleBufferMacro( tx*0.087,ty*0.087 )
				+ target->m_buf_general.sampleBufferMacro( tz*0.087,tx*0.087 )
				+ target->m_buf_general.sampleBufferMacro( tz*0.087,ty*0.087 );
			density += peturbation * 0.35f;

			// Add a step
			density = std::max<ftype>( landmassity*4000 - z - 60, density );
			
			return density;
		}
	} dg_CliffwisePeturb; // Creates the real elevation.
	struct : public density_baseline { // Straight send
		Real operator() ( Real* landmassity_sample, Real subX, Real subY, Real_d x, Real_d y, Real_d z, CWorldGen_Terran* target ) override
		{
			// Sample the heightmap first
			Real landmassity = (*hm_gen)( landmassity_sample, subX, subY, x,y, target );
			Real_d density = landmassity*4000 - z; // When z is smaller than landmassity*4000, Z will be 0-2 for surface, >2 for underground, <0 above ground
			return (Real)density;
		}
	} dg_Straight; // Creates the real elevation.

	density_baseline* densityGenerator = &dg_Straight;//&dg_CliffwisePeturb;
	densityGenerator->hm_gen = landmassityGenerator;

	/*struct _block_generator {
		void operator() ( Terrain::terra_b& block, Real landmassity, const _block_passinfo& bp, const quickAccessData& qd, CWorldGen_Terran* gen )
		{
			Real zDifference = landmassity*4000-bp.z;
			if ( bp.z < landmassity*4000 )
			{
				Real randomValue = gen->m_buf_general_noise.sampleBufferMacro( bp.x*0.04*5,bp.y*0.05*5 )+gen->m_buf_general_noise.sampleBufferMacro( bp.y*0.82*0.7,bp.x*0.77*0.7 );
				//if ( randomValue > 328-zDifference ) {
				if ( randomValue > 328 ) {
					block.block = EB_STONE;
				}
				else if ( randomValue < 160 ) {
					block.block = EB_SAND;
				}
				else
				{
					block.block = EB_DIRT;
					if ( zDifference < 2 && randomValue > 190 && randomValue < 300 )
					{
						block.block = EB_GRASS;

						if ( randomValue > 200 && randomValue < 280 ) {
							sTerraGrass newgrass;
							newgrass.block.x_index = bp.subX;
							newgrass.block.y_index = bp.subY;
							newgrass.block.z_index = bp.subZ;
							newgrass.type = EG_DEFAULT;
							newgrass.position = Vector3d( bp.subX*bp.stepX+1+sinf(randomValue*4.127f)*0.8f, bp.subY*bp.stepY+1+sinf(randomValue*6.753f)*0.8f, bp.subZ*bp.stepZ+1.7f+zDifference );
							qd.gamedata->m_grass.push_back( newgrass );

							if ( TerraGen_3p_CanMakeTree( Vector3d_d(bp.x+cos(randomValue*5.623f),bp.y+cos(randomValue*2.887f),bp.z), 0.09f ) ) {
								sTerraFoiliage newtree;
								newtree.foliage_index = TerraFoliage::GetFoliageType( "CTreeBase" );
								newtree.position = Vector3d( bp.subX*bp.stepX + 1, bp.subY*bp.stepY + 1, bp.subZ*bp.stepZ+ 1 +zDifference );
								memset( newtree.userdata, 0xFF, 48 );
								qd.gamedata->m_foliage_queue.push_back( newtree );
							}
						}
					}
				}
			}
			else
			{
				if ( bp.z > 0 ) {
					block.block = EB_NONE;
				}
				else {
					block.block = EB_WATER;
				}
			}
		}
	} blockGenerator;*/
	struct : public block_baseline {
		void operator() ( Terrain::terra_b& block, Real z_difference, Real density, const _block_passinfo& bp, const quickAccessData& qd, CWorldGen_Terran* gen ) override
		{
			if ( density > 0 )
			{
				Real randomValue = gen->m_buf_general_noise.sampleBufferMacro( bp.x*0.04*5,bp.y*0.05*5 )+gen->m_buf_general_noise.sampleBufferMacro( bp.y*0.82*0.7,bp.x*0.77*0.7 );
				//if ( randomValue > 328-zDifference ) {
				if ( randomValue > 328 ) {
					block.block = EB_STONE;
				}
				else if ( randomValue < 160 ) {
					block.block = EB_SAND;
				}
				else
				{
					block.block = EB_DIRT;
					if ( density < 2 && randomValue > 190 && randomValue < 300 && Terrain::_normal_bias(block.normal_y_z) > -0.1f && Terrain::_normal_bias(block.normal_x_z) > -0.1f  )
					{
						block.block = EB_GRASS;

						if ( randomValue > 200 && randomValue < 280 ) {
							grass_t newgrass;
							newgrass.block.x_index = bp.subX;
							newgrass.block.y_index = bp.subY;
							newgrass.block.z_index = bp.subZ;
							newgrass.type = EG_DEFAULT;
							newgrass.position = Vector3d( bp.subX*bp.stepX+1+sinf(randomValue*4.127f)*0.8f, bp.subY*bp.stepY+1+sinf(randomValue*6.753f)*0.8f, bp.subZ*bp.stepZ+1.7f+density );
							qd.gamedata->grass.push_back( newgrass );

							if ( z_difference < 0 && TerraGen_3p_CanMakeTree( Vector3d_d(bp.x+cos(randomValue*5.623f),bp.y+cos(randomValue*2.887f),bp.z), 0.033f ) ) {
								foliage_t newtree;
								newtree.foliage_index = TerraFoliage::GetFoliageType( "CTreeBase" );
								newtree.position = Vector3d( bp.subX*bp.stepX + 1, bp.subY*bp.stepY + 1, bp.subZ*bp.stepZ+ 1 +density );
								memset( newtree.userdata, 0xFF, 48 );
							//	qd.gamedata->m_foliage_queue.push_back( newtree );
							}
						}
					}
				}
			}
			else
			{
				if ( bp.z > 0 ) {
					block.block = EB_NONE;
				}
				else {
					block.block = EB_WATER;
				}
			}
		}
	} bg_defaultBlocktypes;
	struct : public block_baseline {
		void operator() ( Terrain::terra_b& block, Real z_difference, Real density, const _block_passinfo& bp, const quickAccessData& qd, CWorldGen_Terran* gen ) override
		{
			if ( density > 0 )
			{
				Real randomValue = gen->m_buf_general_noise.sampleBufferMacro( bp.x*0.04*5,bp.y*0.05*5 )+gen->m_buf_general_noise.sampleBufferMacro( bp.y*0.82*0.7,bp.x*0.77*0.7 );
				//if ( randomValue > 328-zDifference ) {
				if ( randomValue > 328 ) {
					block.block = EB_STONE;
				}
				else if ( randomValue < 170 )
				{
					block.block = EB_SAND;
				}
				else
				{
					block.block = EB_DIRT;
					if ( randomValue > 180 && randomValue < 310 && Terrain::_normal_bias(block.normal_y_z) < 0.3f || Terrain::_normal_bias(block.normal_x_z) < 0.3f ) {
						block.block = EB_RIGDESTONE;
					}
					else if ( randomValue > 280 && randomValue < 310 ) {
						block.block = EB_SAND;
					}
					/*if ( density < 2 && randomValue > 190 && randomValue < 300 && Terrain::_normal_bias(block.normal_y_z) > -0.1f && Terrain::_normal_bias(block.normal_x_z) > -0.1f  )
					{
						block.block = EB_GRASS;

						if ( randomValue > 200 && randomValue < 280 ) {
							sTerraGrass newgrass;
							newgrass.block.x_index = bp.subX;
							newgrass.block.y_index = bp.subY;
							newgrass.block.z_index = bp.subZ;
							newgrass.type = EG_DEFAULT;
							newgrass.position = Vector3d( bp.subX*bp.stepX+1+sinf(randomValue*4.127f)*0.8f, bp.subY*bp.stepY+1+sinf(randomValue*6.753f)*0.8f, bp.subZ*bp.stepZ+1.7f+density );
							qd.gamedata->m_grass.push_back( newgrass );

							if ( z_difference < 0 && TerraGen_3p_CanMakeTree( Vector3d_d(bp.x+cos(randomValue*5.623f),bp.y+cos(randomValue*2.887f),bp.z), 0.033f ) ) {
								sTerraFoiliage newtree;
								newtree.foliage_index = TerraFoliage::GetFoliageType( "CTreeBase" );
								newtree.position = Vector3d( bp.subX*bp.stepX + 1, bp.subY*bp.stepY + 1, bp.subZ*bp.stepZ+ 1 +density );
								memset( newtree.userdata, 0xFF, 48 );
							//	qd.gamedata->m_foliage_queue.push_back( newtree );
							}
						}
					}*/
				}
			}
			else
			{
				if ( bp.z > 0 ) {
					block.block = EB_NONE;
				}
				else {
					block.block = EB_WATER;
				}
			}
		}
	} bg_defaultDesertWasteland;
	struct : public block_baseline {
		void operator() ( Terrain::terra_b& block, Real z_difference, Real density, const _block_passinfo& bp, const quickAccessData& qd, CWorldGen_Terran* gen ) override
		{
			if ( density > 0 )
			{
				Real randomValue = gen->m_buf_general_noise.sampleBufferMacro( bp.x*0.04*5,bp.y*0.05*5 )+gen->m_buf_general_noise.sampleBufferMacro( bp.y*0.82*0.7,bp.x*0.77*0.7 );
				//if ( randomValue > 328-zDifference ) {
				if ( randomValue > 338 ) {
					block.block = EB_STONE;
				}
				else if ( randomValue < 294 )
				{
					block.block = EB_SAND;
				}
				else
				{
					block.block = EB_DIRT;
					if ( randomValue > 180 && randomValue < 310 && Terrain::_normal_bias(block.normal_y_z) < 0.3f || Terrain::_normal_bias(block.normal_x_z) < 0.3f ) {
						block.block = EB_RIGDESTONE;
					}
					else if ( randomValue > 297 && randomValue < 310 ) {
						block.block = EB_SAND;
					}
					else if ( randomValue > 314 && randomValue < 324 )
					{
						if ( density < 2 )
						{
							grass_t newgrass;
							newgrass.block.x_index = bp.subX;
							newgrass.block.y_index = bp.subY;
							newgrass.block.z_index = bp.subZ;
							newgrass.type = EG_DRY;
							newgrass.position = Vector3d( bp.subX*bp.stepX+1+sinf(randomValue*4.127f)*0.8f, bp.subY*bp.stepY+1+sinf(randomValue*6.753f)*0.8f, bp.subZ*bp.stepZ+1.7f+density );

							qd.gamedata->grass.push_back( newgrass );
						}
					}
				}
			}
			else
			{
				if ( bp.z > 0 ) {
					block.block = EB_NONE;
				}
				else {
					block.block = EB_WATER;
				}
			}
		}
	} bg_defaultDesert;
	struct : public block_baseline {
		void operator() ( Terrain::terra_b& block, Real z_difference, Real density, const _block_passinfo& bp, const quickAccessData& qd, CWorldGen_Terran* gen ) override
		{
			if ( density > 0 )
			{
				Real randomValue = gen->m_buf_general_noise.sampleBufferMacro( bp.x*0.04*5,bp.y*0.05*5 )+gen->m_buf_general_noise.sampleBufferMacro( bp.y*0.82*0.7,bp.x*0.77*0.7 );
				//if ( randomValue > 328-zDifference ) {
				if ( randomValue > 328 ) {
					block.block = EB_STONE;
				}
				else
				{
					if ( density < 30 ) {
						block.block = EB_STONEBRICK;
					}
					else {
						block.block = EB_DIRT;
					}
				}
			}
			else
			{
				if ( bp.z > 0 ) {
					block.block = EB_NONE;
				}
				else {
					block.block = EB_WATER;
				}
			}
		}
	} bg_stoneTest;
	//block_baseline* blockGenerator = &bg_stoneTest;//&bg_defaultDesertWasteland;
	block_baseline* blockGenerator = &bg_defaultDesert;//bg_defaultDesertWasteland;

	_block_passinfo block_passinfo;
	block_passinfo.stepX = stepX;
	block_passinfo.stepY = stepY;
	block_passinfo.stepZ = stepZ;

	// Landmassity terrain generation
	/*{
		// Loop through sector buffer and set data
		for ( subX = 0; subX < 32; ++subX )
		{
			x = subX*stepX + qd.pMin.x + COctreeTerrain::blockSize*0.5;
			block_passinfo.subX = subX;
			block_passinfo.x = x;
			for ( subY = 0; subY < 32; ++subY )
			{
				y = subY*stepY + qd.pMin.y + COctreeTerrain::blockSize*0.5;
				block_passinfo.subY = subY;
				block_passinfo.y = y;
				// Calculate landmassity at this location
				Real landmassity_cn = landmassityGenerator( landmassity_sample, subX,subY, x,y, this );
				Real landmassity_px = landmassityGenerator( landmassity_sample, subX+1,subY, x+COctreeTerrain::blockSize,y, this );
				Real landmassity_py = landmassityGenerator( landmassity_sample, subX,subY+1, x,y+COctreeTerrain::blockSize, this );

				// Calculate the surface normal
				Vector3d dirNormalX ( COctreeTerrain::blockSize,0,(landmassity_px-landmassity_cn)*4000 );
				Vector3d dirNormalY ( 0,COctreeTerrain::blockSize,(landmassity_py-landmassity_cn)*4000 );
				Vector3d dirNormalZ = dirNormalX.cross( dirNormalY );
				dirNormalZ.normalize();

				// Grab the average height
				Real landmassity = landmassity_cn;

				// Loop through the blocks
				for ( subZ = 0; subZ < 32; ++subZ )
				{
					z = subZ*stepZ + qd.pMin.z + COctreeTerrain::blockSize*0.5;
					block_passinfo.subZ = subZ;
					block_passinfo.z = z;
					// x,y,z are now set to proper values

					// Reset block
					block.raw = 0;

					Real zDifference = landmassity*4000-z;

					Vector3d blockNormal = dirNormalZ;

					Vector3d blockNormalW ( 0,0,0 );
					blockNormalW.z = zDifference;
					blockNormalW.x = blockNormalW.z * ( dirNormalX.x / -dirNormalX.z );
					blockNormalW.y = blockNormalW.z * ( dirNormalY.y / -dirNormalY.z );

					// Set normals (default smooth normals from Landmassity)
					int normal_x, normal_y, normal_z;
					normal_x = Terrain::_normal_unbias( blockNormal.x );
					normal_y = Terrain::_normal_unbias( blockNormal.y );
					normal_z = Terrain::_normal_unbias( blockNormal.z );
					block.normal_y_x = normal_x;
					block.normal_z_x = normal_x;
					block.normal_x_y = normal_y;
					block.normal_z_y = normal_y;
					block.normal_x_z = normal_z;
					block.normal_y_z = normal_z;
					// Set block offset
					block.normal_z_w = Terrain::_depth_unbias( blockNormalW.z / COctreeTerrain::blockSize );
					block.normal_x_w = Terrain::_depth_unbias( blockNormalW.x / COctreeTerrain::blockSize );
					block.normal_y_w = Terrain::_depth_unbias( blockNormalW.y / COctreeTerrain::blockSize );

					// Generate terrain
					blockGenerator( block, landmassity, block_passinfo, qd, this );

					// Generate sidebuffer information
					qd.accessor->SetDataAtSidebufferPosition( block.raw, RangeVector(subX,subY,subZ) );
				}
			}
		}
	}*/
	// Density terrain generation
	{
		// Loop through sector buffer and set data
		for ( subX = 0; subX < 32; ++subX )
		{
			x = subX*stepX + qd.pMin.x + Terrain::BlockSize*0.5;
			block_passinfo.subX = subX;
			block_passinfo.x = x;
			for ( subY = 0; subY < 32; ++subY )
			{
				y = subY*stepY + qd.pMin.y + Terrain::BlockSize*0.5;
				block_passinfo.subY = subY;
				block_passinfo.y = y;

				Real landmassity = (*landmassityGenerator)( landmassity_sample, subX,subY, x,y, this );

				// Loop through the blocks
				for ( subZ = 0; subZ < 32; ++subZ )
				{
					z = subZ*stepZ + qd.pMin.z + Terrain::BlockSize*0.5;
					block_passinfo.subZ = subZ;
					block_passinfo.z = z;
					// x,y,z are now set to proper values
					
					// Calculate density at this location
					Vector3d density_p = Vector3d(
						(*densityGenerator)( landmassity_sample, subX+1,subY, x+Terrain::BlockSize,y,z, this ),
						(*densityGenerator)( landmassity_sample, subX,subY+1, x,y+Terrain::BlockSize,z, this ),
						(*densityGenerator)( landmassity_sample, subX,subY,   x,y,z+Terrain::BlockSize, this )
						);
					Vector3d density_n = Vector3d(
						(*densityGenerator)( landmassity_sample, subX-1,subY, x-Terrain::BlockSize,y,z, this ),
						(*densityGenerator)( landmassity_sample, subX,subY-1, x,y-Terrain::BlockSize,z, this ),
						(*densityGenerator)( landmassity_sample, subX,subY,   x,y,z-Terrain::BlockSize, this )
						);
					Real density = (*densityGenerator)( landmassity_sample, subX,subY, x,y,z, this );
					{
						/*Real sdensity = (density_p.x + density_p.y + density_p.z + density_n.x + density_n.y + density_n.z)/6.0f;
						if ( sdensity < density ) {
							density = sdensity;
						}*/
					}

					// Calculate the surface normal
					Vector3d dirNormal = density_n-density_p;
					dirNormal.normalize();

					// Reset block
					block.raw = 0;

					// Generate normal info
					Vector3d blockNormal = dirNormal;
					Vector3d blockNormalW ( 0,0,0 );
					//blockNormalW.x = 0.5;//1.0f + (1.0f/( (density_p.x/density_n.x) - 1.0f ));
					//blockNormalW.y = 0.5;//1.0f + (1.0f/( (density_p.y/density_n.y) - 1.0f ));
					//blockNormalW.z = 1.0f + (1.0f/( (density_p.z/density_n.z) - 1.0f ));
					blockNormalW.x = 1.0f - (density_p.x)/(density_p.x-density);
					blockNormalW.y = 1.0f - (density_p.y)/(density_p.y-density);
					blockNormalW.z = 1.0f - (density_p.z)/(density_p.z-density);

					/*-0.5 to 0.5
					-0.5 to 0.5 would be a 0.5
					-1 to 2 would be a 2/3
					so total is (density_p.x-density)
					-2 to 1 would be a 1/3
					 n    p
					 1 / (3)

					1 to -2 would be 2/3
					n     p
					-2 / (-3)
					
					n	  p
					2 to -1 would be 1/3
					-1 / (-3)

					(value)/(density_p.x-density)

					value is ((density_p.x-density)+density)/(density_p.x-density)
					(a+b)/a

					(1+b/a)*/
					/*blockNormalW.x = 1.0f + density/(density_p.x-density);
					blockNormalW.y = 1.0f + density/(density_p.y-density);
					blockNormalW.z = 1.0f + density/(density_p.z-density);*/

					// Set normals (default smooth normals from Landmassity)
					int normal_x, normal_y, normal_z;
					normal_x = Terrain::_normal_unbias( blockNormal.x );
					normal_y = Terrain::_normal_unbias( blockNormal.y );
					normal_z = Terrain::_normal_unbias( blockNormal.z );
					block.normal_y_x = normal_x;
					block.normal_z_x = normal_x;
					block.normal_x_y = normal_y;
					block.normal_z_y = normal_y;
					block.normal_x_z = normal_z;
					block.normal_y_z = normal_z;
					// Set block offset
					block.normal_x_w = Terrain::_depth_unbias( blockNormalW.x );
					block.normal_y_w = Terrain::_depth_unbias( blockNormalW.y );
					block.normal_z_w = Terrain::_depth_unbias( blockNormalW.z );

					// Set light based on density
					//block.light_r = std::min<int>( 7, std::max<int>( 0, (int)(14-density*0.2f) ) );
					//block.light_r = std::min<int>( 15, std::max<int>( 0, (int)(-5+density*0.2f) ) );
					block.light_r = std::min<int>( 15, std::max<int>( 0, (int)(-1+density*0.6f) ) );

					// Generate terrain
					(*blockGenerator)( block, landmassity*4000 - z, density, block_passinfo, qd, this );
					if ( Terrain::Checker::BlocktypeOpaque(block.block) ) {
						block.smooth_normal = 1;
					}
					
					// Generate sidebuffer information
					qd.accessor->SetData( block.raw, RangeVector(subX,subY,subZ) );
				}
			}
		}
	}

	// Now, at the end, do the 3D editing
	/*{
		double xOffset = (m_buf_general.sampleBufferMacro( x*3.123,y*3.423 ) - 128.0)*0.1 + 32;
		double yOffset = (m_buf_general.sampleBufferMacro( x*3.023,y*3.523 ) - 128.0)*0.1 + 32;
		double zOffset = (m_buf_general.sampleBufferMacro( x*3.332,y*3.451 ) - 128.0)*0.1 + 32;

		editor.Sub_Sphere( Vector3d_d( nMin.x+xOffset, nMin.y+yOffset, nMin.z+zOffset ), 20.0f );
	}*/

	// For outlands, create a series of 4 spheres in the area
	/*{
		double rootPosition = ((landmassity_sample[0]+landmassity_sample[1]+landmassity_sample[2]+landmassity_sample[3])/4)*4000;
		for ( uint rp = 1; rp < 4; ++rp )
		{
			double xOffset = (m_buf_general.sampleBufferMacro( x*3.123*rp,y*3.423*rp ) - 128.0)*0.15*rp + 32;
			double yOffset = (m_buf_general.sampleBufferMacro( x*3.023*rp,y*3.523*rp ) - 128.0)*0.15*rp + 32;
			double zOffset = (m_buf_general.sampleBufferMacro( x*3.332*rp,y*3.451*rp ) - 128.0)*0.19*rp + 20;

			double sizer = std::max<double>( 0, std::min<double>( 1, (rootPosition-(nMin.z+zOffset))/128 + 0.9f ) );

			editor.Add_Sphere( Vector3d_d( nMin.x+xOffset, nMin.y+yOffset, nMin.z+zOffset ), (sizer*99.0)/(rp+5), EB_DIRT );
		}
		for ( uint rp = 1; rp < 3; ++rp )
		{
			double xOffset = (m_buf_general.sampleBufferMacro( x*2.123*rp,y*2.423*rp ) - 128.0)*0.15*rp + 32;
			double yOffset = (m_buf_general.sampleBufferMacro( x*2.023*rp,y*2.523*rp ) - 128.0)*0.15*rp + 32;
			double zOffset = (m_buf_general.sampleBufferMacro( x*2.332*rp,y*2.451*rp ) - 128.0)*0.19*rp + 20;

			double sizer = std::max<double>( 0, std::min<double>( 1, (rootPosition-(nMin.z+zOffset))/128 + 0.9f ) );

			editor.Sub_Sphere( Vector3d_d( nMin.x+xOffset, nMin.y+yOffset, nMin.z+zOffset ), (sizer*130.0)/(rp+5) );
		}
	}*/

	// Loop across area for grass
	// Loop through sector buffer and set data
	/*for ( subX = 0; subX < 32; ++subX )
	{
		//x = subX*stepX + nMin.x + COctreeTerrain::blockSize*0.5;
		for ( subY = 0; subY < 32; ++subY )
		{
			//y = subY*stepY + nMin.y + COctreeTerrain::blockSize*0.5;
			// Loop through the blocks
			for ( subZ = 0; subZ < 31; ++subZ )
			{
				//z = subZ*stepZ + nMin.z + COctreeTerrain::blockSize*0.5;
				accessor.GetDataAtSidebufferPosition( block.raw, RangeVector(subX,subY,subZ+1) );

				if ( block.block == EB_NONE )
				{
					accessor.GetDataAtSidebufferPosition( block.raw, RangeVector(subX,subY,subZ) );
					if ( block.block == EB_DIRT ) {
						block.block = EB_GRASS;
						accessor.SetDataAtSidebufferPosition( block.raw, RangeVector(subX,subY,subZ) );
					}
				}
			}
		}
	}*/
}
