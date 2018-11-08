#ifndef RENDERER_SHADER_CONSTANT_BUFFER_TYPES_H_
#define RENDERER_SHADER_CONSTANT_BUFFER_TYPES_H_

#include "core/math/Math3d.h"
#include <stdint.h>

namespace renderer
{
	namespace shader
	{
		//	Location
		// Describes shader slot locations that are provided to the user for common shaders.
		// Unique shaders are welcome to define their own Location type.
		enum class Location : uint32_t
		{
			kIgnore = 0xFFFFFFFF,
			kInvalid = 0xFFFFFFFD,
			kPosition = 0,
			kUV0 = 1,
			kColor = 2,
			kNormal = 3,
			kTangent = 4,
			kBinormal = 5,
			kUV1 = 6,
			kBoneWeight = 7,
			kBoneIndices = 8,
		};

		//	VBufferSlot
		// Typed vertex buffer slot description. May be removed later.
		// Unique shaders are welcome to define their own Location type.
		enum VBufferSlot : uint32_t
		{
			kVBufferSlotPosition = (uint32_t)Location::kPosition,
			kVBufferSlotUV0 = (uint32_t)Location::kUV0,
			kVBufferSlotColor = (uint32_t)Location::kColor,
			kVBufferSlotNormal = (uint32_t)Location::kNormal,
			kVBufferSlotTangent = (uint32_t)Location::kTangent,
			kVBufferSlotBinormal = (uint32_t)Location::kBinormal,
			kVBufferSlotUV1 = (uint32_t)Location::kUV1,
			kVBufferSlotBoneWeight = (uint32_t)Location::kBoneWeight,
			kVBufferSlotBoneIndices = (uint32_t)Location::kBoneIndices,
		};

		//	rrBinding
		// Describes slot to bind resource to. Is not actually needed.
		enum rrBinding : uint32_t
		{
			kBinding0 = 0,
			kBinding1 = 1,
			kBinding2 = 2,
			kBinding3 = 3,
			kBinding4 = 4,
			kBinding5 = 5,
			kBinding6 = 6,
			kBinding7 = 7,
			kBinding8 = 8,
			kBinding9 = 9,
			kBinding10 = 10,
			kBinding11 = 11,
			kBinding12 = 12,
			kBinding13 = 13,
			kBinding14 = 14,
			kBinding15 = 15,
			kBinding16 = 16,
			kBinding17 = 17,
			kBinding18 = 18,
			kBinding19 = 19,
		};
	};
}

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

		kAttributeMaxCount	= 11,
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

	enum rrCBufferId
	{
		CBUFFER_PER_OBJECT_MATRICES		= 0,
		CBUFFER_PER_OBJECT_EXTENDED		= 1,
		CBUFFER_PER_CAMERA_INFORMATION	= 2,
		CBUFFER_PER_FRAME_INFORMATION	= 3,
		CBUFFER_PER_PASS_INFORMATION	= 4,
	};
	struct rrCBufferReservedName
	{
		rrCBufferId id;
		const char* token;
	};
	static struct rrCBufferReservedName CBufferNames[] =
	{
		{CBUFFER_PER_OBJECT_MATRICES, "sys_cbuffer_PerObject"},
		{CBUFFER_PER_OBJECT_EXTENDED, "sys_cbuffer_PerObjectExt"},
		{CBUFFER_PER_CAMERA_INFORMATION, "sys_cbuffer_PerCamera"},
		{CBUFFER_PER_FRAME_INFORMATION, "sys_cbuffer_PerFrame"},
		{CBUFFER_PER_PASS_INFORMATION, "sys_cbuffer_PerPass"},
	};

	namespace cbuffer
	{
		/*LAYOUT_PACK_TIGHTLY
		struct rrPerObjectPassSurface
		{
		};
		LAYOUT_PACK_END*/
		//static_assert(sizeof(rrPerObjectPassSurface) == 256, "Alignment of rrPerObjectPassSurface incorrect for the GPU.");

		// store per-model on the gpu
		LAYOUT_PACK_TIGHTLY
		struct rrPerObjectMatrices
		{
			Matrix4x4	modelTRS;
			Matrix4x4	modelRS;
			Matrix4x4	modelViewProjection;
			Matrix4x4	modelViewProjectionInverse;
		};
		LAYOUT_PACK_END
		static_assert(sizeof(rrPerObjectMatrices) == 256, "Alignment of rrPerObjectSurface incorrect for the GPU.");

		LAYOUT_PACK_TIGHTLY
		struct rrPerObjectSurface
		{
			Vector4f	diffuseColor;
			Vector4f	specularColor;
			Vector3f	emissiveColor;
			float		alphaCutoff;
			float		lighting0;
			float		lighting1RimStrength;
			float		lighting2;
			float		lighting3Alpha;
			Vector4f	textureScale;
			Vector4f	textureOffset;

			Vector4f	rr_padding [10];
		};
		LAYOUT_PACK_END
		static_assert(sizeof(rrPerObjectSurface) == 256, "Alignment of rrPerObjectSurface incorrect for the GPU.");

		struct rrPerObjectSamplers
		{
			uint		textureSampler0;
			uint		textureSampler1;
			uint		textureSampler2;
			uint		textureSampler3;
			uint		textureSampler4;
			uint		textureSampler5;
			uint		textureSampler6;
			uint		textureSampler7;
		};
		static_assert(sizeof(rrPerObjectSamplers) == 32, "Alignment of rrPerObjectSamplers incorrect for the GPU.");

		struct rrPerCamera
		{
			Matrix4x4	viewProjection; // todo: add inverse
			Vector4f	worldCameraPosition;
			Vector4f	viewportInfo;
			Vector2f	screenSize;
			Vector2f	pixelRatio;

			Vector4f	rr_padding [1];
		};
		static_assert(sizeof(rrPerCamera) == 128, "Alignment of rrPerCamera incorrect for the GPU.");

		struct rrPerFrame
		{
			Vector4f	time;
			Vector4f	sinTime;
			Vector4f	cosTime;

			Vector4f	fogColor;
			Vector4f	atmoColor;
			Real32		fogEnd;
			Real32		fogScale;

			Vector2f	rr_padding [5];
		};
		static_assert(sizeof(rrPerFrame) == 128, "Alignment of rrPerFrame incorrect for the GPU.");

		struct rrPerPassLightingInfo
		{
			Vector4f	ambientLightFallback;
			int32_t		lightNumber;
			int32_t : 32;
			int32_t : 32;
			int32_t : 32;
		};
		static_assert(sizeof(rrPerPassLightingInfo) == 32, "Alignment of rrPerPassLightingInfo incorrect for the GPU.");
	}
}

#endif RENDERER_SHADER_CONSTANT_BUFFER_TYPES_H_