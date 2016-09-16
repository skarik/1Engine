#version 330

// Inputs from vertex shader
in vec4 v2f_normals;
//varying vec4 v2f_colors;
in vec4 v2f_position;
in vec2 v2f_texcoord0;
in float v2f_fogdensity;

// Depth vals
uniform float zNear;
uniform float zFar;

// Samplers
uniform sampler2D textureSampler0;
uniform sampler2D textureSampler1;

float LinearDepth( in float depth )
{
	return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
	//return depth;
}

// Fog
layout(std140) uniform sys_Fog
{
	vec4	sys_FogColor;
	vec4	sys_AtmoColor;
	float 	sys_FogEnd;
	float 	sys_FogScale;
};

void main ( void )
{
	vec2 offset = vec2( 0.0015, 0.0022 );
	vec4 diffuseColor = texture( textureSampler0, v2f_texcoord0 );
	vec4 depthColor = texture( textureSampler1, v2f_texcoord0 );

	vec4 depthColor0 = texture( textureSampler1, v2f_texcoord0 + vec2(0,offset.y) );
	vec4 depthColor1 = texture( textureSampler1, v2f_texcoord0 - vec2(0,offset.y) );
	vec4 depthColor2 = texture( textureSampler1, v2f_texcoord0 + vec2(offset.x,0) );
	vec4 depthColor3 = texture( textureSampler1, v2f_texcoord0 - vec2(offset.x,0) );
	//float avgDepth = LinearDepth(( depthColor0.x + depthColor1.x + depthColor2.x + depthColor3.x + depthColor.x ) * 0.2);
	float minDepth = LinearDepth( min( min( min( min( depthColor0.x, depthColor1.x ), depthColor2.x ), depthColor3.x), depthColor.x ) );
	float cenDepth = LinearDepth(depthColor.x);

	//float luminance = diffuseColor.r * 0.299 + diffuseColor.g * 0.587 + diffuseColor.b * 0.114;
	//vec4 sqrColor = diffuseColor*diffuseColor;
	//float luminance2 = sqrColor.r * 0.299 + sqrColor.g * 0.587 + sqrColor.b * 0.114;

	//diffuseColor = sqrColor * (luminance/luminance2);

	// Dark summer
	/*diffuseColor.r *= 0.85;
	diffuseColor.g *= 0.85;
	diffuseColor.b *= 1.16;*/

	// Yellow Winter
	/*diffuseColor.r *= 1.24;
	diffuseColor.g *= 1.18;
	diffuseColor.b *= 0.63;*/

	/*diffuseColor.r = depthColor.z;
	diffuseColor.g = diffuseColor.r;
	diffuseColor.b = diffuseColor.r;*/

	//if ( abs(avgDepth - depthColor.z) > 0.01 )
	//if ( (cenDepth-avgDepth)*1000 > 0.5 )

	vec4 outlineColor = diffuseColor*diffuseColor*1.1;
	outlineColor = mix( outlineColor, sys_AtmoColor, clamp( minDepth * 2, 0, 1 ) );

	float depthDifference = (cenDepth-minDepth)*1000;
	depthDifference -= 0.35;
	depthDifference -= min( 500, minDepth * 100 );

	//float darkness = clamp( depthDifference - (0.35 + cenDepth*3.7), 0, 1 );
	float darkness = clamp( depthDifference, 0, 1 );
	diffuseColor = mix( diffuseColor, outlineColor, darkness );
	/*if ( (cenDepth-avgDepth)*1000 > (0.45 + cenDepth*3.7) )
	{
		diffuseColor = diffuseColor*diffuseColor*1.1;//= vec3( 0,0,0 );
		//vec4 targetColor = diffuseColor*diffuseColor*1.1;
		//diffuseColor = mix( diffuseColor, targetColor, 1-cenDepth*2 );
	}*/

	gl_FragColor.rgb = clamp( diffuseColor.rgb, vec3(-4,-4,-4), vec3(4,4,4) );

	//gl_FragColor.rgb = vec3(1,1,1) * (cenDepth*1000);
	//gl_FragColor.rgb = depthColor.zzz;
	//gl_FragColor.r = 1;
	//gl_FragColor.a = diffuseColor.a * v2f_colors.a;
	gl_FragColor.a = 0.98;//v2f_colors.a;
}
