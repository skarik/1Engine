//===============================================================================================//
//	after/types/Regions.h
// 
// Holds definitions for types that define region properties.
// Regions are collections of sectors, created by the terrain generator.
// Think of them less as countries and more of chunks of land that make up a country.
//===============================================================================================//

#ifndef _WORLD_REGIONS_H_
#define _WORLD_REGIONS_H_

#include "core/types.h"
#include "core/containers/arstring.h"
#include "after/types/world/Resources.h"
#include "after/types/WorldVector.h"

namespace World
{
	//	struct regioninfo_t
	// 16-byte structure representing region info for an area.
	struct regioninfo_t
	{
		// Position of the block
		RangeVector	position;
		// Index of region that the block belongs to, or a constant from eRegionConstants
		uint32_t	region;	
	};

	//	struct regionproperties_t
	// 212/256-byte structure that holds the data for an area.
	struct regionproperties_t
	{
		// Name of the region
		arstring<128>	name;

		// Resources the province holds
		eResourceType	resources [8];
		// Culture weights of the area
		Real_32			culture [8];

		// Province that this region belongs to
		uint32_t		owner_province;
		// Allegiance to the said province
		Real_32			owner_allegiance;

		// Weighted opinion towards magic
		Real_32		magi_opinion;
		// Weighted opinion towards tech
		Real_32		tech_opinion;

		// 32-bit flags for region information
		typedef union {
			struct {
				unsigned	has_towns : 1;
			};
			uint32_t raw;
		} regionflags_t;
		regionflags_t	flags;
	};

	//	enum eRegionConstants
	// Reserved region IDs used in generation and storage.
	enum eRegionConstants : uint32_t
	{
		REGION_NONE		= uint32_t(-1),
		REGION_UNUSED	= uint32_t(-2),
		REGION_FAILURE	= uint32_t(-3)
	};
};

#endif//_WORLD_REGIONS_H_