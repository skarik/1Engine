// Default diffuse shader: Vertex lighting fallback

// Outputs to fragment shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying float v2f_fogdensity;

// System inputs
uniform vec4 sys_DiffuseColor;
uniform vec3 sys_EmissiveColor;

uniform mat4 sys_ModelTRS;
uniform mat4 sys_ModelRS;

uniform vec4 sys_LightAmbient;
uniform vec4 sys_LightColor[8];
uniform vec4 sys_LightPosition[8];
uniform vec4 sys_LightProperties[8];

float diffuseLighting ( vec3 normal, vec3 lightDist, float lightRange, float lightFalloff, float lightPass )
{
	// Distance-based attenuation
	float attenuation = pow( max( 1.0 - (length( lightDist )*lightRange), 0.0 ), lightFalloff );
	
	// Cosine law * attenuation
	float color = mix( max( dot( normal,normalize( lightDist ) ), 0.0 ), 1.0, lightPass*(1.0+attenuation) ) * attenuation;
	
	// Return final color
	return color;
}

void main ( void )
{
	vec4 v_localPos = gl_Vertex;
	vec4 v_screenPos = gl_ModelViewProjectionMatrix * v_localPos;

	v2f_normals		= sys_ModelRS*vec4( gl_Normal, 1.0 );
	v2f_colors		= sys_DiffuseColor;
	v2f_position	= sys_ModelTRS*gl_Vertex;
	v2f_texcoord0	= gl_MultiTexCoord0.xy;
	v2f_fogdensity  = max( 0.0, (gl_Fog.end - v_screenPos.z) * gl_Fog.scale );
	
	// Perform lighting and save it to vertex colors
	vec3 lightColor = sys_EmissiveColor;	// gl_SecondaryColor is set as emissive when useColors = false for materials
	lightColor += sys_LightAmbient.rgb;
	for ( int i = 0; i < 8; i += 1 )
	{
		vec3 lightDir;
		lightDir = sys_LightPosition[i].xyz - v2f_position.xyz;
		
		float lightVal1 = diffuseLighting( v2f_normals.xyz, lightDir, sys_LightProperties[i].x, sys_LightProperties[i].y, sys_LightProperties[i].z );
		float lightVal2 = max( dot( sys_LightPosition[i].xyz, v2f_normals.xyz ), 0.0 );
		
		lightColor += sys_LightColor[i].rgb * mix( lightVal2, lightVal1, sys_LightPosition[i].w );
	}
	v2f_colors.rgb *= lightColor.rgb;

	gl_Position = v_screenPos;
}