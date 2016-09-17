#version 330

in vec3 v2f_directional;
in vec3 v2f_directional_projection;

// Samplers
uniform sampler2D textureSampler0; // Starfield
uniform sampler2D textureSampler1;
uniform sampler2D textureSampler2;

// Sys
uniform vec4 sys_Time;
uniform vec3 sys_WorldCameraPos;
uniform vec3 sys_WorldOffset;

uniform vec3 gm_LightDirection;

// Horizon color
uniform vec4 sys_DiffuseColor;
// Sun glow color
uniform vec4 gm_SunColor;
// General sky color
uniform vec4 gm_SkyColor;
// Horizon strength
uniform float gm_HorizonStrength;

// Fog
layout(std140) uniform sys_Fog
{
	vec4	sys_FogColor;
	vec4	sys_AtmoColor;
	float 	sys_FogEnd;
	float 	sys_FogScale;
};

const float WorldRadius = 40000000;
//const float WorldRadius = 20000;

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
// Random value
vec2 random ( vec3 seed3 )
{
	float dot_product = dot(seed3, vec3(12.9898,78.233,45.164));
	dot_product = SmoothTriangleWave(dot_product);
    return vec2( fract(dot_product * 43758.5453), fract(dot_product * 35362.2151) );
}

vec3 build_sky ( vec3 spherical_direction, out float space_effect )
{
	vec3 camera_position = sys_WorldCameraPos + sys_WorldOffset;
	vec3 color = gm_SkyColor.rgb;
	// Normmal atmospheric gradient
	color = pow( color, vec3(1,1,1) * (spherical_direction.z + 2.0F) );

	// Pull the sky into space
	float horizon_pull_strength = clamp( max(camera_position.z, 0.0) / pow(WorldRadius, 0.4) - 0.2, 0.0,1.0 );
	color = mix( color, vec3(0,0,0), horizon_pull_strength );

	// Sample the stars
	vec3 peturbed_sphere = spherical_direction;
	peturbed_sphere.yz += random(round(spherical_direction * 400) / 400) * 0.2;
	vec2 texture_coord = spherical_direction.xy;
	if ( abs(peturbed_sphere.z) < 0.5 )
		if ( abs(peturbed_sphere.y) < 0.5 ) texture_coord = spherical_direction.yz;
		else texture_coord = spherical_direction.xz;
	vec4 starcolor = texture( textureSampler0, texture_coord * vec2(0.5,1) );
	color += starcolor.rgb * horizon_pull_strength;

	space_effect = horizon_pull_strength;
	return color;
}
vec3 build_horizon ( vec3 spherical_direction, vec3 base_color, float space_effect )
{
	vec3 camera_position = sys_WorldCameraPos + sys_WorldOffset;
	vec3 color = base_color;
	float horizon_height = 0.0F;

	// Get the distance to the horizon to see how much to drop it
	float horizon_distance = max(camera_position.z, 0.0) / -spherical_direction.z;
	horizon_height -= pow( max(horizon_distance,0), 2.0 ) / WorldRadius;

	// Create the gradient for the horizon
	float horizon_pull_strength = max(camera_position.z, 0.0) / pow(WorldRadius, 0.5);
	float horizon_factor0 = spherical_direction.z - horizon_height; // 2 at top, 1 at horizon, 0 below horizon
	if ( horizon_factor0 > 1.0 )
	{	// Case of horizon to center of sphere
		horizon_factor0 = (horizon_factor0-1.0) / ((horizon_factor0-1.0)+1.71);
		horizon_factor0 *= horizon_pull_strength;
		horizon_factor0 += 1.0;
	}
	else if ( spherical_direction.z > 0.0 )
	{	// Case of center of sphere and upwards
		horizon_factor0 = 1.0 + mix( horizon_pull_strength, 1.0, spherical_direction.z );
	}
	else
	{	// Horizon and below
		horizon_factor0 = (horizon_factor0 + 1.0) / 2.0;
	}
	float horizon_factor1 = 1.0 - abs(1.0 - horizon_factor0); // 0 at top, 1 at horizon, 0 below horizon


	color = mix(color, sys_DiffuseColor.rgb, horizon_factor1 * (1-space_effect));
	color = mix(color, sys_FogColor.rgb, pow(min(1.0, 2.0 - horizon_factor0),5.0) * gm_HorizonStrength * (1-space_effect));

	/*if ( spherical_direction.z < horizon_height )
	{
		color = mix( color, sys_FogColor.rgb, space_effect );
	}*/

	return color;
}
vec3 build_sun ( vec3 spherical_direction, vec3 base_color, float space_effect )
{
	return base_color;
}

void main ( void )
{
	/*vec4 finalColor = sys_DiffuseColor;
	float fHorizonFactor = abs( v2f_normals.z );
	//gl_FragColor *= fHorizonFactor*sys_DiffuseColor;

	//gl_FragColor += max(0, v2f_directional.x*(1-fHorizonFactor)*(1-fHorizonFactor) )*vec4(0.2,0.2,0.1,0);
	//gl_Fog.color
	//finalColor = sys_DiffuseColor + (gl_Fog.color-sys_DiffuseColor)*(1-fHorizonFactor);
	float averageColor = (sys_DiffuseColor.r*0.9 + sys_DiffuseColor.g + sys_DiffuseColor.b*1.3) / 3.2;
	finalColor = mix( finalColor, vec4( averageColor*0.72, averageColor*0.77, averageColor, 1.0 ), fHorizonFactor-v2f_directional.x*0.3 );
	finalColor = finalColor + (gl_Fog.color-finalColor)*(1.0-fHorizonFactor)*(1.0-fHorizonFactor);
	//finalColor = //finalColor + (gl_Fog.color-finalColor)*(1.0-fHorizonFactor)*(1.0-fHorizonFactor);
	finalColor.a = 1;

	gl_FragColor = finalColor;*/
	vec3 spherical_direction = normalize(v2f_directional);

	vec4 color = vec4(0,0,0,1);
	float space_effect;
	color.rgb = build_sky(spherical_direction, space_effect);
	color.rgb = build_horizon(spherical_direction, color.rgb, space_effect);
	color.rgb = build_sun(spherical_direction, color.rgb, space_effect);

	/*float horizonfactor = 1.0-abs( v2f_normals.z );
	horizonfactor = horizonfactor;

	float sunfactor = dot( gm_LightDirection, v2f_normals );
	sunfactor = max( 0.0, sunfactor );
	sunfactor = sunfactor*sunfactor;
	sunfactor = mix( sunfactor*sunfactor, sunfactor, horizonfactor );

	finalColor = mix( finalColor, sys_DiffuseColor, horizonfactor );
	finalColor = mix( finalColor, sys_FogColor, pow(min(1.0,1.0-v2f_normals.z),5.0)*gm_HorizonStrength );
	finalColor = mix( finalColor, gm_SunColor, sunfactor );

	finalColor.rgb = vec3(1,1,1) * dot( spherical_direction, gm_LightDirection );

	finalColor.a = 1.0;*/

	gl_FragColor = color;
}
