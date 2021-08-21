#ifndef CORE_EXT_MATH_NOISE_MIDPOINT_NOISE_WRAPPER_H_
#define CORE_EXT_MATH_NOISE_MIDPOINT_NOISE_WRAPPER_H_

#include "core/math/noise/BaseNoise.h"
#include "MidpointNoiseBuffer.h"

class MidpointWrappedNoise : public BaseNoise
{
public:
	MidpointWrappedNoise ( int source_octaves, float source_freq, int seed );

	//	Get( pos2d ) : Sample the noise on 2D plane.
	virtual float			Get (const Vector2f& position) override;
	//	Get( pos3d ) : Sample the noise on 3D volume.
	virtual float			Get (const Vector3f& position) override;

private:
	midpoint_buffer_t<128> noise_2d;
	midpoint_buffer3_t<128> noise_3d;
};

#endif//CORE_EXT_MATH_NOISE_MIDPOINT_NOISE_WRAPPER_H_