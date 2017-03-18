// Outputs to fragment shader
varying vec3 v2f_differentials;
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec3 v2f_texcoord0;

// System inputs
uniform mat4 sys_ModelTRS;
uniform mat4 sys_ModelRS;

// normal.x is -1 to 1
// normal.y is -1 to 1
// normal.z is (ysize/xsize)

// texcoord.z is vsize
// texcoord.w is ysize

void main ( void )
{
	/*vec4 v_localPos = gl_Vertex;
	
	float sampleWidth = 0.1;

	v2f_differentials = vec3( gl_Normal.x, gl_Normal.y, 0 );
	
	vec2 temp_differential = vec2( v2f_differentials.x/gl_Normal.z, v2f_differentials.y );
	
	v2f_colors		= gl_Color;
	v2f_position	= sys_ModelTRS*gl_Vertex;
	v2f_texcoord0.xy= gl_MultiTexCoord0.xy + temp_differential * gl_MultiTexCoord0.z * sampleWidth;
	
	v_localPos.xy = v_localPos.xy + temp_differential*gl_MultiTexCoord0.w * sampleWidth;

	v2f_differentials = v2f_differentials*(1+sampleWidth);
	
	vec4 v_screenPos = gl_ModelViewProjectionMatrix * v_localPos;
	gl_Position = v_screenPos;*/
	
		vec4 v_localPos = gl_Vertex;
	vec4 v_screenPos = gl_ModelViewProjectionMatrix * v_localPos;

	v2f_colors		= gl_Color;
	v2f_position	= sys_ModelTRS*gl_Vertex;
	v2f_texcoord0.xy= gl_MultiTexCoord0.xy;
	
	gl_Position = v_screenPos;
}