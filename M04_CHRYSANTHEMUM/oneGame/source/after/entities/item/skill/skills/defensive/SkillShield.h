
#ifndef _SKILL_SHIELD_H_
#define _SKILL_SHIELD_H_

// Shield is the center of defensive magic combat.

#include "after/entities/item/skill/CSkill.h"

class SkillShield : public CSkill
{
public:
	// Constructor
				SkillShield ( void );
				~SkillShield ( void );

	// Item cast
	bool		Use ( int ) override;
	void		EndUse ( int ) override;
	// Item sub-cast
	void		Update ( void ) override;

public:
	static string GetName ( void ) { return "Shield"; }
	static string GetIconFilename ( void ) { return ".res/textures/icons/skill_magishield.png"; }
protected:
	SKILL_DATA_DEFINE {
		SKILLDATA_DEFAULTS;
		data.eTopType		= WeaponItem::SkillActive;
		return data;
	}

public:
	// Shield Aspect
	struct aspect_t
	{
		//CSkill*		source;
		short		source;
		uint64_t	element;
		ftype		strength;

		aspect_t ( void ) 
			: source(NIL), element(0), strength(1)
		{
			;
		}
	};

	// Run through damages
	Damage		OnDamaged ( const Damage& inDamage, DamageFeedback* outFeedback );

	// ChannelCombine. Combines two casting skills for an effect, except the callee is channelling.
	// Returns true if the skill doesn't block non-instant casts.
	// Returns false if the channelling skill should stop skills. Generally, all offensive magic spells are non-instants.
	bool ChannelCombine ( CSkill* callingSkill ) override;
protected:
	// Shield stats
	bool					m_shielding;
	ftype					m_health;
	ftype					m_max_health;
	std::vector<aspect_t>	m_aspects;

	// Shield effects
	CModel*		m_model;

	void ShieldEffectUpdate ( void );
	void ShieldEffectDestroy( void );
};


#endif//_SKILL_SHIELD_H_