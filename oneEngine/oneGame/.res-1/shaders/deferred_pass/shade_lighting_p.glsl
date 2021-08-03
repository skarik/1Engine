#version 430

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_control_flow_attributes : require

#include "../common.glsli"
#include "shade_common_p.glsli"

#ifndef VARIANT_DEBUG_GBUFFERS
#define VARIANT_DEBUG_GBUFFERS 0
#endif

#ifndef VARIANT_DEBUG_LIGHTING
#define VARIANT_DEBUG_LIGHTING 0
#endif

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

//uniform mat4 sys_ModelViewProjectionMatrix;
//uniform mat4 sys_ModelViewProjectionMatrixInverse;
//uniform vec3 sys_CameraRange;

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
//uniform vec3 sys_WorldCameraPos;

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

void ShadePixel ( void )
{
	
	// Use depth to generate the world position
	float pixelDepth 		= texture( textureSampler4, v2f_texcoord0 ).r;
	vec4 pixelPosition = vec4( (v2f_texcoord0.x*2-1),(v2f_texcoord0.y*2-1),pixelDepth,1.0 );
	{
		pixelPosition.z = ( pixelPosition.z*2 - 1 );
        pixelPosition = inverse(sys_ViewProjectionMatrix) * vec4( pixelPosition.xyz, 1.0 );
		pixelPosition.xyzw /= pixelPosition.w;
	}

	// pixelDiffuse
	// rgb	surface diffuse
	// a	unused (used as temp discard in source)
	vec4 pixelDiffuse		= texture( textureSampler0, v2f_texcoord0 );
	// pixelNormal
	// rgb	surface normal
	// a	unused
	vec4 pixelNormal		= texture( textureSampler1, v2f_texcoord0 );
    // pixelLightProperty
    // rgb  specular color
    // a	smoothness
	vec4 pixelLightProperty	= texture( textureSampler2, v2f_texcoord0 );
	// pixelGlow
	// rgb	surface glow
	// a	rim lighting strength
	vec4 pixelGlow			= texture( textureSampler3, v2f_texcoord0 );

    // ==Perform lighting==

	vec4 n_cameraVector;
	n_cameraVector.xyz = sys_WorldCameraPos.xyz - pixelPosition.xyz;
	n_cameraVector.w = length( n_cameraVector.xyz );
	vec3 n_cameraDir = n_cameraVector.xyz / n_cameraVector.w;

    // Generate general rim-light value
	float n_rimValue = max(1.0 - dot( pixelNormal.xyz, n_cameraDir ), 0.0);


#ifdef ENABLE_LIGHTING

    vec3 specularMask = pixelLightProperty.rgb;

    vec3 diffuseColor = lighting_mix(
        pixelDiffuse.xyz,
        pixelNormal.xyz, pixelPosition.xyz, specularMask, pixelLightProperty.a
    );

    float lightingStrength = 0.0;


#else

	/*
    float lightDir = dot(vec3(-0.1,0.6,0.8), pixelNormal.rgb);
    float light = 0.3 + pow(n_rimValue, 4) * 2.5 * max(0.0, lightDir - 0.3) + max(0.0, lightDir) * 0.6;

    //vec3 diffuseColor = pixelNormal.rgb * 0.5 + 0.5;
    //vec3 diffuseColor = vec3(1,1,1) * n_rimValue * 0.5;
    vec3 diffuseColor = pixelDiffuse.rgb * max(0.0, light);
    diffuseColor.rgb += pixelDiffuse.rgb * vec3(0.3,0.25,0.25) * (0.5 + min(1.0, pow(n_rimValue, 2) * 7)) * max(0.0, -lightDir) * 2.0;

    float lightingStrength = 0.0;
	*/
	
#endif
	
	/*
	// ==Perform fog==
	float n_fogDensity = clamp( (sys_FogEnd - n_cameraVector.w) * sys_FogScale, 0, 1 );
	n_fogDensity = mix( 1, n_fogDensity, lightingStrength ); // TODO: Reimplement

	// Mix output with fog
	FragColor.rgb = mix( sys_FogColor.rgb, diffuseColor.rgb, n_fogDensity );
	FragColor.a = pixelDiffuse.a;
	*/
	
	//FragColor.rgb = pixelGlow.rgb * pixelLightProperty.r;//luminColor.rgb*0.5;//vec3(1,1,1) * pixelGlow.a;
	//+ dot( n_cameraDir, pixelNormal.xyz );

}

void main ( void )
{
#if VARIANT_DEBUG_GBUFFERS
	// 4X Debug Output
    if ( v2f_texcoord0.x < 0.5 && v2f_texcoord0.y < 0.5 ) {
		FragColor = texture( textureSampler0, v2f_texcoord0*2 );
	}
	else if ( v2f_texcoord0.x > 0.5 && v2f_texcoord0.y < 0.5 ) {
		FragColor = (texture( textureSampler1, v2f_texcoord0*2 - vec2(1,0) )+1)*0.5;
	}
	else if ( v2f_texcoord0.x < 0.5 && v2f_texcoord0.y > 0.5 ) {
		FragColor = texture( textureSampler2, v2f_texcoord0*2 - vec2(0,1) );
	}
	else if ( v2f_texcoord0.x > 0.5 && v2f_texcoord0.y > 0.5 ) {
		FragColor = texture( textureSampler3, v2f_texcoord0*2 - vec2(1,1) );
	}
	FragColor.a = clamp( FragColor.a , 0.0 , 1.0 );
#else
	// Actually shade pixel
	ShadePixel();
#endif
}
