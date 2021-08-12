// sys/fullbright
// Renders object without lighting, only diffuse multiply blending.
#version 430

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_control_flow_attributes : require

#include "../common.glsli"
#include "../cbuffers.glsli"
#include "output_common_p.glsli"
#include "../random_hash.glsli"

// Inputs from vertex shader
layout(location = 0) in vec4 v2f_colors;
layout(location = 1) in vec2 v2f_texcoord0;
layout(location = 2) in vec3 v2f_normal;

// Samplers
layout(binding = 0, location = 20) uniform sampler2D textureAlbedo;
layout(binding = 2, location = 22) uniform sampler2D textureSurface;

// Tilebomber info
struct rrBombInfo
{
	vec2 uv_position;
	vec2 uv_size;
	ivec2 uv_divs;
	int min_repeat;
	int max_repeat;
	vec4 occurence;
};

layout(binding = CBUFFER_USER0, std430) uniform sys_cbuffer_ArbitraryTileBombInfo
{
	vec4	sys_TilebombBaseTexture;
	float	sys_TilebombChance;
	float	sys_TilebombUnused1;
	float	sys_TilebombScale;
	int		sys_TilebombBombCount;
	//vec4	sys_TilebombBombTexture[16];
	rrBombInfo sys_TilebombBombTexture[16];
};

void main ( void )
{
	vec2 uv_base = mod(v2f_texcoord0.xy, vec2(1.0, 1.0)) * sys_TilebombBaseTexture.zw * 0.999 + sys_TilebombBaseTexture.xy;
	vec2 uv_base_unmod = v2f_texcoord0.xy *  sys_TilebombBaseTexture.zw + sys_TilebombBaseTexture.xy;
	vec2 ddx_uv_base_unmod = dFdx(uv_base_unmod);
	vec2 ddy_uv_base_unmod = dFdy(uv_base_unmod);
	
	// Sample the base textures for the surface
	vec4 diffuseColor = textureGrad( textureAlbedo, uv_base, ddx_uv_base_unmod, ddy_uv_base_unmod );
	vec4 surfaceColor = textureGrad( textureSurface, uv_base, ddx_uv_base_unmod, ddy_uv_base_unmod );
	
	const vec2 uv_scaled = v2f_texcoord0.xy * sys_TilebombScale;
	const ivec2	cell = ivec2(floor(uv_scaled));
	const vec2	cell_position = uv_scaled - cell;
	for (int i_celX = -1; i_celX <= 0; ++i_celX)
	{
		for (int i_celY = -1; i_celY <= 0; ++i_celY)
		{
			const ivec2	l_cell = cell + ivec2(i_celX, i_celY);
			const vec2	l_cell_position = cell_position - vec2(i_celX, i_celY);
			
			// Grab four random values
			vec3 random_base = hashRand4( (l_cell * vec2(0.037, 0.119)).xyxy, 1.0 ).xyz;
			
			// Select a tilebomb
			const int bomb_index = int(floor(random_base.x * sys_TilebombBombCount * 0.9999));
			const rrBombInfo bomb = sys_TilebombBombTexture[bomb_index];
			
			// Select number of tilebombs
			const int bomb_number = (random_base.z < sys_TilebombChance) // Check here against the total chance.
				? int(round(random_base.y * (bomb.max_repeat - bomb.min_repeat))) + bomb.min_repeat
				: 0;
			
			// Bomb the texture!
			for (int i_bombInstance = 0; i_bombInstance < bomb_number; ++i_bombInstance)
			{
				vec4 random = hashRand4( (l_cell * vec2(0.037, 0.119)).xyxy, 1.0 + float(i_bombInstance) );
				
				// Calculate randomly offset tilebomb
				vec2 uv_random = l_cell_position - random.xy;
				
				// Mod the UVs based on the bomb sub-size
				uv_random = uv_random * bomb.uv_divs;
					
				// Choose a UV subindex
				float bomb_subindex_chooser = random.w * 0.9999;
				int bomb_subindex =
					(bomb_subindex_chooser < bomb.occurence.x) ? 0
					: ((bomb_subindex_chooser < bomb.occurence.x + bomb.occurence.y) ? 1
						: ((bomb_subindex_chooser < bomb.occurence.x + bomb.occurence.y + bomb.occurence.z) ? 2
							: ((bomb_subindex_chooser < bomb.occurence.x + bomb.occurence.y + bomb.occurence.z + bomb.occurence.w) ? 3
								: 0
								)
							)
						)
					;
				
				// Get actual UV position
				vec2 uv_random_subindex_offset = vec2(float(bomb_subindex % bomb.uv_divs.y), float(bomb_subindex / bomb.uv_divs.y));
				vec2 uv_random_rebased = ((uv_random + uv_random_subindex_offset) / bomb.uv_divs) * bomb.uv_size * 0.999 + bomb.uv_position;
				
				if (all(greaterThanEqual(uv_random, vec2(0.0, 0.0))) && all(lessThan(uv_random, vec2(1.0, 1.0))))
				{
					// Blend in albedo
					vec4 bomb_sample = textureGrad( textureAlbedo, uv_random_rebased, ddx_uv_base_unmod, ddy_uv_base_unmod);
					diffuseColor.rgb = mix(diffuseColor.rgb, bomb_sample.rgb, bomb_sample.a);
					diffuseColor.a = max(diffuseColor.a, bomb_sample.a);
					
					// Blend in surface properties
					vec4 surface_sample = textureGrad( textureSurface, uv_random_rebased, ddx_uv_base_unmod, ddy_uv_base_unmod);
					surfaceColor = mix(surfaceColor, surface_sample, bomb_sample.a);
				}
			}
		}
	}
	
	if (diffuseColor.a <= sys_AlphaCutoff) discard;

	rrSurfaceInfo result;
	
	result.albedo = diffuseColor * v2f_colors;
	result.normal = normalize(gl_FrontFacing ? -v2f_normal : v2f_normal); //gl_FrontFacing is flipped.
	result.emissive = vec3(0, 0, 0);

	result.metalness = surfaceColor.r * sys_ScaledMetallicness + sys_BaseMetallicness;
	result.smoothness = surfaceColor.g * sys_ScaledSmoothness + sys_BaseSmoothness;
	result.inverse_occlusion = 1.0;
	
	result.shade_model = sys_ShadingModel;
	result.is_frontface = !gl_FrontFacing;
	
	PushOutput(result);
}
