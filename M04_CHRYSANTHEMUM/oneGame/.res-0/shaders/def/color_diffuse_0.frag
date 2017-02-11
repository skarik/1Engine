

vec4 mainDiffuse ( void )
{
	return texture( textureSampler0, v2f_texcoord.xy ) * sys_DiffuseColor;
}