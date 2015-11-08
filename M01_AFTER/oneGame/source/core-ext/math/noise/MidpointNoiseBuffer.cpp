
#include "MidpointNoiseBuffer.h"
#include "core/math/noise/SimplexNoise.h"

// Generate a terrain buffer into a 64x64 buffer
template <unsigned int WIDTH>
void midpoint_buffer_t<WIDTH>::CreateBuffer ( BaseNoise* nnoise, const Real scalx, const Real scaly )
{
	// set seed values
	setSample( 0,0, 128 );

	// perform the min-point formula
	int t_halfstep;
	int t_step = WIDTH;
	int t_x, t_y;
	while ( t_step > 1 )
	{
		// Grab halfstep, as that is what we will be iterating over
		t_halfstep = t_step/2;
		// Loop through the buffer
		for ( t_x = 0; t_x < WIDTH; t_x += t_step ) {
			for ( t_y = 0; t_y < WIDTH; t_y += t_step ) {
				ushort point_0_0, point_1_0, point_0_1, point_1_1;
				point_0_0 = getSample( t_x, t_y );
				point_1_0 = getSample( t_x+t_step, t_y );
				point_0_1 = getSample( t_x, t_y+t_step );
				point_1_1 = getSample( t_x+t_step, t_y+t_step );

				setSample( t_x + t_halfstep, t_y, (point_0_0+point_1_0)/2 );
				setSample( t_x, t_y + t_halfstep, (point_0_0+point_0_1)/2 );

				uchar midpoint = (uchar) (
					( (Real(point_0_0))+(Real(point_1_0))+(Real(point_0_1))+(Real(point_1_1)) )/4.0
					+ (nnoise->Get(t_x*scalx,t_y*scaly)*t_step*(2048.0f/WIDTH))
					);
				setSample( t_x + t_halfstep, t_y + t_halfstep, midpoint );
			}
		}

		// Split step
		t_step = t_halfstep;
	}
}

// Generate a terrain buffer into a 64x64x64 buffer
template <unsigned int WIDTH>
void midpoint_buffer3_t<WIDTH>::CreateBuffer ( BaseNoise* nnoise, const Real scalx, const Real scaly, const Real scalz )
{
	// set seed values
	setSample( 0,0,0, 128 );

	// perform the min-point formula
	int t_halfstep;
	int t_step = WIDTH;
	int t_x, t_y, t_z;
	while ( t_step > 1 )
	{
		// Grab halfstep, as that is what we will be iterating over
		t_halfstep = t_step/2;
		// Loop through the buffer
		for ( t_x = 0; t_x < WIDTH; t_x += t_step ) {
			for ( t_y = 0; t_y < WIDTH; t_y += t_step ) {
				for ( t_z = 0; t_z < WIDTH; t_z += t_step ) {
					ushort point_0_0_0, point_1_0_0, point_0_1_0, point_1_1_0;
					ushort point_0_0_1, point_1_0_1, point_0_1_1, point_1_1_1;
					point_0_0_0 = getSample( t_x, t_y, t_z );
					point_1_0_0 = getSample( t_x+t_step, t_y, t_z );
					point_0_1_0 = getSample( t_x, t_y+t_step, t_z );
					point_1_1_0 = getSample( t_x+t_step, t_y+t_step, t_z );
					point_0_0_1 = getSample( t_x, t_y, t_z+t_step );
					point_1_0_1 = getSample( t_x+t_step, t_y, t_z+t_step );
					point_0_1_1 = getSample( t_x, t_y+t_step, t_z+t_step );
					point_1_1_1 = getSample( t_x+t_step, t_y+t_step, t_z+t_step );

					setSample( t_x + t_halfstep, t_y, t_z, (point_0_0_0+point_1_0_0)/2 );
					setSample( t_x, t_y + t_halfstep, t_z, (point_0_0_0+point_0_1_0)/2 );
					setSample( t_x, t_y, t_z + t_halfstep, (point_0_0_0+point_0_0_1)/2 );

					uchar midpoint = (uchar) (
						( (Real(point_0_0_0))+(Real(point_1_0_0))+(Real(point_0_1_0))+(Real(point_1_1_0))+(Real(point_0_0_1))+(Real(point_1_0_1))+(Real(point_0_1_1))+(Real(point_1_1_1)) )/8.0
						+ (nnoise->Get3D(t_x*scalx,t_y*scaly,t_z*scalz)*t_step*(2048.0f/WIDTH))
						);
					setSample( t_x + t_halfstep, t_y + t_halfstep, t_z + t_halfstep, midpoint );
				}
			}
		}

		// Split step
		t_step = t_halfstep;
	}
}

// Prototypes
template struct midpoint_buffer_t<32>;
template struct midpoint_buffer_t<64>;
template struct midpoint_buffer_t<128>;
template struct midpoint_buffer3_t<32>;
template struct midpoint_buffer3_t<64>;
template struct midpoint_buffer3_t<128>;