varying vec4 drawNormals;

void main ( void )  
{
	//gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
	gl_FragColor = drawNormals;
	gl_FragColor.r = ( dot( drawNormals, vec4( 0.0,0.0,-1.0,0.0 ) ) + 1 ) / 2;
	gl_FragColor.g = gl_FragColor.r;
	gl_FragColor.b = gl_FragColor.r;
}