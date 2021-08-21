#include "MidpointWrappedNoise.h"
#include "core/math/noise/PerlinNoise.h"

MidpointWrappedNoise::MidpointWrappedNoise ( int source_octaves, float source_freq, int seed )
{
	PerlinNoise noise_source (source_octaves, source_freq, 1.0F, seed);
	noise_2d.CreateBuffer(&noise_source, 1.0F / 128.0F, 1.0F / 128.0F);
	noise_3d.CreateBuffer(&noise_source, 1.0F / 128.0F, 1.0F / 128.0F, 1.0F / 128.0F);
}

float MidpointWrappedNoise::Get (const Vector2f& position)
{
	float noiseValue = noise_2d.sampleBufferMicro(position.x * 64, position.y * 64) / 255.0F;
	noiseValue = noiseValue * 2.0F - 1.0F;
	return noiseValue;
}
float MidpointWrappedNoise::Get (const Vector3f& position) 
{
	float noiseValue = noise_3d.sampleBufferMicro(position.x * 64, position.y * 64, position.z * 64) / 255.0F;
	noiseValue = noiseValue * 2.0F - 1.0F;
	return noiseValue;
}