#version 330

// Inputs from vertex shader
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;

// Samplers
uniform sampler2D textureSampler0;

// External inputs to filter
uniform vec4 blendMidtone		= vec4( 1,1,1,-0.05 );
uniform vec4 blendHighlights	= vec4( 1,1,1,0.05 );
uniform vec4 blendShadows		= vec4( 0.98,0.98,1.02,0.08 );

void main ( void )  
{
	float offset = 0.0015;
	vec4 diffuseColor = texture2D( textureSampler0, v2f_texcoord0 );
	
	// Calculate luminance and squared luminances
	float luminance = diffuseColor.r * 0.299 + diffuseColor.g * 0.587 + diffuseColor.b * 0.114;
	vec4 sqrColor = diffuseColor*diffuseColor;
	float sqrLuminance = sqrColor.r * 0.299 + sqrColor.g * 0.587 + sqrColor.b * 0.114;
	
	//diffuseColor = sqrColor * (luminance/sqrLuminance); // Saturation filter
	
	// Dark summer
	/*diffuseColor.r *= 0.85;
	diffuseColor.g *= 0.85;
	diffuseColor.b *= 1.16;*/
	
	// Yellow Winter
	/*diffuseColor.r *= 1.24;
	diffuseColor.g *= 1.18;
	diffuseColor.b *= 0.63;*/
	
	float blendValue;
	float usedLumin = (luminance*0.9+0.11);
	
	// Edit midtones
	//vec4 midtoneEdit = vec4( 1.0,1.0,1.0,0.1 );
	vec4 midtoneEdit = blendMidtone;// = vec4( 1,1,1,-0.05 );
	blendValue = sqrt( max(0,min(1,1-abs(usedLumin*2-1))) );
	diffuseColor.rgb = mix( diffuseColor.rgb, vec3(luminance,luminance,luminance), blendValue*midtoneEdit.a );
	diffuseColor.rgb *= mix( vec3(1,1,1), midtoneEdit.rgb, blendValue );
	
	// Edit highlights
	//vec4 highlightEdit = vec4( 1.2,1.1,1.0,-0.2 );	// RGB+Desaturation
	vec4 highlightEdit = blendHighlights;//= vec4( 1,1,1,0.05 );
	blendValue = sqrt( min(1,max(0,usedLumin*2-1)) );
	diffuseColor.rgb = mix( diffuseColor.rgb, vec3(luminance,luminance,luminance), blendValue*highlightEdit.a );
	diffuseColor.rgb *= mix( vec3(1,1,1), highlightEdit.rgb, blendValue );
	
	// Edit darks
	//vec4 shadowEdit = vec4( 1.1,1.0,1.1,-0.3 );
	vec4 shadowEdit = blendShadows;//= vec4( 0.98,0.98,1.02,0.08 );
	blendValue = sqrt( min(1,max(0,1-usedLumin*2)) );
	diffuseColor.rgb = mix( diffuseColor.rgb, vec3(luminance,luminance,luminance), blendValue*shadowEdit.a );
	diffuseColor.rgb *= mix( vec3(1,1,1), shadowEdit.rgb, blendValue );
	
	// Set output color
	gl_FragColor = diffuseColor;
	gl_FragColor.a = 1;//v2f_colors.a;
}