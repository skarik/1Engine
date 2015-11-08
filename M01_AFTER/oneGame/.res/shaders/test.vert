varying vec4 drawNormals;

in int gl_VertexID;
//in int gl_InstanceID;

void main ( void )
{
	vec4 a = gl_Vertex;
	//a.x = a.x * 0.5;
	//a.y = a.y * 0.5;
	//drawNormals = vec4( gl_Normal, 1 );
	
	// Moves the vertex into world position
	vec3 worldPos = vec3( gl_ModelViewMatrix * gl_Vertex );
	vec3 worldNorms = normalize ( gl_NormalMatrix * gl_Normal );
	
	drawNormals = vec4( worldNorms, 1 );

	gl_Position = gl_ModelViewProjectionMatrix * a;
}
