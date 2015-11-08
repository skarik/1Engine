
uniform float sys_AlphaCutoff;
vec4 mainDiffuse ( void )
{
	vec4 result = texture( textureSampler0, v2f_texcoord.xy ) * sys_DiffuseColor;
	if ( result.a < sys_AlphaCutoff ) {
		discard;
	}
	return vec4( result.rgb, 1.0 );
}