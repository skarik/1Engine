#version 330

// Inputs from vertex shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
//varying vec2 v2f_texcoord1;
varying float v2f_fogdensity;

// Blur properties
uniform float blurAmount		= 0.0;
// Bloom properties
uniform float lightThreshold	= 0.84;
uniform float lightGradient		= 1.40;
// Red desaturaion properties
uniform float redDesaturate		= 0.0;

// Samplers
uniform sampler2D textureSampler0;
//uniform sampler2D textureHalfDiffuse;
uniform sampler2D textureSampler2;
//uniform sampler2D texture16thDiffuse;

void main ( void )  
{
	//float offset = 0.0015;
	vec4 diffuseColor = texture2D( textureSampler0, v2f_texcoord0 );
	
	// Sample for blur
	vec4 blurColor = vec4(0,0,0,0);
	float boffsy = 0.01;
	
	vec2 tex_coord = v2f_texcoord0;//*4;
	// 16-tap blur
	for ( int i = 1; i <= 2; i += 1 ) {
		blurColor += texture( textureSampler2, tex_coord + vec2( 1,0 )*boffsy*i );
		blurColor += texture( textureSampler2, tex_coord + vec2( 0.7,0.7 )*boffsy*i );
		blurColor += texture( textureSampler2, tex_coord + vec2( 0,1 )*boffsy*i );
		blurColor += texture( textureSampler2, tex_coord + vec2( -0.7,0.7 )*boffsy*i );
		blurColor += texture( textureSampler2, tex_coord + vec2( -1,0 )*boffsy*i );
		blurColor += texture( textureSampler2, tex_coord + vec2( -0.7,-0.7 )*boffsy*i );
		blurColor += texture( textureSampler2, tex_coord + vec2( 0,-1 )*boffsy*i );
		blurColor += texture( textureSampler2, tex_coord + vec2( 0.7,-0.7 )*boffsy*i );
	}
	blurColor /= 16.0;
	
	vec4 diffuseColor4 = blurColor;
	
	// Mix with blur factor
	diffuseColor = mix( diffuseColor, diffuseColor4, blurAmount );
	
	// Perform bloom
	float luminance = diffuseColor.r * 0.299 + diffuseColor.g * 0.587 + diffuseColor.b * 0.114;
	diffuseColor.rgb = mix( diffuseColor.rgb, vec3( luminance*(luminance+0.1)*2, luminance, luminance ), clamp( redDesaturate,0,1 ) );
	diffuseColor.rgb = mix( diffuseColor.rgb, (diffuseColor.rgb-0.1)*(vec3(luminance+0.5,luminance,luminance)+0.6), clamp( redDesaturate-1, 0,1 ) );
	
	float luminance_4 = diffuseColor4.r * 0.299 + diffuseColor4.g * 0.587 + diffuseColor4.b * 0.114;
	luminance_4 = min( max( 0, (luminance_4 - (lightThreshold))*lightGradient ), 1 ); // Normal asshole bloom
	
	diffuseColor += luminance_4 * diffuseColor4;
	
	gl_FragColor = diffuseColor;
	gl_FragColor.a = 1;
}