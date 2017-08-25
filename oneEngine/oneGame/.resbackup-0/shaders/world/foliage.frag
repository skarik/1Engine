// Inputs from vertex shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec3 v2f_emissive;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying float v2f_fogdensity;

// Samplers
uniform sampler2D textureSampler0;
uniform sampler2D textureSampler1; // Glowmap

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

// System
uniform float sys_AlphaCutoff;
uniform vec3 sys_WorldCameraPos;

// Game Inputs
uniform float	gm_FadeValue;

float cellShade ( float lumin )
{
	const int levels = 2;
	float t = lumin*levels;
	float dif = 0.5 - mod(t+0.5,1);
	t += dif * min(1,(0.5-abs(dif))*32) * 0.5;
	
	lumin = t/levels;
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
float specularLighting( vec3 normal, vec3 lightdir, vec3 viewdir, float specular_pow, float lightRange, float flip )
{
	// Create the reflection vector
	vec3 reflectdir = 2*dot(normal,lightdir)*normalize(normal) - normalize(lightdir);
	// Attenuation is based on camera position and reflection dir
	float attenuation = max( flip*dot( normalize(reflectdir), normalize(viewdir) ), 0.0 );
	// Also add distance-based attenuation though
	float distance_attenuation = max( 1.0 - (length( lightdir )*lightRange*0.5), 0.0 );
	
	return pow( attenuation*distance_attenuation, specular_pow+((1.0-distance_attenuation)*2.0) );
}

vec3 defaultLighting ( vec4 lightPosition, vec4 lightProperties, vec4 lightColor, vec3 vertDir )
{
	vec3 diffuseColor = vec3( 0,0,0 );

	vec3 lightDir;
	lightDir = lightPosition.xyz - v2f_position.xyz;

	float lightVal1, lightVal2;
	if ( gl_FrontFacing )
	{
		lightVal1 = diffuseLighting( v2f_normals.xyz, lightDir, lightProperties.x, lightProperties.y, lightProperties.z );
		lightVal2 = max( dot( lightPosition.xyz, v2f_normals.xyz ), 0.0 );
		
		//lightVal1 += diffuseLighting( (lightDir-v2f_normals.xyz)*0.5, lightDir, lightProperties.x, lightProperties.y, lightProperties.z );
		//lightVal2 += max( dot( lightPosition.xyz, (lightPosition.xyz-v2f_normals.xyz)*0.5 ), 0.0 );
	}
	else
	{
		lightVal1 = diffuseLighting( (lightDir-v2f_normals.xyz)*0.5, lightDir, lightProperties.x, lightProperties.y, lightProperties.z );
		lightVal2 = max( dot( lightPosition.xyz, (lightPosition.xyz-v2f_normals.xyz)*0.5 ), 0.0 );
		//lightVal1 *= 0.5;
		//lightVal2 *= 0.5;
	}
	
	diffuseColor += lightColor.rgb * cellShade(mix( lightVal2, lightVal1, lightPosition.w ));

	// Specular lighting
	float specLight1, specLight2;
	if ( gl_FrontFacing )
	{
		specLight1 = specularLighting( v2f_normals.xyz, lightDir, vertDir, 40, lightProperties.x,1 );
		specLight2 = specularLighting( v2f_normals.xyz, lightPosition.xyz, vertDir, 40, 0,1 ); 
		
		specLight1 += specularLighting( (lightDir-v2f_normals.xyz)*0.5, lightDir, vertDir, 30, lightProperties.x,-1 ) * 0.6;
		specLight2 += specularLighting( (lightDir-v2f_normals.xyz)*0.5, lightPosition.xyz, vertDir, 30, 0,-1 ) * 0.6; 
	}
	else
	{
		specLight1 = specularLighting( v2f_normals.xyz, lightDir, vertDir, 20, lightProperties.x,1 );
		specLight2 = specularLighting( v2f_normals.xyz, lightPosition.xyz, vertDir, 20, 0,1 ); 
	}
	diffuseColor += lightColor.rgb * mix( specLight2, specLight1, lightPosition.w ) * 1.2;

	return diffuseColor;
}

void main ( void )  
{
	vec4 diffuseColor = texture2D( textureSampler0, v2f_texcoord0 );
	
	float f_alpha = diffuseColor.a * v2f_colors.a;
	if ( f_alpha < sys_AlphaCutoff ) discard;
	
	vec3 lightColor = v2f_emissive;	// gl_SecondaryColor is set as emissive when useColors = false for materials
	lightColor += texture2D( textureSampler1, v2f_texcoord0 ).rgb;
	
	lightColor += sys_LightAmbient.rgb;
	/*for ( int i = 0; i < 8; i += 1 )
	{
		vec3 lightDir;
		lightDir = sys_LightPosition[i].xyz - v2f_position.xyz;
		
		float lightVal1 = diffuseLighting( v2f_normals.xyz, lightDir, sys_LightProperties[i].x, sys_LightProperties[i].y, sys_LightProperties[i].z );
		float lightVal2 = max( dot( sys_LightPosition[i].xyz, v2f_normals.xyz ), 0.0 );
		
		lightColor += sys_LightColor[i].rgb * mix( lightVal2, lightVal1, sys_LightPosition[i].w );
	}*/
	vec3 vertDir = sys_WorldCameraPos-v2f_position.xyz;
	lightColor += defaultLighting ( sys_LightPosition[0], sys_LightProperties[0], sys_LightColor[0], vertDir );
	lightColor += defaultLighting ( sys_LightPosition[1], sys_LightProperties[1], sys_LightColor[1], vertDir );
	lightColor += defaultLighting ( sys_LightPosition[2], sys_LightProperties[2], sys_LightColor[2], vertDir );
	lightColor += defaultLighting ( sys_LightPosition[3], sys_LightProperties[3], sys_LightColor[3], vertDir );
	lightColor += defaultLighting ( sys_LightPosition[4], sys_LightProperties[4], sys_LightColor[4], vertDir );
	lightColor += defaultLighting ( sys_LightPosition[5], sys_LightProperties[5], sys_LightColor[5], vertDir );
	lightColor += defaultLighting ( sys_LightPosition[6], sys_LightProperties[6], sys_LightColor[6], vertDir );
	lightColor += defaultLighting ( sys_LightPosition[7], sys_LightProperties[7], sys_LightColor[7], vertDir );
	
	gl_FragColor = mix( gl_Fog.color, diffuseColor * v2f_colors * vec4( lightColor, 1.0 ), v2f_fogdensity );
	gl_FragColor.a = ;
}