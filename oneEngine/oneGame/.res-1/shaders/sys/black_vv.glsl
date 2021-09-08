// sys/black
// Draws a purely black object.
#version 430

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_control_flow_attributes : require

#include "../common.glsli"
#include "../cbuffers.glsli"

// Inputs
layout(location = 0) in vec3 mdl_Vertex;
layout(location = 1) in vec3 mdl_TexCoord;

// Outputs to fragment shader
layout(location = 0) out vec2 v2f_texcoord0;

void main ( void )
{
	vec4 v_localPos = vec4( mdl_Vertex, 1.0 );
	vec4 v_screenPos = sys_ViewProjectionMatrix * vec4((sys_ModelTRS * v_localPos).xyz, 1.0);

	v2f_texcoord0	= mdl_TexCoord.xy;

	gl_Position = v_screenPos;
}
