
#ifndef _C_PATTERN_CONTROLLER_H_
#define _C_PATTERN_CONTROLLER_H_

#include "engine/behavior/CGameBehavior.h"
#include "after/types/world/Towns.h"
#include "after/types/world/Regions.h"
#include "after/types/WorldVector.h"

//#include "boost/thread.hpp"
#include <mutex>

class CVoxelTerrain;

namespace Terrain
{
	struct quickAccessData;
	//struct regionproperties_t;

	class CPatternController : public CGameBehavior
	{
	public:
		CPatternController ( CVoxelTerrain* n_terra );
		~CPatternController( void );

		// Step Update( ) : Engine call
		// Must be safe to call during the execution of this class's other properties
		// Performs simulation of towns.
		void Update ( void ) override;

		// Generate( ) : Thread-safe call to create or load town information
		void Generate ( const RangeVector& n_index );

		// Excavate( ) : Thread-safe call to rasterize town information
		void Excavate ( const quickAccessData& qd );

	private:
		CVoxelTerrain*	m_terrain;
		//boost::mutex	m_gen_sync;
		std::mutex		m_gen_sync;

	private:
		// ====================================
		// Town Generation
		// ====================================
		struct townGenerationState_t
		{
			uint32_t	region;
			World::regionproperties_t*	region_properties;

			uint32_t	town_count;
			uint32_t	area_count;

			uint8_t		terrain;
			uint8_t		biome;
			Real		elevation;
		};

		// Gen_SpawnChance ( ) : Generates a spawn chance at a given position
		Real	Gen_SpawnChance ( townGenerationState_t& ts, const RangeVector&, const std::vector<RangeVector>& );

		// Gen_PatternType ( ) : Generates a pattern and its properties.
		void	Gen_PatternType ( townGenerationState_t& ts, const RangeVector& );

	private:
		// ====================================
		// Town Excavation
		// ====================================

		// Excavate_Building ( ) : Rasterizes the given building at the given position
		void	Excavate_Building ( const World::patternBuilding_t&, const RangeVector&, const World::patternQuickInfo_t&, const World::patternGameInfo_t&, const quickAccessData& );

	private:
		// ====================================
		// IO
		// ====================================

		bool	Pattern_ExistsAt ( const RangeVector& );

		void	IO_SavePattern ( const RangeVector&, const World::patternData_t&, const World::patternQuickInfo_t&, const World::patternGameInfo_t& );
		void	IO_LoadPattern ( const RangeVector&, World::patternData_t&, World::patternQuickInfo_t&, World::patternGameInfo_t& );
	};
};

#endif//_C_PATTERN_MANAGER_H_