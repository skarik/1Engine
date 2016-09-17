#version 330

in vec3 mdl_Vertex;
in vec3 mdl_TexCoord;
in vec4 mdl_Color;
in vec3 mdl_Normal;

out vec3 v2f_directional;
out vec3 v2f_directional_projection;

uniform mat4 sys_ModelMatrix;
uniform mat4 sys_ModelRotationMatrix;
uniform mat4 sys_ModelMatrixInverse;
uniform mat4 sys_ModelViewProjectionMatrix;

void main ( void )
{
	// Send positions
	v2f_directional = normalize(mdl_Vertex);
	v2f_directional.z = -v2f_directional.z;

	// Send projection positions
	v2f_directional_projection = (sys_ModelViewProjectionMatrix * vec4( v2f_directional, 1.0 )).xyz;

	// Set vertex position
	gl_Position = sys_ModelViewProjectionMatrix * vec4( mdl_Vertex, 1.0 );
}
