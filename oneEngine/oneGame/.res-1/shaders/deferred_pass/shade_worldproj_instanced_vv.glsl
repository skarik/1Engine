#version 430

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_control_flow_attributes : require

#include "../cbuffers.glsli"
#include "../common_lighting.glsli"	

layout(location = 0) in vec3 mdl_Vertex;

// Outputs to fragment shader
layout(location = 0) out vec4 v2f_position;
layout(location = 1) out vec2 v2f_texcoord0;
layout(location = 2) out flat int v2f_lightIndex;

layout(binding = CBUFFER_USER0, std140) uniform sys_cbuffer_User0
{
	int Lighting_FirstIndex;
};

layout(binding = SBUFFER_USER0, std430) readonly buffer Lighting_Data
{
	rrLight Lighting_Params [];
};

void main ( void )
{
	const int lightIndex = Lighting_FirstIndex + gl_InstanceIndex;
	const rrLight lightParams = Lighting_Params[lightIndex];

	// Calculate a rotation matrix for the model
	/*vec3 model_rot_y = lightParams.direction;
	const bool model_rot_y_locked = (model_rot_y.x == 0.0 && model_rot_y.z == 0.0);
	vec3 model_rot_z = normalize(cross(model_rot_y, vec3(0, 1, 0)));
	vec3 model_rot_x = normalize(cross(model_rot_y, model_rot_z));
	mat3 model_rotation = mat3(model_rot_x, model_rot_y, model_rot_z);*/
	// Calculate the light mesh's position in world-space:
	//vec3 model_vertex = (model_rotation * mdl_Vertex) * lightParams.falloff_range + lightParams.position;
	vec3 model_vertex = (mdl_Vertex) * lightParams.falloff_range + lightParams.position;
	
	vec4 v_screenPos = sys_ViewProjectionMatrix * vec4( model_vertex, 1.0 );
	
	v2f_position	= vec4( model_vertex, 1.0 );
	v2f_texcoord0	= v2f_position.xy / v2f_position.w;
	v2f_lightIndex	= lightIndex;

	gl_Position = v_screenPos;
}