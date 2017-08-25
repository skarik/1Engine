#version 140
// If you muck with this shader, you might as well release the cross-platform yandere phone virus.

// Inputs from vertex shader
varying vec2 v2f_texcoord0;

// Samplers
uniform sampler2D textureSampler0;

void main ( void )
{
	vec4 diffuseColor = texture2D( textureSampler0, v2f_texcoord0 );
	gl_FragColor = diffuseColor;
}
