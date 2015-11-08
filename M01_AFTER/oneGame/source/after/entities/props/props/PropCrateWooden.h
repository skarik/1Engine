
#ifndef _PROP_CRATE_WOODEN_H_
#define _PROP_CRATE_WOODEN_H_

#include "after/entities/props/CTerrainProp.h"

class CInventory;

class PropCrateWooden : public CTerrainProp
{
	ClassName( "PropChestBase" );
public:
	explicit PropCrateWooden ( BlockTrackInfo const& inInfo );
	~PropCrateWooden ( void );

	// Update function
	void Update ( void );

	// Interact function
	void OnInteract ( CActor* interactingActor );

	//void OnInteractLookAt ( CActor* interactingActor );
	//void OnInteractLookAway ( CActor* interactingActor );

	void OnTerrainPlacement ( void );
	// Save data
	uint64_t	GetUserdata ( void ) override;
	// Load data
	void		SetUserdata ( const uint64_t& ) override;

	// On-Punched
	void OnPunched ( RaycastHit const& hitInfo );

protected:
	CInventory*	mInventory;
	ftype		mHealth;
	ftype		mLastHitTime;
	uint32_t	mInventoryId;
};

#endif//_PROP_CRATE_WOODEN_H_