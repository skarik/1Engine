#version 330
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

in vec3 mdl_Vertex;
in vec3 mdl_TexCoord;

// Outputs to fragment shader
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying vec2 v2f_texcoord1;

// System inputs
//layout(location = 140) uniform float sys_PixelRatio;
layout(std140) uniform sys_cbuffer_PerCamera
{
    mat4 sys_ViewProjectionMatrix;
    vec4 sys_WorldCameraPos;
    vec4 sys_ViewportInfo;
    vec2 sys_ScreenSize;
    vec2 sys_PixelRatio;
};

void main ( void )
{
	vec4 v_screenPos = vec4( mdl_Vertex, 1.0 );

	v2f_position	= vec4( mdl_Vertex, 1.0 );
    v2f_texcoord0	= mdl_TexCoord.xy;
    vec2 render_scale = sys_ViewportInfo.zw / sys_ScreenSize.xy;
    v2f_texcoord1   = (mdl_TexCoord.xy - vec2(0.5, 0.5) * render_scale) / render_scale;

	gl_Position = v_screenPos;
}
