
vec4 mainLighting ( void )
{
	return vec4(
		sys_LightingOverrides.r,
		(sys_SpecularColor.r + sys_SpecularColor.g + sys_SpecularColor.b) / 3,
		//length(sys_SpecularColor.rgb) / 1.73,
		sys_SpecularColor.a,
		sys_LightingOverrides.g );
}

// colorLighting
// r	lighting effect (0 is fullbright, 1 is normal)
// g	specular add (from uniform)
// b	specular power (from unifrom)
// a	rim lighting strength