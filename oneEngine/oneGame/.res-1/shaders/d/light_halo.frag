// Inputs from vertex shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec3 v2f_emissive;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying float v2f_fogdensity;
varying float v2f_depth;
varying float v2f_fogbrighten;

// Samplers
uniform sampler2D textureDiffuse;

void main ( void )  
{
	//vec4 diffuseColor = texture2D( textureDiffuse, v2f_texcoord0 );
	vec4 diffuseColor = vec4(1,1,1,1);
	diffuseColor.a = max( 0.0, 0.5-length(vec2(0.5-v2f_texcoord0.x,0.5-v2f_texcoord0.y)) )*2.0;

	float fogDistance = 1.2-gl_Fog.end*gl_Fog.scale*0.7;
	
	//gl_FragColor.rgb = mix( gl_Fog.color.rgb, v2f_colors.rgb, v2f_fogdensity ) * diffuseColor.a;
	diffuseColor.a *= max( 0.0, min(1.0, v2f_depth/v2f_colors.a - fogDistance) * min(1.0,3.0-v2f_depth/v2f_colors.a) ) * min(1.0,max(0.0,(1.0-fogDistance)));
	diffuseColor.a = mix( 0.0, diffuseColor.a, min(1.0,v2f_fogdensity+(1.0-v2f_fogbrighten)) );
	diffuseColor.a = min( 1.0, diffuseColor.a );
	gl_FragColor.rgb = normalize(v2f_colors.rgb) * 1.5 * diffuseColor.a;
	//gl_FragColor.a = diffuseColor.a * v2f_colors.a;
	gl_FragColor.a = diffuseColor.a * 0.8;
}