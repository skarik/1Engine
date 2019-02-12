// v2d/default
// Default shader for 2D GUI elements.
#version 430

// Inputs from vertex shader
layout(location = 0) in vec4 v2f_colors;
layout(location = 1) in vec4 v2f_position;
layout(location = 2) in vec2 v2f_texcoord0;

// Outputs
layout(location = 0) out vec4 FragDiffuse;

// Samplers
layout(binding = 0, location = 20) uniform sampler2D textureSampler0;

// Inputs
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
	vec4 diffuseColor = texture( textureSampler0, v2f_texcoord0 );
	float f_alpha = diffuseColor.a * v2f_colors.a * sys_DiffuseColor.a;
	if ( f_alpha < sys_AlphaCutoff ) discard;
	FragDiffuse = diffuseColor * v2f_colors * sys_DiffuseColor;
	FragDiffuse.a = f_alpha;
}
