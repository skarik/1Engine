
#ifndef _X_TRANSFORM_H_
#define _X_TRANSFORM_H_

#include "Vector3d.h"
#include "Quaternion.h"

//	XTransform : 10 Float Structure for animation
struct XTransform
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
static_assert(sizeof(XTransform) == 40, "Invalid size on XvTransform!");

struct XvTransform	// 9 float structure
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
static_assert(sizeof(XvTransform) == 36, "Invalid size on XvTransform!");

//	XrTransform : 16 Float Structure for calculation
struct ALIGNAS(64) XrTransform	// 16 float structure
{
	XrTransform ( const Vector3d & pos =Vector3d(), const Rotator & rot =Rotator(), const Vector3d & scal =Vector3d() )
		: position( pos ), scale( scal ), rotation( rot )
	{
		;
	}
	Vector3d position;
	Vector3d scale;
	Rotator rotation;
};
//static_assert(sizeof(XrTransform) == 64, "Invalid alignment on XrTransform!");


#endif