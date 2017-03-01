#ifndef _GPU_WRAPPER_TEXTURES_H_
#define _GPU_WRAPPER_TEXTURES_H_

#include "core/types/types.h"
#include "renderer/types/types.h"
#include "renderer/types/textureFormats.h"

namespace GPU
{
	// Create a read/write buffer
	RENDER_API glHandle TextureAllocate(
		const glEnum textureType,
		const glEnum textureFormat, 
		const uint width = 0, const uint height = 0, const uint depth = 0
	);
	RENDER_API int TextureSampleSettings(
		const glEnum textureType,
		const glHandle texture,
		const glEnum repeatX, const glEnum repeatY, const glEnum repeatZ,
		const glEnum sampleMinify, const glEnum sampleMagnify
	);
	// Free read/write buffer
	RENDER_API int TextureFree(
		const glHandle texture
	);

	// Create a write-only buffer
	RENDER_API glHandle TextureBufferAllocate(
		const glEnum textureType,
		const glEnum textureFormat, 
		const uint width = 0, const uint height = 0, const uint depth = 0
	);
	// Free write-only buffer
	RENDER_API int TextureBufferFree(
		const glHandle texture
	);
}

#endif//_GPU_WRAPPER_TEXTURES_H_