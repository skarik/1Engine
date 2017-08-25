

#ifndef _C_LUA_WEAPON_ITEM_
#define _C_LUA_WEAPON_ITEM_

#include "after/entities/item/CWeaponItem.h"

class CLuaWeaponItem : public CWeaponItem
{
	ClassName( "LuaWeaponItem" );
public:
	// LuaWeaponItem construction. It must have its entity set at some point before it is majorly used
	explicit CLuaWeaponItem ( void );
	// LuaWeaponItem construction. It must have its entity given as it comes in
	explicit CLuaWeaponItem ( const char* entityName, const char* packageName="" );
	// LuaWeaponItem destruction.
	~CLuaWeaponItem ( void );
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

protected:
	WItemData ItemData ( void ) override;

	// Used in conjunction with ::serialize() to save and load extra Lua data
	void SaveData ( void );
	void LoadData ( void );


protected:
	bool			m_luaReady;
	// Load up Lua object
	void			Initialize ( void );
	void			Cleanup ( void );

protected:
	arstring<128>	m_entityName;	// Lua entity folder
	arstring<96>	m_packageName;	// Mod directory or otherwise
	arstring<30>	m_environment;	// Object environment

	enum eEnableKey_t
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
		EK_LoadData			= 0x10000
	};
	uint32_t		m_enableKey;
};


#endif