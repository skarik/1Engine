#version 140

in vec3 mdl_Vertex;
in vec3 mdl_TexCoord;
in vec4 mdl_Color;

// Outputs to fragment shader
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying vec2 v2f_texcoord_offs;

uniform vec3 gm_SunScreenPosition = vec3( 0.5,0.75,1.0 );

// System inputs
//uniform mat4 sys_ModelViewProjectionMatrix;

void main ( void )
{
	vec4 v_screenPos = /*sys_ModelViewProjectionMatrix **/ vec4( mdl_Vertex, 1.0 );

	v2f_colors		= mdl_Color;
	v2f_position	= vec4( mdl_Vertex, 1.0 );
	v2f_texcoord0	= mdl_TexCoord.xy;
	v2f_texcoord_offs = (gm_SunScreenPosition.xy-v2f_texcoord0)*0.008;

	gl_Position = v_screenPos;
}