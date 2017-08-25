#version 140

// Inputs from vertex shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying float v2f_fogdensity;

// Samplers
uniform sampler2D textureDiffuse;
uniform sampler2D textureSampler1; // Glowmap

// System Inputs
uniform vec4 sys_DiffuseColor;
uniform vec3 sys_EmissiveColor;

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

// Fog
layout(std140) uniform sys_Fog
{
	vec4	sys_FogColor;
	float 	sys_FogEnd;
	float 	sys_FogScale;
};

uniform vec3 sys_WorldCameraPos;

float diffuseLighting ( vec3 normal, vec3 lightDist, float lightRange, float lightFalloff, float lightPass )
{
	// Distance-based attenuation
	float attenuation = pow( max( 1.0 - (length( lightDist )*lightRange), 0.0 ), lightFalloff );
	
	// Cosine law * attenuation
	float color = mix( max( dot( normal,normalize( lightDist ) ), 0.0 ), 1.0, lightPass*(1.0+attenuation) ) * attenuation;
	
	// Return final color
	return color;
}

void main ( void )  
{
	//vec4 diffuseColor = texture2D( textureDiffuse, v2f_texcoord0 );
	vec3 vertDir;
	vertDir = sys_WorldCameraPos-v2f_position.xyz;
	vec4 diffuseColor = vec4( 1,1,1,1 ) * 1-pow(max(1-dot( v2f_normals.xyz, normalize(vertDir) ),0.0), 1);
	vec3 lightColor = sys_EmissiveColor;	// gl_SecondaryColor is set as emissive when useColors = false for materials
	//lightColor += texture2D( textureSampler1, v2f_texcoord0 ).rgb;
	
	lightColor += sys_LightAmbient.rgb;
	for ( int i = 0; i < 8; i += 1 )
	{
		vec3 lightDir;
		lightDir = sys_LightPosition[i].xyz - v2f_position.xyz;
		
		float lightVal1 = diffuseLighting( v2f_normals.xyz, lightDir, sys_LightProperties[i].x, sys_LightProperties[i].y, sys_LightProperties[i].z );
		float lightVal2 = max( dot( sys_LightPosition[i].xyz, v2f_normals.xyz ), 0.0 );
		
		lightColor += sys_LightColor[i].rgb * mix( lightVal2, lightVal1, sys_LightPosition[i].w );
	}
	
	gl_FragColor = mix( sys_FogColor, diffuseColor * sys_DiffuseColor * vec4( lightColor, 1.0 ), v2f_fogdensity );
	//gl_FragColor.rgb = v2f_normals.xyz;
	gl_FragColor.a = diffuseColor.a * sys_DiffuseColor.a;
}