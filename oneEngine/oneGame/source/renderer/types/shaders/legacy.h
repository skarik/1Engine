#ifndef RENDERER_SHADER_LEGACY_TYPES_H_
#define RENDERER_SHADER_LEGACY_TYPES_H_

#include "core/math/Math3d.h"
#include <stdint.h>

namespace renderer
{
	enum rrShaderTag : uint8_t
	{
		SHADER_TAG_DEFAULT = 0,
		SHADER_TAG_SKINNING
	};
	enum rrAttributeId : uint8_t
	{
		ATTRIB_VERTEX		= 0,
		ATTRIB_TEXCOORD0	= 1,
		ATTRIB_COLOR		= 2,
		ATTRIB_NORMAL		= 3,
		ATTRIB_TANGENTS		= 4,
		ATTRIB_BINORMALS	= 5,
		ATTRIB_BONEWEIGHTS	= 6,
		ATTRIB_BONEINDICES	= 7,
		ATTRIB_TEXCOORD2	= 8,
		ATTRIB_TEXCOORD3	= 9,
		ATTRIB_TEXCOORD4	= 10,

		ATTRIB_MAX_COUNT	= 11,
	};

	struct rrAttributeReservedName
	{
		rrAttributeId id;
		const char* token;
	};
	static struct rrAttributeReservedName AttributeNames[] =
	{
		{ ATTRIB_VERTEX,	"mdl_Vertex" },
		{ ATTRIB_TEXCOORD0,	"mdl_TexCoord" },
		{ ATTRIB_COLOR,		"mdl_Color" },
		{ ATTRIB_NORMAL,	"mdl_Normal" },
		{ ATTRIB_TANGENTS,	"mdl_Tangents" },
		{ ATTRIB_BINORMALS,	"mdl_Binormals" },
		{ ATTRIB_BONEWEIGHTS, "mdl_BoneWeights" },
		{ ATTRIB_BONEINDICES, "mdl_BoneIndices" },
		{ ATTRIB_TEXCOORD2, "mdl_TexCoord2" },
		{ ATTRIB_TEXCOORD3, "mdl_TexCoord3" },
		{ ATTRIB_TEXCOORD4, "mdl_TexCoord4" },
	};

	enum rrUniformId : uint8_t
	{
		UNI_SURFACE_DIFFUSE_COLOR	= 0,
		UNI_SURFACE_EMISSIVE_COLOR	= 1,
		UNI_SURFACE_SPECULAR_COLOR	= 2,
		UNI_SURFACE_ALPHA_CUTOFF	= 3,
		UNI_SURFACE_LIGHTING_OVERRIDES	= 4,

		UNI_TEXTURE_SCALE			= 5,
		UNI_TEXTURE_OFFSET			= 6,

		UNI_GAME_WIND_DIRECTION		= 7,
		UNI_GAME_FADE_VALUE			= 8,
		UNI_GAME_HALF_SCALE			= 9,

		UNI_LIGHTING_AMBIENT		= 10,
		UNI_LIGHTING_COUNT			= 11,

		UNI_LIGHTING_PARAM_HACK		= 12,

		UNI_SAMPLER_0				= 20,
		UNI_SAMPLER_1				= 21,
		UNI_SAMPLER_2				= 22,
		UNI_SAMPLER_3				= 23,
		UNI_SAMPLER_4				= 24,
		UNI_SAMPLER_5				= 25,
		UNI_SAMPLER_6				= 26,
		UNI_SAMPLER_7				= 27,
		UNI_SAMPLER_8				= 28,
		UNI_SAMPLER_9				= 29,
		UNI_SAMPLER_10				= 30,
		UNI_SAMPLER_11				= 31,
		UNI_SAMPLER_SHADOW_0		= 32,
		UNI_SAMPLER_SHADOW_1		= 33,
		UNI_SAMPLER_SHADOW_2		= 34,
		UNI_SAMPLER_SHADOW_3		= 35,
		UNI_SAMPLER_SHADOW_4		= 36,
		UNI_SAMPLER_SHADOW_5		= 37,
		UNI_SAMPLER_SHADOW_6		= 38,
		UNI_SAMPLER_SHADOW_7		= 39,
		UNI_SAMPLER_SHADOW_8		= 40,
		UNI_SAMPLER_SHADOW_9		= 41,
		UNI_SAMPLER_SHADOW_10		= 42,
		UNI_SAMPLER_SHADOW_11		= 43,
		UNI_SAMPLER_LIGHT_BUFFER_0	= 44,
		UNI_SAMPLER_LIGHT_BUFFER_1	= 45,
		UNI_SAMPLER_INSTANCE_BUFFER_0	= 46,
		UNI_SAMPLER_INSTANCE_BUFFER_1	= 47,
		UNI_SAMPLER_REFLECTION_0	= 47,
		UNI_SAMPLER_REFLECTION_1	= 48,
		UNI_SAMPLER_REFLECTION_2	= 49,
		UNI_SAMPLER_REFLECTION_3	= 50,
	};
	struct rrUniformReservedName
	{
		rrUniformId id;
		const char* token;
	};
	static struct rrUniformReservedName UniformNames[] =
	{
		{ UNI_SURFACE_DIFFUSE_COLOR,	"sys_DiffuseColor" },
		{ UNI_SURFACE_EMISSIVE_COLOR,	"sys_EmissiveColor" },
		{ UNI_SURFACE_SPECULAR_COLOR,	"sys_SpecularColor" },
		{ UNI_SURFACE_ALPHA_CUTOFF,		"sys_AlphaCutoff" },
		{ UNI_SURFACE_LIGHTING_OVERRIDES,	"sys_LightingOverrides" },

		{ UNI_TEXTURE_SCALE,	"sys_TextureScale" },
		{ UNI_TEXTURE_OFFSET,	"sys_TextureOffset" },

		{ UNI_GAME_WIND_DIRECTION,	"gm_WindDirection" },
		{ UNI_GAME_FADE_VALUE,	"gm_FadeValue" },
		{ UNI_GAME_HALF_SCALE,	"gm_HalfScale" },

		{ UNI_LIGHTING_AMBIENT,	"sys_LightAmbient" },
		{ UNI_LIGHTING_COUNT,	"sys_LightNumber" },

		{ UNI_SAMPLER_0,		"textureSampler0" },
		{ UNI_SAMPLER_SHADOW_0,	"textureShadow0" },
		{ UNI_SAMPLER_LIGHT_BUFFER_0,	"textureLightBuffer" },
		{ UNI_SAMPLER_INSTANCE_BUFFER_0,"textureInstanceBuffer" },
		{ UNI_SAMPLER_REFLECTION_0,		"textureReflection0" },
	};
}

#endif RENDERER_SHADER_LEGACY_TYPES_H_