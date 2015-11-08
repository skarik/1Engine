// If you muck with this shader, you might as well summon something from the tentacle realm.
in vec3 mdl_Vertex;
in vec3 mdl_TexCoord;

// Outputs to fragment shader
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;

//uniform mat4 sys_ModelViewProjectionMatrix;

void main ( void )
{
	//vec4 v_screenPos = sys_ModelViewProjectionMatrix * vec4( mdl_Vertex,1.0 );
	vec4 v_screenPos = vec4( mdl_Vertex, 1.0 );
	
	v2f_position	= vec4( mdl_Vertex, 1.0 );
	v2f_texcoord0	= mdl_TexCoord.xy;

	gl_Position = v_screenPos;
}