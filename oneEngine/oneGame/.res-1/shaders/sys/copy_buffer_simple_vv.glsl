// sys/copy_buffer
// Used to copy a buffer from one buffer to another if blit cannot be used.
// There is no projection calculations that occur. Input vertex coordinates are the device's coordinates.
// If you muck with this shader, you might as well summon something from the tentacle realm.
#version 430

layout(location = 0) in vec3 mdl_Vertex;
layout(location = 1) in vec3 mdl_TexCoord;

// Outputs to fragment shader
layout(location = 0) out vec2 v2f_texcoord0;

void main ( void )
{
	v2f_texcoord0	= mdl_TexCoord.xy;
	gl_Position = vec4( mdl_Vertex, 1.0 );
}
