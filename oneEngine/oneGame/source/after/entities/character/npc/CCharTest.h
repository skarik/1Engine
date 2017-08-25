// Test class for any non-persistent character.
// It'll 'die' after a few hits, supposedly.

#ifndef _C_CHAR_TEST_H_
#define _C_CHAR_TEST_H_

#include "after/entities/character/npc/zoned/CZonedCharacter.h"

#include "engine/physics/motion/CRigidbody.h"
#include "engine/physics/collider/types/CCapsuleCollider.h"

#include "after/states/player/CPlayerStats.h"

namespace NPC
{

	class CCharTest : public CZonedCharacter
	{
		ClassName( "CCharTest" );
	public:

		DEFINE_ZCC(CCharTest);
		~CCharTest ( void );

		void UpdateActive ( void );
		void UpdateInactive ( void );

		void OnInteract ( CActor* interactingActor ) override;

	public:
		// Get eye ray. Commonly used for raycasts.
		Ray		GetEyeRay ( void ) override;

	private:
		// Collision and rigidbody info
		CCapsuleCollider*	pCollision;
		CRigidBody*			pRigidBody;

		//REGISTER_ZONED_CHARACTER(CCharTest);
		void MoveUnstuck ( void );

		//CPlayerStats*		m_stats;
		CRacialStats*		m_stats;

		CCharacter*			pLookAtCharacter;
		bool		bLookingAtCharacter;
		bool		bTalkingAtCharacter;
		Vector3d	vLookatPos;
		Vector3d	vCurrentLookatPos;
		ftype		fGlanceAwayTimer;
		Vector3d	vGlanceAwayAngle;
		Vector3d	vCurrentGlanceAwayAngle;
		//Rotator		mFacing;
		//Rotator		mCurrentFacing;
		Vector3d	vFacingPos;
		Vector3d	vCurrentFacingPos;
	
		ftype		fAttentionDistance;

		ftype		fLookAwayTimer;
	
		bool		bCharacterInView;
		bool		bCharacterInRange;

		struct ai_think_state_t {
			bool isAngry;
			bool isAlerted;
			CCharacter*	target;
		};
		struct ai_lookat_state_t {
			CCharacter*		target;
			bool			bLookingAtCharacter;
			bool			bTalkingAtCharacter;
			Vector3d		vLookatPos;
			Vector3d		vCurrentLookatPos;

			ftype			fGlanceAwayTimer;
			Vector3d		vGlanceAwayAngles;

			Vector3d		vFacingPos;
			Vector3d		vCurrentFacingPos;
		};
	};

}
#endif