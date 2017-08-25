
#ifndef _C_WORLD_GENERATION____TYPE_TERRAIN_
#define _C_WORLD_GENERATION____TYPE_TERRAIN_

#include "CWorldGenerator.h"

#include <vector>
#include <atomic>
#include <unordered_map>

#include "core-ext/math/noise/MidpointNoiseBuffer.h"
#include "after/types/terrain/BiomeTypes.h"
#include "after/types/terrain/TerrainTypes.h"

namespace Terrain
{
	class SidebufferAccessor;
	class SidebufferVolumeEditor;

	struct macroSampleState_t
	{
		Real	landmassity [4];
		uint8_t	terrain [4];
		uint8_t	biome [4];
	};
	struct quickAccessData
	{
		CVoxelTerrain*		terrain;
		Payload*			buffer;
		GamePayload*		gamedata;
		Vector3d_d			pMin;
		Vector3d_d			pMax;
		SidebufferAccessor*		accessor;
		SidebufferVolumeEditor*	editor;
		RangeVector			indexer;
		macroSampleState_t	macro;
	};
	struct areaSticker_t
	{
		bool			special;
		ETerrainType	terrain;
		EBiomeType		biome;
		Vector2d_d		position;
		Vector2d_d		size;
	};

	class DungeonBase;

	class CWorldGen_Terran : public CWorldGenerator
	{
	public:
		explicit		CWorldGen_Terran ( const int continentCount );
						~CWorldGen_Terran( void );

		void		Initialize ( void ) override;
		void		Cleanup ( void ) override;

	public:
		// World queries
		bool		GetInland ( const Vector2d_d& worldposition ) override;
		Vector2d_d	GetWorldSize ( void ) override;
		RangeVector GetWorldSectorSize ( void ) override;

		// Gameplay generation queries
		Vector3d_d	GetSpawnPoint ( const int n_continent, const int n_seed_direction, const int n_index ) override;

		uint8_t		GetTerrainAt ( Vector3d_d& worldPosition ) override;
		uint8_t		GetBiomeAt ( Vector3d_d& worldPosition ) override;

		// Actual generation
		void		GenerateSector  ( inputTerrain_t& n_terrain, Payload* n_buffer, GamePayload* n_gamedata, const Vector3d_d& nMin, const Vector3d_d& nMax, const RangeVector& nIndexer ) override;
		void		GenerateBlockAt ( inputTerrain_t& n_terrain, Payload* n_buffer, const Vector3d_d& worldPosition, terra_b& o_block ) override;
		
	protected:
		// Init_CreateContinents() : Generates the continents.
		// Information is afterwards stored in m_continentPositions and m_continentSizes
		void		Init_CreateContinents( void );

		// Generate_MacroSample() : Performs the initial samples
		void		Generate_MacroSample ( quickAccessData& );
		// Generate_Terrain() : Generates the terrain.
		// Uses a linear interpolation to blend between landmassity samplers.
		// Uses a linear interpolation to blend between density generators.
		// Uses a splotchy blend to blend between block creation.
		void		Generate_Terrain ( const quickAccessData& );
		// Generate_Caves() : Generates cave systems.
		void		Generate_Caves ( const quickAccessData& );
		// Generate_Patterns() : Generates patterns.
		// Towns and castles are both systems of patterns.
		// If required, will also generate the regions for an area.
		void		Generate_Patterns ( const quickAccessData&, inputTerrain_t& n_terrain );
		// Generate_Biomes() : Does a quick 2-pass simulation of growth. Uses biome sampler to generate.
		// Uses a splotchy blend to blend between biomes.
		void		Generate_Biomes ( const quickAccessData& );
		// Generate_Architecture_Test() : Generate test architecture.
		void		Generate_Architecture_Test ( const quickAccessData& );

		// Generate_Lighting_Temporary() : Generates temporary lighting
		void		Generate_Lighting_Temporary ( const quickAccessData& );

		void TerraGen_Dungeons_Generate ( const RangeVector& );
		void TerraGen_Dungeons_Excavate ( const quickAccessData& );
	protected:
		BaseNoise*		noise;

		int			m_continentCount;

		Vector2d_d	m_worldSize;
		Real		m_worldDepth;

		std::vector<Vector2d_d>	m_continentPositions;
		std::vector<Vector2d_d>	m_continentSizes;

		std::vector<areaSticker_t>	m_areaStickers;

		void*		m_model;
	public:
		// GetLandmassity( ) : Returns parametric distance from the coast. Positive for land, negative for water.
		// firstPass will abort the checks on the land hit.
		Real		GetLandmassity ( const Vector2d_d& worldposition, const bool firstPass ) override;

	protected:
		// Interpolation
		template <typename Type>
		Type Interpolate2D ( const Type* four_sample, const Real x, const Real y ) {
			Type value = Math.lerp( y,
				Math.lerp( x, four_sample[0], four_sample[1] ),
				Math.lerp( x, four_sample[2], four_sample[3] )
				);
			return value;
		}

	protected:
		// Dungeon write flag
		std::atomic_flag	m_dungeonIO_flag;
		// Dungeon sample list (list of possible dungeons)
		std::vector<DungeonBase*>	dungeonSamplerList;
		// Hash table of dungeons to generate
		std::unordered_map<RangeVector,DungeonBase*> dungeonList;
		// Generated list of dungeons to currently work with
		std::vector<DungeonBase*>	dungeonLocalList;

	public:
		// Midpoint buffers used to making terrain (each buffer is 4KB)
		midpoint_buffer_t<64>	m_buf_general;
		midpoint_buffer_t<64>	m_buf_desert_macro;
		midpoint_buffer_t<64>	m_buf_desert_micro;
		midpoint_buffer_t<64>	m_buf_hills_macro;
		midpoint_buffer_t<64>	m_buf_hills_micro;
		midpoint_buffer_t<64>	m_buf_mountains_macro;
		midpoint_buffer_t<64>	m_buf_mountains_micro;
		midpoint_buffer_t<64>	m_buf_general_noise;		// Public to simplify the code and not waste effort on a fast access solution

		// Allow access to the samplers
		friend DungeonBase;
	};
};

#endif//_C_WORLD_GENERATION____TYPE_TERRAIN_