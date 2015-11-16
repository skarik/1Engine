// Outputs to fragment shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec3 v2f_emissive;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying float v2f_fogdensity;

// System inputs
uniform mat4 sys_ModelMatrix;
uniform mat4 sys_ModelRotationMatrix;

void main ( void )
{
	vec4 v_localPos = gl_Vertex;
	vec4 v_screenPos = gl_ModelViewProjectionMatrix * v_localPos;

	v2f_normals		= sys_ModelRotationMatrix*vec4( gl_Normal, 1.0 );
	v2f_colors		= gl_Color;
	v2f_emissive	= gl_SecondaryColor.rgb;
	v2f_position	= sys_ModelMatrix*gl_Vertex;
	v2f_texcoord0	= gl_MultiTexCoord0.xy;
	v2f_fogdensity  = max( 0.0, (gl_Fog.end - v_screenPos.z) * gl_Fog.scale );

	gl_Position = v_screenPos;
}

/* Fluxxor cloth

void main ( void )
{
	vec4 v_localPos = gl_Vertex;
	
	v2f_normals		= vec4( gl_Normal, 1.0 )*sys_ModelRotationMatrix;
	v2f_colors		= gl_Color;
	v2f_emissive	= gl_SecondaryColor.rgb;
	v2f_position	= sys_ModelMatrix*gl_Vertex;
	v2f_texcoord0	= gl_MultiTexCoord0.xy;
	

	v_localPos.xyz += v2f_normals.xyz*0.075;
	v_localPos.xyz += vec3( sin( v2f_normals.x*2.2 ), cos( v2f_normals.y*3.2 ), sin( v2f_normals.z*1.7 ) ) * 0.025;
	v_localPos.xyz += vec3(
		sin( (v2f_normals.y*2.4+sys_Time.y+v_localPos.z*3.4)*1.4 )*0.3, 
		cos( (v2f_normals.z*2.4+sys_Time.y+v_localPos.x*3.4)*1.5 )*0.3, 
		sin( (v2f_normals.x*2.4+sys_Time.y+v_localPos.y*3.4)*1.6 ) ) * 0.05;
	
	vec4 v_screenPos = gl_ModelViewProjectionMatrix * v_localPos;
	v2f_fogdensity  = max( 0.0, (gl_Fog.end - v_screenPos.z) * gl_Fog.scale );
	
	gl_Position = v_screenPos;
}*/