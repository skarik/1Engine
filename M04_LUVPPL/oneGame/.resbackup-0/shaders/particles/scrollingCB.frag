// Inputs from vertex shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec3 v2f_emissive;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying float v2f_fogdensity;

// Samplers
uniform sampler2D textureSampler0;
uniform sampler2D textureSampler1;

// Constants
uniform float sys_AlphaCutoff;
uniform vec4 sys_Time;

void main ( void )  
{
	vec4 diffuseColor = texture2D( textureSampler0, v2f_texcoord0 );
	vec4 diffuseColor2 = texture2D( textureSampler1, v2f_texcoord0*5 + vec2( sys_Time.x,sys_Time.y )*0.5 );
	float f_alpha = diffuseColor.a * v2f_colors.a;
	if ( f_alpha < sys_AlphaCutoff ) discard;
	/*diffuseColor.a += diffuseColor2.g*min( 0, (v2f_position.z-24)*0.05 );
	if ( diffuseColor.a < 0 ) {
		discard;
	}
	diffuseColor.rgb *= min( 1, max( diffuseColor2.g+(v2f_position.z-15)*0.05, 0 ) );
	diffuseColor.rgb *= (diffuseColor2.g+2)/3;*/
	diffuseColor.rgb *= (diffuseColor2.g+1)/2;
	
	gl_FragColor.rgb = mix( gl_Fog.color.rgb, diffuseColor.rgb * (v2f_colors.rgb+v2f_emissive), v2f_fogdensity );
	gl_FragColor.a = f_alpha;
}