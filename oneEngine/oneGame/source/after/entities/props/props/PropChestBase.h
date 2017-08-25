

#ifndef _PROP_CHEST_BASE_H
#define _PROP_CHEST_BASE_H

#include "after/entities/props/CTerrainProp.h"

class CInventory;

class PropChestBase : public CTerrainProp
{
	ClassName( "PropChestBase" );
public:
	explicit PropChestBase ( BlockTrackInfo const& inInfo );
	~PropChestBase ( void );

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


	// Return this chest's inventory
	CInventory*	GetInventory ( void ) override;

protected:
	CInventory*	mInventory;
	ftype		mHealth;
	ftype		mLastHitTime;
	uint32_t	mInventoryId;
};

#endif//_PROP_CHEST_BASE_H