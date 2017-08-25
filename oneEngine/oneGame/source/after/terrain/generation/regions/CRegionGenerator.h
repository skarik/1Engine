
#ifndef _C_REGION_CONTROLLER_H_
#define _C_REGION_CONTROLLER_H_

#include <thread>
#include <mutex>

#include "core/containers/arstring.h"
#include "engine/behavior/CGameBehavior.h"
#include "after/types/WorldVector.h"
#include "after/types/world/Resources.h"
#include "after/types/world/Cultures.h"
#include "after/types/world/Regions.h"

class CBinaryFile;
class CVoxelTerrain;

namespace Terrain
{
	struct quickAccessData;

	class CRegionGenerator : public CGameBehavior
	{
	public:
		CRegionGenerator ( CVoxelTerrain* n_terra );
		~CRegionGenerator( void );

		// Step Update( ) : Engine call
		// Must be safe to call during the execution of this class's other properties
		// Performs simulation of regions.
		void Update ( void ) override;

		// Generate( ) : Thread-safe call to create or load region information
		void Generate ( const RangeVector& n_index );

	private:
		CVoxelTerrain*	m_terrain;

		// ====================================
		// Region Generation
		// ====================================
	public:
		// Region_ClosestValidToIndex ( ) : Finds the closest valid region to the index.
		// Validitiy is based on the XY coordinates. The Z coordinate comes into play when there are overlapping regions.
		uint32_t Region_ClosestValidToIndex ( const RangeVector& );
	private:
		// Region_GenerateFloodfill ( ) : Generates a region using a floodfill algorithm.
		// Will swallow existing regions.
		uint32_t Region_GenerateFloodfill ( const RangeVector&, bool& o_isNewArea, std::vector<RangeVector>& o_sectorList );

		// Region_GenerateProperties ( ) : Generates properties for the given region.
		// Will overwrite old properties.
		void Region_GenerateProperties ( const uint32_t n_region, const RangeVector& n_origin, const std::vector<RangeVector>& n_sectors );

		// Region_ChangeRegionTo ( ) : Changes all positions with the given region into another region.
		void Region_ChangeRegionTo ( const uint32_t n_source_region, const uint32_t n_target_region );

		// Region_AppendSectors ( ) : Appends the list of sectors to the existing region
		void Region_AppendSectors ( const uint32_t n_region, const std::vector<RangeVector>& n_sectors );
	private:
		uint32_t		m_gen_nextregion;
		std::mutex		m_gen_floodfill_lock; // Needs to be locked so regions don't overlap

	private:
		// ====================================
		// I/O
		// ====================================
		struct io_intermediate_t
		{
			//uint32_t		fp;
			uint32_t		block;
			uint32_t		block_indexer;
			World::regioninfo_t	data;
		};
		struct io_nodeset_t
		{
			uint32_t			blocks	[13];
			World::regioninfo_t	data	[12];
			uint32_t			count;
		}; // 248 byte struct

		void	IO_Start ( void );
		void	IO_End ( void );
		void	IO_SaveOpen ( void );
		void	IO_Open ( void );
		void	IO_Save ( void );

		void	IO_UpdateNext ( void );

		void	IO_SetRegion ( const World::regioninfo_t& n_index_data );
		void	IO_FindIndex ( const RangeVector& n_index, io_intermediate_t& o_findResult );

		void	IO_ReadBlock ( const uint32_t n_blockindex, io_nodeset_t* o_data );
		void	IO_WriteBlock( const uint32_t n_blockindex, const io_nodeset_t* n_data );

	public:
		// IO_RegionGetSetHasTowns ( ) : Returns true if the region has not generated towns yet.
		// Marks the has_towns flag to true.
		bool	IO_RegionGetSetHasTowns ( const uint32_t n_region );

		void	IO_RegionSaveInfo ( const uint32_t n_region, const World::regionproperties_t* n_data );
		void	IO_RegionSaveSectors ( const uint32_t n_region, const std::vector<RangeVector>& n_sectors );

		void	IO_RegionLoadInfo ( const uint32_t n_region, World::regionproperties_t* n_data );
		void	IO_RegionLoadSectors ( const uint32_t n_region, std::vector<RangeVector>& n_sectors );
	private:
		uint32_t		m_io_next_freeblock;
		int				m_io_usecount;
		std::mutex		m_io_lock_file;
		CBinaryFile*	m_io;
	};
};

#endif//_C_REGION_CONTROLLER_H_