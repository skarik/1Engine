#include "physical/physics/PrPhysics.h"
#include "physical/types/layers.h"

// Initializes the physics engine system.
void PrPhysics::Initialize ( void )
{
	Initialize_CollisionMasks();
	Initialize_Engine();
}

// Loads up default filters
void PrPhysics::Initialize_CollisionMasks ( void )
{
	physical::prLayerMask* collisionMasks = new physical::prLayerMask [physical::layer::MAX_PHYS_LAYER];

	// Start off with entirely disabled collisions
	for ( int i = 0; i < physical::layer::MAX_PHYS_LAYER; ++i )
	{
		collisionMasks[i] = 0;
	}

	// Use a small interface to make it identical to the Havok code (makes it easy to sync)
	// The bitmasks will be symmetrical when mapped out on a graph. There can be no one-way collisions.
	auto enableCollisionsBetween = [&collisionMasks]( physical::prLayer layerA, physical::prLayer layerB )
	{	// Enable the corresponding bits to enable collision
		collisionMasks[layerA] |= ( 1 << layerB );
		collisionMasks[layerB] |= ( 1 << layerA );
	};
	auto disableCollisionsBetween = [&collisionMasks]( physical::prLayer layerA, physical::prLayer layerB )
	{	// Disable the corresponding bits to disable collision
		collisionMasks[layerA] &= ~( 1 << layerB );
		collisionMasks[layerB] &= ~( 1 << layerA );
	};

	// Enable Landscape collisions with all groups
	enableCollisionsBetween( physical::layer::PHYS_LANDSCAPE, physical::layer::PHYS_DEBRIS );
	enableCollisionsBetween( physical::layer::PHYS_LANDSCAPE, physical::layer::PHYS_RAGDOLL );
	enableCollisionsBetween( physical::layer::PHYS_LANDSCAPE, physical::layer::PHYS_RAGDOLL_ESSENTIAL );
	enableCollisionsBetween( physical::layer::PHYS_LANDSCAPE, physical::layer::PHYS_CHARACTER );
	enableCollisionsBetween( physical::layer::PHYS_LANDSCAPE, physical::layer::PHYS_ITEM );
	enableCollisionsBetween( physical::layer::PHYS_LANDSCAPE, physical::layer::PHYS_DYNAMIC );
	enableCollisionsBetween( physical::layer::PHYS_LANDSCAPE, physical::layer::PHYS_SWEPTCOLLISION );
	disableCollisionsBetween( physical::layer::PHYS_LANDSCAPE, physical::layer::PHYS_LANDSCAPE );

	// Only have PHYS_WORLD_TRACE hit Landscape and Dynamic
	enableCollisionsBetween( physical::layer::PHYS_WORLD_TRACE, physical::layer::PHYS_LANDSCAPE );
	enableCollisionsBetween( physical::layer::PHYS_WORLD_TRACE, physical::layer::PHYS_DYNAMIC );
	enableCollisionsBetween( physical::layer::PHYS_WORLD_TRACE, physical::layer::PHYS_RAGDOLL_ESSENTIAL );
	disableCollisionsBetween( physical::layer::PHYS_WORLD_TRACE, physical::layer::PHYS_RAGDOLL );
	disableCollisionsBetween( physical::layer::PHYS_WORLD_TRACE, physical::layer::PHYS_ITEM );
	disableCollisionsBetween( physical::layer::PHYS_WORLD_TRACE, physical::layer::PHYS_CHARACTER );
	disableCollisionsBetween( physical::layer::PHYS_WORLD_TRACE, physical::layer::PHYS_SWEPTCOLLISION );
	disableCollisionsBetween( physical::layer::PHYS_WORLD_TRACE, physical::layer::PHYS_DEBRIS );
	disableCollisionsBetween( physical::layer::PHYS_WORLD_TRACE, physical::layer::PHYS_WORLD_TRACE );
	disableCollisionsBetween( physical::layer::PHYS_WORLD_TRACE, physical::layer::PHYS_HITBOX );

	// Disable debris collisions with all other groups except world
	disableCollisionsBetween( physical::layer::PHYS_DEBRIS, physical::layer::PHYS_RAGDOLL );
	disableCollisionsBetween( physical::layer::PHYS_DEBRIS, physical::layer::PHYS_RAGDOLL_ESSENTIAL );
	disableCollisionsBetween( physical::layer::PHYS_DEBRIS, physical::layer::PHYS_CHARACTER );
	disableCollisionsBetween( physical::layer::PHYS_DEBRIS, physical::layer::PHYS_SWEPTCOLLISION );
	disableCollisionsBetween( physical::layer::PHYS_DEBRIS, physical::layer::PHYS_ITEM );
	disableCollisionsBetween( physical::layer::PHYS_DEBRIS, physical::layer::PHYS_DYNAMIC );
	disableCollisionsBetween( physical::layer::PHYS_DEBRIS, physical::layer::PHYS_DEBRIS );

	// Dynamic Props collide with game essential props, being dynamic, character, item, and essential ragdoll
	enableCollisionsBetween( physical::layer::PHYS_DYNAMIC, physical::layer::PHYS_DYNAMIC );
	enableCollisionsBetween( physical::layer::PHYS_DYNAMIC, physical::layer::PHYS_CHARACTER );
	enableCollisionsBetween( physical::layer::PHYS_DYNAMIC, physical::layer::PHYS_SWEPTCOLLISION );
	enableCollisionsBetween( physical::layer::PHYS_DYNAMIC, physical::layer::PHYS_ITEM );
	enableCollisionsBetween( physical::layer::PHYS_DYNAMIC, physical::layer::PHYS_RAGDOLL_ESSENTIAL );
	disableCollisionsBetween( physical::layer::PHYS_DYNAMIC, physical::layer::PHYS_RAGDOLL );
	disableCollisionsBetween( physical::layer::PHYS_DYNAMIC, physical::layer::PHYS_DEBRIS );

	// Disable ragdoll collisions with all other groups except ragdolls
	disableCollisionsBetween( physical::layer::PHYS_RAGDOLL, physical::layer::PHYS_CHARACTER );
	disableCollisionsBetween( physical::layer::PHYS_RAGDOLL, physical::layer::PHYS_SWEPTCOLLISION );
	disableCollisionsBetween( physical::layer::PHYS_RAGDOLL, physical::layer::PHYS_ITEM );
	disableCollisionsBetween( physical::layer::PHYS_RAGDOLL, physical::layer::PHYS_DYNAMIC );
	enableCollisionsBetween( physical::layer::PHYS_RAGDOLL, physical::layer::PHYS_RAGDOLL_ESSENTIAL );
	enableCollisionsBetween( physical::layer::PHYS_RAGDOLL, physical::layer::PHYS_RAGDOLL );

	// Essential ragdolls act like dynamic props, with the difference that they collide with debris ragdolls
	enableCollisionsBetween( physical::layer::PHYS_RAGDOLL_ESSENTIAL, physical::layer::PHYS_DYNAMIC );
	enableCollisionsBetween( physical::layer::PHYS_RAGDOLL_ESSENTIAL, physical::layer::PHYS_CHARACTER );
	enableCollisionsBetween( physical::layer::PHYS_RAGDOLL_ESSENTIAL, physical::layer::PHYS_SWEPTCOLLISION );
	enableCollisionsBetween( physical::layer::PHYS_RAGDOLL_ESSENTIAL, physical::layer::PHYS_ITEM );
	enableCollisionsBetween( physical::layer::PHYS_RAGDOLL_ESSENTIAL, physical::layer::PHYS_RAGDOLL_ESSENTIAL );
	enableCollisionsBetween( physical::layer::PHYS_RAGDOLL_ESSENTIAL, physical::layer::PHYS_RAGDOLL );
	disableCollisionsBetween( physical::layer::PHYS_RAGDOLL_ESSENTIAL, physical::layer::PHYS_DEBRIS );

	// Items collide with other items, dynamic props, and essential ragdolls only
	disableCollisionsBetween( physical::layer::PHYS_ITEM, physical::layer::PHYS_RAGDOLL );
	enableCollisionsBetween( physical::layer::PHYS_ITEM, physical::layer::PHYS_RAGDOLL_ESSENTIAL );
	disableCollisionsBetween( physical::layer::PHYS_ITEM, physical::layer::PHYS_CHARACTER );
	disableCollisionsBetween( physical::layer::PHYS_ITEM, physical::layer::PHYS_SWEPTCOLLISION );
	disableCollisionsBetween( physical::layer::PHYS_ITEM, physical::layer::PHYS_ITEM );	// CORRECTION: ITEMS DON'T COLLIDE WITH ITEMS
	enableCollisionsBetween( physical::layer::PHYS_ITEM, physical::layer::PHYS_DYNAMIC );
	disableCollisionsBetween( physical::layer::PHYS_ITEM, physical::layer::PHYS_DEBRIS );

	// Weapons collide with only hitboxes and other weapons
	/*disableCollisionsBetween( physical::layer::PHYS_WEAPON, physical::layer::PHYS_RAGDOLL );
	enableCollisionsBetween( physical::layer::PHYS_WEAPON, physical::layer::PHYS_RAGDOLL_ESSENTIAL );
	disableCollisionsBetween( physical::layer::PHYS_WEAPON, physical::layer::PHYS_CHARACTER );
	disableCollisionsBetween( physical::layer::PHYS_WEAPON, physical::layer::PHYS_SWEPTCOLLISION );
	disableCollisionsBetween( physical::layer::PHYS_WEAPON, physical::layer::PHYS_ITEM );	// CORRECTION: ITEMS DON'T COLLIDE WITH ITEMS
	enableCollisionsBetween( physical::layer::PHYS_WEAPON, physical::layer::PHYS_DYNAMIC );
	disableCollisionsBetween( physical::layer::PHYS_WEAPON, physical::layer::PHYS_DEBRIS );*/
	enableCollisionsBetween( physical::layer::PHYS_WEAPON, physical::layer::PHYS_HITBOX );
	enableCollisionsBetween( physical::layer::PHYS_WEAPON, physical::layer::PHYS_WEAPON );

	// Characters collide with dynamic props, other characters, essential ragdolls, and the world only
	disableCollisionsBetween( physical::layer::PHYS_CHARACTER, physical::layer::PHYS_DEBRIS );
	disableCollisionsBetween( physical::layer::PHYS_CHARACTER, physical::layer::PHYS_RAGDOLL );
	enableCollisionsBetween( physical::layer::PHYS_CHARACTER, physical::layer::PHYS_RAGDOLL_ESSENTIAL );
	enableCollisionsBetween( physical::layer::PHYS_CHARACTER, physical::layer::PHYS_CHARACTER );
	enableCollisionsBetween( physical::layer::PHYS_CHARACTER, physical::layer::PHYS_SWEPTCOLLISION );
	disableCollisionsBetween( physical::layer::PHYS_CHARACTER, physical::layer::PHYS_ITEM );
	disableCollisionsBetween( physical::layer::PHYS_CHARACTER, physical::layer::PHYS_HITBOX );
	enableCollisionsBetween( physical::layer::PHYS_CHARACTER, physical::layer::PHYS_DYNAMIC );
	enableCollisionsBetween( physical::layer::PHYS_CHARACTER, physical::layer::PHYS_LANDSCAPE );

	// And bullets collide with everything
	enableCollisionsBetween( physical::layer::PHYS_BULLET_TRACE, physical::layer::PHYS_DEBRIS );
	enableCollisionsBetween( physical::layer::PHYS_BULLET_TRACE, physical::layer::PHYS_RAGDOLL );
	enableCollisionsBetween( physical::layer::PHYS_BULLET_TRACE, physical::layer::PHYS_RAGDOLL_ESSENTIAL );
	enableCollisionsBetween( physical::layer::PHYS_BULLET_TRACE, physical::layer::PHYS_CHARACTER );
	disableCollisionsBetween( physical::layer::PHYS_BULLET_TRACE, physical::layer::PHYS_SWEPTCOLLISION );
	enableCollisionsBetween( physical::layer::PHYS_BULLET_TRACE, physical::layer::PHYS_ITEM );
	enableCollisionsBetween( physical::layer::PHYS_BULLET_TRACE, physical::layer::PHYS_DYNAMIC );
	enableCollisionsBetween( physical::layer::PHYS_BULLET_TRACE, physical::layer::PHYS_LANDSCAPE );
	enableCollisionsBetween( physical::layer::PHYS_BULLET_TRACE, physical::layer::PHYS_HITBOX );

	// Hitboxes collide with whatever
	enableCollisionsBetween( physical::layer::PHYS_HITBOX, physical::layer::PHYS_LANDSCAPE );
	enableCollisionsBetween( physical::layer::PHYS_HITBOX, physical::layer::PHYS_BULLET_TRACE );
	disableCollisionsBetween( physical::layer::PHYS_HITBOX, physical::layer::PHYS_WORLD_TRACE );
	disableCollisionsBetween( physical::layer::PHYS_HITBOX, physical::layer::PHYS_DEBRIS );
	disableCollisionsBetween( physical::layer::PHYS_HITBOX, physical::layer::PHYS_RAGDOLL );
	disableCollisionsBetween( physical::layer::PHYS_HITBOX, physical::layer::PHYS_RAGDOLL_ESSENTIAL );
	disableCollisionsBetween( physical::layer::PHYS_HITBOX, physical::layer::PHYS_CHARACTER );
	disableCollisionsBetween( physical::layer::PHYS_HITBOX, physical::layer::PHYS_SWEPTCOLLISION );
	disableCollisionsBetween( physical::layer::PHYS_HITBOX, physical::layer::PHYS_ITEM );
	disableCollisionsBetween( physical::layer::PHYS_HITBOX, physical::layer::PHYS_DYNAMIC );
	disableCollisionsBetween( physical::layer::PHYS_HITBOX, physical::layer::PHYS_HITBOX );

	// Save filters
	m_collisionMasks = collisionMasks;
}
