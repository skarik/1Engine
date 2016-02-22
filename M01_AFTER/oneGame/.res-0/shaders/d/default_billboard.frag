#version 330

// Inputs from vertex shader
varying vec4 v2f_position;
varying vec4 v2f_colors;
varying vec2 v2f_texcoord0;

// Samplers
uniform sampler2D textureSampler0;
uniform vec4 sys_DiffuseColor;

void main ( void )  
{
	vec4 diffuseColor = texture2D( textureSampler0, v2f_texcoord0 );
	gl_FragColor = diffuseColor * sys_DiffuseColor;//v2f_colors;
}