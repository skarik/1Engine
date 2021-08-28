#version 430

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_control_flow_attributes : require

#include "../cbuffers.glsli"
#include "../deferred_surface.glsli"
#include "../colorspaces.glsli"
#include "../common_math.glsli"

//============================================================================//

// Output to screen
layout(location = 0) out vec4 FragColor;

// Inputs from vertex shader
layout(location = 0) in vec4 v2f_position;
layout(location = 1) in vec2 v2f_texcoord0;
layout(location = 2) in flat int v2f_lightIndex;

// Previous forward rendered output
layout(binding = 0, location = 25) uniform sampler2D textureSamplerColor;
layout(binding = 1, location = 26) uniform sampler2D textureSamplerDepth;
layout(binding = 2, location = 27) uniform sampler3D textureSamplerLUTPrimary;
layout(binding = 3, location = 28) uniform sampler3D textureSamplerLUTSecondary;

//============================================================================//

// Performs dithering
float calculateAlphaDither (float alpha, vec2 clip_position)
{
    const int indexMatrix4x4[16] = int[](
        0,  8,  2,  10,
        12, 4,  14, 6,
        3,  11, 1,  9,
        15, 7,  13, 5);

	ivec2 pixel_index = ivec2( int(clip_position.x), int(clip_position.y) );
	int index = int(mod(float(pixel_index.x), 4.0)) + int(mod(float(pixel_index.y), 4.0)) * 4;
	float index_value = float(indexMatrix4x4[index]) / 16.0 + 0.001;

	return (alpha <= index_value) ? 0.0 : 1.0;
}

float calculateLinearWeight(float value1, float value2)
{
	const float curve = 20.0;
	if (value1 < value2)
	{
		// Closer to value1, on lower half of the curve
	    return pow(value1 / value2, curve) * 0.5;
	}
	else
	{
		// Closer to value2, on upper half of the curve
	    return 1.0 - pow(value2 / value1, curve) * 0.5;
	}
}

float rgbDistanceSqr(vec3 colorA, vec3 colorB)
{
    vec3 delta = (colorA - colorB);
    return dot(delta, delta);
}

//============================================================================//

void main ( void )
{
	// Sample the center pixel's depth.
	float referenceDepth = LinearizeZBufferDepth(texture(textureSamplerDepth, v2f_texcoord0).x);
	
	// Get reference color
	vec4 pixelColor = texture(textureSamplerColor, v2f_texcoord0);
	
	// Get the two closest colors to the LUT
	ivec3 lutLookup = ivec3(saturate(pixelColor.xyz) * 255);
	vec4 paletteColor0 = texelFetch(textureSamplerLUTPrimary, lutLookup, 0);
	vec4 paletteColor1 = texelFetch(textureSamplerLUTSecondary, lutLookup, 0);
	
	// Get the percent between the primary and secondary lookup
	vec2 color_distances = vec2( rgbDistanceSqr(pixelColor.rgb, paletteColor0.rgb), rgbDistanceSqr(pixelColor.rgb, paletteColor1.rgb) );
	float percent = calculateLinearWeight(color_distances.x, color_distances.y);
	
	// Generate dither scale (should stay 2x from 1080p to 720p, only scaling below or above that range.
	float ditherScale = min(
		sys_ScreenSize.x > 1280 ? max(1.0, sys_ScreenSize.x / 1920.0) : min(1.0, sys_ScreenSize.x / 1280.0),
		sys_ScreenSize.y > 720  ? max(1.0, sys_ScreenSize.y / 1080.0) : min(1.0, sys_ScreenSize.y / 720.0)
		);
	vec2 ditherCoords = floor(gl_FragCoord.xy / (2.0 * ditherScale));
	// Dither blend between the two cloest colors
	pixelColor.rgb = mix(paletteColor0.rgb, paletteColor1.rgb, calculateAlphaDither(percent, ditherCoords));
	
	FragColor = pixelColor;
}