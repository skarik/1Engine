#version 430

// Output to screen
layout(location = 0) out vec4 FragColor;

// Inputs from vertex shader
layout(location = 0) in vec4 v2f_position;
layout(location = 1) in vec2 v2f_texcoord0;

// Samplers
layout(binding = 0, location = 20) uniform sampler2D textureSampler0;	// Diffuse
layout(binding = 1, location = 21) uniform sampler2D textureSampler1;	// Normals
layout(binding = 2, location = 22) uniform sampler2D textureSampler2;	// Lighting Properties
layout(binding = 3, location = 23) uniform sampler2D textureSampler3;	// Glow (for now)
layout(binding = 4, location = 24) uniform sampler2D textureSampler4;	// Depth
layout(binding = 5, location = 25) uniform sampler2D textureSampler5; 	// Dithering dots
/*
// Lighting samplers
uniform samplerBuffer textureLightBuffer;
uniform int sys_LightNumber;
uniform vec4 sys_LightAmbient;
// Shadows
uniform mat4 sys_LightMatrix[3];
uniform sampler2D textureShadow0;
uniform sampler2D textureShadow1;
uniform sampler2D textureShadow2;
*/
// Lighting and Shadows
layout(binding = 24, location = 44) uniform samplerBuffer textureLightBuffer;
layout(binding = 4, std140) uniform sys_cbuffer_PerPass
{
	vec4    sys_LightAmbient;
	int     sys_LightNumber;
	int     rr_unused0;
	int     rr_unused1;
	int     rr_unused2;
	vec4	sys_LightParamHack;
	vec4	rr_unused3;
};

layout(binding = 5, std140) uniform def_LightingInfo
{
	mat4 def_LightMatrix0[4];
};
layout(binding = 25, location = 45) uniform samplerBuffer textureLightMatrixBuffer;
layout(binding = 12, location = 32) uniform sampler2D textureShadow0;
layout(binding = 13, location = 33) uniform sampler2D textureShadow1;
layout(binding = 14, location = 34) uniform sampler2D textureShadow2;

vec4 v2f_lightcoord [8];

// System inputs
layout(binding = 2, std140) uniform sys_cbuffer_PerCamera
{
	mat4 sys_ViewProjectionMatrix;
	vec4 sys_WorldCameraPos;
	vec4 sys_ViewportInfo;
	vec2 sys_ScreenSize;
	vec2 sys_PixelRatio;
};
layout(binding = 3, std140) uniform sys_cbuffer_PerFrame
{
	// Time inputs
	vec4    sys_SinTime;
	vec4    sys_CosTime;
	vec4    sys_Time;

	// Fog
	vec4	sys_FogColor;
	vec4	sys_AtmoColor;
	float 	sys_FogEnd;
	float 	sys_FogScale;
};

// 0 for normal lighting, 1 for full wraparound lighting
const float mixthrough = 0.2;

float diffuseLighting ( vec3 normal, vec4 lightDist, float lightRange, float lightFalloff, float lightPass )
{
	// Cosine law breaking ( cuz objects need more detail )
	float normalAttenuate = dot( normal,lightDist.xyz/lightDist.w );
	normalAttenuate = (max( normalAttenuate, -mixthrough ) + mixthrough)/(1+mixthrough);

	// Distance-based attenuation (but double range if facing)
	float attenuation = pow( max( 1.0 - (lightDist.w*lightRange*(1.5-normalAttenuate)), 0.0 ), lightFalloff );
	
	// Attenuation
	float color = mix( normalAttenuate, 1.0, lightPass*(1.0+attenuation) ) * attenuation;
	
	// Return final color
	return color;
}
float specularLighting( vec3 normal, vec4 lightDist, vec3 viewdir, float specular_pow, float lightRange )
{ //TODO: DISTANCE ATTENUATION
	// Create the reflection vector
	vec3 reflectdir = 2*dot(normal,lightDist.xyz)*normal - (lightDist.xyz/lightDist.w);
	// Attenuation is based on camera position and reflection dir
	float attenuation = max( dot( normalize(reflectdir), normalize(viewdir) ), 0.0 );
	// Also add distance-based attenuation though
	float distance_attenuation = max( 1.0 - (lightDist.w*lightRange*0.4), 0.0 );
	
	//return pow( attenuation, specular_pow );
	return pow( attenuation*distance_attenuation, specular_pow+((1.0-distance_attenuation)*2.0) );
}

float cellShade ( float lumin )
{
	const int levels = 2;
	float t = lumin*levels;
	float dif = 0.5 - mod(t+0.5,1);
	t += dif * min(1,(0.5-abs(dif))*32) * 0.5;
	
	lumin = t/levels;
	return lumin;
}
vec2 random ( vec3 seed3 )
{
	float dot_product = dot(seed3, vec3(12.9898,78.233,45.164));
	dot_product = sin(dot_product);
    return vec2( fract(dot_product * 43758.5453), fract(dot_product * 35362.2151) );
}

vec3 echoLighting ( vec4 lightPosition, vec4 lightProperties, vec4 lightColor, in vec4 lightDir, in float rimStrength, in vec3 vertDir, in vec3 vertNormal, in vec4 surfaceInfo )
{
	vec3 resultColor;
	
	// Perform base lighting
	float diffuseLight = 0;
	float diffuseLightO = diffuseLighting( vertNormal, lightDir, lightProperties.x, lightProperties.y, lightProperties.z );

	// lightColor.w is time since sound creation
	// range (lightProperties.x) is calculated in the engine
	
	// Calculate the ripple offset
	float l_rippleOffset = clamp( sin(lightDir.w*3-lightColor.w*27)*0.5+0.5, 0, 1 );
	// Calculate the sound death near the center (gets bigger with time)
	//float l_rippleCenterDecay = 1 - clamp( ((1/lightProperties.x)-lightDir.w)*lightColor.w*0.1, 0, 1 );
	float l_rippleCenterDecay = 1 - clamp( (max(1/lightProperties.x,10)-lightDir.w)*lightColor.w*0.2, 0, 1 );
	
	// Mix between light styles for the directional/nondirectional
	diffuseLight = diffuseLightO * (l_rippleOffset+1)*0.5 * l_rippleCenterDecay;
	// Apply cellshading
	diffuseLight = cellShade(diffuseLight); 
	// Add major rim lighting
	diffuseLight += diffuseLightO * rimStrength * l_rippleCenterDecay;
	
	// Reduce the light a bit
	diffuseLight *= 0.6;
		
	resultColor = lightColor.rgb * clamp( diffuseLight,0,1 );
	
	return resultColor;
}

void main ( void )  
{
	// Use depth to generate the world position
	float pixelDepth 		= texture( textureSampler4, v2f_texcoord0 ).r;
	vec4 pixelPosition = vec4( (v2f_texcoord0.x*2-1),(v2f_texcoord0.y*2-1),pixelDepth,1.0 );
	{
		pixelPosition.z = ( pixelPosition.z*2 - 1 );
		//pixelPosition = sys_ModelViewProjectionMatrixInverse * vec4( pixelPosition.xyz, 1.0 );
		pixelPosition = inverse(sys_ViewProjectionMatrix) * vec4( pixelPosition.xyz, 1.0 );
		pixelPosition.xyzw /= pixelPosition.w;
	}

	/*vec4 pixelPosition = vec4( (v2f_texcoord0.x*2-1),(v2f_texcoord0.y*2-1),pixelDepth,1.0 );
	pixelPosition.z = ( pixelPosition.z*2 - 1 );
	
	pixelPosition = sys_ModelViewProjectionMatrixInverse * vec4( pixelPosition.xyz, 1.0 );
	pixelPosition.xyzw /= pixelPosition.w;*/

	vec4 pixelDiffuse		= texture( textureSampler0, v2f_texcoord0 );
	//vec4 pixelPosition		= texture( textureSampler2, v2f_texcoord0 );	
	vec4 pixelNormal		= texture( textureSampler1, v2f_texcoord0+((random(pixelPosition.rgb)-vec2(0.5,0.5))*0.01) );
	vec4 pixelNormalF		= texture( textureSampler1, v2f_texcoord0 );
	//vec4 pixelLightProperty	= texture( textureSampler3, v2f_texcoord0 );
	vec4 pixelLightProperty = vec4( 1,1,1,1 );
	
	vec4 n_cameraVector = vec4( sys_WorldCameraPos.xyz - pixelPosition.xyz, 0 );
	n_cameraVector.w = length( n_cameraVector.xyz );
	vec3 n_cameraDir = n_cameraVector.xyz / n_cameraVector.w;
	
	float n_rimValue = max(1-dot( pixelNormal.xyz, n_cameraDir ),0.0);
	
	// ==Perform lighting==
	vec3 luminColor = vec3(0,0,0);//sys_LightAmbient.rgb;
	
	// Add some basic color (ambient sound)
	//luminColor += vec3( 0.01,0.02,0.04 ) * 2.8 * abs(sin( dot(pixelPosition.xyz, pixelNormal.yzx)*0.05+sys_Time.y )) * clamp( (250-n_cameraVector.w)/200, 0, 1 ) * (1.5-n_rimValue);
	//luminColor += vec3( 0.01,0.02,0.04 ) * 2.8 * abs( sin( dot(pixelPosition.xyz,pixelPosition.yzx)*0.01+sys_Time.y )) * clamp( (250-n_cameraVector.w)/200, 0, 1 ) * (1.5-n_rimValue);
	luminColor += vec3( 0.01,0.02,0.04 ) * 1.5 * ( 0.5 + abs( sin( dot(n_cameraVector.xyz,n_cameraVector.yzx)*0.002+sys_Time.y*0.4 )) * 0.5 + clamp( pow( n_rimValue, 7 ), 0, 1 )*3 );
	luminColor *= clamp( (120-n_cameraVector.w)/80, 0, 1 );
	//luminColor += vec3( 0.01,0.02,0.04 ) * 5.0 * clamp( pow( n_rimValue, 7 ), 0, 1 );
	luminColor += vec3( 0.01,0.02,0.04 ) * 3.0 * clamp( (20-n_cameraVector.w)/20, 0, 1 ) * (1.5-n_rimValue);
	
	for ( int lightIndex = 0; lightIndex < sys_LightNumber; lightIndex += 1 )
	{
		vec4 lightProperties	= texelFetch( textureLightBuffer, lightIndex*4 + 1 );
		vec4 lightPosition		= texelFetch( textureLightBuffer, lightIndex*4 + 2 );
			
		// Get direction to light 
		vec4 lightDir;
		lightDir.xyz = lightPosition.xyz - pixelPosition.xyz;
		lightDir.w = length(lightDir.xyz);
		if ( lightDir.w*lightPosition.w > 1.4/lightProperties.x ) 
		{	// Skip if light out of range
			continue; // Drops FPS on NVidia if this continue isn't here
		}
		else
		{	// Otherwise perform the light effects
			vec4 lightColor			= texelFetch( textureLightBuffer, lightIndex*4 + 0 );
			vec4 lightDirection		= texelFetch( textureLightBuffer, lightIndex*4 + 3 );
			
			vec3 luminA = echoLighting( lightPosition, lightProperties, lightColor, lightDir, n_rimValue, n_cameraVector.xyz, pixelNormal.xyz, pixelLightProperty );
			vec3 luminF = echoLighting( lightPosition, lightProperties, lightColor, lightDir, n_rimValue, n_cameraVector.xyz, pixelNormalF.xyz, pixelLightProperty );

			luminColor += mix( luminA, luminF, clamp( (luminA+luminF), 0, 1 ) );
		}
	}
	// Rim amount
	luminColor += luminColor*clamp(length(luminColor)+0.1,0,1)*pow(n_rimValue,3)*0.5;
	
	//float lightingStrength = clamp( (pixelLightProperty.r-0.4)/0.6, 0, 1 );
	float lightingStrength = 1.0;
	
	// ==Perform diffuse==
	/*vec3 diffuseColor = pixelDiffuse.rgb;
	// Shadow "outline" effect
	diffuseColor *= 1-(clamp( (pow( clamp(n_rimValue,0,1), 5 )-0.12)/0.1, 0,1 ) * (1-min(1,length(luminColor-sys_LightAmbient.rgb)*4)))*0.3*(lightingStrength);
	*/
	
	// Darken nonvisible parts
	lightingStrength = (pixelDiffuse.a + 1.0)*0.5;
	
	// Output final color
	FragColor.rgb = mix( vec3(0,0,0), luminColor, lightingStrength ); 
	//mix( pixelDiffuse.rgb, diffuseColor*luminColor, clamp( (pixelLightProperty.r-0.4)/0.6, 0, 1 ) ); 
	FragColor.a = lightingStrength;

	//+ dot( n_cameraDir, pixelNormal.xyz );
	
	/*
	// 4X Debug Output
	if ( v2f_texcoord0.x < 0.5 && v2f_texcoord0.y < 0.5 ) {
		FragColor = texture( textureSampler0, v2f_texcoord0*2 );
	}
	else if ( v2f_texcoord0.x > 0.5 && v2f_texcoord0.y < 0.5 ) {
		FragColor = texture( textureSampler1, v2f_texcoord0*2 - vec2(1,0) );
	}
	else if ( v2f_texcoord0.x < 0.5 && v2f_texcoord0.y > 0.5 ) {
		FragColor = texture( textureSampler2, v2f_texcoord0*2 - vec2(0,1) );
	}
	else if ( v2f_texcoord0.x > 0.5 && v2f_texcoord0.y > 0.5 ) {
		FragColor = texture( textureSampler3, v2f_texcoord0*2 - vec2(1,1) );
	}
	FragColor.a = clamp( FragColor.a , 0 , 1 );*/
	
	
}