#version 330

in vec3 mdl_Vertex;
in vec3 mdl_TexCoord;
in vec4 mdl_Color;
in vec3 mdl_Normal;

varying vec3 v2f_normals;
varying vec3 v2f_directional;
varying vec2 v2f_texcoord0;
varying vec3 v2f_position;

uniform mat4 sys_ModelMatrix;
uniform mat4 sys_ModelRotationMatrix;
uniform mat4 sys_ModelMatrixInverse;
uniform mat4 sys_ModelViewProjectionMatrix;

void main ( void )
{
	// Send transformed normals
	v2f_normals		= (sys_ModelRotationMatrix * vec4( mdl_Normal, 1.0 )).xyz;
	// Send untarnished normals
	v2f_directional	= mdl_Normal;
	// Send model texture coordinates
	v2f_texcoord0	= mdl_TexCoord.xy;
	// Send unset model vertices
	v2f_position	= mdl_Vertex;

	// Set vertex position
	gl_Position = sys_ModelViewProjectionMatrix * vec4( mdl_Vertex, 1.0 );
}