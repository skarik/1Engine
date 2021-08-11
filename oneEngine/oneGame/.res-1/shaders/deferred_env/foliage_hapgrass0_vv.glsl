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
	
	vec4 v_localPos = l_worldTransform * vec4( mdl_Vertex, 1.0 );
	vec4 v_screenPos = sys_ModelViewProjectionMatrix * vec4( v_localPos.xyz, 1.0 );

	v2f_colors		= vec4(mdl_Color.rgb * sys_DiffuseColor.rgb * l_variationInfo.color, mdl_Color.a * sys_DiffuseColor.a);
	v2f_texcoord0	= mdl_TexCoord.xy;
	v2f_normal		= mat3(l_worldTransform) * mdl_Normal;

	gl_Position = v_screenPos;
}
