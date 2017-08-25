#version 140

// Inputs from vertex shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec3 v2f_texcoord0;
varying float v2f_fogdensity;
varying vec2 v2f_weatherdensity;
varying vec4 v2f_localposition;

// Samplers
uniform sampler3D textureSampler1;

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

float cellShade ( float lumin )
{
	const int levels = 3;
	float t = lumin*levels;
	float dif = 0.5 - mod(t+0.5,1);
	t += dif * min(1,(0.5-abs(dif))*32) * 0.5;
	
	lumin = t/levels;
	return lumin;
}

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
	vec4 diffuseColor = texture( textureSampler1, v2f_texcoord0 );
	vec3 lightColor = vec3 ( 0.0, 0.0, 0.0 );
	
	diffuseColor += ((vec4(1,1,1,1)-diffuseColor) / max(1,v2f_localposition.z))*v2f_weatherdensity.x;
	//diffuseColor += (vec4(1,1,1,1)-diffuseColor)*v2f_weatherdensity.x;
	
	lightColor += sys_LightAmbient.rgb * v2f_colors.rgb;
	for ( int i = 0; i < 8; i += 1 )
	{
		vec3 lightDir;
		lightDir = sys_LightPosition[i].xyz - v2f_position.xyz;
		
		float lightVal1 = diffuseLighting( v2f_normals.xyz, lightDir, sys_LightProperties[i].x, sys_LightProperties[i].y, sys_LightProperties[i].z );
		float lightVal2 = max( dot( sys_LightPosition[i].xyz, v2f_normals.xyz ), -0.3 );
		float lightValf = cellShade( mix( lightVal2 * v2f_colors.r, lightVal1, sys_LightPosition[i].w ) );
		
		lightColor += sys_LightColor[i].rgb * lightValf;
	}
	
	gl_FragColor = mix( sys_FogColor, diffuseColor * v2f_colors * vec4( lightColor, 1.0 ), v2f_fogdensity );
	gl_FragColor.a = diffuseColor.a * v2f_colors.a;
}