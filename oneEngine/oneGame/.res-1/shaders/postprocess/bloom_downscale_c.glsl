#version 430

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_control_flow_attributes : require

#include "../common.glsli"
#include "../cbuffers.glsli"
#include "../common_lighting.glsli"
#include "../deferred_surface.glsli"
#include "../colorspaces.glsli"
#include "../common_math.glsli"

//=====================================

// Texture Inputs/Outputs
layout(rgba16f, binding = 0, location = 20) uniform writeonly image2D textureColorDownscaled4;
layout(rgba16f, binding = 1, location = 21) uniform writeonly image2D textureColorDownscaled16;
layout(binding = 2, location = 22) uniform sampler2D textureColor;

//=====================================

layout(local_size_x = 4, local_size_y = 4, local_size_z = 1) in;

shared vec4 s_workgroupColors [16];

vec4 textureGatherAverage ( sampler2D inputSampler, vec2 baseCoords, vec2 texelSize )
{
	/*baseCoords += texelSize * 0.5;
	vec4 localAverage =
		( texture(inputSampler, baseCoords)
		+ texture(inputSampler, baseCoords + vec2(texelSize.x, 0))
		+ texture(inputSampler, baseCoords + vec2(0, texelSize.y))
		+ texture(inputSampler, baseCoords + texelSize)) / 4.0;*/
		
	// Use linear sampling to sample in the middle of all four
	vec4 localAverage = texture(inputSampler, baseCoords + texelSize);
	
	// Apply the bloom bias & multiply now
	vec3  bloomColor     = localAverage.rgb;
	float bloomColorLuma = Luminosity(bloomColor.rgb);
	vec3  bloomResult    = bloomColor.rgb * saturate(bloomColorLuma - 0.95);
	
	return vec4(bloomResult, 1.0);
}

void main ( void )
{
	// rough method stolen from https://miketuritzin.com/post/hierarchical-depth-buffers/
	// parallel reduction stolen from
	//	- https://mynameismjp.wordpress.com/2011/08/10/average-luminance-compute-shader/
	//	- https://graptis.blogspot.com/2013/06/compute-shaders-parallel-reduction-for.html
	
	// Fetch 4x4 texel region with textureGather (each textureGather is 2x2)
	
	const vec2 texelSize = vec2(1.0, 1.0) / sys_ScreenSize.xy;
	
	const uvec2 baseTexelCoords = 4 * gl_GlobalInvocationID.xy;
	vec2 gatherCoords1 = (baseTexelCoords + uvec2(0, 0)) * texelSize;
	vec2 gatherCoords2 = (baseTexelCoords + uvec2(2, 0)) * texelSize;
	vec2 gatherCoords3 = (baseTexelCoords + uvec2(0, 2)) * texelSize;
	vec2 gatherCoords4 = (baseTexelCoords + uvec2(2, 2)) * texelSize;
	
	vec4 gatheredTexelValues1 = textureGatherAverage(textureColor, gatherCoords1, texelSize);
	vec4 gatheredTexelValues2 = textureGatherAverage(textureColor, gatherCoords2, texelSize);
	vec4 gatheredTexelValues3 = textureGatherAverage(textureColor, gatherCoords3, texelSize);
	vec4 gatheredTexelValues4 = textureGatherAverage(textureColor, gatherCoords4, texelSize);
	
	// Select smallest & largest values across the region
	vec4 finalAverage = (gatheredTexelValues1 + gatheredTexelValues2 + gatheredTexelValues3 + gatheredTexelValues4) / 4.0;
	
	// Save the results to the local 4x downscale buffer
	imageStore(textureColorDownscaled4, ivec2(gl_GlobalInvocationID.xy), finalAverage);
	
	// Save the results to the workgroup value
	s_workgroupColors[gl_LocalInvocationIndex] = finalAverage;
	
	// Sync up all the threads in the group
	memoryBarrierShared();
	barrier();
	
	// Calculate the average results.
	const uint kTotalThreads = 4 * 4;
	[[unroll]]
	for ( uint s = kTotalThreads / 2; s > 0; s >>= 1 )
	{
		// Sum up the upper half of the valid values
		if (gl_LocalInvocationIndex < s)
			s_workgroupColors[gl_LocalInvocationIndex] += s_workgroupColors[gl_LocalInvocationIndex + s];
		memoryBarrierShared();
		barrier();
	}
	vec4 totalAverage = s_workgroupColors[0] / 16.0;
	
	// Store the result colors to the 16x downscale buffer
	if (gl_LocalInvocationIndex == 0)
	{
		imageStore(textureColorDownscaled16, ivec2(gl_WorkGroupID.xy), totalAverage);
	}
}