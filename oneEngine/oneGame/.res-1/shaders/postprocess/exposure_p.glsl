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

//============================================================================//

void main ( void )
{
	// Get reference color
	vec4 pixelColor = texture(textureSamplerColor, v2f_texcoord0);
	
	// Get the luminosity we use to scale
	float pixelBrightness = Luminosity(pixelColor.rgb);
	// Scale color down by a constant value for now
	pixelColor *= max(0.0, (pixelBrightness * 0.95) / max(pixelBrightness, 0.0001));
	
	// Outut the final color
	FragColor = pixelColor;
}