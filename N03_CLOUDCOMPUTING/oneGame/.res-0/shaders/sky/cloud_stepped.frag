#version 140

varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec2 v2f_texcoord0;
varying vec4 v2f_position;
varying float v2f_fogdensity;

// Samplers
uniform sampler2D textureSampler0;

// Sys
uniform vec4 sys_Time;
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

// Game vars
uniform float gm_Datettime;
uniform float gm_Stormvalue;

// Fog
layout(std140) uniform sys_Fog
{
	vec4	sys_FogColor;
	vec4	sys_AtmoColor;
	float 	sys_FogEnd;
	float 	sys_FogScale;
};

vec2 getplane ( float height )
{
	vec3 plane;
	vec3 real_offset = v2f_position.xyz - sys_WorldCameraPos;
	vec3 offset = normalize(real_offset);
	plane.z = height;

	float time = (plane.z - sys_WorldCameraPos.z) / offset.z;
	plane.x = offset.x * time;
	plane.y = offset.y * time;
	//plane.z = sign(time);
	if ( time < 0.0 ) {
		discard;
	}

	plane.xy += sys_WorldCameraPos.xy;
	if ( length( plane ) > 8000 )
		discard;

	return plane.xy;
}
/*
float diffuseLighting ( vec3 normal, vec3 lightDist, float lightRange, float lightFalloff, float lightPass )
{
	// Distance-based attenuation
	float attenuation = pow( max( 1 - (length( lightDist )*lightRange), 0.0 ), lightFalloff );
	// Cosine law * attenuation
	float color = mix( max( dot( normal,normalize( lightDist ) ), 0.0 ), 1.0, lightPass*(1+attenuation) ) * attenuation;
	// Return final color
	return color;
}*/

void main ( void )
{
	// Get the plane texture coordinates
	vec2 plane0 = getplane( 300 );
	vec2 plane1 = getplane( 400 );

	// Generate the cloud shapes
	float cloudDensity = 0;
	vec4 cloudSamplerMain, cloudSamplerMain_Off;
	cloudSamplerMain		= texture( textureSampler0, plane0*0.0006 + vec2( sys_Time.x*0.10+gm_Datettime*0.30,	sys_Time.x*0.06+gm_Datettime*0.34 ) );
	cloudSamplerMain_Off	= texture( textureSampler0, plane1*0.0008 + vec2( sys_Time.x*0.05-gm_Datettime*0.27,	sys_Time.x*0.07-gm_Datettime*0.37 ) );
	cloudDensity = (cloudSamplerMain.g + cloudSamplerMain_Off.g)*0.5;

	// Get the distance and calculate the fog amount
	float pixelDist = length(vec3(plane0.x,plane0.y,320) - sys_WorldCameraPos) / 12.0;
	float fogdensity  = min( 1, max(0, (sys_FogEnd - pixelDist) * sys_FogScale ) );	// alpha fog
	float fogdensity2 = min( 1, max(0, (sys_FogEnd - pixelDist*3) * sys_FogScale ) );	// diffuse fog

	// Reduce cloud density based on fog
	cloudDensity *= fogdensity;

	float finalAlpha = 1;
	if ( cloudDensity < (1-gm_Stormvalue)/2 ) {
		discard;
	}
	// Set the color
	vec4 diffuseColor = vec4(1,1,1,1);

	// Generate lighting
	vec4 lightColor;
	lightColor = sys_LightAmbient;
	lightColor.rgb += sys_LightColor[0].rgb;

	lightColor.rgb = min( lightColor.rgb, vec3(1,1,1) );

	// Blend color according to the fog
	//diffuseColor = mix( sys_FogColor, diffuseColor, max( fogdensity2-0.0, 0.0 ) / 1.0 );
	/*fogdensity2 = max( fogdensity2 - 0.5, 0.0 ) / 0.5;
	fogdensity2 = (min( fogdensity2 + 0.2, 1.0 ) - 0.2) / 0.8;*/
	fogdensity2 = clamp( ((fogdensity2 - 0.5) * 5) + fogdensity2, 0.0, 1.0 );
	diffuseColor = mix( sys_FogColor, diffuseColor*lightColor, fogdensity2 );

	// Set final alpha
	diffuseColor.a = finalAlpha;
	gl_FragColor = diffuseColor;

	/*cloudDensity = getdensity();

	// ==Perform the final color math==
	// Do fog
	float pixelDist = length(vec3(plane.x,plane.y,320) - sys_WorldCameraPos) / 12.0;
	float fogdensity  = min( 1, max(0, (sys_FogEnd - pixelDist) * sys_FogScale ) );	// alpha fog
	float fogdensity2 = min( 1, max(0, (sys_FogEnd - pixelDist*3) * sys_FogScale ) );	// diffuse fog

	// Create base alpha for clouds
	vec4 diffuseColor;
	//float baseAlpha = mix( 0, 1, max(fogdensity,0) );
	float baseAlpha = fogdensity;
	float finalAlpha = min( 1, cloudDensity ) * baseAlpha;

	// Do colors
	float rDensity = (1-cloudDensity)*0.05 + 0.95;
	diffuseColor = vec4( rDensity,rDensity,rDensity,1 );

	//vec4 cloudColor =
	//	  texture2D( textureDiffuse, plane*0.0007 + vec2( sys_Time.x*0.1+gm_Datettime*0.3, sys_Time.x*0.06+gm_Datettime*0.34 ) )
	//	+ texture2D( textureDiffuse, plane*0.0008 + vec2( sys_Time.x*0.05-gm_Datettime*0.27, sys_Time.x*0.07-gm_Datettime*0.37 ) ).gbra;
	vec4 cloudColor = texture2D( textureSampler0, plane*0.0007 + vec2( sys_Time.x*0.1+gm_Datettime*0.3, sys_Time.x*0.06+gm_Datettime*0.34 ) );
	vec4 cloudColor_Off = texture2D( textureSampler0, plane*0.0008 + vec2( sys_Time.x*0.05-gm_Datettime*0.27, sys_Time.x*0.07-gm_Datettime*0.37 ) );
	cloudColor.r += cloudColor_Off.g;
	cloudColor.g += cloudColor_Off.b;
	cloudColor.b += cloudColor_Off.r;
	diffuseColor.rgb *= ( 1 - (cloudColor.g * 0.13 * gm_Stormvalue)) ;

	vec4 lightColor;
	lightColor = sys_LightAmbient;
	for ( int i = 0; i < 4; i += 1 )
	{
		vec3 lightDir;
		lightDir = sys_LightPosition[i].xyz - vec3( plane.x,plane.y,310 );

		float lightVal1 = diffuseLighting( v2f_normals.xyz, lightDir, sys_LightProperties[i].x, sys_LightProperties[i].y, sys_LightProperties[i].z );
		float lightVal2 = max( dot( sys_LightPosition[i].xyz, v2f_normals.xyz ) + 0.2, 0.0 );
		if ( lightVal2 > 0.2 )
			lightVal2 = lightVal2 * 2 - 0.2;

		lightColor.rgb += sys_LightColor[i].rgb * mix( lightVal2, lightVal1, sys_LightPosition[i].w );
	}

	lightColor += (lightColor-0.3)*2;
	float luminance = lightColor.r * 0.299 + lightColor.g * 0.587 + lightColor.b * 0.114;
	lightColor = (lightColor + vec4( luminance,luminance,luminance,luminance ) + vec4(1,1,1,1))*0.333;
	lightColor = min( vec4(1,1,1,1), lightColor );

	diffuseColor = mix( sys_FogColor, diffuseColor*lightColor, fogdensity2 );

	// Set final alpha
	diffuseColor.a = finalAlpha;
	gl_FragColor = diffuseColor;*/
}
