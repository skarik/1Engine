// v2d/eventide_default
// Default shader for Eventide - uses UV1 to control the texture blending. Otherwise, similar to a translucent 3D shader.
#version 430

// Inputs from vertex shader
layout(location = 0) in vec4 v2f_colors;
layout(location = 1) in vec4 v2f_position;
layout(location = 2) in vec2 v2f_texcoord0;
layout(location = 3) in vec3 v2f_normal;
layout(location = 4) in float v2f_textureStrength;
layout(location = 5) in flat int v2f_textureIndex;

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
#if 0
	textColor = texture( textureSamplers[v2f_textureIndex], v2f_texcoord0 );
#else
	switch (v2f_textureIndex)
	{
	case 0: textColor = texture( textureSamplers[0], v2f_texcoord0 ); break;
	case 1: textColor = texture( textureSamplers[1], v2f_texcoord0 ); break;
	case 2: textColor = texture( textureSamplers[2], v2f_texcoord0 ); break;
	case 3: textColor = texture( textureSamplers[3], v2f_texcoord0 ); break;
	case 4: textColor = texture( textureSamplers[4], v2f_texcoord0 ); break;
	case 5: textColor = texture( textureSamplers[5], v2f_texcoord0 ); break;
	case 6: textColor = texture( textureSamplers[6], v2f_texcoord0 ); break;
	case 7: textColor = texture( textureSamplers[7], v2f_texcoord0 ); break;
	case 8: textColor = texture( textureSamplers[8], v2f_texcoord0 ); break;
	case 9: textColor = texture( textureSamplers[9], v2f_texcoord0 ); break;
	case 10: textColor = texture( textureSamplers[10], v2f_texcoord0 ); break;
	case 11: textColor = texture( textureSamplers[11], v2f_texcoord0 ); break;
	case 12: textColor = texture( textureSamplers[12], v2f_texcoord0 ); break;
	case 13: textColor = texture( textureSamplers[13], v2f_texcoord0 ); break;
	case 14: textColor = texture( textureSamplers[14], v2f_texcoord0 ); break;
	case 15: textColor = texture( textureSamplers[15], v2f_texcoord0 ); break;
	default: textColor = vec4(0,0,0,0); break;
	}
#endif
	textColor.a = sqrt(textColor.a) * v2f_colors.a * sys_DiffuseColor.a;
	
	// Select correct shape type
	FragDiffuse = mix(primColor, textColor, v2f_textureStrength);
	// Apply vertex colors
	FragDiffuse.rgb *= v2f_colors.rgb * sys_DiffuseColor.rgb;
}
