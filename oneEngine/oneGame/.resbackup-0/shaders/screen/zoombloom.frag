// Inputs from vertex shader
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying vec2 v2f_texcoord_offs;

// Samplers
uniform sampler2D textureSampler0;

uniform vec3 gm_SunScreenPosition = vec3( 0.5,0.75,1.0 );
uniform float gm_ZoomColorStrength = 1.0;

void main ( void )  
{
	vec4 diffuseColor0 = texture2D( textureSampler0, v2f_texcoord0 );
	vec4 diffuseColor0_accum = vec4( 0,0,0,1 );
	float currentRadiance = 1;
	const int sampleCount = 32;
	for ( int i = 0; i < sampleCount; i += 1 )	// ended up sampling here because the overlay geometry's blend mode keeps bugging out
	{
		vec4 diffuseColor1 = texture2D( textureSampler0, clamp( v2f_texcoord0 + v2f_texcoord_offs*i, 0.01,0.99 ) );
		
		float luminance1 = diffuseColor1.r * 0.299 + diffuseColor1.g * 0.587 + diffuseColor1.b * 0.114;
		luminance1 = min( max( -0.035, luminance1-1.0 ), 0.125 );
		diffuseColor0_accum += luminance1*diffuseColor1 * currentRadiance * 32;
		currentRadiance *= 0.96;
	}
	diffuseColor0_accum = diffuseColor0_accum * (sampleCount/32.0) * (0.0125) * min( 1, max( 0, gm_SunScreenPosition.z*3.4 ) );
	
	gl_FragColor = diffuseColor0 + diffuseColor0_accum*gm_ZoomColorStrength;
	gl_FragColor.a = 1.0;
}