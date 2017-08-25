// Towns.h
// Definition for town types and town storage structures.

#ifndef _WORLD_TOWNS_H_
#define _WORLD_TOWNS_H_

#include "core/types/types.h"
#include "core/containers/arstring.h"
#include "after/types/WorldVector.h"
#include "core/math/Vector3d.h"
#include "core/math/Ray.h"
#include "core/math/Cubic.h"

#include <vector>

namespace World
{
	// enum eTownType
	// Town type description enumeration.
	enum ePatternType : uint16_t
	{
		TOWN_TYPE_NORMAL = 0,
		TOWN_TYPE_CITY,
		TOWN_TYPE_BANDIT_CAMP,
		TOWN_TYPE_DUNGEON_LAYOUT, // Will this even be used?
		TOWN_TYPE_CASTLE,
		TOWN_TYPE_RUINS_OLD,
		TOWN_TYPE_RUINS_CITY,

		TOWN_TYPE_OUTDATED1 = 199,
		TOWN_TYPE_OUTDATED0 = 200,
	};

	// Town Quickinfo Entry
	//  save alignment in the file is 512 bytes
	struct patternQuickInfo_t
	{
		// The name of the town
		arstring<256>	name;

		// Centering position of the town
		RangeVector		position;

		// Region this town is originally part of
		uint32_t		initial_region;
		// Region this town is currently part of
		uint32_t		region;

		// Bounding box
		//  Defined in local coordinates
		Vector3d		bb_min;
		Vector3d		bb_max;

		// Type of the town
		// used for generation and swapping
		ePatternType		townType; 

		patternQuickInfo_t ( void ) : bb_min(0,0,0), bb_max(0,0,0) {}
	};
	static_assert ( sizeof(patternQuickInfo_t) <= 512, "Misaligned struct" );

	// Town Info Entry
	//  save aligned in file to 512 bytes
	struct patternGameInfo_t
	{
		// If specified, this is the town that this is a sprawl of
		arstring<256>	parent_town;

		// Region this town is part of
		uint32_t		region;

		// Government favor and simulation information
		ftype		govt_favor;		// favor the player has with the current government
		ftype		govt_opinion;	// region's opinion of current government
		uint8_t		govt_type;
		uint8_t		govt_opinion_type;	// wanted government type
	};
	static_assert ( sizeof(patternGameInfo_t) <= 512, "Misaligned struct" );

	// enum eBuildingType
	// Building type enumeration.
	enum class eBuildingType : uint8_t
	{
		BLD_Default = 0,
		BLD_Inn,
		BLD_Pub,
		BLD_Shop,

		BLD_Stoneblock
	};
	// enum eFloorplanValue
	// Pixel value for floortype.
	enum class eFloorplanValue : uint8_t
	{
		FLR_None = 0,
		FLR_WallExternal,
		FLR_WallInterior,
		FLR_DoorwayExternal,
		FLR_DoorwayInterior,
		FLR_InteriorFlooring,
		FLR_ExteriorFlooring,

		FLR_SubmajorConnector,
		FLR_HallwayConnector,
		FLR_DoorwayConnector,
		FLR_StairwellConnector,

		FLR_InteriorTable
	};

// ===============================================================================================
// TOWN PARTS
// The following definitions are for the literal representation of towns.
// 
// ===============================================================================================

	// Pattern Properties
	//  aligned to 64 bytes
	struct patternProperties_t
	{ 
		Real_32		culture [8];
		Real_32		density;
		Real_32		verticality;
		Real_32		circularity;
		Real_32		sublink_amount;
		Real_32		majorlink_amount;
		Real_32		deadend_acceptance;
		Real_32		pad0;
		Real_32		pad1;
	};
	static_assert ( sizeof(patternProperties_t) == 64, "Misaligned struct" );
	// Pattern Road
	//  aligned to 128 bytes
	struct patternRoad_t
	{
		arstring<64>	name;

		Ray				location;
		Real_32			size;
		Real_32			importance;

		Real_32	elevation;
		uint8_t	terra;
		uint8_t	biome;

		char	padding [26];
	};
	static_assert ( sizeof(patternRoad_t) == 128, "Misaligned struct" );
	// Pattern Landmark (location lister)
	//  aligned to 64 bytes
	struct patternLandmark_t
	{
		Vector3d		position;
		Real_32			radius;
		arstring<48>	name;
	};
	static_assert ( sizeof(patternLandmark_t) == 64, "Misaligned struct" );
	// Pattern Instance
	//  aligned to 512 bytes
	struct patternInstance_t
	{
		int16_t			type;
		int16_t			category; // 0 for instanced, 1 for component

		Vector3d		position;
		Vector3d		rotation;
		Vector3d		scale;

		char padding [472];
	};
	static_assert ( sizeof(patternInstance_t) == 512, "Misaligned struct" );
	// Pattern Building
	//  aligned to 256 bytes
	struct patternBuilding_t
	{ 
		Maths::Cubic	box;

		Real_32			elevation;
		Vector3d		entrance;
		Vector3d		entrance_dir;

		eBuildingType	type;
		uint8_t			padding0[3];

		struct planinfo_t
		{
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
		planinfo_t		floorplans[8];
		uint8_t			padding1[8];
	};
	static_assert ( sizeof(patternBuilding_t) == 256, "Misaligned struct" );

	// Pattern Data
	//  stores all the generation information of a town.
	//  possibly can take up a large amount of memory and should only be loaded up on demand.
	struct patternData_t
	{
		patternProperties_t				properties;
		std::vector<patternRoad_t>		roads;
		std::vector<patternLandmark_t>	landmarks;
		std::vector<patternInstance_t>	instances;
		std::vector<patternBuilding_t>	buildings;
	};
};

#endif//_WORLD_TOWNS_H_