varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying float v2f_fogdensity;
varying vec3 v2f_screenpos;
varying vec4 v2f_lightcoord[8];

uniform sampler2D textureDiffuse;
uniform sampler2D textureShadow[8];
uniform sampler2D textureShadow0;
uniform sampler2D textureShadow1;
uniform sampler2D textureShadow2;

uniform vec4 sys_LightAmbient;
uniform vec4 sys_LightColor[8];
uniform vec4 sys_LightPosition[8];
uniform vec4 sys_LightProperties[8];
uniform vec4 sys_LightShadowInfo[8];

uniform vec3 sys_WorldCameraPos;

uniform vec4 terra_BaseAmbient;

float diffuseLighting ( vec3 normal, vec3 lightDist, float lightRange, float lightFalloff, float lightPass )
{
	// Distance-based attenuation
	float attenuation = pow( max( 1.0 - (length( lightDist )*lightRange), 0.0 ), lightFalloff );
	
	// Cosine law * attenuation
	float color = mix( max( dot( normal,normalize( lightDist ) ), 0.0 ), 1.0, lightPass*(1+attenuation) ) * attenuation;
	
	// Return final color
	return color;
}
/*float specularLighting( vec3 normal, vec3 lightdir, vec3 viewdir, float specular_pow )
{
	// Create the reflection vector
	vec3 reflectdir = 2*dot(normal,lightdir)*normalize(normal) - normalize(lightdir);
	// Attenuation is based on camera position and reflection dir
	float attenuation = max( dot( normalize(reflectdir), normalize(viewdir) ), 0.0 );
	
	return pow( attenuation, specular_pow );
}*/

float luminosity;
vec3 defaultLighting ( vec4 lightPosition, vec4 lightProperties, vec4 lightColor, vec3 vertDir )
{
	vec3 diffuseColor = vec3( 0,0,0 );
	
		vec3 lightDir;
		lightDir = lightPosition.xyz - v2f_position.xyz;
		
		float lightVal1 = diffuseLighting( v2f_normals.xyz, lightDir, lightProperties.x, lightProperties.y, lightProperties.z );
		float lightVal2 = max( dot( lightPosition.xyz, v2f_normals.xyz ), 0.0 );
		
		diffuseColor += lightColor.rgb * mix( lightVal2, lightVal1, lightPosition.w );

		// Specular lighting
		//float specLight1 = specularLighting( v2f_normals.xyz, lightDir, vertDir, 32 );
		//float specLight2 = specularLighting( v2f_normals.xyz, lightPosition.xyz, vertDir, 32 ); 
		//diffuseColor += lightColor.rgb * mix( specLight2, specLight1, lightPosition.w ) * 1.4 * (1-luminosity) * v2f_colors.g * 2;
	
	return diffuseColor;
}

void main ( void )  
{
	vec4 diffuseColor = v2f_colors * 2.2;
	//vec4 diffuseColor = v2f_colors;
	vec3 lightColor = vec3 ( 0.0, 0.0, 0.0 );
	vec3 vertDir = sys_WorldCameraPos-v2f_position.xyz;
	vec3 ambientAmount = terra_BaseAmbient.rgb;
	lightColor += sys_LightAmbient.rgb * ambientAmount;
	lightColor += defaultLighting ( sys_LightPosition[0], sys_LightProperties[0], sys_LightColor[0], vertDir ) * ambientAmount;
	lightColor += defaultLighting ( sys_LightPosition[1], sys_LightProperties[1], sys_LightColor[1], vertDir ) * ambientAmount;
	lightColor += defaultLighting ( sys_LightPosition[2], sys_LightProperties[2], sys_LightColor[2], vertDir ) * ambientAmount;
	//lightColor += defaultLighting ( sys_LightPosition[3], sys_LightProperties[3], sys_LightColor[3], vertDir );
	//lightColor += defaultLighting ( sys_LightPosition[4], sys_LightProperties[4], sys_LightColor[4], vertDir );
	//lightColor += defaultLighting ( sys_LightPosition[5], sys_LightProperties[5], sys_LightColor[5], vertDir );
	//lightColor += defaultLighting ( sys_LightPosition[6], sys_LightProperties[6], sys_LightColor[6], vertDir );
	//lightColor += defaultLighting ( sys_LightPosition[7], sys_LightProperties[7], sys_LightColor[7], vertDir );
	
	gl_FragColor = mix( gl_Fog.color, diffuseColor * vec4( lightColor, 1.0 ), v2f_fogdensity*0.5 );
	//gl_FragColor = diffuseColor * v2f_fogdensity;
	//float fog = min(1, length(vertDir)/2048.0 )*0.9;
	//gl_FragColor = diffuseColor * (1-fog*fog*fog);
	gl_FragColor.a = v2f_colors.a;
}