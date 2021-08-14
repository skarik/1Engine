// sys/fullbright
// Renders object without lighting, only diffuse multiply blending.
#version 430

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_control_flow_attributes : require

#include "../common.glsli"
#include "../cbuffers.glsli"

// Inputs from vertex shader
// [None]

// Samplers
layout(binding = 0, location = 20) uniform samplerCube textureSampler0;

// Outputs
layout(location = 0) out vec4 FragDiffuse;

void main ( void )
{
	// Generate the screen ray so the output is the same no matter the camera position:
	vec4 l_screenSpaceRay = vec4(
		2.0 * ((gl_FragCoord.x - sys_ViewportInfo.x) / sys_ViewportInfo.z - 0.5),
		-2.0 * ((gl_FragCoord.y - sys_ViewportInfo.y) / sys_ViewportInfo.w - 0.5),
		1.0,
		1.0);
	const vec3 l_screenRay = normalize((sys_ModelViewProjectionMatrixInverse * l_screenSpaceRay).xyz);
	
	vec4 diffuseColor = texture(textureSampler0, l_screenRay);
	
	FragDiffuse = diffuseColor;
}