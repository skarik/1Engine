
#ifndef _SKILL_FIRE_ARROW_H_
#define _SKILL_FIRE_ARROW_H_

#include "after/entities/item/skill/CSkill.h"

class CParticleSystem;
class CBaseRandomBow;

class SkillFireArrow : public CSkill
{
public:
	// Constructor
				SkillFireArrow ( void );
				~SkillFireArrow ( void );

	// Item cast
	//bool		Use ( int ) override;
	//void		EndUse ( int ) override;
	// Item sub-cast
	//void		Update ( void ) override;

	// Item cast
	bool		Use ( int ) override;

	// Is modifier for bow items
	bool		IsModifier ( const CWeaponItem* ) override;

public:
	static string GetName ( void ) { return "Fire Arrow"; }
	static string GetIconFilename ( void ) { return ".res/textures/icons/skill_firearrow.png"; }
protected:
	SKILL_DATA_DEFINE {
		SKILLDATA_DEFAULTS;
		data.eTopType		= WeaponItem::SkillActive;

		data.iHands			= 0;
		return data;
	}
protected:
	//int			cast_state;
	//ftype		power_timer;
	//Vector3d	target_position;
	//vector<ProjectilePebble*>	pebble_list; // make sure to check instances before mucking with them
	//vector<ItemPebble**>	pebble_list; //check instances are not owned by anyone before mucking with them

	//bool				hasAmmo;
	//ItemPebble*			currentAmmo;
	//CParticleSystem*	ps_firetoss_active;

	//CModel*				mMagicModel;
	CBaseRandomBow*		mBow;
};

#endif//_SKILL_FIRETOSS_H_