in vec3 mdl_Vertex;
in vec3 mdl_Normal;
in vec4 mdl_Color;
in vec3 mdl_TexCoord;

in vec3 mdl_Tangents;
in vec3 mdl_Binormals;

in vec3 mdl_TexCoord2;
in vec3 mdl_TexCoord3;
in vec3 mdl_TexCoord4;

in vec4 mdl_BoneWeights;
in uvec4 mdl_BoneIndices;

// Skinning
layout(std140) uniform sys_SkinningDataMajor
{
	mat4 majorSkinning [256];
};
/*layout(std140) uniform sys_SkinningDataMinor
{
	mat4 minorSkinning [256];
};*/