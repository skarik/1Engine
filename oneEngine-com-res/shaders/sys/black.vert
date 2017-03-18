// sys/black
// Draws a purely black object.
#version 330
// Inputs
in vec3 mdl_Vertex;
in vec3 mdl_TexCoord;

// Outputs to fragment shader
out vec2 v2f_texcoord0;

// System inputs
uniform mat4 sys_ModelTRS;
uniform mat4 sys_ModelRS;
uniform mat4 sys_ModelViewProjectionMatrix;
uniform vec4 sys_DiffuseColor;

void main ( void )
{
	vec4 v_localPos = vec4( mdl_Vertex, 1.0 );
	vec4 v_screenPos = sys_ModelViewProjectionMatrix * v_localPos;

	v2f_texcoord0	= mdl_TexCoord.xy;
	
	gl_Position = v_screenPos;
}