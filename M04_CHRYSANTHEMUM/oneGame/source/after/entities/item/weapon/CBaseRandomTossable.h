
#ifndef _C_BASE_RANDOM_TOSSABLE_H_
#define _C_BASE_RANDOM_TOSSABLE_H_

#include "CBaseRandomItem.h"

class CBaseRandomTossable : public CBaseRandomItem
{
	ClassName( "RandomBaseTossable" );
	BaseClass( "RandomBaseTossable" );
public:
	struct tTossableProperties;

	explicit CBaseRandomTossable ( tTossableProperties&, const WItemData & wdat );
	~CBaseRandomTossable ( void );
	CWeaponItem& operator= ( const CWeaponItem * original ) override;

	// Serialization
	virtual void serialize ( Serializer &, const uint );

	// Properties struct
	struct tTossableProperties
	{
		ftype damage;
		int hands;
		int damagetype;
		ftype speed;
	};
	tTossableProperties tossable_stats;

	bool Use( int x );
protected:
	CBaseRandomItem*	lastTossed;
};

#endif