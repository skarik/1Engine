//#version 140

varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec2 v2f_texcoord0;
varying vec4 v2f_position;
varying float v2f_fogdensity;

// Samplers
uniform sampler2D textureDiffuse;

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

// Global used to save normal calculated from volume sample
vec3 calcNormal;

vec2 getplane ( float height )
{
	vec3 plane;
	vec3 real_offset = v2f_position.xyz - sys_WorldCameraPos;
	vec3 offset = normalize(real_offset);
	plane.z = height;
	if ( offset.z*plane.z < 0.0 ) {
		discard;
	}
	
	float time = (plane.z - sys_WorldCameraPos.z) / offset.z;
	plane.x = offset.x * time;
	plane.y = offset.y * time;
	
	plane.xy += sys_WorldCameraPos.xy;
	if ( length( plane ) > 8000 )
		discard;
	
	return plane.xy;
}

float getdensity ( void )
{
	// Value to be returned
	float cloudDensity = 0.0;
	// Reset normal
	calcNormal.z = 0;

	// Calculate offset
	vec2 planeoffset = vec2( sys_Time.x*0.1+gm_Datettime*0.3, sys_Time.x*0.06+gm_Datettime*0.34 );
	//vec2 layeroffset = vec2( 0,0 );
	vec2 plane, sampleCoord0, sampleCoord1;
	vec4 mainTex;
	
	vec4 warpTex = texture2D( textureDiffuse, (getplane(320.0)*0.001-planeoffset*0.8)*0.02 );
	warpTex.xy = (warpTex.xy-0.5)*0.8;
	
	// Sample cloud volume
	for ( int i = 0; i < 7; i += 1 )
	{
		// Get projected 2D plane coordinate at given height
		plane = getplane( 360.0-(i*11.43) );
		
		// First sample of the clouds
		sampleCoord0 = plane*0.0007 + planeoffset /*+ layeroffset*/ + warpTex.xy;
		mainTex = texture2D( textureDiffuse, sampleCoord0 ).rgba;
		// Offset with the next cloud coordinate
		sampleCoord1 = plane*0.0008 + vec2(planeoffset.y,-planeoffset.x)*0.2 /*+ layeroffset*/ - warpTex.xy*2;
		mainTex += texture2D( textureDiffuse, sampleCoord1 ).rgba;
		// Add this value to the density seen at this pixel
		cloudDensity += mainTex.r;
		
		// Add to z normal
		//calcNormal.z += ((i-3.5)*mainTex.r)/98.0;
	}
	cloudDensity = cloudDensity / 21.0;
	cloudDensity = cloudDensity*cloudDensity*cloudDensity*58 + gm_Stormvalue - 0.2;
	
	// Calculate normal
	const float normalOffsetSample = 0.06;
	vec2 normalOffsetX = vec2(normalOffsetSample,0);
	vec2 normalOffsetY = vec2(0,normalOffsetSample);
	mainTex.r = texture2D( textureDiffuse, sampleCoord0 + normalOffsetX).r
				+ texture2D( textureDiffuse, sampleCoord1 + normalOffsetX ).r;
	mainTex.g = texture2D( textureDiffuse, sampleCoord0 + normalOffsetY ).r
				+ texture2D( textureDiffuse, sampleCoord1 + normalOffsetY ).r;
	mainTex.b = texture2D( textureDiffuse, sampleCoord0 - normalOffsetX ).r
				+ texture2D( textureDiffuse, sampleCoord1 - normalOffsetX ).r;
	mainTex.a = texture2D( textureDiffuse, sampleCoord0 - normalOffsetY ).r
				+ texture2D( textureDiffuse, sampleCoord1 - normalOffsetY ).r;
	
	mainTex *= 0.33;
	mainTex.r = mainTex.r*mainTex.r*mainTex.r*58 + gm_Stormvalue - 0.2;
	mainTex.g = mainTex.g*mainTex.g*mainTex.g*58 + gm_Stormvalue - 0.2;
	mainTex.b = mainTex.b*mainTex.b*mainTex.b*58 + gm_Stormvalue - 0.2;
	mainTex.a = mainTex.a*mainTex.a*mainTex.a*58 + gm_Stormvalue - 0.2;
	//mainTex = max( 0, min( 2, mainTex ));

	mainTex *= 2;
	
	calcNormal.x = mainTex.r-mainTex.b;
	calcNormal.y = mainTex.g-mainTex.a;
	calcNormal.x = -calcNormal.x;
	calcNormal.y = -calcNormal.y;
	calcNormal.z = -1;
	if ( length(calcNormal) > 0 ) {
		calcNormal = normalize(calcNormal);
	}
	
	return max( 0, min( 2, cloudDensity ));
}

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
	// Generate the volume data
	float cloudDensity = 0;
	vec2 plane = getplane( 320 );
	
	cloudDensity = getdensity();
	
	// ==Perform the final color math==
	// Do fog
	float pixelDist = length(vec3(plane.x,plane.y,320) - sys_WorldCameraPos) / 12.0;
	float fogdensity  = min( 1, max(0, (gl_Fog.end - pixelDist) * gl_Fog.scale ) );	// alpha fog
	float fogdensity2 = min( 1, max(0, (gl_Fog.end - pixelDist*4) * gl_Fog.scale ) );	// diffuse fog
	
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
	vec4 cloudColor = texture2D( textureDiffuse, plane*0.0007 + vec2( sys_Time.x*0.1+gm_Datettime*0.3, sys_Time.x*0.06+gm_Datettime*0.34 ) );
	vec4 cloudColor_Off = texture2D( textureDiffuse, plane*0.0008 + vec2( sys_Time.x*0.05-gm_Datettime*0.27, sys_Time.x*0.07-gm_Datettime*0.37 ) );
	cloudColor.r += cloudColor_Off.g;
	cloudColor.g += cloudColor_Off.b;
	cloudColor.b += cloudColor_Off.r;
	diffuseColor.rgb *= ( 1 - (cloudColor.g * 0.13 * gm_Stormvalue)) ;
	
	// Calculate normals
	//float temp = calcNormal.z;
	//calcNormal.z = 0;
	vec3 frag_normals = mix(calcNormal,v2f_normals.xyz,0.3+min(0.7,gm_Stormvalue));
	//frag_normals = v2f_normals.xyz;
	
	vec4 lightColor;
	lightColor = sys_LightAmbient;
	for ( int i = 0; i < 4; i += 1 )
	{
		vec3 lightDir;
		lightDir = sys_LightPosition[i].xyz - vec3( plane.x,plane.y,310 );
		
		float lightVal1 = diffuseLighting( frag_normals, lightDir, sys_LightProperties[i].x, sys_LightProperties[i].y, sys_LightProperties[i].z );
		float lightVal2 = max( dot( sys_LightPosition[i].xyz, frag_normals ) + 0.2, 0.0 );
		if ( lightVal2 > 0.2 )
			lightVal2 = lightVal2 * 2 - 0.2;
		
		lightColor.rgb += sys_LightColor[i].rgb * mix( lightVal2, lightVal1, sys_LightPosition[i].w );
	}
	
	lightColor += (lightColor-0.3)*2;
	float luminance = lightColor.r * 0.299 + lightColor.g * 0.587 + lightColor.b * 0.114;
	lightColor = (lightColor + vec4( luminance,luminance,luminance,luminance ) + vec4(1,1,1,1))*0.333;
	lightColor = min( vec4(1.1,1.1,1.1,1), lightColor );
	
	//diffuseColor = mix( gl_Fog.color, diffuseColor*lightColor, clamp( fogdensity2+min(1.0,length(lightColor)), 0,1 ) );
	//fogdensity2 *= (1+length(lightColor)*(1.2-gm_Stormvalue));
	/*if ( length(gl_Fog.color) > 3.0 ) {
		gl_Fog.color = normalize(gl_Fog.color)*3.0;
		}*/
		
	diffuseColor = mix( gl_Fog.color, diffuseColor*lightColor, fogdensity2 );
	
	// Set final alpha
	diffuseColor.a = finalAlpha;
	gl_FragColor = diffuseColor;
	//gl_FragColor = vec4(calcNormal,diffuseColor.a);
}