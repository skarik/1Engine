
#ifndef _FAUNA_KOMODO_H_
#define _FAUNA_KOMODO_H_

#include "engine-common/entities/CActor.h"

#include "after/entities/character/npc/zoned/CZonedCharacter.h"
#include "after/states/ai/NPC_AIState.h"

class CRigidBody;
class CCapsuleCollider;

namespace NPC
{
	class FaunaKomodo : public CZonedCharacter
	{
		ClassName( "FaunaKomodo" );
	public:
		DEFINE_ZCC(FaunaKomodo);
		~FaunaKomodo ( void );

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
	
		/*void AI_Lookat ( void );
		void AI_Think ( void );
		void AI_Move ( void );

		void AI_OnAngry ( void );
		void AI_OnAlert ( void );
		void AI_OnAquireTarget ( CCharacter* );

		// Moves the character to the target position
		// returns true when target reached
		bool AI_MoveTo ( const Vector3d& );
		// Attacks
		// returns true on actually attacking
		bool AI_Attack ( void );*/
		// Routines for collision detection
		CActor*	FivePointCollision ( void );


		NPC::AIState ai;


		/*void AI_Perform_Angry ( void );
		void AI_Perform_Alert ( void );

		NPC::hull_info	hull;

		struct ai_think_state_t {
			CCharacter*	target;
			Vector3d	lastSeenPosition;
			ftype		timeSinceSeen;
			Vector3d	lastHeardPosition;
			ftype		timeSinceHeard;

			// turns true when wants to kill something
			bool isAngry;
			// turns true when heard or saw something suspicious
			bool isAlerted;	
			// turns true when something suspicious happened, but didn't hear or see anything
			bool isCautious;
		
			// turns true when dead
			bool isDead;

			// Aggression counter. peaceful NPCs will not get angry until this counter increases past 1
			// non-peaceful NPCs will also not get alerted until the counter increases past 1
			ftype aggroCounter;
			// Aggro type
			//  Determines how NPCs will aggro to the player
			// type 0: default, mostly peaceful feral animals
			//		enemies will mostly ignore the target unless the target is too close for too long
			//		or the target attacks the NPC
			// type 1: enemy default, general enemies
			//		enemy will actively seek out and destroy any non-allied target it can find.
			// type 2: character NPCs
			//		will try not to aggro to targets in general unless safety is deemed hazardous, or
			//		the target has type 0 or 1 and is aggroed to self
			char aggroType;
			// Target of agression
			CCharacter*	aggroTarget;
			bool		aggroAnger;
			// Distance to begin to aggro to enemies
			ftype maxAggroDistance;

			// is true when stunned, AI will not process with calculations while stunned
			// however, the timeSince* timers will continue forward
			bool isStunned;
			ftype stunTimer;	// set when stunned, stun is removed when hits zero

			// is true when attacking, stunned, performing (scr.seq), or doing some other action
			// this is not set by the think state, only read by it
			bool isBusy;

			// Anger state
			NPC::ai_angry_state_t	state;
		};

		ai_think_state_t	ai_think;
		NPC::ai_lookat_state_t	ai_lookat;
		NPC::ai_move_state_t		ai_move;

		NPC::ai_wandering_state_t ai_wander;

		void	AI_UpdateAggro ( void );

		void	AI_Wander ( void );*/

		/*Quaternion		lookatRotation;
		Quaternion		faceatRotation;

		Quaternion		targetLookatRotation;
		Quaternion		targetFaceatRotation;*/

		ftype			fadeOutValue;

		bool bIsStunned;
		ftype fStunTimer;


		ftype	fWalkSpeed;
		ftype	fRunSpeed;

		void	MvtDefault ( void );
	};

}


#endif//_FAUNA_DESERT_BIRD_H_