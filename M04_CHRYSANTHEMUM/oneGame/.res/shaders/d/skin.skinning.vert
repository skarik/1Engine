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

// Skinning
layout(std140) uniform sys_SkinningDataMajor
{
	mat4 majorSkinning [256];
};
layout(std140) uniform sys_SkinningDataMinor
{
	mat4 minorSkinning [256];
};

// Lighting and Shadows
layout(std140) uniform sys_LightingInfo
{
	vec4 sys_LightColor[8];
	vec4 sys_LightPosition[8];
	vec4 sys_LightProperties[8];
	vec4 sys_LightShadowInfo[8];
	mat4 sys_LightMatrix[8];
};
varying vec4 v2f_lightcoord[8];
varying vec3 v2f_screenpos;

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
	// Vertex skinning
	vec3 v_finalNorm= vec3( 0,0,0 );
	vec4 v_finalPos = vec4( 0,0,0,0 );
	mat4 v_tempMatrix;
	// Build matrix for bone0
	{
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
	}
	// Vertex skinning 2
	vec3 v_softNorm= vec3( 0,0,0 );
	vec4 v_softPos = vec4( 0,0,0,0 );
	// Build matrix for bone0
	{
		v_tempMatrix = minorSkinning[int(mdl_BoneIndices.x)];
			v_tempMatrix = transpose(v_tempMatrix);
		v_softPos += v_tempMatrix * v_localPos * mdl_BoneWeights.x;
		v_softNorm += mat3(v_tempMatrix) * mdl_Normal * mdl_BoneWeights.x;
		// Build matrix for bone1
		v_tempMatrix = minorSkinning[int(mdl_BoneIndices.y)];
			v_tempMatrix = transpose(v_tempMatrix);
		v_softPos += v_tempMatrix * v_localPos * mdl_BoneWeights.y;
		v_softNorm += mat3(v_tempMatrix) * mdl_Normal * mdl_BoneWeights.y;
		// Build matrix for bone2
		v_tempMatrix = minorSkinning[int(mdl_BoneIndices.z)];
			v_tempMatrix = transpose(v_tempMatrix);
		v_softPos += v_tempMatrix * v_localPos * mdl_BoneWeights.z;
		v_softNorm += mat3(v_tempMatrix) * mdl_Normal * mdl_BoneWeights.z;
		// Build matrix for bone3
		v_tempMatrix = minorSkinning[int(mdl_BoneIndices.w)];
			v_tempMatrix = transpose(v_tempMatrix);
		v_softPos += v_tempMatrix * v_localPos * mdl_BoneWeights.w;
		v_softNorm += mat3(v_tempMatrix) * mdl_Normal * mdl_BoneWeights.w;
	}
	
	// Set the final result
	v_localPos = mix(v_finalPos,v_softPos, pow(1-mdl_Color.r,2) );// pow( min(max(abs(v_localPos.x)-0.3,0)*2,1.5), 2 ) );
	// End vertex skinning
	vec4 v_screenPos = sys_ModelViewProjectionMatrix * v_localPos;

	v2f_normals		= sys_ModelRotationMatrix * vec4( normalize(v_finalNorm), 1.0 );
	v2f_colors		= mdl_Color;
	v2f_position	= sys_ModelMatrix * v_localPos;
	v2f_texcoord0	= mdl_TexCoord.xy;
	v2f_fogdensity = clamp( (sys_FogEnd - v_screenPos.z) * sys_FogScale, 0, 1 );

	v2f_screenpos	= vec3( v_screenPos.x,v_screenPos.y,v_screenPos.w );
	v2f_lightcoord[0] = (v2f_position * sys_LightMatrix[0]);
	v2f_lightcoord[1] = (v2f_position * sys_LightMatrix[1]);
	v2f_lightcoord[2] = (v2f_position * sys_LightMatrix[2]);
	v2f_lightcoord[3] = (v2f_position * sys_LightMatrix[3]);
	
	gl_Position = v_screenPos;
}