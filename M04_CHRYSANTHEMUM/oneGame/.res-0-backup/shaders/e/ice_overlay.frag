// Inputs from vertex shader
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;

// Samplers
uniform sampler2D textureSampler0;

// Inputs
uniform vec4 sys_DiffuseColor;

uniform float gm_icingAmount;

void main ( void )  
{
	vec4 diffuseColor = texture2D( textureSampler0, v2f_texcoord0 );
	
	float alpha = min(1.3, max(sqrt(diffuseColor.a)-(1.0-gm_icingAmount),0.0)*8.0 );
	
	gl_FragColor = diffuseColor;
	gl_FragColor.a = min(1.0, diffuseColor.a * v2f_colors.a * sys_DiffuseColor.a * min(1.0,alpha*max(0.0,gm_icingAmount)) + max(0.0,alpha-1.0) );
}