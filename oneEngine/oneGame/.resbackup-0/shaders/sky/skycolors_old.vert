varying vec4 v2f_normals;
varying vec2 v2f_texcoord0;

uniform mat4 sys_ModelMatrix;
uniform mat4 sys_ModelRotationMatrix;
uniform mat4 sys_ModelMatrixInverse;

void main ( void )
{
	v2f_normals		= sys_ModelRotationMatrix*vec4( gl_Normal, 1.0 );
	v2f_normals.w	= gl_Normal.x;
	v2f_texcoord0	= gl_MultiTexCoord0.xy;

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}