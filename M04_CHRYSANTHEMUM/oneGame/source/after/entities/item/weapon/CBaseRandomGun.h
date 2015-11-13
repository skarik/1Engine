
#ifndef _C_BASE_RANDOM_GUN_H_
#define _C_BASE_RANDOM_GUN_H_

#include "CBaseRandomItem.h"

class CBaseRandomGun : public CBaseRandomItem
{
	ClassName( "RandomBaseGun" );
	BaseClass( "RandomBaseGun" );
public:
	struct tGunWeaponProperties;

	explicit CBaseRandomGun ( tGunWeaponProperties&, const WItemData & wdat  );
	~CBaseRandomGun ( void );
	CWeaponItem& operator= ( const CWeaponItem * original ) override;

	// Properties struct
	struct tGunWeaponProperties
	{
		ftype accuracy;
		ftype damage;
		int hands;
		int clip_size;
		ftype atk_speed;
		ftype reload_speed;
		ftype bullet_speed;
	};
};


#endif