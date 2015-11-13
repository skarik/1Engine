
#ifndef _SKILL_BLINK_H_
#define _SKILL_BLINK_H_

#include "after/entities/item/skill/CSkill.h"

class glMaterial;
class CParticleSystem;

class SkillBlink : public CSkill
{
public:
	// Constructor
				SkillBlink ( void );
				~SkillBlink ( void );

	// Item cast
	bool		Use ( int ) override;
	// Item sub-cast
	void		Update ( void ) override;

	static string GetName ( void ) { return "Blink"; }
	static string GetDescription ( void ) { return "Take advantage of the weak reality and teleport to a target position."; }
	// GetThisLevelInfo
	//  Returns the info of what this skill does this level
	/*static string GetThisLevelInfo ( void ) { return "Baller."; }
	// GetNextLevelInfo
	//  Returns the info of what this skill does next level
	static string GetNextLevelInfo ( void ) { return "More baller."; }
	// GetThisLevelCosts
	//  Sets the input values to the costs of the skill for this level
	static void GetThisLevelCost ( Real& o_health, Real& o_stamina, Real& o_mana ) { o_health=0; o_stamina=0; o_mana=5; };
	// GetNextLevelCosts
	//  Sets the input values to the costs of the skill for this level
	static void GetNextLevelCost ( Real& o_health, Real& o_stamina, Real& o_mana ) { o_health=0; o_stamina=0; o_mana=10; };*/
	static string GetIconFilename ( void ) { return "textures/icons/skill_blink.png"; }

protected:
	SKILL_DATA_DEFINE {
		SKILLDATA_DEFAULTS;
		data.eTopType		= WeaponItem::SkillActive;
		return data;
	}
protected:
	int			cast_state;
	ftype		release_timer;
	Vector3d	blink_start;
	Vector3d	blink_position;
	ftype		blink_timer;

	CParticleSystem*	ps_blink_target;
};

#endif//_SKILL_BLINK_H_