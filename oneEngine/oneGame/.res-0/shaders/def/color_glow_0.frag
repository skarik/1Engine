

vec4 mainGlowmap ( void )
{
	return vec4( texture( textureSampler1, v2f_texcoord.xy ).rgb, v2f_colors.g );
}