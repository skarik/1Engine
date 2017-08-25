varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying float v2f_fogdensity;
varying vec3 v2f_screenpos;

uniform mat4 sys_ModelMatrix;
uniform mat4 sys_ModelRotationMatrix;
uniform mat4 sys_LightMatrix[8];
uniform vec3 sys_WorldCameraPos;

varying vec4 v2f_lightcoord[8];

void main ( void )
{
	v2f_normals		= sys_ModelRotationMatrix * vec4( gl_Normal, 1 );
	//v2f_normals		= vec4( gl_NormalMatrix*gl_Normal,1 );
	v2f_colors		= gl_Color;
	//v2f_position	= sys_ModelMatrix * gl_Vertex;
	//v2f_position	= gl_ModelViewMatrix * gl_Vertex;
	v2f_position	= sys_ModelMatrix*gl_Vertex;
	v2f_texcoord0	= gl_MultiTexCoord0.xy;
	//v2f_fogdensity  = max(0, (gl_Fog.end - v_screenPos.z) * gl_Fog.scale * 1.2);
	//vec3 temp = (v2f_position.xyz-sys_WorldCameraPos);
	//v2f_fogdensity = min( max( 0, 1-sqrt(dot(temp,temp))*0.1*gl_Fog.scale ), 1.0 );
	//float temp = length(v2f_position.xyz-sys_WorldCameraPos);
	//v2f_fogdensity = min( 1, max( 0, 1024/temp ) );
	//v2f_fogdensity = 1;
	//temp = temp/2048;
	//temp = 1;
	//v2f_colors = vec4( temp, temp, temp, 1 );
	//v2f_colors = vec4( 1, 1, 1, 1 );
	vec3 dir = (v2f_position.xyz-vec3(1024,1024,1024))-sys_WorldCameraPos;
	//vec3 dir = v2f_position.xyz-sys_WorldCameraPos;
	//float horizDist = length(v2f_position.xy-sys_WorldCameraPos.xy) / 256;
	float horizDist = length(dir);
	float tempf = horizDist / 312;
	//v2f_position.z -= tempf*tempf*tempf;

	vec4 v_screenPos = gl_ModelViewProjectionMatrix * v2f_position;
	
	float fog = min(1, horizDist/1200.0 )*0.9;
	v2f_fogdensity = (1-fog*fog*fog);
//
	//v2f_texcoord0 = v_screenPos.xy/v_screenPos.z;
	v2f_screenpos = vec3( v_screenPos.x,v_screenPos.y,v_screenPos.w );
	v2f_lightcoord[0] = (gl_Vertex * sys_LightMatrix[0]);
	v2f_lightcoord[1] = (gl_Vertex * sys_LightMatrix[1]);
	v2f_lightcoord[2] = (gl_Vertex * sys_LightMatrix[2]);
	v2f_lightcoord[3] = (gl_Vertex * sys_LightMatrix[3]);
	
	//v2f_lightpos[1] = vec3( sys_LightMatrix[1][0][3], sys_LightMatrix[1][1][3], sys_LightMatrix[1][2][3] );
	
	gl_Position = v_screenPos;
}