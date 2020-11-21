// v2d/eventide_default
// Default shader for Eventide - uses UV1 to control the texture blending. Otherwise, similar to a translucent 3D shader.
#version 430

layout(location = 0) in vec3 mdl_Vertex;
layout(location = 1) in vec3 mdl_TexCoord;
layout(location = 2) in vec4 mdl_Color;
layout(location = 3) in vec3 mdl_Normal;
layout(location = 6) in vec3 mdl_TextureInfo;

// Outputs to fragment shader
layout(location = 0) out vec4 v2f_colors;
layout(location = 1) out vec4 v2f_position;
layout(location = 2) out vec2 v2f_texcoord0;
layout(location = 3) out vec2 v2f_normal;
layout(location = 4) out float v2f_textureStrength;
layout(location = 5) out int v2f_textureIndex;

// Cbuffers
layout(binding = 0, std140) uniform sys_cbuffer_PerObject
{
    mat4 sys_ModelTRS;
    mat4 sys_ModelRS;
    mat4 sys_ModelViewProjectionMatrix;
    mat4 sys_ModelViewProjectionMatrixInverse;
};
layout(binding = 1, std140) uniform sys_cbuffer_PerObjectExt
{
    vec4    sys_DiffuseColor;
    vec4    sys_SpecularColor;
    vec3    sys_EmissiveColor;
    float   sys_AlphaCutoff;
    vec4    sys_LightingOverrides;

    vec4    sys_TextureScale;
    vec4    sys_TextureOffset;
};


void main ( void )
{
	vec4 v_localPos = vec4(mdl_Vertex, 1.0);
	vec4 v_screenPos = sys_ModelViewProjectionMatrix * v_localPos;

	v2f_colors		= mdl_Color;
	v2f_position	= v_localPos;
	v2f_texcoord0   = mdl_TexCoord.xy;
	v2f_normal		= mdl_Normal;

    v2f_textureStrength = clamp(mdl_TextureInfo.r, 0.0, 1.0);
	v2f_textureIndex = int(mdl_TextureInfo.g);

	gl_Position = v_screenPos;
}
