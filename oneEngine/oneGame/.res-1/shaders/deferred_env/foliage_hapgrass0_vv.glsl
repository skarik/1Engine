// sys/fullbright
// Renders object without lighting, only diffuse multiply blending.
#version 430

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_control_flow_attributes : require

#include "../common.glsli"
#include "../cbuffers.glsli"

layout(location = 0) in vec3 mdl_Vertex;
layout(location = 1) in vec3 mdl_TexCoord;
layout(location = 2) in vec4 mdl_Color;
layout(location = 3) in vec3 mdl_Normal;
layout(location = 4) in vec3 mdl_Tangent;
layout(location = 5) in vec3 mdl_Binormal;

// Outputs to fragment shader
layout(location = 0) out vec4 v2f_colors;
layout(location = 1) out vec2 v2f_texcoord0;
layout(location = 2) out vec3 v2f_normal;

// Instancing offset info
layout(binding = CBUFFER_USER0, std140) uniform sys_cbuffer_User0
{
	int instanced_FirstIndex;
};

// Instancing buffers
layout(binding = SBUFFER_USER0, std430) readonly buffer instanced_Transforms
{
	mat4 instanced_Transform [1024];
};

struct Variation
{
	vec3 color;
	int index;
};
layout(binding = SBUFFER_USER1, std430) readonly buffer instanced_Variations
{
	Variation instanced_Variation [1024];
};

void main ( void )
{
	const mat4 l_worldTransform = instanced_Transform[instanced_FirstIndex + gl_InstanceIndex]; 
	const Variation l_variationInfo = instanced_Variation[instanced_FirstIndex + gl_InstanceIndex];
	
	float hacky_heightVariation = 1.0 + 0.2 * sin(l_variationInfo.index * 32.13124 * 4);
	
	vec4 v_localPos = l_worldTransform * vec4( mdl_Vertex.xy, mdl_Vertex.z * hacky_heightVariation, 1.0 );
	float hacky_ao = 0;
	
	vec3 l_offset = vec3(0, 0, 0);
	{
		vec3 v_rootPos = l_worldTransform[3].xyz;
		
		float l_offsetLength = 
			sin(l_variationInfo.index * 32.13124 + sys_Time.w) * 1.0
			+ sin(v_rootPos.x * 0.42 * 2 + v_rootPos.y * 0.16 * 2 + sys_Time.y * 1.7) * 7.0;
			
		l_offset.x += sys_CosTime.x * l_offsetLength;
		l_offset.y += sys_SinTime.x * l_offsetLength;
		
		hacky_ao = clamp(abs(mdl_Vertex.z) * 2.0, 0.0, 1.0) * 0.9 + 0.1;
		// change color as it moves
		hacky_ao += mdl_Vertex.z * (l_offsetLength/10.0) * 0.1;
	}
	v_localPos.xyz += l_offset * mdl_Vertex.z / 28.0; // Offset grass
	
	
	vec4 v_screenPos = sys_ModelViewProjectionMatrix * vec4( v_localPos.xyz, 1.0 );
	// TODO: Move this into either .res-0 or make an option.
	v_screenPos.xy = floor( v_screenPos.xy * (sys_ScreenSize * 0.25 / v_screenPos.z) ) / (sys_ScreenSize * 0.25 / v_screenPos.z);

	v2f_colors		= vec4(mdl_Color.rgb * sys_DiffuseColor.rgb * l_variationInfo.color * hacky_ao, mdl_Color.a * sys_DiffuseColor.a);
	v2f_texcoord0	= mdl_TexCoord.xy;
	v2f_normal		= mat3(l_worldTransform) * mdl_Normal;

	gl_Position = v_screenPos;
}
