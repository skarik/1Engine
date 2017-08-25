#ifndef PHYSICAL_LAYERS_H_
#define PHYSICAL_LAYERS_H_

#include "core/types/types.h"

namespace physical
{
	typedef uint16_t prLayer;
	typedef uint16_t prLayerMask;

	namespace layer
	{
		// prLayerTypes : bit offset layer types
		enum prLayerTypes : uint8_t
		{
			None		= 0xFF,
			WeaponItem	= 0x00,
			Actor		= 0x01,
			Character	= 0x02,
			World		= 0x03,
			Debris		= 0x04,
			Rigidbody	= 0x05,
			Hitboxes	= 0x06,
			Points		= 0x07
		};

		// prLayerMasks : bitmask layer types
		enum prLayerMasks : uint16_t
		{
			MaskNone		= 0x00,
			MaskWeaponItem	= 0x01,
			MaskActor		= 0x02,
			MaskCharacter	= 0x04,
			MaskWorld		= 0x08,
			MaskDebris		= 0x10,
			MaskRigidbody	= 0x20,
			MaskHitboxes	= 0x40,
			MaskPoints		= 0x80
		};

		// prPhysicsTypes : bit offset layer types
		enum prPhysicsTypes : uint8_t
		{
			PHYS_LANDSCAPE			= 0x00,
			PHYS_DEBRIS				= 0x01,
			PHYS_RAGDOLL			= 0x02,
			PHYS_RAGDOLL_ESSENTIAL	= 0x03,
			PHYS_CHARACTER			= 0x04,
			PHYS_ITEM				= 0x05,
			PHYS_WEAPON				= 0x06,
			PHYS_DYNAMIC			= 0x07,
			PHYS_BULLET_TRACE		= 0x08,
			PHYS_WORLD_TRACE		= 0x09,
			PHYS_HITBOX				= 0x0A,
			PHYS_SWEPTCOLLISION		= 0x0B,

			MAX_PHYS_LAYER
		};

		// prPhysicsTypes : bitmask layer types
		enum prPhysicsMasks : uint16_t
		{
			MASK_LANDSCAPE			= 0x001,
			MASK_DEBRIS				= 0x002,
			MASK_RAGDOLL			= 0x004,
			MASK_RAGDOLL_ESSENTIAL	= 0x008,
			MASK_CHARACTER			= 0x010,
			MASK_ITEM				= 0x020,
			MASK_WEAPON				= 0x040,
			MASK_DYNAMIC			= 0x080,
			MASK_BULLET_TRACE		= 0x100,
			MASK_WORLD_TRACE		= 0x200,
			MASK_HITBOX				= 0x300,
			MASK_SWEPTCOLLISION		= 0x400,
		};
	}
};

#endif//PHYSICAL_LAYERS_H_