in vec3 mdl_Vertex;
in vec3 mdl_TexCoord;
in vec4 mdl_Color;
in vec3 mdl_Normal;

varying vec3 v2f_normals;
varying vec3 v2f_directional;
varying vec2 v2f_texcoord0;

uniform mat4 sys_ModelMatrix;
uniform mat4 sys_ModelRotationMatrix;
uniform mat4 sys_ModelMatrixInverse;
uniform mat4 sys_ModelViewProjectionMatrix;

void main ( void )
{
	// Send untarnished normals
	v2f_directional = mdl_Normal;
	// Send transformed normals
	v2f_normals		= (sys_ModelRotationMatrix*vec4( mdl_Normal, 1.0 )).xyz;
	// Send texture coords (Why?)
	v2f_texcoord0	= mdl_TexCoord.xy;

	// Set vertex position
	gl_Position = sys_ModelViewProjectionMatrix * vec4( mdl_Vertex, 1.0 );
}