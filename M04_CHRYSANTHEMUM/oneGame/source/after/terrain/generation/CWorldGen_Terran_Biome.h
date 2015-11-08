
#ifndef _C_WORLD_GEN_TERRAIN_BIOME_WORLDTYPES_H_
#define _C_WORLD_GEN_TERRAIN_BIOME_WORLDTYPES_H_

#include "core/math/Math.h"

#include "CWorldGen_Terran_Landmasses.h"
#include "CWorldGen_Terran.h"

namespace Terrain
{
	struct _block_passinfo_expensive {
		Real_d	x, y, z;
		int32_t	subX, subY, subZ;
		Real	stepX, stepY, stepZ;
		Real_d	elevation;
		Terrain::terra_b	top;
	};
	struct bio_baseline {
		virtual void work ( Terrain::terra_b& block, const _block_passinfo_expensive& bp, const quickAccessData& qd, CWorldGen_Terran* gen ) =0; 
	};

	// ==================================================================
	//
	// Biome Samplers
	//
	// ==================================================================

	// Combiner
	struct fg_combiner_t : public bio_baseline
	{
		int sampleCount;
		Real				weights [8];
		Real				offsets [8];
		bio_baseline*		samplers [8];
		void work ( Terrain::terra_b& block, const _block_passinfo_expensive& bp, const quickAccessData& qd, CWorldGen_Terran* gen ) override
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
			return samplers[max_selection]->work( block, bp, qd, gen );
		}
		void condense ( int* indexer )
		{
			int					new_sampleCount = 0;
			bio_baseline*		new_samplers [8];

			// Loop through the samplers and condense them
			for ( int i = 0; i < sampleCount; ++i )
			{
				bio_baseline* source_sampler = samplers[i];
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

	struct fg_default_t : public bio_baseline
	{
		void work ( Terrain::terra_b& block, const _block_passinfo_expensive& bp, const quickAccessData& qd, CWorldGen_Terran* gen ) override
		{
			// If dirt, check to make type of grass
			if ( block.block == EB_DIRT )
			{
				if ( bp.top.block == EB_NONE )
				{
					block.block = EB_GRASS;
				}
			}
		}
	};
}


#endif//_C_WORLD_GEN_TERRAIN_BIOME_WORLDTYPES_H_