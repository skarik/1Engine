in vec3 mdl_Vertex;
in vec3 mdl_Normal;

// Outputs to fragment shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying float v2f_fogdensity;

// System inputs
uniform vec4	outline_Color;
uniform float	outline_Width;

// Main vertex program
void main ( void )
{
	vec4 v_vertPos	= vec4( mdl_Vertex, 1.0 ) + ( outline_Width * vec4(mdl_Normal,0.0) );
	vec4 v_screenPos = gl_ModelViewProjectionMatrix * v_vertPos;

	v2f_normals		= vec4( gl_NormalMatrix*mdl_Normal,1 );
	v2f_colors		= outline_Color;
	v2f_fogdensity  = max(0,(gl_Fog.end - v_screenPos.z) * gl_Fog.scale);

	gl_Position = v_screenPos;
}