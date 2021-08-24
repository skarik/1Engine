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
bool IntersectRaySphere(rrRay ray, vec3 sphere_origin, float sphere_radius, out float hitDistance)// out vec3 hitPoint) 
{
	const vec3 origin_delta = sphere_origin - ray.origin;
	
	const float origin_delta_lenSq = dot( origin_delta, origin_delta ); 
	const float towardsRay_dot_ray = dot( origin_delta, ray.normal );
	const float towardsRay_dot_raySq = towardsRay_dot_ray * towardsRay_dot_ray;
	
	const float distance_from_edgeSq = ( sphere_radius * sphere_radius ) - origin_delta_lenSq;
	
	[[branch]]
	//if ( distance_from_edgeSq + towardsRay_dot_raySq < 0.0F )
	if ( distance_from_edgeSq < 0.0F && towardsRay_dot_ray < 0.0F )
		return false;
	
	float bSq = origin_delta_lenSq - towardsRay_dot_raySq;
	float f = sqrt( ( sphere_radius * sphere_radius ) - bSq );
	
	hitDistance = towardsRay_dot_ray + ( ( distance_from_edgeSq > 0.0 ) ? f : -f );
	
	return true;
	
	/*vec3 origin_delta = ray.origin - sphere_origin; 
	float b = dot(origin_delta, ray.normal); 
	float c = dot(origin_delta, origin_delta) - sphere_radius * sphere_radius; 

	// Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0) 
	if (c > 0.0f && b > 0.0f)
		return false;
	
	float discr = b * b - c; 
	// A negative discriminant corresponds to ray missing sphere 
	if (discr < 0.0f)
		return false; 

	// Ray now found to intersect sphere, compute smallest t value of intersection
	float discrSqrt = sqrt(discr);
	float t = -b + ((c > 0.0f) ? -discrSqrt : discrSqrt);

	hitDistance = t;

	return true;*/
	
	/*vec3 origin_delta = ray.origin - sphere_origin;
	
	const float radiusSquared = sphere_radius * sphere_radius;
	
	const float delta_dot_ray = dot(origin_delta, ray.normal);
	const float delta_ray_distance = dot(origin_delta, origin_delta) - radiusSquared;
	
	// Exit if ray’s origin outside sphere AND ray pointing away from the sphere
	if (delta_ray_distance > 0.0F && delta_dot_ray > 0.0F)
		return false;
	
	// Flatten origin_delta into the plane passing through sphere_origin perpendicular to the ray.
	// This gives the closest approach of the ray to the center.
	vec3 a = origin_delta - delta_dot_ray * ray.normal;
	
	float aSquared = dot(a, a);
	
	// Cache the subtraction now since we need it twice
	float deltaSquared = radiusSquared - aSquared;
	
	// Closest approach is outside the sphere.
	if (deltaSquared < 0.0F)
		return false;	
	
	// Calculate distance from plane where ray enters/exits the sphere.
	float h = sqrt(deltaSquared);
	
	// Calculate intersection point relative to sphere center.
	if (delta_ray_distance > 0)
	{
		vec3 i = a - h * ray.normal;

		vec3 intersection = sphere_origin + i;

		hitDistance = length(intersection - ray.origin);
	}
	else
	{
		vec3 i = a + h * ray.normal;

		vec3 intersection = sphere_origin + i;

		hitDistance = length(intersection - ray.origin);
	}
	
	return true;*/
}

float SampleCloudDensity ( in vec3 world_position, in rrCloudLayer cloud_info, in float depth )
{
	const float base_scaling = 3.0 / 10000.0;
	float density = textureLod(textureSampler1, world_position * base_scaling, 0).r;
	
	// Apply a low-level cloud
	density *= clamp((0.2 - abs(0.2 - depth)) * 50, 0.0, 1.0);
	
	// Clouds are less dense at the bottom.
	density *= depth;
	
	//density = clamp(density * 4.0 - 2.0, 0.0, 1.0);
	return density;
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
			
			[[loop]]
			for (int i = 0; i < sampleCount; ++i)
			{
				samplePosition += sampleStep;
				
				float newDensity = SampleCloudDensity(samplePosition, cloud_layer_0, i / float(sampleCount));
				
				// Hack density deeper
				density = newDensity + density * (1.0 - newDensity);
				if (density > 0.99)
				{
					break;
				}
			}
			
			skyColor = mix(skyColor, vec3(1, 1, 1), density);
		}
	}
	
	//FragDiffuse = vec4(l_screenRay, 1.0);
	FragDiffuse = vec4(skyColor, 1.0);
}
