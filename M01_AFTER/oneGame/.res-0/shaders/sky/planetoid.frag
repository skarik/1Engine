#version 140

// Inputs from vertex shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying float v2f_fogdensity;

varying vec3 v2f_screenpos;
varying vec3 v2f_screennormal;

// Samplers
uniform sampler2D textureSampler0;
uniform sampler2D textureSampler1; // Cloudmap

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
uniform vec4 sys_ClearColor;

uniform vec3 sys_WorldCameraPos;

// Game Inputs
uniform float	gm_FadeValue;
uniform vec4 sys_Time;

// Fog
layout(std140) uniform sys_Fog
{
	vec4	sys_FogColor;
	float 	sys_FogEnd;
	float 	sys_FogScale;
};

float cellShade ( float lumin )
{
	const int levels = 2;
	float t = lumin*levels;
	float dif = 0.5 - mod(t+0.5,1);
	t += dif * min(1,(0.5-abs(dif))*32) * 0.5;
	
	lumin = t/levels;
	return lumin;
}

float diffuseLighting ( vec3 normal, vec3 lightDist, float lightRange, float lightFalloff, float lightPass, float mixthrough )
{
	// Distance-based attenuation
	float attenuation = pow( max( 1.0 - (length( lightDist )*lightRange), 0.0 ), lightFalloff );
	
	//Cosine law * attenuation
	float normalAttenuate = dot( normal,normalize( lightDist ) );
	normalAttenuate = (max( normalAttenuate, -mixthrough ) + mixthrough)/(1+mixthrough);
	// hard step passthrough
	float color = mix( normalAttenuate, 1.0, lightPass*(1.0+attenuation) ) * attenuation;
	
	// Return final color
	return (color);
}

float specularLighting( vec3 normal, vec3 lightdir, vec3 viewdir, float specular_pow, float lightRange )
{ 
	// Create the reflection vector
	vec3 reflectdir = 2*dot(normal,lightdir)*normalize(normal) - normalize(lightdir);
	// Attenuation is based on camera position and reflection dir
	float attenuation = max( dot( normalize(reflectdir), normalize(viewdir) ), 0.0 );
	// Also add distance-based attenuation though
	//float distance_attenuation = max( 1.0 - (length( lightdir )*lightRange*0.4), 0.0 );
	
	return pow( attenuation, specular_pow );
	//return pow( attenuation*distance_attenuation, specular_pow+((1.0-distance_attenuation)*2.0) );
}

void main ( void )  
{
	
	vec3 lightColor = sys_EmissiveColor;
	
	vec3 lightDir, vertDir;
	vertDir = sys_WorldCameraPos-v2f_position.xyz;
	//float rimValue = pow(max(1-dot( v2f_normals.xyz, normalize(vertDir) ),0.0), 1);
	float rimValue = max(1-dot( v2f_normals.xyz, normalize(vertDir) ),0.0);

	// Based on the normal and camera face diret
	vec2 calcScreennormal;
	calcScreennormal = v2f_screennormal.xy;
	
	vec2 calcTexcoord = vec2( 0.5 + sys_Time.x, 0.5 );
	float dist = 1.9-sqrt(1-(calcScreennormal.x*calcScreennormal.x + calcScreennormal.y*calcScreennormal.y));
	//calcTexcoord += calcScreennormal*0.5*(1-clamp((v2f_screennormal.z-0.707)*1.4,0,1));
	calcTexcoord += calcScreennormal*0.5*dist;
	
	vec2 calcSkytexcoord1 = calcTexcoord*1.1 + vec2( sys_Time.x*0.5 + 0.7*(0.5-calcTexcoord.y), -sys_Time.x*0.1 );
	vec2 calcSkytexcoord2 =-calcTexcoord*0.7 - vec2( sys_Time.x*1.2 - 0.2*(0.5-calcTexcoord.y), +sys_Time.x*0.3 );

	vec4 diffuseColor = texture2D( textureSampler0, calcTexcoord );
	vec4 diffuseColor2 = texture2D( textureSampler1, calcTexcoord );
	vec4 cloudColor1 = texture2D( textureSampler1, calcSkytexcoord1 );
	vec4 cloudColor2 = texture2D( textureSampler1, calcSkytexcoord2 );
	
	if ( diffuseColor2.r < 0.4 ) {
		diffuseColor2.rgb = (diffuseColor2.r+0.6) * vec3( 0.4,0.5,1 );
	}
	else {
		diffuseColor2.rgb = diffuseColor2.r * vec3( 1,1,0.5 );
	}
	diffuseColor2.rgb += vec3(0.1,0.1,0.1);
	diffuseColor = mix( diffuseColor, diffuseColor2, 0.99 );
	
	float mCloudDensity = clamp( ((cloudColor1.r+cloudColor2.r)-0.6)*2, 0, 1 );
	vec3 diffuseClouds = vec3(1,1,1)*(cloudColor2.g+1)*0.5;
	//diffuseColor.rgb = mix( diffuseColor, , mCloudDensity );
	
	// ambient light should always be normalized to length 0.1
	vec3 normalizeAmbient = normalize( sys_LightAmbient.rgb ) * 0.1;
	lightColor += normalizeAmbient; // Add this brighter ambient light
	
	float mixthrough = 0.3;
	
	float lightVal1, lightVal2;
	for ( int i = 1; i < 2; i += 1 )
	{
		lightDir = sys_LightPosition[i].xyz - v2f_position.xyz;
		
		lightVal1 = diffuseLighting(
			v2f_normals.xyz, lightDir,
			sys_LightProperties[i].x, sys_LightProperties[i].y, sys_LightProperties[i].z,
			mixthrough ) * 0.01;
		lightVal2 = dot( sys_LightPosition[i].xyz, v2f_normals.xyz )+1;
		
		float specVal2 = specularLighting( v2f_normals.xyz, sys_LightPosition[i].xyz, vertDir, 1, 0 );
		
		// Now for some magic
		lightVal2 = min( max( (lightVal2-0.4)*4, 0 ), 1 );
		lightVal2 = cellShade( lightVal2 ) * 0.9;

		lightColor += sys_LightColor[i].rgb * mix( lightVal2, lightVal1, sys_LightPosition[i].w ) * 2;
	}
	
	lightColor += (rimValue+pow(rimValue,6)*1.2)*(lightVal2+0.5)*0.7;
	
	lightColor += vec3( 0.1,0.1,0.2 );
	
	diffuseClouds *= (lightColor*0.5+vec3(1,1,1))*0.4; // Multiply clouds by light for a shellish effect
	
	diffuseColor.rgb = mix( diffuseColor.rgb, vec3(lightColor)*diffuseColor.rgb, rimValue );
	diffuseColor.rgb = mix( diffuseColor.rgb, diffuseClouds, mCloudDensity );
	
	gl_FragColor = mix( (sys_FogColor+sys_ClearColor)*0.5, diffuseColor * sys_DiffuseColor * vec4( lightColor, 1.0 ), 0.9 );
	
	//gl_FragColor.rgb = mix( gl_FragColor.rgb, v2f_screennormal, 0.99 );
	
	gl_FragColor.a = 1.0;// diffuseColor.a * sys_DiffuseColor.a;
}