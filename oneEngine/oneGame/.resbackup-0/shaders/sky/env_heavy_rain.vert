// Outputs to fragment shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying vec2 v2f_texcoordi;
varying float v2f_fogdensity;
varying vec4 v2f_offset;

// Sys
uniform vec4 sys_Time;

// System inputs
uniform mat4 sys_ModelMatrix;
uniform mat4 sys_ModelRotationMatrix;

void main ( void )
{
	vec4 v_localPos = gl_Vertex;
	v_localPos.z *= 2;
	vec4 v_screenPos = gl_ModelViewProjectionMatrix * v_localPos;

	v2f_normals		= sys_ModelRotationMatrix*vec4( gl_Normal, 1.0 );
	v2f_colors		= gl_Color;
	v2f_position	= sys_ModelMatrix*v_localPos;
	v2f_offset		= sys_ModelMatrix*vec4(0,0,0,1);
	v2f_texcoordi	= gl_MultiTexCoord0.xy;
	//v2f_texcoord0	= gl_MultiTexCoord0.xy;
	v2f_texcoord0	= vec2(
		sin(v2f_offset.x*2.1)+sin(v2f_offset.y*2.1)
		-v2f_position.x*v2f_normals.y
		+v2f_position.y*v2f_normals.x,-v2f_position.z )*0.09;
	v2f_texcoord0.y -= sys_Time.z*0.8;
	v2f_fogdensity  = max( 0.0, (gl_Fog.end - v_screenPos.z) * gl_Fog.scale );

	gl_Position = v_screenPos;
}