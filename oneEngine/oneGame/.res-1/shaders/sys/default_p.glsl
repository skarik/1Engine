// sys/default
// Default forward rendering shader.
#version 430

// Inputs from vertex shader
layout(location = 0) in vec4 v2f_colors;
layout(location = 1) in vec4 v2f_position;
layout(location = 2) in vec2 v2f_texcoord0;
layout(location = 3) in float v2f_fogdensity;

// Samplers
layout(binding = 0, location = 20) uniform sampler2D textureDiffuse;

// Outputs
layout(location = 0) out vec4 FragDiffuse;

void main ( void )
{
	vec4 diffuseColor = texture( textureDiffuse, v2f_texcoord0 );

	FragDiffuse = diffuseColor * v2f_colors;
	FragDiffuse.a = diffuseColor.a * v2f_colors.a;
}
