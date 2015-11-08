#include "CWearableItem.h"
#include "renderer/logic/model/CModel.h"

CWearableItem::CWearableItem (const WItemData & wData): CWeaponItem (wData)
{
	pModel = NULL;
	/*required_level = 0;
	required_str = 0;
	required_agi = 0;
	required_int = 0;*/
	armor_value = 0;	
	/*icon_filename = "";
	aesthetic = false;*/
	bHideBody = false;
	bHideHair = false;
	sPlayerModel = "";
	sWorldModel = ".res/models/items/tree_resin.FBX";
}

CWearableItem::~CWearableItem (void)
{
	/*required_level = 0;
	required_str = 0;
	required_agi = 0;
	required_int = 0;
	armor_value = 0.0;	
	icon_filename = "";
	aesthetic = false;*/
	// stop making destructors same as constructors
	//  you're wasting (not) precious clock cycles
}
/*
void CWearableItem::CreatePhysics (void)
{
	CWeaponItem::CreatePhysics();
}

void CWearableItem::DrawTooltip (const Vector2d & pos, const WTooltipStyle & gangnam)
{
	CWeaponItem::DrawTooltip(pos, gangnam);
}*/
const string& CWearableItem::GetPlayerModel ( void )
{
	return sPlayerModel;
}
bool CWearableItem::HidesBody ( void ) const
{
	return bHideBody;
}
bool CWearableItem::HidesHair ( void ) const
{
	return bHideHair;
}

void CWearableItem::Update ( void )
{
	CWeaponItem::Update();

	if ( pModel == NULL ) {
		pModel = new CModel( sWorldModel );
	}
}

void CWearableItem::OnEquip ( CActor* interactingActor )
{
	SetOwner( interactingActor );
	interactingActor->OnEquip( this );
}
void CWearableItem::OnUnequip ( CActor* interactingActor )
{
	interactingActor->OnUnequip( this );
}

// Seriously, does ANYONE read the documentation in CWeaponItem.h ???