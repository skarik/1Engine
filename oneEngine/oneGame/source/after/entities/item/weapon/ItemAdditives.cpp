
#include "ItemAdditives.h"
#include "after/entities/item/system/ItemTerraBlok.h"
#include "after/types/terrain/BlockType.h"

ItemAdditives::eItemAdditive ItemAdditives::ItemToAdditive ( CWeaponItem* item )
{
	// Just do a switch case on the item id
	switch ( item->GetID() )
	{
	case 2:
		switch ( ((ItemTerraBlok*)item)->GetType() )
		{
		case Terrain::EB_WOOD:
			return Wood;
		case Terrain::EB_STONE:
		case Terrain::EB_RIGDESTONE:
		case Terrain::EB_STONEBRICK:
			return Stone;
		case Terrain::EB_HEMATITE:
			return Iron;
		}
		break;
	case 50: //branches and shit
	case 51:
	case 52:
		return Wood;
		break;

	default:
		if ( item->GetBaseClassName() == "RandomBaseMelee" ) { //random item, has multiple types
			return Composite;
		}
		break;
	}
	return None;
}

ItemAdditives::eItemAdditive ItemAdditives::StringToAdditive ( const string& value )
{
	if ( value == "wood" ) return Wood;
	if ( value == "bone" ) return Bone;
	if ( value == "iron" ) return Iron;
	return None;
}
