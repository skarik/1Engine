// sys/copy_buffer
// Used to copy a buffer from one buffer to another if blit cannot be used.
// There is no projection calculations that occur. Input vertex coordinates are the device's coordinates.
// If you muck with this shader, you might as well summon something from the tentacle realm.
#version 330

in vec3 mdl_Vertex;
in vec3 mdl_TexCoord;

uniform vec4 sys_DiffuseColor;

// Outputs to fragment shader
out vec2 v2f_texcoord0;

void main ( void )
{
	v2f_texcoord0	= mdl_TexCoord.xy * sys_DiffuseColor.xy;
	gl_Position = vec4( mdl_Vertex, 1.0 );
}
