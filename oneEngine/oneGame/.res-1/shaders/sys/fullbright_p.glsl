// sys/fullbright
// Renders object without lighting, only diffuse multiply blending.
#version 430

// Inputs from vertex shader
layout(location = 0) in vec4 v2f_colors;
layout(location = 1) in vec2 v2f_texcoord0;

// Samplers
layout(binding = 0, location = 20) uniform sampler2D textureSampler0;

// Outputs
layout(location = 0) out vec4 FragDiffuse;

// Game Inputs
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
	vec4 diffuseColor = texture( textureSampler0, v2f_texcoord0 );

	FragDiffuse = diffuseColor * v2f_colors;
}
