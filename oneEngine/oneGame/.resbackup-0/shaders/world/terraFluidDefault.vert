varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying float v2f_fogdensity;
varying vec3 v2f_screenpos;

uniform mat4 sys_ModelMatrix;
uniform mat4 sys_ModelRotationMatrix;
uniform mat4 sys_LightMatrix[8];

varying vec4 v2f_lightcoord[8];

uniform vec4 sys_Time;

varying float v2f_upshift;

void main ( void )
{
	vec4 shiftVertex, startVert;
	vec3 shiftNormal;
	startVert = gl_Vertex;
	shiftVertex.x = sin( startVert.y*0.3 + sys_Time.y*0.9 );
	shiftVertex.y = sin( startVert.z*0.3 + sys_Time.y*0.9 );
	shiftVertex.z = sin( startVert.x*0.3 + sys_Time.y*1.1 ) + cos( startVert.y*0.2+startVert.x*0.1 + sys_Time.y*0.8 );
	shiftVertex.w = 0;
	v2f_upshift = shiftVertex.z*0.1;
	
	shiftNormal.x = cos( startVert.y*0.3 + sys_Time.y*0.9 )*0.9;
	shiftNormal.y = cos( startVert.y*0.3 + sys_Time.y*0.9 )*0.9;
	shiftNormal.z = cos( startVert.x*0.3 + sys_Time.y*1.1 )*1.1 - sin( startVert.y*0.2+startVert.x*0.1 + sys_Time.y*0.8 )*0.8;
	
	//shiftVertex += gl_Vertex;
	//shiftVertex = startVert + (shiftVertex*0.35*gl_Color.g);
	v2f_position	= startVert + (shiftVertex*0.35*gl_Color.g);

	vec4 v_screenPos = gl_ModelViewProjectionMatrix * v2f_position;

	v2f_normals		= vec4( normalize(gl_Normal+(shiftNormal*0.4*gl_Color.g)), 1 );
	//v2f_normals		= vec4( gl_NormalMatrix*gl_Normal,1 );
	v2f_colors		= gl_Color;
	//v2f_position	= sys_ModelMatrix * gl_Vertex;
	//v2f_position	= gl_ModelViewMatrix * gl_Vertex;
	v2f_texcoord0	= gl_MultiTexCoord0.xy;
	//v2f_fogdensity  = max(0, (gl_Fog.end - v_screenPos.z) * gl_Fog.scale );
	v2f_fogdensity  = max(0, (gl_Fog.end*0.5 - v_screenPos.z) * gl_Fog.scale );
//
	//v2f_texcoord0 = v_screenPos.xy/v_screenPos.z;
	v2f_screenpos = vec3( v_screenPos.x,v_screenPos.y,v_screenPos.w );
	v2f_lightcoord[0] = (v2f_position * sys_LightMatrix[0]);
	v2f_lightcoord[1] = (v2f_position * sys_LightMatrix[1]);
	v2f_lightcoord[2] = (v2f_position * sys_LightMatrix[2]);
	v2f_lightcoord[3] = (v2f_position * sys_LightMatrix[3]);
	
	//v2f_lightpos[1] = vec3( sys_LightMatrix[1][0][3], sys_LightMatrix[1][1][3], sys_LightMatrix[1][2][3] );
	
	gl_Position = v_screenPos;
}