// Inputs from vertex shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying float v2f_fogdensity;

// Samplers
uniform sampler2D textureDiffuse;

// System Inputs
uniform vec4 sys_LightAmbient;
uniform vec4 sys_LightColor[8];
uniform vec4 sys_LightPosition[8];
uniform vec4 sys_LightProperties[8];

float diffuseLighting ( vec3 normal, vec3 lightDist, float lightRange, float lightFalloff, float lightPass )
{
	// Distance-based attenuation
	float attenuation = pow( max( 1 - (length( lightDist )*lightRange), 0.0 ), lightFalloff );
	
	// Cosine law * attenuation
	float color = mix( max( dot( normal,normalize( lightDist ) ), 0.0 ), 1.0, lightPass*(1+attenuation) ) * attenuation;
	
	// Return final color
	return color;
}

void main ( void )  
{
	vec4 diffuseColor = texture2D( textureDiffuse, v2f_texcoord0 );
	diffuseColor.rgb = (diffuseColor.rgb+2)/3;
	vec3 lightColor = vec3 ( 0.0, 0.0, 0.0 );
	
	lightColor += sys_LightAmbient.rgb * v2f_colors.rgb;
	for ( int i = 0; i < 8; i += 1 )
	{
		vec3 lightDir;
		lightDir = sys_LightPosition[i].xyz - v2f_position.xyz;
		
		float lightVal1 = diffuseLighting( v2f_normals.xyz, lightDir, sys_LightProperties[i].x, sys_LightProperties[i].y, sys_LightProperties[i].z );
		float lightVal2 = max( dot( sys_LightPosition[i].xyz, v2f_normals.xyz ) + 0.2, 0.0 );
		//float templight = dot( sys_LightPosition[i].xyz, v2f_normals.xyz )+1;
		//float lightVal2 = max( (templight*templight*templight)/8.0, 0.0 );
		if ( lightVal2 > 0.2 )
			lightVal2 = lightVal2 * 2 - 0.2;
		
		lightColor += sys_LightColor[i].rgb * mix( lightVal2 * v2f_colors.r, lightVal1, sys_LightPosition[i].w );
	}
	
	gl_FragColor = mix( gl_Fog.color, diffuseColor * v2f_colors * vec4( lightColor, 1.0 ), max(v2f_fogdensity,0.5) );
	//gl_FragColor = diffuseColor * v2f_colors * vec4( lightColor, 1.0 );
	gl_FragColor.a = diffuseColor.a * v2f_colors.a;
}