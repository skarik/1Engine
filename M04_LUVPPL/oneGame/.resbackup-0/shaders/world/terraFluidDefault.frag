varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying float v2f_fogdensity;
varying vec3 v2f_screenpos;
varying vec4 v2f_lightcoord[8];

varying float v2f_upshift;

uniform sampler2D textureDiffuse;
uniform sampler2D textureShadow[8];
uniform sampler2D textureShadow0;
uniform sampler2D textureShadow1;
uniform sampler2D textureShadow2;

uniform vec4 sys_LightAmbient;
uniform vec4 sys_LightColor[8];
uniform vec4 sys_LightPosition[8];
uniform vec4 sys_LightProperties[8];
uniform vec4 sys_LightShadowInfo[8];

uniform vec3 sys_WorldCameraPos;

float diffuseLighting ( vec3 normal, vec3 lightDist, float lightRange, float lightFalloff, float lightPass )
{
	// Distance-based attenuation
	float attenuation = pow( max( 1.0 - (length( lightDist )*lightRange), 0.0 ), lightFalloff );
	
	// Cosine law * attenuation
	float color = mix( max( dot( normal,normalize( lightDist ) ), 0.0 ), 1.0, lightPass*(1+attenuation) ) * attenuation;
	
	// Return final color
	return color;
}
float specularLighting( vec3 normal, vec3 lightdir, vec3 viewdir, float specular_pow )
{
	// Create the reflection vector
	vec3 reflectdir = 2*dot(normal,lightdir)*normalize(normal) - normalize(lightdir);
	// Attenuation is based on camera position and reflection dir
	float attenuation = max( dot( normalize(reflectdir), normalize(viewdir) ), 0.0 );
	
	return pow( attenuation, specular_pow );
}

vec2 random ( vec3 seed3 )
{
	float dot_product = dot(seed3, vec3(12.9898,78.233,45.164));
	dot_product = sin(dot_product);
    return vec2( fract(dot_product * 43758.5453), fract(dot_product * 35362.2151) );
}
vec3 defaultLighting ( vec4 lightPosition, vec4 lightProperties, vec4 lightColor, vec3 vertDir )
{
	vec3 diffuseColor = vec3( 0,0,0 );
	
		vec3 lightDir;
		lightDir = lightPosition.xyz - v2f_position.xyz;
		
		float lightVal1 = diffuseLighting( v2f_normals.xyz, lightDir, lightProperties.x, lightProperties.y, lightProperties.z );
		float lightVal2 = max( dot( lightPosition.xyz, v2f_normals.xyz ), 0.0 );
		
		diffuseColor += lightColor.rgb * mix( lightVal2, lightVal1, lightPosition.w );
		
		// Specular lighting
		float specLight = specularLighting( v2f_normals.xyz, lightDir, vertDir, 16 );
		float specLight2 = specularLighting( v2f_normals.xyz, lightPosition.xyz, vertDir, 16 ); 
		diffuseColor += lightColor.rgb * mix( specLight2, specLight, lightPosition.w ) * 1.8;
	
	return diffuseColor;
}
vec3 shadowedLighting ( vec4 lightPosition, vec4 lightProperties, vec4 lightColor, vec4 lightCoords, vec4 shadowInfo, sampler2D textureShadow, vec3 vertDir )
{
	vec3 diffuseColor = vec3( 0,0,0 );
	float shadowDist = 1.0;

	vec3 lightDir;
	lightDir = lightPosition.xyz - v2f_position.xyz;
	
	float lightVal1 = diffuseLighting( v2f_normals.xyz, lightDir, lightProperties.x, lightProperties.y, lightProperties.z );
	float lightVal2 = max( dot( lightPosition.xyz, v2f_normals.xyz ), 0.0 );
	
	// Do shadows
	vec4 shadowWcoord = vec4(
		lightCoords.x / lightCoords.w,
		lightCoords.y / lightCoords.w,
		lightCoords.z / lightCoords.w,
		lightCoords.w );
	
	if ( shadowInfo.x > 0.5 )
	{
		if (( shadowWcoord.z > 0.0 )&&( shadowWcoord.x > 0.0 )&&( shadowWcoord.x < 1.0 )&&( shadowWcoord.y > 0.0 )&&( shadowWcoord.y < 1.0 ))
		{
			float difVal = 0.0;
			float distanceFromLight;
			vec2 coord;
			
			for ( int i = 0; i < 4; i += 1 )
			{
				coord = shadowWcoord.st;
				
				coord.x -= 0.5;
				coord.y -= 0.5;
			
				coord.x *= 0.25;
				
				if (( abs(coord.y) <  (0.25*0.25*0.25*0.5) )&&( abs(coord.x) <  (0.25*0.25*0.25*0.5*0.25) ))
				{
					coord *= 64;
					coord.x += 0.75 + 0.125;
					coord.y += 0.5;
				}
				else if (( abs(coord.y) < (0.25*0.25*0.5) )&&( abs(coord.x) <  (0.25*0.25*0.5*0.25) ))
				{
					coord *= 16;
					coord.x += 0.5 + 0.125;
					coord.y += 0.5;
				}
				else if (( abs(coord.y) < (0.25*0.5) )&&( abs(coord.x) <  (0.25*0.5*0.25) ))
				{
					coord *= 4;
					coord.x += 0.25 + 0.125;
					coord.y += 0.5;
				}
				else
				{
					coord.x += 0.125;
					coord.y += 0.5;
				}
				
				coord.y = max( min( coord.y, 1 ), 0 );
				
				coord += random( vec3(v2f_screenpos.xy,i) )*0.0024;
				
				distanceFromLight = texture2D( textureShadow, coord ).z;
				difVal += clamp((shadowWcoord.z - distanceFromLight)*256.0, 0.0,1.0);
			}
			difVal /= 4.0;
			
			// Decrease dif val if close to edge
			difVal *= clamp( 8.0-abs(shadowWcoord.x-0.5)*16.0, 0.0,1.0 );
			difVal *= clamp( 8.0-abs(shadowWcoord.y-0.5)*16.0, 0.0,1.0 );
			
			// Do the color mix
			shadowDist = 1.0-difVal;
		}
		else
		{
			shadowDist = 1.0;
		}
	}
		
	diffuseColor += lightColor.rgb * mix( lightVal2 * v2f_colors.r, lightVal1, lightPosition.w ) * shadowDist;
	
	// Specular lighting
	float specLight = specularLighting( v2f_normals.xyz, lightDir, vertDir, 16 );
	float specLight2 = specularLighting( v2f_normals.xyz, lightPosition.xyz, vertDir, 16 ); 
	diffuseColor += lightColor.rgb * mix( specLight2, specLight, lightPosition.w ) * 1.8 * shadowDist;
	
	return diffuseColor;
}

void main ( void )  
{
	vec4 diffuseColor = texture2D( textureDiffuse, v2f_texcoord0 );
	//vec4 diffuseColor = texture2D( textureDiffuse, v2f_screenpos.xy/v2f_screenpos.z );
	vec3 lightColor = vec3 ( 0.0, 0.0, 0.0 );
	
	//vec4 shadowColor;
	float shadowDist = 1.0;
	vec3 coord;
	
	// This is just a normal map test
	//vec3 pix_normals = v2f_normals.xyz;
	//pix_normals += (diffuseColor.rgb-0.5)*4;
	vec3 vertDir = sys_WorldCameraPos-v2f_position.xyz;
	
	lightColor += sys_LightAmbient.rgb * v2f_colors.rgb;
	
	//lightColor += defaultLighting ( sys_LightPosition[0], sys_LightProperties[0], sys_LightColor[0], vertDir ) * v2f_colors.r;
	lightColor += shadowedLighting( sys_LightPosition[0], sys_LightProperties[0], sys_LightColor[0], v2f_lightcoord[0], sys_LightShadowInfo[0], textureShadow0, vertDir ) * v2f_colors.r;
	lightColor += shadowedLighting( sys_LightPosition[1], sys_LightProperties[1], sys_LightColor[1], v2f_lightcoord[1], sys_LightShadowInfo[1], textureShadow1, vertDir ) * v2f_colors.r;
	lightColor += shadowedLighting( sys_LightPosition[2], sys_LightProperties[2], sys_LightColor[2], v2f_lightcoord[2], sys_LightShadowInfo[2], textureShadow2, vertDir ) * v2f_colors.r;
	//lightColor += defaultLighting ( sys_LightPosition[2], sys_LightProperties[2], sys_LightColor[2], vertDir );
	lightColor += defaultLighting ( sys_LightPosition[3], sys_LightProperties[3], sys_LightColor[3], vertDir );
	lightColor += defaultLighting ( sys_LightPosition[4], sys_LightProperties[4], sys_LightColor[4], vertDir );
	lightColor += defaultLighting ( sys_LightPosition[5], sys_LightProperties[5], sys_LightColor[5], vertDir );
	lightColor += defaultLighting ( sys_LightPosition[6], sys_LightProperties[6], sys_LightColor[6], vertDir );
	lightColor += defaultLighting ( sys_LightPosition[7], sys_LightProperties[7], sys_LightColor[7], vertDir );
	
	/*for ( int i = 0; i < 8; i += 1 )
	{
		vec3 lightDir;
		lightDir = sys_LightPosition[i].xyz - v2f_position.xyz;
		
		float lightVal1 = diffuseLighting( v2f_normals.xyz, lightDir, sys_LightProperties[i].x, sys_LightProperties[i].y, sys_LightProperties[i].z );
		float lightVal2 = max( dot( sys_LightPosition[i].xyz, v2f_normals.xyz ), 0.0 );
		
		// Do shadows
		if ( i == 1 )
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
		}
		
		lightColor += sys_LightColor[i].rgb * mix( lightVal2 * v2f_colors.r, lightVal1, sys_LightPosition[i].w ) * shadowDist;
		
		float specLight = specularLighting( v2f_normals.xyz, lightDir, vertDir, 16 );
		float specLight2 = specularLighting( v2f_normals.xyz, sys_LightPosition[i].xyz, vertDir, 16 ); 
		//lightColor += sys_LightColor[i].rgb * specLight;s
		lightColor += sys_LightColor[i].rgb * mix( specLight2, specLight, sys_LightPosition[i].w ) * 1.8 * shadowDist;
		
	}*/
	
	//gl_FragColor = vec4( v2f_normals.xyz, 1.0 )*0.5 + 0.5;
	gl_FragColor = mix( gl_Fog.color, diffuseColor * vec4( lightColor, 1.0 ) * ( 1+v2f_upshift ), v2f_fogdensity );
	//gl_FragColor = diffuseColor * vec4( lightColor, 1.0 );
	//gl_FragColor.a = diffuseColor.a * v2f_colors.a * 0.7;
	gl_FragColor.a = mix( 1.0, diffuseColor.a * v2f_colors.a * 0.7, v2f_fogdensity );
	
	// Default shadow
	/*
	vec4 diffuseColor = texture2D( textureDiffuse, v2f_texcoord0 );
	vec4 fcolor = vec4(0.0,0.0,0.0,0.0);
	
	vec4 shadowWcoord = vec4(
		v2f_lightcoord[1].x / v2f_lightcoord[1].w,
		v2f_lightcoord[1].y / v2f_lightcoord[1].w,
		v2f_lightcoord[1].z / v2f_lightcoord[1].w,
		v2f_lightcoord[1].w );
	
	if ( sys_LightShadowInfo[1].x > 0.5 )
	{
		if (( shadowWcoord.z > 0 )&&( shadowWcoord.x > 0 )&&( shadowWcoord.x < 1 )&&( shadowWcoord.y > 0 )&&( shadowWcoord.y < 1 ))
		{
			float difVal = 0;
			float distanceFromLight;
			vec2 coord;
			
			for ( int i = 0; i < 4; i += 1 )
			{
				coord = shadowWcoord.st;
				coord += random( vec3(v2f_screenpos.xy,i) )*0.0015;
				distanceFromLight = texture2D( textureShadow[1], coord ).z;
				difVal += clamp((shadowWcoord.z - distanceFromLight)*128, 0,1);
			}
			difVal /= 4;
			
			// Decrease dif val if close to edge
			difVal *= clamp( 4-abs(shadowWcoord.x-0.5)*8, 0,1 );
			difVal *= clamp( 4-abs(shadowWcoord.y-0.5)*8, 0,1 );
			
			// Do the color mix
			diffuseColor = mix( diffuseColor, diffuseColor*0.4, difVal );
		}
	}
	gl_FragColor = diffuseColor;
	gl_FragColor.a = v2f_colors.a;
	*/
}

/*vec3 lightDir;
	float lightVal1;
	float lightVal2;*/
		/*
		lightColor += gl_LightSource[0].ambient.rgb;
		lightDir = v2f_position.xyz - gl_LightSource[0].position.xyz;
		lightVal1 = diffuseLighting( v2f_normals.xyz, lightDir, sys_LightRange[0], sys_LightFalloff[0] );
		lightVal2 = max( dot( gl_LightSource[0].position.xyz, v2f_normals.xyz ), 0.0 );
		lightColor += gl_LightSource[0].diffuse.rgb * mix( lightVal2, lightVal1, gl_LightSource[0].position.w );
		*/
		/*
		lightColor += gl_LightSource[1].ambient.rgb;
		lightDir = v2f_position.xyz - gl_LightSource[1].position.xyz;
		lightVal1 = diffuseLighting( v2f_normals.xyz, lightDir, sys_LightRange[1], sys_LightFalloff[1] );
		lightVal2 = max( dot( gl_LightSource[1].position.xyz, v2f_normals.xyz ), 0.0 );
		lightColor += gl_LightSource[1].diffuse.rgb * mix( lightVal2, lightVal1, gl_LightSource[1].position.w );
		*/
		/*
		lightColor += gl_LightSource[2].ambient.rgb;
		lightDir = v2f_position.xyz - gl_LightSource[2].position.xyz;
		lightVal1 = diffuseLighting( v2f_normals.xyz, lightDir, sys_LightRange[2], sys_LightFalloff[2] );
		lightVal2 = max( dot( gl_LightSource[2].position.xyz, v2f_normals.xyz ), 0.0 );
		lightColor += gl_LightSource[2].diffuse.rgb * mix( lightVal2, lightVal1, gl_LightSource[2].position.w );
		*/
		/*
		lightColor += gl_LightSource[3].ambient.rgb;
		lightDir = v2f_position.xyz - gl_LightSource[3].position.xyz;
		lightVal1 = diffuseLighting( v2f_normals.xyz, lightDir, sys_LightRange[3], sys_LightFalloff[3] );
		lightVal2 = max( dot( gl_LightSource[3].position.xyz, v2f_normals.xyz ), 0.0 );
		lightColor += gl_LightSource[3].diffuse.rgb * mix( lightVal2, lightVal1, gl_LightSource[3].position.w );
		*/