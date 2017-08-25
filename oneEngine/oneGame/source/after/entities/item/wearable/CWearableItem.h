#ifndef _C_WEARABLE_ITEM_H_
#define _C_WEARABLE_ITEM_H_

#include "after/entities/item/CWeaponItem.h"

class CWearableItem : public CWeaponItem
{
	BaseClass ( "WearableItem" );
	ClassName ( "WearableItem" );
public:
	explicit CWearableItem (const WItemData &);

	~CWearableItem (void);

	void Update ( void );
	//void CreatePhysics (void) override;

	//void DrawTooltip (const Vector2d &, const WTooltipStyle &) override;

	void OnEquip ( CActor* interactingActor ) override;
	void OnUnequip ( CActor* interactingActor ) override;

	bool Use ( int v ) override {
		return false;
	}

	const string& GetPlayerModel ( void );
	bool HidesBody ( void ) const;
	bool HidesHair ( void ) const;
protected:

	/*unsigned int required_level; // These should go elsewhere
	unsigned int required_str;
	unsigned int required_agi;
	unsigned int required_int;*/
	float armor_value;	// Probably will use custom code, not the buffs/debuffs code, so keep this
	//string icon_filename; // Using models to render icons
	//bool aesthetic; // Not needed

	string sWorldModel;
	string sPlayerModel;
	bool   bHideBody;
	bool   bHideHair;
};

#endif