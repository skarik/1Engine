
#ifndef _C_WEAPON_ITEM_TYPES_H_
#define _C_WEAPON_ITEM_TYPES_H_

class CWeaponItem;

namespace WeaponItem
{
	enum WeaponItemType
	{
		TypeDefault = 0,
		TypeBag,

		// == Weapon types ==
		TypeWeapon,
		// Blades
		TypeWeaponKnife,
		TypeWeaponSword,
		TypeWeaponLongSword,
		// Axes
		TypeWeaponPick,
		TypeWeaponPickaxe,
		TypeWeaponAxe,
		TypeWeaponGreatAxe,
		// Polearms
		TypeWeaponGlaive, //pole sword
		TypeWeaponPoleAxe,
		TypeWeaponPike,
		TypeWeaponScythe,
		// Diggers
		TypeWeaponShovel,
		// Bashers
		TypeWeaponHammer,
		TypeWeaponWarHammer,
		TypeWeaponClub,
		TypeWeaponFlail,
		
		// == Armor types ==
		TypeArmor,
		TypeArmorHead,
		TypeArmorChest,
		TypeArmorShoulder,
		TypeArmorForearms,
		TypeArmorGloves,
		TypeArmorGreaves,
		TypeArmorBoots,

		// == Clothing types ==
		TypeBelt,
		TypeNecklace,
		TypeRing,
		TypeWearShirt,
		TypeWearHat,
		TypeWearJacket,
		TypeWearForearms,
		TypeWearGloves,
		TypeWearDress,
		TypeWearShorts,
		TypeWearSocks,
		TypeWearFeet
	};
	enum WeaponRarity
	{
		/*RarityNone = 0,
		RarityCommon,
		RarityUncommon,
		RarityRare,
		RarityMythical,
		RarityLegendary,
		RarityHeroic*/
		RarityNone = 0,
		RarityCommon,
		RarityUncommon,
		RarityRare,
		RarityMythical,
		RarityHeroic,
		RarityVillainous,
		RarityGeneric,
		RarityNewLegend
	};
	enum GeneralItemType
	{
		// == Weapon types ==
		//ItemSword,
		ItemBlade,
		ItemAxe,
		//ItemPick,
		//ItemHammer,
		ItemDigger, 
		//ItemMace,
		//ItemScepter,
		ItemBasher,
		ItemPolearm,
		//ItemSpear,

		ItemBow,
		ItemCrossbow,
		
		ItemFlintlock,
		ItemMagitechGun,

		// == Skill types ==
		SkillDefault,
		SkillPassive,
		SkillActive,
		// == Other Equippable types ==
		ItemArmor,
		ItemClothing,
		// == Doesn't fit (default item) ==
		ItemMisc
	};
	/*enum GeneralMaterialType
	{
		MaterialNone,

		MaterialWood,
		MaterialIron
	};*/

	extern const char* GetTypeName ( const WeaponItemType& );
};

typedef WeaponItem::WeaponItemType	ItemType;
typedef WeaponItem::GeneralItemType	GenItemType;
typedef	WeaponItem::WeaponRarity	ItemRarity;

#endif