
//#include "CWeaponItemComponents.h"
//#include "CWeaponItem.h"
#include "engine/physics/raycast/Raycaster.h"
#include "after/terrain/edit/CTerrainAccessor.h"

template <class PropClass>
CTransform* WeaponItem::ComponentPlacement ( CActor* owner, CWeaponItem* item, Vector3d* outNormal )
{
	bool placed = false;

	// Remove cube that player is looking at.
	Ray viewRay = owner->GetEyeRay();
	RaycastHit		result;
	BlockTrackInfo	block;
	if ( Raycaster.Raycast( viewRay, 8.0f, &result, Physics::GetCollisionFilter(Layers::PHYS_BULLET_TRACE,0,31) ) )
	{
		TerrainAccess.GetBlockAtPosition( result, block );
		if ( block.valid )
		{
			PropClass* newComponent = new PropClass( block );
			/*Vector3d targetPos = result.hitPos + result.hitNormal*0.5f;
			targetPos.x = floor( targetPos.x / 2 ) * 2;
			targetPos.y = floor( targetPos.y / 2 ) * 2;
			targetPos.z = floor( targetPos.z / 2 ) * 2;
			targetPos += Vector3d( 1,1,1 );
			if ( result.hitNormal.z > 0.5f ) {
				targetPos.z -= 0.4f;
			}
			else if ( result.hitNormal.z < -0.5f ) {
				targetPos.z += 0.2f;
			}
			else
			{
				targetPos -= result.hitNormal * 0.5f;
			}
			newComponent->transform.position = targetPos;*/
			//AddToTerrain();
			newComponent->transform.position = result.hitPos;

			item->AddToStack(-1);

			if ( outNormal ) {
				*outNormal = result.hitNormal;
			}
			
			return &(newComponent->transform);
		}
	}
	/*
	if ( placed )
	{
		//item->weaponItemState.iCurrentStack -= 1;
		//if ( item->weaponItemState.iCurrentStack == 0 )
		item->AddToStack( -1 );
		//{
			//CGameState::pActive->DeleteObject( this );
			// Delete this object in the inventory
		//}
	}*/
	/*if ( placed )
	{
		holdState = None;
		pOwner = NULL;
		if ( pBody == NULL )
			CreatePhysics();
	}*/

	return NULL;
}