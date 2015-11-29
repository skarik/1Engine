
// Include class definition
#include "CPhysics.h"
#include "physical/system/Layers.h"

// == Static Variable Delarations ==
//int*	CPhysics::systemGroups = NULL;

//===Function Definitions===
void CPhysics::InitLayers ( void )
{
	collisionMasks = new uint16_t [Layers::MAX_PHYS_LAYER];
	// Start off with entirely disabled collisions
	for ( int i = 0; i < Layers::MAX_PHYS_LAYER; ++i )
	{
		collisionMasks[i] = 0;
	}

	// Use a small interface to make it identical to the Havok code (makes it easy to sync)
	// The bitmasks will be symmetrical when mapped out on a graph. There can be no one-way collisions.
	struct filterAssistance_t
	{
		void enableCollisionsBetween ( uint layerA, uint layerB )
		{
			// Enable the corresponding bits to enable collision
			Physics::Active()->collisionMasks[layerA] |= ( 1 << layerB );
			Physics::Active()->collisionMasks[layerB] |= ( 1 << layerA );
		}
		void disableCollisionsBetween ( uint layerA, uint layerB )
		{
			// Disable the corresponding bits to disable collision
			Physics::Active()->collisionMasks[layerA] &= ~( 1 << layerB );
			Physics::Active()->collisionMasks[layerB] &= ~( 1 << layerA );
		}
	} filter_assistance;
	filterAssistance_t* groupFilter = &filter_assistance;

	// Enable Landscape collisions with all groups
	groupFilter->enableCollisionsBetween( Layers::PHYS_LANDSCAPE, Layers::PHYS_DEBRIS );
	groupFilter->enableCollisionsBetween( Layers::PHYS_LANDSCAPE, Layers::PHYS_RAGDOLL );
	groupFilter->enableCollisionsBetween( Layers::PHYS_LANDSCAPE, Layers::PHYS_RAGDOLL_ESSENTIAL );
	groupFilter->enableCollisionsBetween( Layers::PHYS_LANDSCAPE, Layers::PHYS_CHARACTER );
	groupFilter->enableCollisionsBetween( Layers::PHYS_LANDSCAPE, Layers::PHYS_ITEM );
	groupFilter->enableCollisionsBetween( Layers::PHYS_LANDSCAPE, Layers::PHYS_DYNAMIC );
	groupFilter->enableCollisionsBetween( Layers::PHYS_LANDSCAPE, Layers::PHYS_SWEPTCOLLISION );
	groupFilter->disableCollisionsBetween( Layers::PHYS_LANDSCAPE, Layers::PHYS_LANDSCAPE );

	// Only have PHYS_WORLD_TRACE hit Landscape and Dynamic
	groupFilter->enableCollisionsBetween( Layers::PHYS_WORLD_TRACE, Layers::PHYS_LANDSCAPE );
	groupFilter->enableCollisionsBetween( Layers::PHYS_WORLD_TRACE, Layers::PHYS_DYNAMIC );
	groupFilter->enableCollisionsBetween( Layers::PHYS_WORLD_TRACE, Layers::PHYS_RAGDOLL_ESSENTIAL );
	groupFilter->disableCollisionsBetween( Layers::PHYS_WORLD_TRACE, Layers::PHYS_RAGDOLL );
	groupFilter->disableCollisionsBetween( Layers::PHYS_WORLD_TRACE, Layers::PHYS_ITEM );
	groupFilter->disableCollisionsBetween( Layers::PHYS_WORLD_TRACE, Layers::PHYS_CHARACTER );
	groupFilter->disableCollisionsBetween( Layers::PHYS_WORLD_TRACE, Layers::PHYS_SWEPTCOLLISION );
	groupFilter->disableCollisionsBetween( Layers::PHYS_WORLD_TRACE, Layers::PHYS_DEBRIS );
	groupFilter->disableCollisionsBetween( Layers::PHYS_WORLD_TRACE, Layers::PHYS_WORLD_TRACE );
	groupFilter->disableCollisionsBetween( Layers::PHYS_WORLD_TRACE, Layers::PHYS_HITBOX );

	// Disable debris collisions with all other groups except world
	groupFilter->disableCollisionsBetween( Layers::PHYS_DEBRIS, Layers::PHYS_RAGDOLL );
	groupFilter->disableCollisionsBetween( Layers::PHYS_DEBRIS, Layers::PHYS_RAGDOLL_ESSENTIAL );
	groupFilter->disableCollisionsBetween( Layers::PHYS_DEBRIS, Layers::PHYS_CHARACTER );
	groupFilter->disableCollisionsBetween( Layers::PHYS_DEBRIS, Layers::PHYS_SWEPTCOLLISION );
	groupFilter->disableCollisionsBetween( Layers::PHYS_DEBRIS, Layers::PHYS_ITEM );
	groupFilter->disableCollisionsBetween( Layers::PHYS_DEBRIS, Layers::PHYS_DYNAMIC );
	groupFilter->disableCollisionsBetween( Layers::PHYS_DEBRIS, Layers::PHYS_DEBRIS );

	// Dynamic Props collide with game essential props, being dynamic, character, item, and essential ragdoll
	groupFilter->enableCollisionsBetween( Layers::PHYS_DYNAMIC, Layers::PHYS_DYNAMIC );
	groupFilter->enableCollisionsBetween( Layers::PHYS_DYNAMIC, Layers::PHYS_CHARACTER );
	groupFilter->enableCollisionsBetween( Layers::PHYS_DYNAMIC, Layers::PHYS_SWEPTCOLLISION );
	groupFilter->enableCollisionsBetween( Layers::PHYS_DYNAMIC, Layers::PHYS_ITEM );
	groupFilter->enableCollisionsBetween( Layers::PHYS_DYNAMIC, Layers::PHYS_RAGDOLL_ESSENTIAL );
	groupFilter->disableCollisionsBetween( Layers::PHYS_DYNAMIC, Layers::PHYS_RAGDOLL );
	groupFilter->disableCollisionsBetween( Layers::PHYS_DYNAMIC, Layers::PHYS_DEBRIS );

	// Disable ragdoll collisions with all other groups except ragdolls
	groupFilter->disableCollisionsBetween( Layers::PHYS_RAGDOLL, Layers::PHYS_CHARACTER );
	groupFilter->disableCollisionsBetween( Layers::PHYS_RAGDOLL, Layers::PHYS_SWEPTCOLLISION );
	groupFilter->disableCollisionsBetween( Layers::PHYS_RAGDOLL, Layers::PHYS_ITEM );
	groupFilter->disableCollisionsBetween( Layers::PHYS_RAGDOLL, Layers::PHYS_DYNAMIC );
	groupFilter->enableCollisionsBetween( Layers::PHYS_RAGDOLL, Layers::PHYS_RAGDOLL_ESSENTIAL );
	groupFilter->enableCollisionsBetween( Layers::PHYS_RAGDOLL, Layers::PHYS_RAGDOLL );

	// Essential ragdolls act like dynamic props, with the difference that they collide with debris ragdolls
	groupFilter->enableCollisionsBetween( Layers::PHYS_RAGDOLL_ESSENTIAL, Layers::PHYS_DYNAMIC );
	groupFilter->enableCollisionsBetween( Layers::PHYS_RAGDOLL_ESSENTIAL, Layers::PHYS_CHARACTER );
	groupFilter->enableCollisionsBetween( Layers::PHYS_RAGDOLL_ESSENTIAL, Layers::PHYS_SWEPTCOLLISION );
	groupFilter->enableCollisionsBetween( Layers::PHYS_RAGDOLL_ESSENTIAL, Layers::PHYS_ITEM );
	groupFilter->enableCollisionsBetween( Layers::PHYS_RAGDOLL_ESSENTIAL, Layers::PHYS_RAGDOLL_ESSENTIAL );
	groupFilter->enableCollisionsBetween( Layers::PHYS_RAGDOLL_ESSENTIAL, Layers::PHYS_RAGDOLL );
	groupFilter->disableCollisionsBetween( Layers::PHYS_RAGDOLL_ESSENTIAL, Layers::PHYS_DEBRIS );

	// Items collide with other items, dynamic props, and essential ragdolls only
	groupFilter->disableCollisionsBetween( Layers::PHYS_ITEM, Layers::PHYS_RAGDOLL );
	groupFilter->enableCollisionsBetween( Layers::PHYS_ITEM, Layers::PHYS_RAGDOLL_ESSENTIAL );
	groupFilter->disableCollisionsBetween( Layers::PHYS_ITEM, Layers::PHYS_CHARACTER );
	groupFilter->disableCollisionsBetween( Layers::PHYS_ITEM, Layers::PHYS_SWEPTCOLLISION );
	groupFilter->disableCollisionsBetween( Layers::PHYS_ITEM, Layers::PHYS_ITEM );	// CORRECTION: ITEMS DON'T COLLIDE WITH ITEMS
	groupFilter->enableCollisionsBetween( Layers::PHYS_ITEM, Layers::PHYS_DYNAMIC );
	groupFilter->disableCollisionsBetween( Layers::PHYS_ITEM, Layers::PHYS_DEBRIS );

	// Weapons collide with only hitboxes and other weapons
	/*groupFilter->disableCollisionsBetween( Layers::PHYS_WEAPON, Layers::PHYS_RAGDOLL );
	groupFilter->enableCollisionsBetween( Layers::PHYS_WEAPON, Layers::PHYS_RAGDOLL_ESSENTIAL );
	groupFilter->disableCollisionsBetween( Layers::PHYS_WEAPON, Layers::PHYS_CHARACTER );
	groupFilter->disableCollisionsBetween( Layers::PHYS_WEAPON, Layers::PHYS_SWEPTCOLLISION );
	groupFilter->disableCollisionsBetween( Layers::PHYS_WEAPON, Layers::PHYS_ITEM );	// CORRECTION: ITEMS DON'T COLLIDE WITH ITEMS
	groupFilter->enableCollisionsBetween( Layers::PHYS_WEAPON, Layers::PHYS_DYNAMIC );
	groupFilter->disableCollisionsBetween( Layers::PHYS_WEAPON, Layers::PHYS_DEBRIS );*/
	groupFilter->enableCollisionsBetween( Layers::PHYS_WEAPON, Layers::PHYS_HITBOX );
	groupFilter->enableCollisionsBetween( Layers::PHYS_WEAPON, Layers::PHYS_WEAPON );

	// Characters collide with dynamic props, other characters, essential ragdolls, and the world only
	groupFilter->disableCollisionsBetween( Layers::PHYS_CHARACTER, Layers::PHYS_DEBRIS );
	groupFilter->disableCollisionsBetween( Layers::PHYS_CHARACTER, Layers::PHYS_RAGDOLL );
	groupFilter->enableCollisionsBetween( Layers::PHYS_CHARACTER, Layers::PHYS_RAGDOLL_ESSENTIAL );
	groupFilter->enableCollisionsBetween( Layers::PHYS_CHARACTER, Layers::PHYS_CHARACTER );
	groupFilter->enableCollisionsBetween( Layers::PHYS_CHARACTER, Layers::PHYS_SWEPTCOLLISION );
	groupFilter->disableCollisionsBetween( Layers::PHYS_CHARACTER, Layers::PHYS_ITEM );
	groupFilter->disableCollisionsBetween( Layers::PHYS_CHARACTER, Layers::PHYS_HITBOX );
	groupFilter->enableCollisionsBetween( Layers::PHYS_CHARACTER, Layers::PHYS_DYNAMIC );
	groupFilter->enableCollisionsBetween( Layers::PHYS_CHARACTER, Layers::PHYS_LANDSCAPE );

	// And bullets collide with everything
	groupFilter->enableCollisionsBetween( Layers::PHYS_BULLET_TRACE, Layers::PHYS_DEBRIS );
	groupFilter->enableCollisionsBetween( Layers::PHYS_BULLET_TRACE, Layers::PHYS_RAGDOLL );
	groupFilter->enableCollisionsBetween( Layers::PHYS_BULLET_TRACE, Layers::PHYS_RAGDOLL_ESSENTIAL );
	groupFilter->enableCollisionsBetween( Layers::PHYS_BULLET_TRACE, Layers::PHYS_CHARACTER );
	groupFilter->disableCollisionsBetween( Layers::PHYS_BULLET_TRACE, Layers::PHYS_SWEPTCOLLISION );
	groupFilter->enableCollisionsBetween( Layers::PHYS_BULLET_TRACE, Layers::PHYS_ITEM );
	groupFilter->enableCollisionsBetween( Layers::PHYS_BULLET_TRACE, Layers::PHYS_DYNAMIC );
	groupFilter->enableCollisionsBetween( Layers::PHYS_BULLET_TRACE, Layers::PHYS_LANDSCAPE );
	groupFilter->enableCollisionsBetween( Layers::PHYS_BULLET_TRACE, Layers::PHYS_HITBOX );

	// Hitboxes collide with whatever
	groupFilter->enableCollisionsBetween( Layers::PHYS_HITBOX, Layers::PHYS_LANDSCAPE );
	groupFilter->enableCollisionsBetween( Layers::PHYS_HITBOX, Layers::PHYS_BULLET_TRACE );
	groupFilter->disableCollisionsBetween( Layers::PHYS_HITBOX, Layers::PHYS_WORLD_TRACE );
	groupFilter->disableCollisionsBetween( Layers::PHYS_HITBOX, Layers::PHYS_DEBRIS );
	groupFilter->disableCollisionsBetween( Layers::PHYS_HITBOX, Layers::PHYS_RAGDOLL );
	groupFilter->disableCollisionsBetween( Layers::PHYS_HITBOX, Layers::PHYS_RAGDOLL_ESSENTIAL );
	groupFilter->disableCollisionsBetween( Layers::PHYS_HITBOX, Layers::PHYS_CHARACTER );
	groupFilter->disableCollisionsBetween( Layers::PHYS_HITBOX, Layers::PHYS_SWEPTCOLLISION );
	groupFilter->disableCollisionsBetween( Layers::PHYS_HITBOX, Layers::PHYS_ITEM );
	groupFilter->disableCollisionsBetween( Layers::PHYS_HITBOX, Layers::PHYS_DYNAMIC );
	groupFilter->disableCollisionsBetween( Layers::PHYS_HITBOX, Layers::PHYS_HITBOX );

	// Apply filter to the world now
	/*{
		pWorld->setCollisionFilter( groupFilter );
		groupFilter->removeReference();
	}*/
}
