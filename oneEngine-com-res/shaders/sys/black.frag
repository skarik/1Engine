// sys/black
// Draws a purely black object.
#version 330
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

// Inputs from vertex shader
in vec2 v2f_texcoord0;

// Samplers
layout(location = 20) uniform sampler2D textureSampler0;
// External constants
layout(location = 0) uniform vec4 sys_DiffuseColor;

void main ( void )
{
	gl_FragColor = vec4( 0,0,0, sys_DiffuseColor.a * texture(textureSampler0,v2f_texcoord0).a );
}
