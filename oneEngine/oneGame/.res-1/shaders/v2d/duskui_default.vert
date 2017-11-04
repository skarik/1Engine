// v2d/duskui_default
// Default shader for DuskUI - has a special hack to enable swapping between untextured and textured mode.
// Skips the alpha cutoff as well. In texture mode, sqrt's the alpha to counteract the effects of Dusk's double blend.
#version 330
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

in vec3 mdl_Vertex;
in vec3 mdl_TexCoord;
in vec4 mdl_Color;
in vec3 mdl_Normal;

// Outputs to fragment shader
out vec4 v2f_colors;
out vec4 v2f_position;
out vec2 v2f_texcoord0;
out float v2f_textureStrength;

// Inputs
layout(std140) uniform sys_cbuffer_PerObjectExt
{
    vec4    sys_DiffuseColor;
    vec4    sys_SpecularColor;
    vec3    sys_EmissiveColor;
    float   sys_AlphaCutoff;
    vec4    sys_LightingOverrides;

    vec4    sys_TextureScale;
    vec4    sys_TextureOffset;
};
layout(std140) uniform sys_cbuffer_PerFrame
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
	vec4 v_localPos = vec4( mdl_Vertex, 1.0 );
    vec4 v_screenPos = v_localPos;

	v2f_colors		= mdl_Color;
	v2f_position	= v_localPos;
	v2f_texcoord0   = mdl_TexCoord.xy * sys_TextureScale.xy + sys_TextureOffset.xy;

    v2f_textureStrength = min(1.0, abs(mdl_Normal.x) + abs(mdl_Normal.y));

	gl_Position = v_screenPos;
}
