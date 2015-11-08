
#ifndef _C_ANIM_IK_H_
#define _C_ANIM_IK_H_

#include "core/types/types.h"
#include "core/math/Vector3d.h"

#include <string>
using std::string;

enum eAnimIKType
{
	// LookAt IK used for heads
	// bone[0] is the head bone
	IK_LOOKAT	= 0,
	// Footstep IK is used for placing feet on the ground
	// bone[0] is foot bone, bone[1] is middle joint, bone[2] is the thigh
	// input is used to dictate target offset
	IK_FOOTSTEP	= 1,
	// Aiming IK is used for moving the spine and upper arm joints so that the target is aimed at correctly.
	// bone[0] is base of spine
	// bone[1] is middle spine joint
	// bone[2] is the top spine joint
	// bone[3] is the left upper arm
	// bone[4] is the right upper arm
	IK_AIMING	= 2,
	// Prop IK is used to keep characters holding props properly.
	// bone[0] and subinfo[0] apply to prop 1
	// bone[1] and subinfo[1] apply to prop 1
	// bone[2] and subinfo[2] apply to prop 1
	// bone[3] and subinfo[3] apply to prop 1
	IK_PROPS	= 3
};

struct ikinfo_t
{
	eAnimIKType		type;
	string			name;
	bool			enabled;
	Vector3d		input;
	Vector3d		subinput0;
	Vector3d		subinput1;
	ftype			subinfo[4];
	uint32_t		bone [5];
};


#endif