// Inputs from vertex shader
varying vec3 v2f_differentials;
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec3 v2f_texcoord0;

// Samplers
uniform sampler2D textureDiffuse;

void main ( void )  
{
	vec4 diffuseColor = texture2D( textureDiffuse, v2f_texcoord0.xy );
	
	//Don't draw out of the box range
	/*float dif_dist = max( abs(v2f_differentials.x), abs(v2f_differentials.y) );
	if ( dif_dist > 1.00 ) {
		discard;
	}
	else if ( dif_dist > 1 ) {
		diffuseColor.a = 0;
	}*/
	
	//float sampleWidth = v2f_texcoord0.z*0.00005;
	float sampleWidth = 0.002;
	
	vec4 sample0 = texture2D( textureDiffuse, v2f_texcoord0.xy + vec2( sampleWidth,sampleWidth ) );
	vec4 sample1 = texture2D( textureDiffuse, v2f_texcoord0.xy + vec2( sampleWidth,-sampleWidth ) );
	vec4 sample2 = texture2D( textureDiffuse, v2f_texcoord0.xy + vec2( -sampleWidth,-sampleWidth ) );
	vec4 sample3 = texture2D( textureDiffuse, v2f_texcoord0.xy + vec2( -sampleWidth,sampleWidth ) );
	/*vec4 sample4 = texture2D( textureDiffuse, v2f_texcoord0 + vec2( 0.1,0 ) );
	vec4 sample5 = texture2D( textureDiffuse, v2f_texcoord0 + vec2( 0,-0.1 ) );
	vec4 sample6 = texture2D( textureDiffuse, v2f_texcoord0 + vec2( 0,0.1 ) );
	vec4 sample7 = texture2D( textureDiffuse, v2f_texcoord0 + vec2( -0.1,0 ) );*/
	
	//diffuseColor.rgb *= 1-min(max(diffuseColor.a - sample0.a, 0),1);
	//diffuseColor.a = max( diffuseColor.a, (diffuseColor.a - sample0.a) );
	/*
	diffuseColor.rgb *= 1-min(max(diffuseColor.a - sample1.a, 0),1);
	diffuseColor.a = max( diffuseColor.a, diffuseColor.a - sample1.a );
	
	diffuseColor.rgb *= 1-min(max(diffuseColor.a - sample2.a, 0),1);
	diffuseColor.a = max( diffuseColor.a, diffuseColor.a - sample2.a );
	
	diffuseColor.rgb *= 1-min(max(diffuseColor.a - sample3.a, 0),1);
	diffuseColor.a = max( diffuseColor.a, diffuseColor.a - sample3.a );
	*/
	// outline should be sample0.a = 0 diffuse.a = 1
	//diffuseColor.rgb *= sample0.a * max( diffuseColor.a, diffuseColor.a-sample0.a );
	//diffuseColor.a = max( diffuseColor.a, diffuseColor.a-sample0.a );
	sample0.a = max( 0, sample0.a - diffuseColor.a );
	diffuseColor.a += sample0.a;
	diffuseColor.rgb *= 1-sample0.a;
	
	sample1.a = max( 0, sample1.a - diffuseColor.a );
	diffuseColor.a += sample1.a;
	diffuseColor.rgb *= 1-sample1.a;
	
	sample2.a = max( 0, sample2.a - diffuseColor.a );
	diffuseColor.a += sample2.a;
	diffuseColor.rgb *= 1-sample2.a;
	
	sample3.a = max( 0, sample3.a - diffuseColor.a );
	diffuseColor.a += sample3.a;
	diffuseColor.rgb *= 1-sample3.a;
	
	gl_FragColor = diffuseColor * v2f_colors;
	gl_FragColor.a = diffuseColor.a * v2f_colors.a;
}