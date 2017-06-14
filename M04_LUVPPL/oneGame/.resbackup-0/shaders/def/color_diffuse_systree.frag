

vec4 mainDiffuse ( void )
{
	return texture( textureSampler1, v2f_texcoord ) * sys_DiffuseColor;
}