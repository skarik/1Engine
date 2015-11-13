#version 140

varying vec3 v2f_normals;
varying vec3 v2f_directional;

varying vec2 v2f_texcoord0;

uniform sampler2D textureDiffuse;
uniform vec4 sys_Time;

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
	float 	sys_FogEnd;
	float 	sys_FogScale;
};

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
	
	vec4 finalColor = gm_SkyColor;
	
	float horizonfactor = 1.0-abs( v2f_normals.z );
	horizonfactor = horizonfactor;
	
	float sunfactor = dot( gm_LightDirection, v2f_normals );
	sunfactor = max( 0.0, sunfactor );
	sunfactor = sunfactor*sunfactor;
	sunfactor = mix( sunfactor*sunfactor, sunfactor, horizonfactor );
	
	finalColor = mix( finalColor, sys_DiffuseColor, horizonfactor );
	//finalColor = mix( finalColor, mix(finalColor,gl_Fog.color,0.8), pow(horizonfactor,3) );
	//finalColor = mix( finalColor, gl_Fog.color, pow(horizonfactor,14) );
	finalColor = mix( finalColor, sys_FogColor, pow(min(1.0,1.0-v2f_normals.z),5.0)*gm_HorizonStrength );
	finalColor = mix( finalColor, gm_SunColor, sunfactor );
	//finalColor += gm_SunColor*sunfactor;
	//finalColor = mix( finalColor, gm_SkyColor, 0.999 );
	
	//finalColor *= 0.01;
	
	gl_FragColor = finalColor;
	gl_FragColor.a = 1.0;
}