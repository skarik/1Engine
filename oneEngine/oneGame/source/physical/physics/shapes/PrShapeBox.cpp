#include "PrShapeBox.h"
#include "physical/interface/tobt.h"
#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "BulletCollision/CollisionShapes/btBox2dShape.h"

PrShapeBox::PrShapeBox ( const Vector3f& centered_box_size )
{
	if (centered_box_size.z > FLOAT_PRECISION)
	{
		shape = new btBoxShape( physical::bt(centered_box_size * 0.5F) );
	}
	else
	{
		shape = new btBoxShape( physical::bt(Vector3f(centered_box_size.x, centered_box_size.y, 32) * 0.5F) );
	}
}

PrShapeBox::~PrShapeBox ( void )
{
	delete shape;
	shape = NULL;
}