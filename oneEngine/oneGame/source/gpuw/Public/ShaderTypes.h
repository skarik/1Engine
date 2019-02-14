#ifndef GPU_WRAPPER_PUBLIC_SHADER_TYPES_H_
#define GPU_WRAPPER_PUBLIC_SHADER_TYPES_H_

namespace gpu
{
	enum ShaderStage
	{
		kShaderStageVs,		// Vertex shader
		kShaderStageHs,		// Hull shader
		kShaderStageDs,		// Domain shader
		kShaderStageGs,		// Geometry shader
		kShaderStagePs,		// Pixel shader
		kShaderStageCs,		// Compute shader
	};
}

#endif//GPU_WRAPPER_PUBLIC_SHADER_TYPES_H_