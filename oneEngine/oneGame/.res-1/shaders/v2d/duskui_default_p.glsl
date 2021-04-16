// v2d/duskui_default
// Default shader for DuskUI - has a special hack to enable swapping between untextured and textured mode.
// Skips the alpha cutoff as well. In texture mode, sqrt's the alpha to counteract the effects of Dusk's double blend.
#version 430

// Inputs from vertex shader
layout(location = 0) in vec4 v2f_colors;
layout(location = 1) in vec4 v2f_position;
layout(location = 2) in vec2 v2f_texcoord0;
layout(location = 3) in float v2f_textureStrength;
layout(location = 4) in vec4 v2f_scissorCoords;

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
	// Software scissoring
	if (any(lessThan(v2f_position.xy, v2f_scissorCoords.xy)) || any(greaterThan(v2f_position.xy, v2f_scissorCoords.xy + v2f_scissorCoords.zw)))
	{
		discard;
	}

	vec4 primColor, textColor;

	// Generate normal prim color
	primColor = vec4(1, 1, 1, 1);
	primColor.a = v2f_colors.a * sys_DiffuseColor.a;
	
	// Generate text colors 
	textColor = texture( textureSampler0, v2f_texcoord0 );
	textColor.a = sqrt(textColor.a) * v2f_colors.a * sys_DiffuseColor.a;
	
	// Select correct shape type
	FragDiffuse = mix(primColor, textColor, v2f_textureStrength);
	// Apply vertex colors
	FragDiffuse.rgb *= v2f_colors.rgb * sys_DiffuseColor.rgb;
	
	/*vec4 diffuseColor = mix( vec4(1,1,1,1), texture( textureSampler0, v2f_texcoord0 ), v2f_textureStrength );
    float f_alpha = diffuseColor.a * v2f_colors.a * sys_DiffuseColor.a;
    f_alpha = mix( f_alpha, sqrt(f_alpha), v2f_textureStrength );
	FragDiffuse = diffuseColor * v2f_colors * sys_DiffuseColor;
	FragDiffuse.a = f_alpha;*/
}
