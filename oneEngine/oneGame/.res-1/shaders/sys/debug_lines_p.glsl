// sys/debug_lines
// Renders object without lighting, only diffuse multiply blending.
// Specifically for rendering debug lines, using the normals & texcoords for offsets.
#version 430

// Inputs from vertex shader
layout(location = 0) in vec4 v2f_colors;
layout(location = 1) in vec2 v2f_texcoord0;

// Samplers
layout(binding = 0, location = 20) uniform sampler2D textureSampler0;

// Game Inputs
layout(binding = 1, std140) uniform sys_cbuffer_PerObjectExt
{
    vec4    sys_DiffuseColor;
    vec4    sys_SpecularColor;
    vec4    sys_EmissiveColor;
    vec4    sys_LightingOverrides;

    vec4    sys_TextureScale;
    vec4    sys_TextureOffset;
};

// Outputs
layout(location = 0) out vec4 FragDiffuse;

void main ( void )
{
    // Get the width of the line so to pair it down.
    float d = fwidth(v2f_texcoord0.x);
    float a = smoothstep(0.0, d * 1.5, abs(0.5 - v2f_texcoord0.x));
    //if (a > 0.2) discard;

	vec4 diffuseColor = texture( textureSampler0, v2f_texcoord0 );
	FragDiffuse = diffuseColor * v2f_colors;
    FragDiffuse.a = mix(1.0, 0.0, a);
}
