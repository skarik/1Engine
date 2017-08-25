
#ifndef _FAUNA_DESERT_BIRD_H_
#define _FAUNA_DESERT_BIRD_H_

#include "after/entities/character/npc/zoned/CZonedCharacter.h"
#include "core/math/Vector3d.h"

namespace NPC
{
	class FaunaDesertBird : public CZonedCharacter
	{
		ClassName( "CCharTest" );
	public:
		DEFINE_ZCC(FaunaDesertBird);
		~FaunaDesertBird ( void );

		void UpdateActive ( void );
		void UpdateInactive ( void );

	private:
		// Collision and rigidbody info
		//CCapsuleCollider*	pCollision;
		//CRigidBody*			pRigidBody;
		//REGISTER_ZONED_CHARACTER(CCharTest);
		//void MoveUnstuck ( void );

		void UpdateMovement ( void );

		Vector3d	targetPosition;
		Vector3d	velocity;
		Vector3d	acceleration;
	};
}

#endif//_FAUNA_DESERT_BIRD_H_