// sys/fullbright
// Renders object without lighting, only diffuse multiply blending.
#version 330
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

// Inputs from vertex shader
in vec4 v2f_colors;
in vec2 v2f_texcoord0;

// Samplers
layout(location = 20) uniform sampler2D textureSampler0;

// Game Inputs
layout(location = 8) uniform float	gm_FadeValue;
layout(location = 0) uniform vec4 sys_DiffuseColor;

void main ( void )
{
	vec4 diffuseColor = texture( textureSampler0, v2f_texcoord0 );

	gl_FragColor = diffuseColor * v2f_colors;
	gl_FragColor.a = diffuseColor.a * v2f_colors.a;
}
