#ifndef RENDERER_SHADER_CONSTANT_BUFFER_TYPES_H_
#define RENDERER_SHADER_CONSTANT_BUFFER_TYPES_H_

#include <stdint.h>
#include "core/math/Math3d.h"
#include "core/math/vect2d_template.h"
#include "core/math/vect3d_template.h"
#include "core/math/vect4d_template.h"
#include "renderer/types/Shading.h"

namespace renderer
{
	enum rrCBufferId
	{
		CBUFFER_PER_OBJECT_MATRICES		= 0,
		CBUFFER_PER_OBJECT_EXTENDED		= 1,
		CBUFFER_PER_CAMERA_INFORMATION	= 2,
		CBUFFER_PER_FRAME_INFORMATION	= 3,
		CBUFFER_PER_PASS_INFORMATION	= 4,
		// 5 & 6 reserved for future pass information
		CBUFFER_USER0	= 7,
		CBUFFER_USER1	= 8,
		CBUFFER_USER2	= 9,
		CBUFFER_USER3	= 10,
	};

	namespace cbuffer
	{
		//===============================================================================================//
		// Common Cbuffers
		//===============================================================================================//

		// store per-model on the gpu
		LAYOUT_PACK_TIGHTLY
		struct rrPerObjectMatrices
		{
			Matrix4x4	modelTRS;
			Matrix4x4	modelR;
			Matrix4x4	modelViewProjection;
			Matrix4x4	modelViewProjectionInverse;
		};
		LAYOUT_PACK_END
		static_assert(sizeof(rrPerObjectMatrices) == 256, "Alignment of rrPerObjectSurface incorrect for the GPU.");

		LAYOUT_PACK_TIGHTLY
		struct rrPerObjectSurface
		{
			Vector4f	diffuseColor = Vector4f(1, 1, 1, 1);
			float		baseSmoothness = 0.0F;
			float		scaledSmoothness = 1.0F;
			float		baseMetallicness = 0.0F;
			float		scaledMetallicness = 1.0F;
			Vector3f	emissiveColor = Vector4f(1, 1, 1, 1);
			float		alphaCutoff = 0.0F;
			float		lighting0;
			float		lighting1;
			float		lighting2;
			rrShadingModel
						shadingModel = kShadingModelNormal;
			Vector4f	textureScale = Vector4f(1, 1, 1, 1);
			Vector4f	textureOffset = Vector4f(0, 0, 0, 0);

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
			Matrix4x4	viewProjection;
			Matrix4x4	viewProjectionInverse;
			Matrix4x4	view;
			Matrix4x4	viewInverse;
			Matrix4x4	projection;
			Matrix4x4	projectionInverse;
			Vector4f	worldCameraPosition;
			Vector2f	unused;
			Vector2f	screenSizeScaled;
			Vector2f	screenSize;
			Vector2f	pixelRatio;
			float		nearPlane;
			float		farPlane;
			Vector2f	rr_padding [1];
		};
		//static_assert(sizeof(rrPerCamera) == 192, "Alignment of rrPerCamera incorrect for the GPU.");

		struct rrPerFrame
		{
			// Vector of time, containing ``(t/20, t, t*2, t*3)``
			Vector4f	time;
			// Vector of sin(time), containing ``sin(t/8, t/4, t/2, t)``
			Vector4f	sinTime;
			// Vector of cos(time), containing ``cos(t/8, t/4, t/2, t)``
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

		//===============================================================================================//
		// Specific effect CBuffers:
		//===============================================================================================//

		namespace tilebomber
		{
			LAYOUT_PACK_TIGHTLY
			struct rrTilebombParams
			{
				Vector2f base_texture_uv_position;
				Vector2f base_texture_uv_size;

				float bomb_chance = 0.9F;
				float unused1;
				float bomb_scale = 1.0F;
				int32 bomb_count = 1;

				struct rrBombInfo
				{
					Vector2f uv_position;
					Vector2f uv_size;
					Vector2i uv_divs = Vector2i(2, 2);
					int min_repeat = 1;
					int max_repeat = 1;
					Vector4f occurrence = Vector4f(0.25F, 0.25F, 0.25F, 0.25F);
				};
				rrBombInfo bomb_texture [16];
			};
			LAYOUT_PACK_END
		}
	}
}

#endif RENDERER_SHADER_CONSTANT_BUFFER_TYPES_H_