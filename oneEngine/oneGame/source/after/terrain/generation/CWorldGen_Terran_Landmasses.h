
#ifndef _C_WORLD_GEN_TERRAIN_LANDMASSES_WORLDTYPES_H_
#define _C_WORLD_GEN_TERRAIN_LANDMASSES_WORLDTYPES_H_

#include "core/math/Math.h"
#include "after/types/terrain/BlockType.h"

#include "CWorldGen_Terran.h"

namespace Terrain
{
	struct _block_passinfo {
		Real_d x, y, z;
		int32_t subX, subY, subZ;
		Real stepX, stepY, stepZ;
	};
	struct landmassity_baseline {
		virtual Real work ( const Real* landmassity_sample, Real subX, Real subY, Real_d x, Real_d y, CWorldGen_Terran* target ) =0;
	};
	struct density_baseline {
		landmassity_baseline* hm_gen;
		virtual Real work ( const Real* landmassity_sample, Real subX, Real subY, Real_d x, Real_d y, Real_d z, CWorldGen_Terran* target ) =0;
	};
	struct block_baseline {
		virtual void work ( Terrain::terra_b& block, Real z_difference, Real density, const _block_passinfo& bp, const quickAccessData& qd, CWorldGen_Terran* gen ) =0;
	};

	// ==================================================================
	//
	// Landmassity Samplers
	//
	// ==================================================================

	struct lg_noisyGround_t : public landmassity_baseline { // Noisy ground
		Real work ( const Real* landmassity_sample, Real subX, Real subY, Real_d x, Real_d y, CWorldGen_Terran* target ) override
		{
			Real landmassity = Math.lerp( (subY/32.0),
				Math.lerp( (subX/32.0), landmassity_sample[0], landmassity_sample[1] ),
				Math.lerp( (subX/32.0), landmassity_sample[2], landmassity_sample[3] )
				);
			landmassity += (target->m_buf_general.sampleBufferMacro( x*0.2,y*0.2 ) - 128.0) / 12.5;
			return landmassity;
		}
	}; // Creates the real elevation.
	struct lg_tallHills_t : public landmassity_baseline { // Tall hills
		Real work ( const Real* landmassity_sample, Real subX, Real subY, Real_d x, Real_d y, CWorldGen_Terran* target )
		{
			Real landmassity = Math.lerp( (subY/32.0),
				Math.lerp( (subX/32.0), landmassity_sample[0], landmassity_sample[1] ),
				Math.lerp( (subX/32.0), landmassity_sample[2], landmassity_sample[3] )
				);
			landmassity += (target->m_buf_general.sampleBufferMacro( x*0.2,y*0.2 ) - 128.0) / 12.5;
			landmassity += (target->m_buf_general.sampleBufferMacro( x*0.017,y*0.017 ) - 128.0) / 1.5;
			return landmassity;
		}
	};
	struct lg_tallHills2_t : public landmassity_baseline { // Hills variation
		Real work ( const Real* landmassity_sample, Real subX, Real subY, Real_d x, Real_d y, CWorldGen_Terran* target )
		{
			Real landmassity = Math.lerp( (subY/32.0),
				Math.lerp( (subX/32.0), landmassity_sample[0], landmassity_sample[1] ),
				Math.lerp( (subX/32.0), landmassity_sample[2], landmassity_sample[3] )
				);
			landmassity += (target->m_buf_general.sampleBufferMacro( x*0.08,y*0.08 ) - 128.0) / 12.5;
			landmassity += (target->m_buf_general.sampleBufferMacro( x*0.03,y*0.03 ) - 128.0) / 1.5;
			return landmassity;
		}
	};
	struct lg_testChasm_t : public landmassity_baseline { // Chasm
		Real work ( const Real* landmassity_sample, Real subX, Real subY, Real_d x, Real_d y, CWorldGen_Terran* target )
		{
			Real landmassity = Math.lerp( (subY/32.0),
				Math.lerp( (subX/32.0), landmassity_sample[0], landmassity_sample[1] ),
				Math.lerp( (subX/32.0), landmassity_sample[2], landmassity_sample[3] )
				);
			landmassity += (target->m_buf_general.sampleBufferMacro( x*0.2,y*0.2 ) - 128.0) / 12.5;
			landmassity += floorf((target->m_buf_general.sampleBufferMacro( x*0.02,y*0.02 ) - 128.0) / 5.0)*5.0 / 0.25; // Biiig mountains.
			return landmassity;
		}
	};
	struct lg_testMountainous_t : public landmassity_baseline { // Mountainous
		Real work ( const Real* landmassity_sample, Real subX, Real subY, Real_d x, Real_d y, CWorldGen_Terran* target )
		{
			Real landmassity = Math.lerp( (subY/32.0),
				Math.lerp( (subX/32.0), landmassity_sample[0], landmassity_sample[1] ),
				Math.lerp( (subX/32.0), landmassity_sample[2], landmassity_sample[3] )
				);
			landmassity += (target->m_buf_general.sampleBufferMacro( x*0.2,y*0.2 ) - 128.0) / 12.5;
			landmassity += (target->m_buf_general.sampleBufferMacro( x*0.02,y*0.02 ) - 128.0) / 0.075;
			return landmassity;
		}
	};
	struct lg_dunes_t : public landmassity_baseline { // Chillaxed and duneish
		Real work ( const Real* landmassity_sample, Real subX, Real subY, Real_d x, Real_d y, CWorldGen_Terran* target )
		{
			Real landmassity = Math.lerp( (subY/32.0),
				Math.lerp( (subX/32.0), landmassity_sample[0], landmassity_sample[1] ),
				Math.lerp( (subX/32.0), landmassity_sample[2], landmassity_sample[3] )
				);
			landmassity += (target->m_buf_general.sampleBufferMacro( x*0.1,y*0.1 ) - 128.0) / 12.5;
			landmassity += (target->m_buf_general.sampleBufferMacro( x*0.02,y*0.02 ) - 128.0) / 5.0;
			return landmassity;
		}
	};
	struct lg_test_steppes_t : public landmassity_baseline { // Steppish
		Real work ( const Real* landmassity_sample, Real subX, Real subY, Real_d x, Real_d y, CWorldGen_Terran* target )
		{
			Real landmassity = Math.lerp( (subY/32.0),
				Math.lerp( (subX/32.0), landmassity_sample[0], landmassity_sample[1] ),
				Math.lerp( (subX/32.0), landmassity_sample[2], landmassity_sample[3] )
				);
			landmassity += (target->m_buf_general.sampleBufferMacro( x*0.1,y*0.1 ) - 128.0) / 12.5;
			landmassity += floorf((target->m_buf_general.sampleBufferMacro( x*0.03,y*0.03 ) - 128.0) / 9.0)*9.0 / 0.5;
			return landmassity;
		}
	};
	struct lg_test_outlands_t : public landmassity_baseline { // New outlands.
		Real work ( const Real* landmassity_sample, Real subX, Real subY, Real_d x, Real_d y, CWorldGen_Terran* target )
		{
			Real landmassity = Math.lerp( (subY/32.0),
				Math.lerp( (subX/32.0), landmassity_sample[0], landmassity_sample[1] ),
				Math.lerp( (subX/32.0), landmassity_sample[2], landmassity_sample[3] )
				);
			landmassity += (target->m_buf_general.sampleBufferMacro( x*0.05,y*0.05 ) - 128.0) / 12.5;
			landmassity += (target->m_buf_general.sampleBufferMacro( x*0.02,y*0.02 ) - 128.0) / 2.5;

			landmassity += std::max<Real>( (target->m_buf_general.sampleBufferMacro( x*0.3,y*0.3 ) - 170.0), 0 );
			landmassity -= std::max<Real>( (target->m_buf_general.sampleBufferMacro( x*0.27,y*0.27 ) - 180.0), 0 );
			return landmassity;
		}
	};
	struct lg_test_flat_t : public landmassity_baseline {
		Real work ( const Real* landmassity_sample, Real subX, Real subY, Real_d x, Real_d y, CWorldGen_Terran* target )
		{
			//return 24.0f / 4000;
			Real landmassity = Math.lerp( (subY/32.0),
				Math.lerp( (subX/32.0), landmassity_sample[0], landmassity_sample[1] ),
				Math.lerp( (subX/32.0), landmassity_sample[2], landmassity_sample[3] )
				);
			return landmassity;
		}
	};

	struct lg_combiner_t : public landmassity_baseline
	{
		int sampleCount;
		Real					weights [4];
		landmassity_baseline*	samplers [4];
		Real work ( const Real* landmassity_sample, Real subX, Real subY, Real_d x, Real_d y, CWorldGen_Terran* target ) override
		{
			Real landmassity = 0;
			for ( int i = 0; i < sampleCount; ++i )
			{
				landmassity_baseline* sampler = samplers[i];
				landmassity += sampler->work( landmassity_sample, subX,subY, x,y, target ) * weights[i];
			}
			return landmassity;
		}
	};


	
	// ==================================================================
	//
	// Density Samplers
	//
	// ==================================================================

	struct dg_Peturb_t : public density_baseline { // Perturb tall hills
		Real work ( const Real* landmassity_sample, Real subX, Real subY, Real_d x, Real_d y, Real_d z, CWorldGen_Terran* target ) override
		{
			// Sample the heightmap first
			Real landmassity = hm_gen->work( landmassity_sample, subX, subY, x,y, target );
			//return landmassity; // Below above zero means solid.
			// All blocks on the surface should be in the density range of 0-2

			Real density = landmassity - z; // When z is smaller than landmassity*4000, Z will be 0-2 for surface, >2 for underground, <0 above ground

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
	}; // Creates the real elevation.
	struct dg_CliffwisePeturb_t : public density_baseline { // Perturb to cliffs
		Real work ( const Real* landmassity_sample, Real subX, Real subY, Real_d x, Real_d y, Real_d z, CWorldGen_Terran* target ) override
		{
			// Sample the heightmap first
			Real landmassity = hm_gen->work( landmassity_sample, subX, subY, x,y, target );
			//return landmassity; // Below above zero means solid.
			// All blocks on the surface should be in the density range of 0-2

			Real density = landmassity - z; // When z is smaller than landmassity, Z will be 0-2 for surface, >2 for underground, <0 above ground

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
			density = std::min<ftype>( landmassity - z + 14, density );
			density = std::max<ftype>( landmassity - z - 30, density );

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
			density = std::min<ftype>( landmassity - z, density );

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
			density = std::max<ftype>( landmassity - z - 60, density );
			
			return density;
		}
	}; // Creates the real elevation.
	struct dg_Straight_t : public density_baseline { // Straight send
		Real work ( const Real* landmassity_sample, Real subX, Real subY, Real_d x, Real_d y, Real_d z, CWorldGen_Terran* target ) override
		{
			// Sample the heightmap first
			Real landmassity = hm_gen->work( landmassity_sample, subX, subY, x,y, target );
			Real_d density = landmassity - z; // When z is smaller than landmassity, Z will be 0-2 for surface, >2 for underground, <0 above ground
			return (Real)density;
		}
	} ; // Creates the real elevation.

	// System density combiner
	struct dg_combiner_t : public density_baseline
	{
		int sampleCount;
		Real					weights [8];
		density_baseline*		samplers [8];
		Real work ( const Real* landmassity_sample, Real subX, Real subY, Real_d x, Real_d y, Real_d z, CWorldGen_Terran* target ) override
		{
			Real density = 0;
			for ( int i = 0; i < sampleCount; ++i )
			{
				density_baseline* sampler = samplers[i];
				density += sampler->work( landmassity_sample, subX,subY, x,y,z, target ) * weights[i];
			}
			return density;
		}
		void condense ( int* indexer )
		{
			int					new_sampleCount = 0;
			density_baseline*	new_samplers [8];

			// Loop through the samplers and condense them
			for ( int i = 0; i < sampleCount; ++i )
			{
				density_baseline* source_sampler = samplers[i];
				// Start off with could not find a spot
				bool hasSpot = false;
				// Check the new samplers for this sample
				for ( int j = 0; j < new_sampleCount; ++j )
				{
					if ( new_samplers[j] == source_sampler )
					{
						indexer[i] = j;
						hasSpot = true;
						break;
					}
				}
				// If has not found a spot, then take up a spot on the samplers
				if ( !hasSpot )
				{
					new_samplers[new_sampleCount] = source_sampler;
					indexer[i] = new_sampleCount;
					new_sampleCount += 1;
				}
			}
			// Now apply new samplers
			sampleCount = new_sampleCount;
			for ( int i = 0; i < sampleCount; ++i )
			{
				samplers[i] = new_samplers[i];
			}
		}
	};
	
	// ==================================================================
	//
	// Blocktype Samplers
	//
	// ==================================================================

	struct bg_defaultBlocktypes_t : public block_baseline {
		void work ( Terrain::terra_b& block, Real z_difference, Real density, const _block_passinfo& bp, const quickAccessData& qd, CWorldGen_Terran* gen ) override
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
				block.block = EB_NONE;
			}
		}
	};
	struct bg_defaultDesertWasteland_t : public block_baseline {
		void work ( Terrain::terra_b& block, Real z_difference, Real density, const _block_passinfo& bp, const quickAccessData& qd, CWorldGen_Terran* gen ) override
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
				block.block = EB_NONE;
			}
		}
	};
	struct bg_defaultDesert_t : public block_baseline {
		void work ( Terrain::terra_b& block, Real z_difference, Real density, const _block_passinfo& bp, const quickAccessData& qd, CWorldGen_Terran* gen ) override
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
					/*else if ( randomValue > 314 && randomValue < 324 )
					{
						if ( density < 2 )
						{
							sTerraGrass newgrass;
							newgrass.block.x_index = bp.subX;
							newgrass.block.y_index = bp.subY;
							newgrass.block.z_index = bp.subZ;
							newgrass.type = EG_DRY;
							newgrass.position = Vector3d( bp.subX*bp.stepX+1+sinf(randomValue*4.127f)*0.8f, bp.subY*bp.stepY+1+sinf(randomValue*6.753f)*0.8f, bp.subZ*bp.stepZ+1.7f+density );

							qd.gamedata->m_grass.push_back( newgrass );
						}
					}*/
				}
			}
			else
			{
				block.block = EB_NONE;
			}
		}
	};
	struct bg_stoneTest_t : public block_baseline {
		void work ( Terrain::terra_b& block, Real z_difference, Real density, const _block_passinfo& bp, const quickAccessData& qd, CWorldGen_Terran* gen ) override
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
				block.block = EB_NONE;
			}
		}
	};

	struct bg_combiner_t : public block_baseline {
		int sampleCount;
		Real				weights [8];
		Real				offsets [8];
		block_baseline*		samplers [8];
		void work ( Terrain::terra_b& block, Real z_difference, Real density, const _block_passinfo& bp, const quickAccessData& qd, CWorldGen_Terran* gen ) override
		{
			int max_selection = 0;
			Real max_weight = -10000;
			for ( int i = 0; i < sampleCount; ++i )
			{
				Real next_weight = weights[i];
				next_weight += ( gen->m_buf_general_noise.sampleBufferMicro( bp.x + bp.z - bp.y*0.5 + offsets[i], bp.y - bp.x + bp.z*0.5 - offsets[i] ) - 128 )*0.2f;
				if ( next_weight > max_weight ) {
					max_weight = next_weight;
					max_selection = i;
				}
			}
			return samplers[max_selection]->work( block, z_difference, density, bp, qd, gen );
		}
		void condense ( int* indexer )
		{
			int					new_sampleCount = 0;
			block_baseline*		new_samplers [8];

			// Loop through the samplers and condense them
			for ( int i = 0; i < sampleCount; ++i )
			{
				block_baseline* source_sampler = samplers[i];
				// Start off with could not find a spot
				bool hasSpot = false;
				// Check the new samplers for this sample
				for ( int j = 0; j < new_sampleCount; ++j )
				{
					if ( new_samplers[j] == source_sampler )
					{
						indexer[i] = j;
						hasSpot = true;
						break;
					}
				}
				// If has not found a spot, then take up a spot on the samplers
				if ( !hasSpot )
				{
					new_samplers[new_sampleCount] = source_sampler;
					indexer[i] = new_sampleCount;
					new_sampleCount += 1;
				}
			}
			// Now apply new samplers
			sampleCount = new_sampleCount;
			for ( int i = 0; i < sampleCount; ++i )
			{
				samplers[i] = new_samplers[i];
			}
		}
	};

};


#endif//_C_WORLD_GEN_TERRAIN_LANDMASSES_WORLDTYPES_H_