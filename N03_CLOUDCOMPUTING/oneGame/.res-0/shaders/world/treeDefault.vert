#version 140

in vec3 mdl_Vertex;
in vec3 mdl_TexCoord;
in vec4 mdl_Color;
in vec3 mdl_Normal;
// Model inputs
in vec2 mdl_TexCoord4;	// Used for weather state

// Outputs to fragment shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec3 v2f_texcoord0;
varying float v2f_fogdensity;
varying vec2 v2f_weatherdensity;
varying vec4 v2f_localposition;

// System inputs
uniform mat4 sys_ModelMatrix;
uniform mat4 sys_ModelRotationMatrix;
uniform mat4 sys_ModelViewProjectionMatrix;
uniform vec4 sys_SinTime, sys_CosTime;

// Fog
layout(std140) uniform sys_Fog
{
	vec4	sys_FogColor;
	vec4	sys_AtmoColor;
	float 	sys_FogEnd;
	float 	sys_FogScale;
};

void main ( void )
{
	vec4 v_localPos = vec4( mdl_Vertex, 1.0 );
	v_localPos.x += sys_SinTime.y * mdl_Color.z * mdl_Color.y * 0.004;
	v_localPos.y += sys_CosTime.z * mdl_Color.z * mdl_Color.x * 0.004;
	vec4 v_screenPos = sys_ModelViewProjectionMatrix * v_localPos;

	v2f_normals		= sys_ModelRotationMatrix*vec4( mdl_Normal, 0 );
	v2f_colors		= vec4( 1,1,1,1 );
	v2f_localposition = mdl_Color;
	v2f_position	= sys_ModelMatrix*v_localPos;
	v2f_texcoord0	= mdl_TexCoord.xyz; //vec3( gl_MultiTexCoord0.xy, 0.25 );
	v2f_fogdensity  = max( 0.0, (sys_FogEnd - v_screenPos.z) * sys_FogScale );
	v2f_weatherdensity = mdl_TexCoord4;

	gl_Position = v_screenPos;
}