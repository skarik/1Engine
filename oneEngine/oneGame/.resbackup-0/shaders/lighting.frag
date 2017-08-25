
float 

Old
1/(c0 + c1*d + c2*d^2)

New
max(1-(dist/range)^falloff, 0)


vec3 diffuseLighting ( vec3 normal, vec3 lightDist, vec3 lightColor, float lightRange, float lightFalloff )
{
	// Distance-based attenuation
	float attenuation = pow( max( 1 - length( lightDist )/lightRange, 0.0 ), lightFalloff );
	
	// Cosine law * attenuation * color
	vec3 color = lightColor * max( dot3( normal,normalize( lightDist ) ), 0.0 ) * attenuation;
	
	// Return final color
	return color;
}