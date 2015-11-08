
#include "CWorldGen_Terran.h"

#include "core/math/noise/SimplexNoise.h"
#include "core/math/Math.h"

#include "after/types/terrain/BlockType.h"

#include "after/entities/foliage/CTerraFoliageFactory.h"
#include "after/entities/props/CTerrainPropFactory.h"

#include "after/terrain/edit/SidebufferAccessor.h"
#include "after/terrain/edit/csg/SidebufferVolumeEditor.h"
#include "after/terrain/VoxelTerrain.h"

using namespace Terrain;

// ==============================
//     == Sector (Samples) ==
// ==============================

// Generate_MacroSample() : Performs the initial samples
void CWorldGen_Terran::Generate_MacroSample ( quickAccessData& qd )
{
	// Calculate the landmassisity at each corner. (This is the approximate elevation)
	qd.macro.landmassity[0] = GetLandmassity( Vector2d_d(qd.pMin.x,qd.pMin.y), false );
	qd.macro.landmassity[1] = GetLandmassity( Vector2d_d(qd.pMax.x,qd.pMin.y), false );
	qd.macro.landmassity[2] = GetLandmassity( Vector2d_d(qd.pMin.x,qd.pMax.y), false );
	qd.macro.landmassity[3] = GetLandmassity( Vector2d_d(qd.pMax.x,qd.pMax.y), false );

	// Sample the terrain at each corner
	qd.macro.terrain[0] = GetTerrainAt( Vector3d_d(qd.pMin.x,qd.pMin.y,qd.macro.landmassity[0]) );
	qd.macro.terrain[1] = GetTerrainAt( Vector3d_d(qd.pMax.x,qd.pMin.y,qd.macro.landmassity[1]) );
	qd.macro.terrain[2] = GetTerrainAt( Vector3d_d(qd.pMin.x,qd.pMax.y,qd.macro.landmassity[2]) );
	qd.macro.terrain[3] = GetTerrainAt( Vector3d_d(qd.pMax.x,qd.pMax.y,qd.macro.landmassity[3]) );

	// Sample the biome at each corner
	qd.macro.biome[0] = GetBiomeAt( Vector3d_d(qd.pMin.x,qd.pMin.y,qd.macro.landmassity[0]) );
	qd.macro.biome[1] = GetBiomeAt( Vector3d_d(qd.pMax.x,qd.pMin.y,qd.macro.landmassity[1]) );
	qd.macro.biome[2] = GetBiomeAt( Vector3d_d(qd.pMin.x,qd.pMax.y,qd.macro.landmassity[2]) );
	qd.macro.biome[3] = GetBiomeAt( Vector3d_d(qd.pMax.x,qd.pMax.y,qd.macro.landmassity[3]) );
}

// ==============================
//     == Sector (Terrain) ==
// ==============================

//#include "../../../COctreeRenderer.h"
#include "CWorldGen_Terran_Landmasses.h"

void CWorldGen_Terran::Generate_Terrain ( const quickAccessData& qd )
{
	// Create the samplers on the stack
	// Landmassity Generators
	lg_test_flat_t		lg_test_flat;
	lg_dunes_t			lg_dunes;
	lg_tallHills_t		lg_tallHills;
	// Density Generators
	dg_Straight_t		dg_Straight;
	// Block Generators (input is density)
	bg_defaultBlocktypes_t	bg_defaultBlocktypes;
	bg_defaultDesert_t		bg_defaultDesert;
	
	// Landmassity combiner
	lg_combiner_t lg_combiner;
	lg_combiner.samplers[0] = 0; lg_combiner.samplers[1] = 0; lg_combiner.samplers[2] = 0; lg_combiner.samplers[3] = 0; 
	// Density combiner
	dg_combiner_t dg_combiner;
	dg_combiner.samplers[0] = 0; dg_combiner.samplers[1] = 0; dg_combiner.samplers[2] = 0; dg_combiner.samplers[3] = 0;
	dg_combiner.samplers[4] = 0; dg_combiner.samplers[5] = 0; dg_combiner.samplers[6] = 0; dg_combiner.samplers[7] = 0;
	// Block generation combiner
	bg_combiner_t bg_combiner;
	bg_combiner.samplers[0] = 0; bg_combiner.samplers[1] = 0; bg_combiner.samplers[2] = 0; bg_combiner.samplers[3] = 0;
	bg_combiner.samplers[4] = 0; bg_combiner.samplers[5] = 0; bg_combiner.samplers[6] = 0; bg_combiner.samplers[7] = 0;

	// Create all the temporary variables
	Terrain::terra_b block;
	Real_d x,y,z;
	int32_t subX, subY, subZ;
	Real stepX, stepY, stepZ;
	// Calculate step size
	stepX = (Real)((qd.pMax.x - qd.pMin.x)/32);
	stepY = (Real)((qd.pMax.y - qd.pMin.y)/32);
	stepZ = (Real)((qd.pMax.z - qd.pMin.z)/32);

	// Set up combiner samplers for the elevation
	lg_combiner.sampleCount = 4;
	for ( uint sc = 0; sc < lg_combiner.sampleCount; ++sc )
	{
		switch ( qd.macro.terrain[sc] )
		{
		case TER_FLATLANDS:
			lg_combiner.samplers[sc] = &lg_test_flat;
			break;
		case TER_DESERT:
			lg_combiner.samplers[sc] = &lg_dunes;
			break;
		case TER_DEFAULT:
		default:
			lg_combiner.samplers[sc] = &lg_tallHills;
			break;
		}
	}
	landmassity_baseline*	landmassityGenerator	= &lg_combiner;
	//int lg_indexer [4];
	//lg_combiner.condense( lg_indexer );

	// Set up the combiner samplers for the density
	dg_combiner.sampleCount = 8;
	for ( uint sc = 0; sc < dg_combiner.sampleCount; ++sc )
	{
		switch ( qd.macro.terrain[sc]%4 )
		{
		default:
			dg_combiner.samplers[sc] = &dg_Straight;
			break;
		}
	}
	density_baseline*		densityGenerator		= &dg_combiner;
	densityGenerator->hm_gen = landmassityGenerator;
	for ( uint sc = 0; sc < dg_combiner.sampleCount; ++sc ) {
		dg_combiner.samplers[sc]->hm_gen = landmassityGenerator;
	}
	int dg_indexer [8];
	dg_combiner.condense( dg_indexer );
	
	// Set up the combiner samplers for the block generator
	bg_combiner.sampleCount = 8;
	for ( uint sc = 0; sc < bg_combiner.sampleCount; ++sc )
	{
		switch ( qd.macro.terrain[sc]%4 )
		{
		case TER_DESERT:
			bg_combiner.samplers[sc] = &bg_defaultDesert;
			bg_combiner.offsets[sc]  = 1.4f;
			break;
		default:
			bg_combiner.samplers[sc] = &bg_defaultBlocktypes;
			bg_combiner.offsets[sc]  = 0.0f;
			break;
		}
	}
	block_baseline*			blockGenerator			= &bg_combiner;
	int bg_indexer [8];
	bg_combiner.condense( bg_indexer );

	// Create block info
	_block_passinfo block_passinfo;
	block_passinfo.stepX = stepX;
	block_passinfo.stepY = stepY;
	block_passinfo.stepZ = stepZ;

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

				// Generate the weights for the elevation sampler
				const Real xweight = subX/32.0;
				const Real yweight = subY/32.0;
				const Real cornerWeights [4] = {
					(1-xweight) * (1-yweight),
					(xweight) * (1-yweight),
					(1-xweight) * (yweight),
					(xweight) * (yweight)
				};
				memcpy( lg_combiner.weights, cornerWeights, sizeof(Real)*4 );

				// Normalize weights
				//Real fweights = 0;
				//for ( uint sc = 0; sc < lg_combiner.sampleCount; ++sc ) {
				//	fweights += lg_combiner.weights[sc];
				//}
				//for ( uint sc = 0; sc < lg_combiner.sampleCount; ++sc ) {
				//	lg_combiner.weights[sc] /= fweights;
				//}

				// Sample the landmassity
				Real landmassity = landmassityGenerator->work( qd.macro.landmassity, subX,subY, x,y, this );

				// Loop through the blocks
				for ( subZ = 0; subZ < 32; ++subZ )
				{
					z = subZ*stepZ + qd.pMin.z + Terrain::BlockSize*0.5;
					block_passinfo.subZ = subZ;
					block_passinfo.z = z;
					// x,y,z are now set to proper values

					// Generate the weights for the density samplers
					const Real zweight = subZ/32.0;
					const Real cubeWeights [8] = {
						(1-xweight) * (1-yweight) * (1-zweight),
						(xweight)   * (1-yweight) * (1-zweight),
						(1-xweight) * (yweight)   * (1-zweight),
						(xweight)   * (yweight)   * (1-zweight),
						(1-xweight) * (1-yweight) * (zweight),
						(xweight)   * (1-yweight) * (zweight),
						(1-xweight) * (yweight)   * (zweight),
						(xweight)   * (yweight)   * (zweight)
					};
					for ( int i = 0; i < 8; ++i ) {
						dg_combiner.weights[i] = 0;
						bg_combiner.weights[i] = 0;
					}
					for ( int i = 0; i < 8; ++i ) {
						dg_combiner.weights[i] += cubeWeights[dg_indexer[i]];
						bg_combiner.weights[i] += cubeWeights[bg_indexer[i]];
					}
					
					// Calculate density at this location
					Vector3d density_p = Vector3d(
						densityGenerator->work( qd.macro.landmassity, subX+1,subY, x+Terrain::BlockSize,y,z, this ),
						densityGenerator->work( qd.macro.landmassity, subX,subY+1, x,y+Terrain::BlockSize,z, this ),
						densityGenerator->work( qd.macro.landmassity, subX,subY,   x,y,z+Terrain::BlockSize, this )
						);
					Vector3d density_n = Vector3d(
						densityGenerator->work( qd.macro.landmassity, subX-1,subY, x-Terrain::BlockSize,y,z, this ),
						densityGenerator->work( qd.macro.landmassity, subX,subY-1, x,y-Terrain::BlockSize,z, this ),
						densityGenerator->work( qd.macro.landmassity, subX,subY,   x,y,z-Terrain::BlockSize, this )
						);
					Real density = densityGenerator->work( qd.macro.landmassity, subX,subY, x,y,z, this );

					// Calculate the surface normal
					Vector3d dirNormal = density_n-density_p;
					dirNormal.normalize();

					// Reset block
					block.raw = 0;

					// Generate normal info
					Vector3d blockNormal = dirNormal;
					Vector3d blockNormalW ( 0,0,0 );
					blockNormalW.x = 1.0f - (density_p.x)/(density_p.x-density);
					blockNormalW.y = 1.0f - (density_p.y)/(density_p.y-density);
					blockNormalW.z = 1.0f - (density_p.z)/(density_p.z-density);

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

					// Save density
					uint16_t density_clamp = Math.Clamp( density/2 + 2048, 0, 4096 );
					block.light_r = 0x0F & (density_clamp >> 8);
					block.light_g = 0x0F & (density_clamp >> 4);
					block.light_b = 0x0F & (density_clamp);

					// Generate terrain
					blockGenerator->work( block, landmassity - z, density, block_passinfo, qd, this );
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

// ================================
//     == Temporary Lighting ==
// ================================
void CWorldGen_Terran::Generate_Lighting_Temporary ( const quickAccessData& qd )
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

	// Loop through sector buffer and set data
	for ( subX = 0; subX < 32; ++subX )
	{
		x = subX*stepX + qd.pMin.x + Terrain::BlockSize*0.5;
		for ( subY = 0; subY < 32; ++subY )
		{
			y = subY*stepY + qd.pMin.y + Terrain::BlockSize*0.5;
			// Calculate average elevation
			Real elevation = Interpolate2D( qd.macro.landmassity, subX/32.0f, subY/32.0f );
			// Loop through the blocks
			for ( subZ = 0; subZ < 32; ++subZ )
			{
				z = subZ*stepZ + qd.pMin.z + Terrain::BlockSize*0.5;

				// Get block
				qd.accessor->GetData( block.raw, RangeVector(subX,subY,subZ) );

				// Generate density
				ftype density = elevation - z;
				
				// Set light based on density
				//block.light_r = std::min<int>( 7, std::max<int>( 0, (int)(14-density*0.2f) ) );
				//block.light_r = std::min<int>( 15, std::max<int>( 0, (int)(-5+density*0.2f) ) );
				//block.light_r = std::min<int>( 15, std::max<int>( 0, (int)(-1+density*0.6f) ) );
				// Generate light values to use
				block.light_r = std::min<int>( 15, std::max<int>( 0, (int)(-2+density*0.4f) ) );
				block.light_g = block.light_r;
				block.light_b = block.light_r;

				// Save edited block
				qd.accessor->SetData( block.raw, RangeVector(subX,subY,subZ) );
			}
		}
	}
}
