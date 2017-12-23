#version 330
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

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

layout(std140) uniform sys_cbuffer_PerObject
{
    mat4 sys_ModelTRS;
    mat4 sys_ModelRS;
    mat4 sys_ModelViewProjectionMatrix;
    mat4 sys_ModelViewProjectionMatrixInverse;
};
layout(std140) uniform sys_cbuffer_PerCamera
{
    mat4 sys_ViewProjectionMatrix;
    vec4 sys_WorldCameraPos;
    vec4 sys_ViewportInfo;
    vec2 sys_ScreenSize;
    vec2 sys_PixelRatio;
};
layout(std140) uniform sys_cbuffer_PerFrame
{
    // Time inputs
    vec4 sys_SinTime;
    vec4 sys_CosTime;
    vec4 sys_Time;

    // Fog
	vec4	sys_FogColor;
	vec4	sys_AtmoColor;
	float 	sys_FogEnd;
	float 	sys_FogScale;
};

// Material inputs

layout(std140) uniform sys_cbuffer_PerObjectExt
{
    vec4    sys_DiffuseColor;
    vec4    sys_SpecularColor;
    // Emissive RGB - Emissive color.
    // Emissive A - Alpha cutoff.
    vec4    sys_EmissiveColor;
    vec4    sys_LightingOverrides;

    vec4    sys_TextureScale;
    vec4    sys_TextureOffset;
};

// Vertex Outputs
out vec4 v2f_colors;
out vec4 v2f_position;
out vec3 v2f_texcoord;
out vec4 v2f_normals;
out vec4 v2f_binorms;
out vec4 v2f_tangents;

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
    v2f_binorms = sys_ModelRS * vec4(mdl_Tangents, 1.0);
    v2f_tangents= sys_ModelRS * vec4(mdl_Binormals, 1.0);
    v2f_colors	= mdl_Color;
    v2f_position= v_worldPosition;
    v2f_texcoord= mdl_TexCoord * sys_TextureScale.xyz + sys_TextureOffset.xyz;
}
