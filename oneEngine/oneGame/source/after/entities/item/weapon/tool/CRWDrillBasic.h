
#ifndef _C_RW_HAND_DRILL_H_
#define _C_RW_HAND_DRILL_H_

#include "after/entities/item/weapon/CBaseRandomMelee.h"

class CRWDrillBasic : public CBaseRandomMelee
{
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;
		wpdata.iItemId = 250;

		wpdata.sInfo = "Hand Drill";
		wpdata.bCanStack = false;
		wpdata.fWeight = 5.0f;

		return wpdata;
	}
public:
	explicit CRWDrillBasic ( void );
	explicit CRWDrillBasic ( tMeleeWeaponProperties& inProps );
	~CRWDrillBasic ( void );

	void Generate ( void );

	bool Use ( int x );

	void Update ( void );

protected:
	void SetBlockHardness ( void ) override;

	bool bHitBlock;
};

#endif