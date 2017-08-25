
#ifndef _ITEM_TREE_TWIG_
#define _ITEM_TREE_TWIG_

#include "ItemTreeTrunk.h"

class ItemTreeTwig : public ItemTreeTrunk
{

	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;
		wpdata.iItemId = 52;
		wpdata.sInfo = "Twig";
		wpdata.bCanStack = 1;
		wpdata.iMaxStack = 200;
		wpdata.fWeight = 2.0f;
		iWoodType = 0;
		return wpdata;
	}
public:
	ItemTreeTwig ( glMaterial* pInMat );
	ItemTreeTwig ( const Ray& rInShape, const Ray& rInPShape, const ftype fInSize, const ftype fInPSize, glMaterial* pInMat );

	ItemTreeTwig ( void );

};

#endif