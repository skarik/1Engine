#version 430

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_control_flow_attributes : require

#include "../common.glsli"
#include "../cbuffers.glsli"
#include "../common_lighting.glsli"
#include "../deferred_surface.glsli"

//=====================================

// Inputs from shade_common_vv
layout(location = 0) in vec4 v2f_position;
layout(location = 1) in vec2 v2f_texcoord0;
layout(location = 2) in flat int v2f_lightIndex;

//=====================================

// Texture Inputs/Outputs
layout(binding = 0, location = 20) uniform sampler2D textureDepth;

// Output to screen
layout(location = 0) out vec2 FragColor;

//=====================================

vec2 textureGatherMinMax ( sampler2D inputSampler, ivec2 baseCoords, int level )
{
	vec2 gatheredTexel1 = texelFetch(inputSampler, baseCoords + ivec2(0, 0), level).rg;
	vec2 gatheredTexel2 = texelFetch(inputSampler, baseCoords + ivec2(1, 0), level).rg;
	vec2 gatheredTexel3 = texelFetch(inputSampler, baseCoords + ivec2(0, 1), level).rg;
	vec2 gatheredTexel4 = texelFetch(inputSampler, baseCoords + ivec2(1, 1), level).rg;
	
	return vec2(
		min(gatheredTexel1.r, min(gatheredTexel2.r, min(gatheredTexel3.r, gatheredTexel4.r))),
		max(gatheredTexel1.g, max(gatheredTexel2.g, max(gatheredTexel3.g, gatheredTexel4.g)))
		);
}

void main ( void )
{
	const ivec2 baseTexelCoords = 4 * ivec2(gl_FragCoord.xy);
	ivec2 gatherCoords1 = baseTexelCoords + ivec2(0, 0);
	ivec2 gatherCoords2 = baseTexelCoords + ivec2(2, 0);
	ivec2 gatherCoords3 = baseTexelCoords + ivec2(0, 2);
	ivec2 gatherCoords4 = baseTexelCoords + ivec2(2, 2);
	
	vec2 gatheredTexel1 = textureGatherMinMax(textureDepth, gatherCoords1, 0).rg;
	vec2 gatheredTexel2 = textureGatherMinMax(textureDepth, gatherCoords2, 0).rg;
	vec2 gatheredTexel3 = textureGatherMinMax(textureDepth, gatherCoords3, 0).rg;
	vec2 gatheredTexel4 = textureGatherMinMax(textureDepth, gatherCoords4, 0).rg;
	
	float minDepth = min(gatheredTexel1.r, min(gatheredTexel2.r, min(gatheredTexel3.r, gatheredTexel4.r)));
	float maxDepth = max(gatheredTexel1.g, max(gatheredTexel2.g, max(gatheredTexel3.g, gatheredTexel4.g)));
	
	FragColor.rg = vec2(minDepth, maxDepth);
}