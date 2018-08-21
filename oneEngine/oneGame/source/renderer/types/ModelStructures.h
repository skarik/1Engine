#ifndef _RENDERER_TYPES_MODEL_STRUCTURES_H_
#define _RENDERER_TYPES_MODEL_STRUCTURES_H_

#include "core/types/types.h"
#include "renderer/types/types.h"

// Skinning information
struct skinningData_t
{
	uint32_t	bonecount;
	//glHandle	textureBuffer;
	gpuHandle	textureBufferData;
	gpuHandle	textureBufferData_Swap;
	//glHandle	textureSoftBuffer;
	//glHandle	textureSoftBufferData;
	//glHandle	textureSoftBufferData_Swap;
};

#endif//_RENDERER_TYPES_MODEL_STRUCTURES_H_