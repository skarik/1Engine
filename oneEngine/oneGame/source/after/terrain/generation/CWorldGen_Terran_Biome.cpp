
#include "CWorldGen_Terran.h"

#include "core/math/noise/SimplexNoise.h"
#include "core/math/Math.h"

#include "after/entities/foliage/CTerraFoliageFactory.h"
#include "after/entities/props/CTerrainPropFactory.h"

#include "after/terrain/edit/SidebufferAccessor.h"
#include "after/terrain/edit/csg/SidebufferVolumeEditor.h"
#include "after/terrain/VoxelTerrain.h"

using namespace Terrain;


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

// ============================
//     == Sector (Biome) ==
// ============================

//#include "../../../COctreeRenderer.h"
#include "CWorldGen_Terran_Biome.h"

void CWorldGen_Terran::Generate_Biomes ( const quickAccessData& qd )
{
	// Create the samplers on the stack
	// Biome generators
	fg_default_t		fg_default;
	// Biome combiner
	fg_combiner_t fg_combiner;
	fg_combiner.samplers[0] = 0; fg_combiner.samplers[1] = 0; fg_combiner.samplers[2] = 0; fg_combiner.samplers[3] = 0;
	fg_combiner.samplers[4] = 0; fg_combiner.samplers[5] = 0; fg_combiner.samplers[6] = 0; fg_combiner.samplers[7] = 0;

	// Create all the temporary variables
	Terrain::terra_b block;
	Real_d x,y,z;
	int32_t subX, subY, subZ;
	Real stepX, stepY, stepZ;
	// Calculate step size
	stepX = (Real)((qd.pMax.x - qd.pMin.x)/32);
	stepY = (Real)((qd.pMax.y - qd.pMin.y)/32);
	stepZ = (Real)((qd.pMax.z - qd.pMin.z)/32);

	// Set up the combiner samplers for the block generator
	fg_combiner.sampleCount = 8;
	for ( uint sc = 0; sc < fg_combiner.sampleCount; ++sc )
	{
		switch ( qd.macro.terrain[sc]%4 )
		{
		/*case TER_DESERT:
			bg_combiner.samplers[sc] = &bg_defaultDesert;
			bg_combiner.offsets[sc]  = 1.4f;
			break;*/
		default:
			fg_combiner.samplers[sc] = &fg_default;
			fg_combiner.offsets[sc]  = 0.0f;
			break;
		}
	}
	bio_baseline*			biomeGenerator			= &fg_combiner;
	int fg_indexer [8];
	fg_combiner.condense( fg_indexer );

	// Create passinfo struct
	_block_passinfo_expensive block_passinfo;
	block_passinfo.stepX = stepX;
	block_passinfo.stepY = stepY;
	block_passinfo.stepZ = stepZ;

	// Density terrain generation
	{
		// Loop through sector buffer and set data
		for ( subX = 0; subX < 32; ++subX )
		{
			// Set X Values
			x = subX*stepX + qd.pMin.x + Terrain::BlockSize*0.5;
			block_passinfo.subX = subX;
			block_passinfo.x = x;
			const Real xweight = subX/32.0;

			for ( subY = 0; subY < 32; ++subY )
			{
				// Set Y Values
				y = subY*stepY + qd.pMin.y + Terrain::BlockSize*0.5;
				block_passinfo.subY = subY;
				block_passinfo.y = y;
				const Real yweight = subY/32.0;

				for ( subZ = 0; subZ < 32; ++subZ )
				{
					// Set Z Values
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
						fg_combiner.weights[i] = 0;
						fg_combiner.weights[i] = 0;
					}
					for ( int i = 0; i < 8; ++i ) {
						fg_combiner.weights[i] += cubeWeights[fg_indexer[i]];
						fg_combiner.weights[i] += cubeWeights[fg_indexer[i]];
					}

					// Get the block
					terra_b block;
					qd.accessor->GetData( block.raw, RangeVector(subX,subY,subZ) );
					// Generate the rest of the expensive QD
					if ( subZ < 31 ) {
						qd.accessor->GetData( block_passinfo.top.raw, RangeVector(subX,subY,subZ+1) );
					}

					// Generate terrain
					biomeGenerator->work( block, block_passinfo, qd, this );

					// Generate sidebuffer information
					qd.accessor->SetData( block.raw, RangeVector(subX,subY,subZ) );
				}
				// End Z
			}
			// End Y
		}
		// End X
	}
}