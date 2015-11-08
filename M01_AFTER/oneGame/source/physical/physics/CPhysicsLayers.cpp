
// Include class definition
#include "CPhysics.h"
#include "physical/system/Layers.h"

// == Static Variable Delarations ==
//int*	CPhysics::systemGroups = NULL;

//===Function Definitions===
void CPhysics::InitLayers ( void )
{
	// Get the world's filter
	//hkpGroupFilter* groupFilter = (hkpGroupFilter*)pWorld->getCollisionFilter();
	{
		/*hkpGroupFilter* filter = new hkpGroupFilter();
		pWorld->setCollisionFilter( filter );
		filter->removeReference();*/
	}
	//hkpGroupFilter* groupFilter = (hkpGroupFilter*)pWorld->getCollisionFilter();
	hkpGroupFilter* groupFilter = new hkpGroupFilter();

	/*sg_landscape	= groupFilter.getNewSystemGroup();
	sg_debris		= groupFilter.getNewSystemGroup();
	sg_ragdoll		= groupFilter.getNewSystemGroup();
	sg_ragdoll_es	= groupFilter.getNewSystemGroup();
	sg_characters	= groupFilter.getNewSystemGroup();
	sg_items		= groupFilter.getNewSystemGroup();
	sg_dynamic		= groupFilter.getNewSystemGroup();*/
	systemGroups = new int [Layers::MAX_PHYS_LAYER];
	for ( int i = 0; i < Layers::MAX_PHYS_LAYER; ++i )
	{
		systemGroups[i] = groupFilter->getNewSystemGroup();
	}

	// Start off with disabled collisions w/ all
	uint32_t t_allBits = 0;
	for ( int i = 0; i < Layers::MAX_PHYS_LAYER; ++i ) {
			t_allBits |= 1 << i;
	}
	groupFilter->disableCollisionsUsingBitfield( t_allBits, t_allBits );

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
	{
		pWorld->setCollisionFilter( groupFilter );
		groupFilter->removeReference();
	}
}
