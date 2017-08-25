#version 140

in vec3 mdl_Vertex;
in vec3 mdl_TexCoord;
in vec4 mdl_Color;
in vec3 mdl_Normal;

in vec4 ter_Blends1;
in vec4 ter_Blends2;

varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec3 v2f_texcoord0;
varying float v2f_fogdensity;
varying vec3 v2f_screenpos;

varying vec4 v2f_ter_blends1;
varying vec4 v2f_ter_blends2;

uniform mat4 sys_ModelMatrix;
uniform mat4 sys_ModelRotationMatrix;
uniform mat4 sys_ModelViewProjectionMatrix;

// Lighting and Shadows
uniform vec4 sys_LightAmbient;
layout(std140) uniform sys_LightingInfo
{
	vec4 sys_LightColor[8];
	vec4 sys_LightPosition[8];
	vec4 sys_LightProperties[8];
	vec4 sys_LightShadowInfo[8];
	mat4 sys_LightMatrix[8];
};

varying vec4 v2f_lightcoord[8];

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
	v2f_colors		= mdl_Color;
	v2f_position	= sys_ModelMatrix*v_localPos;
	
	v2f_texcoord0.xyz = (1+2*mdl_TexCoord.xyz)/(2*16);
	
	
	v2f_fogdensity  = (sys_FogEnd - v_screenPos.z) * sys_FogScale;

	// Terrain blending values
	v2f_ter_blends1 = ter_Blends1;
	v2f_ter_blends1.xyz /= (v2f_ter_blends1.x+v2f_ter_blends1.y+v2f_ter_blends1.z);
	v2f_ter_blends2 = ter_Blends2;
	
	// Light matrices
	v2f_screenpos = vec3( v_screenPos.x,v_screenPos.y,v_screenPos.w );
	v2f_lightcoord[0] = (v2f_position * sys_LightMatrix[0]);
	v2f_lightcoord[1] = (v2f_position * sys_LightMatrix[1]);
	v2f_lightcoord[2] = (v2f_position * sys_LightMatrix[2]);
	v2f_lightcoord[3] = (v2f_position * sys_LightMatrix[3]);

	gl_Position = v_screenPos;
}