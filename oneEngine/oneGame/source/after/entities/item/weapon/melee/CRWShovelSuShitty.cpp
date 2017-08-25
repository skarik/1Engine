
#include "CRWShovelSuShitty.h"
#include "after/types/terrain/BlockType.h"
#include "renderer/logic/model/CModel.h"

CRWShovelSuShitty::CRWShovelSuShitty ( void )
	: CBaseRandomMelee( tMeleeWeaponProperties(), ItemData() )
{
	holdType = Item::TwoHandedAxe;
}
CRWShovelSuShitty::CRWShovelSuShitty ( tMeleeWeaponProperties& inProps )
	: CBaseRandomMelee( inProps, ItemData() )
{
	holdType = Item::TwoHandedAxe;
}

CRWShovelSuShitty::~CRWShovelSuShitty ( void )
{
	;
}

void CRWShovelSuShitty::SetBlockHardness ( void )
{
	// Set Block Strengths
	cBlockHardness[Terrain::EB_DIRT]		= 1;
	cBlockHardness[Terrain::EB_GRASS]	= 2;
	cBlockHardness[Terrain::EB_SAND]		= 1;
	cBlockHardness[Terrain::EB_CLAY]		= 2;
	cBlockHardness[Terrain::EB_WOOD]		= 6;
	cBlockHardness[Terrain::EB_WIRE]		= 4;
	//cBlockHardness[EB_STONE]	= 4;
	//cBlockHardness[EB_STONEBRICK]=4;
	//cBlockHardness[EB_CRYSTAL]	= 5;
	cBlockHardness[Terrain::EB_ASH]		= 1;
}

void CRWShovelSuShitty::Generate ( void )
{
	CBaseRandomItem::Generate();

	string pickModel = "models/items/pickaxe_su_shitty.FBX";

	//if ( iBladeType == 0 )
	//	pickModel = "models\\items\\pickaxe_su_shitty.FBX";

	pModel = new CModel( pickModel );
	//pModel->transform.scale.y = weapon_stats.reach / 2.5f;
}