vec4 v_localPos = vec4( mdl_Vertex, 1.0 );
// Vertex skinning
vec4 v_finalPos = vec4( 0,0,0,0 );
vec3 v_finalNorm= vec3( 0,0,0 );
mat4 v_tempMatrix;
{
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
}
// Vertex skinning 2
/*vec4 v_softPos = vec4( 0,0,0,0 );
vec3 v_softNorm= vec3( 0,0,0 );
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
*/
// Set the final result
//v_localPos = mix(v_finalPos,v_softPos, pow(mdl_Color.r,2) );// pow( min(max(abs(v_localPos.x)-0.3,0)*2,1.5), 2 ) );
// End vertex skinning
/*float _m_tempSoftBlender = 1-mdl_Color.r;
_m_tempSoftBlender *= _m_tempSoftBlender;
ov_vertex = mix(v_finalPos,v_softPos, _m_tempSoftBlender );
ov_normal = vec4( normalize( mix(v_finalNorm,v_softNorm,_m_tempSoftBlender) ), 1.0 );*/
ov_vertex = v_finalPos;
ov_normal = vec4( normalize( v_finalNorm ), 1.0 );