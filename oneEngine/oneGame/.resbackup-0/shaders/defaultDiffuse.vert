// Outputs to fragment shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying float v2f_fogdensity;

// System inputs
uniform mat4 sys_ModelMatrix;
uniform mat4 sys_ModelRotationMatrix;

void main ( void )
{
	vec4 v_screenPos = gl_ModelViewProjectionMatrix * gl_Vertex;

	v2f_normals		= vec4( gl_Normal, 1 )*sys_ModelRotationMatrix;
	v2f_colors		= gl_Color;
	v2f_position	= sys_ModelMatrix*gl_Vertex;
	v2f_texcoord0	= gl_MultiTexCoord0.xy;
	v2f_fogdensity  = max(0,(gl_Fog.end - v_screenPos.z) * gl_Fog.scale);

	gl_Position = v_screenPos;
}