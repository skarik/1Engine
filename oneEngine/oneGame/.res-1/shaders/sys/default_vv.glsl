// sys/default
// Default forward rendering shader.
#version 430

layout(location = 0) in vec3 mdl_Vertex;
layout(location = 1) in vec3 mdl_TexCoord;
layout(location = 2) in vec4 mdl_Color;
layout(location = 3) in vec3 mdl_Normal;

// Outputs to fragment shader
layout(location = 0) out vec4 v2f_colors;
layout(location = 1) out vec4 v2f_position;
layout(location = 2) out vec2 v2f_texcoord0;
layout(location = 3) out float v2f_fogdensity;

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
layout(binding = 3, std140) uniform sys_cbuffer_PerFrame
{
    // Time inputs
    vec4    sys_SinTime;
    vec4    sys_CosTime;
    vec4    sys_Time;

    // Fog
	vec4	sys_FogColor;
	vec4	sys_AtmoColor;
	float 	sys_FogEnd;
	float 	sys_FogScale;
};

void main ( void )
{
	vec4 v_localPos = vec4(mdl_Vertex, 1.0);
	vec4 v_screenPos = sys_ModelViewProjectionMatrix * v_localPos;

	vec4 v2f_normals	= sys_ModelRS * vec4( mdl_Normal, 1.0 );
	v2f_colors			= sys_DiffuseColor * mdl_Color;
	v2f_position		= sys_ModelTRS * v_localPos;
	v2f_texcoord0		= mdl_TexCoord.xy;
	v2f_fogdensity 	 	= max( 0.0, (sys_FogEnd - v_screenPos.z) * sys_FogScale );

	gl_Position = v_screenPos;
}
