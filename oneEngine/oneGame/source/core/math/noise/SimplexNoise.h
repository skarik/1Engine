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
		m_freq		= freq*0.143f*5.0f;
		m_amp		= amp;
		m_seed		= seed;
	}

	//	Get( pos1d ) : Sample the noise on 1D line.
	virtual float			Get (const float position) override;
	//	Get( pos2d ) : Sample the noise on 2D plane.
	virtual float			Get (const Vector2f& position) override;
	//	Get( pos3d ) : Sample the noise on 3D volume.
	virtual float			Get (const Vector3f& position) override;

private:
	float m_octaves;
	float m_freq;
	float m_amp;
	float m_seed;
};

#endif//_SIMPLEX_NOISE_H_