
#include "PropCrateWooden.h"
#include "after/states/inventory/CInventory.h"
#include "core/math/random/Random.h"
#include "after/entities/character/CAfterPlayer.h"

#include "after/states/CWorldState.h"
#include "core/settings/CGameSettings.h"

using std::cout;
using std::endl;

// Constructor (on terrain)
PropCrateWooden::PropCrateWooden ( BlockTrackInfo const& inInfo )
	: CTerrainProp( inInfo )
{
	m_model = new CModel( "models/props/crate0.FBX" );
	//pModel->GetAnimation()->Stop("open");

	mCollidesWithPlayer = true;

	mLastHitTime = 0.0f;
	mHealth = 35.0f;	

	mInventory = new CInventory( this, 4,4 ); 

	//strcpy( charName, "Wooden Chest" );
	mInventoryId = uint32_t(-1);
}

// Destructor
PropCrateWooden::~PropCrateWooden ( void )
{
	if ( mInventoryId != uint32_t(-1) )
	{
		arstring<256> inventoryFile;
		sprintf( inventoryFile.data, "%s.loot/%d", CGameSettings::Active()->MakeWorldSaveDirectory().c_str(), mInventoryId );
		mInventory->SaveInventory( inventoryFile );
	}
	delete mInventory;
}

// Update takes care of "health"
void PropCrateWooden::Update ( void )
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
void PropCrateWooden::OnTerrainPlacement ( void )
{
	//transform.scale = Vector3d( 1.5f,1.5f,1.5f ) * Random.Range( 0.9f, 1.1f );
	//transform.rotation = Vector3d( Random.Range(-15.0f,15.0f),Random.Range(-15.0f,15.0f),Random.Range(0,360.0f) );
}

// Save data
uint64_t PropCrateWooden::GetUserdata ( void ) 
{
	return uint64_t(mInventoryId);
}
// Load data
void PropCrateWooden::SetUserdata ( const uint64_t& userdata )
{
	uint32_t inputInventory = (uint32_t)userdata;
	if ( inputInventory == uint32_t(-1) )
	{
		mInventoryId = ActiveGameWorld->generationSubsystemState.iContainerCount;
		ActiveGameWorld->generationSubsystemState.iContainerCount += 1;
	}
	else
	{
		mInventoryId = inputInventory;
		arstring<256> inventoryFile;
		sprintf( inventoryFile.data, "%s.loot/%d", CGameSettings::Active()->MakeWorldSaveDirectory().c_str(), mInventoryId );
		mInventory->LoadInventory( inventoryFile );
	}
}

// Interact function
void PropCrateWooden::OnInteract ( CActor* interactingActor )
{
	if ( interactingActor->GetTypeName() == "CPlayer" )
	{
		/*switch ( Random.Next()%4 ) {
			case 0: strcpy( charName, "OH YES, USE ME!" );
				break;
			case 1: strcpy( charName, "Wooden CHEST" );
				break;
			case 2: strcpy( charName, "Sexy Chest of Unknowing" );
				break;
			case 3: strcpy( charName, "Box box box box box" );
				break;
		}*/
		switch ( Random.Next()%4 ) {
			case 0: cout << "OH YES, USE ME!" << endl;
				break;
			case 1: cout << "Wooden CHEST" << endl;
				break;
			case 2: cout << "Sexy Chest of Unknowing" << endl;
				break;
			case 3: cout << "Box box box box box" << endl;
				break;
		}

		((CAfterPlayer*)interactingActor)->OpenInventoryForSwapping(this->mInventory);
	}
}

// On Punched. Is executed when an ItemBlockPuncher or derived hits this object.
void PropCrateWooden::OnPunched ( RaycastHit const& hitInfo )
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