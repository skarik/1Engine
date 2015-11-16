
vec4 _temp_glowmapResult;

// Game Inputs
uniform float	gm_FadeValue = 1;
uniform float	gm_HalfScale = 0.5;

vec4 mainDiffuse ( void )
{
	vec4 t_fadeValue = texture( textureSampler2, v2f_texcoord.xy );		   
	float t_fadeDif = t_fadeValue.r - (gm_FadeValue-0.17);
	if ( t_fadeDif < 0 ) {
		discard;
	}

	vec4 diffuseColor = texture( textureSampler0, v2f_texcoord.xy );
	diffuseColor.rgb -= (0.917-diffuseColor.rgb)*(1-sys_DiffuseColor.rgb); // this line makes texture more pronoused without mucking everything up
	vec4 tattooColor = texture( textureSampler3, vec2(v2f_texcoord.x*gm_HalfScale,v2f_texcoord.y) ) * diffuseColor;

	// Glowing tattoos. Maximize the emissive color.
	_temp_glowmapResult.rgb = (tattooColor.rgb-0.5)*tattooColor.a*2;
	_temp_glowmapResult.a = 1.0;
	
	// Texture and tattoo colors
	diffuseColor = diffuseColor * sys_DiffuseColor;
	float lerpValue = clamp( (length(tattooColor.rgb)-length(vec3(0.51,0.51,0.51)))*2, 0, 1 ); // Calculate color for glowing tattoos
	vec3 targetTattooColor = min(vec3(1,1,1), mix( tattooColor.rgb*2, (tattooColor.rgb-0.5)+diffuseColor.rgb, lerpValue ) );
	diffuseColor.rgb += ( targetTattooColor - diffuseColor.rgb)*tattooColor.a; // Glowing tattoos don't make diffuse go over 1
	
	return diffuseColor;
}