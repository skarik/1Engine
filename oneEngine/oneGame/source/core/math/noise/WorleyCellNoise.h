#ifndef CORE_MATH_WORLEY_CELL_NOISE_H_
#define CORE_MATH_WORLEY_CELL_NOISE_H_

#include "BaseNoise.h"

class WorleyCellNoise : public BaseNoise
{
public:
	WorleyCellNoise (float cellCount, float amp, int seed)
	{
		m_cellcount	= cellCount;
		m_amp		= amp;
		m_seed		= seed;

		init();
	}

	//	Get( pos1d ) : Sample the noise on 1D line.
	virtual float			Get (const float position) override;
	//	Get( pos2d ) : Sample the noise on 2D plane.
	virtual float			Get (const Vector2f& position) override;
	//	Get( pos3d ) : Sample the noise on 3D volume.
	virtual float			Get (const Vector3f& position) override;

private:
	float m_cellcount;
	float m_amp;
	int m_seed;

	void init ( void );
	float m_offset [1024];
};

#endif//CORE_MATH_WORLEY_CELL_NOISE_H_