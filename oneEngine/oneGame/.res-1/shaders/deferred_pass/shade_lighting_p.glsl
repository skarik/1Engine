#version 430

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_control_flow_attributes : require

#include "../common.glsli"
#include "../cbuffers.glsli"
#include "../common_lighting.glsli"
#include "shade_common_p.glsli"

#ifndef VARIANT_PASS
#define VARIANT_PASS 0
#endif

#ifndef VARIANT_STYLE
#define VARIANT_STYLE 0
#endif

#define VARIANT_PASS_DO_INDIRECT_EMISSIVE 1
#define VARIANT_PASS_DO_DIRECT_DIRECTIONAL 2
#define VARIANT_PASS_DO_DIRECT_OMNI 3
#define VARIANT_PASS_DO_DIRECT_SPOTLIGHT 4
#define VARIANT_PASS_DEBUG_GBUFFERS 10
#define VARIANT_PASS_DEBUG_SURFACE 11
#define VARIANT_PASS_DEBUG_LIGHTING 12

#define VARIANT_STYLE_NORMAL 1
#define VARIANT_STYLE_CELSHADED 2

// Previous forward rendered output
layout(binding = 5, location = 25) uniform sampler2D textureSamplerForward;
layout(binding = 6, location = 26) uniform sampler2D textureSamplerShadowMask;

// Output to screen
layout(location = 0) out vec4 FragColor;

// Inputs from vertex shader
layout(location = 0) in vec4 v2f_position;
layout(location = 1) in vec2 v2f_texcoord0;
layout(location = 2) in flat int v2f_lightIndex;

// Lighting Parameters:
layout(binding = SBUFFER_USER0, std430) readonly buffer Lighting_Data
{
	rrLight Lighting_Params [];
};

#if VARIANT_PASS==VARIANT_PASS_DO_DIRECT_DIRECTIONAL

layout(binding = CBUFFER_USER0, std140) uniform sys_cbuffer_User0
{
	int Lighting_FirstIndex;
};

#else
layout(binding = CBUFFER_USER0, std140) uniform sys_cbuffer_User0
{
	int unused;
};
#endif

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

#if VARIANT_PASS==VARIANT_PASS_DO_INDIRECT_EMISSIVE

	{
		const float ndotv = clamp(dot(surface.normal, viewDirection), 0.0, 1.0);
		
		// Get rough environment to reflect
		vec3 skyColor = vec3(0.45, 0.75, 0.90);
		vec3 skyColorBottom = vec3(0.95, 0.85, 0.65);
		vec3 skyReflection = mix(skyColorBottom, skyColor, dot(surface.normal, vec3(0, 0, 1)) * 0.5 + 0.5);
		vec3 environmentColor = skyReflection;
		
		// Calculate reflection level for the environment
		vec3 environmentReflectionColor = EnvBRDFApprox(specularColor, roughnessExp, ndotv);
		
		// Calculate rough direct reflection with diffuse
		vec3 ambientColor = skyReflection * 0.70;
		
		// Sum up emittance
		vec3 emissive = surface.emissive.rgb;
		
		// Sum up the lighting
		vec3 totalLighting = environmentColor * environmentReflectionColor + ambientColor * diffuseColor + emissive;
		
		FragColor.rgb = totalLighting;
		FragColor.a = surface.albedo.a;
	}

#elif VARIANT_PASS==VARIANT_PASS_DO_DIRECT_DIRECTIONAL

	{
		const rrLight lightParams = Lighting_Params[v2f_lightIndex];
		const vec3 lightDirection = -lightParams.direction;
		
		const vec3 halfVec = normalize(viewDirection + lightDirection);
		const float vdoth = clamp(dot(viewDirection, halfVec), 0.0, 1.0);
		const float ndoth = clamp(dot(surface.normal, halfVec), 0.0, 1.0);
		const float ndotv = clamp(dot(surface.normal, viewDirection), 0.0, 1.0);
		const float ndotl = clamp(dot(surface.normal, lightDirection), 0.0, 1.0);
		
		// Add backface-lighting
		[[branch]]
		if (surface.shade_model == kShadeModelThinFoliage)
		{
			vec3 surfaceReflectance = diffuseColor;
			
			#if VARIANT_STYLE==VARIANT_STYLE_NORMAL
			if (!surface.is_frontface)
			{
				surfaceReflectance = mix(surfaceReflectance * 0.7, surfaceReflectance * surfaceReflectance, 0.5);
			}
			#endif
			
			const vec3 lightColor = lightParams.color;
			
			float shadowMask = 1.0;
			// TODO: How to get shadows to not look terrible?
			//[[branch]]
			//if (rrLightGetShadows(lightParams) != 0)
			//{
			//	shadowMask = texture(textureSamplerShadowMask, v2f_texcoord0).r;
			//}

			const float roughnessExpLimited = max(0.01, roughnessExp);
			
			// Calculate reflection components
			vec3 lightF = FresnelTerm(specularColor, vdoth);
			float lightD = DistributionTerm(roughnessExpLimited, ndoth);
			float lightV = VisibilityTerm(roughnessExpLimited, ndotv, ndotl);
			
			// Calculate specular
			vec3 specularLighting = lightColor * lightF * (lightD * lightV * M_PI * ndotl);
			// Caclulate diffuse
			#if VARIANT_STYLE==VARIANT_STYLE_NORMAL
				vec3 diffuseLighting = lightColor * (surface.is_frontface ? ndotl : -ndotl);
			#elif VARIANT_STYLE==VARIANT_STYLE_CELSHADED
				vec3 diffuseLighting = lightColor * mix(0.5, 1.0, clamp(ndotl * 20.0, 0.0, 1.0));
			#endif
			
			// Sum up the lighting
			vec3 totalLighting = (specularLighting + diffuseLighting * surfaceReflectance) * shadowMask;

			FragColor.rgb = totalLighting;
			FragColor.a = surface.albedo.a;
		}
		else
		{
			const vec3 surfaceReflectance = diffuseColor;
			
			const vec3 lightColor = lightParams.color;
			
			float shadowMask = 1.0;
			[[branch]]
			if (rrLightGetShadows(lightParams) != 0)
			{
				shadowMask = texture(textureSamplerShadowMask, v2f_texcoord0).r;
			}

			const float roughnessExpLimited = max(0.01, roughnessExp);
			
			// Calculate reflection components
			vec3 lightF = FresnelTerm(specularColor, vdoth);
			float lightD = DistributionTerm(roughnessExpLimited, ndoth);
			float lightV = VisibilityTerm(roughnessExpLimited, ndotv, ndotl);
			
			// Calculate specular
			vec3 specularLighting = lightColor * lightF * (lightD * lightV * M_PI * ndotl);
			// Caclulate diffuse
			#if VARIANT_STYLE==VARIANT_STYLE_NORMAL
				vec3 diffuseLighting = lightColor * ndotl;
			#elif VARIANT_STYLE==VARIANT_STYLE_CELSHADED
				vec3 diffuseLighting = lightColor * mix(0.0, 1.0, clamp(ndotl * 20.0, 0.0, 1.0));
			#endif
			
			// Sum up the lighting
			vec3 totalLighting = (specularLighting + diffuseLighting * surfaceReflectance) * shadowMask;

			FragColor.rgb = totalLighting;
			FragColor.a = surface.albedo.a;
		}
	}
	
#endif
}

void main ( void )
{
#if VARIANT_PASS==VARIANT_PASS_DEBUG_GBUFFERS

	// 4X Debug Output
    if ( v2f_texcoord0.x < 0.5 && v2f_texcoord0.y < 0.5 ) {
		FragColor = texture( textureSampler0, v2f_texcoord0*2 );
	}
	else if ( v2f_texcoord0.x > 0.5 && v2f_texcoord0.y < 0.5 ) {
		const vec2 uv0 = v2f_texcoord0*2 - vec2(1,0);
		const uvec2 normalBits = texelFetch( textureSampler1, ivec2(uv0.x * sys_ScreenSize.x, uv0.y * sys_ScreenSize.y), 0).xy;
		FragColor = vec4(
						DecodeNormalComponent(normalBits.x >> 16),
						DecodeNormalComponent(normalBits.x & 0xFFFF),
						DecodeNormalComponent(normalBits.y >> 16),
						1.0)
						* 0.5 + 0.5;
	}
	else if ( v2f_texcoord0.x < 0.5 && v2f_texcoord0.y > 0.5 ) {
		FragColor = texture( textureSampler2, v2f_texcoord0*2 - vec2(0,1) );
	}
	else if ( v2f_texcoord0.x > 0.5 && v2f_texcoord0.y > 0.5 ) {
		FragColor = texture( textureSampler3, v2f_texcoord0*2 - vec2(1,1) );
	}
	
#elif VARIANT_PASS==VARIANT_PASS_DEBUG_SURFACE

	const vec2 debugUV = mod(v2f_texcoord0 * 2.0, 1.0);

	rrGBufferValues gbuffer;
	SampleGBuffer(gbuffer, debugUV);
	rrSurfaceInfo surface;
	DecodeSurfaceInfo(surface, gbuffer, debugUV);
	
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
		FragColor.rgb = vec3(surface.metalness, surface.smoothness, surface.inverse_occlusion);
	}
	else if ( v2f_texcoord0.x > 0.5 && v2f_texcoord0.y > 0.5 )
	{
		FragColor.rgb = surface.world_position * 0.5;
	}
	
	FragColor.a = clamp( FragColor.a , 0.0 , 1.0 );
	
#else
	
	// Actually shade pixel
	ShadePixel();
	
#endif
}
