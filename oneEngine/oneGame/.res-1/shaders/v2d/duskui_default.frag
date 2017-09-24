// v2d/duskui_default
// Default shader for DuskUI - has a special hack to enable swapping between untextured and textured mode.
// Skips the alpha cutoff as well. In texture mode, sqrt's the alpha to counteract the effects of Dusk's double blend.
#version 330
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

// Inputs from vertex shader
in vec4 v2f_colors;
in vec4 v2f_position;
in vec2 v2f_texcoord0;
in float v2f_textureStrength;

// Samplers
layout(location = 20) uniform sampler2D textureSampler0;

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

void main ( void )
{
	vec4 diffuseColor = mix( vec4(1,1,1,1), texture( textureSampler0, v2f_texcoord0 ), v2f_textureStrength );
    float f_alpha = diffuseColor.a * v2f_colors.a * sys_DiffuseColor.a;
    f_alpha = mix( f_alpha, sqrt(f_alpha), v2f_textureStrength );
	gl_FragColor = diffuseColor * v2f_colors * sys_DiffuseColor;
    gl_FragColor.a = f_alpha;
}
