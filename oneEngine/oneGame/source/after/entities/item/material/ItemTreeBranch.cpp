
#include "ItemTreeBranch.h"

ItemTreeBranch::ItemTreeBranch ( glMaterial* pInMat )
	: ItemTreeTrunk ( pInMat, ItemData() )
{

}
ItemTreeBranch::ItemTreeBranch ( const Ray& rInShape, const Ray& rInPShape, const ftype fInSize, const ftype fInPSize, glMaterial* pInMat )
	: ItemTreeTrunk ( rInShape, rInPShape, fInSize, fInPSize, pInMat, ItemData() )
{

}


ItemTreeBranch::ItemTreeBranch ( void )
	: ItemTreeTrunk( Ray(Vector3d(0,0,0),Vector3d(0,0,2)), Ray(Vector3d(0,0,0),Vector3d(0,0,2)), 0.4f,0.5f, NULL, ItemData() )
// : ItemTreeTrunk( NULL, ItemData() )
{
	/*rShapeCurrent.dir = Vector3d( 0,0,2 );
	rShapeCurrent.pos = Vector3d( 0,0,0 );
	rShapeParent.dir = Vector3d( 0,0,2 );
	rShapeParent.pos = Vector3d( 0,0,0 );

	fSizeCurrent = 0.4f;
	fSizeParent = 0.5f;*/
}