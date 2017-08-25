varying vec4 v2f_normals;
varying vec2 v2f_texcoord0;

uniform vec4 sys_DiffuseColor;

uniform sampler2D textureDiffuse;
uniform vec4 sys_Time;

void main ( void )  
{
	vec4 finalColor = sys_DiffuseColor;
	float fHorizonFactor = abs( v2f_normals.z );
	//gl_FragColor *= fHorizonFactor*sys_DiffuseColor;
	
	//gl_FragColor += max(0, v2f_normals.w*(1-fHorizonFactor)*(1-fHorizonFactor) )*vec4(0.2,0.2,0.1,0);
	//gl_Fog.color
	//finalColor = sys_DiffuseColor + (gl_Fog.color-sys_DiffuseColor)*(1-fHorizonFactor);
	float averageColor = (sys_DiffuseColor.r*0.9 + sys_DiffuseColor.g + sys_DiffuseColor.b*1.3) / 3.2;
	finalColor = mix( finalColor, vec4( averageColor*0.72, averageColor*0.77, averageColor, 1.0 ), fHorizonFactor-v2f_normals.w*0.3 );
	finalColor = finalColor + (gl_Fog.color-finalColor)*(1.0-fHorizonFactor)*(1.0-fHorizonFactor);
	//finalColor = //finalColor + (gl_Fog.color-finalColor)*(1.0-fHorizonFactor)*(1.0-fHorizonFactor);
	finalColor.a = 1;
	
	gl_FragColor = finalColor;
}