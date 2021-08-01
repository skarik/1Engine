// sys/fullbright
// Renders object without lighting, only diffuse multiply blending.
#version 430

layout(location = 0) in vec3 mdl_Vertex;

// Outputs to fragment shader
// [None]

// System inputs
layout(binding = 0, std140) uniform sys_cbuffer_PerObject
{
    mat4 sys_ModelTRS;
    mat4 sys_ModelRS;
    mat4 sys_ModelViewProjectionMatrix;
    mat4 sys_ModelViewProjectionMatrixInverse;
};


void main ( void )
{
	vec4 v_localPos = vec4( mdl_Vertex, 1.0 );
	vec4 v_screenPos = sys_ModelViewProjectionMatrix * v_localPos;

	gl_Position = v_screenPos;
}
