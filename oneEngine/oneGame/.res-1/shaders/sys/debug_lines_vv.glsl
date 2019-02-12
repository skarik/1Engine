// sys/debug_lines
// Renders object without lighting, only diffuse multiply blending.
// Specifically for rendering debug lines, using the normals & texcoords for offsets.
#version 430

layout(location = 0) in vec3 mdl_Vertex;
layout(location = 3) in vec3 mdl_Normal;     // Direction of the line from start to end.
layout(location = 1) in vec3 mdl_TexCoord;   // X: screen offset sign.
layout(location = 2) in vec4 mdl_Color;

// Outputs to fragment shader
layout(location = 0) out vec4 v2f_colors;
layout(location = 1) out vec2 v2f_texcoord0;

// System inputs
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
    vec4    sys_EmissiveColor;
    vec4    sys_LightingOverrides;

    vec4    sys_TextureScale;
    vec4    sys_TextureOffset;
};
layout(binding = 2, std140) uniform sys_cbuffer_PerCamera
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
    float perspective_w = v_screenPos.w;
    v_screenPos.xy /= perspective_w; // Perform divide now to avoid distortion
    // Clamp XY to screen range
    //v_screenPos.xy = clamp(v_screenPos.xy, -1.0, 1.0);

    // Create the delta:
	vec4 v_screenDelta = sys_ModelViewProjectionMatrix * vec4(mdl_Vertex + mdl_Normal, 1.0);
    v_screenDelta.xy /= perspective_w; // Perform divide now to avoid distortion
    // Clamp XY to screen range
    //v_screenDelta.xy = clamp(v_screenDelta.xy, -1.0, 1.0);
    // Create the delta pointing
    v_screenDelta.xy -= v_screenPos.xy;

    // Offset the screne position now:
    vec2 l_offset = normalize(vec2(v_screenDelta.y, -v_screenDelta.x)) / sys_ViewportInfo.zw;
    v_screenPos.xy += l_offset * mdl_TexCoord.x * 1.5 * 2;

    // Continue normally
	v2f_colors		= mdl_Color * sys_DiffuseColor;
	v2f_texcoord0	= mdl_TexCoord.xy;

    v_screenPos.xy *= perspective_w;
	gl_Position = v_screenPos;
}
