
#ifndef _ITEM_TREE_TRUNK_
#define _ITEM_TREE_TRUNK_

#include "after/entities/item/CWeaponItem.h"

class ItemTreeTrunk : public CWeaponItem
{
	ClassName( "ItemTreeTrunk" );

protected:
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;
		wpdata.iMaxStack = 50;
		wpdata.sInfo = "Tree Trunk";
		wpdata.bCanStack = true;
		wpdata.iItemId = 50;
		wpdata.fWeight = 8.0f;
		iWoodType = 0;
		return wpdata;
	}
public:
	ItemTreeTrunk ( glMaterial* pInMat, const WItemData & wdat );
	ItemTreeTrunk ( const Ray& rInShape, const Ray& rInPShape, const ftype fInSize, const ftype fInPSize, glMaterial* pInMat, const WItemData & wdat );
	ItemTreeTrunk ( glMaterial* pInMat );
	ItemTreeTrunk ( const Ray& rInShape, const Ray& rInPShape, const ftype fInSize, const ftype fInPSize, glMaterial* pInMat );
	~ItemTreeTrunk ( void );

	ItemTreeTrunk ( void );
	CWeaponItem& operator =( const CWeaponItem * original ) override;

	// Function for initializing the collision/physics
	void CreatePhysics ( void );

	// Lookat calls
	void	OnInteractLookAt	( CActor* interactingActor );
	void	OnInteractLookAway	( CActor* interactingActor );

	void Update ( void );
	bool Use ( int );
protected:
	void GenerateMesh ( void );

	Ray rShapeCurrent;
	ftype fSizeCurrent;
	Ray rShapeParent;
	ftype fSizeParent;
	glMaterial* pTreeMat;

	ushort	iWoodType;
};


#endif