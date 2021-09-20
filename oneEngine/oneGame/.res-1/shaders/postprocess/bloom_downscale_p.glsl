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

// Inputs from shade_common_vv
layout(location = 0) in vec4 v2f_position;
layout(location = 1) in vec2 v2f_texcoord0;
layout(location = 2) in flat int v2f_lightIndex;

//=====================================

// Texture Inputs/Outputs
layout(binding = 0, location = 20) uniform sampler2D textureColor;

// CBuffer Params
layout(binding = CBUFFER_USER0, std430) uniform sys_cbuffer_DownscaleParams
{
	vec2	downscaleParam_InputSize;
	vec2	downscaleParam_OutputSize;
	float	downscaleParam_BloomBias;
};

// Output to screen
layout(location = 0) out vec4 FragColor;

//=====================================

vec4 textureGatherAverage ( sampler2D inputSampler, vec2 baseCoords, vec2 texelSize )
{
	// Use linear sampling to sample in the middle of all four
	vec4 localAverage = texture(inputSampler, baseCoords + texelSize);
	
	// Apply the bloom bias & multiply now
	vec3  bloomColor     = localAverage.rgb;
	float bloomColorLuma = Luminosity(bloomColor.rgb);
	vec3  bloomResult    = bloomColor.rgb * min(2.0, max(0.0, bloomColorLuma - downscaleParam_BloomBias));
	
	return vec4(bloomResult, 1.0);
}

void main ( void )
{
	const vec2 texelSize = vec2(1.0, 1.0) / (downscaleParam_InputSize.xy); // Input texel size
	
	const vec2 baseTexelCoords = gl_FragCoord.xy / downscaleParam_OutputSize.xy;
	vec2 gatherCoords1 = baseTexelCoords + uvec2(0, 0) * texelSize;
	vec2 gatherCoords2 = baseTexelCoords + uvec2(2, 0) * texelSize;
	vec2 gatherCoords3 = baseTexelCoords + uvec2(0, 2) * texelSize;
	vec2 gatherCoords4 = baseTexelCoords + uvec2(2, 2) * texelSize;
	
	// Fetch 4x4 texel region with textureGather (each textureGather is 2x2)
	
	vec4 gatheredTexelValues1 = textureGatherAverage(textureColor, gatherCoords1, texelSize);
	vec4 gatheredTexelValues2 = textureGatherAverage(textureColor, gatherCoords2, texelSize);
	vec4 gatheredTexelValues3 = textureGatherAverage(textureColor, gatherCoords3, texelSize);
	vec4 gatheredTexelValues4 = textureGatherAverage(textureColor, gatherCoords4, texelSize);
	
	// Average values across the region
	vec4 finalAverage = (gatheredTexelValues1 + gatheredTexelValues2 + gatheredTexelValues3 + gatheredTexelValues4) / 4.0;
	
	// Save output
	FragColor = finalAverage;
}