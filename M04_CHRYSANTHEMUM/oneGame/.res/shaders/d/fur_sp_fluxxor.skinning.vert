#version 140

in vec3 mdl_Vertex;
in vec3 mdl_TexCoord;
in vec4 mdl_Color;
in vec3 mdl_Normal;

in vec4 mdl_BoneWeights;
in uvec4 mdl_BoneIndices;

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

// Skinning
layout(std140) uniform sys_SkinningDataMajor
{
	mat4 majorSkinning [256];
};

void main ( void )
{
	vec4 v_localPos = vec4( mdl_Vertex, 1.0 );
	// Vertex skinning
	vec3 v_finalNorm= vec3( 0,0,0 );
	vec4 v_finalPos = vec4( 0,0,0,0 );
	mat4 v_tempMatrix;
	// Build matrix for bone0
	v_tempMatrix = majorSkinning[int(mdl_BoneIndices.x)];
		v_tempMatrix = transpose(v_tempMatrix);
	v_finalPos += v_tempMatrix * v_localPos * mdl_BoneWeights.x;
	v_finalNorm += mat3(v_tempMatrix) * mdl_Normal * mdl_BoneWeights.x;
	// Build matrix for bone1
	v_tempMatrix = majorSkinning[int(mdl_BoneIndices.y)];
		v_tempMatrix = transpose(v_tempMatrix);
	v_finalPos += v_tempMatrix * v_localPos * mdl_BoneWeights.y;
	v_finalNorm += mat3(v_tempMatrix) * mdl_Normal * mdl_BoneWeights.y;
	// Build matrix for bone2
	v_tempMatrix = majorSkinning[int(mdl_BoneIndices.z)];
		v_tempMatrix = transpose(v_tempMatrix);
	v_finalPos += v_tempMatrix * v_localPos * mdl_BoneWeights.z;
	v_finalNorm += mat3(v_tempMatrix) * mdl_Normal * mdl_BoneWeights.z;
	// Build matrix for bone3
	v_tempMatrix = majorSkinning[int(mdl_BoneIndices.w)];
		v_tempMatrix = transpose(v_tempMatrix);
	v_finalPos += v_tempMatrix * v_localPos * mdl_BoneWeights.w;
	v_finalNorm += mat3(v_tempMatrix) * mdl_Normal * mdl_BoneWeights.w;
	
	// Set the final result
	v_localPos = v_finalPos;
	// End vertex skinning
	v2f_colors		= mdl_Color;
	// Fluxxor vertex offset
	vec3 offset = vec3(0,0,0);
	offset.x += sys_SinTime.w;
	offset.y += sys_CosTime.z;
	offset.z += sys_SinTime.z*0.2;
	offset.z += cos( sys_Time.z*0.4+v_localPos.x*14.2+v_localPos.y*13.2 )*0.8;
	v_localPos.xyz += offset*(1-v2f_colors.g)*0.09;
	// End vertex offset
	
	vec4 v_screenPos = sys_ModelViewProjectionMatrix * v_localPos;

	v2f_normals		= sys_ModelRotationMatrix*vec4( normalize(v_finalNorm), 1.0 );
	
	v2f_position	= sys_ModelMatrix*v_localPos;
	v2f_texcoord0	= mdl_TexCoord.xy;
	v2f_fogdensity  = max( 0.0, (sys_FogEnd - v_screenPos.z) * sys_FogScale );

	gl_Position = v_screenPos;
}