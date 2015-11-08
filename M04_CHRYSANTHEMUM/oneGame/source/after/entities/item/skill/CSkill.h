#ifndef _C_SKILL_H_
#define _C_SKILL_H_

#include "after/entities/item/CWeaponItem.h"
#include "after/types/Disciplines.h"
#include "core-ext/system/io/serializer.h"

class CTexture;

class CSkill : public CWeaponItem
{
	ClassName( "AbstractSkillItem" );
protected:
	typedef WItemData SkillData;
public:
	// Constructor for initializing default values
	// This NEEDS to be called by derived classes.
	// This is done by calling this function in the initializer list. For example, if a class were
	//  named WeaponP90, the constructor definition should look like the following:
	//	SkillBlink::SkillBlink ( void ) : CSkill( ItemData() )
	// This initializes the class's "system" values to an initial value, as well as performing certain initializing code
	explicit		CSkill ( const SkillData & );
	// Destructor frees any model, collider, or rigidbody in use by the instance.
	// The means that while the variables pBody, pCollider, and pModel are free for use in derived classes,
	//  keep in mind that this base class retains ownership of the pointers.
					~CSkill ( void );
	// Serialization system
	void			serialize ( Serializer &, const uint );
	// Skill class copy operator
	//	Needs to copy more data than a regular weapon
	CWeaponItem& operator= ( const CWeaponItem * original ) override;

	// Function for initializing the collision/physics
	// This essentially replaces the default physics creation with no physics creation.
	void			CreatePhysics ( void ) override;
	
	// Override update behavior.
	// Passives are applied
	void			Update ( void ) override;

	// Sets the skill level
	void			SetLevel ( const int );

	// This is called when the HUD for the current weapon needs to be drawn.
	void			OnDrawHUD ( const WReticleStyle& n_reticleStyle ) override;
	// Grabs the icon for this object
	CTexture*		GetInventoryIcon ( void ) override;
	// Draws the weapon tooltip overlay
	void			DrawTooltip ( const Vector2d &, const WTooltipStyle & ) override;

	// Returns the discipline
	Discipline		GetDiscipline ( const int i ) { return discipline[i]; };
	// Returns if this skill is a modifier for the target item
	virtual bool	IsModifier ( const CWeaponItem* ) { return false; }

	// Checks if has enough mana
	bool			CanCast ( void );
	// Checks if is casting
	bool			IsCasting ( void ) { return isCasting; }
	// Sets casting state to false
	void			StopCasting ( void ) { isCasting = false; }
	// Checks if is channelling
	bool			IsChannelling ( void ) { return isChannelling; }
	// Return the spell's current level
	int				GetLevel(void);

	// Skill Combining

	// SkillCombine. Combines two casting skills for an effect. Is called when both skills are casted and not channelling.
	// Return true if something happens. Generally, objects look through all the spells, and stop the skills by setting isCasting to false.
	// Returning true will normally stop non-instant skills.
	virtual bool CastCombine ( CSkill* combineSkill ) { return false; }
	// ChannelCombine. Combines two casting skills for an effect, except the callee is channelling.
	// Returns true if the skill doesn't block non-instant casts.
	// Returns false if the channelling skill should stop skills. Generally, all offensive magic spells are non-instants.
	virtual bool ChannelCombine ( CSkill* callingSkill ) { return true; }


	// Skill Properties
	//  (just hide this with child ones)

	// GetName
	//  Returns the name of the skill.
	static string GetName ( void ) { return "Skill"; }
	// GetDescription
	//  Returns a description of the skill.
	static string GetDescription ( void ) { return "A skill. Undefined (base class)."; }
	// GetThisLevelInfo
	//  Returns the info of what this skill does this level
	static string GetThisLevelInfo ( int level ) { return "Baller."; }
	// GetNextLevelInfo
	//  Returns the info of what this skill does next level
	static string GetNextLevelInfo ( int level ) { return "More baller."; }
	// GetThisLevelCosts
	//  Sets the input values to the costs of the skill for this level
	static void GetThisLevelCost ( int level, Real& o_health, Real& o_stamina, Real& o_mana ) { o_health=0; o_stamina=0; o_mana=5; };
	// GetNextLevelCosts
	//  Sets the input values to the costs of the skill for this level
	static void GetNextLevelCost ( int level, Real& o_health, Real& o_stamina, Real& o_mana ) { o_health=0; o_stamina=0; o_mana=10; };
	// GetIconFilename
	//  Returns the filename of the icon
	static string GetIconFilename ( void ) { return ".res/textures/icons/skill_default.png"; }

	// GetIsPassive
	//  Returns if skill is passive. Static skills cannot be grabbed from the skill tree.
	static bool GetIsPassive ( void ) { return false; }
	// DoPassiveSet
	//  Passive code performed when the skill is loaded up.
	static void DoPassiveSet ( CCharacter* character, const int level ) { ; }
protected:
	// Passive code
	//virtual void	DoPassive ( void ) {;}

	// Skill states
	bool	isCasting;
	bool	isChannelling;

	// Skill specific data
	Real	mana_cost;
	Real	stamina_cost;
	Real	health_cost;
	ftype	cooldown;
	int		skill_lvl;
	bool	passive;
	string	icon_filename;

	DisciplineAreas	disciplineArea [2];
	Discipline		discipline [2];
	SubDisciplines	subDiscipline [2];

	// This defines the default item data for skills.
	WITEM_DATA_DEFINE
	{
		WITEMDATA_DEFAULTS;

		wpdata.iItemId = -1;

		wpdata.sInfo		= "";
		wpdata.bCanStack	= false;
		wpdata.bIsSkill		= true;
		wpdata.bIsTradable	= false;

		wpdata.eItemType	= WeaponItem::TypeDefault;
		wpdata.eTopType		= WeaponItem::SkillDefault;
		wpdata.eItemRarity	= WeaponItem::RarityNone;

		wpdata.fWeight = 0.0f;

		discipline[0] = DscNone;
		discipline[1] = DscNone;

		return wpdata;
	};
#define SKILL_DATA_DEFINE protected: virtual WItemData ItemData ( void )
#define SKILLDATA_DEFAULTS WItemData data = CSkill::ItemData(); \
		data.sIconOverride = GetIconFilename().c_str(); \
		data.sInfo = GetName().c_str();

	// Subtracts mana
	void			CastMana ( void );
	// Returns the focus color
	Color			GetFocusColor ( void );

private:
	CTexture*	tex_icon;

};

#endif//_C_SKILL_H_