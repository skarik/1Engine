

#include "CGameBehavior.h"
#include "CGameHandle.h"
#include "engine/state/CGameState.h"

// Constructor
//  adds behavior to the list of GBs in CGameState
//  active is default to true
CGameBehavior::CGameBehavior ( void )
{
	referenceCount = 1;
	persistent = false;
	layer = Layers::None;
	name = "Game Behavior";

	// Set active target to this
	active.setTarget( this );

	// Add to the list last
	// gbbools default to true, so already active.
	netid	= -1; // Set invalid net ID
	id		= CGameState::Active()->AddBehavior( this );
	guid	= CGameState::Active()->GetGUID();
}
// Destructor
//  removes behavior from the list of GBs in CGameState
//	also checks for references, in order to check that there are not dangling references
CGameBehavior::~CGameBehavior ( void )
{
	if ( referenceCount > 0 ) {
		throw core::NullReferenceException();
	}
	CGameState::Active()->RemoveBehavior( id );
}


// Private ID 'safe' set
void CGameBehavior::SetId( const gameid_t nId )
{
	id = nId;
}
// Private net ID 'safe' set
void CGameBehavior::SetNetId ( const netid_t nId )
{
	netid = nId;
}

// Delete object
void CGameBehavior::DeleteObject ( CGameBehavior* pObjectToDelete )
{
	CGameState::Active()->DeleteObject( pObjectToDelete );
}
// Delete object delayed
void CGameBehavior::DeleteObjectDelayed ( CGameBehavior* pObjectToDelete, float fDeleteTime )
{
	CGameState::Active()->DeleteObjectDelayed( pObjectToDelete,fDeleteTime );
}

// Returns a pointer to the spot in the array where the object is held.
// This is very unsafe and can cause memory leaks if not used properly!
// Avoid use of this function if possible!
/*CGameBehavior**	CGameBehavior::GetArrayPointer ( void )
{
	return CGameState::pActive->GetBehaviorArrayPointer ( this );
}*/

CGameHandle	CGameBehavior::GetHandle ( void )
{
	return CGameHandle( this );
}

//	SetPersistence
void CGameBehavior::SetPersistence ( bool nPersistence )
{
	persistent = nPersistence;
}