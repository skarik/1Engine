// sys/fullbright
// Renders object without lighting, only diffuse multiply blending.
#version 330
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

in vec3 mdl_Vertex;
in vec3 mdl_TexCoord;
in vec4 mdl_Color;

// Outputs to fragment shader
out vec4 v2f_colors;
out vec2 v2f_texcoord0;

// System inputs
layout(std140) uniform sys_cbuffer_PerObject
{
    mat4 sys_ModelTRS;
    mat4 sys_ModelRS;
    mat4 sys_ModelViewProjectionMatrix;
    mat4 sys_ModelViewProjectionMatrixInverse;
};
layout(std140) uniform sys_cbuffer_PerObjectExt
{
    vec4    sys_DiffuseColor;
    vec4    sys_SpecularColor;
    vec4    sys_EmissiveColor;
    vec4    sys_LightingOverrides;

    vec4    sys_TextureScale;
    vec4    sys_TextureOffset;
};

void main ( void )
{
	vec4 v_localPos = vec4( mdl_Vertex, 1.0 );
	vec4 v_screenPos = sys_ModelViewProjectionMatrix * v_localPos;

	v2f_colors		= mdl_Color * sys_DiffuseColor;
	v2f_texcoord0	= mdl_TexCoord.xy;

	gl_Position = v_screenPos;
}
