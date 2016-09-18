in vec3 mdl_Vertex;
in vec3 mdl_Normal;
in vec4 mdl_Color;
in vec3 mdl_TexCoord;

in vec3 mdl_Tangents;
in vec3 mdl_Binormals;

in vec3 mdl_TexCoord2;
in vec3 mdl_TexCoord3;
in vec3 mdl_TexCoord4;

in int gl_InstanceID;

// Samplers
uniform samplerBuffer textureInstanceBuffer;

vec4 quat_conj(vec4 q)
{
	return vec4(-q.x, -q.y, -q.z, q.w);
}
vec4 quat_mult(vec4 qa, vec4 qb)
{
	vec4 qr;
	qr.w = qa.w*qb.w - qa.x*qb.x - qa.y*qb.y - qa.z*qb.z;
	qr.x = qa.w*qb.x + qa.x*qb.w - qa.y*qb.z + qa.z*qb.y;
	qr.y = qa.w*qb.y + qa.x*qb.z + qa.y*qb.w - qa.z*qb.x;
	qr.z = qa.w*qb.z - qa.x*qb.y + qa.y*qb.x + qa.z*qb.w;
	return qr;
}
vec4 quat_rotate_point(vec4 q, vec3 p)
{
	vec4 v = vec4(p, 0);
	vec4 qi = quat_conj(q);
	vec4 v0 = quat_mult(q,   v);
	return    quat_mult(v0, qi);
}
