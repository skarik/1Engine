#include "gpuw/gpuw_common.h"
#include "gpuw/base/Texture.base.h"

/*#ifdef GPU_API_OPENGL
#	include "gpuw/ogl/Texture.h"
#endif

#ifdef GPU_API_VULKAN
#	include "gpuw/vk/Texture.vk.h"
#endif

#ifdef GPU_API_DIRECTX11
#	include "gpuw/dx11/Texture.dx11.h"
#endif*/

namespace gpu
{
	class TextureDynamic : public gpu::base::Texture
	{
	public:
		// @brief Is this texture valid to be used?
		//		If the texture has not been created, it will be removed.
		GPUW_API bool			valid ( void ) const override
		{
			return pInternal ? pInternal->valid() : false;
		}
		
		// @brief Returns native index or pointer to the resource.
		GPUW_API gpuHandle		nativePtr ( void ) override
		{
			return pInternal ? pInternal->nativePtr() : gpuHandle();
		}

		// @brief Creates a texture
		GPUW_API int			allocate (
			const core::gfx::tex::arTextureType textureType,
			const core::gfx::tex::arColorFormat textureFormat, 
			const uint width = 0, const uint height = 0, const uint depth = 0, const uint levels = 0
		) override;

		// Sampler is in a different object.

		// @brief Destroys any allocated texture, if existing.
		GPUW_API int			free ( void ) override;

		// @brief Uploads data to the texture from a buffer
		GPUW_API int			upload ( gpu::base::BaseContext* context, gpu::base::Buffer& buffer, const uint level, const uint arraySlice ) override
		{
			return pInternal ? pInternal->upload(context, buffer, level, arraySlice) : gpu::kErrorNullReference;
		}

		// @brief Uploads data to a buffer from this texture
		GPUW_API int			copy ( gpu::base::BaseContext* context, gpu::base::Buffer& buffer, const uint level, const uint arraySlice ) override
		{
			return pInternal ? pInternal->copy(context, buffer, level, arraySlice) : gpu::kErrorNullReference;
		}

	private:
		gpu::base::Texture*	pInternal = nullptr;
	};

	typedef TextureDynamic Texture;
}