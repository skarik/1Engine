
#ifndef _SKILL_HAIL_OF_ARROWS_H_
#define _SKILL_HAIL_OF_ARROWS_H_

#include "after/entities/item/skill/CSkill.h"

class CBaseRandomBow;
class CRenderPlane;

class SkillHailOfArrows : public CSkill
{
public:
	// Constructor
				SkillHailOfArrows ( void );
				~SkillHailOfArrows ( void );

	// Item cast
	bool		Use ( int ) override;

	// Is modifier for bow items
	bool		IsModifier ( const CWeaponItem* ) override;

	void		Update (void ) override;

public:
	static string GetName ( void ) { return "Hail of Arrows"; }
	static string GetIconFilename ( void ) { return ".res/textures/icons/skill_hailofarrows.png"; }
protected:
	SKILL_DATA_DEFINE {
		SKILLDATA_DEFAULTS;
		data.eTopType		= WeaponItem::SkillActive;

		data.iHands			= 0;
		return data;
	}
protected:
	CBaseRandomBow*		mBow;
	CRenderPlane*		mAoeModel;

	ftype	mAoeFadeTime;
	ftype	mAoeAlpha;
};

#endif//_SKILL_HAIL_OF_ARROWS_H_