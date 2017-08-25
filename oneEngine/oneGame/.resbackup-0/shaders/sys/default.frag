// sys/default
// Default forward rendering shader.
#version 330

// Inputs from vertex shader
in vec4 v2f_colors;
in vec4 v2f_position;
in vec2 v2f_texcoord0;
in float v2f_fogdensity;

// Samplers
uniform sampler2D textureDiffuse;

// Game Inputs
uniform float	gm_FadeValue;

void main ( void )  
{
	vec4 diffuseColor = texture( textureDiffuse, v2f_texcoord0 );
	//vec4 diffuseColor = vec4(1,1,1,1);
	
	//gl_FragColor = mix( gl_Fog.color, diffuseColor * v2f_colors, v2f_fogdensity );
	gl_FragColor = diffuseColor * v2f_colors;
	gl_FragColor.a = diffuseColor.a * v2f_colors.a;
}