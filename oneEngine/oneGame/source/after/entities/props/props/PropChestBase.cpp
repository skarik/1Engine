
#include "PropChestBase.h"
#include "after/states/inventory/CInventory.h"
#include "core/math/random/Random.h"
#include "after/entities/character/CAfterPlayer.h"

#include "after/states/CWorldState.h"
#include "core/settings/CGameSettings.h"

// Constructor (on terrain)
PropChestBase::PropChestBase ( BlockTrackInfo const& inInfo )
	: CTerrainProp( inInfo )
{
	bInteractable = true;

	m_model = new CModel( string("models/props/chest_wood.FBX") );
	//pModel->GetAnimation()->Stop("open");

	mCollidesWithPlayer = true;

	mLastHitTime = 0.0f;
	mHealth = 35.0f;	

	mInventory = new CInventory( this, 4,6 ); 

	//strcpy( charName, "Wooden Chest" );
	mInventoryId = uint32_t(-1);
}

// Destructor
PropChestBase::~PropChestBase ( void )
{
	if ( mInventoryId != uint32_t(-1) )
	{
		arstring<256> inventoryFile;
		sprintf( inventoryFile.data, "%s.loot/%d", CGameSettings::Active()->GetTerrainSaveDir().c_str(), mInventoryId );
		mInventory->SaveInventory( inventoryFile );
	}
	delete mInventory;
}

// Update takes care of "health"
void PropChestBase::Update ( void )
{
	CTerrainProp::Update();

	if ( mLastHitTime > 0 ) {
		mLastHitTime -= Time::deltaTime;
	}
	else {
		mHealth = std::min<ftype>( 35.0f, mHealth + 5*Time::deltaTime );
	}
}


// Terrain placement. Override the set scale and rotation
void PropChestBase::OnTerrainPlacement ( void )
{
	//transform.scale = Vector3d( 1.5f,1.5f,1.5f ) * Random.Range( 0.9f, 1.1f );
	//transform.rotation = Vector3d( Random.Range(-15.0f,15.0f),Random.Range(-15.0f,15.0f),Random.Range(0,360.0f) );
}

// Save data
uint64_t PropChestBase::GetUserdata ( void ) 
{
	return uint64_t(mInventoryId);
}
// Load data
void PropChestBase::SetUserdata ( const uint64_t& userdata )
{
	uint32_t inputInventory = (uint32_t)userdata;
	if ( inputInventory == uint32_t(-1) )
	{
		if ( ActiveGameWorld )
		{
			mInventoryId = ActiveGameWorld->generationSubsystemState.iContainerCount;
			ActiveGameWorld->generationSubsystemState.iContainerCount += 1;
		}
		else
		{
			throw Core::NoWorldException();
		}
	}
	else
	{
		mInventoryId = inputInventory;
		arstring<256> inventoryFile;
		sprintf( inventoryFile.data, "%s.loot/%d", CGameSettings::Active()->GetTerrainSaveDir().c_str(), mInventoryId );
		mInventory->LoadInventory( inventoryFile );
	}
}

// Interact function
void PropChestBase::OnInteract ( CActor* interactingActor )
{
	if ( interactingActor->GetTypeName() == "CPlayer" )
	{
		((CAfterPlayer*)interactingActor)->OpenInventoryForSwapping(this->mInventory);
	}
}

// On Punched. Is executed when an ItemBlockPuncher or derived hits this object.
void PropChestBase::OnPunched ( RaycastHit const& hitInfo )
{
	/*ItemFloraSnowLily* pNewItem = new ItemFloraSnowLily();
	pNewItem->transform.Get( transform );

	DeleteObject( this );*/
	//pModel->GetAnimation()->Play("open");
	//if ( true )//mInventory->IsEmpty() )

	if ( mInventory->IsEmpty() )
	{
		mLastHitTime = 2.0f;
		mHealth -= 10.0f;
		if ( mHealth < 0.0f ) {
			DeleteObject( this );
		}
	}
}


// Return this chest's inventory
CInventory*	PropChestBase::GetInventory ( void ) 
{
	return mInventory;
}