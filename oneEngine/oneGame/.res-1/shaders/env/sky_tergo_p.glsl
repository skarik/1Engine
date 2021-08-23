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

// Intersects ray r = p + td, |d| = 1, with sphere s and, if intersecting, 
// returns t value of intersection and intersection point q 
bool IntersectRaySphere(vec3 origin, vec3 dir, vec3 sphere_origin, float sphere_radius, out float hitDistance)// out vec3 hitPoint) 
{
	vec3 centerDist = origin - sphere_origin; 
	float b = dot(centerDist, dir); 
	float c = dot(centerDist, centerDist) - sphere_radius * sphere_radius; 

	// Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0) 
	if (c > 0.0f && b > 0.0f)
		return false;
	
	float discr = b * b - c; 
	// A negative discriminant corresponds to ray missing sphere 
	if (discr < 0.0f)
		return false; 

	// Ray now found to intersect sphere, compute smallest t value of intersection
	float t = -b - sqrt(discr); 

	// If t is negative, ray started inside sphere so clamp t to zero 
	//if (t < 0.0f) t = 0.0f; 
	//hitPoint = p + t * d; 
	hitDistance = abs(t);

	return true;
}

float raySphereIntersect(vec3 r0, vec3 rd, vec3 s0, float sr)
{
    // - r0: ray origin
    // - rd: normalized ray direction
    // - s0: sphere center
    // - sr: sphere radius
    // - Returns distance from r0 to first intersecion with sphere,
    //   or -1.0 if no intersection.
    float a = dot(rd, rd);
    vec3 s0_r0 = r0 - s0;
    float b = 2.0 * dot(rd, s0_r0);
    float c = dot(s0_r0, s0_r0) - (sr * sr);
    if (b*b - 4.0*a*c < 0.0)
	{
        return -1.0;
    }
    return (-b - sqrt((b*b) - 4.0*a*c))/(2.0*a);
}

float SampleCloudDensity ( vec3 world_position )
{
	float density = textureLod(textureSampler1, world_position / 15000.0, 0).r;
	density = clamp(density * 4.0 - 2.0, 0.0, 1.0);
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
	
	// Sample the cubemap using the ray
	vec4 skyCubemap = texture(textureSampler0, l_screenRay);
	
	// Start with the sky cubemap
	vec3 skyColor = skyCubemap.rgb;
	
	//const float kCloud0Bottom = 0.0;
	const rrCloudLayer cloud_layer_0 = rrCloudLayer(1000.0, 4500.0, 20000.0);
	
	[[branch]]
	if (l_screenRay.z > 0.0)
	{
		// Raycast against a sphere
		float bottomLayerHitDistance = 0.0;
		bool isBottomHit = IntersectRaySphere(
			sys_WorldCameraPos.xyz,
			l_screenRay,
			vec3(sys_WorldCameraPos.xy, cloud_layer_0.radius),
			cloud_layer_0.radius + cloud_layer_0.bottom,
			bottomLayerHitDistance);
		
		[[branch]]
		if (isBottomHit)
		{
			float topLayerHitDistance = bottomLayerHitDistance;
			IntersectRaySphere(
				sys_WorldCameraPos.xyz,
				l_screenRay,
				vec3(sys_WorldCameraPos.xy, cloud_layer_0.radius),
				cloud_layer_0.radius + cloud_layer_0.top,
				topLayerHitDistance);
			
			const int kSampleCount = 8;
			const float kSampleStepLength = (cloud_layer_0.top - cloud_layer_0.bottom) / kSampleCount;
			
			const vec3 sampleDelta = (topLayerHitDistance - bottomLayerHitDistance) * l_screenRay;
			const float sampleDeltaLength = length(sampleDelta);
			const vec3 sampleStep = (sampleDelta / sampleDeltaLength) * kSampleStepLength;
			
			const int sampleCount = int(sampleDeltaLength / kSampleStepLength + 1);
			vec3 samplePosition = sys_WorldCameraPos.xyz + l_screenRay * bottomLayerHitDistance;
			float density = 0.0;
			
			[[loop]]
			for (int i = 0; i < sampleCount; ++i)
			{
				samplePosition += sampleStep;
				
				float newDensity = SampleCloudDensity(samplePosition);
				
				// Hack density deeper
				density = newDensity + density * (1.0 - newDensity);
				if (density > 0.99)
				{
					break;
				}
			}
			
			skyColor = mix(skyColor, vec3(1, 1, 1), density);
			//skyColor = vec3(1, 1, 1) * (bottomLayerHitDistance / cloud_layer_0.top) * 0.25;
		}
	}
	
	FragDiffuse = vec4(skyColor, 1.0);
}
