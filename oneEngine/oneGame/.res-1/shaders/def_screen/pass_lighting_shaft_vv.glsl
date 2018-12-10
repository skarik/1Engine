#version 430

layout(location = 0) in vec3 mdl_Vertex;
layout(location = 1) in vec3 mdl_TexCoord;

// Outputs to fragment shader
layout(location = 0) out vec4 v2f_position;
layout(location = 1) out vec2 v2f_texcoord0;


void main ( void )
{
	vec4 v_screenPos = vec4( mdl_Vertex, 1.0 );

	v2f_position	= vec4( mdl_Vertex, 1.0 );
	v2f_texcoord0	= mdl_TexCoord.xy;

	gl_Position = v_screenPos;
}