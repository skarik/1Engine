
#ifndef _SKILL_MAGIC_BOLT_H_
#define _SKILL_MAGIC_BOLT_H_

#include "after/entities/item/skill/CSkill.h"

class glMaterial;
class CParticleSystem;

class SkillMagicBolt : public CSkill
{
public:
	// Constructor
				SkillMagicBolt ( void );
				~SkillMagicBolt ( void );

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
	static string GetName ( void ) { return "Magic Bolt"; }
	static string GetIconFilename ( void ) { return "textures/icons/skill_magicbolt.png"; }
protected:
	SKILL_DATA_DEFINE {
		SKILLDATA_DEFAULTS;
		data.eTopType		= WeaponItem::SkillActive;
		return data;
	}
protected:
	CParticleSystem*	ps_firetoss_active;
};

#endif//_SKILL_MAGIC_BOLT_H_