// Inputs from vertex shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
///varying vec4 v2f_position;
varying float v2f_fogdensity;

void main ( void )  
{
	// Make value affected by fog. Otherwise, no edits to the outline color
	gl_FragColor = mix( gl_Fog.color, v2f_colors, v2f_fogdensity );
}