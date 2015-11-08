// v2d/default
// Default shader for 2D GUI elements.
#version 330

// Inputs from vertex shader
in vec4 v2f_colors;
in vec4 v2f_position;
in vec2 v2f_texcoord0;

// Samplers
uniform sampler2D textureSampler0;

// Inputs
uniform vec4 sys_DiffuseColor;
uniform float sys_AlphaCutoff;

void main ( void )  
{
	vec4 diffuseColor = texture( textureSampler0, v2f_texcoord0 );
	float f_alpha = diffuseColor.a * v2f_colors.a * sys_DiffuseColor.a;
	if ( f_alpha < sys_AlphaCutoff ) discard;
	gl_FragColor = diffuseColor * v2f_colors * sys_DiffuseColor;
	gl_FragColor.a = f_alpha;
}