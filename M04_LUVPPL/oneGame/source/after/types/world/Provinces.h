
#ifndef _WORLD_PROVINCES_H_
#define _WORLD_PROVINCES_H_

#include "core/types.h"
#include "core/containers/arstring.h"
#include "after/types/WorldVector.h"
#include "after/types/world/Resources.h"

namespace World
{
	//	struct province_entry_t
	// 16-byte aligned provice array entry.
	// Entry in list of province sectors.
	struct province_entry_t
	{
		int32_t		x;
		int32_t		y;
		// Region of the area for quick reference
		uint32_t	region;
		int32_t		w;
	};

	//	struct province_properties_t
	// 128-byte fitting province info entry
	// Profiles information about the province.
	struct province_properties_t
	{
		// Resources the province holds
		eResourceType	resources [8];
		// Culture weights of the area
		Real_32			culture [8];

		ftype		magi_opinion;	// 0 is neutral, -1 is dislike, 1 is like
		ftype		tech_opinion;	// 0 is neutral, -1 is dislike, 1 is like
		ftype		player_warmup_opinion;	// initial (weak) region thought of player
		ftype		player_overall_opinion;	// true region thought of player

		ftype		govt_favor;		// favor the player has with the current government
		ftype		govt_opinion;	// region's opinion of current government
		uint8_t		govt_type;
		uint8_t		govt_opinion_type;	// wanted government type
	};
};

#endif//_WORLD_PROVINCES_H_