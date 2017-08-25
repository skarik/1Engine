
vec4 sampleWithNormal ( sampler3D sampler, vec3 normal, vec3 position, float depth, float scale )
{
	// From GPU Gems 3
	vec4 colorZ = texture( sampler, vec3(position.xy*scale,depth) );
	vec4 colorX = texture( sampler, vec3(position.yz*scale,max(depth,0.09375)) );
	vec4 colorY = texture( sampler, vec3(position.xz*scale,max(depth,0.09375)) );
	vec3 blend_weights = abs( normal.xyz );   // Tighten up the blending zone:  
	blend_weights = pow(blend_weights,vec3(6,6,6));
	blend_weights = max(blend_weights, 0);      // Force weights to sum to 1.0 (very important!)  
	blend_weights /= (blend_weights.x + blend_weights.y + blend_weights.z );
	// now, get color
	vec4 color =	colorX.xyzw * blend_weights.xxxx +  
					colorY.xyzw * blend_weights.yyyy +  
					colorZ.xyzw * blend_weights.zzzz;  
	return color;
}

vec4 mainDiffuse ( void )
{
	//return texture( textureSampler0, v2f_texcoord.xy ) * sys_DiffuseColor;
	vec4 diffuseColor0 = sampleWithNormal( textureSampler0, v2f_normals.xyz, v2f_position.xyz, v2f_texcoord.x, v2f_ter_blends2.x );
	vec4 diffuseColor1 = sampleWithNormal( textureSampler0, v2f_normals.xyz, v2f_position.xyz, v2f_texcoord.y, v2f_ter_blends2.y );
	vec4 diffuseColor2 = sampleWithNormal( textureSampler0, v2f_normals.xyz, v2f_position.xyz, v2f_texcoord.z, v2f_ter_blends2.z );
	vec4 diffuseColor = diffuseColor0*v2f_ter_blends1.x + diffuseColor1*v2f_ter_blends1.y + diffuseColor2*v2f_ter_blends1.z;
	return vec4(diffuseColor.rgb*0.707,1);
}
