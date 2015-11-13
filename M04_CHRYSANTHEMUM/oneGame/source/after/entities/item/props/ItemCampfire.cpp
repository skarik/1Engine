

#include "ItemCampfire.h"
#include "after/entities/props/props/PropCampfire.h"
//#include "CVoxelTerrain.h"
#include "core/time/time.h"
#include "engine/physics/raycast/Raycaster.h"
#include "renderer/logic/model/CModel.h"
#include "after/types/terrain/BlockTracker.h"
#include "after/terrain/edit/CTerrainAccessor.h"

// Constructor (on ground)
ItemCampfire::ItemCampfire ( void )
	: CWeaponItem( ItemData() )
{
	transform.scale = Vector3d( 0.1f, 0.1f, 0.1f );

	if ( pModel == NULL ) {
		pModel = new CModel( string("models/props/campfire.FBX") );
		pModel->transform.scale = Vector3d( 0.1f, 0.1f, 0.1f );
	}
}

// Destructor
ItemCampfire::~ItemCampfire ( void )
{

}

// Lookats create some outlines
void	ItemCampfire::OnInteractLookAt	( CActor* interactingActor )
{
	if ( interactingActor->GetTypeName() == "CPlayer" )
	{
		//pModel->renderSettings.cOutlineColor = Color( 1.0f,1,1 );
		//pModel->renderSettings.fOutlineWidth = 0.02f;
	}
}
void	ItemCampfire::OnInteractLookAway	( CActor* interactingActor )
{
	if ( interactingActor->GetTypeName() == "CPlayer" )
	{
		//pModel->renderSettings.fOutlineWidth = -1.0f;
	}
}

// Use places the object on a terrain bit
bool ItemCampfire::Use ( int x )
{
	if ( x == Item::USecondary )
	{
		if ( !CanUse( x ) )
		{
			SetCooldown( x, Time::deltaTime*3.0f );
			return true;
		}
		SetCooldown( x, Time::deltaTime*3.0f );

		bool placed = false;

		// Remove cube that player is looking at.
		Ray viewRay = pOwner->GetEyeRay();
		RaycastHit		result;
		BlockTrackInfo	block;
		if ( Raycaster.Raycast( viewRay, 8.0f, &result, Physics::GetCollisionFilter(Layers::PHYS_BULLET_TRACE,0,31), pOwner ) )
		{
			TerrainAccess.GetBlockAtPosition( result, block );
			if ( block.valid && result.hitNormal.z > 0.5f )
			{
				PropCampfire* newComponent = new PropCampfire( block );
				newComponent->transform.position = result.hitPos;// + result.hitNormal*0.5f;
				//AddToTerrain();
				placed = true;
			}
		}

		if ( placed )
		{
			weaponItemState.iCurrentStack -= 1;
			if ( weaponItemState.iCurrentStack == 0 )
			{
				//CGameState::pActive->DeleteObject( this );
				// Delete this object in the inventory
			}
		}

		return true;
	}
	else
	{
		return false;
	}
}