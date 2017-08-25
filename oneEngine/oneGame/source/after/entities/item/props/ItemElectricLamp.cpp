

#include "ItemElectricLamp.h"
#include "after/entities/props/props/ElectricLampComponent.h"
//#include "CVoxelTerrain.h"
#include "core/time/Time.h"
#include "engine/physics/raycast/Raycaster.h"
#include "renderer/logic/model/CModel.h"
#include "after/types/terrain/BlockTracker.h"
#include "after/terrain/edit/CTerrainAccessor.h"

// Constructor (on ground)
ItemElectricLamp::ItemElectricLamp ( void )
	: CWeaponItem( ItemData() )
{
	InitializeCommon();
}
// Constructor (on terrain)
/*ItemElectricLamp::ItemElectricLamp ( BlockInfo const& inInfo )
	: CWeaponItem()
{
	SetHoldState( CWeaponItem::Hover );

	mBlockInfo = inInfo;

	InitializeCommon();
}*/
// Common code
void ItemElectricLamp::InitializeCommon ( void )
{
	/*pLight = new CLight();
	pLight->diffuseColor= Color( 0.8f,0.75f,0.7f );
	pLight->range		= -0.01f;

	bLightOn = false;*/
	transform.scale = Vector3d( 0.2f, 0.2f, 0.2f );

	if ( pModel == NULL )
		pModel = new CModel( string("models\\geosphere.FBX") );
	//pModel->transform.scale = Vector3d( 0.01f,0.01f,0.01f );
}
// Destructor
ItemElectricLamp::~ItemElectricLamp ( void )
{
	/*if ( pLight != NULL )
		delete pLight;
	pLight = NULL;*/
}

// Update
void ItemElectricLamp::Update ( void )
{
	/*if ( bLightOn )
	{
		pLight->range += ( 6.0f - pLight->range ) * Time::TrainerFactor( 70.0f );
	}
	else
	{
		pLight->range += ( -0.01f - pLight->range ) * Time::TrainerFactor( 40.0f );
	}	*/
}

// Lookats create some outlines
void	ItemElectricLamp::OnInteractLookAt	( CActor* interactingActor )
{
	if ( interactingActor->GetTypeName() == "CPlayer" )
	{
		//pModel->renderSettings.cOutlineColor = Color( 1.0f,1,1 );
		//pModel->renderSettings.fOutlineWidth = 0.02f;
	}
}
void	ItemElectricLamp::OnInteractLookAway	( CActor* interactingActor )
{
	if ( interactingActor->GetTypeName() == "CPlayer" )
	{
		//pModel->renderSettings.fOutlineWidth = -1.0f;
	}
}

// Use places the object on a terrain bit
bool ItemElectricLamp::Use ( int x )
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
				ElectricLampComponent* newComponent = new ElectricLampComponent( block );
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

// Presimulation is empty, really
/*void ElectricLamp::PreSimulation ( void )
{
	// This is the code for a power source
	// Check our block for power
	
	
}
// Postsimulation is checking for on or off
void ElectricLamp::PostSimulation ( void )
{
	// This is the code for a power source
	// Check all neighboring blocks for a pow
	if ( holdState == Hover )
	{
		if ( GetPowerBit( mBlockInfo.pBlock8->temp[mBlockInfo.b1index] ) )
		{
			bLightOn = true;
		}
		else
		{
			bLightOn = false;
		}
	}
	
}

// On Punched. Is executed when an ItemBlockPuncher or derived hits this object.
virtual void ElectricLamp::OnPunched ( RaycastHit const& hitInfo )
{
	if ( holdState == Hover )
	{
		this->SetHoldState( CWeaponItem::None );
	}
}*/