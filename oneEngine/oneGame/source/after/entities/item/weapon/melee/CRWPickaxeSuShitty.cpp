
#include "CRWPickaxeSuShitty.h"
#include "after/types/terrain/BlockType.h"
#include "renderer/logic/model/CModel.h"

CRWPickaxeSuShitty::CRWPickaxeSuShitty ( void )
	: CBaseRandomMelee( tMeleeWeaponProperties(), ItemData() )
{
	holdType = Item::TwoHandedAxe;
}
CRWPickaxeSuShitty::CRWPickaxeSuShitty ( tMeleeWeaponProperties& inProps )
	: CBaseRandomMelee( inProps, ItemData() )
{
	holdType = Item::TwoHandedAxe;
}

CRWPickaxeSuShitty::~CRWPickaxeSuShitty ( void )
{
	;
}

void CRWPickaxeSuShitty::SetBlockHardness ( void )
{
	// Set Block Strengths
	cBlockHardness[Terrain::EB_DIRT]		= 6;
	cBlockHardness[Terrain::EB_GRASS]		= 6;
	cBlockHardness[Terrain::EB_SAND]		= 7;
	cBlockHardness[Terrain::EB_CLAY]		= 5;
	cBlockHardness[Terrain::EB_WOOD]		= 6;
	cBlockHardness[Terrain::EB_WIRE]		= 4;
	cBlockHardness[Terrain::EB_STONE]		= 4;
	cBlockHardness[Terrain::EB_STONEBRICK]	= 4;
	cBlockHardness[Terrain::EB_CRYSTAL]		= 5;
}

void CRWPickaxeSuShitty::Generate ( void )
{
	CBaseRandomItem::Generate();

	string pickModel = "models/items/pickaxe_su_shitty.FBX";

	//if ( iBladeType == 0 )
	//	pickModel = "models\\items\\pickaxe_su_shitty.FBX";

	pModel = new CModel( pickModel );
	//pModel->transform.scale.y = weapon_stats.reach / 2.5f;
}