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

		// Vega/Navi+:
		kShaderStagePrim,	// Primitive shader
		kShaderStageMesh,	// Mesh shader
		kShaderStageAmpl,	// Amplification shader

		// Raytracing:
		kShaderStageRGen,	// Raygen shader
		kShaderStageRInt,	// Intersection shader
		kShaderStageRCHit,	// Closest-hit shader
		kShaderStageRAHit,	// Any-hit shader
		kShaderStageRMiss,	// Miss shader
	};
}

#endif//GPU_WRAPPER_PUBLIC_SHADER_TYPES_H_