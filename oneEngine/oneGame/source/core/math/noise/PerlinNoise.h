#ifndef CORE_NOISE_PERLIN_H_
#define CORE_NOISE_PERLIN_H_

#include <algorithm>
#include <stdlib.h>
#include "BaseNoise.h"
#include "core/math/random/Random.h"

class PerlinNoise : public BaseNoise
{
public:
	PerlinNoise (int octaves, float freq, float amp, int seed);

	//	Get( pos1d ) : Sample the noise on 1D line.
	virtual float			Get (const float position) override;
	//	Get( pos2d ) : Sample the noise on 2D plane.
	virtual float			Get (const Vector2f& position) override;
	//	Get( pos3d ) : Sample the noise on 3D volume.
	virtual float			Get (const Vector3f& position) override;

private:
	//	init() : Sets up the noise init
	void					init ( void );
	//	noise(vec3) : Samples the noise
	float					noise ( float vec[3] );


	int					m_seed;
	int					m_octaves;
	float				m_frequency;
	float				m_amplitude;

	uint8				p[512];
};

#endif//CORE_NOISE_PERLIN_H_