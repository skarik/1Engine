#include "PrShapeBox.h"
#include "physical/interface/tobt.h"
#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "BulletCollision/CollisionShapes/btBox2dShape.h"

PrShapeBox::PrShapeBox ( const Vector3d& centered_box_size )
{
	if (centered_box_size.z > FLOAT_PRECISION)
	{
		shape = new btBoxShape( physical::bt(centered_box_size * 0.5F) );
	}
	else
	{
		shape = new btBox2dShape( physical::bt(centered_box_size * 0.5F) );
	}
}

PrShapeBox::~PrShapeBox ( void )
{
	delete shape;
	shape = NULL;
}