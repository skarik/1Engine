#version 140

in vec3 mdl_Vertex;
in vec3 mdl_TexCoord;
in vec4 mdl_Color;
in vec3 mdl_Normal;

// Outputs to fragment shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying float v2f_fogdensity;

// System inputs
uniform mat4 sys_ModelMatrix;
uniform mat4 sys_ModelRotationMatrix;
uniform mat4 sys_ModelViewProjectionMatrix;

// Fog
layout(std140) uniform sys_Fog
{
	vec4	sys_FogColor;
	float 	sys_FogEnd;
	float 	sys_FogScale;
};

void main ( void )
{
	vec4 v_localPos = vec4( mdl_Vertex, 1.0 );
	vec4 v_screenPos = sys_ModelViewProjectionMatrix * v_localPos;

	v2f_normals		= sys_ModelRotationMatrix*vec4( normalize(mdl_Normal), 1.0 );
	v2f_colors		= mdl_Color;
	v2f_position	= sys_ModelMatrix*v_localPos;
	v2f_texcoord0	= mdl_TexCoord.xy;
	v2f_fogdensity	= (sys_FogEnd - v_screenPos.z) * sys_FogScale;
	
	gl_Position = v_screenPos;
}









/*
 //Fluxxor cloth
 
 uniform vec4 sys_Time;

void main ( void )
{
	vec4 v_localPos = gl_Vertex;
	
	v2f_normals		= sys_ModelRotationMatrix*vec4( gl_Normal, 1.0 );
	v2f_colors		= gl_Color;
	//v2f_emissive	= gl_SecondaryColor.rgb;
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