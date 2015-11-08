//===============================================================================================//
//
//	class CRegionManager
// implementation of the town and region manager
// 
// Controls the towns given a terrain and a CTerraGenDefault.
// The town manager requires a valid CWorldState instance to exist in order to work properly.
//
// When writing functionality for this class, keep in mind that it will be run in a separate threads.
// That is THREADS, plural. This will have run of the main thread, simulation thread, and generation thread.
// In addition to those, it has its own thread that it uses to run tasks.
// Thus, IO functions must be checked, and interfaces with other parts of the game must be thread-safe.
//
//===============================================================================================//

#ifndef _C_PROVINCE_MANAGER_H_
#define _C_PROVINCE_MANAGER_H_

// Includes
#include "core/types/types.h"
#include "engine/behavior/CGameBehavior.h"
#include "after/types/WorldVector.h"
#include "after/types/world/Provinces.h"

class CVoxelTerrain;

namespace World
{

	class CProvinceManager : public CGameBehavior
	{
		ClassName( "CProvinceManager" );
		//typedef boost::thread	thread;
		//typedef boost::mutex	mutex;
	public:
		explicit			CProvinceManager ( CVoxelTerrain* n_terrain );//, CTerraGenDefault* pGenerator );
							~CProvinceManager ( void );

		void				Update ( void );
		void				Simulate ( void );
		void				Generate ( const RangeVector& );

		//typedef World::sRegionMapEntry		sRegionMapEntry;
		//typedef World::sProvinceAreaEntry	sProvinceAreaEntry;
		//typedef World::sProvinceInfo		sProvinceInfo;

		char	bufferGeneration [1024];

	public:
		//  GetRegion
		// Returns the region index that the given position belongs to.
		uint32_t			GetRegion ( const rangeint& x, const rangeint& y );

		//	GetProvinceArea
		// Populates a vector of sProvinceAreaEntry with all the areas in the province. Returns number of areas.
		uint32_t			GetProvinceArea ( uint32_t n_region, std::vector<province_entry_t>& out_province_area );
		//	GetProvinceHasTowns
		// Returns if province has had town generation.
		uint8_t				GetProvinceHasTowns ( uint32_t n_region );
		//	GetProvinceTownAreas - DOESN'T WORK
		// Populates a vector of sProvinceAreaEntry with all the town areas in the province.
		// Can be used in conjuction with the town manager.
		// May not create the correct results.
		// Returns number of areas.
		uint32_t			GetProvinceTownAreas ( uint32_t n_region, std::vector<province_entry_t>& out_province_area );
		//	GetProvinceInfo
		// Returns province info that's saved. Returns if true if valid data filled.
		bool				GetProvinceInfo ( uint32_t n_region, province_properties_t* out_province_info );
		//	GetProvinceName
		// Returns province name. Array should be 256 chars long. Returns if true if valid data filled.
		bool				GetProvinceName ( uint32_t n_region, char* out_province_name );

		//	SetProvinceHasTowns
		// Sets if province has had town generation.
		void				SetProvinceHasTowns ( uint32_t n_region, uint8_t n_towns_exist );
		//	SetProvinceInfo
		// Sets and saves province info.
		void				SetProvinceInfo ( uint32_t n_region, const province_properties_t* n_province_info );
		//	SetProvinceName
		// Sets and saves province name.
		void				SetProvinceName ( uint32_t n_region, const char* n_province_name );

	private:
		CVoxelTerrain*		terrain;
	/*private:
		CVoxelTerrain*		terrain;		// Pointer to associated terrain
		CTerraGenDefault*	generator;		// Pointer to associated mesh generation (used for regenerating towns)

		// Define struct for the threaded IO
		//struct		sDiskIOThread;
		//	friend	sDiskIOThread;
		// Boost Thread Objects
		mutex				mtLock;			// Lock for IO in the generation in the generation
		thread				mtThread;		// Thread for the generation in the generation....yo dawg

		// Region Map File
		CBinaryFile			regionFile;
		mutex				regionFileLock;
		uint32_t			regionCounter;
		uint32_t			regionIndex;

		// Generation state
		bool				m_generateProperties;

	private:
		// RegionsFloodfill ( RANGEVECTOR source position )
		// Generates a region given a position to start with.
		// Will call RegionsAdd on regions it finds that are not part of a region.
		// Map file must already be open.
		// Returns the ID of the region created, -2 if no region, or -1 if failed.
		uint32_t			RegionsFloodfill ( const RangeVector& sourcePosition );
		// RegionsAdd ( RANGEVECTOR position, INTEGER region_id )
		// Adds a position to the region map file. Map file must already be open.
		// Returns the region that the position actually got.
		uint32_t			RegionsAdd ( const RangeVector& position, uint32_t region_id );
		// RegionsSearch ( RANGEVECTOR position )
		// Searches for a position in the region map file. Map file must already be open.
		// The position's array is returned. If it cannot be found, uint32_t(-1) is returned.
		uint32_t			RegionsSearch ( const RangeVector& position );
		// RegionsSet ( RANGEVECTOR position, INTEGER region_id )
		// Sets a position in the region map file to the given region. Map file must already be open.
		// If the position isn't in the region map, calls RegionsAdd.
		// The region that the position actually got is returned.
		uint32_t			RegionsSet ( const RangeVector& position, uint32_t region_id );
		// RegionsSetFromRegion ( INTEGER old_region_id, INTEGER new_region_id );
		// Sets all regions with the given region ID to the new region. Map file must already be open.
		void				RegionsSetFromRegion ( uint32_t old_region_id, uint32_t new_region_id );
		// RegionsCreateProvince ( INTEGER region_id )
		// Searches for all regions with the given region ID and adds them to their province file.
		// Map file must already be open.
		// Also gives the provice basic starting properties for culture.
		void				RegionCreateProvince ( uint32_t region_id );

		// SectorSetHasTowns ( RANGEVECTOR position, BOOL new_value )
		// SectorSet
		// If the given sector is in a region, sets that it contains a town.
		//void				SectorSetHasTowns ( const RangeVector& position, bool has_towns );

		//friend CTownManager;

		// ProvinceOpenFile ( INTEGER region_id, BINARYFILE file, BOOL create_file )
		// Opens the region file to read or write province information.
		// On default, if the file does not exist, it will be created.
		void				ProvinceOpenFile ( uint32_t region_id, CBinaryFile& file, const bool create_file = true );
		// ProvinceAdd ( RANGEVECTOR position, BINARYFILE file )
		// Adds the given position to the province file. Province file must be open.
		void				ProvinceAdd ( const RangeVector& position, CBinaryFile& file );
		// ProvinceSearch ( RANGEVECTOR position, BINRARYFILE file )
		// Searches the given province for the given area. Province file must be open.
		// Returns true the the area is part of the province, false otherwise.
		bool				ProvinceSearch ( const RangeVector& position, CBinaryFile& file );
		// ProvinceSetName ( STRING name, BIRANYFILE file )
		// Sets the name of the province. Province file must be open.
		void				ProvinceSetName ( const char* name, CBinaryFile& file );
		// ProvinceSectorSetTowns ( RANGEVECTOR position, BINARYFILE file, BOOL has_town )
		// Searchs the given province for the given area. Upon finding it, sets if area has a town.
		// Returns true if province was found and the value was set to the new value.
		bool				ProvinceSectorSetTowns ( const RangeVector& position, CBinaryFile& file, bool n_has_towns=true );
		*/
	};

	// Threaded IO struct
	/*struct CRegionManager::sDiskIOThread
	{
		CRegionManager* manager;
		void	operator() ( void );
	};*/

	// Global pointer to active town manager

	//  ActiveRegionManager
	// Current terrain's region manager.
	extern CProvinceManager* ProvinceManager;
};

#endif//_C_PROVINCE_MANAGER_H_