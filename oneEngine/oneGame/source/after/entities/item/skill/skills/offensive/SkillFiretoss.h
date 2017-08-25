
#ifndef _SKILL_FIRETOSS_H_
#define _SKILL_FIRETOSS_H_

#include "after/entities/item/skill/CSkill.h"

class glMaterial;
class CParticleSystem;
class CLight;

class SkillFiretoss : public CSkill
{
public:
	// Constructor
				SkillFiretoss ( void );
				~SkillFiretoss ( void );

	// Item cast
	bool		Use ( int ) override;
	void		EndUse ( int ) override;
	// Item sub-cast
	void		Update ( void ) override;
	// Attack function
	void		Attack ( XTransform& )	override;

	// CastCombine. Combines two casting skills for an effect. Is called when both skills are casted and not channelling.
	// Return true if something happens. Generally, objects look through all the spells, and stop the skills by setting isCasting to false.
	// Returning true will normally stop non-instant skills.
	bool CastCombine ( CSkill* combineSkill ) override;

public:
	static string GetName ( void ) { return "Firetoss"; }
	static string GetIconFilename ( void ) { return ".res/textures/icons/skill_firetoss.png"; }
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
	//vector<ItemPebble**>	pebble_list; //check instances are not owned by anyone before mucking with them

	bool				hasAmmo;
	//ItemPebble*			currentAmmo;
	CParticleSystem*	ps_firetoss_active;

	CModel*				mMagicModel;
	CLight*				mMagicLight;
};

#endif//_SKILL_FIRETOSS_H_