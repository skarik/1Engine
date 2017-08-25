#version 140

in vec3 mdl_Vertex;
in vec3 mdl_TexCoord;
in vec4 mdl_Color;
in vec3 mdl_Normal;

// Outputs to fragment shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying float v2f_fogdensity;

// System inputs
uniform mat4 sys_ModelTRS;
uniform mat4 sys_ModelTRSInverse;
uniform mat4 sys_ModelViewProjectionMatrix;
uniform mat4 sys_ModelRS;

// Fog
layout(std140) uniform sys_Fog
{
	vec4	sys_FogColor;
	float 	sys_FogEnd;
	float 	sys_FogScale;
};

void main ( void )
{
	vec4 v_localPos = vec4( mdl_Vertex, 1.0 );
	vec4 v_screenPos = sys_ModelViewProjectionMatrix * v_localPos;

	v2f_normals		= sys_ModelRS * vec4( mdl_Normal, 1.0 );
	v2f_colors		= mdl_Color;
	v2f_position	= sys_ModelTRS * v_localPos;
	v2f_texcoord0	= mdl_TexCoord.xy;
	v2f_fogdensity  = max( 0.0, (sys_FogEnd - v_screenPos.z) * sys_FogScale );

	gl_Position = v_screenPos;
}