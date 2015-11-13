#ifndef _SKILL_PRESSURE_BLAST_H_
#define _SKILL_PRESSURE_BLAST_H_

#include "after/entities/item/skill/CSkill.h"

class glMaterial;
class CParticleSystem;

class SkillPressureBlast : public CSkill
{
public:
	// Constructor
				SkillPressureBlast ( void );
				~SkillPressureBlast ( void );

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
	static string GetName ( void ) { return "Pressure Blast"; }
	//static string GetIconFilename ( void ) { return ".res/textures/icons/skill_iceball.png"; }
protected:
	SKILL_DATA_DEFINE {
		SKILLDATA_DEFAULTS;
		data.eTopType		= WeaponItem::SkillActive;
		return data;
	}
protected:
	CParticleSystem*	ps_waterblast_active;
};

#endif//_SKILL_PRESSURE_BLAST_H_