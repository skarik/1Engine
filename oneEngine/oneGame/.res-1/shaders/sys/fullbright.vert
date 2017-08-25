// sys/fullbright
// Renders object without lighting, only diffuse multiply blending.
#version 330
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

in vec3 mdl_Vertex;
in vec3 mdl_TexCoord;
in vec4 mdl_Color;

// Outputs to fragment shader
out vec4 v2f_colors;
out vec2 v2f_texcoord0;

// System inputs
uniform mat4 sys_ModelTRS;
uniform mat4 sys_ModelRS;
uniform mat4 sys_ModelViewProjectionMatrix;
layout(location = 0) uniform vec4 sys_DiffuseColor;

void main ( void )
{
	vec4 v_localPos = vec4( mdl_Vertex, 1.0 );
	vec4 v_screenPos = sys_ModelViewProjectionMatrix * v_localPos;

	v2f_colors		= mdl_Color * sys_DiffuseColor;
	v2f_texcoord0	= mdl_TexCoord.xy;

	gl_Position = v_screenPos;
}
