// v2d/duskui_default
// Default shader for DuskUI - has a special hack to enable swapping between untextured and textured mode.
// Skips the alpha cutoff as well. In texture mode, sqrt's the alpha to counteract the effects of Dusk's double blend.
#version 430

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_control_flow_attributes : require

#include "../common.glsli"
#include "../cbuffers.glsli"

// Inputs from vertex shader
layout(location = 0) in vec4 v2f_colors;
layout(location = 1) in vec4 v2f_position;
layout(location = 2) in vec2 v2f_texcoord0;
layout(location = 3) in float v2f_textureStrength;
layout(location = 5) in float v2f_mouseGlow;

// CBuffers
layout(binding = CBUFFER_USER0, std430) uniform sys_cbuffer_DuskUI_Params
{
	vec4 sys_DuskUI_PositionTransform;
	vec4 sys_DuskUI_ScissorCoords;
	vec2 sys_DuskUI_GlowPosition;
	float sys_DuskUI_GlowSize;
	float sys_DuskUI_GlowStrength;
};

// Outputs
layout(location = 0) out vec4 FragDiffuse;

// Samplers
layout(binding = 0, location = 20) uniform sampler2D textureSampler0;

void main ( void )
{
	// Software scissoring
	if (any(lessThan(v2f_position.xy, sys_DuskUI_ScissorCoords.xy)) || any(greaterThan(v2f_position.xy, sys_DuskUI_ScissorCoords.xy + sys_DuskUI_ScissorCoords.zw)))
	{
		discard;
	}
	
	// Check mouse glow
	float glow_additive = max(0.0, v2f_mouseGlow);
	float glow_multiply = max(0.0, -v2f_mouseGlow);

	vec4 primColor, textColor;

	{
		// Generate normal prim color
		primColor = vec4(1, 1, 1, 1);
		primColor.a = v2f_colors.a * sys_DiffuseColor.a;
		
		float glow_gradient = pow(max(0.0, 1.0 - (length(v2f_position.xy - sys_DuskUI_GlowPosition) / sys_DuskUI_GlowSize)), 3.0);
		primColor = mix(
			primColor + glow_gradient * sys_DuskUI_GlowStrength * glow_additive, 
			primColor + primColor * sys_DuskUI_GlowStrength * 4.0 * glow_gradient,
			glow_multiply);
	}
	
	{
		// Generate text colors 
		textColor = texture( textureSampler0, v2f_texcoord0 );
		textColor.a = sqrt(textColor.a) * v2f_colors.a * sys_DiffuseColor.a;
	}
	
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
