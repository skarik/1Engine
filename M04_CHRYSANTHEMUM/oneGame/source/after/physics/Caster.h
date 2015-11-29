//===============================================================================================//
//		engine-common/physics/Caster.h
//
//	Various casting routines for collision detection in the world.
//===============================================================================================//
#ifndef _ENGINE_COMMON_PHYISCS_CASTER_H_
#define _ENGINE_COMMON_PHYISCS_CASTER_H_


//=========================================//
// Includes
//=========================================//
#include "physical/physics/CPhysics.h"
#include "engine/physics/collider/sCollision.h"
#include "engine/physics/raycast/Raycaster.h"
#include "engine-common/types/ItemProperties.h"
//=========================================//
// Prototypes
//=========================================//
struct BlockTrackInfo;


//===============================================================================================//
// Caster class definition
//===============================================================================================//
class Caster
{
public:
	// THE FOLLOWING HELPERS SHOULD BE RETHOUGHT. THE HITTYPE MAY NEED TO GO TO ENGINE-COMMON
	// Helper function used for raycasting, since it's not trivial

	//===============================================================================================//
	// Collision casting
	//===============================================================================================//

	//	Raycast ( ray, range, out RaycastHit, out BlockTrackInfo, out CGameBehavior*, out outDamageMultiplier, filter, mismatch )
	// Casts a ray into the world, querying game data.
	static Item::HitType Raycast (
		const Ray& ray, const ftype& range,
		RaycastHit* outHitResult, BlockTrackInfo* outBlockInfo, CGameBehavior** outHitBehavior, ftype* outDamageMultiplier,
		const physCollisionFilter& collisionFilter=Physics::GetCollisionFilter(Layers::PHYS_BULLET_TRACE,0,31), void* mismatch=NULL
		);

	static Item::HitType Raycast (
		const Ray& ray, const ftype& range,
		RaycastHit* outHitResult, CGameBehavior** outHitBehavior, ftype* outDamageMultiplier,
		const physCollisionFilter& collisionFilter=Physics::GetCollisionFilter(Layers::PHYS_BULLET_TRACE,0,31), void* mismatch=NULL
		);

	static Item::HitType Linecast (
		const Ray& ray, const ftype& range, physShape* pShape,
		RaycastHit* outHitResult, CGameBehavior** outHitBehavior, ftype* outDamageMultiplier,
		const physCollisionFilter& collisionFilter=Physics::GetCollisionFilter(Layers::PHYS_BULLET_TRACE,0,31), void* mismatch=NULL
		);

	//===============================================================================================//
	// Collision classification
	//===============================================================================================//

	static Item::HitType GetHitType (
		const sCollision& collision,
		CGameBehavior** outHitBehavior, ftype* outDamageMultiplier );
};

#endif//_ENGINE_COMMON_PHYISCS_CASTER_H_




