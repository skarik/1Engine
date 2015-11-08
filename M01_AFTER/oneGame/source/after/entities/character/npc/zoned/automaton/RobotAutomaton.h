
#ifndef _ROBOT_AUTOMATON_H_
#define _ROBOT_AUTOMATON_H_

#include "../CZonedCharacter.h"
#include "after/states/ai/NPC_AIState.h"

class CRigidBody;
class CCapsuleCollider;

namespace NPC
{
	class RobotAutomaton : public CZonedCharacter
	{
		ClassName( "RobotAutomaton" );
	public:
		DEFINE_ZCC(RobotAutomaton);
		~RobotAutomaton ( void );

		void UpdateActive ( void );
		void UpdateInactive ( void );

		void OnInteract ( CActor* interactingActor ) override;

		void OnAnimationEvent ( const Animation::eAnimSystemEvent eventType, const Animation::tag_t tag ) override;

		void OnDeath ( Damage const& ) override;
	
		void OnDamaged	( Damage const& hitDamage, DamageFeedback* dmgFeedback=NULL ) override;

		bool PerformAttack ( void ) override;

		// Returns a pointer to the combat target
		CCharacter*	GetCombatTarget ( void ) override;

		// Get eye ray. Commonly used for raycasts.
		Ray		GetEyeRay ( void ) override;

		Rotator GetAimRotator ( void ) override;
	private:
		// Collision and rigidbody info
		CCapsuleCollider*	pCollision;
		CRigidBody*			pRigidBody;

		void MoveUnstuck ( void );
	
		// Routines for collision detection
		CActor*	FivePointCollision ( void );

		NPC::AIState ai;

		ftype			fadeOutValue;

		bool bIsStunned;
		ftype fStunTimer;

		ftype	fWalkSpeed;
		ftype	fRunSpeed;

		void	MvtDefault ( void );
	};

}

#endif//_ROBOT_AUTOMATON_H_