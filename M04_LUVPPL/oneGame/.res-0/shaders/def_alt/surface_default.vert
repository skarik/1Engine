#version 330
#define VERTEX_PROCESSOR

// Attribute inputs
in vec3 mdl_Vertex;
in vec3 mdl_Normal;
in vec4 mdl_Color;
in vec3 mdl_TexCoord;

in vec3 mdl_Tangents;
in vec3 mdl_Binormals;

in vec3 mdl_TexCoord2;
in vec3 mdl_TexCoord3;
in vec3 mdl_TexCoord4;

// System inputs
uniform mat4 sys_ModelTRS;
uniform mat4 sys_ModelRS;
uniform mat4 sys_ModelViewProjectionMatrix;
uniform mat4 sys_ViewProjectionMatrix;

// State inputs
//uniform vec3 sys_WorldCameraPos;

// Material inputs
uniform vec4 sys_TextureScale;
uniform vec4 sys_TextureOffset;

// Time inputs
uniform vec4 sys_SinTime;
uniform vec4 sys_CosTime;
uniform vec4 sys_Time;

// Fog
layout(std140) uniform sys_Fog
{
	vec4	sys_FogColor;
	vec4	sys_AtmoColor;
	float 	sys_FogEnd;
	float 	sys_FogScale;
};

// Vertex Outputs
out vec4 v2f_normals;
out vec4 v2f_colors;
out vec4 v2f_position;

out vec3 v2f_texcoord;

void main ( void )
{
    vec4 ov_vertex;
    vec4 ov_normal;

    VERTEX_PROCESSOR;

    vec4 v_worldPosition	= sys_ModelTRS * ov_vertex;
    vec4 v_worldNormal		= sys_ModelRS * ov_normal;
    vec4 v_screenPosition	= sys_ModelViewProjectionMatrix * ov_vertex;

    gl_Position = v_screenPosition;
    v2f_normals = v_worldNormal;
    v2f_colors	= mdl_Color;
    v2f_position= v_worldPosition;
    v2f_texcoord= mdl_TexCoord * sys_TextureScale.xyz + sys_TextureOffset.xyz;
}
