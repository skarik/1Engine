// left click throws pebble out
// right click sends them to target

#ifndef _SKILL_MAGIKINESIS_H_
#define _SKILL_MAGIKINESIS_H_

#include "after/entities/item/skill/CSkill.h"

class glMaterial;
class CParticleSystem;
class ItemPebble;

class SkillMagikinesis : public CSkill
{
public:
	// Constructor
				SkillMagikinesis ( void );
				~SkillMagikinesis ( void );

	// Item cast
	bool		Use ( int ) override;
	void		EndUse ( int ) override;
	// Item sub-cast
	void		Update ( void ) override;

public:
	static string GetName ( void ) { return "Magikinesis"; }
	static string GetIconFilename ( void ) { return ".res/textures/icons/skill_magikinesis.png"; }
protected:
	SKILL_DATA_DEFINE {
		SKILLDATA_DEFAULTS;
		data.eTopType		= WeaponItem::SkillActive;
		return data;
	}
protected:
	int			cast_state;
	ftype		power_timer;
	Vector3d	target_position;
	//vector<ProjectilePebble*>	pebble_list; // make sure to check instances before mucking with them
	//std::vector<ItemPebble**>	pebble_list; //check instances are not owned by anyone before mucking with them
	std::vector<CGameHandle>	pebble_list;

	bool				hasAmmo;
	ItemPebble*			currentAmmo;
	CParticleSystem*	ps_magikinesis_active;
};

#endif//_SKILL_BLINK_H_