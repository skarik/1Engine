// sys/fullbright
// Renders object without lighting, only diffuse multiply blending.
#version 430

#extension GL_GOOGLE_include_directive : require
#include "../common.glsli"
#include "../cbuffers.glsli"

layout(location = 0) in vec3 mdl_Vertex;

// Outputs to fragment shader
// [None]

void main ( void )
{
	vec4 v_localPos = vec4( mdl_Vertex, 1.0 );
	vec4 v_screenPos = sys_ModelViewProjectionMatrix * v_localPos;

	gl_Position = v_screenPos;
}
