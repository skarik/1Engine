// Default diffuse shader: Vertex lighting fallback

// Inputs from vertex shader
varying vec4 v2f_normals;
varying vec4 v2f_colors;
varying vec4 v2f_position;
varying vec2 v2f_texcoord0;
varying float v2f_fogdensity;

// Samplers
uniform sampler2D textureDiffuse;

void main ( void )  
{
	vec4 diffuseColor = texture2D( textureDiffuse, v2f_texcoord0 );
	gl_FragColor = mix( gl_Fog.color, diffuseColor * v2f_colors * vec4( lightColor, 1.0 ), v2f_fogdensity );
	gl_FragColor.a = diffuseColor.a * v2f_colors.a;
}