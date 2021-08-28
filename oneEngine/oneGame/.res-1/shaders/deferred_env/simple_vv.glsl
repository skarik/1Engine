// sys/fullbright
// Renders object without lighting, only diffuse multiply blending.
#version 430

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_control_flow_attributes : require

#include "../common.glsli"
#include "../cbuffers.glsli"

layout(location = 0) in vec3 mdl_Vertex;
layout(location = 1) in vec3 mdl_TexCoord;
layout(location = 2) in vec4 mdl_Color;
layout(location = 3) in vec3 mdl_Normal;
layout(location = 4) in vec3 mdl_Tangent;
layout(location = 5) in vec3 mdl_Binormal;

// Outputs to fragment shader
layout(location = 0) out vec4 v2f_colors;
layout(location = 1) out vec2 v2f_texcoord0;
layout(location = 2) out vec3 v2f_normal;

void main ( void )
{
	vec4 v_localPos = vec4( mdl_Vertex, 1.0 );
	vec4 v_screenPos = sys_ModelViewProjectionMatrix * v_localPos;
	// TODO: Move this into either .res-0 or make an option.
	v_screenPos.xy = floor( v_screenPos.xy * (sys_ScreenSize * 0.25 / v_screenPos.z) ) / (sys_ScreenSize * 0.25 / v_screenPos.z);

	v2f_colors		= mdl_Color * sys_DiffuseColor;
	v2f_texcoord0	= mdl_TexCoord.xy;
	v2f_normal		= mat3(sys_ModelR) * mdl_Normal.xyz;

	gl_Position = v_screenPos;
}
