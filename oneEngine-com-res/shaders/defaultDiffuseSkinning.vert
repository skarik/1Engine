// Outputs to fragment shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying float v2f_fogdensity;

// System inputs
uniform mat4 sys_ModelTRS;
uniform mat4 sys_ModelRS;
uniform mat4 sys_BoneMatrix[24];
uniform float sys_BoneOffset;

// Per-vertex Attributes
attribute vec4 sys_BoneWeights;
attribute vec4 sys_BoneIndices;

void main ( void )
{
	mat4 finalMatx;
	sys_BoneIndices.x -= sys_BoneOffset;
	sys_BoneIndices.y -= sys_BoneOffset;
	sys_BoneIndices.z -= sys_BoneOffset;
	sys_BoneIndices.w -= sys_BoneOffset;
	finalMatx = sys_BoneMatrix[int(sys_BoneIndices.x)] * sys_BoneWeights.x;
	finalMatx += sys_BoneMatrix[int(sys_BoneIndices.y)] * sys_BoneWeights.y;
	finalMatx += sys_BoneMatrix[int(sys_BoneIndices.z)] * sys_BoneWeights.z;
	finalMatx += sys_BoneMatrix[int(sys_BoneIndices.w)] * sys_BoneWeights.w;
	
	vec4 v_vertexPos = finalMatx*gl_Vertex;
	
	v2f_normals		= normalize( (finalMatx * vec4( gl_NormalMatrix*gl_Normal,0 )) );
	v2f_colors		= gl_Color;
	v2f_position	= gl_ModelViewMatrix * v_vertexPos;
	v2f_texcoord0	= gl_MultiTexCoord0.xy;
	
	vec4 v_screenPos = gl_ModelViewProjectionMatrix * v_vertexPos;
	v2f_fogdensity  = max(0,(gl_Fog.end - v_screenPos.z) * gl_Fog.scale);
	
	/*v2f_colors = sys_BoneWeights;
	
	// This makes it bright white...meaning the bone indices are being passed wrong.
	if ( sys_BoneIndices.x > 250 )
		sys_BoneIndices.x = -1;
	if ( sys_BoneIndices.y > 250 )
		sys_BoneIndices.y = -1;
	if ( sys_BoneIndices.z > 250 )
		sys_BoneIndices.z = -1;
	if ( sys_BoneIndices.w > 250 )
		sys_BoneIndices.w = -1;
	v2f_colors = vec4( float(sys_BoneIndices.x),float(sys_BoneIndices.y),float(sys_BoneIndices.z),float(sys_BoneIndices.w) ) * ( 1.0/256 );*/

	gl_Position = v_screenPos;
}