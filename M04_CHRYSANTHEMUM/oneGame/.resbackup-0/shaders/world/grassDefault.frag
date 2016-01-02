#version 140

varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying float v2f_fogdensity;
varying vec3 v2f_screenpos;

varying float v2f_camDist;

uniform sampler2D textureSampler0;
uniform sampler2D textureShadow[8];

uniform float sys_AlphaCutoff;

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

varying vec4 v2f_lightcoord[8];

// Fog
layout(std140) uniform sys_Fog
{
	vec4	sys_FogColor;
	float 	sys_FogEnd;
	float 	sys_FogScale;
};

float random ( vec3 seed3 )
{
	float dot_product = dot(seed3, vec3(12.9898,78.233,45.164));
    return fract(sin(dot_product) * 43758.5453);
}

//http://gamedev.stackexchange.com/questions/59797/glsl-shader-change-hue-saturation-brightness
vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}
vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

// 0 for normal lighting, 1 for full wraparound lighting
const float mixthrough = 0.0;
/*float diffuseLighting ( vec3 normal, vec3 lightDist, float lightRange, float lightFalloff, float lightPass )
{
	// Distance-based attenuation
	float attenuation = pow( max( 1.0 - (length( lightDist )*lightRange), 0.0 ), lightFalloff );
	
	// Cosine law * attenuation
	//float color = mix( dot( normal,normalize( lightDist ) ), 1.0, lightPass*(1.0+attenuation) ) * attenuation;
	// Cosine law breaking ( cuz objects need more detail )
	float normalAttenuate = dot( normal,normalize( lightDist ) );
	normalAttenuate = (max( normalAttenuate, -mixthrough ) + mixthrough)/(1+mixthrough);
	// Attenuation
	float color = mix( normalAttenuate, 1.0, lightPass*(1.0+attenuation) ) * attenuation;
	
	// Return final color
	return color;
}*/
float diffuseLighting ( vec3 normal, vec3 lightDist, float lightRange, float lightFalloff, float lightPass )
{
	// Distance-based attenuation
	float attenuation = pow( max( 1.0 - (length( lightDist )*lightRange), 0.0 ), lightFalloff );
	
	// Cosine law * attenuation
	float color = mix( max( dot( normal,normalize( lightDist ) ), 0.0 ), 1.0, lightPass*(1.0+attenuation) ) * attenuation;
	
	// Return final color
	return color;
}
vec3 defaultLighting ( vec3 normal, vec4 lightPosition, vec4 lightProperties, vec4 lightColor, vec3 vertDir )
{
	vec3 resultColor;
	
	// Get direction to light
	vec3 lightDir;
	lightDir = lightPosition.xyz - v2f_position.xyz;
	
	// Perform base lighting
	float lightVal1 = diffuseLighting( normal.xyz, lightDir, lightProperties.x, lightProperties.y, (lightProperties.z+0.7)/2 );
	float lightVal2 = dot( lightPosition.xyz, normal.xyz );
	lightVal2 = (max( lightVal2, -mixthrough ) + mixthrough)/(1+mixthrough); // Directional light mixthrough
	
	// Mix between light styles for the directional/nondirectional
	float lightValf = mix( lightVal2, lightVal1, lightPosition.w );
	//lightValf = cellShade(lightValf); // Apply cellshading
		
	resultColor = lightColor.rgb * max( 0, lightValf );
	
	return resultColor;
}

void main ( void )  
{
	/*gl_FragColor = vec4(1,1,1,1);
	return;*/

	vec4 diffuseColor = texture2D( textureSampler0, v2f_texcoord0 );
	vec3 apparentNormal = vec3( v2f_normals.x, v2f_normals.y, v2f_normals.z*v2f_colors.a + 0.2 );
	
	float f_alpha = clamp( clamp( (1.0-v2f_camDist/96.0)*4.0, 0.0,1.0 ) * diffuseColor.a - (70.0/(v2f_camDist*v2f_camDist*v2f_camDist*v2f_camDist)), 0.0,1.0 );// * 0.07 * 0.5;
	if ( f_alpha < sys_AlphaCutoff ) {
		discard;
	}
	
	// Rim effect
	vec3 vertDir;
	vertDir = vec3( 0,0,0 );//sys_WorldCameraPos-v2f_position.xyz;
	
	vec3 lightColor = sys_LightAmbient.rgb * v2f_colors.rgb;
	lightColor += defaultLighting ( apparentNormal, sys_LightPosition[0], sys_LightProperties[0], sys_LightColor[0], vertDir );
	lightColor += defaultLighting ( apparentNormal, sys_LightPosition[1], sys_LightProperties[1], sys_LightColor[1], vertDir );
	lightColor += defaultLighting ( apparentNormal, sys_LightPosition[2], sys_LightProperties[2], sys_LightColor[2], vertDir );
	lightColor += defaultLighting ( apparentNormal, sys_LightPosition[3], sys_LightProperties[3], sys_LightColor[3], vertDir );
	
	/*for ( int i = 0; i < 8; i += 1 )
	{
		vec3 lightDir;
		lightDir = sys_LightPosition[i].xyz - v2f_position.xyz;
		
		float lightVal1 = diffuseLighting( v2f_normals.xyz, lightDir, sys_LightProperties[i].x, sys_LightProperties[i].y, (sys_LightProperties[i].z+0.7)/2 );
		float lightVal2 = max( dot( sys_LightPosition[i].xyz, vec3( v2f_normals.x,v2f_normals.y,v2f_normals.z*v2f_colors.a + 0.2) ), 0.0 );
	*/	
		// Shadows on light 1
		/*if ( i == 1 )
		{
			vec4 shadowWcoord = vec4(
				v2f_lightcoord[1].x / v2f_lightcoord[1].w,
				v2f_lightcoord[1].y / v2f_lightcoord[1].w,
				v2f_lightcoord[1].z / v2f_lightcoord[1].w,
				v2f_lightcoord[1].w );
			
			if ( sys_LightShadowInfo[1].x > 0.5 )
			{
				if (( shadowWcoord.z > 0.0 )&&( shadowWcoord.x > 0.0 )&&( shadowWcoord.x < 1.0 )&&( shadowWcoord.y > 0.0 )&&( shadowWcoord.y < 1.0 ))
				{
					float difVal = 0.0;
					float distanceFromLight;
					vec2 coord;
					
					for ( int i = 0; i < 4; i += 1 )
					{
						coord = shadowWcoord.st;
						coord += random( vec3(v2f_screenpos.xy,i) )*0.0015;
						distanceFromLight = texture2D( textureShadow[1], coord ).z;
						difVal += clamp((shadowWcoord.z - distanceFromLight)*512.0, 0.0,1.0);
					}
					difVal /= 4.0;
					
					// Decrease dif val if close to edge
					difVal *= clamp( 4.0-abs(shadowWcoord.x-0.5)*8.0, 0.0,1.0 );
					difVal *= clamp( 4.0-abs(shadowWcoord.y-0.5)*8.0, 0.0,1.0 );
					
					// Do the color mix
					//diffuseColor = mix( diffuseColor, diffuseColor*0.4, difVal );
					shadowDist = 1.0-difVal;
				}
				else
				{
					shadowDist = 1.0;
				}
			}
		}
		else
		{
			shadowDist = 1.0;
		}*/
	/*	shadowDist = 1;
		
		lightColor += sys_LightColor[i].rgb * mix( lightVal2, lightVal1, sys_LightPosition[i].w ) * shadowDist;
	}*/
	
	// ====MOVE THIS ART-SIDE====
	/*diffuseColor.r -= 0.09;
	diffuseColor.g += 0.08;
	diffuseColor.rgb *= 0.85;*/
	// ====MOVE THIS ART-SIDE====
	
	// Shift the hue with the hue shift value
	/*vec3 diffuseHSV = rgb2hsv( diffuseColor.rgb );
	diffuseHSV.r = 0.13-diffuseHSV.r;
	diffuseHSV.g -= 0.1;
	diffuseColor.rgb = hsv2rgb( diffuseHSV );*/
	
	
	gl_FragColor = mix( sys_FogColor, diffuseColor * vec4( lightColor, 1.0 ), v2f_fogdensity );
	//gl_FragColor.rgb = mix( gl_FragColor.rgb, (v2f_colors.a+1)/2, 0.99 );
	//gl_FragColor = diffuseColor;
	//gl_FragColor.a = diffuseColor.a * v2f_colors.a;
	//gl_FragColor.a = diffuseColor.a;
	//gl_FragColor.a = clamp( (1-v2f_screenpos.z*0.02)*4, 0,1 ) * diffuseColor.a;
	//gl_FragColor.a = clamp( clamp( (1.0-v2f_camDist/96.0)*4.0, 0.0,1.0 ) * diffuseColor.a - (70.0/(v2f_camDist*v2f_camDist*v2f_camDist*v2f_camDist)), 0.0,1.0 );// * 0.07 * 0.5;
	gl_FragColor.a = f_alpha;
	//gl_FragColor.a += (1-gl_FragColor.a)*0.99;
}
