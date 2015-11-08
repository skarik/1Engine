
#ifndef _C_TOWN_MANAGER_H_
#define _C_TOWN_MANAGER_H_

// Includes
#include "CGameBehavior.h"
#include "RangeVector.h"
#include "boost/thread.hpp"
#include "standard_types.h"
#include "arstring.h"
#include "CBinaryFile.h"
#include "Ray.h"
#include "Cubic.h"
#include "world/BlockTypes.h"
#include "world/Cultures.h"
#include "world/DirectionFlags.h"
#include "world/Towns.h"
// Include OSF format
#include "mccosf.h"

class CVoxelTerrain;
class CTerraGenDefault;
class CBoob;

class CTownManager : public CGameBehavior
{
	ClassName( "CTownManager" );
	typedef boost::thread	thread;
	typedef boost::mutex	mutex;
public:
	explicit			CTownManager ( CVoxelTerrain* pTerrain, CTerraGenDefault* pGenerator );
						~CTownManager ( void );

	void				Update ( void );
	void				Simulate ( void );
	void				Generate ( const RangeVector& );
	void				CreateFirstPass ( CBoob*, const RangeVector& );

	// Internally used types
	struct	sTownEntry;
	struct	sTownUsageEntry;
	struct	sTownInfo;
	struct	sPatternProperties;
	struct	sPatternRoad;
	struct	sPatternLandmark;
	struct	sPatternInstance;
	struct	sPatternBuilding;
	struct	sPattern;
	struct	sFloorplanNote;

public:
	//	GetTownInfo
	// Returns town info that's saved. Returns if true if valid data filled.
	/*bool				GetTownInfo ( arstring<256> town_name, uint32_t n_region, sTownInfo* out_town_info );
	bool				GetTownInfo ( CBinaryFile& town_file, sTownInfo* out_town_info );
	//	SetTownInfo
	// Sets and saves town info.
	void				SetTownInfo ( arstring<256> town_name, uint32_t n_region, const sTownInfo* n_town_info );
	void				SetTownInfo ( CBinaryFile& town_file, const sTownInfo* n_town_info );*/

	bool				GetTownAtSector( const RangeVector& inProvinceArea, sTownInfo* out_town_info );

private:
	CVoxelTerrain*		terrain;		// Pointer to associated terrain
	CTerraGenDefault*	generator;		// Pointer to associated mesh generation (used for regenerating towns)

	// Generation
	// Town Creation
	uchar				areaBiome;
	uchar				areaTerra;
	ftype				areaElevation;
	sPattern*			genTown;
	sTownEntry*			genTownEntry;

public:
	// Town Excavation
	unsigned char		floor_plan_buffer	[4096];
	uint32_t			floor_plan_width;
	uint32_t			floor_plan_height;

	Terrain::terra_t	component_plan_buffer [4096];
	Terrain::terra_t	component_plan_temp [4096];
	uint32_t			component_plan_x;
	uint32_t			component_plan_y;
	uint32_t			component_plan_z;

private:
	// Town map file
	vector<sTownUsageEntry>	townMap;
	CBinaryFile			townMapFile;
	uint32_t			townCounter;
	mutex				townMapFileLock;

	// Town General IO
	char				io_buffer	[1024];
	mutex				townGeneralIOLock;

public:
	// ImageToFloorPlan ( PNG filename )
	// Loads a PNG file and converts it to a 2D floor plan array.
	// The new array is returned. Returns NULL if an error occurs.
	// The result is also stored in the fixed size buffer floor_plan_buffer
	//  and its width and height stored in floor_plan_width and _height.
	char*				ImageToFloorPlan ( const char* fname );	

	// VxgToComponent ( MCCVXG filename )
	// Loads a mcc.VXG file and converts it to a linear component array.
	// The new array is returned. Returns NULL if an error occurs.
	// The result is also stored in the fixed size buffer component_plan_buffer
	//  and its width, height, and depth stored in component_plan_x,*y, and *z.
	Terrain::terra_t*	VxgToComponent ( const char* fname );

	// GetFloorplanDoorways
	void				GetFloorplanDoorways ( vector<sFloorplanNote> & infolist );

	// FloorplanRotateClockwise
	//void				FloorplanRotateClockwise ( void );
	// FloorplanRotateCounterClockwise
	// FloorplanFlipX
	void				FloorplanFlipX ( void );
	// FloorplanFlipY
	void				FloorplanFlipY ( void );
	// FloorplanRotate180
	void				FloorplanRotate180 ( void );

	// ComponentRotateClockwise
	void				ComponentRotateClockwise ( void );		
	// ComponentFlipX
	void				ComponentFlipX ( void );
	// ComponentFlipY
	void				ComponentFlipY ( void );
	// ComponentMakeFold
	// Makes a line tilable component into a corner piece
	void				ComponentMakeFold ( void );

private:
	//	LoadTowns ( RANGEVECTOR centerposition, INT width )
	// Loads nearby towns from disk
	void				LoadTowns ( const RangeVector& ncenterposition, int nwidth );

	// == Creation routines ==
	
	//  CreateRoads_1p
	// First pass of roads. Creates the initial terrain for roads.
	void				CreateRoads_1p ( const sTownEntry& townEntry, const sPattern& townPattern, CBoob* pBoob, const RangeVector& position );
	//  CreateBuildings_1p
	// First pass of buildings. Creates the initial terrain for buildings.
	void				CreateBuildings_1p ( const sTownEntry& townEntry, const sPattern& townPattern, CBoob* pBoob, const RangeVector& position );

	//	ExcavateFloorplan
	// Excavates floorplan of given building
	void				ExcavateFloorplan ( const sPattern& townPattern, const sPatternBuilding& buildingEntry, CBoob* pBoob, const RangeVector& position );
	//
	inline ftype		GetExcavationFloorplanResult ( const sPatternBuilding& buildingEntry, int seed );
	inline int			GetExcavationRandomInt ( const sPatternBuilding& buildingEntry, int seed );
	//  ExcavateInstance
	// Excavates given instance
	void				ExcavateInstance ( const sPattern& townPattern, const sPatternInstance& instanceEntry, CBoob* pBoob, const RangeVector& position );

	// = Functions to load components =
	
	mccOSF_entry_info_t cultureManifestLocations [8];

	//  PrepassCultureManifest
	// Loads the nodes in the culture manifest.
	void				PrepassCultureManifest ( void );
	//	VxgLoadComponent
	// Given component name and culture type, will load in a random or designated component.
	// Calls VxgToComponent to load the data. Returns false if a failure occurred.
	bool				LoadComponent ( const World::eCultureType target_culture, const char* component_name, int roll_index=-1 );
	//  GetCultureWallheight
	// Given target culture, will load in the target wall height.
	int					GetCultureWallHeight ( const World::eCultureType );
	int					GetCultureRoofOffset ( const World::eCultureType );

	// == IO routines ==

	// TownSaveEntry ( TOWN_ENTRY town )
	// Saves town to the town map file.
	void				TownSaveEntry ( sTownEntry* town_entry );
	// TownSearchEntry ( RANGEVECTOR position, TOWN_ENTRY found_data )
	// Searches for the town entry matching the position, and returns true if found.
	// Saves the found entry's data into "found_data"
	bool				TownSearchEntry ( const RangeVector& town_position, sTownEntry* town_entry=NULL );

	// TownOpenFile ( STRING townname, INTEGER initial_region_id, BINARYFILE file, BOOL create_file )
	// TownOpenFile ( RANGEVECTOR position, BINARYFILE file, BOOL create_file )
	// Opens the town file to read or write town information.
	// On default, if the file does not exist, it will be created.
	//void				TownOpenFile ( arstring<256> town_name, uint32_t initial_region_id, CBinaryFile& file, const bool create_file = true );
	void				TownOpenFile ( const RangeVector& town_position, CBinaryFile& file, const bool create_file = true );
	// TownOpenFileWrite ( STRING townname, INTEGER initial_region_id, BINARYFILE file, BOOL create_file )
	// TownOpenFileWrite ( RANGEVECTOR position, BINARYFILE file, BOOL create_file )
	// Opens the town file to write town information. Any existing information is deleted.
	// On default, if the file does not exist, it will be created.
	//void				TownOpenFileWrite ( arstring<256> town_name, uint32_t initial_region_id, CBinaryFile& file, const bool create_file = true );
	void				TownOpenFileWrite ( const RangeVector& town_position, CBinaryFile& file, const bool create_file = true );
	// TownFileExists ( RANGEVECTOR position )
	// Checks if the given town exists. Doesn't not create the file.
	bool				TownFileExists ( const RangeVector& town_position );
	// TownWriteInfo ( BINARYFILE file, TOWNINFO info_to_write )
	// Writes town info in struct to file
	void				TownWriteInfo ( CBinaryFile& file, const sTownInfo* town_info );
	// TownWritePattern ( BINARYFILE file, PATTERN pattern_to_write )
	// Writes town pattern to file
	void				TownWritePattern ( CBinaryFile& file, const sPattern* pattern );
	// TownReadInfo ( BINARYFILE file, TOWNINFO info_to_use )
	// Reads town info in struct from file
	void				TownReadInfo ( CBinaryFile& file, sTownInfo* town_info );
	// TownReadPattern ( BINARYFILE file, PATTERN pattern_to_use )
	// Reads town pattern from file
	void				TownReadPattern ( CBinaryFile& file, sPattern* pattern );
};

// Town Mapfile Entry, aligned in file to 512 bytes (actual is over 256)
struct	CTownManager::sTownEntry {
	RangeVector		position;
	//ftype			radius;
	//ftype			height;
	Vector3d		bb_min;
	Vector3d		bb_max;
	uint32_t		initial_region;
	uint32_t		region;
	arstring<256>	name;

	ushort			townType; //used for generation and swapping

	sTownEntry ( void ) : bb_min(0,0,0), bb_max(0,0,0) {}
};
// Town system Mapfile holder.
struct	CTownManager::sTownUsageEntry {
	sTownEntry		entry;
	uint8_t			unused_count;
};
// Town Info Entry, aligned in file to 512 bytes (actual is over 256)
struct	CTownManager::sTownInfo {
	uint32_t		region;
	arstring<256>	parent_town;

	ftype		govt_favor;		// favor the player has with the current government
	ftype		govt_opinion;	// region's opinion of current government
	uint8_t		govt_type;
	uint8_t		govt_opinion_type;	// wanted government type
};
// Pattern Properties
struct	CTownManager::sPatternProperties { // 64 Bytes
	ftype		culture [8];
	ftype		density;
	ftype		verticality;
	ftype		circularity;
	ftype		sublink_amount;
	ftype		majorlink_amount;
	ftype		deadend_acceptance;
	ftype		pad0;
	ftype		pad1;
};
// Pattern Road
struct	CTownManager::sPatternRoad { // 128 Bytes
	Ray				location;
	ftype			size;
	ftype			importance;
	arstring<64>	name;
	uchar	terra;
	uchar	biome;
	ftype	elevation;
	char padding [26];
};
// Pattern Location
struct	CTownManager::sPatternLandmark { // 64 Bytes
	Vector3d		position;
	ftype			radius;
	arstring<48>	name;
};
// Pattern Instance
struct	CTownManager::sPatternInstance { // 512 Bytes
	//char padding [512];
	Vector3d		position;
	Vector3d		rotation;
	Vector3d		scale;
	int16_t			type;
	int16_t			category; // 0 for instanced, 1 for component
	char padding [472];
};
// Pattern Building
struct	CTownManager::sPatternBuilding { // 256 Bytes
	Maths::Cubic	box;
	ftype			elevation;
	Vector3d		entrance;
	Vector3d		entrance_dir;
	uchar			type;
	uchar			genstyle;
	uchar			padding[2];
	// 200
	//char padding [202];
	struct floorplanvalue { //24 bytes
		char name[16];
		char angle;	// 1 flip X, 2 flip Y, 3 flip both
		char sizex;
		char sizey;
		char sizez;
		char posx; //local coords
		char posy;
		char posz;
		char pad;
	};
	floorplanvalue	floorplans[8];
	uchar			padding1[8];
};
// A Pattern.
struct	CTownManager::sPattern { // Is not 1-to-1 with file
	sPatternProperties			properties;
	vector<sPatternRoad>		roads;
	vector<sPatternLandmark>	landmarks;
	vector<sPatternInstance>	instances;
	vector<sPatternBuilding>	buildings;
};
// Floorplan info
struct	CTownManager::sFloorplanNote {
	int x, y;
	int w, h;
	EFaceDir direction;
	World::eFloorplanValue type;
};


// Global pointer to active town manager
namespace World
{
	extern CTownManager* ActiveTownManager;
};



#endif//_C_TOWN_MANAGER_H_