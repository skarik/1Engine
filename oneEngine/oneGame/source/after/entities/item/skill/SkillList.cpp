
#include "SkillList.h"
#include "after/entities/item/CWeaponItemFactory.h"

#include "after/lua/CLuaSkill.h"

void WeaponItem::CWeaponItemFactory::RegisterSkillTypes ( void )
{
	// -1 is reserved for system INVALID
	RegisterSkill( CLuaSkill, -3 );

	RegisterSkill( SkillBlink, -10 );
	RegisterSkill( SkillMagikinesis, -11 );
	RegisterSkill( SkillFiretoss, -12 );
	RegisterSkill( SkillMagicBolt, -13 );
	RegisterSkill( SkillFrostBall, -14 );

	RegisterSkill( SkillFireArrow, -20 );
	RegisterSkill( SkillSplitshot, -21 );
	RegisterSkill( SkillReflectshot, -22 );
	RegisterSkill( SkillHailOfArrows, -23 );

	RegisterSkill( SkillShield, -42 );
	RegisterSkill( SkillPressureBlast, -52);
}