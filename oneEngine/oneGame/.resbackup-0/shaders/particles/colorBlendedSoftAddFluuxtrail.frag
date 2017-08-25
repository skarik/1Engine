// Inputs from vertex shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec3 v2f_emissive;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying float v2f_fogdensity;

// Constants
uniform vec4 sys_Time;
uniform vec4 sys_SinTime;

// Samplers
uniform sampler2D textureSampler0;
uniform sampler2D textureSampler1; // bubbllesss

// Constants
uniform float sys_AlphaCutoff;

void main ( void )  
{
	vec4 diffuseColor = texture2D( textureSampler0, v2f_texcoord0 );
	
	float alpha = 0.0;
	float alphamap = texture2D( textureSampler1, vec2(v2f_texcoord0.x,v2f_texcoord0.y*9.0+sys_Time.z*1.5)*0.7 ).r;
	alphamap -= texture2D( textureSampler1,(vec2(v2f_texcoord0.x*1.5,v2f_texcoord0.y*7.2+sys_Time.z*2.0) + vec2(sys_SinTime.y,sys_Time.y*0.3)*3.0)*0.4 ).r;
	alphamap -= texture2D( textureSampler1,(vec2(v2f_texcoord0.x*1.5,v2f_texcoord0.y*7.2+sys_Time.z*2.0) + vec2(sys_SinTime.y,sys_Time.y*0.3)*3.0)*0.4 ).r;
	//gl_FragColor.a = bubbleColor.r;
	if ( alphamap+0.4 < diffuseColor.a*diffuseColor.a ) {
		alpha = 1.0;
	}
	else {
		alpha = 0.0;
	}
	alpha *= diffuseColor.a*v2f_colors.a;
	
	if ( alpha < sys_AlphaCutoff ) discard;

	gl_FragColor.rgb = mix( gl_Fog.color.rgb, diffuseColor.rgb * (v2f_colors.rgb+v2f_emissive), v2f_fogdensity ) * alpha;
	gl_FragColor.a = alpha;
	
}
/*
float alphamap = texture2D( textureSampler2, v2f_texcoord0*0.8 + vec2(sys_CosTime.x,sys_Time.y*0.7) );
	alphamap -= texture2D( textureSampler2, v2f_texcoord0*1.5 + vec2(sys_SinTime.y,sys_Time.y*0.3) );
	*/