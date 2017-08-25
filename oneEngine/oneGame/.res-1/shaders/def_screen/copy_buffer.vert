#version 140
// If you muck with this shader, you might as well summon something from the tentacle realm.
in vec3 mdl_Vertex;
in vec3 mdl_TexCoord;

// Outputs to fragment shader
varying vec2 v2f_texcoord0;

void main ( void )
{
	v2f_texcoord0	= mdl_TexCoord.xy;
	gl_Position = vec4( mdl_Vertex, 1.0 );
}
