#version 330
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

//#define DEBUG_OUTPUT
#define ENABLE_LIGHTING
//#define ENABLE_JOKE_AMBIENT_OCCLUSION
//#define ENABLE_DITHERING
//#define LIGHTING_FLAT
#define LIGHTING_CELLD

// Output to screen
layout(location = 0) out vec4 FragColor;

// Inputs from vertex shader
in vec4 v2f_position;
in vec2 v2f_texcoord0;
in vec2 v2f_texcoord1;

// Samplers
layout(location = 20) uniform sampler2D textureSampler0;	// Diffuse
layout(location = 21) uniform sampler2D textureSampler1;	// Normals
layout(location = 22) uniform sampler2D textureSampler2;	// Lighting Properties
layout(location = 23) uniform sampler2D textureSampler3;	// Glow (for now)
layout(location = 24) uniform sampler2D textureSampler4;	// Depth
layout(location = 25) uniform sampler2D textureSampler5;  // Palette
layout(location = 26) uniform sampler3D textureSampler6;  // 3D Palette
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
layout(location = 44) uniform samplerBuffer textureLightBuffer;
layout(std140) uniform sys_cbuffer_PerPass
{
    vec4    sys_LightAmbient;
    int     sys_LightNumber;
    int     rr__unused0;
    int     rr__unused1;
    int     rr__unused2;
};

/*
textureLightBuffer layout:
offset 0
    float red;
    float green;
    float blue;
    float specular;
offset 1
    float range;
    float falloff;
    float passthrough;
    float hasshadow;
offset 2
    float x;
    float y;
    float z;
    float directional;
offset 3
    float dx;
    float dy;
    float dz;
    float dw;
*/

/*layout(std140) uniform sys_LightingInfo
{
	vec4 sys_LightColor[8];
	vec4 sys_LightPosition[8];
	vec4 sys_LightProperties[8];
	vec4 sys_LightShadowInfo[8];
	mat4 sys_LightMatrix[8];
};*/
/*layout(location = 128) uniform mat4 sys_ModelViewProjectionMatrix;
layout(location = 129) uniform mat4 sys_ModelViewProjectionMatrixInverse;*/
//uniform vec3 sys_CameraRange;

/*layout(std140) uniform def_LightingInfo
{
	mat4 def_LightMatrix0[4];
};*/
layout(location = 45) uniform samplerBuffer textureLightMatrixBuffer;
layout(location = 32) uniform sampler2D textureShadow0;
layout(location = 33) uniform sampler2D textureShadow1;
layout(location = 34) uniform sampler2D textureShadow2;

//vec4 v2f_lightcoord [8];

// System inputs
/*layout(location = 130) uniform vec3 sys_WorldCameraPos;
layout(location = 131) uniform vec4 sys_ViewportInfo;

// Fog
layout(std140) uniform sys_Fog
{
	vec4	sys_FogColor;
	vec4	sys_AtmoColor;
	float 	sys_FogEnd;
	float 	sys_FogScale;
};*/
layout(std140) uniform sys_cbuffer_PerCamera
{
    mat4 sys_ViewProjectionMatrix;
    vec4 sys_WorldCameraPos;
    vec4 sys_ViewportInfo;
    vec2 sys_ScreenSize;
    vec2 sys_PixelRatio;
};
layout(std140) uniform sys_cbuffer_PerFrame
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

// ========================================================================== //
// Utility functions:

// Converts input RGB to HSV (all values 0-1)
vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}
// Converts input HSV to RGB (all values 0-1)
vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
// Returns luminosity of the inupt RGB value
float rgb2luma(vec3 color)
{
    return dot(color, vec3(0.299, 0.587, 0.114));
}

// ========================================================================== //
// Shading model:

//      lighting_reflection_source()
// Calculates scene reflections.
// Yeah this is horse shit, taking some bullshit junk from...screen-space reflections.
vec3 lighting_reflection_source ( vec3 surfaceNormal, float surfaceSmoothness )
{
    // Pull target pixel color and normal
    vec2 lookup = texture(textureSampler0, v2f_texcoord0 - vec2(-surfaceNormal.x, surfaceNormal.y - 0.134) * 0.05).rg;
    vec3 normal = texture(textureSampler1, v2f_texcoord0 - vec2(-surfaceNormal.x, surfaceNormal.y - 0.134) * 0.05).rgb;
    // Darken the pixel based on the normal, environment, and material properties
    lookup.r *= rgb2luma(sys_LightAmbient.rgb) * 2.0;
    lookup.r *= 0.75 - dot(surfaceNormal, normal) * 0.25;
    lookup.r *= surfaceSmoothness;
    lookup.r *= 2.0;
    // Sample color from the palette
    return texture(textureSampler5, lookup).rgb;
}

//      lighting_collect()
// Performs light collection routine: Loops through all the lights and performs simple calcuations.
// The first two arguments are the RGB outputs of the color.
void lighting_collect ( out vec3 diffuseFactor, out vec3 specularFactor, in vec3 surfaceNormal, in vec3 surfacePosition )
{
    diffuseFactor = vec3(0,0,0);
    specularFactor = vec3(0,0,0);

    for ( int lightIndex = 0; lightIndex < sys_LightNumber; lightIndex += 1 )
	{
        vec4 lightColor			= texelFetch( textureLightBuffer, lightIndex*4 + 0 );
		vec4 lightProperties	= texelFetch( textureLightBuffer, lightIndex*4 + 1 );
		vec4 lightPosition		= texelFetch( textureLightBuffer, lightIndex*4 + 2 );
        vec4 lightDirection		= texelFetch( textureLightBuffer, lightIndex*4 + 3 );

		// Get direction of surface to light
		vec4 surface_to_light;
		surface_to_light.xyz = lightPosition.xyz - surfacePosition.xyz;
        //surface_to_light.z += 0.3 / lightProperties.x;
        //surface_to_light.x *= 0.5;
        surface_to_light.z = -surface_to_light.z;
        surface_to_light.x *= 0.5;
        surface_to_light.y *= 1.0 - (surfaceNormal.y * 0.33);
		surface_to_light.w = max(0.01, length(surface_to_light.xyz));

        // Calculate diffuse
        float surface_dot_light = clamp( dot(surfaceNormal, surface_to_light.xyz/surface_to_light.w), -1.0, 1.0 );
        float diffuse_light_range = lightProperties.x;// * (1.0 - max(0.0, surface_dot_light) * 0.25);
        float diffuse_attenuation = pow( max( 1.0 - (surface_to_light.w * diffuse_light_range), 0.0 ), lightProperties.y );
        diffuseFactor += lightColor.rgb * max(0.0, surface_dot_light) * diffuse_attenuation;

        // Calculate specular
        // TODO
	}

    // Add ambient color
    diffuseFactor += sys_LightAmbient.rgb;
}

//      lighting_reflection()
// Input light via reflections. Usually for metal.
// Metal to non-metal blending is controlled by geometry pass.
vec3 lighting_reflection( vec3 surfaceSpecularMask, vec3 specular, vec3 reflection )
{
    return (specular + reflection) * surfaceSpecularMask;
}
//      lighting_diffuse()
// Input light via diffuse reflection. Metals have a near-zero albedo.
vec3 lighting_diffuse ( vec3 diffuse, vec3 albedo )
{
    return diffuse * albedo;//+ fresnel_reflection
}

//      lighting_step()
// Takes light and cel-shades it
float lighting_step ( float light_level )
{
    // Round light to same number of levels as the palette width
    const float light_levels = 6 / 2;
    float  light_stretched = light_level * light_levels;
	float  light_div_dif = 0.5 - mod(light_stretched + 0.5, 1);
	light_stretched += light_div_dif * min(1.0, (0.5-abs(light_div_dif)) * 350) * 0.5;
	float light = (light_stretched)/light_levels;
    light = round(light * 6) / 6;

    return light;
}
//      lighting_mix()
// Performs surface coloring, lighting, and reflection.
vec3 lighting_mix (
    vec3 pixelLookup,
    vec3 surfaceNormal, vec3 surfacePosition, vec3 surfaceSpecularMask, float surfaceSmoothness )
{
    // First collect the values of diffuse lights.
    vec3 diffuse_factor;
    vec3 specular_factor;
    lighting_collect(
        // Outputs
        diffuse_factor, specular_factor,
        // Inputs
        surfaceNormal, surfacePosition
    );
    // Generate reflection
    vec3 reflection_factor = lighting_reflection_source( surfaceNormal, surfaceSmoothness );

    // Normally, the result would be simple:
    //return lighting_diffuse(...) + lighting_reflection(...);
    // However, since we want everything to stay on the same palette, we change that.

    // First, we calculate diffuse lighting.
    vec4 diffuse_light;
    diffuse_light.rgb = lighting_diffuse(diffuse_factor, vec3(1,1,1));
    diffuse_light.a = rgb2luma(diffuse_light.rgb);
    // Next, we calculate reflection lighting.
    vec4 reflection_light;
    reflection_light.rgb = lighting_reflection(surfaceSpecularMask, specular_factor, reflection_factor);
    reflection_light.a = rgb2luma(reflection_light.rgb);
    reflection_light.rgb = (reflection_light.rgb + reflection_light.aaa) * 0.5;

    // The brightness of the diffuse + reflection is used to offset the X lookup in the palette.
    float light_level = diffuse_light.a;// + reflection_light.a;
    pixelLookup.x += lighting_step((light_level - 0.5) * 2.0);
    pixelLookup.x += reflection_light.a * 2.0;
    // Now, the palette is sampled and we have our base world color.
    vec3 surfaceColor = texture( textureSampler5, pixelLookup.xy ).rgb;

    // With the world color, we need to blend in the light color. To properly do this, we use the light's saturation and hue.
    vec3 light_color = normalize(hsv2rgb(vec3( rgb2hsv(diffuse_light.rgb + reflection_light.rgb).xy, 1.0 ))) * 1.732;
    // We multiply the world by this color, then use that to cast into all the available colors
    surfaceColor *= light_color;
    surfaceColor = texture( textureSampler6, surfaceColor.xyz ).rgb;

    return surfaceColor;
}

// ========================================================================== //
// Shadow sampling utilities (noise filter):

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

/*
#ifdef ENABLE_JOKE_AMBIENT_OCCLUSION

float ao_test ( void )
{
    vec2 offset = vec2( 1.0 / 1280, 1.0 / 720 ) * 2.0;
    vec4 pixelNormalCenter = texture( textureSampler1, v2f_texcoord0 );
    vec4 pixelNormalBlurred = vec4(0,0,0,0);

    pixelNormalBlurred += texture( textureSampler1, v2f_texcoord0 + vec2(offset.x,0) );
    pixelNormalBlurred += texture( textureSampler1, v2f_texcoord0 + vec2(0,offset.y) );
    pixelNormalBlurred += texture( textureSampler1, v2f_texcoord0 + vec2(-offset.x,0) );
    pixelNormalBlurred += texture( textureSampler1, v2f_texcoord0 + vec2(0,-offset.y) );

    pixelNormalBlurred += texture( textureSampler1, v2f_texcoord0 + vec2(offset.x,offset.y) ) * 2.0 / 3.0;
    pixelNormalBlurred += texture( textureSampler1, v2f_texcoord0 + vec2(offset.x,-offset.y) ) * 2.0 / 3.0;
    pixelNormalBlurred += texture( textureSampler1, v2f_texcoord0 + vec2(-offset.x,offset.y) ) * 2.0 / 3.0;
    pixelNormalBlurred += texture( textureSampler1, v2f_texcoord0 + vec2(-offset.x,-offset.y) ) * 2.0 / 3.0;

    pixelNormalBlurred += texture( textureSampler1, v2f_texcoord0 + vec2(offset.x,0) * 2 ) / 2.0;
    pixelNormalBlurred += texture( textureSampler1, v2f_texcoord0 + vec2(0,offset.y) * 2 ) / 2.0;
    pixelNormalBlurred += texture( textureSampler1, v2f_texcoord0 + vec2(-offset.x,0) * 2 ) / 2.0;
    pixelNormalBlurred += texture( textureSampler1, v2f_texcoord0 + vec2(0,-offset.y) * 2 ) / 2.0;

    pixelNormalBlurred += texture( textureSampler1, v2f_texcoord0 + vec2(offset.x,offset.y) * 2 ) / 3.0;
    pixelNormalBlurred += texture( textureSampler1, v2f_texcoord0 + vec2(offset.x,-offset.y) * 2 ) / 3.0;
    pixelNormalBlurred += texture( textureSampler1, v2f_texcoord0 + vec2(-offset.x,offset.y) * 2 ) / 3.0;
    pixelNormalBlurred += texture( textureSampler1, v2f_texcoord0 + vec2(-offset.x,-offset.y) * 2 ) / 3.0;

    pixelNormalBlurred /= 10;

    float dist = length(max(pixelNormalBlurred.xy - pixelNormalCenter.xy, vec2(0,0)));
    //float dist = length(pixelNormalBlurred.xy - pixelNormalCenter.xy);
    return dist;
}

#endif*/

// ========================================================================== //
// Main shader:

void main ( void )
{
	// Use depth to generate the world position
	float pixelDepth   = texture( textureSampler4, v2f_texcoord0 ).r;
    //vec4 pixelPosition = vec4( (v2f_texcoord0.x*2-1),(v2f_texcoord0.y*2-1), pixelDepth, 1.0 );
	//vec4 pixelPosition = vec4( (v2f_texcoord0.x*2 - 0.5),(v2f_texcoord0.y*2 - 0.5), pixelDepth, 1.0 );
    vec4 pixelPosition = vec4( v2f_texcoord1.xy, pixelDepth, 1.0 );
	/*{
		pixelPosition.z = ( pixelPosition.z*2 - 1 );
        pixelPosition = sys_ModelViewProjectionMatrixInverse * vec4( pixelPosition.xyz, 1.0 );
		pixelPosition.xyzw /= pixelPosition.w;
	}*/
    pixelPosition.x *= sys_ViewportInfo.z;
    pixelPosition.y *= -sys_ViewportInfo.w;
    pixelPosition.z *= (pixelPosition.z - 0.5) * -1000;
    pixelPosition.xy += sys_WorldCameraPos.xy;

	// pixelLookup
	// xy	palette lookup
	// a	unused (used as temp discard in source)
	vec4 pixelLookup		= texture( textureSampler0, v2f_texcoord0 );
	// pixelNormal
	// rgb	surface normal
	// a	ambient occlusion
	vec4 pixelNormal		= texture( textureSampler1, v2f_texcoord0 );
	// pixelLightProperty
    // rgb  specular color
	// a	smoothness
	vec4 pixelLightProperty	= texture( textureSampler2, v2f_texcoord0 );
	// pixelGlow
	// rgb	surface glow
	// a	rim lighting strength
	vec4 pixelGlow			= texture( textureSampler3, v2f_texcoord0 );

    // Fix up the normals quickly here
    pixelNormal.w = length(pixelNormal.xyz);
    pixelNormal.xyz /= pixelNormal.w;

    // ==Perform lighting==

	vec4 n_cameraVector;
	n_cameraVector.xyz = sys_WorldCameraPos.xyz - pixelPosition.xyz + vec3(0,0,500);
	n_cameraVector.w = length( n_cameraVector.xyz );
	vec3 n_cameraDir = n_cameraVector.xyz / n_cameraVector.w;
    vec3 n_cameraDirRim = vec3(0,0.707,0.707);

    // Generate general rim-light value
	float n_rimValue = max(1.0 - dot( pixelNormal.xyz, n_cameraDirRim ), 0.0);

#ifdef ENABLE_LIGHTING

    vec3 specularMask = max(vec3(0.0,0.0,0.0), texture(textureSampler5, pixelLightProperty.xy).rgb - vec3(0.16,0.16,0.16));

    vec3 diffuseColor = lighting_mix(
        pixelLookup.xyz,
        pixelNormal.xyz, pixelPosition.xyz, specularMask, pixelLightProperty.a
    );

#else

    vec2 rounded_coord = v2f_texcoord0.xy;

    float light = 0.5 - (length(vec2(0.5,0.5) /*+ random(rounded_coord.xxy)/10.0*/ - rounded_coord.xy) * 3.0);
    pixelLookup.x = ((pixelLookup.x + light) + (pixelLookup.x * light)) * 0.5;
    float lightingStrength = 0.0;

    // pixelDiffuse - Generate via pulling from the palette.
    // rgb  surface color
    vec4 pixelDiffuse       = texture( textureSampler5, pixelLookup.xy );
    vec3 diffuseColor = pixelDiffuse.rgb;

#endif


	// ==Perform fog==
	//float n_fogDensity = clamp( (sys_FogEnd - n_cameraVector.w) * sys_FogScale, 0, 1 );
	//n_fogDensity = mix( 1, n_fogDensity, lightingStrength );
	// Mix output with fog

    FragColor.rgb = diffuseColor;
    //if ( v2f_texcoord0.x > 0.5 )
    //    FragColor.rgb = luminColor.rgb;
    //FragColor.rgb += diffuseColor.rgb * 0.1;
    //FragColor.rgb = ((pixelPosition.zzz) + 500.0)/1000.0;


	// Output fog mix
	//FragColor.rgb = mix( sys_FogColor.rgb, diffuseColor.rgb, n_fogDensity );
	FragColor.a = 1.0;

	//FragColor.rgb = pixelGlow.rgb * pixelLightProperty.r;//luminColor.rgb*0.5;//vec3(1,1,1) * pixelGlow.a;
	//+ dot( n_cameraDir, pixelNormal.xyz );

#ifdef DEBUG_OUTPUT
	// 4X Debug Output
    if ( v2f_texcoord0.x < 0.5 && v2f_texcoord0.y < 0.5 ) {
		FragColor = clamp( texture( textureSampler0, v2f_texcoord0*2 ) * vec4(1.0, 8.0, 1.0, 1.0), 0.0, 1.0 );
	}
	else if ( v2f_texcoord0.x > 0.5 && v2f_texcoord0.y < 0.5 ) {
        //FragColor = vec4( (texture( textureSampler1, v2f_texcoord0*2 - vec2(1,0) ).rgb+1)*0.5, 1.0 );
		FragColor = clamp( texture( textureSampler4, v2f_texcoord0*2 - vec2(1,0) ).rrra, 0.0, 1.0 );
	}
	else if ( v2f_texcoord0.x < 0.5 && v2f_texcoord0.y > 0.5 ) {
        //FragColor = texture( textureSampler2, v2f_texcoord0*2 - vec2(0,1) );
        //FragColor = vec4( texture( textureSampler5, v2f_texcoord0*2 - vec2(0,1) ).rgb, 1.0 );
		/*FragColor = vec4(
            texture( textureSampler6,
                vec3(
                    v2f_texcoord0*2 - vec2(0,1),
                    fract(sys_WorldCameraPos.x / 128.0)
                )
            ).rgb, 1.0 );*/
        FragColor = vec4( pixelPosition.rgb, 1.0 );
	}
	else if ( v2f_texcoord0.x > 0.5 && v2f_texcoord0.y > 0.5 ) {
        //FragColor = texture( textureSampler2, v2f_texcoord0*2 - vec2(1,1) );
		FragColor = vec4( texture( textureSampler2, v2f_texcoord0*2 - vec2(1,1) ).rgb, 1.0 );
	}

	FragColor.a = clamp( FragColor.a , 0.0 , 1.0 );
#endif

}
