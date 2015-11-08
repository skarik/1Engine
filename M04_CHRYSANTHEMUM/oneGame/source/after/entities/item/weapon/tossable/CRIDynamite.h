
#ifndef _C_RANDOM_ITEM_DYNAMITE_H_
#define _C_RANDOM_ITEM_DYNAMITE_H_

#include "after/entities/item/weapon/CBaseRandomTossable.h"

class CRIDynamite : public CBaseRandomTossable
{
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;
		wpdata.iItemId = 150;

		wpdata.sInfo = "Dynamite";
		wpdata.bCanStack = true;
		wpdata.iMaxStack = 5;

		wpdata.fWeight = 4.0f;

		return wpdata;
	}
public:
	explicit CRIDynamite ( tTossableProperties& inProps );
	~CRIDynamite ( void );

	CRIDynamite ( void );

	void Generate ( void );

	bool Use( int x );

	void Update ( void );

protected:
	bool lit;
	ftype fusetime;
	
};

#endif