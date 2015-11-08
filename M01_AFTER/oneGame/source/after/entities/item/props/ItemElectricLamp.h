
#ifndef _ITEM_ELECTRIC_LAMP_
#define _ITEM_ELECTRIC_LAMP_

#include "after/entities/item/CWeaponItem.h"
//#include "after/entities/props/CTerrainProp.h"
//#include "CLight.h"

class ItemElectricLamp : public CWeaponItem
{
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;
		wpdata.iItemId = 100;
		wpdata.sInfo = "Electric Lamp";
		wpdata.bCanStack = 1;
		wpdata.iMaxStack = 25;
		wpdata.fWeight = 1.0f;
		return wpdata;
	}
public:
	ItemElectricLamp ( void );
	//ItemElectricLamp ( BlockInfo const& inInfo );
	~ItemElectricLamp ( void );

	// Create common
	void InitializeCommon ( void );

	// Update function
	void Update ( void );

	// Use function
	bool Use( int x );

	// Lookat calls
	void	OnInteractLookAt	( CActor* interactingActor );
	void	OnInteractLookAway	( CActor* interactingActor );

	// Pre-simulation. Is executed after the electric current map clears
	//void PreSimulation ( void );
	// Post-simulation. Is executed after all simulation has finished
	//void PostSimulation ( void );

	// On-Punched
	//void OnPunched ( RaycastHit const& hitInfo );

private:
	//CLight*	pLight;
	//bool	bLightOn;

};

#endif