// Inputs from vertex shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec3 v2f_emissive;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying float v2f_fogdensity;

// Samplers
uniform sampler2D textureSampler0;

// Constants
uniform float sys_AlphaCutoff;

void main ( void )  
{
	vec4 diffuseColor = texture2D( textureSampler0, v2f_texcoord0 );
	float f_alpha = diffuseColor.a * v2f_colors.a;
	if ( f_alpha < sys_AlphaCutoff ) discard;
	
	gl_FragColor.rgb = mix( gl_Fog.color.rgb, diffuseColor.rgb * (v2f_colors.rgb+v2f_emissive), v2f_fogdensity );
	gl_FragColor.a = f_alpha;
}