#version 330

// Inputs from vertex shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec3 v2f_emissive;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying float v2f_fogdensity;

// Samplers
uniform sampler2D textureSampler0;
uniform samplerBuffer textureInstanceBuffer;

// Constants
uniform float sys_AlphaCutoff;

// Fog
layout(std140) uniform sys_Fog
{
	vec4	sys_FogColor;
	vec4	sys_AtmoColor;
	float 	sys_FogEnd;
	float 	sys_FogScale;
};

void main ( void )
{
	vec4 diffuseColor = texture2D( textureSampler0, v2f_texcoord0 );
	float f_alpha = diffuseColor.a * v2f_colors.a;
	if ( f_alpha < sys_AlphaCutoff ) discard;

	gl_FragColor.rgb = mix( sys_FogColor.rgb, diffuseColor.rgb * (v2f_colors.rgb+v2f_emissive), v2f_fogdensity ) * f_alpha;
	gl_FragColor.a = f_alpha;
}
