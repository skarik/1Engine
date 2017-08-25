// v2d/default
// Default shader for 2D GUI elements.
#version 330
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

in vec3 mdl_Vertex;
in vec3 mdl_TexCoord;
in vec4 mdl_Color;
in vec3 mdl_Normal;

// Outputs to fragment shader
out vec4 v2f_colors;
out vec4 v2f_position;
out vec2 v2f_texcoord0;

// System inputs
uniform mat4 sys_ModelTRS;
uniform mat4 sys_ModelRS;
uniform mat4 sys_ModelViewProjectionMatrix;

// Material inputs
layout(location = 5) uniform vec4 sys_TextureScale;
layout(location = 6) uniform vec4 sys_TextureOffset;

void main ( void )
{
	vec4 v_localPos = vec4( mdl_Vertex, 1.0 );
	vec4 v_screenPos = sys_ModelViewProjectionMatrix * v_localPos;

	v2f_colors		= mdl_Color;
	v2f_position	= sys_ModelTRS*v_localPos;
	v2f_texcoord0   = mdl_TexCoord.xy * sys_TextureScale.xy + sys_TextureOffset.xy;

	gl_Position = v_screenPos;
}
