// Inputs from vertex shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying float v2f_fogdensity;

// Depth vals
uniform float zNear;
uniform float zFar;

// Samplers
uniform sampler2D textureDiffuse;
uniform sampler2D textureDepth;

float LinearDepth( in float depth )
{
	return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
	//return depth;
}

void main ( void )  
{
	float offset = 0.0015;
	vec4 diffuseColor = texture2D( textureDiffuse, v2f_texcoord0 );
	vec4 depthColor = texture2D( textureDepth, v2f_texcoord0 );
	
	vec4 depthColor0 = texture2D( textureDepth, v2f_texcoord0 + vec2(0,offset) );
	vec4 depthColor1 = texture2D( textureDepth, v2f_texcoord0 - vec2(0,offset) );
	vec4 depthColor2 = texture2D( textureDepth, v2f_texcoord0 + vec2(offset,0) );
	vec4 depthColor3 = texture2D( textureDepth, v2f_texcoord0 - vec2(offset,0) );
	float avgDepth = LinearDepth(( depthColor0.z + depthColor1.z + depthColor2.z + depthColor3.z + depthColor.z ) * 0.2);
	float cenDepth = LinearDepth(depthColor.z);
	
	//gl_FragColor = diffuseColor * v2f_colors;
	//gl_FragColor.r = 1;
	//gl_FragColor.b = 0;'
	//gl_FragColor.r = diffuseColor.a;
	float luminance = diffuseColor.r * 0.299 + diffuseColor.g * 0.587 + diffuseColor.b * 0.114;
	vec4 sqrColor = diffuseColor*diffuseColor;
	float luminance2 = sqrColor.r * 0.299 + sqrColor.g * 0.587 + sqrColor.b * 0.114;
	
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
	if ( (cenDepth-avgDepth)*1000 > (0.45 + cenDepth*3.7) )
	{
		diffuseColor = diffuseColor*diffuseColor*1.1;//= vec3( 0,0,0 );
		//vec4 targetColor = diffuseColor*diffuseColor*1.1;
		//diffuseColor = mix( diffuseColor, targetColor, 1-cenDepth*2 );
	}
	
	/*if ( (cenDepth-avgDepth)*1000 > 0.01 )
	{
		diffuseColor = diffuseColor*diffuseColor*1.1;//= vec3( 0,0,0 );
	}*/
	
	gl_FragColor = diffuseColor;
	//gl_FragColor.a = diffuseColor.a * v2f_colors.a;
	gl_FragColor.a = v2f_colors.a;
}