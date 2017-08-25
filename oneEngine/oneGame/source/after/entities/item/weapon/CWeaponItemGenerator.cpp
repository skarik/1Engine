
// Include base classes
#include "engine/behavior/CGameBehavior.h"
#include "after/entities/item/weapon/CBaseRandomMelee.h"
#include "CBaseRandomBow.h"
#include "CBaseRandomGun.h"
#include "after/entities/item/weapon/CBaseRandomTossable.h"

// Include class definition
#include "CWeaponItemGenerator.h"

// Include utilities needed for randomized weapons
#include "core/math/Math.h"
#include "core/math/random/Random.h"
#include "after/types/terrain/TerrainTypes.h"
#include "core-ext/system/io/mccosf.h"

// Include classes that need to be checked
#include "after/states/inventory/CPlayerInventory.h"
#include "after/entities/character/CAfterPlayer.h"

// Include other items to drop
#include "after/entities/item/props/ItemTorch.h"
#include "after/entities/item/props/ItemLantern.h"
#include "after/entities/item/ammo/ItemPebble.h"
#include "after/entities/item/junk/ItemJunkSkull.h"

// Include all generatable weapons
#include "after/entities/item/weapon/melee/CRWSwordShortShitty.h"
#include "after/entities/item/weapon/melee/CRWPickaxeSuShitty.h"
#include "after/entities/item/weapon/melee/CRWShovelSuShitty.h"
#include "after/entities/item/weapon/melee/CRWHoe.h"

#include "after/entities/item/weapon/bow/CRWBowShitty.h"

#include "after/entities/item/weapon/melee/CRWSword.h"
#include "after/entities/item/weapon/melee/CRWDagger.h"
#include "after/entities/item/weapon/melee/CRWSpear.h"

// Include all generatable items
#include "after/entities/item/weapon/tool/CRWDrillBasic.h"

// Include throwables
#include "after/entities/item/weapon/tossable/CRIDynamite.h"

#include "core/system/io/CBinaryFile.h"


// Class Instance
CWeaponItemGenerator ItemGenerator;

// ========================
// Random Weapon System
// ========================
CBinaryFile* CWeaponItemGenerator::LoadInRandomItemFile ( const char* s_filename )
{
	// Open file, read in all the classes
	// ALL OF THEM
	CBinaryFile* file = new CBinaryFile();
	file->Open( s_filename, CBinaryFile::IO_READ );

	if ( !file->IsOpen() ) {
		Debug::Console->PrintError( "Could not open the randomized weapon file. This is extremely bad. Must be fixed." );
		return file;
	}

	// clear out lists
	itemParts.clear();
	itemInfo.clear();
	requiredComponents.clear();
	optionalComponents.clear();


	COSF_Loader osf ( file->GetFILE() );
	mccOSF_entry_info_t entry;
	// Load in osf
	// Skip to definition area
	do
	{
		osf.GetNext( entry );
	}
	while ( (entry.type != MCCOSF_ENTRY_MARKER) && (strcmp(entry.name,"definition") != 0) );

	// Read into until hit the #partlist
	do
	{
		osf.GetNext( entry );
		if ( entry.type == MCCOSF_ENTRY_NORMAL )
		{
			// Required list input
			if ( strcmp(entry.name,"require") == 0 )
			{
				// strtok the value to get the values
				char* token;
				token = strtok( entry.value, ", \n\t" );
				do
				{
					requiredComponents.push_back( arstring<64>(token) );
					token = strtok( NULL, ", \n\t" );
				}
				while ( token != NULL );
			}
			// Optional list input
			else if ( strcmp(entry.name,"optional") == 0 )
			{
				// strtok the value to get the values
				char* token;
				token = strtok( entry.value, ", \n\t" );
				do
				{
					optionalComponents.push_back( arstring<64>(token) );
					token = strtok( NULL, ", \n\t" );
				}
				while ( token != NULL );
			}
			// Misc options
			else
			{
				randomItemInfoEntry_t newEntry;
				newEntry.key = entry.name;
				newEntry.value = entry.value;
				itemInfo.push_back( newEntry );
			}
		}
	}
	while ( (entry.type != MCCOSF_ENTRY_MARKER) && (strcmp(entry.name,"partlist") != 0) );

	// Loop through the part files until the #end
	do
	{
		osf.GetNext( entry );
		if ( entry.type == MCCOSF_ENTRY_OBJECT )
		{
			// Take the name and look through the entry
			randomItemPartEntry_t newEntry;
			/*for ( uint i = 0; i < requiredComponents.size(); ++i )
			{
				if ( requiredComponents[i] == entry.name )
				{

				}
			}*/
			newEntry.type = entry.name;
			newEntry.component = entry.value;
			//printf( "cmp: %s, %s\n", newEntry.type.c_str(), newEntry.component.c_str() );

			itemParts.push_back( newEntry );
		}
	}
	while ( (entry.type != MCCOSF_ENTRY_MARKER) && (strcmp(entry.name,"end") != 0) );

	// And done loading the info
	return file;
}

int CWeaponItemGenerator::RandomItemChooseComponent ( const char* s_component, const char* s_partial_match )
{
	int matchCount = 0;
	// Count the number of matches
	for ( uint i = 0; i < itemParts.size(); ++i ) 
	{
		//printf( "%s %s\n", itemParts[i].component, itemParts[i].type );
		if ( itemParts[i].type.compare(s_component) )
		{
			//printf( "match: %s vs %s\n", itemParts[i].type.c_str(), s_partial_match );
			if ( strstr( itemParts[i].component, s_partial_match ) )
			{
				matchCount += 1;
			}
		}
	}
	//printf( "match count: %d\n", matchCount );
	// Choose a value
	int choice = Random.Next() % matchCount;
	// Step to that target
	matchCount = 0;
	//printf( "matching %d/%d...", choice, matchCount );
	for ( uint i = 0; i < itemParts.size(); ++i )
	{
		if ( itemParts[i].type.compare(s_component) )
		{
			if ( strstr( itemParts[i].component, s_partial_match ) )
			{
				//printf( "%d ", i );
				if ( choice == matchCount ) {
					return i;
				}
				matchCount += 1;
			}
		}
	}

	// No match
	return -1;
}

arstring<64>&	CWeaponItemGenerator::GetItemInfoValue ( const char* s_key )
{
	for ( uint i = 0; i < itemInfo.size(); ++i )
	{
		if ( itemInfo[i].key.compare(s_key) )
		{
			return itemInfo[i].value;
		}
	}
	static arstring<64> failure = arstring<64>("0");
	return failure;
}


void			CWeaponItemGenerator::LoadInRandomComponentInfo ( CBinaryFile* file, const randomItemPartEntry_t& n_entry )
{
	//printf( "%s %s\n", n_entry.type.c_str(), n_entry.component.c_str() );

	// Clear item info
	itemInfo.clear();
	// Go back to start
	fseek( file->GetFILE(), 0, SEEK_SET );
	// Search through the file again
	COSF_Loader osf ( file->GetFILE() );
	mccOSF_entry_info_t entry;

	do
	{
		osf.GetNext( entry );
		// Look for the object
		if (( n_entry.type.compare(entry.name) )&&( n_entry.component.compare(entry.value) ))
		{
			osf.GoInto( entry );
			osf.GetNext( entry );
			do
			{
				randomItemInfoEntry_t newEntry;
				newEntry.key = entry.name;
				newEntry.value = entry.value;
				
				//printf( "key: %s | value: %s\n", newEntry.key.c_str(), newEntry.value.c_str() );

				itemInfo.push_back( newEntry );

				osf.GetNext( entry );
			}
			while ( entry.type != MCCOSF_ENTRY_END );
			return; // Found it, time to leave
		}
	}
	while ( true );
}




// ========================
// Chance for drops
// ========================

#include "engine-common/network/playerlist.h"

bool CWeaponItemGenerator::ChanceDiggingDrops ( void )
{
	auto playerList = Network::GetPlayerActors();
	// It's default a VERY small chance unless the player for some reason has an empty inventory. 
	if ( playerList[0].actor->GetInventory()->IsEmpty() ) {
		return ( Random.Range( 0.0f, 100.0f ) < 4.0f ); // 4% chance
	}
	else {
		return ( Random.Range( 0.0f, 100.0f ) < 0.1f ); // 0.1% chance
	}
}
bool CWeaponItemGenerator::ChanceDesertTripDrops ( void )
{
	auto playerList = Network::GetPlayerActors();
	// It's default a VERY small chance unless the player for some reason has an empty inventory. 
	if ( playerList[0].actor->GetInventory()->IsEmpty() ) {
		return ( Random.Range( 0.0f, 100.0f ) < 1.0f ); // 1% chance
	}
	else {
		return ( Random.Range( 0.0f, 100.0f ) < 0.1f ); // 0.1% chance
	}
}
bool CWeaponItemGenerator::ChanceGrass ( ushort nGrassType )
{
	return ( Random.Range( 0.0f, 100.0f ) < 10.0f );
}

// ========================
// Grouped/Themed Drops
// ========================

CWeaponItem*	CWeaponItemGenerator::DropDigging ( const Vector3d& generationPosition, ushort nBlockType )
{
	auto playerList = Network::GetPlayerActors();
	CWeaponItem* pItem = NULL;
	if ( playerList[0].actor->GetInventory()->IsEmpty() ) {
		// Choose either a torch or a crummy weapon.
		if ( Random.Next() % 2 == 0 ) {
			pItem = new ItemTorch;
			pItem->transform.position = generationPosition;
			pItem->transform.rotation.Euler( Vector3d( random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ) ) );
			pItem->transform.SetDirty();
		}
		else {
			if ( Random.Next() % 3 == 0 ) {
				pItem = MakePickaxeSuShitty( generationPosition );
			}
			else {
				pItem = MakeShittySword( generationPosition );
			}
		}
	}
	else {
		// 50% of the time is just junk
		if ( Random.Range( 0.0f, 1.0f ) < 0.5f )
		{
			// Drop Junk
			pItem = new ItemJunkSkull;
			pItem->transform.position = generationPosition;
			pItem->transform.rotation.Euler( Vector3d( random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ) ) );
			pItem->transform.SetDirty();
		}
		else
		{
			// Drop Treasure
			switch ( Random.Next() % 3 )
			{
			case 0:
				pItem = MakePickaxeSuShitty( generationPosition );
				break;
			case 1:
				pItem = MakeShovelSuShitty( generationPosition );
				break;
			default:
				pItem = MakeShittySword( generationPosition );
			}
		}
	}
	// Set to item just floating there. Like a boss.
	pItem->SetHoldState( Item::Hover );
	return pItem;
}
CWeaponItem*	CWeaponItemGenerator::DropDesertTrip ( const Vector3d& generationPosition )
{
	auto playerList = Network::GetPlayerActors();
	CWeaponItem* pItem = NULL;
	if ( playerList[0].actor->GetInventory()->IsEmpty() ) {
		// Choose either a lantern, a crummy weapon, or a skull.
		switch ( Random.Next() % 3 )
		{
		case 0: // Drop a lantern
			pItem = new ItemLantern;
			pItem->transform.position = generationPosition;
			pItem->transform.rotation.Euler( Vector3d( random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ) ) );
			pItem->transform.SetDirty();
			break;
		case 1: // Drop a crummy weapon
			if ( Random.Next() % 3 == 0 ) {
				pItem = MakePickaxeSuShitty( generationPosition );
			}
			else {
				pItem = MakeShittySword( generationPosition );
			}
			break;
		case 2: // Drop a skull :D
			pItem = new ItemJunkSkull;
			pItem->transform.position = generationPosition;
			pItem->transform.rotation.Euler( Vector3d( random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ) ) );
			pItem->transform.SetDirty();
			break;
		}
	}
	else {
		// 50% of the time is just junk
		if ( Random.Range( 0.0f, 1.0f ) < 0.5f )
		{
			// Drop Junk
			pItem = new ItemJunkSkull;
			pItem->transform.position = generationPosition;
			pItem->transform.rotation.Euler( Vector3d( random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ) ) );
			pItem->transform.SetDirty();
		}
		else
		{
			// Drop Treasure
			switch ( Random.Next() % 3 )
			{
			case 0:
				pItem = MakePickaxeSuShitty( generationPosition );
				break;
			case 1:
				pItem = MakeShovelSuShitty( generationPosition );
				break;
			default:
				pItem = MakeShittySword( generationPosition );
			}
		}
	}
	// Set to item just floating there.
	pItem->SetHoldState( Item::Hover );
	return pItem;
}
CWeaponItem*	CWeaponItemGenerator::DropGrass	( const Vector3d& generationPosition, ushort nGrassType )
{
	return NULL;
}

CWeaponItem*	CWeaponItemGenerator::DropJunk	( const Vector3d& generationPosition )
{
	CWeaponItem* pItem;
	// Drop Junk
	pItem = new ItemJunkSkull;
	pItem->transform.position = generationPosition;
	pItem->transform.rotation.Euler( Vector3d( random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ) ) );
	pItem->transform.SetDirty();
	return pItem;
}


// ========================
// Debug/Specific Drops
// ========================

// Function definitions
CWeaponItem* CWeaponItemGenerator::MakeShittySword ( const Vector3d& generationPosition )
{
	/*CBaseRandomMelee::tMeleeWeaponProperties weaponProperties;
	weaponProperties.reach			= Random.Range( 1.7f, 3.2f );
	weaponProperties.damage			= 11.0f;
	weaponProperties.hands			= 1;
	weaponProperties.damagetype		= DamageType::Slash;
	weaponProperties.recover_time	= 0.7f; 

	CRWSwordShortShitty* pWeapon = new CRWSwordShortShitty ( weaponProperties, rand()%5, rand()%5 );
	pWeapon->Generate(); // Force cache and load the weapon immediately

	pWeapon->transform.position = generationPosition;
	pWeapon->transform.rotation.Euler( Vector3d( random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ) ) );
	pWeapon->transform.SetDirty();*/

	// Load in the target file
	/*CBinaryFile* file = LoadInRandomItemFile( ".res/items/sword_onehanded_basic.txt" );

	// then, choose shit
	int bladeChoice = RandomItemChooseComponent( "blade", "unremarkable" );
	int hiltChoice = RandomItemChooseComponent( "hilt", "unremarkable" );
	arstring<128> bladeModel;
	arstring<128> hiltModel;

	// Get info
	CBaseRandomMelee::tMeleeWeaponProperties weaponProperties;
	weaponProperties.reach			= atof( GetItemInfoValue( "basereach" ) );
	weaponProperties.damage			= atof( GetItemInfoValue( "basedamage" ) );
	weaponProperties.hands			= atof( GetItemInfoValue( "basehands" ) );
	weaponProperties.damagetype		= DamageType::Slash;
	weaponProperties.recover_time	= atof( GetItemInfoValue( "baserecover" ) );
	
	// Set the properties
	printf( "\ntt %d %d\n", bladeChoice, hiltChoice );
	LoadInRandomComponentInfo( file, itemParts[bladeChoice] );
	for ( uint i = 0; i < itemInfo.size(); ++i )
	{
		if ( itemInfo[i].key.compare("model") )
		{
			bladeModel = itemInfo[i].value;
			printf( "bladeModel: %s\n", bladeModel.c_str() );
		}
		else if ( itemInfo[i].key.compare("recover") ) {
			weaponProperties.recover_time += atof( itemInfo[i].value );
		}
		else if ( itemInfo[i].key.compare("damage") ) {
			weaponProperties.damage += atof( itemInfo[i].value );
		}
		else if ( itemInfo[i].key.compare("reach") ) {
			weaponProperties.reach += atof( itemInfo[i].value );
		}
	}
	LoadInRandomComponentInfo( file, itemParts[hiltChoice] );
	for ( uint i = 0; i < itemInfo.size(); ++i )
	{
		if ( itemInfo[i].key.compare("model") )
		{
			hiltModel = itemInfo[i].value;
			printf( "hiltModel: %s\n", hiltModel.c_str() );
		}
		else if ( itemInfo[i].key.compare("recover") ) {
			weaponProperties.recover_time += atof( itemInfo[i].value );
		}
		else if ( itemInfo[i].key.compare("damage") ) {
			weaponProperties.damage += atof( itemInfo[i].value );
		}
		else if ( itemInfo[i].key.compare("reach") ) {
			weaponProperties.reach += atof( itemInfo[i].value );
		}
	}

	// Close file
	delete file;

	// Create weapon
	CRWSwordShortShitty* pWeapon = new CRWSwordShortShitty ( weaponProperties, bladeModel, hiltModel );
	pWeapon->Generate(); // Force cache and load the weapon immediately

	pWeapon->transform.position = generationPosition;
	pWeapon->transform.rotation.Euler( Vector3d( random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ) ) );
	pWeapon->transform.SetDirty();

	// Return result
	return (CWeaponItem*)pWeapon;*/

	if ( Random.Next()%10 == 0 )
	{
		CRWSpear* pWeapon = new CRWSpear ( );

		pWeapon->Generate(); // Force cache and load the weapon immediately

		// Set positions
		pWeapon->transform.position = generationPosition;
		pWeapon->transform.rotation.Euler( Vector3d( random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ) ) );
		pWeapon->transform.SetDirty();

		// Return result
		return (CWeaponItem*)pWeapon;
	}
	else if ( Random.Next()%3 == 0 )
	{
		// Create weapon
		CRWDagger* pWeapon = new CRWDagger ( );

		// Set the parts randomly
		char partListBlade [][32] = {
			"set0.basic", "set0.fancy", "set0.merchant", "set0.wide", "set0.eastern"
		};
		char partListHilt [][32] = {
			"set0.basic", "set0.thumbhook", "set0.backend", "set0.formal", "set0.farmersai",
		};
		ItemAdditives::eItemAdditive materials [] = {
			ItemAdditives::Wood, ItemAdditives::Iron
		};
		pWeapon->part_info.materialMain = materials[Random.Next()%2];
		pWeapon->part_info.materialSecondary = materials[Random.Next()%2];

		pWeapon->part_info.componentMain = partListBlade[Random.Next()%5];
		pWeapon->part_info.componentSecondary = partListHilt[Random.Next()%5];

		pWeapon->Generate(); // Force cache and load the weapon immediately

		// Set positions
		pWeapon->transform.position = generationPosition;
		pWeapon->transform.rotation.Euler( Vector3d( random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ) ) );
		pWeapon->transform.SetDirty();

		// Return result
		return (CWeaponItem*)pWeapon;
	}
	else
	{
		// Create weapon
		CRWSword* pWeapon = new CRWSword ( );

		// Set the parts randomly
		char partListBlade [][32] = {
			"unremarkable.basic", "unremarkable.wide", "unremarkable.pointed", "unremarkable.eastern", "unremarkable.merchant",
			"set1.desertthin", "set1.desertthick", "set1.basic1", "set1.basic2"
		};
		char partListHilt [][32] = {
			"unremarkable.basic", "unremarkable.long", "unremarkable.short", "unremarkable.eastern", "unremarkable.merchant",
			"set1.desert1", "set1.desert2", "set1.basic1", "set1.basic2"
		};
		ItemAdditives::eItemAdditive materials [] = {
			ItemAdditives::Wood, ItemAdditives::Iron
		};
		pWeapon->part_info.materialMain = materials[Random.Next()%2];
		pWeapon->part_info.materialSecondary = materials[Random.Next()%2];

		pWeapon->part_info.componentMain = partListBlade[Random.Next()%9];
		pWeapon->part_info.componentSecondary = partListHilt[Random.Next()%9];

		pWeapon->Generate(); // Force cache and load the weapon immediately

		// Set positions
		pWeapon->transform.position = generationPosition;
		pWeapon->transform.rotation.Euler( Vector3d( random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ) ) );
		pWeapon->transform.SetDirty();

		// Return result
		return (CWeaponItem*)pWeapon;
	}
}	

CWeaponItem* CWeaponItemGenerator::MakePickaxeSuShitty ( const Vector3d& generationPosition )
{
	CBaseRandomMelee::tMeleeWeaponProperties weaponProperties;
	weaponProperties.reach			= 1.6f;
	weaponProperties.damage			= 12.0f;
	weaponProperties.hands			= 2;
	weaponProperties.damagetype		= DamageType::Pierce;
	weaponProperties.recover_time	= 1.4f; 

	CRWPickaxeSuShitty* pWeapon = new CRWPickaxeSuShitty ( weaponProperties );
	pWeapon->Generate(); // Force cache and load the weapon immediately

	pWeapon->transform.position = generationPosition;
	pWeapon->transform.rotation.Euler( Vector3d( random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ) ) );
	pWeapon->transform.SetDirty();

	return (CWeaponItem*)pWeapon;
}	
CWeaponItem* CWeaponItemGenerator::MakeHoe ( const Vector3d& generationPosition )
{
	/*CBaseRandomMelee::tMeleeWeaponProperties weaponProperties;
	weaponProperties.reach			= 1.6f;
	weaponProperties.damage			= 12.0f;
	weaponProperties.hands			= 2;
	weaponProperties.damagetype		= DamageType::Pierce;
	weaponProperties.recover_time	= 1.4f; */

	CRWHoe* pWeapon = new CRWHoe ( );
	pWeapon->Generate(); // Force cache and load the weapon immediately

	pWeapon->transform.position = generationPosition;
	pWeapon->transform.rotation.Euler( Vector3d( random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ) ) );
	pWeapon->transform.SetDirty();

	return (CWeaponItem*)pWeapon;
}

CWeaponItem* CWeaponItemGenerator::MakeShovelSuShitty ( const Vector3d& generationPosition )
{
	CBaseRandomMelee::tMeleeWeaponProperties weaponProperties;
	weaponProperties.reach			= 1.4f;
	weaponProperties.damage			= 10.0f;
	weaponProperties.hands			= 2;
	weaponProperties.damagetype		= DamageType::Slash;
	weaponProperties.recover_time	= 0.8f; 

	CRWShovelSuShitty* pWeapon = new CRWShovelSuShitty ( weaponProperties );
	pWeapon->Generate(); // Force cache and load the weapon immediately

	pWeapon->transform.position = generationPosition;
	pWeapon->transform.rotation.Euler( Vector3d( random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ) ) );
	pWeapon->transform.SetDirty();

	return (CWeaponItem*)pWeapon;
}	

CWeaponItem* CWeaponItemGenerator::MakeTestBow ( const Vector3d& generationPosition )
{
	CBaseRandomBow::tBowWeaponProperties weaponProperties;
	weaponProperties.range		= 200.0f; //200 foot valid range
	weaponProperties.damage		= 12.0f;
	weaponProperties.hands		= 2;
	weaponProperties.speed		= 1.0f;
	weaponProperties.pspeed		= 40.0f;

	CRWBowShitty* pWeapon = new CRWBowShitty ( weaponProperties );
	pWeapon->Generate();

	pWeapon->transform.position = generationPosition;
	pWeapon->transform.rotation.Euler( Vector3d( random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ) ) );
	pWeapon->transform.SetDirty();

	return (CWeaponItem*)pWeapon;
}
 
CWeaponItem* CWeaponItemGenerator::MakeBasicDrill ( const Vector3d& generationPosition )
{
	CBaseRandomMelee::tMeleeWeaponProperties weaponProperties;
	weaponProperties.reach			= 2.5f;
	weaponProperties.damage			= 1.0f;
	weaponProperties.hands			= 2;
	weaponProperties.damagetype		= DamageType::Slash;
	weaponProperties.recover_time	= 0.3f;

	CRWDrillBasic* pWeapon = new CRWDrillBasic ( weaponProperties );
	pWeapon->Generate(); // Force cache and load the weapon immediately

	pWeapon->transform.position = generationPosition;
	pWeapon->transform.rotation.Euler( Vector3d( random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ) ) );
	pWeapon->transform.SetDirty();

	return (CWeaponItem*)pWeapon;
}

CWeaponItem* CWeaponItemGenerator::MakeDynamite ( const Vector3d& generationPosition )
{
	CBaseRandomTossable::tTossableProperties tossableProperties;
	tossableProperties.damage		= random_range( 100.0f, 120.0f );
	tossableProperties.damagetype	= DamageType::Blast;
	tossableProperties.hands		= 1;
	tossableProperties.speed		= 0.3f;

	CRIDynamite* pWeapon = new CRIDynamite ( tossableProperties );
	pWeapon->Generate(); // Force cache and load the weapon immediately

	pWeapon->transform.position = generationPosition;
	pWeapon->transform.rotation.Euler( Vector3d( random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ), random_range( -180.0f, 180.0f ) ) );
	pWeapon->transform.SetDirty();

	return (CWeaponItem*)pWeapon;
}