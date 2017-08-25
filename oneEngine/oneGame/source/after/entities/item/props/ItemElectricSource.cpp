

#include "ItemElectricSource.h"
#include "after/entities/props/props/ElectricSourceComponent.h"
//#include "CVoxelTerrain.h"
#include "core/time/time.h"
#include "engine/physics/raycast/Raycaster.h"
#include "renderer/material/glMaterial.h"
#include "renderer/logic/model/CModel.h"
#include "after/types/terrain/BlockTracker.h"
#include "after/terrain/edit/CTerrainAccessor.h"

// Constructor (on ground)
ItemElectricSource::ItemElectricSource ( void )
	: CWeaponItem( ItemData() )
{
	InitializeCommon();
}
// Common code
void ItemElectricSource::InitializeCommon ( void )
{
	transform.scale = Vector3d( 0.2f, 0.2f, 0.2f );

	glMaterial* newMat = new glMaterial();
	newMat->m_diffuse = Color( 0.6f,0.3f,0.3f );
	newMat->removeReference();

	pModel = new CModel( string("models\\geosphere.FBX") );
}
// Destructor
ItemElectricSource::~ItemElectricSource ( void )
{

}

// Update
void ItemElectricSource::Update ( void )
{
	
}

// Use places the object on a terrain bit
bool ItemElectricSource::Use ( int x )
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
			if ( block.valid )
			{
				ElectricSourceComponent* newComponent = new ElectricSourceComponent( block );
				newComponent->transform.position = result.hitPos + result.hitNormal*0.5f;
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
		/*if ( placed )
		{
			holdState = None;
			pOwner = NULL;
			if ( pBody == NULL )
				CreatePhysics();
		}*/

		return true;
	}
	else
	{
		return false;
	}
}