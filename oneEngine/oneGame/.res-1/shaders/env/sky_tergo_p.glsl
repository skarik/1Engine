// sys/fullbright
// Renders object without lighting, only diffuse multiply blending.
#version 430

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_control_flow_attributes : require

#include "../common.glsli"
#include "../cbuffers.glsli"
#include "../common_math.glsli"

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

float SampleCloudDensity ( in vec3 world_position, in rrCloudLayer cloud_info, in float height, in float coverage )
{
	// TODO: fix this
	if (height < 0 || height > 1)
		return 0;
	
	vec3 panning_offset = vec3(sys_Time.y * 0.1, sys_Time.y * 0.2, 0) * 0.5;
	
	const float base_scaling = 1.3 / 10000.0;
	vec3 density_source = textureLod(textureSampler1, world_position * base_scaling + panning_offset, 0).rgb;
	float density = density_source.r * 2.0; // hack THICCER
	
	// Get density based on the distance
	//const float distanceDensifier = pow(length(world_position.xy - sys_WorldCameraPos.xy) / (cloud_info.top), 3.0);
	const float distanceDensifier = 0.0;
	
	// Model the cloud
	density *= 
		// Apply a low-level cloud
		CloudLayerCumulonimbus(height);
	
	// Apply coverage
	//density = max(0.0, density * (1.0 + coverage) - (1.0 - coverage));
	density = max(0.0, (density - coverage) * (1.0 + coverage));
	
	// Clouds are less dense at the bottom.
	density *= saturate(height * 5.0);
	
	// Thicken them up for style
	//density = clamp(density * 4.0 - 2.0, 0.0, 1.0);
	
	// Thicken them up in the distance
	density *= 1.0 + distanceDensifier;
	return density;
}

float HenyeyGreenstein ( in float g, in float cos_angle )
{
	float denominator = max(0.0, 1.0 + g * g + 2.0 * g * cos_angle);
	denominator *= sqrt( denominator );
	return (1.0 - g * g) / (4.0 * M_PI * denominator);
}

float GetCloudOcclusionAtPoint ( in vec3 base_sample_point, in vec3 cone_direction, in rrCloudLayer cloud_info )
{
	// Sample in the direction of the light
	const vec3 sample_offset [6] = {
		vec3( -0.4383571830936342f, 0.5220059888450893f, -0.6210293836116813f ),
		vec3( 0.645078911159678f, -0.16886083129524965f, -0.6215859176476752f ),
		vec3( 0.47347633867056343f, 0.7029367735909412f, 0.4028391197688177f ),
		vec3( -0.4524931547352832f, -0.6949186255019494f, -0.45661626007276584f ),
		vec3( -0.6942663333097578f, 0.14650800506244402f, 0.6394775843692075f ),
		vec3( 0.38439149674154544f, -0.7207124972496046f, 0.529811642626739f ),
	};
	const float sample_distance [6] = {
		10.0,
		20.0,
		40.0,
		60.0,
		100.0,
		250.0
	};
	
	float occlusion = 0.0;
	
	[[loop]]
	for (int occlude_sample_index = 0; occlude_sample_index < 6; ++occlude_sample_index)
	{
		const float sample_lengthen_factor = 0.5 * (cloud_info.top - cloud_info.bottom) / sample_distance[5];
		const vec3 light_sample_offset =
			sample_lengthen_factor * sample_distance[occlude_sample_index] *
			normalize( cone_direction + sample_offset[occlude_sample_index] * 0.4 );
			
		// Get actual sample point within the volume
		const vec3 next_sample_point = base_sample_point + light_sample_offset;
		
		// Sample the clouds
		float occludedDensity = 
			SampleCloudDensity(
				next_sample_point,
				cloud_info,
				// This approximation is good enough
				next_sample_point.z / (cloud_info.top - cloud_info.bottom),
				0.5
			);
			
		occlusion += occludedDensity;
		[[branch]]
		if (occlusion > 0.99)
		{
			break;
		}
	}
	
	return occlusion;
}

vec3 SampleStar ( in vec3 view_ray, in vec3 position, in vec3 color, in float intensity, in float size, in float glare_size )
{
	float v_dot_star = dot(view_ray, position);
	
	const float star_angle = 1.0 - size;
	const float glare_angle = 1.0 - (size + glare_size);
	
	[[branch]]
	if (v_dot_star > glare_angle)
	{
		return 
			v_dot_star > star_angle 
			? color * intensity
			: ((v_dot_star - glare_angle) / (star_angle - glare_angle)) * color * 0.5;
	}
	
	return vec3(0, 0, 0);
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
	
	// Add a sun
	skyColor += SampleStar(l_screenRay, lightDirection, vec3(1.0, 0.78, 0.68), 20.0, 0.005, 0.05);
	
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
				
				const float parametric_cloud_height = i / float(sampleCount);
				
				float newDensity =
					SampleCloudDensity(
						samplePosition,
						cloud_layer_0,
						parametric_cloud_height,
						0.5
					);
				
				// Sample occlusion in another direction
				[[branch]]
				if (newDensity > 0.0)
				{
					float occlusion =
						GetCloudOcclusionAtPoint(
							samplePosition,
							lightDirection,
							cloud_layer_0
						);
					
					// Self-shadowing (bit of a hack)
					float shadow_coeff = exp(-occlusion * 2.0);
					
					// Beers coeff to make edges of clouds brighter
					float beers_coeff = exp(-newDensity);
					// Beers coeff modded with light occlusion
					//float beers_coeff = exp(-newDensity * (1.0 - occlusion));
					
					// Powder (ala HZD) darkens wispy edges to give details within clouds.
					// Looks great even on low-res clouds, giving more of the "outline" for the style.
					float powder_coeff = 1.0 - exp(-newDensity * 2.0);
					
					float newLighting = beers_coeff * powder_coeff * HenyeyGreenstein(0.5, -dot(l_screenRay, lightDirection)) * shadow_coeff;
					
					// Attenuate previous lighting but add new lighting
					//lighting = lighting * exp(-newDensity * (1.0 - occlusion) * 0.5) + newLighting;
					lighting += newLighting;
				}
				
				// Hack density deeper
				density = newDensity + density * (1.0 - newDensity);
				
				[[branch]]
				if (density > 0.99)
				{
					break;
				}
			}
			
			[[branch]]
			if (density > 0.0)
			{
				//float lighting = clamp(1.0 - lightingOcclusion, 0.0, 1.0);
				//newLighting = clamp(1.0 - lightingOcclusion, 0.0, 1.0);
				// Blend in new lighting
				//lighting = mix(lighting, newLighting, newDensity);
				
				// Sample the sky in order to do distance reflections
				vec3 cloudReflection = texture(textureSampler0, vec3(l_screenRay.xy, -l_screenRay.z)).rgb;
				
				
				vec3 cloudAlbedo = vec3(1, 1, 1);
				float cloudLighting = lighting;
					// Ambient light
					/*0.2
					// Beer's Law
					//+ exp(-density)
					//exp(-density) * HenyeyGreenstein(0.5, density) * 12.0 * lighting;
					+ lighting;*/
				
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
	}
	
	//FragDiffuse = vec4(l_screenRay, 1.0);
	FragDiffuse = vec4(skyColor, 1.0);
}
