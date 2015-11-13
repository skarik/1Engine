#version 140

// Inputs from vertex shader
in vec4 v2f_normals;
in vec4 v2f_colors;
in vec4 v2f_position;
in vec2 v2f_texcoord0;
in float v2f_fogdensity;

// Samplers
uniform sampler2D textureSampler0;
uniform sampler2D textureSampler1; // Glowmap

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
uniform vec4 sys_ClearColor;

uniform vec3 sys_WorldCameraPos;

// Game Inputs
uniform float	gm_FadeValue;

// Fog
layout(std140) uniform sys_Fog
{
	vec4	sys_FogColor;
	float 	sys_FogEnd;
	float 	sys_FogScale;
};

float diffuseLighting ( vec3 normal, vec3 lightDist, float lightRange, float lightFalloff, float lightPass )
{
	// Distance-based attenuation
	float attenuation = pow( max( 1.0 - (length( lightDist )*lightRange), 0.0 ), lightFalloff );
	
	// Cosine law * attenuation
	float color = mix( max( dot( normal,normalize( lightDist ) ), 0.0 ), 1.0, lightPass*(1.0+attenuation) ) * attenuation;
	
	// Return final color
	return color;
}

vec3 defaultLighting ( vec4 lightPosition, vec4 lightProperties, vec4 lightColor, vec3 vertDir, vec3 vertNormal )
{
	vec3 resultColor = vec3(0,0,0);

	vec3 lightDir = lightPosition.xyz - v2f_position.xyz;
		
	float lightVal1 = diffuseLighting( vertNormal, lightDir, lightProperties.x, lightProperties.y, lightProperties.z ) * 0.01;
	//float lightVal2 = max( (dot( sys_LightPosition[i].xyz, v2f_normals.xyz )+1)*0.5, 0.0 );
	float lightVal2 = (dot( lightPosition.xyz, vertNormal )+1)*0.5;
	lightVal2 = max( 0, min( 1.2, ((lightVal2-0.7)*10) + 5 ) ) * 0.7;
	
	vec3 targetColor = lightColor.rgb;// + vec3(0.1,0.1,0.1)*(1-sys_LightPosition[i].w);
	//targetColor = normalize( targetColor ) * 0.1; // normalize to brightness 0.9, full bright 2.7
	resultColor += targetColor * mix( lightVal2, lightVal1, lightPosition.w );
	
	return resultColor;
}

void main ( void )  
{
	vec4 diffuseColor = ( texture( textureSampler0, v2f_texcoord0 ) + sys_ClearColor )*0.5;
	vec3 lightColor = sys_EmissiveColor;	// gl_SecondaryColor is set as emissive when useColors = false for materials
	lightColor += texture( textureSampler1, v2f_texcoord0 ).rgb;
	
	vec3 vertDir;
	vertDir = sys_WorldCameraPos-v2f_position.xyz;
	float lightRim = pow(max(1-dot( v2f_normals.xyz, normalize(vertDir) ),0.0), 1);

	// ambient light should always be normalized to length 0.8
	vec3 normalizeAmbient = normalize( sys_LightAmbient.rgb ) * 0.7;
	
	lightColor += normalizeAmbient*(1+lightRim*0.3);
	
	/*for ( int i = 0; i < 2; i += 1 )
	{
		lightDir = sys_LightPosition[i].xyz - v2f_position.xyz;
		
		float lightVal1 = diffuseLighting( v2f_normals.xyz, lightDir, sys_LightProperties[i].x, sys_LightProperties[i].y, sys_LightProperties[i].z ) * 0.01;
		//float lightVal2 = max( (dot( sys_LightPosition[i].xyz, v2f_normals.xyz )+1)*0.5, 0.0 );
		float lightVal2 = (dot( sys_LightPosition[i].xyz, v2f_normals.xyz )+1)*0.5;
		lightVal2 = max( 0, min( 1.2, ((lightVal2-0.7)*10) + 5 ) ) * 0.7;
		
		vec3 targetColor = sys_LightColor[i].rgb;// + vec3(0.1,0.1,0.1)*(1-sys_LightPosition[i].w);
		//targetColor = normalize( targetColor ) * 0.1; // normalize to brightness 0.9, full bright 2.7
		lightColor += targetColor * mix( lightVal2, lightVal1, sys_LightPosition[i].w );
	}*/
	lightColor += defaultLighting( sys_LightPosition[0], sys_LightProperties[0], sys_LightColor[0], vertDir, v2f_normals.xyz );
	lightColor += defaultLighting( sys_LightPosition[1], sys_LightProperties[1], sys_LightColor[1], vertDir, v2f_normals.xyz );
	
	if ( length(lightColor) > 2.3 ) {
		lightColor = normalize( lightColor ) * 2.3;
	}
	
	//gl_FragColor = mix( gl_Fog.color, diffuseColor * sys_DiffuseColor * vec4( lightColor, 1.0 ), v2f_fogdensity+0.5 );
	gl_FragColor = mix( (sys_FogColor+sys_ClearColor)*0.5, diffuseColor * sys_DiffuseColor * vec4( lightColor, 1.0 ), 0.8 );
	//gl_FragColor = vec4( 1,1,1,1 );
	gl_FragColor.a = diffuseColor.a * sys_DiffuseColor.a;
}