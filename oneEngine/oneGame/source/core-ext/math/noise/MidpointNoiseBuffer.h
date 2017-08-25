//===============================================================================================//
//	MidpointNoiseBuffer.h
//
// Contains definitions for
//	midpoint_buffer_t : 2D noise buffer
//	midpoint_buffer3_t : 3D noise buffer
//===============================================================================================//

#ifndef _LOOPED_TERRAIN_BUFFER_H_
#define _LOOPED_TERRAIN_BUFFER_H_

#include "core/types/types.h"
#include "core/math/Math.h"

class BaseNoise;

//===============================================================================================//
//	struct midpoint_buffer_t
//
// Holds a stack-allocated block of random data.
// WIDTH template parameter MUST be a power of 2.
//===============================================================================================//
template <unsigned int WIDTH>
struct midpoint_buffer_t
{
public:
	uchar data [WIDTH*WIDTH];
		
public:
	// Safe wrapping
	inline uchar getSample ( const int x, const int y )
	{
		return data[(x&(WIDTH-1))+((y&(WIDTH-1))*WIDTH)];
	}
	inline void setSample ( const int x, const int y, const uchar value )
	{
		data[(x&(WIDTH-1))+((y&(WIDTH-1))*WIDTH)] = value;
	}

	// Sampling
	inline uchar sampleBufferMicro ( const Real_d x, const Real_d y )
	{
		return getSample( (int)x, (int)y );
	}
	inline Real sampleBufferMacro ( const Real_d x, const Real_d y )
	{
		int sample_x = (int)floor(x);
		int sample_y = (int)floor(y);
		uchar p_0_0 = getSample( sample_x, sample_y );
		uchar p_1_0 = getSample( sample_x+1, sample_y );
		uchar p_0_1 = getSample( sample_x, sample_y+1 );
		uchar p_1_1 = getSample( sample_x+1, sample_y+1 );

		return	math::lerp( Real(x-sample_x),
					math::lerp( Real(y-sample_y), p_0_0, p_0_1 ),
					math::lerp( Real(y-sample_y), p_1_0, p_1_1 ) );
	}
		
	// Generate a terrain buffer into a 64x64 buffer
	void CreateBuffer ( BaseNoise* nnoise, const Real scalx, const Real scaly );
};


//===============================================================================================//
//	struct midpoint_buffer3_t
//
// Holds a stack-allocated 3D block of random data.
// WIDTH template parameter MUST be a power of 2.
//===============================================================================================//
template <unsigned int WIDTH>
struct midpoint_buffer3_t
{
public:
	uchar data [WIDTH*WIDTH*WIDTH];
		
public:
	// Safe wrapping
	inline uchar getSample ( const int x, const int y, const int z )
	{
		return data[(x&(WIDTH-1))+((y&(WIDTH-1))*WIDTH)+((z&(WIDTH-1))*WIDTH*WIDTH)];
	}
	inline void setSample ( const int x, const int y, const int z, const uchar value )
	{
		data[(x&(WIDTH-1))+((y&(WIDTH-1))*WIDTH)+((z&(WIDTH-1))*WIDTH*WIDTH)] = value;
	}

	// Sampling
	inline uchar sampleBufferMicro ( const Real_d x, const Real_d y, const Real_d z )
	{
		return getSample( (int)x, (int)y, (int)z );
	}
	inline Real sampleBufferMacro ( const Real_d x, const Real_d y, const Real_d z )
	{
		int sample_x = (int)floor(x);
		int sample_y = (int)floor(y);
		int sample_z = (int)floor(z);
		uchar p_0_0_a = getSample( sample_x, sample_y, sample_z );
		uchar p_1_0_a = getSample( sample_x+1, sample_y, sample_z );
		uchar p_0_1_a = getSample( sample_x, sample_y+1, sample_z );
		uchar p_1_1_a = getSample( sample_x+1, sample_y+1, sample_z );

		uchar p_0_0_b = getSample( sample_x, sample_y, sample_z+1 );
		uchar p_1_0_b = getSample( sample_x+1, sample_y, sample_z+1 );
		uchar p_0_1_b = getSample( sample_x, sample_y+1, sample_z+1 );
		uchar p_1_1_b = getSample( sample_x+1, sample_y+1, sample_z+1 );

		return	math::lerp( Real(z-sample_z),
					math::lerp( Real(x-sample_x),
						math::lerp( Real(y-sample_y), p_0_0_a, p_0_1_a ),
						math::lerp( Real(y-sample_y), p_1_0_a, p_1_1_a ) ),
					math::lerp( Real(x-sample_x),
						math::lerp( Real(y-sample_y), p_0_0_b, p_0_1_b ),
						math::lerp( Real(y-sample_y), p_1_0_b, p_1_1_b ) ) );
	}
		
	// Generate a noise buffer into a 64x64x64 buffer
	void CreateBuffer ( BaseNoise* nnoise, const Real scalx, const Real scaly, const Real scalz );
};


#endif//_LOOPED_TERRAIN_BUFFER_H_