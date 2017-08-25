
vec4 mainLighting ( void )
{
	return vec4(
		1,
		v2f_colors.g,
		sys_SpecularColor.a,
		0.2 );
}

// colorLighting
// r	lighting effect (0 is fullbright)
// g	specular add (from uniform)
// b	specular power (from unifrom)
// a	rim lighting strength