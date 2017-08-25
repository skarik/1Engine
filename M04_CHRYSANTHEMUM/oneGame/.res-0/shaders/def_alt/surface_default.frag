#version 330
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

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
layout(location = 0) uniform vec4 sys_DiffuseColor;
layout(location = 1) uniform vec4 sys_SpecularColor;
layout(location = 2) uniform vec3 sys_EmissiveColor;
layout(location = 3) uniform float sys_AlphaCutoff;
layout(location = 4) uniform vec3 sys_LightingOverrides;

uniform vec3 sys_WorldCameraPos;

uniform mat4 sys_ModelRS;

// Samplers
layout(location = 20) uniform sampler2D textureSampler0; // Diffuse RGB, Specular/Discard A
layout(location = 21) uniform sampler2D textureSampler1; // Normals RGB, Height A
layout(location = 22) uniform sampler2D textureSampler2; // Ambient Occlusion R, Metallic G, Smoothness B, Discard A
layout(location = 23) uniform sampler2D textureSampler3; // Overlay RGB, Blend Style A (0 for MUL-BIAS, 1 for LERP)

// Vertex Outputs
in vec4 v2f_normals;
in vec4 v2f_colors;
in vec4 v2f_position;
in vec3 v2f_texcoord;

vec4 mainDiffuse ( vec4 colorDiffuse, vec4 colorOverlay, vec4 surfaceProperties )
{
    // pixelDiffuse - Generate via pulling from the palette.
    // rgb  surface color

    // Start with a diffuse pushed to zero by metallicness
    /*vec4 result = colorDiffuse * sys_DiffuseColor * (1.0 - surfaceProperties.g);
    // MUL-BIAS
    result.rgb *= mix( min(colorOverlay.rgb * 2.0, 1.0), vec3(1.0,1.0,1.0), colorOverlay.a );
    // MIX
    result.rgb += (colorOverlay.rgb - result.rgb) * colorOverlay.a;*/

    // Start with input palette
    vec4 result = colorDiffuse;
    // Push palette brightness down
    result.r *= sys_DiffuseColor.r * (1.0 - surfaceProperties.g);

    return vec4( result.rgb, 1.0 );
}

vec4 mainGlowmap ( vec4 colorOverlay )
{
    // pixelGlow
	// rgb	surface glow
	// a	rim lighting strength

    return vec4(
        max(colorOverlay.rgb - vec3(0.5,0.5,0.5), 0.0) * 2.0 * colorOverlay.a,
        sys_LightingOverrides.g );
}
vec4 mainLighting ( vec4 colorDiffuse, vec4 surfaceProperties )
{
    // pixelLightProperty
    // rgb  specular color
	// a	smoothness

    /*return vec4(
		colorDiffuse.rgb * surfaceProperties.g * 2.0,
		surfaceProperties.b );*/
    return vec4(
        // Instead use a palette lookup RGB
		(colorDiffuse.r + 0.5) * surfaceProperties.g * 2.0,
        colorDiffuse.g,
        colorDiffuse.b,
		surfaceProperties.b );
}

vec4 mainNormals ( vec4 colorNormals, vec4 colorSurface )
{
    // pixelNormal
    // rgb	surface normal
    // a	ambient occlusion

    vec4 baseNormal = vec4( v2f_normals.xyz, 1.0 );
    // TODO: Use normal map properly
    baseNormal.xyz = colorNormals.xyz * 2.0 - 1.0;
    vec4 transformedNormal = sys_ModelRS * vec4(baseNormal.xyz, 1.0);

    return vec4(transformedNormal.xyz, colorSurface.r);
}

void main ( void )
{
    vec4 colorDiffuse = texture( textureSampler0, v2f_texcoord.xy );
    if ( colorDiffuse.a < sys_AlphaCutoff ) discard;
    vec4 colorSurface = texture( textureSampler2, v2f_texcoord.xy );
    if ( colorSurface.a < 0.5 ) discard;
    vec4 colorNormals = texture( textureSampler1, v2f_texcoord.xy );
    vec4 colorOverlay = texture( textureSampler3, v2f_texcoord.xy );

    FragDiffuse  = mainDiffuse(colorDiffuse, colorOverlay, colorSurface);
    FragNormals  = mainNormals(colorNormals, colorSurface);
    FragLighting = mainLighting(colorDiffuse, colorSurface);
    FragGlowmap  = mainGlowmap(colorOverlay);
}
