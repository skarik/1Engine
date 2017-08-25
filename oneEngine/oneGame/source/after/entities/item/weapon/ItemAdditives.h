
#ifndef _ITEM_ADDITIVES_H_
#define _ITEM_ADDITIVES_H_

#include <string>
#include "core/types/types.h"

class CWeaponItem;

class ItemAdditives
{
private: ItemAdditives() {}
public:
	enum eItemAdditive : uint16_t
	{
		// Invalid
		Invalid,
		// Can't use as an additive.
		None,
		// Can't obtain additive, essentially uncraftable.
		Unobtainable,

		// Used for weapons used as components, as they can represent more than one type of material
		Composite,

		// Bad additives
		Stone,

		// Simple additives
		Wood,
		Bone,
		Iron,
		BlastIron,
		Steel,
		Crystal,
		Dragonbone,
		Dragontooth,
		Dragonsteel,
		Titanium,

		// Specific additives
		String
	};

	static eItemAdditive ItemToAdditive ( CWeaponItem* item );
	static eItemAdditive StringToAdditive ( const std::string& value );
};

#endif//_ITEM_ADDITIVES_H_