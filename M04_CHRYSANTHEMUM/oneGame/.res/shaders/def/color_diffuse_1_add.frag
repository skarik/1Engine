

vec4 mainDiffuse ( void )
{
	vec4 color = texture( textureSampler0, v2f_texcoord.xy ) * sys_DiffuseColor;
	return vec4( color.rgb, color.a*length(color.rgb)/1.73 );
}