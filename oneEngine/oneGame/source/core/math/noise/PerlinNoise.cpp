#include <stdlib.h>
#include <stdio.h>
#include <cmath>

#include "core/math/Math.h"
#include "PerlinNoise.h"

//	Fade ( t ) : Super strong fade
template <typename Number> inline
static constexpr Number	Fade(Number t) noexcept
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

//	RandomGradient ( hash, x, y, z ) : Random gradient select given input
template <typename Number> inline
static constexpr Number	RandomGradient(uint8 hash, Number x, Number y, Number z) noexcept
{
	// Use low 4 bits for hash code
    const uint h = hash & 15;
	// Use hash code for 12 directions
    const Number u = (h < 8) ? x : y;
	const Number v = (h < 4) ? y : ((h == 12 || h == 14) ? x : z);
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

//	Noise ( vec3, randombuf ) : Performs "improved perlin noise"
template <typename Number> inline
static constexpr Number Noise(Number vec[3], uint8 p[512])
{
	// Select proper unit cube with the point
	const int32 X = ((int32)std::floor(vec[0])) & 0xFF;
	const int32 Y = ((int32)std::floor(vec[1])) & 0xFF;
	const int32 Z = ((int32)std::floor(vec[2])) & 0xFF;
	
	// Get the fraction of the coordinate in the cube
	const float x = math::fract(vec[0]);
	const float y = math::fract(vec[1]);
	const float z = math::fract(vec[2]);

	// Get the fade curves
	const float xfade = Fade(x);
	const float yfade = Fade(y);
	const float zfade = Fade(z);

	// Hash coordinates of the unit cube's corners
	const int32 A = p[X] + Y;
	const int32 AA = p[A] + Z;
	const int32 AB = p[A + 1] + Z;
	const int32 B = p[X + 1] + Y;
	const int32 BA = p[B] + Z;
	const int32 BB = p[B + 1] + Z;

	// Add blended results from 8 corners of the cube
	return math::lerp(zfade,
				math::lerp(yfade,
					math::lerp(xfade,	RandomGradient(p[AA  ], x  , y  , z   ),
										RandomGradient(p[BA  ], x-1, y  , z   )),
					math::lerp(xfade,	RandomGradient(p[AB  ], x  , y-1, z   ),
										RandomGradient(p[BB  ], x-1, y-1, z   ))),
				math::lerp(yfade, 
					math::lerp(xfade,	RandomGradient(p[AA+1], x  , y  , z-1 ),
										RandomGradient(p[BA+1], x-1, y  , z-1 )),
					math::lerp(xfade,	RandomGradient(p[AB+1], x  , y-1, z-1 ),
										RandomGradient(p[BB+1], x-1, y-1, z-1 ))));
}

//	Weight ( octaves ) : Return total amount of amplitude with the given octave number
static constexpr float Weight ( const int32 octaves ) noexcept
{
	float amp = 1;
	float value = 0;

	[[unroll]]
	for (int32 i = 0; i < octaves; ++i)
	{
		value += amp;
		amp /= 2;
	}

	return value;
}

void PerlinNoise::init(void)
{
	std::mt19937 random_engine (m_seed);

	// Fill up the sample array
	for (int i = 0; i < 256; ++i)
	{
		p[i] = (uint8)i;
	}

	// Shuffle the values
	for (int i = 255; i >= 0; --i)
	{
		int swap_index = random_engine() % 256;
		std::swap(p[i], p[swap_index]);
	}
	
	// Fill in the second half of the array
	for (int i = 0; i < 256; ++i)
	{
		p[i + 256] = (uint8)i;
	}
}

float PerlinNoise::noise ( float vec[3] )
{
	float result = 0.0F;
	float amplitude = m_amplitude;

	vec[0] *= m_frequency;
	vec[1] *= m_frequency;
	vec[2] *= m_frequency;

	for (int octave = 0; octave < m_octaves; ++octave)
	{
		result += Noise(vec, p) * amplitude;
		vec[0] *= 2.0F;
		vec[1] *= 2.0F;
		vec[2] *= 2.0F;
		amplitude /= 2.0F;
	}

	return result;
}

PerlinNoise::PerlinNoise (int octaves,float freq,float amp,int seed)
{
	m_octaves = octaves;
	m_frequency = freq;
	m_amplitude = amp;
	m_seed = seed;

	init();
}

float PerlinNoise::Get ( const float position )
{
	float vec[3] = {position, 0, 0};
	return noise(vec) / Weight(m_octaves);
}

float PerlinNoise::Get ( const Vector2f& position )
{
	float vec[3] = {position.x, position.y, 0};
	return noise(vec) / Weight(m_octaves);
}

float PerlinNoise::Get ( const Vector3f& position )
{
	float vec[3] = {position.x, position.y, position.z};
	return noise(vec) / Weight(m_octaves);
}