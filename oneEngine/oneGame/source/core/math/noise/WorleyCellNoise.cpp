#include "WorleyCellNoise.h"
#include "core/math/Math.h"

#include <algorithm>
#include <random>

static Vector3f RandomCellPoint( int32 celX, int32 celY, int32 celZ, float offsets [1024] )
{
	return Vector3f(
		celX + offsets[(celX              + celY^celZ) & (1024 - 1)],
		celY + offsets[(celY * 912        + celX^celZ) & (1024 - 1)],
		celZ + offsets[(celZ * 1024 * 871 + celX^celY) & (1024 - 1)]
	);
}

//	Noise ( vec3, randombuf ) : Performs 3x3 sample for worley
static float WorleyCell(float vec[3], float offsets [1024])
{
	// Select proper unit cube with the point
	const int32 X = ((int32)std::floor(vec[0])) & 0xFF;
	const int32 Y = ((int32)std::floor(vec[1])) & 0xFF;
	const int32 Z = ((int32)std::floor(vec[2])) & 0xFF;

	float distance = 1.0e10F;

	// Sample the 3x3 cells to get the minimum distance to a cell
	for (int32 xo = -1; xo <= 1; xo++)
	{
		for (int32 yo = -1; yo <= 1; yo++)
		{
			for (int32 zo = -1; zo <= 1; zo++)
			{
				const int32 Xi = X + xo;
				const int32 Yi = Y + yo;
				const int32 Zi = Z + zo;
				
				const Vector3f celPosition = RandomCellPoint(Xi, Yi, Zi, offsets);

				distance = std::min<float>(distance, (celPosition - Vector3f(vec[0], vec[1], vec[2])).magnitude());
			}
		}
	}

	distance = math::saturate(distance);

	return distance;
}


float WorleyCellNoise::Get (const float position)
{
	float vec[3] = {position * m_cellcount, 0.0F, 0.0F};
	float result = WorleyCell(vec, m_offset);
	return (result - 0.5F) * 2.0F * m_amp;
}

float WorleyCellNoise::Get (const Vector2f& position)
{
	float vec[3] = {position.x * m_cellcount, position.y * m_cellcount, 0.0F};
	float result = WorleyCell(vec, m_offset);
	return (result - 0.5F) * 2.0F * m_amp;
}

float WorleyCellNoise::Get (const Vector3f& position)
{
	float vec[3] = {position.x * m_cellcount, position.y * m_cellcount, position.z * m_cellcount};
	float result = WorleyCell(vec, m_offset);
	return (result - 0.5F) * 2.0F * m_amp;
}

void WorleyCellNoise::init ( void )
{
	std::mt19937 random_engine (m_seed);
	std::uniform_real_distribution distro (0.0F, +1.0F);

	for (int i = 0; i < 1024; ++i)
	{
		m_offset[i] = distro(random_engine);
	}
}