
#include "ItemTreeTwig.h"

ItemTreeTwig::ItemTreeTwig ( glMaterial* pInMat )
	: ItemTreeTrunk ( pInMat, ItemData() )
{
	
}
ItemTreeTwig::ItemTreeTwig ( const Ray& rInShape, const Ray& rInPShape, const ftype fInSize, const ftype fInPSize, glMaterial* pInMat )
	: ItemTreeTrunk ( rInShape, rInPShape, fInSize, fInPSize, pInMat, ItemData() )
{
	
}

ItemTreeTwig::ItemTreeTwig ( void )
	: ItemTreeTrunk( Ray(Vector3d(0,0,0),Vector3d(0,0,2)), Ray(Vector3d(0,0,0),Vector3d(0,0,2)), 0.1f,0.2f, NULL, ItemData() )
// : ItemTreeTrunk( NULL, ItemData() )
{
	/*rShapeCurrent.dir = Vector3d( 0,0,2 );
	rShapeCurrent.pos = Vector3d( 0,0,0 );
	rShapeParent.dir = Vector3d( 0,0,2 );
	rShapeParent.pos = Vector3d( 0,0,0 );

	fSizeCurrent = 0.2f;
	fSizeParent = 0.3f;*/
}