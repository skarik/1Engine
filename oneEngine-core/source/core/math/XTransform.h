
#ifndef _X_TRANSFORM_H_
#define _X_TRANSFORM_H_

#include "Vector3d.h"
#include "Quaternion.h"

struct XTransform	// 10 byte structure
{
	XTransform ( const Vector3d & pos =Vector3d(), const Quaternion & rot =Quaternion(), const Vector3d & scal =Vector3d() )
		: position( pos ), rotation( rot ), scale( scal )
	{
		;
	}
	Vector3d position;
	Quaternion rotation;
	Vector3d scale;
};
struct XvTransform	// 9 byte structure
{
	XvTransform ( const Vector3d & pos =Vector3d(), const Vector3d & rot =Vector3d(), const Vector3d & scal =Vector3d() )
		: position( pos ), rotation( rot ), scale( scal )
	{
		;
	}
	Vector3d position;
	Vector3d rotation;
	Vector3d scale;
};

#endif