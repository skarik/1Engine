#include "noisetest.h"
#include "renderer/texture/RrTexture3D.h"

constexpr static int kNoiseSeed = 1337;
constexpr static int kNoiseSize = 64;

void m04::test::worldgen::GeneratorTest::Setup ( void )
{
	StaticNoiseBlock<kNoiseSize> block;
	block.Generate(kNoiseSeed);

	noiseTexture = RrTexture3D::CreateUnitialized("loz_static_texture");
	noiseTexture->Upload(
		false,
		block.noise,
		kNoiseSize, kNoiseSize, kNoiseSize,
		core::gfx::tex::kColorFormatRGBA8UI,
		core::gfx::tex::kWrappingRepeat, core::gfx::tex::kWrappingRepeat, core::gfx::tex::kWrappingRepeat,
		core::gfx::tex::kMipmapGenerationNone, core::gfx::tex::kSamplingLinear);

	// for noise, want a function in the shader that is approximately 
	/*
	sample_noise( x, y, octaves )
	{
		float noise = 0.0;
		for (int i = 0; i < octaves; ++i )
		{
			noise += sample(x * i, y * i) / float(i + 1);
		}
		return noise;
	}

	*/

	// for the world
	// have a selection of "area shapes"
	// we want our sampling coordinates for these shapes to be in polar coordinates (r, angle)
	//
	//	position = (length((x,y)), atan(y, x))
	//

	// have a bunch of list of the circles


	// note that this shit is all in the mind so may not work
}