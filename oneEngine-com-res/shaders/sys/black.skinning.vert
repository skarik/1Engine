// sys/black
// Draws a purely black object.
#version 330
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

in vec3 mdl_Vertex;
in vec3 mdl_TexCoord;
in vec4 mdl_Color;

in vec4 mdl_BoneWeights;
in uvec4 mdl_BoneIndices;

// Outputs to fragment shader
out vec2 v2f_texcoord0;

// System inputs
uniform mat4 sys_ModelTRS;
uniform mat4 sys_ModelRS;
uniform mat4 sys_ModelViewProjectionMatrix;

// Skinning
layout(std140) uniform sys_SkinningDataMajor
{
	mat4 majorSkinning [256];
};

void main ( void )
{
	vec4 v_localPos = vec4( mdl_Vertex, 1.0 );
	// Vertex skinning
	vec4 v_finalPos = vec4( 0,0,0,0 );
	mat4 v_tempMatrix;
	// Build matrix for bone0
	{
		v_tempMatrix = majorSkinning[int(mdl_BoneIndices.x)];
			v_tempMatrix = transpose(v_tempMatrix);
		v_finalPos += v_tempMatrix * v_localPos * mdl_BoneWeights.x;
		// Build matrix for bone1
		v_tempMatrix = majorSkinning[int(mdl_BoneIndices.y)];
			v_tempMatrix = transpose(v_tempMatrix);
		v_finalPos += v_tempMatrix * v_localPos * mdl_BoneWeights.y;
		// Build matrix for bone2
		v_tempMatrix = majorSkinning[int(mdl_BoneIndices.z)];
			v_tempMatrix = transpose(v_tempMatrix);
		v_finalPos += v_tempMatrix * v_localPos * mdl_BoneWeights.z;
		// Build matrix for bone3
		v_tempMatrix = majorSkinning[int(mdl_BoneIndices.w)];
			v_tempMatrix = transpose(v_tempMatrix);
		v_finalPos += v_tempMatrix * v_localPos * mdl_BoneWeights.w;
	}
	// Vertex skinning 2
	/*vec4 v_softPos = vec4( 0,0,0,0 );
	// Build matrix for bone0
	{
		v_tempMatrix = minorSkinning[int(mdl_BoneIndices.x)];
			v_tempMatrix = transpose(v_tempMatrix);
		v_softPos += v_tempMatrix * v_localPos * mdl_BoneWeights.x;
		// Build matrix for bone1
		v_tempMatrix = minorSkinning[int(mdl_BoneIndices.y)];
			v_tempMatrix = transpose(v_tempMatrix);
		v_softPos += v_tempMatrix * v_localPos * mdl_BoneWeights.y;
		// Build matrix for bone2
		v_tempMatrix = minorSkinning[int(mdl_BoneIndices.z)];
			v_tempMatrix = transpose(v_tempMatrix);
		v_softPos += v_tempMatrix * v_localPos * mdl_BoneWeights.z;
		// Build matrix for bone3
		v_tempMatrix = minorSkinning[int(mdl_BoneIndices.w)];
			v_tempMatrix = transpose(v_tempMatrix);
		v_softPos += v_tempMatrix * v_localPos * mdl_BoneWeights.w;
	}*/

	// Set the final result
	v_localPos = v_finalPos;//mix(v_finalPos,v_softPos, pow(mdl_Color.r,2) );
	vec4 v_screenPos = sys_ModelViewProjectionMatrix * v_localPos;

	v2f_texcoord0	= mdl_TexCoord.xy;

	gl_Position = v_screenPos;
}
