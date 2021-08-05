// sys/fullbright
// Renders object without lighting, only diffuse multiply blending.
#version 430

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_control_flow_attributes : require

#include "../common.glsli"
#include "../cbuffers.glsli"
#include "output_common_p.glsli"

// Inputs from vertex shader
layout(location = 0) in vec4 v2f_colors;
layout(location = 1) in vec2 v2f_texcoord0;
layout(location = 2) in vec3 v2f_normal;

// Samplers
layout(binding = 0, location = 20) uniform sampler2D textureSampler0;

void main ( void )
{
	vec4 diffuseColor = texture( textureSampler0, v2f_texcoord0 );
	if (diffuseColor.a <= sys_AlphaCutoff) discard;

	rrSurfaceInfo result;
	
	result.albedo = diffuseColor * v2f_colors;
	result.normal = normalize(gl_FrontFacing ? v2f_normal : -v2f_normal);
	
	result.metalness = 0.0;
	result.smoothness = 0.0;
	result.emissive = vec3(0, 0, 0);
	
	PushOutput(result);
}
