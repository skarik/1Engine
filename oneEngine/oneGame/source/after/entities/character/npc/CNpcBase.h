// class CNpcBase
// Base class for all persistent NPCs in the world.
// Retains information of 

#ifndef _C_NPC_BASE_H_
#define _C_NPC_BASE_H_

#include "engine/physics/motion/CRigidbody.h"
#include "engine/physics/collider/types/CCapsuleCollider.h"

#include "after/entities/character/npc/zoned/CZonedCharacter.h"
#include "after/states/player/CPlayerStats.h"
#include "after/states/ai/NPC_AIState.h"
#include "after/states/NPC_CombatState.h"
#include "after/types/character/NPC_WorldState.h"
#include "after/interfaces/CAfterCharacterAnimator.h"
#include "after/entities/item/CWeaponItem.h"

class CEmulatedInputControl;

namespace NPC
{

	class CNpcBase : public CZonedCharacter
	{
		ClassName( "CNpcBase" );
	public:
		DEFINE_ZCC(CNpcBase);
		~CNpcBase ( void );
		
		void PostLoad ( void ) override;

		// Updates
		void UpdateActive ( void );
		void UpdateInactive ( void );

		void OnInteract ( CActor* interactingActor ) override;

		void	OnDeath ( Damage const& ) override;
		void	OnDamaged	( Damage const& hitDamage, DamageFeedback* dmgFeedback=NULL ) override;
		void	OnDealDamage( Damage&, CActor* receivingCharacter ) override;
		void	OnAnimationEvent ( const Animation::eAnimSystemEvent eventType, const Animation::tag_t tag ) override;
		void	OnEquip ( CItemBase* pItem ) override;
		void	OnUnequip ( CItemBase* pItem ) override;

		// Sets the movement scaling.
		void SetMovementSpeedScale ( float ) override;

		// Apply stun. Returns true on successful stun. Ignores the second argument.
		bool	ApplyStun ( const char* n_stunname, const ftype n_stunlength ) override;
		// Apply linear motion offset. Returns true when alive.
		bool	ApplyLinearMotion ( const Vector3d& n_motion, const ftype n_motionlength ) override;

	public:
		// AI responses
		void OnTalkTo ( CCharacter* talkingCharacter ) override;

		// SpeakDialogue makes this character speak dialogue. It takes either a raw path or a sound file
		void	SpeakDialogue ( const string& soundFile ) override;
		// PerformActionList performs the actions given in the list string. See dialogue documentation.
		void	PerformActionList ( const string& actionList ) override;
		// PerformExpressionList performs the expressions given in the list string. See dialogue documentation.
		void	PerformExpressionList ( const string& expressionList ) override;

		// All Perform* functions are commands issued by the AI.

		// Attempt to attack with current equpiment
		bool PerformAttack ( void ) override;
		// Attempt to attack
		bool PerformDefend ( void ) override;

		// Get if character is allied
		bool		IsAlliedToCharacter ( CCharacter* character ) override;
		// Get if this character is an enemy
		bool		IsEnemyToCharacter ( CCharacter* character ) override;
	public:
		// ===Base Animation Interface===
		void	PlayItemAnimation ( const NPC::eItemAnimType nActionName, const int nSubset, const int nHand, const float fArg, const float fAnimSpeed=0, const float fAttackSkip=0 ) override;
		Real	GetItemAnimationLength ( const NPC::eItemAnimType nActionName, const int nSubset, const int nHand ) override;
		void	PlayAnimation ( const string& sActionName ) override;

		// Is this character performing a melee attack? The input to the function is the hand to check.
		// If the hand is being used with a melee attack, then return true.
		bool	IsAttackingMelee ( const short hand ) override;
		// Get the frame of the melee attack that the character is attacking. This is used for determining attack cancelling during melee attacks.
		// Attack cancelling is used for both combos and for sword clashing.
		int		GetAttackingFrame ( const short hand ) override;

		// ===Transform Getters===
		XTransform	GetHoldTransform ( char i_handIndex=0 ) override;
		XTransform	GetBeltTransform ( char i_beltIndex ) override;

		//	GetAI
		// returns the NPC's ai handler
		// is not const because if you want to talk with the AI, you're probably changing its state
		AIState* GetAI ( void ) { return &ai; };

		//  PartyHost
		// Party code. In AI, characters in the same party are allied, work together, and share information.
		// Concept is similar to Source squads.
		void SetPartyHost ( const uint64_t & nhost ) { m_worldstate.partyHost=nhost; };
		uint64_t GetPartyHost ( void ) { return m_worldstate.partyHost; };

		//  Stats
		// Set new race stats. This is very dangerous, and a time consuming task for the engine.
		// The character loader uses this function to set character appearances. Other behaviors unpredictable.
		void SetRaceStats ( CRacialStats* n_stats_to_copy );

		// Returns a pointer to misc stats
		CRacialStats*	GetRacialStats ( void ) override {
			return m_stats;
		}

	public:
		// Get eye ray. Commonly used for raycasts.
		Ray			GetEyeRay ( void ) override;
		// Get aiming direction. Used for melee weapons.
		Rotator		GetAimRotator ( void ) override;
		// Get aiming arc. Used for melee weapons.
		Vector4d	GetAimingArc ( void ) override;

		// Returns a pointer to the shield. Returns NULL if no shield.
		SkillShield*	GetShield ( void ) override;
		void	SetShield ( SkillShield* ) override;
		// Returns a list of all the casting spells in a vector. Default implemenation returns a list of empty skills.
		std::vector<CSkill*>	GetCastingSkills ( void ) override;

		// Returns a pointer to the combat target
		CCharacter*	GetCombatTarget ( void ) override;
		// Returns type of character (0 animal, 1 monster, 2 sentinent)
		NPC::eAggroAIType GetAggroType ( void ) override { return NPC::AGGRO_CHARACTER; }
		// Returns the faction info
		eCharacterFaction	GetFaction ( void ) { return m_worldstate.mFaction; }
	private:
		//  StunWithAnimation()
		// Stuns the player with the animation. Will stun for entire duration of animation.
		// Returns true when animation is found.
		bool	StunWithAnimation ( const string& nStunName, const ftype nTimescale=1.0 );

		// Collision and rigidbody info
		CCapsuleCollider*	pCollision;
		CRigidBody*			pRigidBody;

		void MoveUnstuck ( void );
			
		// CHARACTER STATS
		CRacialStats*		m_stats;
		CRacialStats*		base_race_stats;

		// NPC STATS
		sWorldState			m_worldstate;
		sPreferences		m_prefs;
		sOpinions			m_opinions;

		// Emulated input for the character control
		CEmulatedInputControl* input;

		AIState			ai;
		sCombatInfo		combatInfo;
		CAfterCharacterAnimator	animator;

		// GAMEPLAY
		CPlayerInventory*	inventory;
		SkillShield*		m_shield;

		ftype				m_aiming_angle;

private:
		// MOVEMENT
		bool	bIsProne;
		bool	bIsCrouching;
		bool	bCanUncrouch;
		bool	bCanUnprone;
		bool	bWantsCrouch;
		bool	bWantsProne;
		bool	bIsSprinting;
		bool	bCanSprint;

		float	fCurrentHeight;
		float	fSlideCounter;

		// Current stun timer to count when to disable the stun
		ftype		fStunTimer;
		bool		bIsStunned;
		std::list<Vector4d>	vLinearMotions;
		ftype		fMovementMultiplier;

		void	MvtInit ( void );

		void	MvtPerform ( void );

		void	MvtCommonCrouching ( void ); //Common movement routine, such as crouching
		void	MvtSetPlayerHeightStick ( float fnPlayerHeight );
	};

}

#endif