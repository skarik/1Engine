#include "WorleyNoise.h"
#include "core/math/Math.h"

#include <algorithm>
#include <random>

float WorleyNoise::Get (const float position)
{
	float result = get(Vector3f(position, 0.5F, 0.5F));
	return (result - 0.5F) * 2.0F * m_amp;
}

float WorleyNoise::Get (const Vector2f& position)
{
	float result = get(Vector3f(position, 0.5F));
	return (result - 0.5F) * 2.0F * m_amp;
}

float WorleyNoise::Get (const Vector3f& position)
{
	float result = get(Vector3f(position));
	return (result - 0.5F) * 2.0F * m_amp;
}

void WorleyNoise::init ( void )
{
	std::mt19937 random_engine (m_seed);
	std::uniform_real_distribution distro (0.0F, 1.0F);

	// Generate points
	for (int i = 0; i < m_cellcount; ++i)
	{
		m_points.push_back({distro(random_engine), distro(random_engine), distro(random_engine)});
	}

	// Generate the cell divs
	const int celDim = math::floor(1.0F / m_cellsize) + 1;
	m_pointCache.resize(celDim * celDim * celDim);
	
	// Precache cels to look up
	for (int ix = 0; ix < celDim; ++ix)
	{
		for (int iy = 0; iy < celDim; ++iy)
		{
			for (int iz = 0; iz < celDim; ++iz)
			{
				std::vector<int>& cacheList = m_pointCache[ix + iy * celDim + iz * celDim * celDim];
				const Vector3f celCenter ((ix + 0.5F) * m_cellsize, (iy + 0.5F) * m_cellsize, (iz + 0.5F) * m_cellsize);

				// Loop through all the cells and see for anything that's in range.
				for (int i = 0; i < m_cellcount; ++i)
				{
					if ((celCenter - m_points[i]).sqrMagnitude() < (m_cellsize * m_cellsize * 3.0F))
					{
						cacheList.push_back(i);
					}
				}
				//
			}
		}
	}
}

float WorleyNoise::get ( const Vector3f& position )
{
	// Get the cel
	const int celDim = math::floor(1.0F / m_cellsize) + 1;
	const int32 celX = (int32)(position.x / m_cellsize);
	const int32 celY = (int32)(position.y / m_cellsize);
	const int32 celZ = (int32)(position.z / m_cellsize);
	const int32 celIndex = celX + celY * celDim + celZ * celDim * celDim;

	float distance = 1.0e10F;

	const std::vector<int>& cacheList = m_pointCache[celIndex];
	for (int i = 0; i < cacheList.size(); ++i)
	{
		distance = std::min<float>(distance, (position - m_points[cacheList[i]]).magnitude() / m_cellsize);
	}

	return math::saturate(distance);
}
