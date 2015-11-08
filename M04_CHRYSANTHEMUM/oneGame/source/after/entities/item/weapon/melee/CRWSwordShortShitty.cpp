
#include "CRWSwordShortShitty.h"

#include "renderer/logic/model/CModel.h"

CRWSwordShortShitty::CRWSwordShortShitty ( void )
	: CBaseRandomMelee( tMeleeWeaponProperties(), ItemData() )
{
	//iBladeType = 0;
	//iGripType = 0;
	sBladeModel	= ".res/models/weapons/swordssu/blade1.fbx";
	sGripModel	= ".res/models/weapons/swordssu/grip1.fbx";
	holdType = Item::Sword;
}
CRWSwordShortShitty::CRWSwordShortShitty ( tMeleeWeaponProperties& inProps, const char* bladeModel, const char* gripModel )
	: CBaseRandomMelee( inProps, ItemData() )
{
	//iBladeType = bladeType;
	//iGripType = gripType;
	sBladeModel = bladeModel;
	sGripModel = gripModel;

	holdType = Item::Sword;
}

CRWSwordShortShitty::~CRWSwordShortShitty ( void )
{
	;
}

void CRWSwordShortShitty::Generate ( void )
{
	CBaseRandomItem::Generate();

	/*string swordModel = ".res\\models\\weapons\\swordssu\\blade1.FBX";
	string gripModel = ".res\\models\\weapons\\swordssu\\grip1.FBX";

	if ( iBladeType == 0 )
		swordModel = ".res\\models\\weapons\\swordssu\\blade1.FBX";
	else if ( iBladeType == 1 )
		swordModel = ".res\\models\\weapons\\swordssu\\blade2.FBX";
	else if ( iBladeType == 2 )
		swordModel = ".res\\models\\weapons\\swordssu\\blade3.FBX";
	else if ( iBladeType == 3 )
		swordModel = ".res\\models\\weapons\\swordssu\\blade4.FBX";
	else if ( iBladeType == 4 )
		swordModel = ".res\\models\\weapons\\swordssu\\blade5.FBX";

	pModel = new CModel( swordModel );
	pModel->transform.scale.y = weapon_stats.reach / 2.5f;

	if ( iGripType == 0 )
		gripModel = ".res\\models\\weapons\\swordssu\\grip1.FBX";
	else if ( iGripType == 1 )
		gripModel = ".res\\models\\weapons\\swordssu\\grip2.FBX";
	else if ( iGripType == 2 )
		gripModel = ".res\\models\\weapons\\swordssu\\grip3.FBX";
	else if ( iGripType == 3 )
		gripModel = ".res\\models\\weapons\\swordssu\\grip4.FBX";
	else if ( iGripType == 4 )
		gripModel = ".res\\models\\weapons\\swordssu\\grip5.FBX";

	vModels.push_back( new CModel ( gripModel ) );*/

	pModel = new CModel( sBladeModel.c_str() );
	pModel->transform.scale.y = weapon_stats.reach / 2.5f;

	vModels.push_back( new CModel ( sGripModel.c_str() ) );
}