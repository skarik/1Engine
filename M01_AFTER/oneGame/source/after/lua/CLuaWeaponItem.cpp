
#include "CLuaWeaponItem.h"
#include "engine-common/lua/CLuaController.h"

CLuaWeaponItem::CLuaWeaponItem ( void )
	: CWeaponItem( ItemData() ), m_luaReady(false), m_enableKey(0)
{

}
CLuaWeaponItem::CLuaWeaponItem ( const char* entityName, const char* packageName )
	: CWeaponItem( ItemData() ), m_luaReady(false), m_enableKey(0)
{
	// Check length of inputs
	if ( strlen( entityName ) > 127 ) {
		throw Core::OutOfMemoryException();
	}
	if ( strlen( packageName ) > 95 ) {
		throw Core::OutOfMemoryException();
	}
	// Set package names for loading in
	m_entityName	= entityName;
	m_packageName	= packageName;
	// Create lua object
	Initialize();

	// This is performed in the Initialize routine
	//weaponItemData = ItemData();
	//weaponItemState.iCurrentStack = 1;
	//weaponItemState.sItemName = weaponItemData.sInfo;
}
// WeaponItem destruction.
CLuaWeaponItem::~CLuaWeaponItem ( void )
{
	Cleanup();
}
// Setup LuaWeaponItem entityName and packageName
void CLuaWeaponItem::SetEntityName ( const char* entityName, const char* packageName )
{
	// Check length of inputs
	if ( strlen( entityName ) > 127 ) {
		throw Core::OutOfMemoryException();
	}
	if ( strlen( packageName ) > 95 ) {
		throw Core::OutOfMemoryException();
	}
	// Set package names for loading in
	m_entityName	= entityName;
	m_packageName	= packageName;
	// Create lua object
	Initialize();
}

void CLuaWeaponItem::serialize ( Serializer & ser, const uint ver )
{
	CWeaponItem::serialize( ser, ver );

	// Save the entity and package names
	ser & m_entityName;
	std::string t_packageName = m_packageName;
	ser & t_packageName;
	m_packageName = t_packageName.c_str();

	// Something to do with save and load lol.

	// Setup this object now
	Initialize();
}
// LuaWeapon class copy operator
//	Needs to copy more data than a regular weapon (namely the Lua object data
CWeaponItem& CLuaWeaponItem::operator= ( const CWeaponItem * original ) 
{
	CWeaponItem::operator=(original);
	CLuaWeaponItem* lwep = (CLuaWeaponItem*)original;

	m_entityName = lwep->m_entityName;
	m_packageName = lwep->m_packageName;
	
	// Setup this object now
	Initialize();

	return *this;
}

// Load up Lua object
void CLuaWeaponItem::Initialize ( void )
{
	if ( !m_luaReady )
	{
		// Create environment value
		sprintf( m_environment.data, "LWEP_%p", this );
		// Shiet
		if ( m_packageName.length() > 0 ) {
		Lua::Controller->LoadLuaFile( std::string("../../addons/") + m_packageName.c_str() + "/lua/" + m_entityName.c_str() + ".lua", m_environment.c_str(), m_environment.c_str() );
		}
		else {
			Lua::Controller->LoadLuaFile( std::string("") + m_entityName.c_str() + ".lua", m_environment.c_str(), m_environment.c_str() );
		}
		lua_State* L = Lua::Controller->GetState();

		Lua::Controller->SetEnvironment( m_environment.c_str() );

		// Check the loaded Lua
		m_enableKey = 0;	// Clear enabled keys

		// Define lua check macro (very repetitive code)
#define checkLuaFunction(a,b) \
	lua_getfield( L, -1, a ); \
	if ( lua_isfunction( L, -1 ) ) { \
		m_enableKey |= b; \
	} \
	lua_pop(L,1);

		checkLuaFunction( "OnCreate", EK_OnCreate );
		checkLuaFunction( "OnDestroy", EK_OnDestroy );

		checkLuaFunction( "Update", EK_Update );
		checkLuaFunction( "LateUpdate", EK_LateUpdate );
		checkLuaFunction( "PostUpdate", EK_PostUpdate );
		checkLuaFunction( "FixedUpdate", EK_FixedUpdate );

		checkLuaFunction( "OnReceiveSignal", EK_OnReceiveSignal );

		checkLuaFunction( "Use", EK_OnUse );
		checkLuaFunction( "StartUse", EK_StartUse );
		checkLuaFunction( "EndUse", EK_EndUse );
		checkLuaFunction( "Attack", EK_Attack );
		checkLuaFunction( "Subdraw", EK_Subdraw );

		checkLuaFunction( "ItemData", EK_ItemData );
		checkLuaFunction( "GetIsBusy", EK_GetIsBusy );
		checkLuaFunction( "Interrupt", EK_Interrupt );

		checkLuaFunction( "SaveData", EK_SaveData );
		checkLuaFunction( "LoadData", EK_LoadData );

#undef checkLuaFunction

		Lua::Controller->ResetEnvironment();

		// Lua is ready! Mark it so
		m_luaReady = true;

		// Setup the ItemData now.
		weaponItemData = ItemData();
		weaponItemState.sItemName = weaponItemData.sInfo;
		// Now perform OnCreate again.
		OnCreate();
	}

}
void CLuaWeaponItem::Cleanup ( void )
{
	if ( m_luaReady )
	{
		// Fix this shit later
	}
}


void CLuaWeaponItem::OnCreate ( void )
{
	// This has the potential to be called twice
	if ( m_enableKey & EK_OnCreate )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "OnCreate",0 );
		Lua::Controller->ResetEnvironment();
	}
}
void CLuaWeaponItem::OnDestroy ( void )
{
	if ( m_enableKey & EK_OnDestroy )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "OnDestroy",0 );
		Lua::Controller->ResetEnvironment();
	}
}

void CLuaWeaponItem::Update ( void )
{
	if ( m_enableKey & EK_Update )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "Update",1, Lua::FN_DOUBLE, (double)Time::deltaTime );
		Lua::Controller->ResetEnvironment();
	}
}
void CLuaWeaponItem::LateUpdate ( void )
{
	CWeaponItem::LateUpdate();
	if ( m_enableKey & EK_LateUpdate )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "LateUpdate",1, Lua::FN_DOUBLE, (double)Time::deltaTime );
		Lua::Controller->ResetEnvironment();
	}
}
void CLuaWeaponItem::PostUpdate ( void )
{
	if ( m_enableKey & EK_PostUpdate )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "PostUpdate",1, Lua::FN_DOUBLE, (double)Time::deltaTime );
		Lua::Controller->ResetEnvironment();
	}
}

void CLuaWeaponItem::FixedUpdate ( void )
{
	CWeaponItem::FixedUpdate();
	if ( m_enableKey & EK_FixedUpdate )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "FixedUpdate",1, Lua::FN_DOUBLE, (double)Time::deltaTime );
		Lua::Controller->ResetEnvironment();
	}
}

void CLuaWeaponItem::OnReceiveSignal ( const uint64_t signal )
{
	if ( m_enableKey & EK_OnReceiveSignal )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "FixedUpdate",1, Lua::FN_INT, signal );
		Lua::Controller->ResetEnvironment();
	}
}



bool CLuaWeaponItem::Use ( int use )
{
	bool result = false;
	if ( m_enableKey & EK_OnUse )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		int returnValues = Lua::Controller->Call( "Use",1, Lua::FN_INT, use );

		if ( returnValues == 1 ) {
			result = lua_toboolean( L,-1 );	lua_pop( L,1 );
			Lua::Controller->PopValid( returnValues-1 );
		}
		else {
			Lua::Controller->PopValid( returnValues );
			Debug::Console->PrintError( "lweap::Use needs to return one value!\n" );
		}
		Lua::Controller->ResetEnvironment();
	}
	return result;
}
void CLuaWeaponItem::StartUse ( int use )
{
	if ( m_enableKey & EK_StartUse )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "StartUse",1, Lua::FN_INT, use );
		Lua::Controller->ResetEnvironment();
	}
}
void CLuaWeaponItem::EndUse ( int use )
{
	if ( m_enableKey & EK_EndUse )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "EndUse",1, Lua::FN_INT, use );
		Lua::Controller->ResetEnvironment();
	}
}
void CLuaWeaponItem::Attack ( XTransform& )
{
	if ( m_enableKey & EK_Attack )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "Attack", 0 );
		Lua::Controller->ResetEnvironment();
	}
}
void CLuaWeaponItem::SubDraw ( void )
{
	if ( m_enableKey & EK_Subdraw )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "SubDraw", 0 );
		Lua::Controller->ResetEnvironment();
	}
}


bool CLuaWeaponItem::GetIsBusy ( void ) const
{
	if ( m_enableKey & EK_OnUse )
	{
		bool result = false;
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		int returnValues = Lua::Controller->Call( "GetIsBusy", 0 );

		if ( returnValues == 1 ) {
			result = lua_toboolean( L,-1 );	lua_pop( L,1 );
			Lua::Controller->PopValid( returnValues-1 );
		}
		else {
			Lua::Controller->PopValid( returnValues );
			Debug::Console->PrintError( "lweap::Use needs to return one value!\n" );
		}
		Lua::Controller->ResetEnvironment();
		return result;
	}
	else
	{
		return CWeaponItem::GetIsBusy();
	}
}

void CLuaWeaponItem::Interrupt ( void )
{
	if ( m_enableKey & EK_Interrupt )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "Interrupt", 0 );
		Lua::Controller->ResetEnvironment();
	}
}


CWeaponItem::WItemData CLuaWeaponItem::ItemData ( void )
{
	WItemData data = CWeaponItem::ItemData();
	data.iItemId = 3;

	if ( m_enableKey & EK_ItemData )
	{
		bool result = false;
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		int returnValues = Lua::Controller->Call( "ItemData", 0 );

		if ( returnValues == 1 )
		{
			//result = lua_toboolean( L,-1 );	lua_pop( L,1 );

#define getTableValue(NAME,VALUE,FUNC,TYPE) \
	lua_getfield( L, -1, NAME ); \
	if ( !lua_isnil( L, -1 ) ) { \
		VALUE = (TYPE)FUNC( L, -1 ); \
	} \
	lua_pop(L,1);

			/*lua_getfield( L, -1, "info" );
			if ( !lua_isnil( L, -1 ) ) {
				data.sInfo = lua_tostring( L, -1 );
			}
			lua_pop(L,1);*/
			getTableValue( "info", data.sInfo, lua_tostring, char* );
			getTableValue( "canStack", data.bCanStack, lua_toboolean, bool );
			getTableValue( "maxStack", data.iMaxStack, lua_tointeger, short );

			getTableValue( "isSkill", data.bIsSkill, lua_toboolean, bool );
			getTableValue( "isTradable", data.bIsTradable, lua_toboolean, bool );

			getTableValue( "canDegrade", data.bCanDegrade, lua_toboolean, bool );
			getTableValue( "maxDurability", data.iMaxDurability, lua_tointeger, short );

			getTableValue( "itemType", data.eItemType, lua_tointeger, ItemType );
			getTableValue( "topType", data.eTopType, lua_tointeger, GenItemType );
			getTableValue( "itemRarity", data.eItemRarity, lua_tointeger, ItemRarity );

#undef getTableValue

			Lua::Controller->PopValid( returnValues-1 );
		}
		else {
			Lua::Controller->PopValid( returnValues );
			Debug::Console->PrintError( "lweap::ItemData needs to return one value!\n" );
			throw Core::NullReferenceException();
		}
		Lua::Controller->ResetEnvironment();
	}

	return data;
}

// Used in conjunction with ::serialize() to save and load extra Lua data
void CLuaWeaponItem::SaveData ( void )
{
	/*if ( m_enableKey & EK_SaveData )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "SaveData", 0 );
		Lua::Controller->ResetEnvironment();
	}*/
}
void CLuaWeaponItem::LoadData ( void )
{
	/*if ( m_enableKey & EK_LoadData )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "LoadData", 0 );
		Lua::Controller->ResetEnvironment();
	}*/
}
