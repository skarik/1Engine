
#ifndef _FOLIAGE_CACTUS_DEFAULT_H_
#define _FOLIAGE_CACTUS_DEFAULT_H_

#include "after/entities/foliage/CTreeBase.h"
#include "engine-common/types/Damage.h"

class FoliageCactusDefault : public CTreeBase
{
	ClassName( "FoliageCactusDefault" );
public:
	FoliageCactusDefault ( void );

	void GenerateTreeData ( void );

	Damage GetClimbDamage ( void ) override {
		Damage dmg;
		dmg.amount = 2.0f;
		dmg.type = DamageType::Reflect;
		dmg.stagger_chance = 0;
		return dmg;
	};

protected:
	CWeaponItem* CreateResinItem ( void ) override { return NULL; };

};

#endif//_FOLIAGE_CACTUS_DEFAULT_H_