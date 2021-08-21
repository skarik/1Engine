#version 430

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_control_flow_attributes : require

#include "../common.glsli"
#include "../cbuffers.glsli"
#include "../common_lighting.glsli"
#include "../deferred_surface.glsli"

//=====================================

// Texture Inputs/Outputs
layout(r8, binding = 0, location = 20) uniform image2D textureShadowMask;
layout(binding = 1, location = 21) uniform sampler2D textureDepth;
layout(binding = 2, location = 22) uniform sampler2D textureDepthDownscale16;
layout(binding = 3, location = 23) uniform usampler2D textureNormals;

// CBuffers
layout(binding = CBUFFER_USER0, std140) uniform sys_cbuffer_User0
{
	int Lighting_FirstIndex;
};

// Structured Buffers
layout(binding = SBUFFER_USER0, std430) readonly buffer Lighting_Data
{
	rrLight Lighting_Params [];
};

//=====================================

layout(local_size_x = 4, local_size_y = 4, local_size_z = 1) in;

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
	
	// Load in the current shadow mask since we want to edit the current data
	vec4 shadowMask = imageLoad(textureShadowMask, uv0);
	
	[[branch]]
	if (shadowMask.x > 0.1) // Only trace where there is no shadow
	{
		// Sample the normals now
		uvec2 gbuffer_normals = texelFetch( textureNormals, uv0, 0).xy;
		vec3 world_normal;
		DecodePackedNormals(gbuffer_normals, world_normal);
		
		// Compare with the grazing angle:
		// Generate camera ray by using the camera position & calculated pixel position.
		vec4 l_cameraVector;
		l_cameraVector.xyz = sys_WorldCameraPos.xyz - world_position.xyz;
		l_cameraVector.w = length( l_cameraVector.xyz );
		const vec3 viewDirection = l_cameraVector.xyz / l_cameraVector.w;
		// Check "fresnel" value.
		const float ndotv = dot(world_normal, viewDirection);
		
		const float kFresnelLimit = 0.10;
		[[branch]]
		if (ndotv > kFresnelLimit)
		{
			// Grab the lighting information for the current light
			const rrLight lightParams = Lighting_Params[Lighting_FirstIndex];
			const vec3 lightDirection = -lightParams.direction;
			
			float surface_lighting_estimation = dot(world_normal, lightDirection);
			
			if (surface_lighting_estimation > 0.0)
			{
				// The following stolen from https://github.com/turanszkij/WickedEngine/commit/961414186b49a66c4aec41f9f7ef7b05a330c214
				
				const float kRange = 0.5;
				const uint kSampleCount = 8;
				//const float kThickness = 0.25 / sys_CameraFarPlane;
				const float kThicknessDistanceFade = min(1.0, 10.0 / l_cameraVector.w);
				const float kThickness = 0.25 * kThicknessDistanceFade * kThicknessDistanceFade;
				const float kStepSize = kRange / kSampleCount;
				//const float kOffset = 0.5;
				
				[[branch]]
				if (kThickness > 0.01)
				{
					const vec3 rayDirection = normalize(lightDirection);
					vec3 rayPosition = world_position.xyz;// + rayDirection * kStepSize * kOffset;
					
					// Get the position in screenspace
					vec4 projected_rayPosition = sys_ViewProjectionMatrix * vec4(rayPosition + rayDirection * kRange, 1.0);
					projected_rayPosition.xyz /= projected_rayPosition.w;
					projected_rayPosition.xy = projected_rayPosition.xy * vec2(0.5, -0.5) + vec2(0.5, 0.5);
					
					// Sample downscaled buffer for smallest depth (This helps a LOT on low spec AMD)
					float ray_depth_fastcheck_real = LinearizeZBufferDepth(projected_rayPosition.z);
					float ray_depth_fastcheck_sample = LinearizeZBufferDepth(texelFetch(textureDepthDownscale16, ivec2(projected_rayPosition.xy * sys_ScreenSize.xy / 16.0), 0).x);
					float ray_depth_fastcheck_delta = ray_depth_fastcheck_real - ray_depth_fastcheck_sample;
					[[branch]]
					if (ray_depth_fastcheck_delta <= -kThickness) // Sample a second time halfway down the ray if it misses.
					{
						// Get the position in screenspace
						vec4 projected_rayPosition = sys_ViewProjectionMatrix * vec4(rayPosition + rayDirection * kRange * 0.5, 1.0);
						projected_rayPosition.xyz /= projected_rayPosition.w;
						projected_rayPosition.xy = projected_rayPosition.xy * vec2(0.5, -0.5) + vec2(0.5, 0.5);
						
						// Sample downscaled buffer for smallest depth
						float ray_depth_fastcheck_real = LinearizeZBufferDepth(projected_rayPosition.z);
						float ray_depth_fastcheck_sample = LinearizeZBufferDepth(texelFetch(textureDepthDownscale16, ivec2(projected_rayPosition.xy * sys_ScreenSize.xy / 16.0), 0).x);
						ray_depth_fastcheck_delta = ray_depth_fastcheck_real - ray_depth_fastcheck_sample;
						
						/*[[branch]]
						if (ray_depth_fastcheck_delta <= 0) // Sample a third time half again down the ray if it misses.
						{
							// Get the position in screenspace
							vec4 projected_rayPosition = sys_ViewProjectionMatrix * vec4(rayPosition + rayDirection * kRange * 0.25, 1.0);
							projected_rayPosition.xyz /= projected_rayPosition.w;
							projected_rayPosition.xy = projected_rayPosition.xy * vec2(0.5, -0.5) + vec2(0.5, 0.5);
							
							// Sample downscaled buffer for smallest depth
							float ray_depth_fastcheck_real = LinearizeZBufferDepth(projected_rayPosition.z);
							float ray_depth_fastcheck_sample = LinearizeZBufferDepth(texelFetch(textureDepthDownscale16, ivec2(projected_rayPosition.xy * sys_ScreenSize.xy / 16.0), 0).x);
							ray_depth_fastcheck_delta = ray_depth_fastcheck_real - ray_depth_fastcheck_sample;
						}*/
					}
					
					[[branch]]
					if (ray_depth_fastcheck_delta > -kThickness) // If the fast check was OK, then we do the expensive check. 
					{
						[[loop]]
						for (uint i = 0; i < kSampleCount; ++i)
						{
							// Get new position in world space to check.
							rayPosition += rayDirection * kStepSize;
							
							// Get the position in screenspace
							vec4 projected_rayPosition = sys_ViewProjectionMatrix * vec4(rayPosition, 1.0);
							projected_rayPosition.xyz /= projected_rayPosition.w;
							projected_rayPosition.xy = projected_rayPosition.xy * vec2(0.5, -0.5) + vec2(0.5, 0.5);
							
							// Check the depth of this ray so far
							const float ray_depth_real = LinearizeZBufferDepth(projected_rayPosition.z);
							const float ray_depth_sample = LinearizeZBufferDepth(texelFetch(textureDepth, ivec2(projected_rayPosition.xy * sys_ScreenSize.xy), 0).x);
							const float ray_depth_delta = ray_depth_real - ray_depth_sample;
							
							[[branch]]
							if (ray_depth_delta > 0 && ray_depth_delta < kThickness)
							{
								shadowMask.r = min(
									// Don't lighten shadows
									shadowMask.r, 
									mix(1.0,
										// Feather out the final 25%
										clamp(i * 4.0 / (kSampleCount - 1.0) - 3.0, 0.0, 1.0),
										// Fade in over fresnel limit value
										clamp((ndotv - kFresnelLimit) * 6.0, 0.0, 1.0)
										)
									);
								
								imageStore(textureShadowMask, uv0, shadowMask);
								
								break;
							}
						}
					}
				} // End thickness-distance fade check
			}
			// End surface lighting estimation
		}
		// End grazing angle check
	}
	// End out-of-shadow check
}