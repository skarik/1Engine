// sys/default
// Default forward rendering shader.
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
layout(location = 3) out float v2f_fogdensity;

void main ( void )
{
	vec4 v_localPos = vec4(mdl_Vertex, 1.0);
	vec4 v_screenPos = sys_ViewProjectionMatrix * vec4((sys_ModelTRS * v_localPos).xyz, 1.0);

	vec4 v2f_normals	= sys_ModelR * vec4( mdl_Normal, 1.0 );
	v2f_colors			= sys_DiffuseColor * mdl_Color;
	v2f_position		= sys_ModelTRS * v_localPos;
	v2f_texcoord0		= mdl_TexCoord.xy;
	v2f_fogdensity 	 	= max( 0.0, (sys_FogEnd - v_screenPos.z) * sys_FogScale );

	gl_Position = v_screenPos;
}
