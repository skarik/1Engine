// sys/black
// Draws a purely black object.
#version 330
// Inputs from vertex shader
in vec2 v2f_texcoord0;

// Samplers
uniform sampler2D textureSampler0;
// External constants
uniform vec4 sys_DiffuseColor;

void main ( void )  
{
	gl_FragColor = vec4( 0,0,0, sys_DiffuseColor.a * texture(textureSampler0,v2f_texcoord0).a );
}