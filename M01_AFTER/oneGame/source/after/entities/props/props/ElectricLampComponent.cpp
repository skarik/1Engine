
#include "ElectricLampComponent.h"
#include "after/entities/item/props/ItemElectricLamp.h"
#include "after/terrain/VoxelTerrain.h"
#include "core/time/time.h"
#include "engine/physics/raycast/Raycaster.h"
#include "renderer/light/CLight.h"

// Constructor (on terrain)
ElectricLampComponent::ElectricLampComponent ( BlockTrackInfo const& inInfo )
	: CTerrainProp( inInfo )
{
	//SetHoldState( CWeaponItem::Hover );

	//mBlockInfo = inInfo;
	//AddToTerrain();

	InitializeCommon();
}
// Common code
void ElectricLampComponent::InitializeCommon ( void )
{
	pLight = new CLight();
	pLight->diffuseColor= Color( 0.8f,0.75f,0.7f );
	pLight->range		= 0.01f;

	bLightOn = false;

	m_model = new CModel( "models/geosphere.FBX" );
	m_model->transform.scale = Vector3d( 0.4f,0.4f,0.4f );
}
// Destructor
ElectricLampComponent::~ElectricLampComponent ( void )
{
	RemoveFromTerrain();

	if ( pLight != NULL )
		delete pLight;
	pLight = NULL;
}

// Update
void ElectricLampComponent::Update ( void )
{
	if ( bLightOn )
	{
		pLight->range += ( 12.0f - pLight->range ) * Time::TrainerFactor( 5.0f );
		pLight->diffuseColor.red = 2.0f;
		pLight->pass = 0.3f;
	}
	else
	{
		pLight->range += ( 0.01f - pLight->range ) * Time::TrainerFactor( 1.0f );
		pLight->diffuseColor.red = 0.0f;
		pLight->pass = 0.01f;
	}
	pLight->transform.Get( transform );
}

// Lookats create some outlines
void	ElectricLampComponent::OnInteractLookAt	( CActor* interactingActor )
{
	if ( interactingActor->GetTypeName() == "CPlayer" )
	{
		//pModel->renderSettings.cOutlineColor = Color( 1.0f,1,1 );
		//pModel->renderSettings.fOutlineWidth = 0.08f;
	}
}
void	ElectricLampComponent::OnInteractLookAway	( CActor* interactingActor )
{
	if ( interactingActor->GetTypeName() == "CPlayer" )
	{
		//pModel->renderSettings.fOutlineWidth = -1.0f;
	}
}

// Presimulation is empty, really
void ElectricLampComponent::PreSimulation ( void )
{
	// This is the code for a power source
	/*SetWireBitChar( mBlockInfo.pBlock8->temp[b1index], 0 );
	SetPowerBitChar( mBlockInfo.pBlock8->temp[b1index], 1 );*/
	// Check our block for power
	
	
}
// Postsimulation is checking for on or off
void ElectricLampComponent::PostSimulation ( void )
{
	// This is the code for a power source
	/*SetWireBitChar( mBlockInfo.pBlock8->temp[b1index], 0 );
	SetPowerBitChar( mBlockInfo.pBlock8->temp[b1index], 1 );*/
	// Check all neighboring blocks for a pow
	//if ( GetPowerBit( mBlockInfo.pBlock8->data[mBlockInfo.b1index] ) )
	//if ( mBlockInfo.pBlock8->data[mBlockInfo.b1index].power )
	if ( mBlockInfo.pBlock->voltage > 0 )
	{
		bLightOn = true;
	}
	else
	{
		bLightOn = false;
	}
	
}

// On Punched. Is executed when an ItemBlockPuncher or derived hits this object.
void ElectricLampComponent::OnPunched ( RaycastHit const& hitInfo )
{
	ItemElectricLamp* pNewItem = new ItemElectricLamp();
	pNewItem->transform.Get( transform );

	DeleteObject( this );
}