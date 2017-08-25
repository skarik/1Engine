varying vec3 v2f_normals;
varying vec3 v2f_directional;
varying vec2 v2f_texcoord0;
varying vec3 v2f_position;

uniform sampler2D textureSampler0; // Diffusemap
uniform sampler2D textureSampler1; // Dustmap

// Main blend color
uniform vec4 sys_DiffuseColor;
// Time used for...twinkling.
uniform vec4 sys_Time;
// Horizon strength
uniform float gm_HorizonStrength;

vec4 mulTex ( sampler2D n_texture, vec2 n_texscale )
{
	vec4 colorX = texture2D( n_texture, v2f_position.yz*n_texscale );
	vec4 colorY = texture2D( n_texture, (v2f_position.xz+vec2(0,0.5))*n_texscale );
	vec4 colorZ = texture2D( n_texture, (v2f_position.xy+vec2(0.5,0))*n_texscale );
	vec3 blend_weights = abs( v2f_directional );   // Tighten up the blending zone:  
	//blend_weights = (blend_weights - 0.1) * 7;  
	//blend_weights = blend_weights * 7;
	blend_weights = pow(blend_weights,vec3(6,6,6));
	blend_weights = max(blend_weights,0.0);      // Force weights to sum to 1.0 (very important!)  
	blend_weights /= ( blend_weights.x + blend_weights.y + blend_weights.z );
	// now, get color
	vec4 color =	colorX.xyzw * blend_weights.xxxx +  
					colorY.xyzw * blend_weights.yyyy +  
					colorZ.xyzw * blend_weights.zzzz;  
	return color;
}

void main ( void )  
{
	vec3 finalColor;
	vec4 color1 = texture2D( textureSampler1, v2f_texcoord0.xy + vec2(sys_Time.x*0.07,-sys_Time.x*0.07) ).rgba;
	color1 = normalize( color1 + vec4(0.3,0.3,0.7,0) );
	
	finalColor = mulTex( textureSampler0, vec2(0.5,1.0)*0.7 ).rgb;
	finalColor = finalColor*(finalColor+0.1)*(color1.rgb+0.7)*1.6;

	float fHorizonFactor = abs(v2f_normals.z)*(gm_HorizonStrength) + (1.0-gm_HorizonStrength);
	finalColor *= pow( abs(fHorizonFactor), 0.4 )*sys_DiffuseColor.rgb;
	
	finalColor += max(0.0, abs(v2f_directional.x)*(1.0-fHorizonFactor)*(1.0-fHorizonFactor) )*vec3(0.2,0.2,0.1);
	
	gl_FragColor = vec4( finalColor, 1.0 );
}