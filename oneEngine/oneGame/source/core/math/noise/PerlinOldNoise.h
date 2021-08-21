// http://www.flipcode.com/archives/Perlin_Noise_Class.shtml
// http://mrl.nyu.edu/~perlin/doc/oscar.html#noise
// http://freespace.virgin.net/hugo.elias/models/m_perlin.htm

#ifndef PERLIN_H_
#define PERLIN_H_

#include <algorithm>
#include <stdlib.h>
#include "BaseNoise.h"
#include "core/math/random/Random.h"

#define SAMPLE_SIZE 0x100 // 256 values

class PerlinOldNoise : public BaseNoise
{
public:
	PerlinOldNoise (int octaves, float freq, float amp, int seed);

	//	Get( pos2d ) : Sample the noise on 2D plane.
	virtual float			Get (const Vector2f& position) override;
	//	Get( pos3d ) : Sample the noise on 3D volume.
	virtual float			Get (const Vector3f& position) override;

private:
	void init_perlin(int n,float p);
	float perlin_noise_2D(float vec[2]);
	float perlin_noise_3D(float vec[3]);

	float noise1(float arg);
	float noise2(float vec[2]);
	float noise3(float vec[3]);
	void normalize2(float v[2]);
	void normalize3(float v[3]);
	void init(void);

	int   mOctaves;
	float mFrequency;
	float mAmplitude;
	int   mSeed;

	int p[SAMPLE_SIZE + SAMPLE_SIZE + 2];
	float g3[SAMPLE_SIZE + SAMPLE_SIZE + 2][3];
	float g2[SAMPLE_SIZE + SAMPLE_SIZE + 2][2];
	float g1[SAMPLE_SIZE + SAMPLE_SIZE + 2];

private:
	//typedef boost::random::mt19937 base_generator_type;
	//typedef boost::random::rand48 base_generator_type;
	typedef std::ranlux48 base_generator_type;
	base_generator_type mRand;

};

#undef SAMPLE_SIZE

#endif