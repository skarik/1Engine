in vec3 mdl_Vertex;

// System inputs
uniform mat4 sys_ModelViewProjectionMatrix;

void main ( void )
{
	vec4 v_localPos = vec4( mdl_Vertex, 1.0 );
	vec4 v_screenPos = sys_ModelViewProjectionMatrix * v_localPos;

	gl_Position = v_screenPos;
}