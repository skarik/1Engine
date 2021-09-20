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
layout(binding = 1, location = 26) uniform sampler2D textureSamplerBlurred;
layout(binding = 2, location = 27) uniform sampler2D textureSamplerBlurredHi;

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
	// Get reference color
	vec4 pixelColor = texture(textureSamplerColor, v2f_texcoord0);
	
	// Get bloom color
	const vec2 screenSizeDivisor = sys_ScreenSize.xy * 0.25;
	vec4 bloomColor = texture(textureSamplerBlurred, floor(v2f_texcoord0 * screenSizeDivisor) / screenSizeDivisor) * 1.5;
	bloomColor += texture(textureSamplerBlurredHi, floor(v2f_texcoord0 * screenSizeDivisor) / screenSizeDivisor) * 0.5;
	//vec3 bloomColorHSV = RGBtoHSV(bloomColor.rgb);
	//float bloomColorLuma = Luminosity(bloomColor.rgb);
	
	//vec3 bloomResult = bloomColor.rgb * saturate(bloomColorLuma - 0.93);
	vec3 bloomResult = bloomColor.rgb;
	
	// Add the bloom
	pixelColor.rgb += bloomResult * 0.2;
	
	FragColor = pixelColor;
}