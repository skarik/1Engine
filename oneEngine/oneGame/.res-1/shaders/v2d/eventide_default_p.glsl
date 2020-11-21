// v2d/eventide_default
// Default shader for Eventide - uses UV1 to control the texture blending. Otherwise, similar to a translucent 3D shader.
#version 430

// Inputs from vertex shader
layout(location = 0) in vec4 v2f_colors;
layout(location = 1) in vec4 v2f_position;
layout(location = 2) in vec2 v2f_texcoord0;
layout(location = 3) in vec2 v2f_normal;
layout(location = 4) in float v2f_textureStrength;
layout(location = 5) in int v2f_textureIndex;

// Outputs
layout(location = 0) out vec4 FragDiffuse;

// Samplers
layout(binding = 0, location = 20) uniform sampler2D textureSamplers[16];

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
	vec4 primColor, textColor;

	// Generate normal prim color
	primColor = vec4(1, 1, 1, 1);
	primColor.a = v2f_colors.a * sys_DiffuseColor.a;
	
	// Generate text colors 
	textColor = texture( textureSamplers[v2f_textureIndex], v2f_texcoord0 );
	textColor.a = sqrt(textColor.a) * v2f_colors.a * sys_DiffuseColor.a;
	
	// Select correct shape type
	FragDiffuse = mix(primColor, textColor, v2f_textureStrength);
	// Apply vertex colors
	FragDiffuse.rgb *= v2f_colors.rgb * sys_DiffuseColor.rgb;
}
