#version 140

// Inputs from vertex shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying float v2f_fogdensity;
varying vec2 v2f_weatherdensity;

// Samplers
uniform sampler2D textureSampler0;

// System
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

// Fog
layout(std140) uniform sys_Fog
{
	vec4	sys_FogColor;
	vec4	sys_AtmoColor;
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
	vec4 diffuseColor = texture2D( textureSampler0, v2f_texcoord0 );
	float f_alpha = diffuseColor.a * v2f_colors.a;
	if ( f_alpha < sys_AlphaCutoff ) discard;

	vec3 lightColor = vec3 ( 0.0, 0.0, 0.0 );

	// Shift the hue with the hue shift value
	/*vec3 diffuseHSV = rgb2hsv( diffuseColor.rgb );
	diffuseHSV.r -= 0.18;
	diffuseHSV.g += 0.2;
	diffuseColor.rgb = hsv2rgb( diffuseHSV );*/

	diffuseColor.rgb += (vec3(1,1,1)-diffuseColor.rgb)*v2f_weatherdensity.x*(min(v2f_normals.z+0.4,1));

	float fresnelFactor = (( dot( v2f_normals.xyz, vec3( 0.0,0.0,1.0 ) ) + 3.0 )/4.0);

	lightColor += sys_LightAmbient.rgb * fresnelFactor;
	for ( int i = 0; i < 8; i += 1 )
	{
		vec3 lightDir;
		lightDir = sys_LightPosition[i].xyz - v2f_position.xyz;

		float lightVal1 = diffuseLighting( v2f_normals.xyz, lightDir, sys_LightProperties[i].x, sys_LightProperties[i].y, sys_LightProperties[i].z );
		float lightVal2 = max( dot( sys_LightPosition[i].xyz, v2f_normals.xyz ), 0.0 );

		lightColor += sys_LightColor[i].rgb * mix( lightVal2, lightVal1, sys_LightPosition[i].w ) * 1.6;

	}

	gl_FragColor = mix( sys_FogColor, diffuseColor * v2f_colors * vec4( lightColor, 1.0 ), v2f_fogdensity );
	gl_FragColor.a = f_alpha;
}
