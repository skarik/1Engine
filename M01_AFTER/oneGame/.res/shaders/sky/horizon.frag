#version 140

// Inputs from vertex shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying float v2f_fogdensity;

// Samplers
uniform sampler2D textureSampler0; // Diffuse map

// System Inputs
uniform vec4 sys_DiffuseColor;
uniform vec3 sys_EmissiveColor;

uniform vec3 sys_WorldCameraPos;

// Lighting and Shadows
uniform vec4 sys_LightAmbient;
layout(std140) uniform sys_LightingInfo
{
	vec4 sys_LightColor[8];
	vec4 sys_LightPosition[8];
	vec4 sys_LightProperties[8];
	vec4 sys_LightShadowInfo[8];
	mat4 sys_LightMatrix[8];
};

// Game Inputs
uniform float	gm_FadeValue;

// Fog
layout(std140) uniform sys_Fog
{
	vec4	sys_FogColor;
	float 	sys_FogEnd;
	float 	sys_FogScale;
};

// 0 for normal lighting, 1 for full wraparound lighting
const float mixthrough = 0.2;

float diffuseLighting ( vec3 normal, vec3 lightDist, float lightRange, float lightFalloff, float lightPass )
{
	// Distance-based attenuation
	float attenuation = pow( max( 1.0 - (length( lightDist )*lightRange), 0.0 ), lightFalloff );
	
	// Cosine law * attenuation
	//float color = mix( dot( normal,normalize( lightDist ) ), 1.0, lightPass*(1.0+attenuation) ) * attenuation;
	// Cosine law breaking ( cuz objects need more detail )
	float normalAttenuate = dot( normal,normalize( lightDist ) );
	normalAttenuate = (max( normalAttenuate, -mixthrough ) + mixthrough)/(1+mixthrough);
	// Attenuation
	float color = mix( normalAttenuate, 1.0, lightPass*(1.0+attenuation) ) * attenuation;
	
	// Return final color
	return color;
}
float cellShade ( float lumin )
{
	const int levels = 2;
	float t = lumin*levels;
	float dif = 0.5 - mod(t+0.5,1);
	t += dif * min(1,(0.5-abs(dif))*32) * 0.5;
	
	lumin = t/levels;
	/*const float minval = 0.3;
	if ( lumin < -minval ) {
		lumin = -minval;
	}
	if ( lumin < minval ) {
		lumin = minval*((lumin+minval)/(minval*2));
	}*/
	return lumin;
}

vec3 defaultLighting ( vec4 lightPosition, vec4 lightProperties, vec4 lightColor, vec3 vertDir )
{
	vec3 resultColor;
	// Get direction to light
	vec3 lightDir;
	lightDir = lightPosition.xyz - v2f_position.xyz;
	// Perform base lighting
	float lightVal1 = diffuseLighting( v2f_normals.xyz, lightDir, lightProperties.x, lightProperties.y, lightProperties.z );
	float lightVal2 = dot( lightPosition.xyz, v2f_normals.xyz );
	lightVal2 = (max( lightVal2, -mixthrough ) + mixthrough)/(1+mixthrough); // Directional light mixthrough
	// Mix between light styles for the directional/nondirectional
	float lightValf = mix( lightVal2, lightVal1, lightPosition.w );
	lightValf = cellShade(lightValf); // Apply cellshading
		
	resultColor = lightColor.rgb * max( 0, lightValf );
	
	return resultColor;
}

void main ( void )  
{
	// Diffuse color
	vec4 diffuseColor = texture2D( textureSampler0, v2f_texcoord0 );
	
	// Rim effect
	vec3 vertDir;
	vertDir = sys_WorldCameraPos-v2f_position.xyz;
	float lightVal3 = max(1-dot( v2f_normals.xyz, normalize(vertDir) ),0.0);
	
	// Lighting
	vec3 lightColor = sys_EmissiveColor;
	lightColor += sys_LightAmbient.rgb;
	// Fixed-function lighting
	lightColor += defaultLighting ( sys_LightPosition[0], sys_LightProperties[0], sys_LightColor[0], vertDir );
	lightColor += defaultLighting ( sys_LightPosition[1], sys_LightProperties[1], sys_LightColor[1], vertDir );
	// Rim amount
	lightColor += lightColor*clamp(length(lightColor-sys_LightAmbient.rgb)-0.3,0,1)*pow(lightVal3,3);
	
	// Shadow "outline" effect
	//diffuseColor.rgb *= 1-(clamp( (pow( clamp(lightVal3,0,1), 5 )-0.12)/0.1, 0,1 ) * (1-min(1,length(lightColor-sys_LightAmbient.rgb)*4)))*0.3;
	
	gl_FragColor.rgb = mix( sys_FogColor.rgb, diffuseColor.rgb * sys_DiffuseColor.rgb * lightColor, max( 0.0, v2f_fogdensity) );
	gl_FragColor.a = diffuseColor.a * sys_DiffuseColor.a;
}