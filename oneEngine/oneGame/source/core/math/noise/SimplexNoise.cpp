#include "SimplexNoise.h"
#include "private/simplex_noise.h"	// C implmentation
#include "core/math/Math.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>

float SimplexNoise::Get(const float position)
{
	return octave_noise_2d( m_octaves, 0.5f, m_freq, position, m_seed )*m_amp;
}

float SimplexNoise::Get(const Vector2f& position)
{
	return octave_noise_3d( m_octaves, 0.5f, m_freq, position.x,position.y, m_seed )*m_amp;
}

float SimplexNoise::Get(const Vector3f& position)
{
	return octave_noise_4d( m_octaves, 0.5f, m_freq, position.x,position.y,position.z, m_seed )*m_amp;
}