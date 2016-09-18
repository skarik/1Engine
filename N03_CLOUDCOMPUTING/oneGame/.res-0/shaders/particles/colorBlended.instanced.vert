#version 330

// Inputs from system
in vec3 mdl_Vertex;
in vec2 mdl_TexCoord;
in vec4 mdl_Color;
in vec3 mdl_Normal;

in int gl_InstanceID;

// Samplers
uniform sampler2D textureSampler0;
uniform samplerBuffer textureInstanceBuffer;

// Outputs to fragment shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec3 v2f_emissive;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying float v2f_fogdensity;

// System inputs
uniform mat4 sys_ModelMatrix;
uniform mat4 sys_ModelRotationMatrix;
uniform mat4 sys_ModelViewProjectionMatrix;
uniform mat4 sys_ViewProjectionMatrix;

uniform vec4 sys_DiffuseColor;
uniform vec3 sys_EmissiveColor;

// Fog
layout(std140) uniform sys_Fog
{
	vec4	sys_FogColor;
	vec4	sys_AtmoColor;
	float 	sys_FogEnd;
	float 	sys_FogScale;
};

vec4 quat_conj(vec4 q)
{
	return vec4(-q.x, -q.y, -q.z, q.w);
}
vec4 quat_mult(vec4 qa, vec4 qb)
{
	vec4 qr;
	/*qr.x = (q1.w * q2.x) + (q1.x * q2.w) + (q1.y * q2.z) - (q1.z * q2.y);
	qr.y = (q1.w * q2.y) - (q1.x * q2.z) + (q1.y * q2.w) + (q1.z * q2.x);
	qr.z = (q1.w * q2.z) + (q1.x * q2.y) - (q1.y * q2.x) + (q1.z * q2.w);
	qr.w = (q1.w * q2.w) - (q1.x * q2.x) - (q1.y * q2.y) - (q1.z * q2.z);*/

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

void main ( void )
{

	vec4 inst_pos = texelFetch( textureInstanceBuffer, gl_InstanceID*2 + 0 );
	vec4 inst_rot = texelFetch( textureInstanceBuffer, gl_InstanceID*2 + 1 );

	vec4 v_localScaledPos = sys_ModelMatrix * vec4(mdl_Vertex,1.0);
	vec4 v_localRotatedPos = quat_rotate_point(inst_rot, v_localScaledPos.xyz);
	vec4 v_worldPos = v_localRotatedPos + inst_pos;
	v_worldPos.w = 1.0;

	//vec4 v_screenPos = sys_ModelViewProjectionMatrix * v_worldPos;
	vec4 v_screenPos = sys_ViewProjectionMatrix * v_worldPos;

	v2f_normals		= sys_ModelRotationMatrix*vec4( mdl_Normal, 1.0 );
	v2f_colors		= mdl_Color * sys_DiffuseColor;
	v2f_emissive	= sys_EmissiveColor;
	v2f_position	= v_localRotatedPos;
	v2f_texcoord0	= mdl_TexCoord.xy;
	v2f_fogdensity  = max( 0.0, (sys_FogEnd - v_screenPos.z) * sys_FogScale );

	gl_Position = v_screenPos;
}
