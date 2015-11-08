
#include "Caster.h"

//#include "../CVoxelTerrain.h"
#include "engine/behavior/CGameBehavior.h"
#include "engine/behavior/CGameObject.h"
#include "engine/physics/collider/sCollision.h"
#include "engine/physics/raycast/Raycaster.h"
#include "engine/physics/motion/CRigidbody.h"

#include "engine-common/entities/CActor.h"
#include "engine-common/physics/motion/CRagdollCollision.h"

#include "after/terrain/VoxelTerrain.h"
#include "after/types/terrain/BlockTracker.h"

Item::HitType Caster::GetHitType (
		const sCollision& collision,
		CGameBehavior** outHitBehavior, ftype* outDamageMultiplier )
{
	if ( collision.m_hit_Other )
	{
		/*if (( !CVoxelTerrain::terrainList.empty() )&&( collision.m_hit_Other == CVoxelTerrain::terrainList[0] ))
		{
			*outHitBehavior = collision.m_hit_Other;
			return Item::HIT_TERRAIN;
		}
		else if ( collision.m_hit_Other == COctreeTerrain::GetActive() )
		{
			*outHitBehavior = collision.m_hit_Other;
			return Item::HIT_TERRAIN;
		}*/
		if ( collision.m_hit_Other == CVoxelTerrain::GetActive() )
		{
			*outHitBehavior = collision.m_hit_Other;
			return Item::HIT_TERRAIN;
		}
		else if ( collision.m_hit_Other->GetTypeName() == "CRagdollCollision" )
		{
			CRagdollCollision* pHitRagdoll = (CRagdollCollision*) collision.m_hit_Other;
			if ( outDamageMultiplier ) {
				*outDamageMultiplier = pHitRagdoll->GetMultiplier( (physRigidBody*) collision.m_hit_Other );
			}
			*outHitBehavior = pHitRagdoll->GetActor();
			if ( *outHitBehavior ) {
				return Item::HIT_CHARACTER;
			}
		}
		else if ( ((CRigidBody*)collision.m_hit_Other)->GetOwner() == NULL )
		{
			*outHitBehavior = NULL;
			return Item::HIT_UNKNOWN;
		}
		else if ( ((CRigidBody*)collision.m_hit_Other)->GetOwner()->GetBaseClassName() == "CFoliage_TreeBase" )
		{
			*outHitBehavior = ((CRigidBody*)collision.m_hit_Other)->GetOwner();
			return Item::HIT_TREE;
		}
		else if ( ((CRigidBody*)collision.m_hit_Other)->GetOwner()->GetBaseClassName() == "CGameObject_TerrainProp" )
		{
			*outHitBehavior = ((CRigidBody*)collision.m_hit_Other)->GetOwner();
			return Item::HIT_COMPONENT;
		}
		else
		{
			CGameObject* pHitObject = ((CRigidBody*)collision.m_hit_Other)->GetOwner();
			if ( pHitObject )
			{
				if ( pHitObject->GetBaseClassName() == "CActor_Character" )
				{
					*outHitBehavior = pHitObject;
					if ( outDamageMultiplier ) {
						*outDamageMultiplier = 1;
					}
					return Item::HIT_CHARACTER;
				}
				else if ( pHitObject->GetTypeName() == "CActor" )
				{
					*outHitBehavior = pHitObject;
					if ( outDamageMultiplier ) {
						*outDamageMultiplier = 1;
					}
					return Item::HIT_ACTOR;
				}
			}
			else
			{
				std::cout << "Checked a rigidbody with no associated Gamebehavior!" << std::endl;
			}
		}
		return Item::HIT_UNKNOWN;
	}
	return Item::HIT_NONE;
}

Item::HitType Caster::Raycast (
		const Ray& ray, const ftype& range,
		RaycastHit* outHitResult, CGameBehavior** outHitBehavior, ftype* outDamageMultiplier,
		const uint32_t collisionFilter, void* mismatch
		)
{
	BlockTrackInfo dirty;
	return Raycast( ray, range, outHitResult, &dirty, outHitBehavior, outDamageMultiplier, collisionFilter, mismatch );
}
Item::HitType Caster::Raycast (
		const Ray& ray, const ftype& range,
		RaycastHit* outHitResult, BlockTrackInfo* outBlockInfo, CGameBehavior** outHitBehavior, ftype* outDamageMultiplier,
		const uint32_t collisionFilter, void* mismatch
		)
{
	uint32_t targetFilter = collisionFilter;
	if ( targetFilter == 0 ) {
		targetFilter = Physics::GetCollisionFilter(Layers::PHYS_BULLET_TRACE,0,31);
	}

	//outBlockInfo
	throw Core::NotYetImplementedException( "Returning block data" );

	if ( Raycaster.Raycast( ray, range, outHitResult, targetFilter, mismatch ) )
	{
		if ( outHitResult->pHitBehavior == NULL )
		{
			*outHitBehavior = NULL;
			return Item::HIT_UNKNOWN;
		}
		// CVoxelTerrain::terrainList[0] is always guarenteed to be the current active terrain, even if there are multiple terrains existing.
		/*else if (( !CVoxelTerrain::terrainList.empty() )&&( outHitResult->pHitBehavior == CVoxelTerrain::terrainList[0] ))
		{
			*outHitBehavior = outHitResult->pHitBehavior;
			return Item::HIT_TERRAIN;
		}
		else if ( outHitResult->pHitBehavior == COctreeTerrain::GetActive() )
		{
			*outHitBehavior = outHitResult->pHitBehavior;
			return Item::HIT_TERRAIN;
		}*/
		else if ( outHitResult->pHitBehavior == CVoxelTerrain::GetActive() )
		{
			*outHitBehavior = outHitResult->pHitBehavior;
			return Item::HIT_TERRAIN;
		}
		else if ( outHitResult->pHitBehavior->GetTypeName() == "CRagdollCollision" )
		{
			CRagdollCollision* pHitRagdoll = (CRagdollCollision*) outHitResult->pHitBehavior;
			if ( outDamageMultiplier ) {
				*outDamageMultiplier = pHitRagdoll->GetMultiplier( (physRigidBody*) outHitResult->pHitBody );
			}
			*outHitBehavior = pHitRagdoll->GetActor();
			if ( *outHitBehavior ) {
				return Item::HIT_CHARACTER;
			}
		}
		else if ( ((CRigidBody*)outHitResult->pHitBehavior)->GetOwner() == NULL )
		{
			*outHitBehavior = NULL;
			return Item::HIT_UNKNOWN;
		}
		else if ( ((CRigidBody*)outHitResult->pHitBehavior)->GetOwner()->GetBaseClassName() == "CFoliage_TreeBase" )
		{
			*outHitBehavior = ((CRigidBody*)outHitResult->pHitBehavior)->GetOwner();
			return Item::HIT_TREE;
		}
		else if ( ((CRigidBody*)outHitResult->pHitBehavior)->GetOwner()->GetBaseClassName() == "CGameObject_TerrainProp" )
		{
			*outHitBehavior = ((CRigidBody*)outHitResult->pHitBehavior)->GetOwner();
			return Item::HIT_COMPONENT;
		}
		else
		{
			CGameObject* pHitObject = ((CRigidBody*)outHitResult->pHitBehavior)->GetOwner();
			if ( pHitObject )
			{
				if ( pHitObject->GetBaseClassName() == "CActor_Character" )
				{
					*outHitBehavior = pHitObject;
					if ( outDamageMultiplier ) {
						*outDamageMultiplier = 1;
					}
					return Item::HIT_CHARACTER;
				}
				else if ( pHitObject->GetTypeName() == "CActor" )
				{
					*outHitBehavior = pHitObject;
					if ( outDamageMultiplier ) {
						*outDamageMultiplier = 1;
					}
					return Item::HIT_ACTOR;
				}
			}
			else
			{
				std::cout << "Traced into a rigidbody with no associated Gamebehavior!" << std::endl;
			}
		}
		return Item::HIT_UNKNOWN;
	}


	return Item::HIT_NONE;
}

Item::HitType Caster::Linecast (
		const Ray& ray, const ftype& range, physShape* pShape,
		RaycastHit* outHitResult, CGameBehavior** outHitBehavior, ftype* outDamageMultiplier,
		const uint32_t collisionFilter, void* mismatch
		)
{
	uint32_t targetFilter = collisionFilter;
	if ( targetFilter == 0 ) {
		targetFilter = Physics::GetCollisionFilter(Layers::PHYS_BULLET_TRACE,0,31);
	}

	if ( Raycaster.Linecast( ray, range, pShape, outHitResult, 1, targetFilter, mismatch ) )
	{
		if ( outHitResult->pHitBehavior == NULL )
		{
			*outHitBehavior = NULL;
			return Item::HIT_UNKNOWN;
		}
		// CVoxelTerrain::terrainList[0] is always guarenteed to be the current active terrain, even if there are multiple terrains existing.
		/*else if (( !CVoxelTerrain::terrainList.empty() )&&( outHitResult->pHitBehavior == CVoxelTerrain::terrainList[0] ))
		{
			*outHitBehavior = outHitResult->pHitBehavior;
			return Item::HIT_TERRAIN;
		}
		else if (( COctreeTerrain::GetActive() )&&( outHitResult->pHitBehavior == COctreeTerrain::GetActive() ))
		{
			*outHitBehavior = outHitResult->pHitBehavior;
			return Item::HIT_TERRAIN;
		}*/
		else if ( outHitResult->pHitBehavior == CVoxelTerrain::GetActive() )
		{
			*outHitBehavior = outHitResult->pHitBehavior;
			return Item::HIT_TERRAIN;
		}
		else if ( outHitResult->pHitBehavior->GetTypeName() == "CRagdollCollision" )
		{
			CRagdollCollision* pHitRagdoll = (CRagdollCollision*) outHitResult->pHitBehavior;
			if ( outDamageMultiplier ) {
				*outDamageMultiplier = pHitRagdoll->GetMultiplier( (physRigidBody*) outHitResult->pHitBody );
			}
			*outHitBehavior = pHitRagdoll->GetActor();
			if ( *outHitBehavior ) {
				return Item::HIT_CHARACTER;
			}
		}
		else if ( ((CRigidBody*)outHitResult->pHitBehavior)->GetOwner() == NULL )
		{
			*outHitBehavior = NULL;
			return Item::HIT_UNKNOWN;
		}
		else if ( ((CRigidBody*)outHitResult->pHitBehavior)->GetOwner()->GetBaseClassName() == "CFoliage_TreeBase" )
		{
			*outHitBehavior = ((CRigidBody*)outHitResult->pHitBehavior)->GetOwner();
			return Item::HIT_TREE;
		}
		else if ( ((CRigidBody*)outHitResult->pHitBehavior)->GetOwner()->GetBaseClassName() == "CGameObject_TerrainProp" )
		{
			*outHitBehavior = ((CRigidBody*)outHitResult->pHitBehavior)->GetOwner();
			return Item::HIT_COMPONENT;
		}
		else
		{
			CGameObject* pHitObject = ((CRigidBody*)outHitResult->pHitBehavior)->GetOwner();
			if ( pHitObject )
			{
				if ( pHitObject->GetBaseClassName() == "CActor_Character" )
				{
					*outHitBehavior = pHitObject;
					if ( outDamageMultiplier ) {
						*outDamageMultiplier = 1;
					}
					return Item::HIT_CHARACTER;
				}
				else if ( pHitObject->GetTypeName() == "CActor" )
				{
					*outHitBehavior = pHitObject;
					if ( outDamageMultiplier ) {
						*outDamageMultiplier = 1;
					}
					return Item::HIT_ACTOR;
				}
			}
			else
			{
				std::cout << "Casted into a rigidbody with no associated Gamebehavior!" << std::endl;
			}
		}
		return Item::HIT_UNKNOWN;
	}


	return Item::HIT_NONE;
}