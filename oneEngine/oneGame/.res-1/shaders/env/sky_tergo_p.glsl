// sys/fullbright
// Renders object without lighting, only diffuse multiply blending.
#version 430

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_control_flow_attributes : require

#include "../common.glsli"
#include "../cbuffers.glsli"

// Inputs from vertex shader
// [None]

// Samplers
layout(binding = 0, location = 20) uniform samplerCube textureSampler0;
layout(binding = 1, location = 21) uniform sampler3D textureSampler1;

// Outputs
layout(location = 0) out vec4 FragDiffuse;

struct rrCloudLayer
{
	float bottom;
	float top;
	float radius;
};

struct rrRay
{
	vec3 origin;
	vec3 normal;
};

// Intersects ray r = p + td, |d| = 1, with sphere s and, if intersecting, 
// returns t value of intersection and intersection point q 
bool IntersectRaySphere(rrRay ray, vec3 sphere_origin, float sphere_radius, out float hitDistance)
{
	const vec3 origin_delta = sphere_origin - ray.origin;
	
	const float origin_delta_lenSq = dot( origin_delta, origin_delta ); 
	const float towardsRay_dot_ray = dot( origin_delta, ray.normal );
	const float towardsRay_dot_raySq = towardsRay_dot_ray * towardsRay_dot_ray;
	
	const float distance_from_edgeSq = ( sphere_radius * sphere_radius ) - origin_delta_lenSq;
	
	[[branch]]
	if ( distance_from_edgeSq < 0.0F && towardsRay_dot_ray < 0.0F )
		return false;
	
	float bSq = origin_delta_lenSq - towardsRay_dot_raySq;
	float f = sqrt( ( sphere_radius * sphere_radius ) - bSq );
	
	hitDistance = towardsRay_dot_ray + ( ( distance_from_edgeSq > 0.0 ) ? f : -f );
	
	return true;
}

float CloudLayerStratus ( in float height )
{
	return clamp((0.04 - abs(0.08 - height)) / 0.04 * 14, 0.0, 1.0);
}

float CloudLayerCumulus ( in float height )
{
	return clamp((0.2 - abs(0.25 - height)) / 0.2 * 6, 0.0, 1.0);
}

float CloudLayerCumulonimbus ( in float height )
{
	return clamp((0.45 - abs(0.5 - height)) / 0.45 * (1 - height) * 15, 0.0, 1.0);
}

float SampleCloudDensity ( in vec3 world_position, in rrCloudLayer cloud_info, in float depth, in float coverage )
{
	// TODO: fix this
	if (depth < 0 || depth > 1)
		return 0;
	
	vec3 panning_offset = vec3(sys_Time.y * 0.1, sys_Time.y * 0.2, 0);
	
	const float base_scaling = 1.3 / 10000.0;
	vec3 density_source = textureLod(textureSampler1, world_position * base_scaling + panning_offset, 0).rgb;
	float density = density_source.r * 2.0;
	
	// Get density based on the distance
	const float distanceDensifier = pow(length(world_position.xy - sys_WorldCameraPos.xy) / (cloud_info.top), 3.0);
	
	// Model the cloud
	density *= 
		// Apply a low-level cloud
		CloudLayerCumulonimbus(depth);
	
	// Apply coverage
	//density = max(0.0, density * (1.0 + coverage) - (1.0 - coverage));
	density = max(0.0, (density - coverage) * (1.0 + coverage));
	
	// Clouds are less dense at the bottom.
	density *= depth;
	
	// Thicken them up for style
	//density = clamp(density * 4.0 - 2.0, 0.0, 1.0);
	
	// Thicken them up in the distance
	density *= 1.0 + distanceDensifier;
	return density;
}

float HenyeyGreenstein ( in float g, in float cos_angle )
{
	return (1.0 - g * g) / (2.0 * pow(1.0 + g * g + 2.0 * g * cos_angle, 1.5));
}

void main ( void )
{
	// Generate the screen ray so the output is the same no matter the camera position:
	vec4 l_screenSpaceRay = vec4(
		2.0 * ((gl_FragCoord.x - sys_ViewportInfo.x) / sys_ViewportInfo.z - 0.5),
		-2.0 * ((gl_FragCoord.y - sys_ViewportInfo.y) / sys_ViewportInfo.w - 0.5),
		1.0,
		1.0);
	const vec3 l_screenRay = normalize((sys_ModelViewProjectionMatrixInverse * l_screenSpaceRay).xyz);
	const rrRay l_pixelRay = rrRay(sys_WorldCameraPos.xyz, l_screenRay);
	
	// Sample the cubemap using the ray
	vec4 skyCubemap = texture(textureSampler0, l_screenRay);
	
	// Start with the sky cubemap
	vec3 skyColor = skyCubemap.rgb;
	
	const vec3 lightDirection = normalize(vec3(0.7F, 0.2F, 0.7F));
	
	// Define static cloud layers
	const rrCloudLayer cloud_layer_0 = rrCloudLayer(1000.0, 4500.0, 20000.0);
	const rrCloudLayer cloud_layer_1 = rrCloudLayer(-1000.0, -2500.0, 20000.0);
	
	[[branch]]
	if (l_screenRay.z > 0.0)
	{
		const vec3 world_center = vec3(sys_WorldCameraPos.xy, -cloud_layer_0.radius);
		
		// Raycast against a sphere
		float bottomLayerHitDistance = 0.0;
		bool isBottomHit = IntersectRaySphere(
			l_pixelRay,
			world_center,
			cloud_layer_0.radius + cloud_layer_0.bottom,
			bottomLayerHitDistance);
		
		[[branch]]
		if (isBottomHit)
		{
			float topLayerHitDistance = bottomLayerHitDistance;
			IntersectRaySphere(
				l_pixelRay,
				world_center,
				cloud_layer_0.radius + cloud_layer_0.top,
				topLayerHitDistance);
			
			const int kMinSampleCount = 64;
			const int kMaxSampleCount = 128;
			
			// Generate the ray
			const vec3 sampleDelta = (topLayerHitDistance - bottomLayerHitDistance) * l_screenRay;
			
			// Set up initial sampling position
			vec3 samplePosition = vec3(sys_WorldCameraPos.xy + l_screenRay.xy * bottomLayerHitDistance, 0.0);
			
			// Get number of samples we want to do. Above, we do less. At horizon, we do more.
			const int sampleCount = int(mix(float(kMaxSampleCount), float(kMinSampleCount), l_screenRay.z));
			const vec3 sampleStep = sampleDelta / sampleCount;
			
			// Start with no density
			float density = 0.0;
			float lighting = 0.0;
			
			[[loop]]
			for (int i = 0; i < sampleCount; ++i)
			{
				samplePosition += sampleStep;
				
				float newDensity =
					SampleCloudDensity(
						samplePosition,
						cloud_layer_0,
						i / float(sampleCount),
						0.5
					);
					
				float newLighting = 0.0;
				// Sample in the direction of the light
				const vec3 sample_offset [6] = {
					vec3(0.16, -0.12, 0.94),
					vec3(0.72, 0.42, -0.12),
					vec3(0.0, 0.0, 0.0),
					vec3(0.0, 0.0, 0.0),
					vec3(0.0, 0.0, 0.0),
					vec3(0.0, 0.0, 0.0)
				};
				const float sample_distance [6] = {
					1.0,
					2.0,
					4.0,
					8.0,
					16.0,
					256.0
				};
				for (int light_sample = 0; light_sample < 6; ++light_sample)
				{
					const vec3 light_sample_offset =
						(cloud_layer_0.top - cloud_layer_0.bottom) / 256.0 *
						(sample_distance[light_sample] * lightDirection
						+ sample_offset[light_sample] * sample_distance[light_sample] * 0.5);
						
					float occludedDensity = 
						SampleCloudDensity(
							samplePosition + light_sample_offset,
							cloud_layer_0,
							i / float(sampleCount),
							0.5
						);
						
					newLighting += 1.0 - occludedDensity;
				}
				newLighting /= 6.0;
				
				
				// Hack density deeper
				density = newDensity + density * (1.0 - newDensity);
				//lighting = lighting * (1.0 - newDensity) + newLighting * newDensity;
				lighting = mix(lighting, newLighting, newDensity);
				if (density > 0.99)
				{
					break;
				}
			}
			
			
			vec3 cloudAlbedo = vec3(1, 1, 1);
			float cloudLighting =
				// Ambient light
				0.5
				// Beer's Law
				//exp(-density) * HenyeyGreenstein(0.5, density) * 12.0 * lighting;
				+ exp(-density * (1.0 - lighting));
			
			const vec3 cloudLight = vec3(1, 1, 1);
			const vec3 cloudShadow = vec3(0, 0, 0);
			
			vec3 cloudColor = cloudAlbedo * cloudLighting;
					
			
			skyColor =
				mix(
					skyColor,
					cloudColor,
					// Fade out at the horizon
					mix(0.0, density, clamp(l_screenRay.z * 80, 0.0, 1.0))
				);
		}
	}
	
	//FragDiffuse = vec4(l_screenRay, 1.0);
	FragDiffuse = vec4(skyColor, 1.0);
}
