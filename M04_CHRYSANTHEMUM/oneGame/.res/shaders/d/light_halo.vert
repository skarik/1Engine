in vec3 mdl_Vertex;
in vec3 mdl_TexCoord;
in vec4 mdl_Color;
in vec3 mdl_Normal;

// Outputs to fragment shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec3 v2f_emissive;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying float v2f_fogdensity;
varying float v2f_depth;
varying float v2f_fogbrighten;

// System inputs
uniform mat4 sys_ModelMatrix;
uniform mat4 sys_ModelRotationMatrix;
uniform mat4 sys_ModelViewProjectionMatrix;

uniform vec4 sys_DiffuseColor;
uniform vec3 sys_EmissiveColor;

void main ( void )
{
	vec4 v_localPos = vec4( mdl_Vertex, 1.0 );
	vec4 v_screenPos = sys_ModelViewProjectionMatrix * v_localPos;
	
	v2f_normals		= sys_ModelRotationMatrix*vec4( mdl_Normal, 1.0 );
	v2f_colors		= sys_DiffuseColor;
	v2f_emissive	= sys_EmissiveColor;
	v2f_position	= sys_ModelMatrix*v_localPos;
	v2f_texcoord0	= mdl_TexCoord.xy;
	v2f_fogbrighten = min( max( 0.0, abs(gl_Fog.end*0.5 - v_screenPos.z) * gl_Fog.scale * 2.0 ), 1.0 ); // 0 at fullbright point
	v2f_fogdensity  = max( 0.0, (gl_Fog.end*2.5 - v_screenPos.z) * gl_Fog.scale );
	v2f_depth		= v_screenPos.w;

	v_screenPos.z -= 0.01; // bring it forward
	gl_Position = v_screenPos;
}