#version 330

// Inputs from vertex shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying float v2f_fogdensity;

// Samplers
uniform sampler2D textureSampler0;
//uniform sampler2D textureSampler1; // Glowmap
uniform sampler2D textureSampler1; // Colormap
uniform sampler2D textureSampler2; // Fluxxflow map

// System Inputs
uniform vec4 sys_DiffuseColor;
uniform vec3 sys_EmissiveColor;

// Lighting and Shadows
uniform vec4 sys_LightAmbient;
layout(std140) uniform sys_LightingInfo
{
	vec4 sys_LightColor[8];
	vec4 sys_LightPosition[8];
	vec4 sys_LightProperties[8];
	vec4 sys_LightShadowInfo[8];
	mat4 sys_LightMatrix[8];
};

// Fog
layout(std140) uniform sys_Fog
{
	vec4	sys_FogColor;
	float 	sys_FogEnd;
	float 	sys_FogScale;
};

// Game Inputs
uniform float	sys_AlphaCutoff;

uniform vec3 sys_WorldCameraPos;

uniform vec4 sys_SinTime, sys_CosTime, sys_Time;


float cellShade ( float lumin )
{
	const int levels = 3;
	float t = lumin*levels;
	float dif = 0.5 - mod(t+0.5,1);
	t += dif * min(1,(0.5-abs(dif))*32) * 0.5;
	
	lumin = t/levels;
	/*const float minval = 0.3;
	if ( lumin < -minval ) {
		lumin = -minval;
	}
	if ( lumin < minval ) {
		lumin = minval*((lumin+minval)/(minval*2));
	}*/
	return lumin;
}

float diffuseLighting ( vec3 normal, vec3 lightDist, float lightRange, float lightFalloff, float lightPass )
{
	// Distance-based attenuation
	float attenuation = pow( max( 1.0 - (length( lightDist )*lightRange), 0.0 ), lightFalloff );
	
	// Cosine law * attenuation
	float color = mix( max( dot( normal,normalize( lightDist ) ), 0.0 ), 1.0, lightPass*(1.0+attenuation) ) * attenuation;
	
	// Return final color
	return color;
}

vec2 random ( vec3 seed3 )
{
	float dot_product = dot(seed3, vec3(12.9898,78.233,45.164));
	dot_product = sin(dot_product);
    return vec2( fract(dot_product * 43758.5453), fract(dot_product * 35362.2151) );
}

float specularLighting( vec3 normal, vec3 lightdir, vec3 viewdir, float specular_pow, float lightRange )
{
	// Squeeze the lighting
	//normal.z *= 2.4;
	//normal.z *= 2.4;
	//lightdir.z *= 2.4;
	//normal.xy += (random( vec3(normal.x,normal.y,lightdir.z)*0.000005 )*0.1);
	vec3 	n_normal	= normalize(normal);
	float 	l_lightdist	= length( lightdir );
	vec3 	n_lightdir	= lightdir/l_lightdist;
	vec3 	n_viewdir	= normalize(viewdir);
	
	// Create the reflection vector
	vec3 reflectdir = 2*dot(normal,lightdir)*n_normal - n_lightdir;
	// Attenuation is based on camera position and reflection dir
	float attenuation = max( dot( normalize(reflectdir), n_viewdir ), 0.0 );
	
	// Move the highlight down to 60 degrees
	const float t_angleBias = 0.707;
	attenuation = (t_angleBias-abs(t_angleBias-attenuation))/t_angleBias;
	// Widen the highlight
	attenuation = min( 1, attenuation+0.07 );
	// If highlight is facing away from camera, remove it
	attenuation *= clamp( dot(n_normal,n_viewdir)+0.35, 0.0,1.0 );
	// If highlight is facing away from the light, remove it
	float normal_attenuation = dot( n_normal, n_lightdir );
	attenuation *= clamp( normal_attenuation+0.85, 0.0,1.0 );
	// If highlight is flush with the light, remove it
	attenuation *= clamp( 1.7-normal_attenuation, 0.0,1.0 );
	// If highlight is facing downwards, remove it
	attenuation *= clamp( 1.0+n_normal.z, 0.0, 1.0 );
	
	// Also add distance-based attenuation though
	float distance_attenuation = max( 1.0 - (l_lightdist*lightRange*0.3), 0.0 );
	
	// Perform exponent and distance attenuation
	attenuation = pow( attenuation*distance_attenuation, specular_pow*(2.0-distance_attenuation) );
	// Cell shade it
	attenuation = cellShade(attenuation*2);

	return attenuation;
}

void main ( void )  
{
	vec4 diffuseColor = texture( textureSampler0, v2f_texcoord0 );
	float f_alpha = diffuseColor.a * sys_DiffuseColor.a;
	float alphamap = texture( textureSampler2, v2f_texcoord0*1.3*4 + vec2(sys_CosTime.x,-sys_Time.y) ).r;
	alphamap -= texture( textureSampler2, v2f_texcoord0*2.2*2 + vec2(sys_SinTime.y,-sys_Time.y*0.3) ).r;
	alphamap *= 0.46;
	if ( v2f_colors.r < 0.5-alphamap ) {
		f_alpha = 0;
	}
	if ( f_alpha < sys_AlphaCutoff ) {
		discard;
	}
	
	vec3 colorMap = texture( textureSampler1, v2f_texcoord0 ).rgb;
	vec3 lightColor = sys_EmissiveColor;	// gl_SecondaryColor is set as emissive when useColors = false for materials
	//lightColor += texture2D( textureSampler1, v2f_texcoord0 ).rgb;
	
	vec3 vertDir;	// Calculate rim lighting
	vertDir = sys_WorldCameraPos-v2f_position.xyz;
	float lightVal3	= 1-dot( v2f_normals.xyz, normalize(vertDir) );
	float rimValue	= pow(lightVal3, 2)*0.5;
	lightVal3 = max( lightVal3, 0 );
	
	float intensity;
	vec3 lightDir;
	lightColor += sys_LightAmbient.rgb;
	for ( int i = 0; i < 8; i += 1 )
	{
		lightDir = sys_LightPosition[i].xyz - v2f_position.xyz;
		
		// Diffuse lighting
		float lightVal1 = diffuseLighting( v2f_normals.xyz, lightDir, sys_LightProperties[i].x, sys_LightProperties[i].y, sys_LightProperties[i].z );
		float lightVal2 = max( dot( sys_LightPosition[i].xyz, v2f_normals.xyz ), 0.0 );
		
		// Specular lighting
		float specLight1 = specularLighting( v2f_normals.xyz, lightDir, vertDir, 14, sys_LightProperties[i].x ) * 0.9;
		float specLight2 = specularLighting( v2f_normals.xyz, sys_LightPosition[i].xyz, vertDir, 14, 0 ) * 0.9; 
		
		// Light intensity value
		intensity = cellShade( mix( lightVal2, lightVal1, sys_LightPosition[i].w ) );
		intensity += intensity*rimValue;
		intensity += mix( specLight2, specLight1, sys_LightPosition[i].w );
		lightColor += sys_LightColor[i].rgb * intensity;
	}
	// Shadow "outline" effect
	diffuseColor.rgb *= 1-(clamp( (pow( clamp(lightVal3,0,1), 5 )-0.12)/0.1, 0,1 ) * (1-min(1,length(lightColor-sys_LightAmbient.rgb)*4)))*0.3;
	// Final light rim
	lightColor += sys_LightAmbient.rgb * (rimValue*0.5);
	
	gl_FragColor = mix( sys_FogColor, diffuseColor * mix( vec4(1,1,1,1), sys_DiffuseColor, colorMap.r ) * vec4( lightColor, 1.0 ), v2f_fogdensity );
	gl_FragColor.a = diffuseColor.a;
}