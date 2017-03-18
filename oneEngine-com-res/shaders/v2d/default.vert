// v2d/default
// Default shader for 2D GUI elements.
#version 330

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

void main ( void )
{
	vec4 v_localPos = vec4( mdl_Vertex, 1.0 );
	vec4 v_screenPos = sys_ModelViewProjectionMatrix * v_localPos;

	v2f_colors		= mdl_Color;
	v2f_position	= sys_ModelTRS*v_localPos;
	v2f_texcoord0	= mdl_TexCoord.xy;
	
	gl_Position = v_screenPos;
}