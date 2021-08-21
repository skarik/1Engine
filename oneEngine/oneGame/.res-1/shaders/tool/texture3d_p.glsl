// sys/fullbright
// Renders object without lighting, only diffuse multiply blending.
#version 430

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_control_flow_attributes : require

#include "../common.glsli"
#include "../cbuffers.glsli"

// Inputs from vertex shader
layout(location = 0) in vec4 v2f_colors;
layout(location = 1) in vec3 v2f_texcoord0;
layout(location = 2) in vec3 v2f_worldcoord;

// Samplers
layout(binding = 0, location = 20) uniform sampler3D textureSampler0;

// Outputs
layout(location = 0) out vec4 FragDiffuse;

void main ( void )
{
	// Create the actual ray that's rasterizing this pixel
	vec3 rayOriginWorld = sys_WorldCameraPos.xyz;
	vec3 rayVectorWorld = v2f_worldcoord - rayOriginWorld;
	vec3 rayVectorRenormalizedWorld = normalize(rayVectorWorld);
	
	// Transform the ray into the local object's space
	vec3 rayOrigin = (inverse(sys_ModelTRS) * vec4(rayOriginWorld, 1.0)).xyz;
	vec3 rayVector = inverse(mat3(sys_ModelTRS)) * rayVectorWorld;
	vec3 rayVectorRenormalized = inverse(mat3(sys_ModelTRS)) * rayVectorRenormalizedWorld;
	
	// Start with black
	vec4 diffuseColor = vec4(0.0);
	
	// Set up the ray step to go 1 texel at a time
	const vec3 rayStep = rayVectorRenormalized;//normalize(rayVector);
	const float brightnessRatio = 1.414 * length(rayVectorRenormalized) / length(rayVectorRenormalizedWorld);
	
	// Hit spot is the 3D texcoord.
	vec3 rayStart = v2f_texcoord0;
	vec3 sampleCoord = rayStart;
	
	[[loop]]
	while (all(greaterThanEqual(sampleCoord, vec3(0.0, 0.0, 0.0))) && all(lessThanEqual(sampleCoord, vec3(1.0, 1.0, 1.0))))
	{
		// Add in the colors
		diffuseColor += textureLod( textureSampler0, sampleCoord, 0 ) * brightnessRatio;

		// Step forward
		sampleCoord += rayStep;
	}
	//diffuseColor = texture( textureSampler0, hitCoord );

	diffuseColor.a = 1.0;
	FragDiffuse = diffuseColor * v2f_colors;
	//FragDiffuse.rgb = hitCoord + vec3(0.5, 0.5, 0.5);
	//FragDiffuse.rgb = abs(rayVectorRenormalized);
	//FragDiffuse.rgb = v2f_worldcoord;
}
