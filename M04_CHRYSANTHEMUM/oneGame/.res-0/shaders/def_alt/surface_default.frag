#version 330

// System outputs
layout(location = 0) out vec4 FragDiffuse;
layout(location = 1) out vec4 FragNormals;
layout(location = 2) out vec4 FragLighting;
layout(location = 3) out vec4 FragGlowmap;

// Fog Iinput
layout(std140) uniform sys_Fog
{
	vec4	sys_FogColor;
	vec4	sys_AtmoColor;
	float 	sys_FogEnd;
	float 	sys_FogScale;
};

// System Inputs
uniform vec4 sys_DiffuseColor;
uniform vec4 sys_SpecularColor;
uniform vec3 sys_EmissiveColor;

uniform vec3 sys_WorldCameraPos;
uniform vec3 sys_LightingOverrides;

// Samplers
uniform sampler2D textureSampler0; // Diffuse RGB, Specular/Discard A
uniform sampler2D textureSampler1; // Normals RG, Height B, Ambient Occlusion A
uniform sampler2D textureSampler2; // Discard A
uniform sampler2D textureSampler3; // Overlay RGB, Blend Style A (0 for MUL-BIAS, 1 for LERP)

// Vertex Outputs
in vec4 v2f_normals;
in vec4 v2f_colors;
in vec4 v2f_position;
in vec3 v2f_texcoord;

uniform float sys_AlphaCutoff;


vec4 mainDiffuse ( vec4 colorDiffuse, vec4 colorOverlay )
{
	/*vec4 result = texture( textureSampler0, v2f_texcoord.xy ) * sys_DiffuseColor;
	if ( result.a < sys_AlphaCutoff ) {
		discard;
	}
	return vec4( result.rgb, 1.0 );*/

    vec4 result = colorDiffuse;
    // MUL-BIAS
    result.rgb *= mix( min(colorOverlay.rgb * 2.0, 1.0), vec3(1.0,1.0,1.0), colorOverlay.a );
    // MIX
    result.rgb += (colorOverlay.rgb - result.rgb) * colorOverlay.a;

    return vec4( result.rgb, 1.0 );
}

vec4 mainGlowmap ( vec4 colorOverlay )
{
	//return vec4( texture( textureSampler1, v2f_texcoord.xy ).rgb, v2f_colors.g );
    return vec4( max(colorOverlay.rgb - vec3(0.5,0.5,0.5), 0.0) * 2.0 * colorOverlay.a, v2f_colors.g );
}
vec4 mainLighting ( void )
{
	return vec4(
		sys_LightingOverrides.r,
		(sys_SpecularColor.r + sys_SpecularColor.g + sys_SpecularColor.b) / 3,
		sys_SpecularColor.a,
		sys_LightingOverrides.g );
}

vec4 mainNormals ( vec4 colorNormals )
{
    vec4 baseNormal = vec4( v2f_normals.xyz, 1.0 );

    // TODO: Use normal map properly
    baseNormal.xy = colorNormals.xy * 2.0 - 1.0; // Unbias the normal
    baseNormal.z = sqrt(1.0 - dot(baseNormal.xy,baseNormal.xy));

    return baseNormal;
}

// colorLighting
// r	lighting effect (0 is fullbright, 1 is normal)
// g	specular add (from uniform)
// b	specular power (from unifrom)
// a	rim lighting strength

void main ( void )
{
    vec4 colorDiffuse = texture( textureSampler0, v2f_texcoord.xy );
    if ( colorDiffuse.a < sys_AlphaCutoff ) discard;
    vec4 colorDiscard = texture( textureSampler2, v2f_texcoord.xy );
    if ( colorDiscard.a < 0.5 ) discard;
    vec4 colorNormals = texture( textureSampler1, v2f_texcoord.xy );
    vec4 colorOverlay = texture( textureSampler3, v2f_texcoord.xy );

    /*vec4 colorDiffuse = mainDiffuse();
    vec4 colorGlowmap = mainGlowmap();
    vec4 colorNormals = v2f_normals;
    //vec4 colorPosition= v2f_position;
    //colorNormals.w = v2f_position.w;
    colorNormals.w = 1.0;
    vec4 colorLighting= mainLighting();*/

    FragDiffuse  = mainDiffuse(colorDiffuse, colorOverlay);
    FragNormals  = mainNormals(colorNormals);
    FragLighting = mainLighting();
    FragGlowmap  = mainGlowmap(colorOverlay);
}
