#ifndef _ITEM_TORCH_H_
#define _ITEM_TORCH_H_

#include "after/entities/item/CWeaponItem.h"
#include "engine/physics/collider/types/CCapsuleCollider.h"

class CLight;
class CParticleSystem;

class ItemTorch : public CWeaponItem
{
	ClassName( "ItemTorch" );
protected:
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;

		wpdata.iItemId = 4;

		wpdata.sInfo = "Torch";
		wpdata.bCanStack = true;
		wpdata.iMaxStack = 45;
		wpdata.fWeight = 1.0f;

		wpdata.bCanDegrade = true;
		wpdata.iMaxDurability = 600;

		return wpdata;
	}
public:
	// == Constructor ==
	ItemTorch ( void );
	// == Destructor ==
	~ItemTorch ( void );
	
	// Update function
	void Update ( void );

	// Use function
	bool Use( int x );
	
private:
	CLight* pLight;
	CParticleSystem* pFireSystem;

	ftype degradeTimer;
};

#endif