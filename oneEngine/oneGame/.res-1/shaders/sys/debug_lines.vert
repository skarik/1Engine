// sys/debug_lines
// Renders object without lighting, only diffuse multiply blending.
// Specifically for rendering debug lines, using the normals & texcoords for offsets.
#version 330
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

in vec3 mdl_Vertex;
in vec3 mdl_Normal;     // Direction of the line from start to end.
in vec3 mdl_TexCoord;   // X: screen offset sign.
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
layout(std140) uniform sys_cbuffer_PerCamera
{
    mat4 sys_ViewProjectionMatrix;
    vec4 sys_WorldCameraPos;
    vec4 sys_ViewportInfo;
    vec2 sys_ScreenSize;
    vec2 sys_PixelRatio;
};

void main ( void )
{
	vec4 v_localPos = vec4( mdl_Vertex, 1.0 );
	vec4 v_screenPos = sys_ModelViewProjectionMatrix * v_localPos;

    // Create the delta:
	vec4 v_screenDelta = sys_ModelViewProjectionMatrix * vec4(mdl_Vertex + mdl_Normal, 1.0);
    v_screenDelta -= v_screenPos;

    // Offset the screne position now:
    vec2 l_offset = normalize(vec2(v_screenDelta.y, -v_screenDelta.x)) / sys_ViewportInfo.zw;
    v_screenPos.xy += l_offset * mdl_TexCoord.x * 1.5;

    // Continue normally
	v2f_colors		= mdl_Color * sys_DiffuseColor;
	v2f_texcoord0	= mdl_TexCoord.xy;

	gl_Position = v_screenPos;
}
