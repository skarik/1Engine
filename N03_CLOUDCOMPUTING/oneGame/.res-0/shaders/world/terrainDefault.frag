#version 140

varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec3 v2f_texcoord0;
varying float v2f_fogdensity;
varying vec3 v2f_screenpos;
varying vec4 v2f_lightcoord[8];

varying vec4 v2f_ter_blends1;
varying vec4 v2f_ter_blends2;

uniform sampler3D textureSampler0;
uniform sampler3D textureSampler1; // Glowmap
//uniform sampler2D textureCaustics;
uniform sampler2D textureShadow0;
uniform sampler2D textureShadow1;
uniform sampler2D textureShadow2;

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
uniform vec4 terra_BaseAmbient;

// System (world)
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
	const int levels = 4;
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
	float color = mix( max( dot( normal,normalize( lightDist ) ), 0.0 ), 1.0, lightPass*(1+attenuation) ) * attenuation;

	// Return final color
	return color;
}
float specularLighting( vec3 normal, vec3 lightdir, vec3 viewdir, float specular_pow, float lightRange )
{
	// Create the reflection vector
	vec3 reflectdir = 2*dot(normal,lightdir)*normalize(normal) - normalize(lightdir);
	// Attenuation is based on camera position and reflection dir
	float attenuation = max( dot( normalize(reflectdir), normalize(viewdir) ), 0.0 );
	// Also add distance-based attenuation though
	float distance_attenuation = max( 1.0 - (length( lightdir )*lightRange*0.4), 0.0 );

	return pow( attenuation*distance_attenuation, specular_pow+((1.0-distance_attenuation)*2.0) );
}

// Sine wave appoximation method (benchmarked to about 3% speed increase over all shadows)
float SmoothCurve( float x ) {
	return x * x *( 3.0 - 2.0 * x );
}
float TriangleWave( float x ) {
	return abs( fract( x + 0.5 ) * 2.0 - 1.0 );
}
float SmoothTriangleWave( float x ) {
	return (SmoothCurve( TriangleWave( x ) ))*2 - 1;
}
// Random
vec2 random ( vec3 seed3 )
{
	float dot_product = dot(seed3, vec3(12.9898,78.233,45.164));
	dot_product = SmoothTriangleWave(dot_product);
    return vec2( fract(dot_product * 43758.5453), fract(dot_product * 35362.2151) );
}


float luminosity;
vec3 defaultLighting ( vec4 lightPosition, vec4 lightProperties, vec4 lightColor, vec3 vertDir )
{
	vec3 diffuseColor = vec3( 0,0,0 );

	vec3 lightDir;
	lightDir = lightPosition.xyz - v2f_position.xyz;

	float lightVal1 = diffuseLighting( v2f_normals.xyz, lightDir, lightProperties.x, lightProperties.y, lightProperties.z );
	float lightVal2 = max( dot( lightPosition.xyz, v2f_normals.xyz ), -0.2 );

	diffuseColor += lightColor.rgb * cellShade(mix( lightVal2, lightVal1, lightPosition.w ));

	// Specular lighting
	float specLight1 = specularLighting( v2f_normals.xyz, lightDir, vertDir, 32, lightProperties.x );
	float specLight2 = specularLighting( v2f_normals.xyz, lightPosition.xyz, vertDir, 32, 0 );
	diffuseColor += lightColor.rgb * mix( specLight2, specLight1, lightPosition.w ) * 1.4 * (1-luminosity) * v2f_colors.g * 2; //1-luminosity for fake specular map

	return diffuseColor;
}
float shadowCalculate ( vec4 lightCoords, vec4 shadowInfo, sampler2D textureShadow )
{
	float shadowDist = 1.0;
	// Generate texture coords
	vec4 shadowWcoord = vec4(
		lightCoords.x / lightCoords.w,
		lightCoords.y / lightCoords.w,
		lightCoords.z / lightCoords.w,
		lightCoords.w );
	// Get rid of the non-uniformity. Always sample shadows.
	{
		float depthDifference = 0.0;
		float distanceFromLight;
		vec2 coord;
		float bias = 0;

		for ( int i = 0; i < 4; i += 1 )
		{
			coord = shadowWcoord.st;

			coord.x -= 0.5;
			coord.y -= 0.5;

			coord.x *= 0.25;

			const float cspd = 0.98; // Cascade padding value

			// Lookup the coordinate in the cascade map
			if (( abs(coord.y) <  (0.25*0.25*0.25*0.5*cspd) )&&( abs(coord.x) <  (0.25*0.25*0.25*0.5*0.25*cspd) ))
			{
				coord *= 64;
				coord.x += 0.75 + 0.125;
				coord.y += 0.5;
				bias = 0.02;
			}
			else if (( abs(coord.y) < (0.25*0.25*0.5*cspd) )&&( abs(coord.x) <  (0.25*0.25*0.5*0.25*cspd) ))
			{
				coord *= 16;
				coord.x += 0.5 + 0.125;
				coord.y += 0.5;
				bias = 0.08;
			}
			else if (( abs(coord.y) < (0.25*0.5*cspd) )&&( abs(coord.x) <  (0.25*0.5*0.25*cspd) ))
			{
				coord *= 4;
				coord.x += 0.25 + 0.125;
				coord.y += 0.5;
				bias = 0.64;
			}
			else
			{
				coord.x += 0.125;
				coord.y += 0.5;
				bias = 2.56;
			}

			// Limit Y coordinate
			coord.y = max( min( coord.y, 1.0 ), 0.0 );

			coord += random( vec3(v2f_screenpos.xy,i) )*0.0012;
			coord += vec2( SmoothTriangleWave(i*0.25),SmoothTriangleWave(i*0.25+0.25) )*0.0012;

			distanceFromLight = texture( textureShadow, coord ).r;
			depthDifference += clamp((shadowWcoord.z - distanceFromLight)*1024.0 - bias, 0.0,1.0);
		}
		depthDifference /= 4.0;

		// Decrease dif val if close to edge
		depthDifference *= clamp( 8.0-abs(shadowWcoord.x-0.5)*16.0, 0.0,1.0 );
		depthDifference *= clamp( 8.0-abs(shadowWcoord.y-0.5)*16.0, 0.0,1.0 );

		// Do the color mix
		shadowDist = clamp( 1.0-depthDifference, 0.0,1.0 );
	}
	// If out of range or shadows are not enabled, set shadowDist to 1
	if (( shadowInfo.x <= 0.5 )||(!(( shadowWcoord.z > 0.0 )&&( shadowWcoord.x > 0.0 )&&( shadowWcoord.x < 1.0 )&&( shadowWcoord.y > 0.0 )&&( shadowWcoord.y < 1.0 ))))
	{
		shadowDist = 1.0;
	}

	return shadowDist;
}

vec4 sampleWithNormal ( sampler3D sampler, vec3 normal, vec3 position, float depth, float scale )
{
// From GPU Gems 3
	/*vec4 sample = texture3D( sampler, vec3(position.xy*v2f_texcoord0.x,v2f_texcoord0.z) );
	vec4 colorZ = vec4(0,0,1,1)+sample*0.01;//texture3D( sampler, vec3(position.xy*v2f_texcoord0.x,v2f_texcoord0.z) );
	vec4 colorX = vec4(1,0,0,1);//texture3D( sampler, vec3(position.yz*v2f_texcoord0.x,v2f_texcoord0.z) );
	vec4 colorY = vec4(0,1,0,1);//texture3D( sampler, vec3(position.xz*v2f_texcoord0.x,v2f_texcoord0.z) );
	vec4 sample = texture3D( sampler, vec3(position.xy*v2f_texcoord0.x,v2f_texcoord0.z) );*/
	vec4 colorZ = texture( sampler, vec3(position.xy*scale,depth) );
	vec4 colorX = texture( sampler, vec3(position.yz*scale,max(depth,0.09375)) );
	vec4 colorY = texture( sampler, vec3(position.xz*scale,max(depth,0.09375)) );
	//vec4 color = mix( mix( colorX, colorY, pow(abs(normal.y),1) ), colorZ, pow(abs(normal.z),1) );
	vec3 blend_weights = abs( normal.xyz );   // Tighten up the blending zone:
	//blend_weights = (blend_weights - 0.1) * 7;
	//blend_weights = blend_weights * 7;
	blend_weights = pow(blend_weights,vec3(6,6,6));
	blend_weights = max(blend_weights, 0);      // Force weights to sum to 1.0 (very important!)
	blend_weights /= (blend_weights.x + blend_weights.y + blend_weights.z );
	// now, get color
	vec4 color =	colorX.xyzw * blend_weights.xxxx +
					colorY.xyzw * blend_weights.yyyy +
					colorZ.xyzw * blend_weights.zzzz;
	return color;
}

// brightness in v2f_ter_blends1.w
// hueshift in v2f_ter_blends2.w
/*
void getHSV ( vec3 rgb, out vec3 hsv )
{
	float K = 0.0;
	float temp;

    if (rgb.g < rgb.b)
    {
        //std::swap(g, b);
		rgb.gb = rgb.bg;
        K = -1.0;
    }

    if (rgb.r < rgb.g)
    {
        //std::swap(r, g);
		rgb.rg = rgb.gr;
        K = -2.0 / 6.0 - K;
    }

    float chroma = rgb.r - min(rgb.g, rgb.b);
    hsv.x = abs(K + (rgb.g - rgb.b) / (6.0 * chroma + 1e-20));
    hsv.y = chroma / (rgb.r + 1e-20);
    hsv.z = rgb.r;
}*/

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

void main ( void )
{
	//gl_FragColor = vec4( 1,1,1,1 );
	//return;
	//vec4 diffuseColor = texture3D( textureDiffuse, v2f_texcoord0 );
	vec4 diffuseColor0 = sampleWithNormal( textureSampler0, v2f_normals.xyz, v2f_position.xyz, v2f_texcoord0.x, v2f_ter_blends2.x );
	vec4 diffuseColor1 = sampleWithNormal( textureSampler0, v2f_normals.xyz, v2f_position.xyz, v2f_texcoord0.y, v2f_ter_blends2.y );
	vec4 diffuseColor2 = sampleWithNormal( textureSampler0, v2f_normals.xyz, v2f_position.xyz, v2f_texcoord0.z, v2f_ter_blends2.z );
	vec4 diffuseColor = diffuseColor0*v2f_ter_blends1.x + diffuseColor1*v2f_ter_blends1.y + diffuseColor2*v2f_ter_blends1.z;
	vec4 glowColor0 = sampleWithNormal( textureSampler1, v2f_normals.xyz, v2f_position.xyz, v2f_texcoord0.x, v2f_ter_blends2.x );
	vec4 glowColor = glowColor0*v2f_ter_blends1.x;
	// Shift the hue with the hue shift value
	vec3 diffuseHSV = rgb2hsv( diffuseColor.rgb );
	diffuseHSV.r += v2f_ter_blends2.w;
	diffuseColor.rgb = hsv2rgb( diffuseHSV );
	// Calculate brightness for faux-map and desaturation/saturation effects
	luminosity = diffuseColor.r * 0.299 + diffuseColor.g * 0.587 + diffuseColor.b * 0.114;
	diffuseColor = mix( vec4(luminosity,luminosity,luminosity,1), diffuseColor, v2f_colors.b );
	// Darken the diffuse color with the darken value
	diffuseColor.rgb = mix( diffuseColor.rgb*diffuseColor.rgb-vec3(0.4,0.4,0.4), diffuseColor.rgb, v2f_ter_blends1.w );

	// Begin light calculations
	vec3 lightColor = vec3 ( 0.0, 0.0, 0.0 );
	lightColor += glowColor.rgb * v2f_colors.a * 2.0;

	//vec4 shadowColor;
	float shadowDist = 1.0;
	vec3 coord;

	vec3 vertDir = sys_WorldCameraPos-v2f_position.xyz;

	// This is just a normal map test
	//vec3 pix_normals = v2f_normals.xyz;
	//pix_normals += (diffuseColor.rgb-0.5)*4;
	vec3 ambientAmount = vec3(v2f_colors.r,v2f_colors.r,v2f_colors.r);//max( vec3(v2f_colors.r,v2f_colors.r,v2f_colors.r), terra_BaseAmbient.rgb );

	lightColor += /*vec3(0.2,0.2,0.2) +*/ (sys_LightAmbient.rgb-terra_BaseAmbient.rgb);
	//lightColor += shadowedLighting( sys_LightPosition[0], sys_LightProperties[0], sys_LightColor[0], v2f_lightcoord[0], sys_LightShadowInfo[0], textureShadow[0], vertDir );
	//lightColor += defaultLighting ( sys_LightPosition[0], sys_LightProperties[0], sys_LightColor[0], vertDir ) * ambientAmount;
	/*vec3 light = defaultLighting ( sys_LightPosition[0], sys_LightProperties[0], sys_LightColor[0], vertDir );
	lightColor +=
		light
		* (shadowCalculate ( v2f_lightcoord[0], sys_LightShadowInfo[0], textureShadow0 ) - min(0,light.r+light.g+light.b) );*/
	lightColor += defaultLighting ( sys_LightPosition[0], sys_LightProperties[0], sys_LightColor[0], vertDir )
		* shadowCalculate ( v2f_lightcoord[0], sys_LightShadowInfo[0], textureShadow0 );
	lightColor *= ambientAmount;
	lightColor += terra_BaseAmbient.rgb;
	lightColor += defaultLighting ( sys_LightPosition[1], sys_LightProperties[1], sys_LightColor[1], vertDir );
	//lightColor += shadowedLighting( sys_LightPosition[2], sys_LightProperties[2], sys_LightColor[2], v2f_lightcoord[2], sys_LightShadowInfo[2], textureShadow2, vertDir ) * ambientAmount;
	lightColor += defaultLighting ( sys_LightPosition[2], sys_LightProperties[2], sys_LightColor[2], vertDir );
	lightColor += defaultLighting ( sys_LightPosition[3], sys_LightProperties[3], sys_LightColor[3], vertDir );
	lightColor += defaultLighting ( sys_LightPosition[4], sys_LightProperties[4], sys_LightColor[4], vertDir );
	lightColor += defaultLighting ( sys_LightPosition[5], sys_LightProperties[5], sys_LightColor[5], vertDir );
	lightColor += defaultLighting ( sys_LightPosition[6], sys_LightProperties[6], sys_LightColor[6], vertDir );
	lightColor += defaultLighting ( sys_LightPosition[7], sys_LightProperties[7], sys_LightColor[7], vertDir );


	//gl_FragColor = vec4( v2f_normals.xyz, 1.0 )*0.5 + 0.5;
	//gl_FragColor = (vec4( v2f_normals.xyz, 1.0 )*0.5 + 0.5) * vec4( lightColor, 1.0 );
	gl_FragColor = mix( sys_FogColor, diffuseColor * vec4( lightColor, 1.0 ), max(0,min(1,v2f_fogdensity+length(lightColor)*0.3)) );
	//gl_FragColor.rgb = vec3(1,1,1);
	//gl_FragColor.a = diffuseColor.a * v2f_colors.a;
	//gl_FragColor.rgb = gl_FragColor.rgb*0.01 + ((v2f_normals.xyz*0.5+0.5))*0.99;
	//gl_FragColor.rgb = gl_FragColor.rgb*0.5 + ((v2f_normals.xyz*0.5+0.5))*0.5;

	gl_FragColor.a = 1.0;

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
