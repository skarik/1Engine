#version 140

in vec3 mdl_Vertex;
in vec3 mdl_TexCoord;
in vec4 mdl_Color;
in vec3 mdl_Normal;

varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec2 v2f_texcoord0;
varying vec4 v2f_position;
varying float v2f_fogdensity;

uniform mat4 sys_ModelMatrix;
uniform mat4 sys_ModelRotationMatrix;
uniform mat4 sys_ModelMatrixInverse;
uniform mat4 sys_ModelViewProjectionMatrix;

// Fog
layout(std140) uniform sys_Fog
{
	vec4	sys_FogColor;
	vec4	sys_AtmoColor;
	float 	sys_FogEnd;
	float 	sys_FogScale;
};

void main ( void )
{
	v2f_normals		= sys_ModelRotationMatrix*vec4( mdl_Normal, 1.0 );
	v2f_normals.w	= mdl_Normal.x;
	v2f_colors		= mdl_Color;
	v2f_texcoord0	= mdl_TexCoord.xy;
	v2f_position	= sys_ModelMatrix*vec4( mdl_Vertex, 1.0 );

	vec4 v_screenPos = sys_ModelViewProjectionMatrix * vec4( mdl_Vertex, 1.0 );
	v2f_fogdensity = clamp( (sys_FogEnd - v_screenPos.z) * sys_FogScale, 0, 1 );

	gl_Position = v_screenPos;
}
