// Inputs from vertex shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying vec2 v2f_texcoordi;
varying float v2f_fogdensity;
varying vec4 v2f_offset;

// Samplers
uniform sampler2D textureDiffuse;
uniform sampler2D textureSampler1; // Glowmap

// System Inputs
uniform vec4 sys_DiffuseColor;
uniform vec3 sys_EmissiveColor;

uniform vec4 sys_LightAmbient;
uniform vec4 sys_LightColor[8];
uniform vec4 sys_LightPosition[8];
uniform vec4 sys_LightProperties[8];

// Game Inputs
uniform float	gm_FadeValue;
uniform vec4 sys_SinTime;
uniform vec4 sys_Time;

float diffuseLighting ( vec3 normal, vec3 lightDist, float lightRange, float lightFalloff, float lightPass )
{
	// Distance-based attenuation
	float attenuation = pow( max( 1.0 - (length( lightDist )*lightRange), 0.0 ), lightFalloff );
	
	// Cosine law * attenuation
	float color = mix( max( dot( normal,normalize( lightDist ) ), 0.0 ), 1.0, lightPass*(1.0+attenuation) ) * attenuation;
	
	// Return final color
	return color;
}

void main ( void )  
{
	const float scale = 1/16.666666666666666666666666666667;

	//gm_FadeValue = (sys_SinTime.w+1)/2;
	//gm_FadeValue = 0.1;
	//float testval = 0.5;
	
	vec2 texCoord = v2f_texcoord0;
	/*texCoord.x += abs(v2f_normals.y)*v2f_offset.x*scale;
	texCoord.x -= abs(v2f_normals.x)*v2f_offset.y*scale;
	texCoord.y -= v2f_offset.z*scale;*/
	
	vec4 diffuseColor = texture2D( textureDiffuse, texCoord );
	/*vec2 offset = vec2(
		sin(texCoord.x+sys_SinTime.z+v2f_position.x+v2f_position.y-sys_Time.y),
		cos(texCoord.y+sys_SinTime.z+v2f_position.z+sys_Time.x+v2f_position.x+v2f_position.y) );
	diffuseColor = texture2D( textureDiffuse, texCoord+offset*diffuseColor.b*0.1 );*/
	vec3 lightColor = sys_EmissiveColor;	// gl_SecondaryColor is set as emissive when useColors = false for materials
	//lightColor += texture2D( textureSampler1, v2f_texcoord0 ).rgb;
	
	diffuseColor.a = diffuseColor.r;
	//diffuseColor.a -= (1-(gm_FadeValue+diffuseColor.g*diffuseColor.a));
	if ( gm_FadeValue < (1-diffuseColor.g*diffuseColor.a) ) {
		diffuseColor.a = 0;
	}
	diffuseColor = max( vec4(0,0,0,0), diffuseColor );
	
	lightColor += sys_LightAmbient.rgb*1.6;
	for ( int i = 0; i < 8; i += 1 )
	{
		vec3 lightDir;
		lightDir = sys_LightPosition[i].xyz - v2f_position.xyz;
		
		float lightVal1 = diffuseLighting( v2f_normals.xyz, lightDir, sys_LightProperties[i].x*0.6, sys_LightProperties[i].y, sys_LightProperties[i].z );
		float lightVal2 = max( dot( sys_LightPosition[i].xyz, v2f_normals.xyz ), 0.0 );
		
		lightColor += sys_LightColor[i].rgb * mix( lightVal2, lightVal1, sys_LightPosition[i].w );
	}
	
	float targetAlpha = sys_DiffuseColor.a;
	targetAlpha *= 3*(0.5-abs(0.5-v2f_texcoordi.x));
	targetAlpha *= 1.2*(0.5-abs(0.5+v2f_texcoordi.y));
	targetAlpha = max( 0.0, min( 1.0, targetAlpha ) );
	
	gl_FragColor = mix( gl_Fog.color, sys_DiffuseColor * vec4( lightColor, 1.0 ), v2f_fogdensity );
	gl_FragColor.a = diffuseColor.a * targetAlpha;
}