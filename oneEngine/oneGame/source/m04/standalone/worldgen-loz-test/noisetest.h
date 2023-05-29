#ifndef M04_TEST_LOZ_WORLDGEN_H_
#define M04_TEST_LOZ_WORLDGEN_H_

#include "core/math/random/Random.h"

class RrTexture;
class RrTexture3D;

namespace m04 {
namespace test {
namespace worldgen {

	template <int Size>
	class StaticNoiseBlock
	{
	public:
		//	Generate() : Sets up all the default values used for the noise block.
		void					Generate ( int seed )
		{
			std::mt19937 l_generator;
			l_generator.seed(seed);

			for (int i = 0; i < Size * Size * Size; ++i)
			{
				noise[i] = l_generator();
			}
		}

	public:
		uint32_t			noise [Size * Size * Size];
	};

	class GeneratorTest
	{
	public:
		void					Setup ( void );

	private:
		RrTexture3D*		noiseTexture = NULL;
	};


}}}

#endif//M04_TEST_LOZ_WORLDGEN_H_