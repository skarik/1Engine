#version 330

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

uniform vec4 sys_SinTime, sys_CosTime, sys_Time;

// Fog
layout(std140) uniform sys_Fog
{
	vec4	sys_FogColor;
	float 	sys_FogEnd;
	float 	sys_FogScale;
};


void main ( void )
{
	v2f_colors		= mdl_Color;
	
	vec4 v_localPos = vec4( mdl_Vertex, 1.0 );
	vec3 offset = vec3(0,0,0);
	offset.x += sys_SinTime.w;
	offset.y += sys_CosTime.z;
	offset.z += sys_SinTime.z*0.2;
	offset.z += cos( sys_Time.z*0.4+v_localPos.x*14.2+v_localPos.y*13.2 )*0.8;
	
	v_localPos.xyz += offset*(1.0-v2f_colors.g)*0.09;
	
	vec4 v_screenPos = sys_ModelViewProjectionMatrix * v_localPos;

	v2f_normals		= sys_ModelRotationMatrix*vec4( mdl_Normal, 1.0 );
	
	v2f_position	= sys_ModelMatrix*v_localPos;
	v2f_texcoord0	= mdl_TexCoord.xy;
	v2f_fogdensity  = max( 0.0, (sys_FogEnd - v_screenPos.z) * sys_FogScale );

	gl_Position = v_screenPos;
}