#version 140

in vec3 mdl_Vertex;
in vec3 mdl_TexCoord;
in vec4 mdl_Color;
in vec3 mdl_Normal;

varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying float v2f_fogdensity;
varying vec3 v2f_screenpos;

varying float v2f_camDist;

uniform vec3 sys_WorldCameraPos;

uniform mat4 sys_ModelMatrix;
uniform mat4 sys_ModelRotationMatrix;
uniform mat4 sys_ModelViewProjectionMatrix;

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
varying vec4 v2f_lightcoord[8];

uniform vec4 gm_WindDirection;
uniform vec3 worldOffset;

uniform vec4 sys_SinTime, sys_CosTime, sys_Time;

//in vec4 mdl_Lcoords;
//in vec4 mdl_Mcoords;

// Fog
layout(std140) uniform sys_Fog
{
	vec4	sys_FogColor;
	float 	sys_FogEnd;
	float 	sys_FogScale;
};

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
float random ( vec3 seed3 )
{
	float dot_product = dot(seed3, vec3(12.9898,78.233,45.164));
    return fract(SmoothTriangleWave(dot_product) * 43758.5453);
}

void main ( void )
{
	vec4 v_localPos = vec4( mdl_Vertex, 1.0 );
	// Calculate squared vertex weight
	float vertWeight = mdl_Color.a * mdl_Color.a;
	vec3 offset = vec3 ( 0.0,0.0,0.0 );
	
	// Slightly offset grass height based on position
	v_localPos.z += random( v_localPos.xyz )*vertWeight*1.5;
	// Offset the grass
	offset.x += (gm_WindDirection.x*12.0*(sys_SinTime.y+SmoothTriangleWave(v_localPos.z*0.2+v_localPos.x*0.12+sys_Time.z)+2.0)+sys_CosTime.z*0.3)*vertWeight;
	offset.y += (gm_WindDirection.y*12.0*(sys_CosTime.z+SmoothTriangleWave(v_localPos.z*0.2-v_localPos.y*0.12+sys_Time.z+0.5)+2.0)+sys_SinTime.y*0.3)*vertWeight;
	// Offset the grass back down based on the wind (giving a circular effect) TODO: fast sqrt for circular effect
	v_localPos.z -= (12.0*(gm_WindDirection.x*gm_WindDirection.x + gm_WindDirection.y*gm_WindDirection.y))*vertWeight;
	// Offset the location position by the position offset
	v_localPos.xyz += offset;
	
	// Push away from the camera
	vec3 dir = sys_WorldCameraPos-(sys_ModelMatrix*v_localPos).xyz;
	v2f_camDist		= length( dir );
	dir /= v2f_camDist*v2f_camDist*v2f_camDist;
	// Add a vector away from the camera
	v_localPos.xy -= dir.xy * 8.0 * vertWeight;
	v_localPos.z -= 8.0/(v2f_camDist*v2f_camDist) * vertWeight;
	
	// Calculate the screen position now based on the new vertex position
	vec4 v_screenPos = sys_ModelViewProjectionMatrix * v_localPos;

	v2f_normals		= vec4(mdl_Normal,1.0);//sys_ModelRotationMatrix*vec4( gl_Normal, 1.0 );
	v2f_normals.xy += offset.xy * 0.6;
	v2f_normals		= normalize( v2f_normals );
	v2f_colors		= mdl_Color * 1.07;
	//v2f_position	= sys_ModelMatrix*gl_Vertex; // model matrix is invalid for the grass object (is not a model)
	v2f_position	= sys_ModelMatrix*v_localPos;
	v2f_texcoord0	= mdl_TexCoord.xy;
	v2f_fogdensity = clamp( (sys_FogEnd - v_screenPos.z) * sys_FogScale, 0, 1 );
	
	
	v2f_screenpos = vec3( v_screenPos.x,v_screenPos.y,v_screenPos.w );
	v2f_lightcoord[0] = (v2f_position * sys_LightMatrix[0]);
	v2f_lightcoord[1] = (v2f_position * sys_LightMatrix[1]);
	v2f_lightcoord[2] = (v2f_position * sys_LightMatrix[2]);
	v2f_lightcoord[3] = (v2f_position * sys_LightMatrix[3]);
	
	gl_Position = v_screenPos;
}