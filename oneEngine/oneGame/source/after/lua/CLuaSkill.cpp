
#include "CLuaSkill.h"
#include "engine-common/lua/CLuaController.h"
#include "after/entities/item/CWeaponItem.h"

CLuaSkill::CLuaSkill ( void )
	: m_luaReady(false), m_enableKey(0), m_lastLevel(-10), CSkill( CSkill::ItemData() )
{

}
CLuaSkill::CLuaSkill ( const char* entityName, const char* packageName )
	: m_luaReady(false), m_enableKey(0), m_lastLevel(-10), CSkill( CSkill::ItemData() )
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
CLuaSkill::~CLuaSkill ( void )
{
	Cleanup();
}
// Setup LuaWeaponItem entityName and packageName
void CLuaSkill::SetEntityName ( const char* entityName, const char* packageName )
{
	// Reset level value to force Update
	m_lastLevel = -10;
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

void CLuaSkill::serialize ( Serializer & ser, const uint ver )
{
	CSkill::serialize( ser, ver );

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
CWeaponItem& CLuaSkill::operator= ( const CWeaponItem * original ) 
{
	CWeaponItem::operator=(original);
	CLuaSkill* lwep = (CLuaSkill*)original;

	m_entityName = lwep->m_entityName;
	m_packageName = lwep->m_packageName;
	
	// Setup this object now
	Initialize();

	return *this;
}

// Load up Lua object
void CLuaSkill::Initialize ( void )
{
	if ( !m_luaReady )
	{
		// Create environment value
		sprintf( m_environment.data, "LWEP_%x", this );
		// Shiet
		if ( m_packageName.length() > 0 ) {
			Lua::Controller->LoadLuaFile( std::string("../../addons/skills/") + m_packageName.c_str() + "/lua/" + m_entityName.c_str() + ".lua", m_environment.c_str(), m_environment.c_str() );
		}
		else {
			Lua::Controller->LoadLuaFile( std::string("skills/") + m_entityName.c_str() + ".lua", m_environment.c_str(), m_environment.c_str() );
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

		// Set up the skill's static data (if it's not done already)
		StaticInitialize( m_entityName );

		// Lua is ready! Mark it so
		m_luaReady = true;
		// Reset for update!
		m_lastLevel = -10;

		// Setup the icon data
		icon_filename = GetIconFilename( m_entityName );
		// Setup the ItemData now.
		weaponItemData = ItemData();
		weaponItemState.sItemName = weaponItemData.sInfo;
		// Now perform OnCreate again.
		OnCreate();
	}

}
void CLuaSkill::Cleanup ( void )
{
	if ( m_luaReady )
	{
		// Fix this shit later
	}
}


void CLuaSkill::OnCreate ( void )
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
void CLuaSkill::OnDestroy ( void )
{
	if ( m_enableKey & EK_OnDestroy )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "OnDestroy",0 );
		Lua::Controller->ResetEnvironment();
	}
}

void CLuaSkill::Update ( void )
{
	CSkill::Update();
	if ( m_enableKey & EK_Update )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "Update",1, Lua::FN_DOUBLE, (double)Time::deltaTime );
		Lua::Controller->ResetEnvironment();
	}

	if ( m_lastLevel != GetLevel() )
	{
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->SetEnvironment( m_environment.c_str() );

#define getTableValue(NAME,VALUE,FUNC,TYPE) \
	lua_getfield( L, -1, NAME ); \
	if ( !lua_isnil( L, -1 ) ) { \
		VALUE = (TYPE)FUNC( L, -1 ); \
	} \
	lua_pop(L,1);

		getTableValue( "cooldown", cooldown, lua_tonumber, Real );
		getTableValue( "anim_subset", iAnimationSubset, lua_tointeger, int );
		getTableValue( "anim_type", holdType, lua_tointeger, Item::HoldType );

		lua_pushinteger( L, GetLevel() );
		lua_setfield( L, -2, "level" );

#undef getTableValue
	
		Lua::Controller->ResetEnvironment();

		GetThisLevelCost( m_entityName, GetLevel(), health_cost, stamina_cost, mana_cost );
		m_lastLevel = GetLevel();
	}

}
void CLuaSkill::LateUpdate ( void )
{
	CSkill::LateUpdate();
	if ( m_enableKey & EK_LateUpdate )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "LateUpdate",1, Lua::FN_DOUBLE, (double)Time::deltaTime );
		Lua::Controller->ResetEnvironment();
	}
}
void CLuaSkill::PostUpdate ( void )
{
	if ( m_enableKey & EK_PostUpdate )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "PostUpdate",1, Lua::FN_DOUBLE, (double)Time::deltaTime );
		Lua::Controller->ResetEnvironment();
	}
}

void CLuaSkill::FixedUpdate ( void )
{
	CSkill::FixedUpdate();
	if ( m_enableKey & EK_FixedUpdate )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "FixedUpdate",1, Lua::FN_DOUBLE, (double)Time::deltaTime );
		Lua::Controller->ResetEnvironment();
	}
}

void CLuaSkill::OnReceiveSignal ( const uint64_t signal )
{
	if ( m_enableKey & EK_OnReceiveSignal )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "FixedUpdate",1, Lua::FN_INT, signal );
		Lua::Controller->ResetEnvironment();
	}
}



bool CLuaSkill::Use ( int use )
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
void CLuaSkill::StartUse ( int use )
{
	if ( m_enableKey & EK_StartUse )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "StartUse",1, Lua::FN_INT, use );
		Lua::Controller->ResetEnvironment();
	}
}
void CLuaSkill::EndUse ( int use )
{
	if ( m_enableKey & EK_EndUse )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "EndUse",1, Lua::FN_INT, use );
		Lua::Controller->ResetEnvironment();
	}
}
void CLuaSkill::Attack ( XTransform& )
{
	if ( m_enableKey & EK_Attack )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "Attack", 0 );
		Lua::Controller->ResetEnvironment();
	}
}
void CLuaSkill::SubDraw ( void )
{
	if ( m_enableKey & EK_Subdraw )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "SubDraw", 0 );
		Lua::Controller->ResetEnvironment();
	}
}


bool CLuaSkill::GetIsBusy ( void ) const
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

void CLuaSkill::Interrupt ( void )
{
	if ( m_enableKey & EK_Interrupt )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "Interrupt", 0 );
		Lua::Controller->ResetEnvironment();
	}
}

// Returns if this skill is a modifier for the target item
bool CLuaSkill::IsModifier ( const CWeaponItem* nswep )
{
	if ( m_enableKey & EK_IsModifier )
	{
		bool result = false;
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		int returnValues = Lua::Controller->Call( "IsModifier", 1, Lua::FN_USERDATA, nswep );

		if ( returnValues == 1 ) {
			result = lua_toboolean( L,-1 );	lua_pop( L,1 );
			Lua::Controller->PopValid( returnValues-1 );
		}
		else {
			Lua::Controller->PopValid( returnValues );
			Debug::Console->PrintError( "lskil::IsModifier needs to return one value!\n" );
		}
		Lua::Controller->ResetEnvironment();
		return result;
	}
	else
	{
		return false;
	}
}
// SkillCombine. Combines two casting skills for an effect. Is called when both skills are casted and not channelling.
// Return true if something happens. Generally, objects look through all the spells, and stop the skills by setting isCasting to false.
// Returning true will normally stop non-instant skills.
bool CLuaSkill::CastCombine ( CSkill* combineSkill ) // default return false
{
	if ( m_enableKey & EK_CastCombine )
	{
		bool result = false;
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		int returnValues = Lua::Controller->Call( "CastCombine", 1, Lua::FN_USERDATA, combineSkill );

		if ( returnValues == 1 ) {
			result = lua_toboolean( L,-1 );	lua_pop( L,1 );
			Lua::Controller->PopValid( returnValues-1 );
		}
		else {
			Lua::Controller->PopValid( returnValues );
			Debug::Console->PrintError( "lskil::IsModifier needs to return one value!\n" );
		}
		Lua::Controller->ResetEnvironment();
		return result;
	}
	else
	{
		return false;
	}
}
// ChannelCombine. Combines two casting skills for an effect, except the callee is channelling.
// Returns true if the skill doesn't block non-instant casts.
// Returns false if the channelling skill should stop skills. Generally, all offensive magic spells are non-instants.
bool CLuaSkill::ChannelCombine ( CSkill* callingSkill ) //default return true
{
	if ( m_enableKey & EK_ChannelCombine )
	{
		bool result = false;
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		int returnValues = Lua::Controller->Call( "ChannelCombine", 1, Lua::FN_USERDATA, callingSkill );

		if ( returnValues == 1 ) {
			result = lua_toboolean( L,-1 );	lua_pop( L,1 );
			Lua::Controller->PopValid( returnValues-1 );
		}
		else {
			Lua::Controller->PopValid( returnValues );
			Debug::Console->PrintError( "lskil::IsModifier needs to return one value!\n" );
		}
		Lua::Controller->ResetEnvironment();
		return result;
	}
	else
	{
		return true;
	}
}




CWeaponItem::WItemData CLuaSkill::ItemData ( void )
{
	WItemData data = CSkill::ItemData();
	data.iItemId = -3;
	data.bIsSkill = true;
	data.bIsTradable = false;
	data.bCanStack = false;
	data.bCanDegrade = false;

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
			//getTableValue( "canStack", data.bCanStack, lua_toboolean, bool );
			//getTableValue( "maxStack", data.iMaxStack, lua_tointeger, short );

			//getTableValue( "isSkill", data.bIsSkill, lua_toboolean, bool );
			//getTableValue( "isTradable", data.bIsTradable, lua_toboolean, bool );

			//getTableValue( "canDegrade", data.bCanDegrade, lua_toboolean, bool );
			//getTableValue( "maxDurability", data.iMaxDurability, lua_tointeger, short );

			//getTableValue( "itemType", data.eItemType, lua_tointeger, ItemType );
			//getTableValue( "topType", data.eTopType, lua_tointeger, GenItemType );
			//getTableValue( "itemRarity", data.eItemRarity, lua_tointeger, ItemRarity );

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

	// Set the passive/active item top type
	if ( GetIsPassive( this->m_entityName ) ) {
		data.eTopType = WeaponItem::SkillPassive;
	}
	else {
		data.eTopType = WeaponItem::SkillActive;
	}

	return data;
}

// Used in conjunction with ::serialize() to save and load extra Lua data
void CLuaSkill::SaveData ( void )
{
	/*if ( m_enableKey & EK_SaveData )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "SaveData", 0 );
		Lua::Controller->ResetEnvironment();
	}*/
}
void CLuaSkill::LoadData ( void )
{
	/*if ( m_enableKey & EK_LoadData )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "LoadData", 0 );
		Lua::Controller->ResetEnvironment();
	}*/
}



// Skill Properties
//  (just hide this with child ones)

// GetName
//  Returns the name of the skill.
string CLuaSkill::GetName ( const char* entityName )
{
	auto dataResult = m_datamap.find( entityName );
	if ( dataResult == m_datamap.end() ) {
		// Add data result to the list (by setting up the Lua)
		StaticInitialize(entityName);
		// Locate it again
		dataResult = m_datamap.find( entityName );
	}

	return dataResult->second.name;
}
// GetDescription
//  Returns a description of the skill.
string CLuaSkill::GetDescription ( const char* entityName )
{
	auto dataResult = m_datamap.find( entityName );
	if ( dataResult == m_datamap.end() ) {
		// Add data result to the list (by setting up the Lua)
		StaticInitialize(entityName);
		// Locate it again
		dataResult = m_datamap.find( entityName );
	}

	return dataResult->second.description;
}
// GetThisLevelInfo
//  Returns the info of what this skill does this level
string CLuaSkill::GetThisLevelInfo ( const char* entityName, const int level )
{
	auto envResult = m_staticmap.find( entityName );
	if ( envResult == m_staticmap.end() ) {
		// Add data result to the list (by setting up the Lua)
		StaticInitialize(entityName);
		// Locate it again
		envResult = m_staticmap.find( entityName );
	}

	string result = "this level info";
	if ( envResult->second.key & EK_SkGetThisLevelInfo )
	{
		Lua::Controller->SetEnvironment( envResult->second.env.c_str() );
		lua_State* L = Lua::Controller->GetState();
		// Call lua info with input level
		int returnValues = Lua::Controller->Call( "GetThisLevelInfo",1, Lua::FN_INT, level );
		// Check the return values
		if ( returnValues == 1 ) {
			result = lua_tostring( L,-1 );	lua_pop( L,1 );
			Lua::Controller->PopValid( returnValues-1 );
		}
		else {
			Lua::Controller->PopValid( returnValues );
			Debug::Console->PrintError( "static lskl::GetThisLevelInfo needs to return one value!\n" );
		}
		Lua::Controller->ResetEnvironment();
	}
	return result;
}
// GetNextLevelInfo
//  Returns the info of what this skill does next level
string CLuaSkill::GetNextLevelInfo ( const char* entityName, const int level )
{
	auto envResult = m_staticmap.find( entityName );
	if ( envResult == m_staticmap.end() ) {
		// Add data result to the list (by setting up the Lua)
		StaticInitialize(entityName);
		// Locate it again
		envResult = m_staticmap.find( entityName );
	}

	string result = "next level info";
	if ( envResult->second.key & EK_SkGetNextLevelInfo )
	{
		Lua::Controller->SetEnvironment( envResult->second.env.c_str() );
		lua_State* L = Lua::Controller->GetState();
		// Call lua info with input level
		int returnValues = Lua::Controller->Call( "GetNextLevelInfo",1, Lua::FN_INT, level );
		// Check the return values
		if ( returnValues == 1 ) {
			result = lua_tostring( L,-1 );	lua_pop( L,1 );
			Lua::Controller->PopValid( returnValues-1 );
		}
		else {
			Lua::Controller->PopValid( returnValues );
			Debug::Console->PrintError( "static lskl::GetNextLevelInfo needs to return one value!\n" );
		}
		Lua::Controller->ResetEnvironment();
	}
	return result;
}
// GetThisLevelCosts
//  Sets the input values to the costs of the skill for this level
void CLuaSkill::GetThisLevelCost ( const char* entityName, const int level, Real& o_health, Real& o_stamina, Real& o_mana )
{
	auto envResult = m_staticmap.find( entityName );
	if ( envResult == m_staticmap.end() ) {
		// Add data result to the list (by setting up the Lua)
		StaticInitialize(entityName);
		// Locate it again
		envResult = m_staticmap.find( entityName );
	}

	if ( envResult->second.key & EK_SkGetThisLevelCost )
	{
		Lua::Controller->SetEnvironment( envResult->second.env.c_str() );
		lua_State* L = Lua::Controller->GetState();
		// Call lua info with input level
		int returnValues = Lua::Controller->Call( "GetThisLevelCost",1, Lua::FN_INT, level );
		// Check the return values
		if ( returnValues == 3 ) {
			o_mana		= (Real)lua_tonumber( L,-1 );	lua_pop( L,1 );
			o_stamina	= (Real)lua_tonumber( L,-1 );	lua_pop( L,1 );
			o_health	= (Real)lua_tonumber( L,-1 );	lua_pop( L,1 );
			Lua::Controller->PopValid( returnValues-1 );
		}
		else {
			Lua::Controller->PopValid( returnValues );
			Debug::Console->PrintError( "static lskl::GetThisLevelCost needs to return three values!\n" );
			CSkill::GetThisLevelCost( level, o_health, o_stamina, o_mana );
		}
		Lua::Controller->ResetEnvironment();
	}
	else
	{
		CSkill::GetThisLevelCost( level, o_health, o_stamina, o_mana );
	}
}
// GetNextLevelCosts
//  Sets the input values to the costs of the skill for this level
void CLuaSkill::GetNextLevelCost ( const char* entityName, const int level, Real& o_health, Real& o_stamina, Real& o_mana )
{
	auto envResult = m_staticmap.find( entityName );
	if ( envResult == m_staticmap.end() ) {
		// Add data result to the list (by setting up the Lua)
		StaticInitialize(entityName);
		// Locate it again
		envResult = m_staticmap.find( entityName );
	}

	if ( envResult->second.key & EK_SkGetNextLevelCost )
	{
		Lua::Controller->SetEnvironment( envResult->second.env.c_str() );
		lua_State* L = Lua::Controller->GetState();
		// Call lua info with input level
		int returnValues = Lua::Controller->Call( "GetNextLevelCost",1, Lua::FN_INT, level );
		// Check the return values
		if ( returnValues == 1 ) {
			o_mana		= (Real)lua_tonumber( L,-1 );	lua_pop( L,1 );
			o_stamina	= (Real)lua_tonumber( L,-1 );	lua_pop( L,1 );
			o_health	= (Real)lua_tonumber( L,-1 );	lua_pop( L,1 );
			Lua::Controller->PopValid( returnValues-1 );
		}
		else {
			Lua::Controller->PopValid( returnValues );
			Debug::Console->PrintError( "static lskl::GetNextLevelCost needs to return one value!\n" );
		}
		Lua::Controller->ResetEnvironment();
	}
	else
	{
		CSkill::GetNextLevelCost( level, o_health, o_stamina, o_mana );
	}
}
// GetIconFilename
//  Returns the filename of the icon
string CLuaSkill::GetIconFilename ( const char* entityName )
{
	auto dataResult = m_datamap.find( entityName );
	if ( dataResult == m_datamap.end() ) {
		// Add data result to the list (by setting up the Lua)
		StaticInitialize(entityName);
		// Locate it again
		dataResult = m_datamap.find( entityName );
	}

	return dataResult->second.icon;
}

// GetIsPassive
//  Returns if skill is passive. Static skills cannot be grabbed from the skill tree.
bool CLuaSkill::GetIsPassive ( const char* entityName )
{
	auto dataResult = m_datamap.find( entityName );
	if ( dataResult == m_datamap.end() ) {
		// Add data result to the list (by setting up the Lua)
		StaticInitialize(entityName);
		// Locate it again
		dataResult = m_datamap.find( entityName );
	}

	return dataResult->second.passive;
}

// DoPassiveSet
//  Passive code performed when the skill is loaded up.
void CLuaSkill::DoPassiveSet ( const char* entityName, CCharacter* character, const int level )
{
	auto envResult = m_staticmap.find( entityName );
	if ( envResult == m_staticmap.end() ) {
		// Add data result to the list (by setting up the Lua)
		StaticInitialize(entityName);
		// Locate it again
		envResult = m_staticmap.find( entityName );
	}

	if ( envResult->second.key & EK_SkGetPassiveStart )
	{
		Lua::Controller->SetEnvironment( envResult->second.env.c_str() );
		lua_State* L = Lua::Controller->GetState();
		// Call lua info with input level
		Lua::Controller->CallPop( "DoPassiveSet",2, Lua::FN_INT, level, Lua::FN_USERDATA, character );
		// Check the return values
		Lua::Controller->ResetEnvironment();
	}
}

std::unordered_map<string,CLuaSkill::skillProperty_t> CLuaSkill::m_datamap;
std::unordered_map<string,CLuaSkill::staticProperty_t> CLuaSkill::m_staticmap;

void CLuaSkill::StaticInitialize ( const char* entityName )
{
	// First check if not in map
	if ( m_datamap.find(entityName) == m_datamap.end() )
	{
		// If it's not in the map, start off by setting everything about this file to zero.
		m_datamap[entityName] = skillProperty_t();
		m_datamap[entityName].name = "entityName";
		m_datamap[entityName].description = "";
		m_datamap[entityName].icon = CSkill::GetIconFilename();
		m_datamap[entityName].passive = false;

		m_staticmap[entityName] = staticProperty_t();
		m_staticmap[entityName].env = "";
		m_staticmap[entityName].key = 0;


		// Create environment value
		arstring<128> t_package ( "" );
		arstring<128> t_environment;
		sprintf( t_environment.data, "SLSKL_%s", entityName );
		// Shiet
		if ( t_package.length() > 0 ) {
			Lua::Controller->LoadLuaFile( std::string("../../addons/skills/") + t_package.c_str() + "/lua/" + entityName + ".lua", t_environment.c_str(), t_environment.c_str() );
		}
		else {
			Lua::Controller->LoadLuaFile( std::string("skills/") + entityName + ".lua", t_environment.c_str(), t_environment.c_str() );
		}
		lua_State* L = Lua::Controller->GetState();

		Lua::Controller->SetEnvironment( t_environment.c_str() );
		
		// Set the map's environment value and begin grabbing values from it
		m_staticmap[entityName].env = t_environment;

		// Define lua check macro (very repetitive code)
		#define checkLuaFunction(a,b) \
			lua_getfield( L, -1, a ); \
			if ( lua_isfunction( L, -1 ) ) { \
				m_staticmap[entityName].key |= b; \
			} \
			lua_pop(L,1);

		checkLuaFunction( "GetThisLevelInfo", EK_SkGetThisLevelInfo );
		checkLuaFunction( "GetNextLevelInfo", EK_SkGetNextLevelInfo );
		checkLuaFunction( "GetThisLevelCost", EK_SkGetThisLevelCost );
		checkLuaFunction( "GetNextLevelCost", EK_SkGetNextLevelCost );
		checkLuaFunction( "DoPassiveSet", EK_SkGetPassiveStart );
		checkLuaFunction( "SkillInfo", EK_SkSkillProperties );

		#undef checkLuaFunction

		// Grab values for the name, description, icon, and passive
		if ( m_staticmap[entityName].key & EK_SkSkillProperties )
		{
			int returnValues = Lua::Controller->Call( "SkillInfo", 0 );
			if ( returnValues == 1 )
			{
				#define getTableValue(NAME,VALUE,FUNC,TYPE) \
					lua_getfield( L, -1, NAME ); \
					if ( !lua_isnil( L, -1 ) ) { \
						VALUE = (TYPE)FUNC( L, -1 ); \
					} \
					lua_pop(L,1);

				getTableValue( "name",			m_datamap[entityName].name,			lua_tostring, char* );
				getTableValue( "description",	m_datamap[entityName].description,	lua_tostring, char* );
				getTableValue( "icon",			m_datamap[entityName].icon,			lua_tostring, char* );
				getTableValue( "passive",		m_datamap[entityName].passive,		lua_toboolean, bool );

				#undef getTableValue

				Lua::Controller->PopValid( returnValues-1 );
			}
			else {
				Lua::Controller->PopValid( returnValues );
				Debug::Console->PrintError( "lweap::ItemData needs to return one value!\n" );
				throw Core::NullReferenceException();
			}
		}

		Lua::Controller->ResetEnvironment();
	}
}