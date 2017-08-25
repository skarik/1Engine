
#ifndef _C_LUA_SKILL_H_
#define _C_LUA_SKILL_H_

#include "after/entities/item/skill/CSkill.h"
#include <unordered_map>

class CLuaSkill : public CSkill
{
	ClassName( "LuaSkill" );
public:
	// LuaWeaponItem construction. It must have its entity set at some point before it is majorly used
	explicit CLuaSkill ( void );
	// LuaWeaponItem construction. It must have its entity given as it comes in
	explicit CLuaSkill ( const char* entityName, const char* packageName="" );
	// LuaWeaponItem destruction.
	~CLuaSkill ( void );
	// Setup LuaWeaponItem entityName and packageName
	void SetEntityName ( const char* entityName, const char* packageName="" );

	// Serialization system
	void			serialize ( Serializer &, const uint );
	// LuaWeapon class copy operator
	//	Needs to copy more data than a regular weapon
	CWeaponItem& operator= ( const CWeaponItem * original ) override;

	// Create and end calls
	void OnCreate ( void ) override; // This is actually called twice.
	void OnDestroy ( void ) override;

	// Update step
	void Update ( void ) override;
	// LateUpdate step. Calls the CWeaponItem::LateUpdate() (See CWeaponItem.h)
	void LateUpdate ( void ) override;
	// PostUpdate step.
	void PostUpdate ( void ) override;
	// FixedUpdate step. Calls the CWeaponItem::FixedUpdate() (See CWeaponItem.h)
	void FixedUpdate ( void ) override;

	// Signal reciever
	void OnReceiveSignal ( const uint64_t signal ) override;

	// Usage calls. See CWeaponItem.h for usage.
	bool Use ( int ) override;
	void StartUse ( int ) override;
	void EndUse ( int ) override;
	void Attack ( XTransform& ) override;
	void SubDraw ( void ) override;

	// Various Query calls. See CWeaponItem.h for usage.
	bool GetIsBusy ( void ) const override; // Calls CWeaponItem::GetIsBusy() for a result if not defined in Lua.
	void Interrupt ( void ) override;


	// Skill Combining
	// Returns if this skill is a modifier for the target item
	bool IsModifier ( const CWeaponItem* ) override;
	// SkillCombine. Combines two casting skills for an effect. Is called when both skills are casted and not channelling.
	// Return true if something happens. Generally, objects look through all the spells, and stop the skills by setting isCasting to false.
	// Returning true will normally stop non-instant skills.
	bool CastCombine ( CSkill* combineSkill ) override; // default return false
	// ChannelCombine. Combines two casting skills for an effect, except the callee is channelling.
	// Returns true if the skill doesn't block non-instant casts.
	// Returns false if the channelling skill should stop skills. Generally, all offensive magic spells are non-instants.
	bool ChannelCombine ( CSkill* callingSkill ) override; //default return true


	// Skill Properties
	//  (just hide this with child ones)

	// GetName
	//  Returns the name of the skill.
	static string GetName ( const char* entityName );
	// GetDescription
	//  Returns a description of the skill.
	static string GetDescription ( const char* entityName );
	// GetThisLevelInfo
	//  Returns the info of what this skill does this level
	static string GetThisLevelInfo ( const char* entityName, const int level );
	// GetNextLevelInfo
	//  Returns the info of what this skill does next level
	static string GetNextLevelInfo ( const char* entityName, const int level );
	// GetThisLevelCosts
	//  Sets the input values to the costs of the skill for this level
	static void GetThisLevelCost ( const char* entityName, const int level, Real& o_health, Real& o_stamina, Real& o_mana );
	// GetNextLevelCosts
	//  Sets the input values to the costs of the skill for this level
	static void GetNextLevelCost ( const char* entityName, const int level, Real& o_health, Real& o_stamina, Real& o_mana );
	// GetIconFilename
	//  Returns the filename of the icon
	static string GetIconFilename ( const char* entityName );

	// GetIsPassive
	//  Returns if skill is passive. Static skills cannot be grabbed from the skill tree.
	static bool GetIsPassive ( const char* entityName );

	// DoPassiveSet
	//  Passive code performed when the skill is loaded up.
	static void DoPassiveSet ( const char* entityName, CCharacter* character, const int level );
protected:
	WItemData ItemData ( void ) override;

	// Used in conjunction with ::serialize() to save and load extra Lua data
	void SaveData ( void );
	void LoadData ( void );

	// Used to store skill properties
	struct skillProperty_t {
		string	name;
		string	description;
		string	icon;
		bool	passive;
	};
	static std::unordered_map<string,skillProperty_t> m_datamap;
	struct staticProperty_t {
		string		env;
		uint32_t	key;
	};
	static std::unordered_map<string,staticProperty_t> m_staticmap;

protected:
	int				m_lastLevel;
	bool			m_luaReady;
	// Load up Lua object
	void			Initialize ( void );
	void			Cleanup ( void );

	static void		StaticInitialize ( const char* entityName );

protected:
	arstring<128>	m_entityName;	// Lua entity folder
	arstring<96>	m_packageName;	// Mod directory or otherwise
	arstring<30>	m_environment;	// Object environment

	enum eEnableKey_t : uint32_t
	{
		EK_OnCreate			= 0x1,
		EK_OnDestroy		= 0x2,
		EK_Update			= 0x4,
		EK_LateUpdate		= 0x8,
		EK_PostUpdate		= 0x10,
		EK_FixedUpdate		= 0x20,
		EK_OnReceiveSignal	= 0x40,

		EK_OnUse			= 0x80,
		EK_StartUse			= 0x100,
		EK_EndUse			= 0x200,
		EK_Attack			= 0x400,
		EK_Subdraw			= 0x800,

		EK_ItemData			= 0x1000,
		EK_GetIsBusy		= 0x2000,
		EK_Interrupt		= 0x4000,

		EK_SaveData			= 0x8000,
		EK_LoadData			= 0x10000,

		EK_IsModifier		= 0x20000,
		EK_CastCombine		= 0x40000,
		EK_ChannelCombine	= 0x80000

		/*EK_SkGetName			= 0x100000,
		EK_SkGetDescription		= 0x200000,
		EK_SkGetThisLevelInfo	= 0x400000,
		EK_SkGetNextLevelInfo	= 0x800000,
		EK_SkGetThisLevelCost	= 0x1000000,
		EK_SkGetNextLevelCost	= 0x2000000,
		EK_SkGetIconFilename	= 0x4000000,
		EK_SkGetIsPassive		= 0x8000000,

		EK_SkGetPassiveStart	= 0x10000000,*/
	};
	uint32_t		m_enableKey;

	enum eStaticEnableKey_t : uint32_t
	{
		EK_SkGetThisLevelInfo	= 0x1,
		EK_SkGetNextLevelInfo	= 0x2,
		EK_SkGetThisLevelCost	= 0x4,
		EK_SkGetNextLevelCost	= 0x8,
		EK_SkGetPassiveStart	= 0x10,

		EK_SkSkillProperties	= 0x20
	};
	//static uint32_t	m_staticEnableKey;


};


#endif//_C_LUA_SKILL_H_