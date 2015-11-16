
// Light direction
uniform vec3 gm_LightDirection;
// Horizon color
//uniform vec4 sys_DiffuseColor;
// Sun glow color
uniform vec4 gm_SunColor;
// General sky color
uniform vec4 gm_SkyColor;
// Horizon strength
uniform float gm_HorizonStrength;

vec4 mainDiffuse ( void )
{
	vec4 finalColor = gm_SkyColor;
	
	float horizonfactor = 1-abs( v2f_normals.z );
	horizonfactor = horizonfactor;
	
	float sunfactor = dot( gm_LightDirection, v2f_normals.xyz );
	sunfactor = max( 0, sunfactor );
	sunfactor = sunfactor*sunfactor;
	sunfactor = mix( sunfactor*sunfactor, sunfactor, horizonfactor );
	
	finalColor = mix( finalColor, sys_DiffuseColor, horizonfactor );
	finalColor = mix( finalColor, sys_FogColor, pow(min(1,1-v2f_normals.z),5)*gm_HorizonStrength );
	finalColor = mix( finalColor, gm_SunColor, sunfactor );
	
	return vec4( finalColor.rgb, 1.0 );
}