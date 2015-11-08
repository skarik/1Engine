// SimplexNoise.h
// A wrapper for the simplex noise found at http://code.google.com/p/battlestar-tux/source/browse/procedural/simplexnoise.h
// Has the same interface as the Perlin class, making them pretty interchangable


#ifndef _SIMPLEX_NOISE_H_
#define _SIMPLEX_NOISE_H_


#include "BaseNoise.h"

class SimplexNoise : public BaseNoise
{
public:
	SimplexNoise(int octaves,float freq,float amp,float seed)
	{
		m_octaves	= (float)octaves;
		//m_freq		= freq*0.5f;//= freq*0.143f*5.0f;
		m_freq		= freq*0.143f*5.0f;
		m_amp		= amp;
		m_seed		= seed;
	}

	float Get(float x,float y);
	float Get3D(float x,float y,float z);
	float Unnormalize(float value);
private:
	float m_octaves;
	float m_freq;
	float m_amp;
	float m_seed;
};

#endif//_SIMPLEX_NOISE_H_