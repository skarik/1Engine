#version 330

in vec3 mdl_Vertex;
in vec3 mdl_TexCoord;
//in vec4 mdl_Color;
//in vec3 mdl_Normal;

// Outputs to fragment shader
varying vec4 v2f_position;
//varying vec4 v2f_colors;
varying vec2 v2f_texcoord0;

uniform mat4 sys_ModelViewProjectionMatrix;

//uniform vec4 sys_DiffuseColor;

void main ( void )
{
	vec4 v_screenPos = sys_ModelViewProjectionMatrix * vec4( mdl_Vertex, 1.0 );

	v2f_position	= vec4( mdl_Vertex, 1 );
	//v2f_colors		= sys_DiffuseColor;
	v2f_texcoord0	= mdl_TexCoord.xy;

	gl_Position = v_screenPos;
}