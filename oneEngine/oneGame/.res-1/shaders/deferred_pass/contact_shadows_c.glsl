#version 430

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_control_flow_attributes : require

#include "../common.glsli"
#include "../cbuffers.glsli"
#include "../common_lighting.glsli"

//=====================================

// Texture Inputs/Outputs
layout(r8, binding = 0, location = 20) uniform image2D textureShadowMask;
layout(binding = 1, location = 21) uniform sampler2D textureDepth;

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

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1)

void main ( void )
{
	ivec2 uv0 = ivec2(gl_GlobalInvocationID.xy);
	
	// Read in the depth & generate world position
	vec3 unprojected_position = vec3(
		(uv0.xy / sys_ScreenSize.xy) * 2.0 - vec2(1.0, 1.0),
		texelFetch(textureDepth, uv0, 0).r
		);
	vec4 undivided_position = sys_ViewProjectionMatrixInverse * vec4( unprojected_position.xyz, 1.0 );
	vec3 world_position = undivided_position.xyz / undivided_position.w;
	
	// Load in the current shadow mask since we want to edit the current data
	vec4 shadowMask = imageLoad(textureShadowMask, uv0);
	
	// TODO: Also sample normals. For grazing angles, we can skip surfaces.
	
	[[branch]]
	if (shadowMask.x > 0.1) // Only trace where there is no shadow
	{
		const rrLight lightParams = Lighting_Params[Lighting_FirstIndex];
		const vec3 lightDirection = -lightParams.direction;
		
		// The following stolen from https://github.com/turanszkij/WickedEngine/commit/961414186b49a66c4aec41f9f7ef7b05a330c214
		
		const float kRange = 0.25;
		const uint kSampleCount = 8;
		const float kThickness = 0.25 / sys_CameraFarPlane;
		const float kStepSize = kRange / kSampleCount;
		const float kOffset = 0.0;
		
		const vec3 rayDirection = normalize(mat3(sys_ViewMatrix) * lightDirection) * vec3(1, -1, 1);
		vec3 rayPosition = (sys_ViewMatrix * vec4(world_position, 1.0)).xyz + rayDirection * kStepSize * kOffset;
		
		// TODO: Replace this loop with something that's not as terrible. could probably do a splitting search...
		[[loop]]
		for (uint i = 0; i < kSampleCount; ++i)
		{
			// Get new position in world space to check.
			rayPosition += rayDirection * kStepSize;
			
			// Get the position in screenspace
			vec4 projected_rayPosition = sys_ProjectionMatrix * vec4(rayPosition, 1.0);
			projected_rayPosition.xyz /= projected_rayPosition.w;
			projected_rayPosition.xy = projected_rayPosition.xy * vec2(0.5, 0.5) + vec2(0.5, 0.5);
			
			// Check the depth of this ray so far
			const float ray_depth_real = projected_rayPosition.z;
			const float ray_depth_sample = texelFetch(textureDepth, ivec2(projected_rayPosition.xy * sys_ScreenSize.xy), 0).x;
			const float ray_depth_delta = ray_depth_real - ray_depth_sample;
			
			if (ray_depth_delta > 0 && ray_depth_delta < kThickness)
			{
				shadowMask = vec4(0.0, 0.0, 0.0, 1.0);
				imageStore(textureShadowMask, uv0, shadowMask);
				
				break;
			}
			
			/*if (i == 2)
			{
				shadowMask = vec4(1.0 - max(0.0, ray_depth_delta * sys_CameraFarPlane), 0.0, 0.0, 1.0);
				imageStore(textureShadowMask, uv0, shadowMask);
				
				break;
			}*/
		}
	}
	
	
	/*[[branch]]
	if (shadowMask.x < 0.5)
	{
		// Clear to bright
		if (uv0.x < 100)
			shadowMask = vec4(1.0, 0.0, 0.0, 1.0);
		else
			shadowMask = vec4(0.5, 0.0, 0.0, 1.0);
		
		imageStore(textureShadowMask, uv0, shadowMask);
	}*/
}