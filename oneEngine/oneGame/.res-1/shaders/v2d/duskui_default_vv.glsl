// v2d/duskui_default
// Default shader for DuskUI - has a special hack to enable swapping between untextured and textured mode.
// Skips the alpha cutoff as well. In texture mode, sqrt's the alpha to counteract the effects of Dusk's double blend.
#version 430

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_control_flow_attributes : require

#include "../common.glsli"
#include "../cbuffers.glsli"

layout(location = 0) in vec3 mdl_Vertex;
layout(location = 1) in vec3 mdl_TexCoord;
layout(location = 2) in vec4 mdl_Color;
layout(location = 3) in vec3 mdl_Normal;
layout(location = 6) in vec4 mdl_ScissorCoords; // Stored in UV1, Slot 6

// Outputs to fragment shader
layout(location = 0) out vec4 v2f_colors;
layout(location = 1) out vec4 v2f_position;
layout(location = 2) out vec2 v2f_texcoord0;
layout(location = 3) out float v2f_textureStrength;
layout(location = 4) out vec4 v2f_scissorCoords;

void main ( void )
{
	vec4 v_localPos = vec4( mdl_Vertex, 1.0 );
    vec4 v_screenPos = v_localPos;

	v2f_colors		= mdl_Color;
	v2f_position	= v_localPos;
	v2f_texcoord0   = mdl_TexCoord.xy * sys_TextureScale.xy + sys_TextureOffset.xy;

    v2f_textureStrength = min(1.0, abs(mdl_Normal.x) + abs(mdl_Normal.y));
	
	v2f_scissorCoords = mdl_ScissorCoords;

	gl_Position = v_screenPos;
}
