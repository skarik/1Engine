// sys/black
// Draws a purely black object.
#version 430

// Inputs from vertex shader
layout(location = 0) in vec2 v2f_texcoord0;

// Samplers
layout(binding = 0, location = 20) uniform sampler2D textureSampler0;

// Outputs
layout(location = 0) out vec4 FragDiffuse;

// External constants
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
	FragDiffuse = vec4( 0,0,0, sys_DiffuseColor.a * texture(textureSampler0, v2f_texcoord0).a );
}
