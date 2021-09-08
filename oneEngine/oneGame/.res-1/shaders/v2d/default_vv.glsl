// v2d/default
// Default shader for 2D GUI elements.
#version 430

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_control_flow_attributes : require

#include "../common.glsli"
#include "../cbuffers.glsli"

layout(location = 0) in vec3 mdl_Vertex;
layout(location = 1) in vec3 mdl_TexCoord;
layout(location = 2) in vec4 mdl_Color;
layout(location = 3) in vec3 mdl_Normal;

// Outputs to fragment shader
layout(location = 0) out vec4 v2f_colors;
layout(location = 1) out vec4 v2f_position;
layout(location = 2) out vec2 v2f_texcoord0;

void main ( void )
{
	vec4 v_localPos = vec4( mdl_Vertex, 1.0 );
    vec4 v_screenPos = sys_ModelTRS * v_localPos;

	v2f_colors		= mdl_Color;
	v2f_position	= v_localPos;
	v2f_texcoord0   = mdl_TexCoord.xy * sys_TextureScale.xy + sys_TextureOffset.xy;

	gl_Position = v_screenPos;
}
