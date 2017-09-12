// v2d/default
// Default shader for 2D GUI elements.
#version 330
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

// Inputs from vertex shader
in vec4 v2f_colors;
in vec4 v2f_position;
in vec2 v2f_texcoord0;

// Samplers
layout(location = 20) uniform sampler2D textureSampler0;

// Inputs
/*layout(location = 0) uniform vec4 sys_DiffuseColor;
layout(location = 3) uniform float sys_AlphaCutoff;*/
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
	vec4 diffuseColor = texture( textureSampler0, v2f_texcoord0 );
	float f_alpha = diffuseColor.a * v2f_colors.a * sys_DiffuseColor.a;
	if ( f_alpha < sys_AlphaCutoff ) discard;
	gl_FragColor = diffuseColor * v2f_colors * sys_DiffuseColor;
	gl_FragColor.a = f_alpha;
}
