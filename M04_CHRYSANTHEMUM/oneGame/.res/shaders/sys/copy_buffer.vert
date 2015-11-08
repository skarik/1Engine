// sys/copy_buffer
// Used to copy a buffer from one buffer to another if blit cannot be used.
// There is no projection calculations that occur. Input vertex coordinates are the device's coordinates.
// If you muck with this shader, you might as well summon something from the tentacle realm.
#version 330

in vec3 mdl_Vertex;
in vec3 mdl_TexCoord;

// Outputs to fragment shader
out vec4 v2f_position;
out vec2 v2f_texcoord0;

void main ( void )
{
	vec4 v_screenPos = vec4( mdl_Vertex, 1.0 );
	
	v2f_position	= vec4( mdl_Vertex, 1.0 );
	v2f_texcoord0	= mdl_TexCoord.xy;

	gl_Position = v_screenPos;
}