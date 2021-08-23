#ifndef CORE_MATH_WORLEY_NOISE_H_
#define CORE_MATH_WORLEY_NOISE_H_

#include "BaseNoise.h"
#include <vector>

// WorleyNoise based on randomly placed points in a 0-1 volume.
// Note that samples outside of the 0 to 1 range will not be valid unless `wrap` is enabled.
// `wrap` also greatly decreases performance

class WorleyNoise : public BaseNoise
{
public:
	WorleyNoise (int cellCount, float cellSize, float amp, int seed, bool wrap)
	{
		m_cellcount	= cellCount;
		m_cellsize = cellSize;
		m_amp		= amp;
		m_seed		= seed;
		m_wrap		= wrap;

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
	bool	m_wrap;

	void				init ( void );
	float				get ( const Vector3f& position );

	std::vector<Vector3f>
						m_points;
	std::vector<std::vector<int>>
						m_pointCache;

};

#endif//CORE_MATH_WORLEY_NOISE_H_