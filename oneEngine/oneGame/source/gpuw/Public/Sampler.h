#ifndef GPU_WRAPPER_PUBLIC_SAMPLER_H_
#define GPU_WRAPPER_PUBLIC_SAMPLER_H_

#include "core/types/types.h"
#include "core/gfx/textureFormats.h"
#include "renderer/types/types.h"

namespace gpu
{
	struct SamplerCreationDescription
	{
		// filter used when texture magnified
		core::gfx::tex::arSamplingFilter
							magFilter = core::gfx::tex::kSamplingLinear;
		// filter used when texture minified
		core::gfx::tex::arSamplingFilter
							minFilter = core::gfx::tex::kSamplingLinear;
		// mipmaps enabled?
		bool				mipmaps = false;
		// filter used with mipmap levels. Can only be Nearest Neighbor or Linear.
		core::gfx::tex::arSamplingFilter
							mipmapMode = core::gfx::tex::kSamplingLinear;
		// wrapping modes:
		core::gfx::tex::arWrappingType
							wrapmodeX = core::gfx::tex::kWrappingRepeat;
		core::gfx::tex::arWrappingType
							wrapmodeY = core::gfx::tex::kWrappingRepeat;
		core::gfx::tex::arWrappingType
							wrapmodeZ = core::gfx::tex::kWrappingRepeat;
		// border color for the sampler
		float				borderColor [4] = {};
		// Mipmap options:
		float				mipmapLodBias = 0.0F;	// LOD bias (TODO: desc)
		float				mipmapMinLod = -1000.0F;	// Minimum mipmap level to display
		float				mipmapMaxLod =  1000.0F;	// Maximum mipmap level to display
		// is anisotropic sampling enabled?
		bool				anisotropy = false;
		float				maxAnisotropy = 1.0F;	// Anisotropy level, anything greater than 1.0 is anisotropy.


		SamplerCreationDescription&
								MagFilter ( const core::gfx::tex::arSamplingFilter filter )
		{
			magFilter = filter;
			return *this;
		}

		SamplerCreationDescription&
								MinFilter ( const core::gfx::tex::arSamplingFilter filter )
		{
			minFilter = filter;
			return *this;
		}

		SamplerCreationDescription&
								WrapmodeX ( const core::gfx::tex::arWrappingType wrapmode )
		{
			wrapmodeX = wrapmode;
			return *this;
		}

		SamplerCreationDescription&
								WrapmodeY ( const core::gfx::tex::arWrappingType wrapmode )
		{
			wrapmodeY = wrapmode;
			return *this;
		}

		SamplerCreationDescription&
								WrapmodeZ ( const core::gfx::tex::arWrappingType wrapmode )
		{
			wrapmodeZ = wrapmode;
			return *this;
		}
	};
}

#endif//GPU_WRAPPER_PUBLIC_SAMPLER_H_