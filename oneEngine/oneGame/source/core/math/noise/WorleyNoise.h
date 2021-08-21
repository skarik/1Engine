#ifndef CORE_MATH_WORLEY_NOISE_H_
#define CORE_MATH_WORLEY_NOISE_H_

#include "BaseNoise.h"
#include <vector>

class WorleyNoise : public BaseNoise
{
public:
	WorleyNoise (int cellCount, float cellSize, float amp, int seed)
	{
		m_cellcount	= cellCount;
		m_cellsize = cellSize;
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
	int		m_cellcount;
	float	m_cellsize;
	float	m_amp;
	int		m_seed;

	void				init ( void );
	float				get ( const Vector3f& position );

	std::vector<Vector3f>
						m_points;
	std::vector<std::vector<int>>
						m_pointCache;

};

#endif//CORE_MATH_WORLEY_NOISE_H_