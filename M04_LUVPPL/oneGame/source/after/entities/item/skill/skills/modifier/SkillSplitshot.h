
#ifndef _SKILL_SPLITSHOT_H_
#define _SKILL_SPLITSHOT_H_

#include "after/entities/item/skill/CSkill.h"

class CParticleSystem;
class CBaseRandomBow;

class SkillSplitshot : public CSkill
{
public:
	// Constructor
				SkillSplitshot ( void );
				~SkillSplitshot ( void );

	// Item cast
	bool		Use ( int ) override;

	// Is modifier for bow items
	bool		IsModifier ( const CWeaponItem* ) override;

public:
	static string GetName ( void ) { return "Splitshot"; }
	//static string GetIconFilename ( void ) { return ".res/textures/icons/skill_firetoss.png"; }
protected:
	SKILL_DATA_DEFINE {
		SKILLDATA_DEFAULTS;
		data.eTopType		= WeaponItem::SkillActive;

		data.iHands			= 0;
		return data;
	}
protected:
	CBaseRandomBow*		mBow;
};

#endif//_SKILL_SPLITSHOT_H_