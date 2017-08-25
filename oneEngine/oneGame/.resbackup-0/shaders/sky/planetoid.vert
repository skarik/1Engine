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

varying vec3 v2f_screenpos;
varying vec3 v2f_screennormal;

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

	v2f_normals		= sys_ModelRotationMatrix*vec4( mdl_Normal, 1.0 );
	v2f_colors		= mdl_Color;
	v2f_position	= sys_ModelMatrix*v_localPos;
	v2f_texcoord0	= mdl_TexCoord.xy;
	v2f_fogdensity  = 1.0;//max( 0.0, (gl_Fog.end - v_screenPos.z) * gl_Fog.scale );

	v2f_screenpos = v_screenPos.xyz;
	v2f_screennormal = mat3(sys_ModelViewProjectionMatrix) * mdl_Normal;//normalize(gl_NormalMatrix*mdl_Normal);
	v2f_screennormal = normalize(v2f_screennormal);
	//v_screenPos.z *= 0.9;
	gl_Position = v_screenPos;
}