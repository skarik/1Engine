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
varying vec2 v2f_texcoord1;
varying float v2f_fogdensity;

// System inputs
uniform mat4 sys_ModelMatrix;
uniform mat4 sys_ModelMatrixInverse;
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
	vec4 v_localPos = vec4( mdl_Vertex, 1.0 );
	vec4 v_screenPos = sys_ModelViewProjectionMatrix * v_localPos;

	v2f_normals		= sys_ModelRotationMatrix * vec4( mdl_Normal, 1.0 );
	v2f_normals = normalize(v2f_normals);
	v2f_colors		= mdl_Color;
	v2f_position	= sys_ModelMatrix * v_localPos;
	v2f_texcoord0	= mdl_TexCoord.xy + vec2( sys_SinTime.y*0.3 - sys_Time.y*0.4, sys_CosTime.x + sys_Time.y*0.4 );
	v2f_texcoord1	= mdl_TexCoord.xy + vec2( sys_CosTime.x + sys_Time.y*0.8, sys_SinTime.y*0.3 - sys_Time.y*0.8 );
	v2f_texcoord1 *= 2.3;
	v2f_fogdensity  = max( 0.0, (sys_FogEnd - v_screenPos.z) * sys_FogScale );

	gl_Position = v_screenPos;
}