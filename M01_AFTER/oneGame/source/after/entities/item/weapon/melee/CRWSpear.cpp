
#include "CRWSpear.h"

#include "renderer/logic/model/CModel.h"

CRWSpear::CRWSpear ( void )
	: CBaseRandomMelee( tMeleeWeaponProperties(), ItemData() )
{
	holdType = Item::Sword;
}
/*CRWSword::CRWSword ( tMeleeWeaponProperties& inProps )
	: CBaseRandomMelee( inProps, ItemData() )
{
	holdType = Sword;
}*/

CRWSpear::~CRWSpear ( void )
{
	;
}

#include "after/entities/item/weapon/ItemRandomizer.h"
using namespace WeaponItem;
#include "core/system/io/FileUtils.h"
#include "core-ext/system/io/mccosf.h"

void CRWSpear::Generate ( void )
{
	CBaseRandomMelee::Generate();


	string spearModel = "models/weapons/spearst0/basic.FBX";
	pModel = new CModel( spearModel );

	base_weapon_stats.damagetype	= DamageType::Slash;
	base_weapon_stats.reach			= 4.5f;
	base_weapon_stats.damage		= 11.0f;
	base_weapon_stats.hands			= 1;
	base_weapon_stats.recover_time	= 0.8f; 
/*
	CModel* lastModel;

	bool parts_found [4] = {false,false,false,false};
	if ( part_info.component3.compare("") ) {
		parts_found[2] = true;
	}
	if ( part_info.component4.compare("") ) {
		parts_found[3] = true;
	}

	base_weapon_stats.damagetype		= DamageType::Slash;

	int tifilecount = 0;
	while ( true )
	{
		arstring<256> tPartlistFilename;
		sprintf( tPartlistFilename, ".res/items/sword%d.txt", tifilecount );
		if ( !IO::FileExists( tPartlistFilename ) ) {
			break;
		}
		tifilecount += 1;
	
		// Open up the target partlist.
		CBinaryFile tPartlistFile;
		tPartlistFile.Open( tPartlistFilename, CBinaryFile::IO_READ );

		COSF_Loader partfile ( tPartlistFile.GetFILE() );
		mccOSF_entry_info_t currentEntry;
		do
		{
			partfile.GetNext( currentEntry );

			// Read in the base stats
			if ( currentEntry.type == MCCOSF_ENTRY_NORMAL )
			{
				if ( strcmp( currentEntry.name, "basedamage" ) == 0 ) {
					base_weapon_stats.damage = atof( currentEntry.value );
				}
				else if ( strcmp( currentEntry.name, "baserecover" ) == 0 ) {
					base_weapon_stats.recover_time = atof( currentEntry.value );
				}
				else if ( strcmp( currentEntry.name, "basereach" ) == 0 ) {
					base_weapon_stats.reach = atof( currentEntry.value );
				}
				else if ( strcmp( currentEntry.name, "basehands" ) == 0 ) {
					base_weapon_stats.hands = atoi( currentEntry.value );
				}
			}
			else if ( currentEntry.type == MCCOSF_ENTRY_OBJECT )
			{
				// Check if object is a part being searched for
				int targetPart = -1;
				if ( !parts_found[0] && IsPart(0,currentEntry.name) && part_info.componentMain.compare(currentEntry.value) ) {
					targetPart = 0;
				}
				else if (!parts_found[1] && IsPart(1,currentEntry.name) && part_info.componentSecondary.compare(currentEntry.value)) {
					targetPart = 1;
				}
				else if (!parts_found[2] && IsPart(2,currentEntry.name) && part_info.component3.compare(currentEntry.value)) {
					targetPart = 2;
				}
				else if (!parts_found[3] && IsPart(3,currentEntry.name) && part_info.component4.compare(currentEntry.value)) {
					targetPart = 3;
				}

				// Reset state
				lastModel = NULL;

				ItemAdditives::eItemAdditive lastAdditive	= ItemAdditives::None;
				bool validAdditive							= false;

				// Load in part data
				if ( targetPart != -1 )
				{
					parts_found[targetPart] = true;
					partfile.GoInto( currentEntry );
					do
					{
						partfile.GetNext( currentEntry );
						if ( currentEntry.type == MCCOSF_ENTRY_NORMAL )
						{
							if ( strcmp(currentEntry.name,"brand") == 0 ) {
								if ( targetPart == 0 ) {
									part_info.brand = currentEntry.value;
								}
							}
							else if ( strcmp(currentEntry.name,"model") == 0 ) {
								lastModel = new CModel( currentEntry.value );
								if ( targetPart == 0 ) {
									pModel = lastModel;
									//pModel->transform.scale.y = weapon_stats.reach / 2.5f;
								}
								else {
									vModels.push_back( lastModel );
								}
							}
							else if ( strcmp(currentEntry.name,"material") == 0 ) {
								if ( lastModel ) {
									// Need to parse out the material data
									auto values = StringUtils::Split( string(currentEntry.value), ",;" );
									// Grab the current material
									ItemAdditives::eItemAdditive additive;
										 if ( targetPart == 0 ) additive = part_info.materialMain;
									else if ( targetPart == 1 ) additive = part_info.materialSecondary;
									else if ( targetPart == 2 ) additive = part_info.material3;
									else if ( targetPart == 3 ) additive = part_info.material4;
									// Find the matching material
									for ( uint i = 0; i < values.size(); ++i )
									{
										lastAdditive = ItemAdditives::StringToAdditive( values[i] );
										if ( lastAdditive == additive )
										{
											if ( i+1 < values.size() ) {
												lastModel->DuplicateMaterials();
												lastModel->GetMaterial()->loadFromFile(values[i+1].c_str());
											}
											validAdditive = true; // Mark as proper additive found
										}
									}
								}
								else {
									throw std::exception( "BAD MATERIAL ORDER" );
								}
							}
							else if ( strcmp(currentEntry.name,"damage") == 0 ) {
								//base_weapon_stats.damage += atof(currentEntry.value);
								ModifyNumber( base_weapon_stats.damage, currentEntry.value );
							}
							else if ( strcmp(currentEntry.name,"recover") == 0 ) {
								//base_weapon_stats.recover_time += atof(currentEntry.value);
								ModifyNumber( base_weapon_stats.recover_time, currentEntry.value );
							}
							else if ( strcmp(currentEntry.name,"reach") == 0 ) {
								//base_weapon_stats.reach += atof(currentEntry.value);
								ModifyNumber( base_weapon_stats.reach, currentEntry.value );
							}
						}
					}
					while ( currentEntry.type != MCCOSF_ENTRY_END );

					// Set the proper additive now if invalid additive
					if ( !validAdditive && lastAdditive != ItemAdditives::None )
					{
							 if ( targetPart == 0 ) part_info.materialMain = lastAdditive;
						else if ( targetPart == 1 ) part_info.materialSecondary = lastAdditive;
						else if ( targetPart == 2 ) part_info.material3 = lastAdditive;
						else if ( targetPart == 3 ) part_info.material4 = lastAdditive;
					}
				}
				// End part looking
			}
		}
		while ( currentEntry.type != MCCOSF_ENTRY_EOF );
		tPartlistFile.Close();

		if ( parts_found[0]&&parts_found[1]&&parts_found[2]&&parts_found[3] ) {
			break;
		}
	}

	bool brandFound = false;
	tifilecount = 0;
	while ( true )
	{
		arstring<256> tPartlistFilename;
		sprintf( tPartlistFilename, ".res/items/sword%d.txt", tifilecount );
		if ( !IO::FileExists( tPartlistFilename ) ) {
			break;
		}
		tifilecount += 1;
	
		// Open up the target partlist.
		CBinaryFile tPartlistFile;
		tPartlistFile.Open( tPartlistFilename, CBinaryFile::IO_READ );

		COSF_Loader partfile ( tPartlistFile.GetFILE() );
		mccOSF_entry_info_t currentEntry;
		do
		{
			partfile.GetNext( currentEntry );

			// Read in brand stats
			if ( currentEntry.type == MCCOSF_ENTRY_OBJECT )
			{
				// Check for matching brand
				if ( (strcmp( currentEntry.name, "brand" ) == 0)&&(strcmp( currentEntry.value, part_info.brand ) == 0) )
				{
					brandFound = true;
					partfile.GoInto( currentEntry );
					do
					{
						partfile.GetNext( currentEntry );
						if ( currentEntry.type == MCCOSF_ENTRY_NORMAL )
						{
							if ( strcmp(currentEntry.name,"damage") == 0 ) {
								ModifyNumber( base_weapon_stats.damage, currentEntry.value );
							}
							else if ( strcmp(currentEntry.name,"recover") == 0 ) {
								ModifyNumber( base_weapon_stats.recover_time, currentEntry.value );
							}
							else if ( strcmp(currentEntry.name,"reach") == 0 ) {
								ModifyNumber( base_weapon_stats.reach, currentEntry.value );
							}	
						}
					}
					while ( currentEntry.type != MCCOSF_ENTRY_END );
				}
			}
		}
		while ( currentEntry.type != MCCOSF_ENTRY_EOF );
		tPartlistFile.Close();

		if ( brandFound ) {
			break;
		}
	}

	// Set description
	weaponItemData.sInfo = part_info.brand;// + " made";
	weaponItemData.sInfo = weaponItemData.sInfo + " made";
	*/
}