
#ifndef _TERRAIN_PROP_DATA_IO_H_
#define _TERRAIN_PROP_DATA_IO_H_

#include "core/types/types.h"

namespace Terrain
{
	//=========================================//
	// Prop type
	enum propdata_type_t : uint16_t
	{
		PROPTYPE_INVALID = 0,
		PROPTYPE_FOLIAGE = 1,
		PROPTYPE_COMPONENT = 2,
		PROPTYPE_GRASS = 3
	};

	//=========================================//
	// 64 byte prop type
	typedef union {
		struct {
			propdata_type_t i_type;
			unsigned short i_index;
			char raw_data [60];
		};
		struct foliage_t {
			propdata_type_t type;
			unsigned short foliage_index;
			float x, y, z;
			char branch_data [48];
		} foliage;
		struct component_t {
			propdata_type_t type;
			unsigned short component_index;
			float x, y, z;
			float xrot, yrot, zrot;
			float xscal, yscal, zscal;
			uint64_t userdata;
			uint8_t	block_x;
			uint8_t	block_y;
			uint8_t	block_z;
			uint8_t	block_w;
			char padding [12];
		} component;
	} propdata_big_t;
	static_assert( sizeof(propdata_big_t)==64, "propdata_big_t is not 64 bytes" );

	//=========================================//
	// 32 byte prop type
	typedef union {
		struct {
			propdata_type_t i_type;
			char raw_data [30];
		};
		struct grass_t {
			// Instance type
			propdata_type_t type;
			// Grass type
			unsigned short grass_type;
			// offset from sector origin
			float offsetx, offsety, offsetz;
			char padding [16];
		} grass;
	} propdata_small_t;
	static_assert( sizeof(propdata_small_t)==32, "propdata_small_t is not 32 bytes" );

};

#endif//_TERRAIN_PROP_DATA_IO_H_