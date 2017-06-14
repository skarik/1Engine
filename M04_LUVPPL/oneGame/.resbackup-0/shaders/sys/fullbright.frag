// sys/fullbright
// Renders object without lighting, only diffuse multiply blending.
#version 330

// Inputs from vertex shader
in vec4 v2f_colors;
in vec2 v2f_texcoord0;

// Samplers
uniform sampler2D textureSampler0;

// Game Inputs
uniform float	gm_FadeValue;
uniform vec4 sys_DiffuseColor;

void main ( void )  
{
	vec4 diffuseColor = texture2D( textureSampler0, v2f_texcoord0 );
	
	gl_FragColor = diffuseColor * v2f_colors;
	gl_FragColor.a = diffuseColor.a * v2f_colors.a;
}