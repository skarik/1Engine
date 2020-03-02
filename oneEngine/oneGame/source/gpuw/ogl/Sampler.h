#ifndef _GPU_WRAPPER_SAMPLER_H_
#define _GPU_WRAPPER_SAMPLER_H_

#include "core/types/types.h"
#include "core/gfx/textureFormats.h"
#include "renderer/types/types.h"

namespace gpu
{
	class Device;

	struct SamplerCreationDescription
	{
		// filter used when texture magnified
		core::gfx::tex::arSamplingFilter
							magFilter;
		// filter used when texture minified
		core::gfx::tex::arSamplingFilter
							minFilter;
		// mipmaps enabled?
		bool				mipmaps;
		// filter used with mipmap levels. Can only be Nearest Neighbor or Linear.
		core::gfx::tex::arSamplingFilter
							mipmapMode;
		// wrapping modes:
		core::gfx::tex::arWrappingType
							wrapmodeX;
		core::gfx::tex::arWrappingType
							wrapmodeY;
		core::gfx::tex::arWrappingType
							wrapmodeZ;
		// border color for the sampler
		float				borderColor [4];
		// Mipmap options:
		float				mipmapLodBias;	// LOD bias (TODO: desc)
		float				mipmapMinLod;	// Minimum mipmap level to display
		float				mipmapMaxLod;	// Maximum mipmap level to display
		// is anisotropic sampling enabled?
		bool				anisotropy;
		float				maxAnisotropy;	// Anisotropy level, anything greater than 1.0 is anisotropy.

		// Default values:
		SamplerCreationDescription ( void )
			: magFilter(core::gfx::tex::kSamplingLinear), minFilter(core::gfx::tex::kSamplingLinear),
			mipmaps(false), mipmapMode(core::gfx::tex::kSamplingLinear),
			wrapmodeX(core::gfx::tex::kWrappingRepeat), wrapmodeY(core::gfx::tex::kWrappingRepeat), wrapmodeZ(core::gfx::tex::kWrappingRepeat),
			borderColor(),
			mipmapLodBias(0.0F),
			mipmapMinLod(-1000.0F), mipmapMaxLod(1000.0F),
			anisotropy(false), maxAnisotropy(1.0F)
			{}
	};

	// defines how a texture is sampled within the shader
	class Sampler
	{
	public:
		GPUW_API explicit			Sampler ( void )
			: m_sampler(0)
			{}

		GPUW_API int			create ( Device* device, const SamplerCreationDescription* params );
		GPUW_API int			destroy ( Device* device );

		//	valid() : is this sampler valid to be used?
		GPUW_API bool			valid ( void );
		//	nativePtr() : returns native index or pointer to the resource.
		GPUW_API gpuHandle		nativePtr ( void );

	private:
		unsigned int		m_sampler;
	};

}

#endif//_GPU_WRAPPER_TEXTURES_H_