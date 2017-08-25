
#ifndef _C_ACTOR_ANIMATION_H_
#define _C_ACTOR_ANIMATION_H_

#include "engine-common/types/ActorAnimation.h"

namespace NPC
{
	enum eMoveAnimType : animtype_t
	{
		MoveAnimFall,
		MoveAnimFallSlide,

		MoveAnimSwimIdle,
		MoveAnimSwimMove,
		MoveAnimSwimTread,

		MoveAnimProneIdle,
		MoveAnimProneMove,

		MoveAnimCrouchIdle,
		MoveAnimCrouchMove,
		MoveAnimCrouchSprint,

		MoveAnimWalkIdle,
		MoveAnimWalkWalk,		// Walking (60 base)
		MoveAnimWalkMove,		// Running
		MoveAnimWalkSprint,		// Sprinting

		MoveAnimMoveWallrunLeft,
		MoveAnimMoveWallrunRight,

		MoveAnim_USER
	};
	typedef eMoveAnimType MoveAnim;
	enum class eItemAnimType : animtype_t
	{
		Draw,
		Idle,
		Toss,
		Holster,
		Cast,
		Precast,
		Attack,
		Quickstrike,
		Charge,
		Defend,
		DefendStop,
		Parry,

		PunchNeutral,
		PunchNeutralMiss,
		PunchNeutralBreak,

		h2hPunch_L,
		h2hPunch_R,
		h2hKick_L,
		h2hKick_R,

		h2hInterrupt,

		INVALID = 0xFFFF
	};
	typedef eItemAnimType ItemAnim;
};

#endif//_C_ACTOR_ANIMATION_H_