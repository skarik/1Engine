#version 430

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_control_flow_attributes : require

#include "../common.glsli"
#include "../cbuffers.glsli"
#include "../common_lighting.glsli"
#include "../common_math.glsli"
#include "../deferred_surface.glsli"

//=====================================

// Texture Inputs/Outputs
layout(r8, binding = 0, location = 20) uniform image2D textureShadowMask;
layout(binding = 1, location = 21) uniform sampler2D textureDepth;
layout(binding = 2, location = 22) uniform sampler2D textureShadowMap;
layout(binding = 3, location = 23) uniform usampler2D textureNormals;

// CBuffers
layout(binding = CBUFFER_USER0, std430) uniform sys_cbuffer_PerCamera_ShadowCam
{
    mat4 sys_ShadowViewProjectionMatrix;
	mat4 sys_ShadowViewProjectionMatrixInverse;
	mat4 sys_ShadowViewMatrix;
	mat4 sys_ShadowViewMatrixInverse;
	mat4 sys_ShadowProjectionMatrix;
	mat4 sys_ShadowProjectionMatrixInverse;
    vec4 sys_ShadowWorldCameraPos;
    vec4 sys_ShadowViewportInfo;
    vec2 sys_ShadowScreenSize;
    vec2 sys_ShadowPixelRatio;
	float sys_ShadowCameraNearPlane;
	float sys_ShadowCameraFarPlane;
};

// Structured Buffers
layout(binding = SBUFFER_USER0, std430) readonly buffer Lighting_Data
{
	rrLight Lighting_Params [];
};

//=====================================

void GetPoissonDiskCentered ( inout vec2 samples[16] )
{
	// TODO: make a better distribution
	samples[0] = vec2( 0.0f, 0.0f );
	samples[1] = vec2( 0.37938176825979075f, 0.9197961450483562f );
	samples[2] = vec2( 0.44376077488621757f, -0.8681705017844068f );
	samples[3] = vec2( -0.9770041668388132f, -0.14352717652490568f );
	samples[4] = vec2( -0.5072300050925068f, 0.8173721868214128f );
	samples[5] = vec2( 0.9705969658196754f, -0.07411190125385869f );
	samples[6] = vec2( -0.5447278385494391f, -0.7330150126693368f );
	samples[7] = vec2( 0.7924953615781184f, 0.4744373857303271f );
	samples[8] = vec2( -0.04774441730255103f, 0.528506747258193f );
	samples[9] = vec2( 0.4669766355018519f, -0.2961978681469587f );
	samples[10] = vec2( -0.6828021954665753f, 0.3004207569186437f );
	samples[11] = vec2( -0.12308177890302552f, -0.988754387788333f );
	samples[12] = vec2( -0.4287891798409814f, -0.08872259098105253f );
	samples[13] = vec2( -0.09824971501658032f, -0.4723684027780304f );
	samples[14] = vec2( 0.4029932102673596f, 0.20294659628372153f );
	samples[15] = vec2( -0.10681304298418018f, 0.9445536419747136f );
}

void GetPoissonDisk ( inout vec2 samples[16] )
{
	samples[0] = vec2( -0.7451572426064842f, 0.5965690130573613f );
	samples[1] = vec2( -0.25413562199622775f, 0.5649694749537443f );
	samples[2] = vec2( 0.0811950194344549f, 0.9878880966855352f );
	samples[3] = vec2( 0.2934032692264268f, 0.540281458026134f );
	samples[4] = vec2( 0.7624257004722369f, 0.6047629314931843f );
	samples[5] = vec2( -0.528382512974986f, 0.017211548645211475f );
	samples[6] = vec2( -0.9760839504024266f, 0.03282596767803959f );
	samples[7] = vec2( 0.09509074784161684f, 0.11531192154757579f );
	samples[8] = vec2( 0.6019533729678995f, 0.1301509661193808f );
	samples[9] = vec2( -0.8853574363497273f, -0.46040592488140675f );
	samples[10] = vec2( -0.18226921262609838f, -0.46271446524867643f );
	samples[11] = vec2( 0.40425013610644256f, -0.35170806548224487f );
	samples[12] = vec2( 0.9727771296742238f, -0.22639479503867072f );
	samples[13] = vec2( -0.36219378727350315f, -0.9288666773955668f );
	samples[14] = vec2( 0.2992998212781267f, -0.9295434259658686f );
	samples[15] = vec2( 0.6890030873181967f, -0.7045467584605423f );
}

//=====================================

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

void main ( void )
{
	ivec2 uv0 = ivec2(gl_GlobalInvocationID.xy);
	if (uv0.x >= sys_ScreenSize.x || uv0.y >= sys_ScreenSize.y)
		return; // Skip out-of-range threads

	// Read in the depth & generate world position
	vec3 unprojected_position = vec3(
		uv0.x / sys_ScreenSize.x * 2.0 - 1.0, -(uv0.y / sys_ScreenSize.y * 2.0 - 1.0),
		texelFetch(textureDepth, uv0, 0).r
		);
	vec4 undivided_position = sys_ViewProjectionMatrixInverse * vec4( unprojected_position.xyz, 1.0 );
	vec3 world_position = undivided_position.xyz / undivided_position.w;
	
	// Sample the normals now
	uvec2 gbuffer_normals = texelFetch( textureNormals, uv0, 0).xy;
	vec3 world_normal;
	DecodePackedNormals(gbuffer_normals, world_normal);
	
	const float kShadowBias = 0.001;
	//const float kShadowBiasBlend = 0.001;
	
	// At the pixel, transform world position to the shadow's matrix
	vec4 shadowprojected_position = sys_ShadowViewProjectionMatrix * vec4(world_position, 1.0);
	shadowprojected_position.xyz /= shadowprojected_position.w;
	shadowprojected_position.xy = shadowprojected_position.xy * vec2(0.5, -0.5) + vec2(0.5, 0.5);
	
	// Get reference depth for shadow map
	const float referenceLinearDepth = LinearizeZBufferDepth(shadowprojected_position.z, sys_ShadowCameraNearPlane, sys_ShadowCameraFarPlane) * sys_ShadowCameraFarPlane;
	
	// Sample the shadow map:
	
	vec2 sample_offsets [16];
	GetPoissonDiskCentered(sample_offsets);
	
	// Sample the center pixel
	float shadowCoverage = 0.0;
	
	float sampledDepth, sampledLinearDepth;
	sampledDepth = textureLod( textureShadowMap, shadowprojected_position.xy, 0 ).r;
	sampledLinearDepth = LinearizeZBufferDepth(sampledDepth, sys_ShadowCameraNearPlane, sys_ShadowCameraFarPlane) * sys_ShadowCameraFarPlane;
	
	// If pixel is too far from bias, we skip this pixel
	[[branch]]
	if ( referenceLinearDepth - sampledLinearDepth < kShadowBias )
		return;
	
	float total_sample_weight = 0.0;
	[[unroll]]
	for (int i = 1; i < 16; ++i)
	{
		float sampleWeight = 2.0 - length(sample_offsets[i]);
		
		sampledDepth = textureLod( textureShadowMap, shadowprojected_position.xy + sample_offsets[i] / 1024 * 2.0, 0 ).r;
		sampledLinearDepth = LinearizeZBufferDepth(sampledDepth, sys_ShadowCameraNearPlane, sys_ShadowCameraFarPlane) * sys_ShadowCameraFarPlane;
		
		shadowCoverage +=  (referenceLinearDepth - sampledLinearDepth > kShadowBias) ? sampleWeight : 0.0;
		total_sample_weight += sampleWeight;
		
	}
	shadowCoverage /= total_sample_weight;
	
	// Create the projected mask
	vec4 shadowMask = vec4( 1, 1, 1, 1 );
	shadowMask.r = mix(1.0, 0.0, saturate(2.0 * mix(-2.0, +1.0, shadowCoverage)));
	
	imageStore(textureShadowMask, uv0, shadowMask);
	
}