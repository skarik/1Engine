
#ifndef _PHYS_WRAPPER_MOTION_H_
#define _PHYS_WRAPPER_MOTION_H_

#include "wrapper_common.h"

enum physMotionQualityType : int
{
	HK_COLLIDABLE_QUALITY_CRITICAL,
	HK_COLLIDABLE_QUALITY_DEBRIS,
	HK_COLLIDABLE_QUALITY_MOVING,
	HK_COLLIDABLE_QUALITY_CHARACTER
};

class physMotion
{
public:
	//enum CollidableQualityType : int
	typedef physMotionQualityType CollidableQualityType;

	enum MotionType : int
	{
		MOTION_FIXED,
		MOTION_KEYFRAMED,
		MOTION_DYNAMIC,
		MOTION_CHARACTER,
	};
};

//typedef physMotion::CollidableQualityType physMotionQualityType;
typedef physMotion::MotionType physMotionType;

typedef physMotion::CollidableQualityType hkpCollidableQualityType;

#endif//_PHYS_WRAPPER_MOTION_H_