#ifndef RENDERER_SHADER_VERTEX_ATTRIBUTE_TYPES_H_
#define RENDERER_SHADER_VERTEX_ATTRIBUTE_TYPES_H_

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
			kUV2 = 9,
			kUV3 = 10,
			kMAX_POSITION,
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
			kVBufferSlotUV2 = (uint32_t)Location::kUV2,
			kVBufferSlotUV3 = (uint32_t)Location::kUV3,
			// Max engine-defined slot position.
			kVBufferSlotMaxPosition,
			// Max recommended vertex attributes.
			kVBufferSlotMaxCount = 16,
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

#endif RENDERER_SHADER_VERTEX_ATTRIBUTE_TYPES_H_