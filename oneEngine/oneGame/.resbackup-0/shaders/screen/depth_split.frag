// Inputs from vertex shader
varying vec4 v2f_normals;
//varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying float v2f_fogdensity;

// Depth vals
uniform float zNear;
uniform float zFar;
uniform float zCenter;

// Samplers
uniform sampler2D textureSampler0;
uniform sampler2D textureSampler1;
uniform sampler2D textureSampler2;

float LinearDepth( in float depth )
{
	return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
	//return depth;
}

// Sine wave appoximation method (benchmarked to about 3% speed increase over all shadows)
float SmoothCurve( float x ) {  
	return x * x *( 3.0 - 2.0 * x );  
}  
float TriangleWave( float x ) {  
	return abs( fract( x + 0.5 ) * 2.0 - 1.0 );  
}  
float SmoothTriangleWave( float x ) {  
	return (SmoothCurve( TriangleWave( x ) ))*2 - 1;  
} 
// Random
vec2 random ( vec3 seed3 )
{
	float dot_product = dot(seed3, vec3(12.9898,78.233,45.164));
	dot_product = SmoothTriangleWave(dot_product);
    return vec2( fract(dot_product * 43758.5453), fract(dot_product * 35362.2151) );
}

void main ( void )  
{
	vec2 offset = vec2( 0.0015, 0.0022 );
	
	vec4 splitStrength	= texture( textureSampler2, vec2(v2f_texcoord0.x,1-v2f_texcoord0.y) );
	v2f_texcoord0 += random( vec3( v2f_texcoord0.x, v2f_texcoord0.y, splitStrength.r ) ) * splitStrength.r * vec2(0.08,0.3);
	
	vec4 diffuseColor	= texture( textureSampler0, v2f_texcoord0 );
	vec4 depthColor		= texture( textureSampler1, v2f_texcoord0 );
	
	/*vec4 depthColor0 = texture2D( textureSampler1, v2f_texcoord0 + vec2(0,offset.y) );
	vec4 depthColor1 = texture2D( textureSampler1, v2f_texcoord0 - vec2(0,offset.y) );
	vec4 depthColor2 = texture2D( textureSampler1, v2f_texcoord0 + vec2(offset.x,0) );
	vec4 depthColor3 = texture2D( textureSampler1, v2f_texcoord0 - vec2(offset.x,0) );
	float avgDepth = LinearDepth(( depthColor0.z + depthColor1.z + depthColor2.z + depthColor3.z + depthColor.z ) * 0.2);*/
	float cenDepth = LinearDepth( depthColor.z );
	//float cenDepthCenter = zCenter / (zFar - zNear);
	float cenDepthCenter = zCenter / (zFar - zNear);
	//cenDepth = 3;//abs(cenDepthCenter-cenDepth);
	float cenOffset = 0.5 + abs(cenDepthCenter-cenDepth);
	float cenStrength = splitStrength.r*3 + abs(cenDepthCenter-cenDepth)*2 + 0.2;
	
	vec4 diffuseColor0 = texture2D( textureSampler0, v2f_texcoord0 + vec2(0,-offset.y)*cenOffset );
	vec4 diffuseColor1 = texture2D( textureSampler0, v2f_texcoord0 + vec2(offset.x,offset.y*0.5)*cenOffset );
	vec4 diffuseColor2 = texture2D( textureSampler0, v2f_texcoord0 + vec2(-offset.x,offset.y*0.5)*cenOffset );
	
	vec3 diffuseSplit = vec3( diffuseColor0.r, diffuseColor1.g, diffuseColor2.b );
	diffuseColor.rgb += (diffuseSplit-diffuseColor.rgb)*clamp( cenStrength*10, 0, 1 );
	diffuseColor.rgb *= 1 - splitStrength.r*0.15;
	vec3 midpoint = vec3(0.3,0.3,0.3);
	diffuseColor.rgb += ((diffuseColor.rgb*diffuseColor.rgb*0.9)-diffuseColor.rgb)*0.1;
	//diffuseColor.rgb = min( diffuseColor.rgb, ((diffuseColor.rgb-midpoint)*0.8)+midpoint );
	gl_FragColor.rgb = diffuseColor.rgb;
	
	//gl_FragColor.rgb = vec3(cenDepth,cenDepth,cenDepth);
	//gl_FragColor.rgb = splitStrength.rgb;
	
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
	//float darkness = clamp( ((cenDepth-avgDepth)*1000) - (0.35 + cenDepth*3.7), 0, 1 );
	//diffuseColor = mix( diffuseColor, diffuseColor*diffuseColor*1.1, darkness );
	/*if ( (cenDepth-avgDepth)*1000 > (0.45 + cenDepth*3.7) )
	{
		diffuseColor = diffuseColor*diffuseColor*1.1;//= vec3( 0,0,0 );
		//vec4 targetColor = diffuseColor*diffuseColor*1.1;
		//diffuseColor = mix( diffuseColor, targetColor, 1-cenDepth*2 );
	}*/
	
	//gl_FragColor.rgb = clamp( diffuseColor.rgb, vec3(-4,-4,-4), vec3(4,4,4) );
	
	//gl_FragColor.r = 1;
	//gl_FragColor.a = diffuseColor.a * v2f_colors.a;
	gl_FragColor.a = 0.98;//v2f_colors.a;
}