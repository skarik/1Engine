
#ifndef _C_BOOB_BITS_HPP_
#define _C_BOOB_BITS_HPP_

#include "core/types/types.h"
//#include <cmath>
#include <algorithm>

namespace Terrain
{
	typedef union {
		struct {
			unsigned	block	: 10;
			unsigned	wire	: 1;
			unsigned	power	: 1;
			unsigned	water	: 4;
			unsigned	snow	: 4;
			unsigned	light	: 3;
			unsigned	pipe	: 1;
			unsigned	pressure: 2;
			unsigned	magma	: 4;
			unsigned	energy	: 2;
        };
        uint32_t raw;
	} terra_t;
	static_assert( sizeof(terra_t)==sizeof(uint32_t), "Blocktype TERRA is not 32 bits" );

	typedef union {
		struct {
			// Block data
			unsigned	block	: 8;
			// Geometry data (normals are done via 3 face normals + 3 side blocks relative face normals)
			unsigned	normal_x_y: 3;
			unsigned	normal_x_z: 3;
			unsigned	normal_x_w: 3;
			unsigned	normal_y_x: 3;
			unsigned	normal_y_z: 3;
			unsigned	normal_y_w: 3;
			unsigned	normal_z_x: 3;
			unsigned	normal_z_y: 3;
			unsigned	normal_z_w: 3;
			unsigned	smooth_normal: 1;	// if normals should be averaged on calculation.
			// Lighting (lighting is done via the bordering air block)
			unsigned	light_r	: 4;
			unsigned	light_g	: 4;
			unsigned	light_b	: 4;
			// Fluids
			unsigned	fluid_type	: 2;	// 4 fluid types
			unsigned	fluid_level	: 4;	// 16 fluid levels
			// Pipes and electricity
			unsigned	wire		: 1;	// does this block have a wire?
			unsigned	voltage		: 2;	// 0: no power, 1: 0.3, 2: 0.6, 3: full power
			unsigned	pipe		: 1;	// does this block have a pipe?
			unsigned	pressure	: 2;	// 0: no pressure, 3: full pressure
			// Temperature
			unsigned	temperature	: 2;	// 0 freezing, 1 cool, 2 warm, 3 melting
			// Farming flag
			unsigned	nutrients	: 2;	// bit 0: has water, bit 1: ???
		};
		uint64_t raw;
	} terra_b;
	static_assert( sizeof(terra_b)==sizeof(uint64_t), "Blocktype SMOOTH is not 64 bits" );

	typedef union {
		struct {
			unsigned	loaded	: 1;
			unsigned	solid	: 1;
			unsigned	r		: 8;
			unsigned	g		: 8;
			unsigned	b		: 8;
			unsigned	pad		: 6;
		};
		uint32_t raw;
	} terra_t_lod;
	static_assert( sizeof(terra_t_lod)==sizeof(uint32_t), "Blocktype LOD is not 32 bits" );
	
	inline unsigned char GetSource ( terra_t data )
	{
		return (( data.wire == 0 )&&( data.power != 0 ));
	}

	inline uchar BlockHasValidNormals ( const terra_b& blk )
	{
#ifdef _FIXED_ENDIAN_
		return ((blk.raw & 0xFFFFFFF00)==0);
#else
		return !( blk.normal_x_y == 0 && blk.normal_x_z == 0
			&& blk.normal_y_x == 0 && blk.normal_y_z == 0
			&& blk.normal_z_x == 0 && blk.normal_z_y == 0 );
#endif
	}
	inline void BlockCopyNormals ( terra_b& dest, const terra_b& src )
	{
#ifdef _FIXED_ENDIAN_
		dest.raw &= ~0xFFFFFFF00;
		dest.raw |= src.raw & 0xFFFFFFF00;
#else
		dest.normal_x_y = src.normal_x_y;
		dest.normal_x_z = src.normal_x_z;
		dest.normal_x_w = src.normal_x_w;
		dest.normal_y_x = src.normal_y_x;
		dest.normal_y_z = src.normal_y_z;
		dest.normal_y_w = src.normal_y_w;
		dest.normal_z_x = src.normal_z_x;
		dest.normal_z_y = src.normal_z_y;
		dest.normal_y_w = src.normal_y_w;
		dest.smooth_normal = src.smooth_normal;
#endif
	}
	inline void BlockSetInvalidNormals ( terra_b& blk )
	{
#ifdef _FIXED_ENDIAN_
		blk.raw &= ~0xFFFFFFF00;
#else
		blk.normal_x_y = 0;
		blk.normal_x_z = 0;
		blk.normal_x_w = 0;
		blk.normal_y_x = 0;
		blk.normal_y_z = 0;
		blk.normal_y_w = 0;
		blk.normal_z_x = 0;
		blk.normal_z_y = 0;
		blk.normal_y_w = 0;
		blk.smooth_normal = 0;
#endif
	}

	inline Real _normal_bias ( const uchar value )
	{
		Real result = std::min<Real>( std::max<Real>( (value-4)/3.0f, -1 ), +1 );
		return (result>0)?(powf(result,2.0f)):(-powf(fabs(result),2.0f));
	}
	inline char _normal_unbias ( const Real value )
	{
		//return std::min<char>( std::max<char>( (char)((value*3)+4 +0.5), 1 ), 7 );
		Real result = (value>0)?(powf(value,1/2.0f)):(-powf(fabs(value),1/2.0f));
		return std::min<char>( std::max<char>( (char)((result*3)+4 +0.5), 1 ), 7 );
	}
	inline Real _depth_bias ( const uchar value )
	{
		return std::min<Real>( std::max<Real>( (value-1)/6.0f, 0 ), +1 );
	}
	inline char _depth_unbias ( const Real value )
	{
		return std::min<char>( std::max<char>( (char)((value*6)+1 +0.5), 1 ), 7 );
	}
};

#endif