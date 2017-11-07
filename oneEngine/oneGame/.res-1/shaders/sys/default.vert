// sys/default
// Default forward rendering shader.
#version 330
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

in vec3 mdl_Vertex;
in vec3 mdl_TexCoord;
in vec4 mdl_Color;
in vec3 mdl_Normal;

// Outputs to fragment shader
out vec4 v2f_colors;
out vec4 v2f_position;
out vec2 v2f_texcoord0;
out float v2f_fogdensity;
/*
// System inputs
uniform mat4 sys_ModelTRS;
uniform mat4 sys_ModelRS;
uniform mat4 sys_ModelViewProjectionMatrix;

layout(location = 0) uniform vec4 sys_DiffuseColor;
layout(location = 1) uniform vec3 sys_EmissiveColor;

// Fog
layout(std140) uniform sys_Fog
{
	vec4	sys_FogColor;
	float 	sys_FogEnd;
	float 	sys_FogScale;
};*/

layout(std140) uniform sys_cbuffer_PerObject
{
    mat4 sys_ModelTRS;
    mat4 sys_ModelRS;
    mat4 sys_ModelViewProjectionMatrix;
    mat4 sys_ModelViewProjectionMatrixInverse;
};
layout(std140) uniform sys_cbuffer_PerObjectExt
{
    vec4    sys_DiffuseColor;
    vec4    sys_SpecularColor;
    vec3    sys_EmissiveColor;
    float   sys_AlphaCutoff;
    vec4    sys_LightingOverrides;

    vec4    sys_TextureScale;
    vec4    sys_TextureOffset;
};
layout(std140) uniform sys_cbuffer_PerFrame
{
    // Time inputs
    vec4    sys_SinTime;
    vec4    sys_CosTime;
    vec4    sys_Time;

    // Fog
	vec4	sys_FogColor;
	vec4	sys_AtmoColor;
	float 	sys_FogEnd;
	float 	sys_FogScale;
};

/*
uniform vec4 sys_LightAmbient;
uniform vec4 sys_LightColor[8];
uniform vec4 sys_LightPosition[8];
uniform vec4 sys_LightProperties[8];
*/
/*
float diffuseLighting ( vec3 normal, vec3 lightDist, float lightRange, float lightFalloff, float lightPass )
{
	// Distance-based attenuation
	float attenuation = pow( max( 1.0 - (length( lightDist )*lightRange), 0.0 ), lightFalloff );

	// Cosine law * attenuation
	float color = mix( dot( normal,normalize( lightDist ) ), 1.0, lightPass*(1.0+attenuation) ) * attenuation;

	// Return final color
	return color;
}
*/

void main ( void )
{
	vec4 v_localPos = mdl_Vertex;
	vec4 v_screenPos = sys_ModelViewProjectionMatrix * v_localPos;

	vec4 v2f_normals	= sys_ModelRS*vec4( mdl_Normal, 1.0 );
	v2f_colors			= sys_DiffuseColor*mdl_Color;
	v2f_position		= sys_ModelTRS*mdl_Vertex;
	v2f_texcoord0		= mdl_TexCoord.xy;
	v2f_fogdensity 	 	= max( 0.0, (sys_FogEnd.end - v_screenPos.z) * sys_FogScale.scale );

	gl_Position = v_screenPos;

	// Calculate lighting
	/*vec3 lightColor = sys_EmissiveColor;
	lightColor += sys_LightAmbient.rgb;

	for ( int i = 0; i < 8; i += 1 )
	{
		vec3 lightDir;
		lightDir = sys_LightPosition[i].xyz - v2f_position.xyz;

		float lightVal1 = diffuseLighting( v2f_normals.xyz, lightDir, sys_LightProperties[i].x, sys_LightProperties[i].y, sys_LightProperties[i].z );
		float lightVal2 = dot( sys_LightPosition[i].xyz, v2f_normals.xyz );
		float lightValf = mix( lightVal2, lightVal1, sys_LightPosition[i].w );
		//lightValf = cellShade(lightValf);

		lightColor += sys_LightColor[i].rgb * max( 0, lightValf );
	}

	v2f_colors.rgb *= lightColor;*/
}