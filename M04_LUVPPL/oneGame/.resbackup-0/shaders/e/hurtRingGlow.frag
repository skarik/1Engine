// Inputs from vertex shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec3 v2f_emissive;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying float v2f_fogdensity;

// System input
uniform vec3 sys_WorldCameraPos;

// Samplers
uniform sampler2D textureSampler0;

// Constants
uniform float sys_AlphaCutoff;

void main ( void )  
{
	vec4 diffuseColor = texture2D( textureSampler0, v2f_texcoord0 );
	float f_alpha = diffuseColor.a * v2f_colors.a;
	if ( f_alpha < sys_AlphaCutoff ) discard;
	
	// Camera direction for rim lighting
	vec3 vertDir;
	vertDir = sys_WorldCameraPos-v2f_position.xyz;
	float t_rimlighting = max( 1.0 - dot( v2f_normals.xyz, normalize(vertDir) ),0.0);
	
	// Add rim lighting
	diffuseColor.rgb += vec3( 1.0,1.0,1.0 ) * pow( t_rimlighting, 3.0 ) * 0.1;//max( diffuseColor.rgb, vec3(1,1,1) * min( t_rimlighting * f_alpha * 16, 1 ) );
	f_alpha += t_rimlighting;
	
	gl_FragColor.rgb = mix( gl_Fog.color.rgb, diffuseColor.rgb * (v2f_colors.rgb+v2f_emissive), v2f_fogdensity );
	gl_FragColor.a = f_alpha;
}