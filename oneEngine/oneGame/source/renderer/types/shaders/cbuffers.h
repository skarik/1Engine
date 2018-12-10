#ifndef RENDERER_SHADER_CONSTANT_BUFFER_TYPES_H_
#define RENDERER_SHADER_CONSTANT_BUFFER_TYPES_H_

#include "core/math/Math3d.h"
#include <stdint.h>

namespace renderer
{
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
			Vector4f	lightParamHack;
			int32_t : 32;
			int32_t : 32;
			int32_t : 32;
			int32_t : 32;
		};
		static_assert(sizeof(rrPerPassLightingInfo) == 64, "Alignment of rrPerPassLightingInfo incorrect for the GPU.");
	}
}

#endif RENDERER_SHADER_CONSTANT_BUFFER_TYPES_H_