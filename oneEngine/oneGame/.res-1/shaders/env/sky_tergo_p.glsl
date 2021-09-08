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
	return clamp((0.2 - abs(0.25 - height)) / 0.2 * (1 - height) * 8, 0.0, 1.0);
}

float CloudLayerCumulonimbus ( in float height )
{
	//return clamp((0.45 - abs(0.5 - height)) / 0.45 * (1 - height) * 9, 0.0, 1.0);
	return saturate((0.45 - abs(0.5 - height)) / 0.45 * saturate((1 - height) * 5) * 5);
}

float SampleCloudDensity ( in vec3 world_position, in rrCloudLayer cloud_info, in float height, in float coverage, in float shape )
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
		mix(mix(CloudLayerStratus(height),
				CloudLayerCumulus(height),
				saturate(shape * 2.0)),
			CloudLayerCumulonimbus(height),
			saturate(shape * 2.0 - 1.0));
	
	// Apply coverage
	//density = max(0.0, density * (1.0 + coverage) - (1.0 - coverage));
	//density = saturate((density - (1.0 - coverage)) * (1.0 + coverage) + max(0.0, coverage - 0.8) / 0.5);
	density = max(0.0, (density - (1.0 - coverage)) * (1.0 + coverage));
	
	// Clouds are less dense at the bottom.
	density *= saturate(height * 3.0);
	
	// Thicken them up for style
	//density = clamp(density * 4.0 - 2.0, 0.0, 1.0);
	
	// Thicken them up in the distance
	density *= 1.0 + distanceDensifier;
	return saturate(density);
}

float HenyeyGreenstein ( in float g, in float cos_angle )
{
	float denominator = max(0.0, 1.0 + g * g + 2.0 * g * cos_angle);
	denominator *= sqrt( denominator );
	return (1.0 - g * g) / (4.0 * M_PI * denominator);
}

float GetCloudOcclusionAtPoint ( in vec3 base_sample_point, in vec3 cone_direction, in rrCloudLayer cloud_info, in float coverage, in float shape, in uint shuffle )
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
		5.0,
		10.0,
		15.0,
		20.0,
		25.0,
		250.0
	};
	
	float occlusion = 0.0;
	
	[[loop]]
	for (uint occlude_sample_index = 0; occlude_sample_index < 6; ++occlude_sample_index)
	{
		//const uint shuffled_index = (occlude_sample_index + shuffle + uint(sys_FrameIndex)) % 6;
		const uint shuffled_index = (occlude_sample_index + shuffle) % 6;
		
		const float sample_lengthen_factor = 0.5 * abs(cloud_info.top - cloud_info.bottom) / sample_distance[5];
		const vec3 light_sample_offset =
			sample_lengthen_factor * sample_distance[occlude_sample_index] *
			( cone_direction + sample_offset[shuffled_index] * 0.5 );
			//normalize( cone_direction + sample_offset[shuffled_index] * 0.75 );
			
		// Get actual sample point within the volume
		const vec3 next_sample_point = base_sample_point + light_sample_offset;
		
		// Sample the clouds
		float occludedDensity = 
			SampleCloudDensity(
				next_sample_point,
				cloud_info,
				// This approximation is good enough
				next_sample_point.z / abs(cloud_info.top - cloud_info.bottom),
				coverage,
				shape
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

vec3 TraceClouds (
	in rrRay ray,
	in rrCloudLayer cloudLayer,
	in vec3 lightDirection,
	in vec3 lightColor,
	in vec3 ambientLight,
	in float coverage,
	in float shape,
	in float absorption,
	out float blend )
{
	[[branch]]
	if (sign(ray.normal.z * cloudLayer.top) > 0.0)
	{
		const vec3 world_center = vec3(sys_WorldCameraPos.xy, -cloudLayer.radius);
		
		// Raycast against a sphere
		float bottomLayerHitDistance = 0.0;
		bool isBottomHit = IntersectRaySphere(
			ray,
			world_center,
			cloudLayer.radius + cloudLayer.bottom,
			bottomLayerHitDistance);
		
		[[branch]]
		if (isBottomHit)
		{
			float topLayerHitDistance = bottomLayerHitDistance;
			IntersectRaySphere(
				ray,
				world_center,
				cloudLayer.radius + cloudLayer.top,
				topLayerHitDistance);
			
			const int kMinSampleCount = 64;
			const int kMaxSampleCount = 128;
			
			// Generate the ray
			const vec3 sampleDelta = abs(topLayerHitDistance - bottomLayerHitDistance) * ray.normal;
			
			// Set up initial sampling position
			vec3 samplePosition = vec3(
				sys_WorldCameraPos.xy + ray.normal.xy * min(topLayerHitDistance, bottomLayerHitDistance),
				(ray.normal.z > 0.0) ? 0.0 : abs(cloudLayer.top - cloudLayer.bottom)
				);
			
			// Get number of samples we want to do. Above, we do less. At horizon, we do more.
			const int sampleCount = int(mix(float(kMaxSampleCount), float(kMinSampleCount), abs(ray.normal.z)));
			const vec3 sampleStep = sampleDelta / sampleCount;
			
			// Grab sample step length
			const float sampleStepLength = length(sampleStep);
			
			// Start with no density
			const float absorption_multiplier = 0.05 * absorption;
			
			float transmittance = 1.0;
			vec3 lighting = vec3(0.0, 0.0, 0.0);
			
			[[loop]]
			for (uint i = 0; i < sampleCount; ++i)
			{
				const float parametric_cloud_height = 
					(ray.normal.z > 0.0)
					? (i / float(sampleCount))
					: (1.0 - i / float(sampleCount));
				
				float newDensity =
					SampleCloudDensity(
						samplePosition,
						cloudLayer,
						parametric_cloud_height,
						coverage,
						shape
					);
				
				// Sample occlusion in another direction
				[[branch]]
				if (newDensity > 0.0)
				{
					float occlusion =
						GetCloudOcclusionAtPoint(
							samplePosition,
							lightDirection,
							cloudLayer,
							coverage,
							shape,
							i
						);
					
					// Self-shadowing (bit of a hack)
					float shadow_coeff = 1.0; //exp(-occlusion);
					
					// Beers coeff to make edges of clouds brighter
					float beers_coeff = exp(-newDensity * sampleStepLength * absorption_multiplier);
					// Powder (ala HZD) darkens wispy edges to give details within clouds.
					// Looks great even on low-res clouds, giving more of the "outline" for the style.
					float powder_coeff = 1.0 - exp(-newDensity * sampleStepLength * absorption_multiplier * 2.0);
					
					// Start with light color
					vec3 in_lighting = lightColor;

					// Apply the scattering & shadow coeff
					in_lighting *= HenyeyGreenstein(0.5, -dot(ray.normal, lightDirection)) * shadow_coeff;
					// Bump up the current scattering level
					in_lighting *= 2.0;
					// Add ambient light
					in_lighting += mix(0.1, 0.5, saturate(parametric_cloud_height)) * ambientLight;
					
					// Normalize lighting to the density
					in_lighting *= newDensity;
					{
						// Clamp lighting
						in_lighting = saturate(in_lighting);
						// Apply beers-powder (yum)
						in_lighting *= beers_coeff * powder_coeff * 3.0;
					}
					// Denormalize lighting
					in_lighting /= max(newDensity, 0.001);

					// Acculmulate lighting
					lighting += in_lighting * transmittance;
					// Stop transmittance now
					transmittance *= beers_coeff;
				}
				
				[[branch]]
				if (transmittance < 0.01)
				{
					break;
				}
				
				// Step the sampling forward
				samplePosition += sampleStep;
			}
			
			[[branch]]
			if (transmittance < 1.0)
			{
				// Sample the sky in order to do distance reflections
				vec3 cloudReflection = texture(textureSampler0, vec3(ray.normal.xy, -ray.normal.z)).rgb;
				
				vec3 cloudAlbedo = vec3(1, 1, 1);
				vec3 cloudLighting = lighting;
				
				// Calculate grazing angle for far distances
				float normal_blend = dot(normalize(world_center - (bottomLayerHitDistance * ray.normal + ray.origin)), ray.normal);
				float normal_blend_power = 1.0 - pow(1.0 - abs(normal_blend), 9.0);
				
				vec3 cloudColor = cloudAlbedo * cloudLighting;
				// Mix in the reflection at the far distance
				cloudColor = mix(cloudColor, cloudReflection, max(1.0 - normal_blend_power, pow(1.0 - abs(ray.normal.z), 5.0)));
				
				// Fade out at the horizon
				blend = mix(0.0, saturate(1.02 - transmittance * 1.02), clamp(abs(ray.normal.z) * 80, 0.0, 1.0));
				// Fade out at the grazing angle
				blend = mix(0.0, blend, normal_blend_power);
				
				return cloudColor;
			}
		}
	}
	
	blend = 0.0;
	return vec3(0, 0, 0);
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
	vec4 l_screenSpaceRayTransformed = sys_ViewProjectionMatrixInverse * l_screenSpaceRay;
	const vec3 l_screenRay = normalize(l_screenSpaceRayTransformed.xyz / l_screenSpaceRayTransformed.w);
	const rrRay l_pixelRay = rrRay(sys_WorldCameraPos.xyz, l_screenRay);
	
	// Sample the cubemap using the ray
	vec4 skyCubemap = texture(textureSampler0, l_screenRay);
	
	// Start with the sky cubemap
	vec3 skyColor = skyCubemap.rgb;
	
	const vec3 lightDirection = normalize(vec3(0.7F, 0.2F, 0.7F));
	const vec3 lightColor = vec3(1.1, 1.0, 0.9) * 0.6;
	const vec3 ambientLight = vec3(0.45, 0.75, 0.90) * 0.75;
	
	// Add a sun
	skyColor += SampleStar(l_screenRay, lightDirection, vec3(1.0, 0.78, 0.68), 20.0, 0.005, 0.05);
	
	// Define static cloud layers
	const rrCloudLayer cloud_layer_0 = rrCloudLayer(1000.0, 4500.0, 80000.0);
	//const rrCloudLayer cloud_layer_1 = rrCloudLayer(-1000.0, -2500.0, 20000.0);
	const rrCloudLayer cloud_layer_1 = rrCloudLayer(-4000.0, -500.0, 400000.0);
	
	const float coverage = 0.5;
	const float shape = 0.1;
	
	//float cloud_blend_0 = 0.0;
	//vec3 cloud_color_0 = TraceClouds(l_pixelRay, cloud_layer_0, lightDirection, lightColor, ambientLight, coverage, shape, 1.0, cloud_blend_0);
	//skyColor = mix(skyColor, cloud_color_0, cloud_blend_0);
	
	//float cloud_blend_1 = 0.0;
	//vec3 cloud_color_1 = TraceClouds(l_pixelRay, cloud_layer_1, lightDirection, lightColor, ambientLight, 1.0, 0.7, 1.3, cloud_blend_1);
	//skyColor = mix(skyColor, cloud_color_1, cloud_blend_1);
	
	//FragDiffuse = vec4(l_screenRay, 1.0);
	FragDiffuse = vec4(skyColor, 1.0);
}
