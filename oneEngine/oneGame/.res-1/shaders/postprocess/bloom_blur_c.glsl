#version 430

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_control_flow_attributes : require

#include "../common.glsli"
#include "../cbuffers.glsli"
#include "../common_lighting.glsli"
#include "../deferred_surface.glsli"

//=====================================

// Texture Inputs/Outputs
layout(binding = 0, location = 20) uniform sampler2D textureColor;
layout(rgba16f, binding = 1, location = 21) uniform writeonly image2D textureColorBlurred;

layout(binding = CBUFFER_USER0, std430) uniform sys_cbuffer_BlurParams
{
	vec2	blurParam_Direction;
	int		taps;
};

//=====================================

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

void main ( void )
{
	ivec2 uv0 = ivec2(gl_GlobalInvocationID.xy);
	if (uv0.x >= sys_ScreenSize.x || uv0.y >= sys_ScreenSize.y)
		return; // Skip out-of-range threads
		
	const uvec2 imageSize = textureSize(textureColor, 0);
	const vec2 texelSize = vec2(1, 1) / imageSize;
	const vec2 normalizedTexelCoords = vec2(uv0) * texelSize;
	
	[[branch]]
	if (taps == 7)
	{
		// Point-sampled 7-tap gaussian blur
		const float kOffsets [4] = {
			0.0,
			1.0,
			2.0,
			3.0
		};	
		const float kWeights [4] = {
			0.383103,
			0.241843,
			0.060626,
			0.005980
		};
		
		const vec2 uv0_center = normalizedTexelCoords + vec2(0.5, 0.5) * texelSize;
		
		vec4 result = texture( textureColor, uv0_center ) * kWeights[0];
		
		[[unroll]]
		for ( uint i = 1; i < 4; ++i )
		{
			result += texture( textureColor, uv0_center + (blurParam_Direction.xy * kOffsets[i]) * texelSize ) * kWeights[i];
			result += texture( textureColor, uv0_center - (blurParam_Direction.xy * kOffsets[i]) * texelSize ) * kWeights[i];
		}
		
		imageStore(textureColorBlurred, ivec2(uv0), result);
	}
	else if (taps == 9)
	{
		// Linear-sampled 9-tap gaussian blur
		const float kOffsets [5] = {
			0.00000000,
			1.42857143,
			3.33333333,
			5.23809524,
			7.14285714,
		};
		const float kWeights [5] = {
			0.17620411,
			0.28032472,
			0.11089769,
			0.01940710,
			0.00126844,
		};
		
		const vec2 uv0_center = normalizedTexelCoords + vec2(0.5, 0.5) * texelSize;
		
		vec4 result = texture( textureColor, uv0_center ) * kWeights[0];
		
		[[unroll]]
		for ( uint i = 1; i < 5; ++i )
		{
			result += texture( textureColor, uv0_center + (blurParam_Direction.xy * kOffsets[i]) * texelSize ) * kWeights[i];
			result += texture( textureColor, uv0_center - (blurParam_Direction.xy * kOffsets[i]) * texelSize ) * kWeights[i];
		}
		
		imageStore(textureColorBlurred, ivec2(uv0), result);
	}
}