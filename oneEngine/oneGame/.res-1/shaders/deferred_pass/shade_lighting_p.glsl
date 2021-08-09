#version 430

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_control_flow_attributes : require

#include "../common.glsli"
#include "../cbuffers.glsli"
#include "shade_common_p.glsli"

#ifndef VARIANT_DEBUG_GBUFFERS
#define VARIANT_DEBUG_GBUFFERS 0
#endif

#ifndef VARIANT_DEBUG_SURFACE
#define VARIANT_DEBUG_SURFACE 1
#endif

#ifndef VARIANT_DEBUG_LIGHTING
#define VARIANT_DEBUG_LIGHTING 0
#endif

#ifndef VARIANT_PASS_DO_INDIRECT_EMISSIVE
#define VARIANT_PASS_DO_INDIRECT_EMISSIVE 0
#endif
#ifndef VARIANT_PASS_DO_DIRECT_DIRECTIONAL
#define VARIANT_PASS_DO_DIRECT_DIRECTIONAL 0
#endif
#ifndef VARIANT_PASS_DO_DIRECT_OMNI
#define VARIANT_PASS_DO_DIRECT_OMNI 0
#endif
#ifndef VARIANT_PASS_DO_DIRECT_SPOTLIGHT
#define VARIANT_PASS_DO_DIRECT_SPOTLIGHT 0
#endif

// Previous forward rendered output
layout(binding = 5, location = 25) uniform sampler2D textureSamplerForward;

// Output to screen
layout(location = 0) out vec4 FragColor;

// Inputs from vertex shader
layout(location = 0) in vec4 v2f_position;
layout(location = 1) in vec2 v2f_texcoord0;

// Lighting and Shadows
/*layout(binding = 24, location = 44) uniform samplerBuffer textureLightBuffer;
layout(binding = 5, std140) uniform def_LightingInfo
{
	mat4 def_LightMatrix0[4];
};
layout(binding = 25, location = 45) uniform samplerBuffer textureLightMatrixBuffer;
layout(binding = 12, location = 32) uniform sampler2D textureShadow0;
layout(binding = 13, location = 33) uniform sampler2D textureShadow1;
layout(binding = 14, location = 34) uniform sampler2D textureShadow2;

vec4 v2f_lightcoord [8];*/


layout(binding = CBUFFER_USER0, std140) uniform sys_cbuffer_User0
{
	int unused;
};

// https://www.unrealengine.com/en-US/blog/physically-based-shading-on-mobile
vec3 EnvBRDFApprox(vec3 specularColor, float roughness, float ndotv)
{
	const vec4 c0 = vec4(-1, -0.0275, -0.572, 0.022);
	const vec4 c1 = vec4(1, 0.0425, 1.04, -0.04);
	vec4 r = roughness * c0 + c1;
	float a004 = min(r.x * r.x, exp2(-9.28 * ndotv)) * r.x + r.y;
	vec2 AB = vec2(-1.04, 1.04) * a004 + r.zw;
	return specularColor * AB.x + AB.y;
}

// stolen from https://www.shadertoy.com/view/4sSfzK
float VisibilityTerm(float roughness, float ndotv, float ndotl)
{
	float r2 = roughness * roughness;
	float gv = ndotl * sqrt(ndotv * (ndotv - ndotv * r2) + r2);
	float gl = ndotv * sqrt(ndotl * (ndotl - ndotl * r2) + r2);
	return 0.5 / max(gv + gl, 0.00001);
}

float DistributionTerm(float roughness, float ndoth)
{
	float r2 = roughness * roughness;
	float d = (ndoth * r2 - ndoth) * ndoth + 1.0;
	return r2 / (d * d * M_PI);
}

vec3 FresnelTerm(vec3 specularColor, float vdoth)
{
	vec3 fresnel = specularColor + (1. - specularColor) * pow((1. - vdoth), 5.);
	return fresnel;
}

void ShadePixel ( void )
{
	// Sample the GBuffer
	rrGBufferValues gbuffer;
	SampleGBuffer(gbuffer, v2f_texcoord0);
	
	// Decode the surface information from it
	rrSurfaceInfo surface;
	DecodeSurfaceInfo(surface, gbuffer, v2f_texcoord0);
	surface.normal = -surface.normal; // TODO: WHY
	
	// Skip any pixels where there is nothing to calculate
	if (surface.albedo.a <= 0.0)
		discard;
	
	const vec3 diffuseColor = mix(surface.albedo.rgb, vec3(0.0, 0.0, 0.0), surface.metalness);
	const vec3 specularColor = mix(vec3(0.02, 0.02, 0.02), surface.albedo.rgb, surface.metalness);
	const float roughness = 1.0 - surface.smoothness;
	const float roughnessExp = roughness * roughness;//pow(roughness, 2.0);
	
	// Generate camera ray by using the camera position & calculated pixel position.
	vec4 l_cameraVector;
	l_cameraVector.xyz = sys_WorldCameraPos.xyz - surface.world_position.xyz;
	l_cameraVector.w = length( l_cameraVector.xyz );
	const vec3 viewDirection = l_cameraVector.xyz / l_cameraVector.w;
	
	const vec3 reflectRay = reflect(-viewDirection, surface.normal);
	
	//vec3 halfVec = normalize(viewDirection + lightDir);
	//float vdoth = saturate(dot(viewDirection, halfVec));
	//float ndoth = saturate(dot(surface.normal, halfVec));
	const float ndotv = clamp(dot(surface.normal, viewDirection), 0.0, 1.0);
	//float ndotl = saturate(dot(normal, lightDir));
	
#if VARIANT_PASS_DO_INDIRECT_EMISSIVE
	{
		// Get rough environment to reflect
		vec3 skyColor = vec3(0.45, 0.75, 0.90);
		vec3 skyColorBottom = vec3(0.95, 0.85, 0.65);
		vec3 skyReflection = mix(skyColorBottom, skyColor, dot(surface.normal, vec3(0, 0, 1)) * 0.5 + 0.5);
		vec3 environmentColor = skyReflection;
		
		// Calculate reflection level for the environment
		vec3 environmentReflectionColor = EnvBRDFApprox(specularColor, roughnessExp, ndotv);
		
		// Calculate rough direct reflection with diffuse
		vec3 ambientColor = skyReflection * 0.1;
		
		// Sum up emittance
		vec3 emissive = surface.emissive.rgb;
		
		// Sum up the lighting
		vec3 totalLighting = environmentColor * environmentReflectionColor + ambientColor * diffuseColor + emissive;
		
		FragColor.rgb = totalLighting;
		FragColor.a = surface.albedo.a;
	}
#endif
	
#if VARIANT_PASS_DO_DIRECT_DIRECTIONAL
	{
		vec3 lightDirection = normalize(vec3(0.7, 0.2, 0.7));
		vec3 lightColor = vec3(1.0, 0.9, 0.8);

		const vec3 halfVec = normalize(viewDirection + lightDirection);
		const float vdoth = clamp(dot(viewDirection, halfVec), 0.0, 1.0);
		const float ndoth = clamp(dot(surface.normal, halfVec), 0.0, 1.0);
		//const float ndotv = clamp(dot(surface.normal, viewDirection), 0.0, 1.0);
		const float ndotl = clamp(dot(surface.normal, lightDirection), 0.0, 1.0);

		const float roughnessExpLimited = max(0.01, roughnessExp);
		
		// Calculate reflection components
		vec3 lightF = FresnelTerm(specularColor, vdoth);
		float lightD = DistributionTerm(roughnessExpLimited, ndoth);
		float lightV = VisibilityTerm(roughnessExpLimited, ndotv, ndotl);
		
		// Calculate specular
		vec3 specularLighting = lightColor * lightF * (lightD * lightV * M_PI * ndotl);
		// Caclulate diffuse
		vec3 diffuseLighting = vec3(1.0, 1.0, 1.0) * ndotl;
		
		// Sum up the lighting
		vec3 totalLighting = specularLighting + diffuseLighting * diffuseColor;

		FragColor.rgb = totalLighting;
		FragColor.a = surface.albedo.a;
	}
#endif

#ifdef ENABLE_LIGHTING

		// Use depth to generate the world position
	/*float pixelDepth 		= texture( textureSampler4, v2f_texcoord0 ).r;
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
	float n_rimValue = max(1.0 - dot( pixelNormal.xyz, n_cameraDir ), 0.0);*/
	

    /*vec3 specularMask = pixelLightProperty.rgb;

    vec3 diffuseColor = lighting_mix(
        pixelDiffuse.xyz,
        pixelNormal.xyz, pixelPosition.xyz, specularMask, pixelLightProperty.a
    );

    float lightingStrength = 0.0;*/


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
#	if !VARIANT_DEBUG_SURFACE

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
	
#	else

	rrGBufferValues gbuffer;
	SampleGBuffer(gbuffer, mod(v2f_texcoord0 * 2.0, 1.0));
	rrSurfaceInfo surface;
	DecodeSurfaceInfo(surface, gbuffer, mod(v2f_texcoord0 * 2.0, 1.0));
	
	FragColor.a = 1.0;
	
	if ( v2f_texcoord0.x < 0.5 && v2f_texcoord0.y < 0.5 )
	{
		FragColor = surface.albedo;
	}
	else if ( v2f_texcoord0.x > 0.5 && v2f_texcoord0.y < 0.5 )
	{
		FragColor.rgb = surface.normal * 0.5 + 0.5;
	}
	else if ( v2f_texcoord0.x < 0.5 && v2f_texcoord0.y > 0.5 )
	{
		FragColor.rgb = vec3(surface.metalness, surface.smoothness, 0.5);
	}
	else if ( v2f_texcoord0.x > 0.5 && v2f_texcoord0.y > 0.5 )
	{
		FragColor.rgb = surface.world_position * 0.5;
	}
	
	FragColor.a = clamp( FragColor.a , 0.0 , 1.0 );
	
#	endif
#else
	// Actually shade pixel
	ShadePixel();
#endif
}
