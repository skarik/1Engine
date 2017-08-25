// Inputs from vertex shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying float v2f_fogdensity;

// Samplers
uniform sampler2D textureDiffuse;

// System Inputs
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
	vec3 lightColor = vec3 ( 0.0, 0.0, 0.0 );
	
	for ( int i = 0; i < 8; i += 1 )
	{
		lightColor += gl_LightSource[i].ambient.rgb;
		
		vec3 lightDir;
		lightDir = gl_LightSource[i].position.xyz - v2f_position.xyz;
		
		float lightVal1 = diffuseLighting( v2f_normals.xyz, lightDir, sys_LightProperties[i].x, sys_LightProperties[i].y, sys_LightProperties[i].z );
		float lightVal2 = max( dot( gl_LightSource[i].position.xyz, v2f_normals.xyz ), 0.0 );
		
		lightColor += gl_LightSource[i].diffuse.rgb * mix( lightVal2, lightVal1, gl_LightSource[i].position.w );
		
	}
	
	gl_FragColor = mix( gl_Fog.color, diffuseColor * v2f_colors * vec4( lightColor, 1.0 ), v2f_fogdensity );
	gl_FragColor.a = diffuseColor.a * v2f_colors.a;
	//gl_FragColor = v2f_colors;
	//gl_FragColor.a = 1.0;
}