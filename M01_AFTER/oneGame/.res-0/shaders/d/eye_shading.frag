#version 140

// Inputs from vertex shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying float v2f_fogdensity;

// Samplers
uniform sampler2D textureSampler0;  // Base
uniform sampler2D textureSampler1; // Iris
uniform sampler2D textureSampler2; // Pupil

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

// World Reflection
layout(std140) uniform sys_ReflectInfo
{
	vec4 sys_ReflectMinBox;
	vec4 sys_ReflectMaxBox;
	vec4 sys_ReflectSource;
};
uniform samplerCube textureReflection0; // World Reflection

uniform vec3 sys_WorldCameraPos;

// Fog
layout(std140) uniform sys_Fog
{
	vec4	sys_FogColor;
	vec4	sys_AtmoColor;
	float 	sys_FogEnd;
	float 	sys_FogScale;
};

float cellShade ( float lumin )
{
	const int levels = 2;
	float t = lumin*levels;
	float dif = 0.5 - mod(t+0.5,1);
	t += dif * min(1,(0.5-abs(dif))*32) * 0.5;

	lumin = t/levels;
	return lumin;
}
float specularCellShade ( float lumin )
{
	/*const int levels = 2;
	float t = lumin*levels;
	float dif = 0.5 - mod(t+0.5,1);
	t += dif * min(1,(0.5-abs(dif))*32) * 0.5;

	lumin = t/levels;
	return lumin;*/
	return max( 0, min( 1.414, (lumin-0.6)*2 ) );
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
float specularLighting( vec3 normal, vec3 lightdir, vec3 viewdir, float specular_pow,  float lightRange )
{ //TODO: DISTANCE ATTENUATION
	// Create the reflection vector
	vec3 reflectdir = 2*dot(normal,lightdir)*normalize(normal) - normalize(lightdir);
	// Attenuation is based on camera position and reflection dir
	float attenuation = max( dot( normalize(reflectdir), normalize(viewdir) ), 0.0 );
	// Also add distance-based attenuation though
	float distance_attenuation = max( 1.0 - (length( lightdir )*lightRange*0.15), 0.0 );

	return pow( attenuation*distance_attenuation, specular_pow+((1.0-distance_attenuation)*2.0) );
}

// Box projection code shamelessly ripped from http://devlog-martinsh.blogspot.com
// because sphere projection kept giving a really drunk result
vec3 bpcem ( in vec3 n_raydir )
{
	vec3 nrdir = normalize(n_raydir);
	vec3 rbmax = (sys_ReflectMaxBox.xyz - v2f_position.xyz)/nrdir;
	vec3 rbmin = (sys_ReflectMinBox.xyz - v2f_position.xyz)/nrdir;

	vec3 rbminmax;
	rbminmax.x = (nrdir.x>0.0)?rbmax.x:rbmin.x;
	rbminmax.y = (nrdir.y>0.0)?rbmax.y:rbmin.y;
	rbminmax.z = (nrdir.z>0.0)?rbmax.z:rbmin.z;
	float fa = min(min(rbminmax.x, rbminmax.y), rbminmax.z);
	vec3 posonbox = v2f_position.xyz + nrdir * fa;
	return posonbox - sys_ReflectSource.xyz;
}

void main ( void )
{
	vec4 baseColor = texture2D( textureSampler0, v2f_texcoord0 );
	vec4 irisColor = texture2D( textureSampler1, v2f_texcoord0 ) * sys_DiffuseColor;
	vec4 pupilColor = texture2D( textureSampler2, v2f_texcoord0 ) * sys_DiffuseColor * 0.8;
	pupilColor.rgb += pupilColor.rgb*pupilColor.rgb*1.6; //Psuedo glowmap on the pupil

	vec4 diffuseColor = baseColor*(1-irisColor.a) + (irisColor*irisColor.a*(1-pupilColor.a)) + (pupilColor*pupilColor.a);

	float baseStr = min( 1, (baseColor.r+irisColor.r*irisColor.a)*4 );
	//v2f_normals.xyz *= 1-(irisColor.a*2);
	//v2f_normals.xyz *= 1-(pupilColor.a*2);

	// Create direction vector from camera to pixel
	vec3 vertDir = sys_WorldCameraPos-v2f_position.xyz;
	// Create fresnel aspect
	float fresnelAspect = (1-dot( v2f_normals.xyz, normalize( vertDir ) ) );

	// Create the reflection vector and compute reflection
	vec3 pixelReflect = normalize(vertDir) - 2*dot(v2f_normals.xyz,normalize(vertDir))*normalize(v2f_normals.xyz);
	//pixelReflect.z += 0.5;
	//pixelReflect += -v2f_normals.xyz*0.5;
	vec4 reflectionColor = texture( textureReflection0, bpcem(pixelReflect).xzy,4 );

	//vec3 lightColor = sys_EmissiveColor;	// gl_SecondaryColor is set as emissive when useColors = false for materials
	vec3 lightColor = vec3( 0,0,0 );
	vec3 specColor = vec3( 0,0,0 );
	for ( int i = 0; i < 8; i += 1 )
	{
		vec3 lightDir;
		lightDir = sys_LightPosition[i].xyz - v2f_position.xyz;

		float lightVal1 = diffuseLighting( v2f_normals.xyz, lightDir, sys_LightProperties[i].x, sys_LightProperties[i].y, sys_LightProperties[i].z );
		float lightVal2 = max( dot( sys_LightPosition[i].xyz, v2f_normals.xyz ), 0.0 );

		float specLight1 = specularLighting( v2f_normals.xyz, lightDir, vertDir, 23, sys_LightProperties[i].x );
		float specLight2 = specularLighting( v2f_normals.xyz, sys_LightPosition[i].xyz, vertDir, 23, 0 );

		// lame ass reflect here
		//lightColor += sys_LightColor[i].rgb * min( 1.9, mix( lightVal2, lightVal1, sys_LightPosition[i].w )*(pupilColor.a*pupilColor.a*4)*(mix( specLight2, specLight1, sys_LightPosition[i].w )+1.4) );

		lightColor += sys_LightColor[i].rgb * mix( lightVal2, lightVal1, sys_LightPosition[i].w );
		specColor += sys_LightColor[i].rgb * specularCellShade( mix( specLight2, specLight1, sys_LightPosition[i].w ) * 3 * baseStr );
	}
	// Add specular highlights
	diffuseColor.rgb += (diffuseColor.rgb+vec3(0.3,0.3,0.3))*specColor;
	// Add a little fresnel lighting
	lightColor += (vec3(0.5,0.5,0.5)+sys_EmissiveColor) * cellShade(fresnelAspect * irisColor.a * 1.4 * max(-v2f_normals.z+0.3,0.5));
	// Add any iruis glows
	lightColor += sys_EmissiveColor * irisColor.a;
	// Add reflection
	lightColor += 0.4 * (lightColor+vec3(0.1,0.1,0.1)) * (
			specularCellShade( pow(reflectionColor.r,2) * 3 ) *
			min(max(v2f_normals.z+0.433,0),1) *
			(1-fresnelAspect + irisColor.a*2)
		);
	//lightColor += lightColor*reflectionColor.rgb;
	//diffuseColor.rgb += reflectionColor.rgb;
	// Add the ambient color
	lightColor = max( vec3(0.2,0.2,0.3)*min( length(sys_LightAmbient.rgb)*0.7, 1.5 ),lightColor );
	lightColor += sys_LightAmbient.rgb*0.7;


	gl_FragColor = mix( sys_FogColor, diffuseColor * vec4( lightColor, 1.0 ), v2f_fogdensity );

	/*gl_FragColor.rgb *= 0.01;
	gl_FragColor.rgb += reflectionColor.rgb;*/

	gl_FragColor.a = 1.0;// * sys_DiffuseColor.a;
}
