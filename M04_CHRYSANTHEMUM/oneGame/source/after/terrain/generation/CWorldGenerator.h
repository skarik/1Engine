
#ifndef _C_WORLD_GENERATOR_H_
#define _C_WORLD_GENERATOR_H_

#include "core/math/Vector3d.h"
#include "core/math/Vector2d.h"
#include "core/math/vect3d_template.h"
#include "core/math/vect2d_template.h"
#include "core/math/noise/BaseNoise.h"
#include "after/types/WorldVector.h"
#include "after/types/terrain/BlockData.h"
#include "after/terrain/data/Node.h"

class CVoxelTerrain;

namespace Terrain
{
	class CPatternController;
	//class CRegionController;
	class CRegionGenerator;

	struct inputTerrain_t
	{
		CVoxelTerrain*		terrain;
		CPatternController*	patterns;
		CRegionGenerator*	regions;
	};

	class CWorldGenerator
	{
	public:
					CWorldGenerator ( void );
		virtual		~CWorldGenerator( void );

		virtual	void		Initialize ( void ) =0;
		virtual void		Cleanup ( void ) = 0;

	public:
		// World queries
		virtual bool		GetInland ( const Vector2d_d& worldposition ) =0;
		virtual Vector2d_d	GetWorldSize ( void ) =0;
		virtual RangeVector GetWorldSectorSize ( void ) =0;

		// Gameplay generation queries
		virtual Vector3d_d	GetSpawnPoint ( const int n_continent, const int n_seed_direction, const int n_index ) =0;

		virtual uint8_t		GetTerrainAt ( Vector3d_d& worldPosition ) =0;
		virtual uint8_t		GetBiomeAt ( Vector3d_d& worldPosition ) =0;
		virtual Real		GetLandmassity ( const Vector2d_d& worldposition, const bool firstPass ) =0;

		// Actual generation functions
		virtual void		GenerateSector  ( inputTerrain_t& n_terrain, Payload* n_buffer, GamePayload* n_gamedata, const Vector3d_d& nMin, const Vector3d_d& nMax, const RangeVector& nIndexer ) =0;
		virtual void		GenerateBlockAt ( inputTerrain_t& n_terrain, Payload* n_buffer, const Vector3d_d& worldPosition, terra_b& o_block ) =0;

	public:
		// to set, all code must go through this value
		void SetSeed ( const uint32_t newSeed ) {
			m_seed = newSeed;
		}
		uint32_t GetSeed ( void ) {
			return m_seed;
		}
	protected:
		// seed can not be accidentally directly set by internal classes
		const uint32_t& Seed ( void ) const {
			return m_seed;
		}
	private:
		uint32_t	m_seed;

	};
};

#endif//_C_WORLD_GENERATOR_H_