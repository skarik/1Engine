
/*
// Samplers
uniform sampler2D textureSampler0;  // Base
uniform sampler2D textureSampler1; // Iris
uniform sampler2D textureSampler2; // Pupil
*/

vec4 _temp_glowmapResult;

float cellShade ( float lumin )
{
	const int levels = 2;
	float t = lumin*levels;
	float dif = 0.5 - mod(t+0.5,1);
	t += dif * min(1,(0.5-abs(dif))*32) * 0.5;
	
	lumin = t/levels;
	return lumin;
}

vec4 mainDiffuse ( void )
{
	vec2 v2f_texcoord0	= v2f_texcoord.xy*(1+0.15) - (vec2(1,-1)*0.5*0.15);
	//return texture( textureSampler0, v2f_texcoord.xy ) * sys_DiffuseColor;
	vec4 baseColor = texture( textureSampler0, v2f_texcoord0.xy );
	vec4 irisColor = texture( textureSampler1, v2f_texcoord0.xy ) * sys_DiffuseColor;
	vec4 pupilColor = texture( textureSampler2, v2f_texcoord0.xy ) * sys_DiffuseColor * 0.8;
	pupilColor.rgb += pupilColor.rgb*pupilColor.rgb*1.6; //Psuedo glowmap on the pupil
	
	vec4 diffuseColor = baseColor*(1-irisColor.a) + (irisColor*irisColor.a*(1-pupilColor.a)) + (pupilColor*pupilColor.a);

	float baseStr = min( 1, (baseColor.r+irisColor.r*irisColor.a)*4 );
	
	vec3 lightColor = vec3( 1,1,1 );
	vec3 vertDir = sys_WorldCameraPos-v2f_position.xyz;
	lightColor += (vec3(0.5,0.5,0.5)+sys_EmissiveColor) * cellShade((1-dot( v2f_normals.xyz, normalize( vertDir ) )) * irisColor.a * 1.4); // Add a little fresnel lighting
	lightColor += sys_EmissiveColor * irisColor.a;
	
	diffuseColor.a = 1;
	
	_temp_glowmapResult = vec4(0,0,0,1.0);
	_temp_glowmapResult.rgb = sys_EmissiveColor * irisColor.a; 
	
	return diffuseColor * vec4( lightColor, 1.0 );
}
