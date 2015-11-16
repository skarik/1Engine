
#define vert VERTEXSHADER
#define frag FRAGMENTSHADER

uniform mat4 ProjectionModelviewMatrix;
void vert ( void )
{
	gl_Position = ProjectionModelviewMatrix * gl_Vertex;
}


void frag ( void )
{
	gl_FragColor = vec4(1,1,0,1);
}