

vec4 mainGlowmap ( void )
{
	return vec4( _temp_glowmapResult.rgb, v2f_colors.g );
}