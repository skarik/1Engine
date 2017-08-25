#version 150

// Inputs from vertex shader
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying vec2 v2f_texcoord_offs;

// Samplers
uniform sampler2D textureSampler0;
uniform sampler2D textureSampler1;

uniform vec3 gm_SunScreenPosition = vec3( 0.5,0.75,1.0 );
uniform float gm_ZoomColorStrength = 1.0;

void main ( void )  
{
	vec4 diffuseColor0 = texture2D( textureSampler0, v2f_texcoord0 );
	vec4 diffuseColor2 = texture2D( textureSampler1, v2f_texcoord0 );
	vec4 diffuseColor0_accum	= vec4( 0,0,0,1 );
	vec4 skyColor0_accum		= vec4( 0,0,0,1 );
	float currentRadiance = 1;
	const int sampleCount = 32;
	for ( int i = 0; i < sampleCount; i += 1 )	// ended up sampling here because the overlay geometry's blend mode keeps bugging out
	{
		vec2 t_samplePosition = clamp( v2f_texcoord0 + v2f_texcoord_offs*i, 0.01,0.99 );
		vec4 diffuseColor1 = texture2D( textureSampler0, t_samplePosition );
		vec4 skyColor1 = texture2D( textureSampler1, t_samplePosition );
		
		float luminance1 = diffuseColor1.r * 0.299 + diffuseColor1.g * 0.587 + diffuseColor1.b * 0.114;
		luminance1 = min( max( 0, luminance1-1.0 ), 0.125 )*0.5;
		diffuseColor0_accum += luminance1*diffuseColor1 * currentRadiance * sampleCount;
		
		//float luminance2 = skyColor1.r * 0.299 + skyColor1.g * 0.587 + skyColor1.b * 0.114;
		//luminance2 = max( 0, luminance2-1 );
		skyColor1 = max( skyColor1-0.5, 0 )*0.33;
		skyColor0_accum += skyColor1 * currentRadiance * sampleCount;
		
		currentRadiance *= 0.96;// + (0.02*luminance2);
	}
	float sunTangential = max( 0, (pow(abs(gm_SunScreenPosition.z-0.1),1.5)) * 1.5 * sign(gm_SunScreenPosition.z) );
	diffuseColor0_accum = diffuseColor0_accum * (32.0/sampleCount) * (0.0125) * min( 1, sunTangential*3.4 );
	skyColor0_accum = skyColor0_accum * (32.0/sampleCount) * (0.0125) * min( 1, sunTangential*3.4 );
	
	diffuseColor0 += (skyColor0_accum+diffuseColor0_accum)*gm_ZoomColorStrength;
	gl_FragColor = diffuseColor0 * mix( 1, (0.3/(32.0/sampleCount)), gm_ZoomColorStrength*min( 0.8,sunTangential*1.3 ) );
	//diffuseColor0 + (skyColor0_accum)*gm_ZoomColorStrength;//diffuseColor0 + (skyColor0_accum)*gm_ZoomColorStrength;
	//gl_FragColor = diffuseColor2;
	//gl_FragColor = skyColor0_accum;
	gl_FragColor.a = 1.0;
}