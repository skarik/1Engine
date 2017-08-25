// Inputs from vertex shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
//varying vec2 v2f_texcoord1;
varying float v2f_fogdensity;

// Depth vals
uniform float lightThreshold	= 0.8;
uniform float lightGradient		= 1.4;

// Samplers
uniform sampler2D textureSampler0;//textureDiffuse;
uniform sampler2D textureSampler1;//textureHalfDiffuse;
uniform sampler2D textureSampler2;//texture4thDiffuse;
uniform sampler2D textureSampler3;//texture16thDiffuse;

//uniform vec4 sys_Time; // Supplied manually by screen shader instance (is normally not available in screen shaders)
//uniform sampler2D textureDepth;
/*
float LinearDepth( in float depth )
{
	return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
	//return depth;
}*/

/*vec2 random ( vec3 seed3 )
{
	float dot_product = dot(seed3, vec3(12.9898,78.233,45.164));
	dot_product = sin(dot_product);
    return vec2( fract(dot_product * 43758.5453), fract(dot_product * 35362.2151) );
}*/

void main ( void )  
{
	float offset = 0.0015;
	vec4 diffuseColor = texture2D( textureSampler0, v2f_texcoord0 );
	//vec4 diffuseColor2 = texture2D( textureHalfDiffuse, v2f_texcoord0/*+random(vec3(v2f_texcoord0.x,v2f_texcoord0.y,sys_Time.x))*0.001*/ );
	//vec4 diffuseColor3 = texture2D( texture4thDiffuse, v2f_texcoord0/*+random(vec3(v2f_texcoord0.y,sys_Time.y,v2f_texcoord0.x))*0.002*/ );
	//vec4 diffuseColor4 = texture2D( texture16thDiffuse, v2f_texcoord0/*+random(vec3(sys_Time.w,v2f_texcoord0.x,v2f_texcoord0.y))*0.004*/ );
	
	//diffuseColor3 = (diffuseColor2+diffuseColor3)*0.5;	// Cheapest goddamn blur ever
	//diffuseColor4 = (diffuseColor3+diffuseColor4)*0.5;			// except unfortunately I'm a better person than that
	
	vec4 blurColor = vec4(0,0,0,0);
	float boffsy = 0.01;
	
	vec2 tex_coord = v2f_texcoord0;//*4;
	for ( int i = 1; i <= 2; i += 1 ) {
	blurColor += texture2D( textureSampler2, tex_coord + vec2( 1,0 )*boffsy*i );
	blurColor += texture2D( textureSampler2, tex_coord + vec2( 0.7,0.7 )*boffsy*i );
	blurColor += texture2D( textureSampler2, tex_coord + vec2( 0,1 )*boffsy*i );
	blurColor += texture2D( textureSampler2, tex_coord + vec2( -0.7,0.7 )*boffsy*i );
	blurColor += texture2D( textureSampler2, tex_coord + vec2( -1,0 )*boffsy*i );
	blurColor += texture2D( textureSampler2, tex_coord + vec2( -0.7,-0.7 )*boffsy*i );
	blurColor += texture2D( textureSampler2, tex_coord + vec2( 0,-1 )*boffsy*i );
	blurColor += texture2D( textureSampler2, tex_coord + vec2( 0.7,-0.7 )*boffsy*i );
	}
	blurColor /= 16.0;
	
	vec4 diffuseColor4 = blurColor;
	
	//diffuseColor = (diffuseColor+diffuseColor2)*0.5;
	//diffuseColor = (diffuseColor+diffuseColor3)*0.5;
	//diffuseColor = (diffuseColor+diffuseColor4)*0.5;
	//diffuseColor = diffuseColor2;
	/*vec4 depthColor = texture2D( textureDepth, v2f_texcoord0 );
	
	vec4 depthColor0 = texture2D( textureDepth, v2f_texcoord0 + vec2(0,offset) );
	vec4 depthColor1 = texture2D( textureDepth, v2f_texcoord0 - vec2(0,offset) );
	vec4 depthColor2 = texture2D( textureDepth, v2f_texcoord0 + vec2(offset,0) );
	vec4 depthColor3 = texture2D( textureDepth, v2f_texcoord0 - vec2(offset,0) );
	float avgDepth = LinearDepth(( depthColor0.z + depthColor1.z + depthColor2.z + depthColor3.z + depthColor.z ) * 0.2);
	float cenDepth = LinearDepth(depthColor.z);
	*/
	//gl_FragColor = diffuseColor * v2f_colors;
	//gl_FragColor.r = 1;
	//gl_FragColor.b = 0;'
	//gl_FragColor.r = diffuseColor.a;
	float luminance = diffuseColor.r * 0.299 + diffuseColor.g * 0.587 + diffuseColor.b * 0.114;
	//vec4 sqrColor = diffuseColor*diffuseColor;
	//float luminance2 = sqrColor.r * 0.299 + sqrColor.g * 0.587 + sqrColor.b * 0.114;
	float luminance_4 = diffuseColor4.r * 0.299 + diffuseColor4.g * 0.587 + diffuseColor4.b * 0.114;
	//luminance_4 = min( max( 0, (luminance_4 - 0.4)*3 ), 1 ); // Dark elf bloom
	luminance_4 = min( max( 0, (luminance_4 - lightThreshold)*lightGradient ), 1 ); // Normal asshole bloom
	
	diffuseColor += luminance_4 * diffuseColor4;
	
	//diffuseColor = sqrColor * (luminance/luminance2);
	
	// Dark summer
	/*diffuseColor.r *= 0.85;
	diffuseColor.g *= 0.85;
	diffuseColor.b *= 1.16;*/
	
	// Yellow Winter
	/*diffuseColor.r *= 1.24;
	diffuseColor.g *= 1.18;
	diffuseColor.b *= 0.63;*/
	
	// Blue winter
	/*diffuseColor.r *= 1.01;
	diffuseColor.g *= 0.99;
	diffuseColor.b *= 1.28;*/
	
	/*diffuseColor.r = depthColor.z;
	diffuseColor.g = diffuseColor.r;
	diffuseColor.b = diffuseColor.r;*/
	
	gl_FragColor = diffuseColor;
	//gl_FragColor.a = diffuseColor.a * v2f_colors.a;
	gl_FragColor.a = v2f_colors.a;
}