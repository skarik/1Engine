
#ifndef _GAME_LAYERS_H_
#define _GAME_LAYERS_H_

namespace Layers
{
	enum LayerTypes
	{
		None		= 0,
		WeaponItem	= 1,
		Actor		= 2,
		Character	= 4,
		World		= 8,
		Debris		= 16,
		Rigidbody	= 32,
		Hitboxes	= 64,
		Points		= 128
	};

	typedef unsigned int Layer;

	enum PhysicsTypes
	{
		PHYS_LANDSCAPE	= 0,
		PHYS_DEBRIS,
		PHYS_RAGDOLL,
		PHYS_RAGDOLL_ESSENTIAL,
		PHYS_CHARACTER,
		PHYS_ITEM,
		PHYS_WEAPON,
		PHYS_DYNAMIC,
		PHYS_BULLET_TRACE,
		PHYS_WORLD_TRACE,
		PHYS_HITBOX,
		PHYS_SWEPTCOLLISION,
		MAX_PHYS_LAYER
	};
};

#endif