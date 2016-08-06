
#include "CZonedCharacterController.h"
#include "CZonedCharacter.h"

#include "core/time/time.h"
#include "core/debug/CDebugConsole.h"
#include "engine/state/CGameState.h"
#include "engine-common/entities/CPlayer.h"
#include "after/entities/CCharacterModel.h"

using namespace std;
using NPC::CZonedCharacter;

// Static Vars
//vector<CZonedCharacter::characterinfo_t> CZonedCharacter::vCharacterList;
CPlayer*	CZonedCharacter::currentActivePlayer = NULL;

short CZonedCharacter::biomeType = 0;
short CZonedCharacter::terraType = 0;

// == Constructor ==
CZonedCharacter::CZonedCharacter ( const Vector3d & inPosition, const uint64_t & inID, const Rotator & inRotation )
	: CCharacter(), characterId(inID),
	fOutOfRangeTime(30), bOutOfRange(false), fActiveAreaCheckTime(30), bOutOfActiveArea(false), bSaveOnUnload(true)
{
	//ZCC_AddInstance();
	//NPC::Manager->AddCharacter( this );

	transform.position = inPosition;
	transform.rotation = inRotation;
	transform.SetDirty();

	pModel = NULL;
	animator = NULL;
	pCharModel = NULL;

	if ( currentActivePlayer == NULL ) {
		currentActivePlayer = (CPlayer*)CGameState::Active()->FindFirstObjectWithTypename( "CPlayer" );
	}
}

// == Destructor ==
CZonedCharacter::~CZonedCharacter ( void )
{
	/*if ( characterId >= 1024 ) {
		// Save the character
		NPC::Manager->SaveNPCOnDestroy ( this );
	}*/
	if ( bSaveOnUnload )
	{
		if ( characterId >= 1024 )
		{
			cout << "  **saving character...";
			// Save the character
			if ( NPC::Manager )
			{
				NPC::Manager->SaveCharacterOnUnload ( this );
				bSaveOnUnload = false;
				cout << "done" << endl;
			}
			else
			{
				Debug::Console->PrintError( "Could not find the NPC::Manager instance!\n" );
			}
		}
		bSaveOnUnload = false;
	}
	ZCC_RemoveInstance();

	delete_safe( pModel );
	delete_safe( pCharModel );
}

// == Update ==
void CZonedCharacter::Update ( void )
{
	CCharacter::Update();

	// Check for active area
	fActiveAreaCheckTime += Time::smoothDeltaTime;
	if ( (fActiveAreaCheckTime > 3.0f) || (!bOutOfActiveArea) )
	{
		fActiveAreaCheckTime = 0.0f;
		//bOutOfActiveArea	= !Zones.IsActiveArea( transform.position );
		bOutOfActiveArea	= !Zones.IsCollidableArea( transform.position );
		if ( bOutOfActiveArea )
		{
			bOutOfRange = !Zones.IsSuperArea( transform.position );
		}
		else
		{
			bOutOfRange = false;
			fOutOfRangeTime = 0;
		}
	}

	// Check for super active area
	if ( bOutOfRange || bOutOfActiveArea )
	{
		fOutOfRangeTime += Time::deltaTime;
		// Delete if hit time threadhold
		if ( fOutOfRangeTime > 3.0f || true )
		{
			fOutOfRangeTime = 0;
			cout << "Deleting character of ID " << ZCC_GetCharacterId() << " (leaving superarea)" << endl;
			/*if ( characterId >= 1024 )
				SaveInfo();*/
			if ( characterId >= 1024 ) {
				cout << "  **saving character...";
				// Save the character
				NPC::Manager->SaveCharacterOnUnload ( this );
				bSaveOnUnload = false;
				cout << "done" << endl;
			}
			DeleteObject( this );
		}
	}
	else {
		fOutOfRangeTime = 0; // Needed during override mode
	}

	// Perform update
	if ( bOutOfActiveArea )
	{
		if ( pModel )
			pModel->SetVisibility( false );
		if ( pCharModel )
			pCharModel->SetVisibility( false );
		UpdateInactive();
	}
	else
	{
		if ( pModel )
			pModel->SetVisibility( true );
		if ( pCharModel )
			pCharModel->SetVisibility( true );
		UpdateActive();
	}

}

// Nuthin' yet
void CZonedCharacter::LateUpdate ( void )
{}
void CZonedCharacter::PostUpdate ( void )
{}

// Save info
/*void CZonedCharacter::SaveInfo ( void )
{
	
}
void CZonedCharacter::LoadInfo ( void )
{
	
}*/