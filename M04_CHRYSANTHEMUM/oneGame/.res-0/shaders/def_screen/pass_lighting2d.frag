#version 330

//#define DEBUG_OUTPUT
#define ENABLE_LIGHTING
//#define ENABLE_DITHERING
//#define LIGHTING_FLAT
#define LIGHTING_CELLD

// Output to screen
layout(location = 0) out vec4 FragColor;

// Inputs from vertex shader
in vec4 v2f_position;
in vec2 v2f_texcoord0;

// Samplers
uniform sampler2D textureSampler0;	// Diffuse
uniform sampler2D textureSampler1;	// Normals
//uniform sampler2D textureSampler2;	// Position (not anymore, now it shall be WARP...maybe)
//uniform sampler2D textureSampler2;  // Warp
uniform sampler2D textureSampler2;	// Lighting Properties
uniform sampler2D textureSampler3;	// Glow (for now)
uniform sampler2D textureSampler4;	// Depth
uniform sampler2D textureSampler5; // Palette
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
uniform samplerBuffer textureLightBuffer;
uniform int sys_LightNumber;
uniform vec4 sys_LightAmbient;
/*layout(std140) uniform sys_LightingInfo
{
	vec4 sys_LightColor[8];
	vec4 sys_LightPosition[8];
	vec4 sys_LightProperties[8];
	vec4 sys_LightShadowInfo[8];
	mat4 sys_LightMatrix[8];
};*/
uniform mat4 sys_ModelViewProjectionMatrix;
uniform mat4 sys_ModelViewProjectionMatrixInverse;
//uniform vec3 sys_CameraRange;

layout(std140) uniform def_LightingInfo
{
	mat4 def_LightMatrix0[4];
};
uniform samplerBuffer textureLightMatrixBuffer;
uniform sampler2D textureShadow0;
uniform sampler2D textureShadow1;
uniform sampler2D textureShadow2;

vec4 v2f_lightcoord [8];

// System inputs
uniform vec3 sys_WorldCameraPos;

// Fog
layout(std140) uniform sys_Fog
{
	vec4	sys_FogColor;
	vec4	sys_AtmoColor;
	float 	sys_FogEnd;
	float 	sys_FogScale;
};

// 0 for normal lighting, 1 for full wraparound lighting
const float mixthrough = 0.2;

float diffuseLighting ( vec3 normal, vec4 lightDist, float lightRange, float lightFalloff, float lightPass, out float backfaceColor )
{
	// Distance-based attenuation
	float attenuation = pow( max( 1.0 - (lightDist.w*lightRange), 0.0 ), lightFalloff );

	// Cosine law breaking ( cuz objects need more detail )
	float normalAttenuate = dot( normal,lightDist.xyz/lightDist.w );
    //normalAttenuate = max( 0.0, normalAttenuate );
	normalAttenuate = (max( normalAttenuate, -mixthrough ) + mixthrough)/(1+mixthrough);
	// Attenuation
	//float color = mix( normalAttenuate, 1.0, lightPass*(1.0+attenuation) ) * attenuation;
	// Output backlighting
	//backfaceColor = mix( clamp( 1.0-normalAttenuate, 0.0,1.0 ), 0.0, lightPass*(1.0+attenuation) ) * attenuation;

    float color = (normalAttenuate + 0.5) * attenuation;

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

vec3 defaultLighting ( vec4 lightPosition, vec4 lightProperties, vec4 lightColor, in vec4 lightDir, in float rimStrength, in vec3 vertDir, in vec3 vertNormal, in vec4 surfaceInfo, in float backsideGlow )
{
	vec3 resultColor;

    lightPosition.w = 1.0; // Force point lights, not directional

	// Perform base lighting
	float diffuseLight, diffuseLightO, diffuseLightD;
	float difbackLight, difbackLightO, difbackLightD;
	diffuseLightO = diffuseLighting( vertNormal, lightDir, lightProperties.x, lightProperties.y, lightProperties.z, difbackLightO );
	/*diffuseLightD = dot( lightPosition.xyz, vertNormal );
	{
		difbackLightD = clamp( -diffuseLightD, 0.0,1.0 );
		diffuseLightD = (max( diffuseLightD, -mixthrough ) + mixthrough)/(1+mixthrough); // Directional light mixthrough
	}*/
	// Mix between light styles for the directional/nondirectional
	//diffuseLight = mix( diffuseLightD, diffuseLightO, lightPosition.w );
	//difbackLight = 0.8 * (1-backsideGlow) * mix( difbackLightD, difbackLightO, lightPosition.w );

	// Add major rim lighting
	//diffuseLight += diffuseLight * rimStrength * surfaceInfo.a;
	// Apply cellshading
	//diffuseLight = cellShade(diffuseLight);

	// Specular lighting
	float specularLight = 0;
	/*float specularLightO = specularLighting( vertNormal, lightDir, vertDir, surfaceInfo.b, lightProperties.x );
	float specularLightD = specularLighting( vertNormal, vec4(lightPosition.xyz,1), vertDir, surfaceInfo.b, 0 );
	// Mix between light styles for the directional/nondirectional
	specularLight = mix( specularLightD, specularLightO, lightPosition.w );*/

    //resultColor = lightColor.rgb * max( 0, difbackLight+diffuseLight+specularLight*surfaceInfo.g );

#ifdef LIGHTING_FLAT
    // make the color more fullbright + fast change to zero
    diffuseLightO -= 1.0 / 6;
    diffuseLightO = max(0.0, min(1.0, diffuseLightO * 200.0)) * 0.5;
#endif

	resultColor = vec3(1,1,1) * diffuseLightO * lightColor.rgb;

	return resultColor;
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
// Random value
vec2 random ( vec3 seed3 )
{
	float dot_product = dot(seed3, vec3(12.9898,78.233,45.164));
	dot_product = SmoothTriangleWave(dot_product);
    return vec2( fract(dot_product * 43758.5453), fract(dot_product * 35362.2151) );
}

float shadowCalculate ( /*vec4 lightCoords,*/ in float s_strength, sampler2D textureShadow, vec4 m_randomizer/*, vec4 shadowInfo*/ )
{
	float shadowDist = 1.0;
	// Generate texture coords
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
			/*coord.xy = random( vec3(m_randomizer.xy,i) ).xy*(1-cspd)*0.12;
			coord.z = 0;
			coord.xy += vec2( SmoothTriangleWave(i*0.25),SmoothTriangleWave(i*0.25+0.25) )*(1-cspd)*0.12;*/

			//float spread;
			//spread = sqrt(0.02/bias)*2;
			coord.xy = random( vec3(v2f_texcoord0.xy,i)+sys_WorldCameraPos )*(1-cspd)*0.12;
			//coord.xy = vec2(0,0);
			coord.z = 0;
			coord.xy += vec2( SmoothTriangleWave(i*0.25),SmoothTriangleWave(i*0.25+0.25) )*(1-cspd)*0.12;//*spread;

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
			else
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
	if (( s_strength <= 0.5 )||( shadowWcoord3.z < 0.0 )||( minCoords.x > 0.5 )||( minCoords.y > 0.5 ))
	{
		shadowDist = 1.0;
	}

	return shadowDist;
}
/*
float linearizeD ( float depth )
{
	return (2*sys_CameraRange.x) / ( sys_CameraRange.y+sys_CameraRange.x - depth * (sys_CameraRange.y-sys_CameraRange.x) );
}
float delinearizeD ( float edepth )
{
	//return (2*sys_CameraRange.x) / (sys_CameraRange.y+sys_CameraRange.x - depth * (sys_CameraRange.y-sys_CameraRange.x));
	return ( sys_CameraRange.y*edepth + sys_CameraRange.x*(edepth-2) )/( edepth * (sys_CameraRange.y-sys_CameraRange.x) );
}
*/

void main ( void )
{
	// Use depth to generate the world position
	float pixelDepth 		= texture( textureSampler4, v2f_texcoord0 ).r;
	vec4 pixelPosition = vec4( (v2f_texcoord0.x*2-1),(v2f_texcoord0.y*2-1),pixelDepth,1.0 );
	/*{
		pixelPosition.z = ( pixelPosition.z*2 - 1 );
        pixelPosition = sys_ModelViewProjectionMatrixInverse * vec4( pixelPosition.xyz, 1.0 );
		pixelPosition.xyzw /= pixelPosition.w;
	}*/
    pixelPosition.x *= 1280 * 0.25;
    pixelPosition.y *= -720 * 0.25;
    pixelPosition.xyz += sys_WorldCameraPos;

	// pixelLookup
	// xy	palette lookup
	// a	unused (used as temp discard in source)
	vec4 pixelLookup		= texture( textureSampler0, v2f_texcoord0 );
	// pixelNormal
	// rgb	surface normal
	// a	unused
	vec4 pixelNormal		= texture( textureSampler1, v2f_texcoord0 );
	// pixelLightProperty
	// r	lighting effect (0 is fullbright, 1 is normal)
	// g	specular add (from uniform)
	// b	specular power (from unifrom)
	// a	rim lighting strength
	vec4 pixelLightProperty	= texture( textureSampler2, v2f_texcoord0 );
	// pixelGlow
	// rgb	surface glow
	// a	backside lighting to add
	vec4 pixelGlow			= texture( textureSampler3, v2f_texcoord0 );

    // TODO: do proper normals. for now, fuck with it
    //pixelNormal = vec4(0,0,-1,0);
    //pixelNormal.xy += vec2( v2f_texcoord0.x*2-1, v2f_texcoord0.y*2-1 );
    //pixelNormal.xyz = normalize(pixelNormal.xyz);

	vec4 n_cameraVector;
	n_cameraVector.xyz = sys_WorldCameraPos - pixelPosition.xyz;
	n_cameraVector.w = length( n_cameraVector.xyz );
	vec3 n_cameraDir = n_cameraVector.xyz / n_cameraVector.w;

	float n_rimValue = max(1-dot( pixelNormal.xyz, n_cameraDir ),0.0);

	v2f_lightcoord[0] = pixelPosition * def_LightMatrix0[0];
	v2f_lightcoord[1] = pixelPosition * def_LightMatrix0[1];
	v2f_lightcoord[2] = pixelPosition * def_LightMatrix0[2];
	v2f_lightcoord[3] = pixelPosition * def_LightMatrix0[3];

#ifdef ENABLE_LIGHTING
	// ==Perform lighting==
	float lightingStrength = 1.0;//clamp( (pixelLightProperty.r-0.4)/0.6, 0, 1 );
	vec3 luminColor = vec3( 0,0,0 );
	//if ( lightingStrength > 0.5 )
	//{
		//luminColor = sys_LightAmbient.rgb;
		for ( int lightIndex = 0; lightIndex < sys_LightNumber; lightIndex += 1 )
		{
			vec4 lightProperties	= texelFetch( textureLightBuffer, lightIndex*4 + 1 );
			vec4 lightPosition		= texelFetch( textureLightBuffer, lightIndex*4 + 2 );

			// Get direction to light
			vec4 lightDir;
			lightDir.xyz = lightPosition.xyz - floor(pixelPosition.xyz);
            lightDir.z = 0;
            lightDir.x *= 0.5;

			lightDir.w = length(lightDir.xyz);

            //luminColor += vec3(1,1,1) * max(0.0, 1.0 - length(lightDir)*0.01);
			/*if ( lightDir.w*lightPosition.w > 1.4/lightProperties.x )
			{	// Skip if light out of range
				continue; // Drops FPS on NVidia if this continue isn't here
			}
			else*/
			//{	// Otherwise perform the light effects
				vec4 lightColor			= texelFetch( textureLightBuffer, lightIndex*4 + 0 );
				vec4 lightDirection		= texelFetch( textureLightBuffer, lightIndex*4 + 3 );

				luminColor += defaultLighting( lightPosition, lightProperties, lightColor, lightDir, n_rimValue, n_cameraVector.xyz, pixelNormal.xyz, pixelLightProperty, pixelGlow.a ) * 0.5;
			//}
		}
		// Rim amount
		//luminColor += luminColor*clamp(length(luminColor-sys_LightAmbient.rgb)-0.3,0,1)*pow(n_rimValue,3)*0.5 * pixelLightProperty.a;
	/*}
	else
	{
		luminColor = vec3( 1,1,1 );
	}*/
    luminColor = mix( vec3(1,1,1), luminColor, lightingStrength );
	// ==Perform diffuse==
	//vec3 diffuseColor = pixelDiffuse.rgb;
	// Shadow "outline" effect
	//diffuseColor *= 1-(clamp( (pow( clamp(n_rimValue,0,1), 5 )-0.12)/0.1, 0,1 ) * (1-min(1,length(luminColor-sys_LightAmbient.rgb)*4)))*0.3*(lightingStrength);

	// Choose lumin based on glow
	//pixelGlow.a *= pixelLightProperty.r;
	//luminColor = (luminColor*(1-pixelGlow.a)) + max( luminColor*(pixelGlow.a), pixelGlow.rgb );

	// Create color diffuse*lighting result
	//diffuseColor.rgb = diffuseColor*luminColor;
#ifdef LIGHTING_CELLD

    float light = -1.0 + luminColor.r * 2.0;

    const float light_levels = 3;
    float light_stretched = light * light_levels;
	float light_div_dif = 0.5 - mod(light_stretched + 0.5, 1);
	light_stretched += light_div_dif * min(1,(0.5-abs(light_div_dif)) * 350) * 0.5;
	light = (light_stretched)/light_levels;

    light = round(light * 6) / 6;
    light += round(sys_LightAmbient.r * 2.0 * 6) / 6;

#else
        luminColor += sys_LightAmbient.rgb;
        float light = -1.0 + luminColor.r * 2.0;
        //pixelLookup.x = ((pixelLookup.x + light) + (pixelLookup.x * light)) * 0.5;

    #ifdef ENABLE_DITHERING
        // create dithering push
        vec2 coord = round(gl_FragCoord.xy * 0.5);
        float ditherpush = fract( (coord.x + coord.y) * 0.5 ) * 0.1;

        // light needs to be split into 6 levels (based on width of texture5/palette)
        light = round(light * 6 + ditherpush - 0.5) / 6;
    #else
        // light needs to be split into 6 levels (based on width of texture5/palette)
        light = round(light * 6) / 6;
    #endif
#endif
    pixelLookup.x = pixelLookup.x + light;

#else

    vec2 rounded_coord = v2f_texcoord0.xy;
    rounded_coord.x = floor(rounded_coord.x * 640.0) / 640.0;
    rounded_coord.y = floor(rounded_coord.y * 360.0) / 360.0;

    float light = 0.5 - (length(vec2(0.5,0.5) /*+ random(rounded_coord.xxy)/10.0*/ - rounded_coord.xy) * 3.0);
    pixelLookup.x = ((pixelLookup.x + light) + (pixelLookup.x * light)) * 0.5;
    float lightingStrength = 0.0;

#endif
    // pixelDiffuse
    // rgb  surface color
    vec4 pixelDiffuse       = texture( textureSampler5, pixelLookup.xy );

    vec3 diffuseColor = pixelDiffuse.rgb;



	// ==Perform fog==
	float n_fogDensity = clamp( (sys_FogEnd - n_cameraVector.w) * sys_FogScale, 0, 1 );
	n_fogDensity = mix( 1, n_fogDensity, lightingStrength );
	// Mix output with fog

    //FragColor.rgb = luminColor.rgb;
    //FragColor.rgb += diffuseColor.rgb * 0.1;
    //FragColor.rgb += pixelPosition.rgb;

    FragColor.rgb = diffuseColor;

	// Output fog mix
	//FragColor.rgb = mix( sys_FogColor.rgb, diffuseColor.rgb, n_fogDensity );
	FragColor.a = 1.0;

    //gl_FragDepth = pixelDepth;

	//FragColor.rgb = pixelGlow.rgb * pixelLightProperty.r;//luminColor.rgb*0.5;//vec3(1,1,1) * pixelGlow.a;
	//+ dot( n_cameraDir, pixelNormal.xyz );

#ifdef DEBUG_OUTPUT
	// 4X Debug Output
    if ( v2f_texcoord0.x < 0.5 && v2f_texcoord0.y < 0.5 ) {
		FragColor = clamp( texture( textureSampler0, v2f_texcoord0*2 ) * vec4(1.0, 8.0, 1.0, 1.0), 0.0, 1.0 );
	}
	else if ( v2f_texcoord0.x > 0.5 && v2f_texcoord0.y < 0.5 ) {
		FragColor = (texture( textureSampler1, v2f_texcoord0*2 - vec2(1,0) )+1)*0.5;
	}
	else if ( v2f_texcoord0.x < 0.5 && v2f_texcoord0.y > 0.5 ) {
        //FragColor = texture( textureSampler2, v2f_texcoord0*2 - vec2(0,1) );
		FragColor = vec4( texture( textureSampler5, v2f_texcoord0*2 - vec2(0,1) ).rgb, 1.0 );
	}
	else if ( v2f_texcoord0.x > 0.5 && v2f_texcoord0.y > 0.5 ) {
		FragColor = texture( textureSampler3, v2f_texcoord0*2 - vec2(1,1) );
	}
	FragColor.a = clamp( FragColor.a , 0.0 , 1.0 );
#endif

}
