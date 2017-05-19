
#include "SimplexNoise.h"
#include "simplex_noise.h"	// C implmentation
#include "core/math/Math.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>

float SimplexNoise::Get(float x,float y)
{
	return octave_noise_3d( m_octaves, 0.5f, m_freq, x,y, m_seed )*m_amp;
}
float SimplexNoise::Get3D(float x,float y,float z)
{
	return octave_noise_4d( m_octaves, 0.5f, m_freq, x,y,z, m_seed )*m_amp;
}

float SimplexNoise::Unnormalize(float value)
{
	float result = value/2;
	result += ((1-exp(-fabs(result/0.707f))) + ( math::square<float>( (m_amp/2)-fabs(result) )*fabs(result) ) ) * math::sgn<float>( result );
	result = std::max<float>( -0.5f, std::min<float>( 0.5f, result ) );
	return result*2;
}