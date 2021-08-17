#version 430

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_control_flow_attributes : require

#include "../common.glsli"
#include "../cbuffers.glsli"
#include "../common_lighting.glsli"
#include "../deferred_surface.glsli"

//=====================================

// Texture Inputs/Outputs
layout(rg32f, binding = 0, location = 20) uniform writeonly image2D textureDepthDownscaled4;
layout(rg32f, binding = 1, location = 21) uniform writeonly image2D textureDepthDownscaled16;
layout(binding = 2, location = 22) uniform sampler2D textureDepth;

//=====================================

layout(local_size_x = 4, local_size_y = 4, local_size_z = 1) in;

shared uint s_workgroupMinDepthEncoded;
shared uint s_workgroupMaxDepthEncoded;

void main ( void )
{
	// rough method stolen from https://miketuritzin.com/post/hierarchical-depth-buffers/
	
	if (gl_LocalInvocationIndex == 0)
	{
		s_workgroupMinDepthEncoded = floatBitsToUint(1.0);
		s_workgroupMaxDepthEncoded = floatBitsToUint(0.0);
	}
	
	// Sync up all the threads in the group
	memoryBarrierShared();
	barrier();
	
	// Fetch 4x4 texel region with textureGather (each textureGather is 2x2)
	
	const vec2 texelSize = vec2(1.0, 1.0) / sys_ScreenSize.xy;
	
	const uvec2 baseTexelCoords = 4 * gl_GlobalInvocationID.xy;
	vec2 gatherCoords1 = (baseTexelCoords + uvec2(1, 1)) * texelSize;
	vec2 gatherCoords2 = (baseTexelCoords + uvec2(3, 1)) * texelSize;
	vec2 gatherCoords3 = (baseTexelCoords + uvec2(1, 3)) * texelSize;
	vec2 gatherCoords4 = (baseTexelCoords + uvec2(3, 3)) * texelSize;
	
	vec4 gatheredTexelValues1 = textureGather(textureDepth, gatherCoords1);
	vec4 gatheredTexelValues2 = textureGather(textureDepth, gatherCoords2);
	vec4 gatheredTexelValues3 = textureGather(textureDepth, gatherCoords3);
	vec4 gatheredTexelValues4 = textureGather(textureDepth, gatherCoords4);
	
	// Select smallest & largest values across the region
	vec4 gatheredMin = min(min(gatheredTexelValues1, gatheredTexelValues2),
						   min(gatheredTexelValues3, gatheredTexelValues4));
	vec4 gatheredMax = max(max(gatheredTexelValues1, gatheredTexelValues2),
						   max(gatheredTexelValues3, gatheredTexelValues4));
						   
	float finalMin = min(min(gatheredMin.x, gatheredMin.y), min(gatheredMin.z, gatheredMin.w));
	float finalMax = max(max(gatheredMax.x, gatheredMax.y), max(gatheredMax.z, gatheredMax.w));
	
	// Save the results to the local 4x downscale buffer
	imageStore(textureDepthDownscaled4, ivec2(gl_GlobalInvocationID.xy), vec4(finalMin, finalMax, 0.0, 0.0));
	
	// Save the results to the workgroup value
	atomicMin(s_workgroupMinDepthEncoded, floatBitsToUint(finalMin));
	atomicMax(s_workgroupMaxDepthEncoded, floatBitsToUint(finalMax));
	
	// Sync up all the threads in the group
	memoryBarrierShared();
	barrier();
	
	// Store the result depths to the 16x downscale buffer
	if (gl_LocalInvocationIndex == 0)
	{
		float minDepth = uintBitsToFloat(s_workgroupMinDepthEncoded);
		float maxDepth = uintBitsToFloat(s_workgroupMaxDepthEncoded);
		
		imageStore(textureDepthDownscaled16, ivec2(gl_WorkGroupID.xy), vec4(minDepth, maxDepth, 0.0, 0.0));
	}
}