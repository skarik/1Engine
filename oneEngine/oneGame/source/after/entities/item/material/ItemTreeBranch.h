
#ifndef _ITEM_TREE_BRANCH_
#define _ITEM_TREE_BRANCH_

#include "ItemTreeTrunk.h"
#include "renderer/material/glMaterial.h"
#include "engine/physics/raycast/Raycaster.h"
#include "after/entities/item/CWeaponItem.h"

class ItemTreeBranch : public ItemTreeTrunk
{
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;
		wpdata.iItemId = 51;
		wpdata.sInfo = "Tree Branch";
		wpdata.bCanStack = 1;
		wpdata.iMaxStack = 100;	
		wpdata.fWeight = 4.0f;
		iWoodType = 0;
		return wpdata;
	}
public:
	ItemTreeBranch ( glMaterial* pInMat );
	ItemTreeBranch ( const Ray& rInShape, const Ray& rInPShape, const ftype fInSize, const ftype fInPSize, glMaterial* pInMat );

	ItemTreeBranch ( void );
};

#endif