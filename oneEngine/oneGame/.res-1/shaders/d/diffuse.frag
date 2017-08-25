#version 140

//#define USE_REFLECTSPECULAR

// Inputs from vertex shader
//in vec4 v2f_normals;
in mat3 v2f_tangent;
in vec4 v2f_colors;
in vec4 v2f_position;
in vec2 v2f_texcoord0;
in vec3 v2f_screenpos;
in float v2f_fogdensity;

// Samplers
uniform sampler2D textureSampler0; // Diffuse map
uniform sampler2D textureSampler1; // Glowmap
uniform sampler2D textureSampler2; // Specular map
uniform sampler2D textureSampler3; // Normal map

// System Inputs
uniform vec4 sys_DiffuseColor;
uniform vec3 sys_EmissiveColor;
uniform float sys_AlphaCutoff;

uniform vec3 sys_WorldCameraPos;

uniform vec4 sys_LightAmbient;

// Lighting and Shadows
layout(std140) uniform sys_LightingInfo
{
	vec4 sys_LightColor[8];
	vec4 sys_LightPosition[8];
	vec4 sys_LightProperties[8];
	vec4 sys_LightShadowInfo[8];
	mat4 sys_LightMatrix[8];
};
uniform sampler2D textureShadow0;
uniform sampler2D textureShadow1;
uniform sampler2D textureShadow2;

in vec4 v2f_lightcoord[8];

// World Reflection
#ifdef USE_REFLECTSPECULAR
layout(std140) uniform sys_ReflectInfo
{
	vec4 sys_ReflectMinBox;
	vec4 sys_ReflectMaxBox;
	vec4 sys_ReflectSource;
};
uniform samplerCube textureReflection0; // World Reflection
#endif

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

// Sine wave appoximation method (benchmarked to about 3% speed increase over all shadows)
float SmoothCurve( float x ) {  
	return x * x *( 3.0 - 2.0 * x );  
}  
float TriangleWave( float x ) {  
	return abs( fract( x + 0.5 ) * 2.0 - 1.0 );  
}  
float SmoothTriangleWave( float x ) {  
	return (SmoothCurve( TriangleWave( x ) ))*2 - 1;  
} 
// Randomizer
vec3 random ( vec3 seed3 )
{
	float dot_product = dot(seed3, vec3(12.9898,78.233,45.164));
	dot_product = SmoothTriangleWave(dot_product);
    return vec3( fract(dot_product * 43758.5453), fract(dot_product * 35362.2151), fract(dot_product * 56433.2357) );
}


vec3 defaultLighting ( vec4 lightPosition, vec4 lightProperties, vec4 lightColor, vec3 vertDir, vec3 vertNormal )
{
	vec3 resultColor;
	// Get direction to light
	vec3 lightDir;
	lightDir = lightPosition.xyz - v2f_position.xyz;
	// Perform base lighting
	float lightVal1 = diffuseLighting( vertNormal, lightDir, lightProperties.x, lightProperties.y, lightProperties.z );
	float lightVal2 = dot( lightPosition.xyz, vertNormal );
	lightVal2 = (max( lightVal2, -mixthrough ) + mixthrough)/(1+mixthrough); // Directional light mixthrough
	// Mix between light styles for the directional/nondirectional
	float lightValf = mix( lightVal2, lightVal1, lightPosition.w );
	lightValf = cellShade(lightValf); // Apply cellshading
		
	resultColor = lightColor.rgb * max( 0, lightValf );
	
	return resultColor;
}
float shadowCalculate ( /*vec4 lightCoords,*/ vec4 shadowInfo, sampler2D textureShadow )
{
	float shadowDist = 1.0;
	// Generate texture coords
	/*vec4 shadowWcoord = vec4(
		lightCoords.x / lightCoords.w,
		lightCoords.y / lightCoords.w,
		lightCoords.z / lightCoords.w,
		lightCoords.w );*/
	vec4 shadowWcoord3 = v2f_lightcoord[0];
	shadowWcoord3.xyz /= shadowWcoord3.w;
	vec4 shadowWcoord2 = v2f_lightcoord[1];
	shadowWcoord2.xyz /= shadowWcoord2.w;	
	vec4 shadowWcoord1 = v2f_lightcoord[2];
	shadowWcoord1.xyz /= shadowWcoord1.w;
	vec4 shadowWcoord0 = v2f_lightcoord[3];
	shadowWcoord0.xyz /= shadowWcoord0.w;
	vec2 minCoords;
	// Get rid of the non-uniformity. Always sample shadows.
	{
		const float cspd = 0.98; // Cascade padding value
					
		float depthDifference = 0.0;
		float distanceFromLight;
		vec3 coord; 
		float bias = 0;
		for ( int i = 0; i < 4; i += 1 )
		{
			coord.xy = random( vec3(v2f_screenpos.xy,i) ).xy*(1-cspd)*0.12;
			coord.z = 0;
			coord.xy += vec2( SmoothTriangleWave(i*0.25),SmoothTriangleWave(i*0.25+0.25) )*(1-cspd)*0.12;
		
			if ( (abs(shadowWcoord0.x-0.5) < 0.5*cspd) && (abs(shadowWcoord0.y-0.5) < 0.5*cspd) )
			{
				coord += shadowWcoord0.xyz;
				coord.x = coord.x*0.25 + 0.75;
				bias = 0.02;
			}
			else if ( (abs(shadowWcoord1.x-0.5) < 0.5*cspd) && (abs(shadowWcoord1.y-0.5) < 0.5*cspd) )
			{
				coord += shadowWcoord1.xyz;
				coord.x = coord.x*0.25 + 0.50;
				bias = 0.08;
			}
			else if ( (abs(shadowWcoord2.x-0.5) < 0.5*cspd) && (abs(shadowWcoord2.y-0.5) < 0.5*cspd) )
			{
				coord += shadowWcoord2.xyz;
				coord.x = coord.x*0.25 + 0.25;
				bias = 0.64;
			}
			else //if ( abs(shadowWcoord3.x-0.5) < 0.5*cspd && abs(shadowWcoord3.y-0.5) < 0.5*cspd )
			{
				coord += shadowWcoord3.xyz;
				coord.x = coord.x*0.25;
				bias = 2.56;
			}
			
			// Limit Y coordinate
			coord.y = max( min( coord.y, 1.0 ), 0.0 );
			
			distanceFromLight = texture( textureShadow, coord.xy ).r;
			depthDifference += clamp((coord.z - distanceFromLight)*1024.0 - bias, 0.0,1.0);
		}
		depthDifference /= 4.0;
		
		minCoords = min( abs(shadowWcoord3.xy-vec2(0.5,0.5)), abs(shadowWcoord2.xy-vec2(0.5,0.5)) );
		minCoords = min( minCoords, abs(shadowWcoord1.xy-vec2(0.5,0.5)) );
		minCoords = min( minCoords, abs(shadowWcoord0.xy-vec2(0.5,0.5)) );
		//minCoords.xy /= cspd;
		// Decrease dif val if close to edge
		/*depthDifference *= clamp( 8.0-abs(maxCoords.x-0.5)*16.0, 0.0,1.0 );
		depthDifference *= clamp( 8.0-abs(maxCoords.y-0.5)*16.0, 0.0,1.0 );*/
		depthDifference *= clamp( 8.0-minCoords.x*16.0, 0.0,1.0 );
		depthDifference *= clamp( 8.0-minCoords.y*16.0, 0.0,1.0 );
		
		// Do the color mix
		shadowDist = clamp( 1.0-depthDifference, 0.0,1.0 );
	}
	// If out of range or shadows are not enabled, set shadowDist to 1
	if (( shadowInfo.x <= 0.5 )||( shadowWcoord3.z < 0.0 )||( minCoords.x > 0.5 )||( minCoords.y > 0.5 ))
	{
		shadowDist = 1.0;
	}

	return shadowDist;
}

#ifdef USE_REFLECTSPECULAR
// Box projection code shamelessly ripped from http://devlog-martinsh.blogspot.com
// because sphere projection kept giving a really drunk result
vec3 bpcem ( in vec3 n_raydir )
{
	vec3 nrdir = normalize(n_raydir);
	vec3 rbmax = (sys_ReflectMaxBox.xyz - v2f_position.xyz)/nrdir;
	vec3 rbmin = (sys_ReflectMinBox.xyz - v2f_position.xyz)/nrdir;

	vec3 rbminmax;
	rbminmax.x = (nrdir.x>0.0)?rbmax.x:rbmin.x;
	rbminmax.y = (nrdir.y>0.0)?rbmax.y:rbmin.y;
	rbminmax.z = (nrdir.z>0.0)?rbmax.z:rbmin.z;
	float fa = min(min(rbminmax.x, rbminmax.y), rbminmax.z);
	vec3 posonbox = v2f_position.xyz + nrdir * fa;
	return posonbox - sys_ReflectSource.xyz;
}
#endif

void main ( void )  
{
	// Diffuse color
	vec4 diffuseColor = texture( textureSampler0, v2f_texcoord0 );
	float f_alpha = diffuseColor.a * sys_DiffuseColor.a;
	if ( f_alpha < sys_AlphaCutoff ) discard;
	
	// Grab specular amount
	vec4 pixelSpecular = texture( textureSampler2, v2f_texcoord0 );
	
	// Normal mapping
	vec3 normalColor = texture( textureSampler3, v2f_texcoord0 ).rgb;
	normalColor = normalColor*2 - 1;
	/*{
		vec4 difbitX = texture2D( textureSampler0, v2f_texcoord0+vec2(0.006,0) );
		vec4 difbitY = texture2D( textureSampler0, v2f_texcoord0+vec2(0,0.006) );
		float l0 = length( diffuseColor.rgb );
		float lX = length( difbitX.rgb );
		float lY = length( difbitY.rgb );
		vec3 vecX = vec3( -0.2, 0, lX-l0 );
		vec3 vecY = vec3( 0, -0.2, lY-l0 );
		normalColor = cross( vecX, vecY );
		if ( normalColor.z < 0 ) {
			normalColor = -normalColor;
		}
		normalColor = normalize( normalColor );
	}*/
	vec3 pixelNormal = v2f_tangent * normalColor;
	
	// Camera direction
	vec3 vertDir;
	vertDir = sys_WorldCameraPos-v2f_position.xyz;
	
	// Create the reflection vector and compute reflection
#ifdef USE_REFLECTSPECULAR
	vec3 pixelReflect = normalize(vertDir) - 2*dot(pixelNormal,normalize(vertDir))*normalize(pixelNormal);
	vec4 reflectionColor = texture( textureReflection0, bpcem(pixelReflect).xzy );
#endif

	// Rim effect
	float lightVal3 = max(1-dot( pixelNormal, normalize(vertDir) ),0.0);
	
	// Lighting
	vec3 lightColor = sys_EmissiveColor;	// gl_SecondaryColor is set as emissive when useColors = false for materials
	lightColor += texture( textureSampler1, v2f_texcoord0 ).rgb;
	// Fixed-function lighting
	lightColor +=
		defaultLighting ( sys_LightPosition[0], sys_LightProperties[0], sys_LightColor[0], vertDir, pixelNormal )
		* shadowCalculate( sys_LightShadowInfo[0], textureShadow0 );
	lightColor += defaultLighting ( sys_LightPosition[1], sys_LightProperties[1], sys_LightColor[1], vertDir, pixelNormal );
	lightColor += defaultLighting ( sys_LightPosition[2], sys_LightProperties[2], sys_LightColor[2], vertDir, pixelNormal );
	lightColor += defaultLighting ( sys_LightPosition[3], sys_LightProperties[3], sys_LightColor[3], vertDir, pixelNormal );
	lightColor += defaultLighting ( sys_LightPosition[4], sys_LightProperties[4], sys_LightColor[4], vertDir, pixelNormal );
	// Rim amount (based on direct lights)
	lightColor += lightColor*clamp(length(lightColor)-0.3,0,1)*pow(lightVal3,3);
	// Ambient
	lightColor += sys_LightAmbient.rgb;
	
	// Shadow "outline" effect
	diffuseColor.rgb *= 1-(clamp( (pow( clamp(lightVal3,0,1), 5 )-0.12)/0.1, 0,1 ) * (1-min(1,length(lightColor-sys_LightAmbient.rgb)*4)))*0.3;
	
#ifdef USE_REFLECTSPECULAR
	// Reflection
	lightColor += reflectionColor.rgb * lightColor * (pixelSpecular.r+max(length(reflectionColor)*0.3-0.2,0));
#endif

	gl_FragColor.rgb = mix( sys_FogColor.rgb, diffuseColor.rgb * sys_DiffuseColor.rgb * lightColor, v2f_fogdensity );
	//gl_FragColor.rgb = mix( gl_FragColor.rgb, sys_DiffuseColor.rgb*lightColor*0.3, 0.999 );
	//gl_FragColor.rgb = mix( gl_FragColor.rgb, vec3((v2f_lightcoord[3].xy-vec2(0.5,0.5)),0), 0.999 );
	/*
	if ( abs(v2f_lightcoord[3].x-0.5) < 0.5 && abs(v2f_lightcoord[3].y-0.5) < 0.5 ) {
		gl_FragColor.rgb = gl_FragColor.rgb*0.01 + vec3( 0.7,0.3,0.3 )*v2f_lightcoord[3].z*2;
	}
	else if ( abs(v2f_lightcoord[2].x-0.5) < 0.5 && abs(v2f_lightcoord[2].y-0.5) < 0.5 ) {
		gl_FragColor.rgb = gl_FragColor.rgb*0.01 + vec3( 0.7,0.7,0.3 )*v2f_lightcoord[2].z*2;
	}
	else if ( abs(v2f_lightcoord[1].x-0.5) < 0.5 && abs(v2f_lightcoord[1].y-0.5) < 0.5 ) {
		gl_FragColor.rgb = gl_FragColor.rgb*0.01 + vec3( 0.3,0.7,0.7 )*v2f_lightcoord[1].z*2;
	}
	else if ( abs(v2f_lightcoord[0].x-0.5) < 0.5 && abs(v2f_lightcoord[0].y-0.5) < 0.5 ) {
		gl_FragColor.rgb = gl_FragColor.rgb*0.01 + vec3( 0.3,0.3,0.7 )*v2f_lightcoord[0].z*2;
	}
	else {
		gl_FragColor.rgb = gl_FragColor.rgb*0.01 + vec3( 0.3,0.3,0.3 );
	}
	gl_FragColor.rgb *= lightColor;*/
	//gl_FragColor.rgb = mix( gl_FragColor.rgb, lightVal3*vec3(1,1,1), 0.999 );
	//gl_FragColor.rgb = mix( gl_FragColor.rgb, (normalColor.rgb+1)/2, 0.999 );
	//gl_FragColor.rgb = mix( gl_FragColor.rgb, (pixelNormal.rgb+1)/2, 0.999 );
	//gl_FragColor.rgb = mix( gl_FragColor.rgb, (v2f_tangent[0].rgb+1)/2, 0.999 );
	gl_FragColor.a = f_alpha;
}