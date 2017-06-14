
#ifndef _C_WEAPON_ITEM_COMPONENTS_H_
#define _C_WEAPON_ITEM_COMPONENTS_H_

class CWeaponItem;
class CActor;

#include "core/common.h"
#include "core/types/types.h"

namespace WeaponItem
{
	template <class PropClass>
	CTransform* ComponentPlacement ( CActor* owner, CWeaponItem* item, Vector3d* outNormal=NULL );
};

#include "CWeaponItemComponents.hpp"

#endif