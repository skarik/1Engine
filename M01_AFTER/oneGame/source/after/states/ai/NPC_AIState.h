
#ifndef _NPC_AI_STATE_H_
#define _NPC_AI_STATE_H_

#include "core/common.h"
#include "core/types/types.h"
#include "core/math/Vector3d.h"
#include "core/math/Ray.h"
#include "core/containers/arstring.h"
#include "after/states/NPC_CombatState.h"
#include "after/types/character/NPC_AIFocus.h"
#include "after/types/character/AggroEnum.h"

#include <vector>

class CCharacter;
class CRigidBody;

#include "engine-common/types/Damage.h"

namespace NPC
{
	class CZonedCharacter; // Class prototype

	// struct hull_info
	// General collision hull info for AI's.
	struct hull_info {
		ftype	height;
		ftype	radius;
	};

	/*struct ai_angry_state_t {
		int		mode;

		Vector3d seekOffset;
		Vector3d circleOffset;

		static const int Seeking = 0;
		static const int Circling = 1;
		static const int Attack = 2;

		bool	performedAttack;
		bool	waitOnAttackResult;
		bool	attackResultHit;
		ftype	waitResultTimer;

		int		circlingDirection;
		ftype	circlingTime;
		ftype	nextCirclingTime;
	};*/
	/*struct ai_aggro_info_t {
		// Aggression counter. peaceful NPCs will not get angry until this counter increases past 1
		// non-peaceful NPCs will also not get alerted until the counter increases past 1
		ftype aggroCounter;
		// Target of agression
		CCharacter*	target;
		// if target is valid pointer
		bool		valid;
		// if target is allied
		bool		targetAllied;
		// if target is hostile
		bool		targetHostile;
		// turns true when suspcious of target
		bool		targetSuspicious;
		// turns true when wants to kill target
		bool		isAngryAt;
		// turns true when saw die
		bool		sawDead;
		// Keeping track of the target
		Vector3d	lastSeenPosition;
		ftype		timeSinceSeen;
		Vector3d	lastHeardPosition;
		ftype		timeSinceHeard;
		// Keeping track of the threat level
		ftype		threatTimer;
		ftype		threatLevel;
		ftype		timeBothered;
		// Keeping track of the aggro level (threat level)
		ftype		aggroLevel;

		bool operator< ( const ai_aggro_info_t& second ) const {
			return second.aggroLevel < aggroLevel;
		}
	};*/

	// Aggression times
	struct ai_aggro_properties_t
	{
		ftype		alertTime;
		ftype		angerTime;

		ftype		alertCooldownTime;
		ftype		angerCooldownTime;

		ftype		alertAggro;
		ftype		angerAggro;
	};
	// Combat capabilities
	struct ai_combat_properties_t
	{
		ftype		meleeRange;
		ftype		rangedRange;
		ftype		magicRange;

		bool		hasMeleeWeapon;
		bool		hasRangedWeapon;
		bool		hasMagicWeapon;
	};
	// Lookat options
	struct ai_lookat_properties_t
	{
		bool		glanceAway;
		ftype		glanceAwayStart;
		ftype		glanceAwayRate;

		bool		lookAway;
		ftype		lookAwayStart;
		ftype		lookAwayRate;

		ftype		headTurnSpeed;
		ftype		eyeTurnSpeed;

		// Default options constructor
		ai_lookat_properties_t();
	};

	struct ai_memory_postion_info_t {
		Vector3d	position;
		ftype		time;
	};
	struct ai_memory_target_state_t
	{
		// Target of agression
		CCharacter*	target;
		// if target is valid pointer
		bool		target_valid;

		// Aggression counter. peaceful NPCs will not get angry until this counter increases past 1
		// non-peaceful NPCs will also not get alerted until the counter increases past 1
		ftype aggroCounter;

		// Keeping track of time since seen and time since heared
		ai_memory_postion_info_t	seen;
		ai_memory_postion_info_t	heard;
		
		// Constants for when target_valid is false
		// if target is allied
		bool		targetAllied;
		// if target is hostile
		bool		targetHostile;
		// type of enemy
		eAggroAIType	targetType;

		// turns true when suspcious of target
		bool		targetSuspicious;

		// State information
		enum eMemoryState {
			MEM_UNKNOWN,
			MEM_VISIBLE,
			MEM_HIDDEN
		};
		eMemoryState	topicalState;

		// turns true when wants to kill target (depending on type of target, will eventually turn false again)
		bool		isAngryAt;
		// turns true when saw die
		bool		sawDead;

		// Keeping track of the threat level
		ftype		threatTimer;
		ftype		threatLevel;
		ftype		timeBothered;

		// Keeping track of the aggro level (threat level)
		ftype		aggroLevel;
		// Sort via the threat level
		bool operator< ( const ai_memory_target_state_t& second ) const {
			return second.aggroLevel < aggroLevel;
		}
	};

	struct ai_think_state_t
	{
		// Aggro timer
		ftype		aggroUpdateTime;
		ftype		aggroTimer;

		// Current info state of the AI.
		enum eMainThinkAIState : uint8_t
		{
			AI_RELAXED			= 0,
			AI_ALERT			= 1,
			AI_SEARCHING		= 2,
			AI_ANGRY			= 4
		};
		eMainThinkAIState	infostate;
		
		// is true when dead
		// this is not set by the think state, only read by it
		bool isDead;
		// is true when stunned, AI will not process with calculations while stunned
		// however, the timeSince* timers will continue forward
		// this is not set by the think state, only read by it
		bool isStunned;
		// is true when attacking, stunned, performing (scr.seq), or doing some other action
		// this is not set by the think state, only read by it
		bool isBusy;

		// Aggression counter. peaceful NPCs will not get angry until this counter increases past info_aggro's time
		// non-peaceful NPCs will also not get alerted until the counter increases past info_aggro's time
		ftype aggroCounter;
		// Peace counter.  NPCs will not calm down until the counter increases past info_aggro's time.
		ftype peaceCounter;
		// Aggrotype
		eAggroAIType aggroType;

		// Targets of aggression
		int	target;
		std::vector<ai_memory_target_state_t>	aggroTargets;
		// Distance to begin to aggro to enemies
		ftype maxAggroDistance;

		// Topical (conversational) state
		enum eTopicalAIState {
			Focus,
			Greet,
			Talk
		};
		eTopicalAIState		relax_state;
	};

	// Keeps track of AI attack responses
	struct ai_attack_response_t
	{
		// == OUTPUTS (FROM HERE) ==

		// If sent attack request, and waiting for input.
		bool	atk_waitForResult;

		// == INPUTS (TO HERE) ==

		// If was able to attack
		bool	atk_performed;
		// If attack hit
		bool	atk_hit;

		// == VARIABLES ==

		// Each attack is given an approximate time based on the current method of attack.
		// This is the time that the system will wait for a response.
		// This ATK query check is done C++ side. The Lua calls Check_AI_
		ftype	atk_waittime;
	};
	
	struct ai_lookat_state_t {
		CCharacter*		target;
		bool			targetInRange;
		bool			targetInView;

		bool			isLookingAtCharacter;
		bool			isTalkingAtCharacter;

		Vector3d		lookatPos;
		ftype			lookAwayTimer;
		ftype			lookAwayWanderTimer;
		Vector3d		lookAwayWanderOffset;
		ftype			lookatOverride;

		Vector3d		glanceAwayAngle;
		ftype			glanceAwayTimer;

		Vector3d		facingPos;

		Vector3d		walkingLookatPos;
		Vector3d		walkingFacingPos;
		ftype			walkingLookTimer;
		bool			walkingLook;
		bool			walkingFace;

		Vector3d		currentGlanceAwayAngle;
		Vector3d		currentLookatPos;
		Vector3d		currentFacingPos;

		//ftype			maxAttentionDistance;
		ftype			maxTalkDistance;

		ftype			minDotView;
	};
	struct ai_move_feedback_state_t {
		bool	isWalking;
		bool	isRunning;
		bool	wantsCrouch;
		bool	wantsProne;

		Vector3d	final_target;
	};
	struct ai_move_state_t {
		// Constants
		//ftype	walkSpeed;
		//ftype	runSpeed;

		// Current States
		ai_move_feedback_state_t	state;
		
		ftype	wants_reset_timer;

		Vector3d	target;
		bool	validTarget;

		// Path information
		int			path_pos;
		Vector3d	path_target;
		ftype		path_time;
		std::vector<Vector3d>	path;
	};

	struct ai_wandering_state_t {
		Vector3d	wanderHeading;
		Vector3d	wanderHeadingOffset;

		bool	avoidingObstacle;
		int		avoidDirection;
		ftype	avoidTime;
	};
	struct ai_talk_state_t {
		ftype		greetTimer;
		CCharacter*	followTarget;
	};

	class AIState;
	class AIAccessor;

	//	class AIFocus
	// Class for specializing NPC behavior.
	class AIFocus
	{
	public:
		// Constructor
		explicit AIFocus ( AIState* n_ai ) : ai(n_ai) {}
		virtual ~AIFocus ( void ) { Destruction(); }
		
		// Virtual function for creation
		virtual void Initialize ( void ) {}
		// Virtual function for destruction
		virtual void Destruction ( void ) {}

		// Pure virtual function for behavior
		virtual bool Execute ( void ) =0;

		// Virtual functions for party events
		virtual void PartyCmdFollowMe ( CCharacter* ) {}
		virtual void PartyCmdMoveTo ( const Vector3d& ) {}

		// Target AI that working with
		AIState*	ai;
	};

	//  class AIState
	// Class for unified AI control, pathfinding, and general NPC behavior.
	class AIState : public CCallbackObserver
	{
	public:
		AIState ( void );
		~AIState ( void );

		//	Initialize()
		// Startup routine for the AI. Should be called after all the custom options have been set.
		void Initialize ( void );

		//	Think()
		// Main routine for the AI.
		void Think ( void );

		// == Setters ==
		// Set isDead
		void SetIsDead ( const bool n_dead )
		{
			ai_think.isDead = n_dead;
		}
		// Set isStunned
		void SetIsStunned ( const bool n_stunned )
		{
			ai_think.isStunned = n_stunned;
		}
		// Set isBusy
		void SetIsBusy ( const bool n_busy )
		{
			ai_think.isBusy = n_busy;
		}
		// Set aggro type
		void SetAggroType ( const eAggroAIType n_aggroType )
		{
			ai_think.aggroType = n_aggroType;
		}

		//	SetFocus()
		// Changes the AI's focus. It will delete the current AI's focus and then create a new one.
		// If the focus is already set to the same one, it will recreate regardless. Thus, this should
		// not be called every frame.
		void SetFocus ( const eBasicAIFocus nnew_focus, const char* nnew_routine=NULL );

		// == Getters ==
		//  GetFocus()
		// Gets the AI's focus.
		void GetFocus ( eBasicAIFocus& o_focus, char* o_routine );

		//	Vector3d MoveGetTarget
		// Returns the target position to move to that the AI_Move routine has calculated.
		//Vector3d	GetMoveTarget ( void );

		// Returns the move feedback
		ai_move_feedback_state_t& GetMoveFeedback ( void );

		// == Queries ==
		// Returns the current main aggro target, but only if in combat.
		// Returns NULL if the main target doesn't match this criteria.
		CCharacter* QueryAggroTarget ( void ) const;
		// Return the current facing target
		Vector3d QueryFacingPosition ( void ) const;
		// Return the current facing target
		Vector3d QueryLookatPosition ( void ) const;
		// Return the current facing target
		Vector3d QueryEyeGlanceAngles ( void ) const;

		// == Requests ==
		// Request an interation with the actor, querying the AI_Greet/AI_Talk states.
		// Returns true on successful talk switch. Returns false if actor is not relaxed.
		bool RequestActorInteract ( CCharacter* interactingCharacter, const bool n_interact );
		// Request a panic in the AI, for response to when stunned by an attack.
		// Returns true if upped the state of affairs (shit just got real)
		bool RequestStunPanic ( void );
		// Request a new infostate for the AI_Think module.
		bool RequestInfoState ( const ai_think_state_t::eMainThinkAIState );
		// Request a position to look at, and override the AI lookat position
		// Returns true pretty much always, unless stunned.
		bool RequestLookatOverride ( const Vector3d& n_targetPosition );

		// == General Events ==
		//	OnAquirePossibleTarget( CCharacter* target, (optional) force aggro )
		// Inputs on happenings
		void OnAquirePossibleTarget ( CCharacter*, bool nforceAggro = false );
		//	OnGetDamaged( Damage target )
		// Call when the owner character is damaged.
		void OnGetDamaged ( Damage const& hitDamage );
		//	OnGreet()
		// Input for greeting
		void OnGreet ( CCharacter* );

		// == Party Command Events ==
		// Called when wants a follow. Calling character is passed in.
		void PartyCmdFollowMe ( CCharacter* );
		// Called when wants a move to. Target position to move to is passed in.
		void PartyCmdMoveTo ( const Vector3d& );
	private:
		// Main AI Routines
		void	AI_Lookat ( void );
		void	AI_Think ( void );
		void	AI_Move ( void );
		// Main AI Subroutines
		void	AI_OnAngry ( void );
		void	AI_OnAlert ( void );

		// Moves the character to the target position by modifying the AI targets
		// returns true when target reached
		bool AI_MoveTo ( const Vector3d&, bool nAllowSprint=false );
		// Attacks
		// returns true on actually attacking
		bool	AI_Attack ( void );

		void	AI_Perform_Angry ( void );
		void	AI_Perform_Alert ( void );

		void	AI_UpdateAggro ( void );
		void		AI_GenerateAggro ( ai_memory_target_state_t& o_aggro_state, const bool n_bias );
		void		AI_Aggro_State ( const bool n_addAggro, const bool n_addAnger );

		// Topical AI States
		void	AI_Wander ( void );
		void	AI_Greet ( void );
		void	AI_Talk ( void );

		void	AI_Follow ( void );

		// Callback for on target attack. Takes a void* of the character.
		int cb_AIState_OnTargetAttack ( void* );

		// Utitily to check if target is in view
		bool isInView ( const Vector3d& nTargetPos, const ftype nViewDistance );
		bool isInViewAngle ( const Vector3d& nTargetPos, const ftype nViewAngleDot );
	public:
		// Public settable properties
		CZonedCharacter*		owner;
		CRigidBody*				rigidbody;

		hull_info				hull;
		ai_aggro_properties_t	info_aggro;
		ai_combat_properties_t	info_combat;
		ai_lookat_properties_t	info_lookat;

		// Lua routine info
		arstring<128>		routine_alert;
		arstring<128>		routine_angry;
	private:
		ai_think_state_t	ai_think;
		ai_lookat_state_t	ai_lookat;
		ai_move_state_t		ai_move;

		ai_wandering_state_t ai_wander;
		ai_talk_state_t		ai_talk;

		// Lua Routine information. (This should be changed).
		//bool				routine_alert_use_c;
		arstring<128>		routine_alert_environment;
		//bool				routine_angry_use_c;
		arstring<128>		routine_angry_environment;

	public:
		// Public AI feedback
		ai_attack_response_t	response_atk;

	private:
		AIFocus*			ai_focus;
		eBasicAIFocus	i_focus_index;
		arstring<64>	i_focus_routine;

	private:
		Ray			ownerEyeRay;
		bool		haveTarget;

	private:
		friend AIAccessor;
		friend AIFocus;

		class AIF_WandererBehavior : public AIFocus
		{
		public:
			explicit AIF_WandererBehavior ( AIState* n_ai ) : AIFocus(n_ai) {;}
			bool Execute ( void ) override;
		};
		class AIF_CompanionBehavior : public AIFocus
		{
		public:
			explicit AIF_CompanionBehavior ( AIState* n_ai ) : AIFocus(n_ai) {;}

			void Initialize ( void ) override;
			void Destruction ( void ) override;
			bool Execute ( void ) override;

			// Virtual functions for party events
			void PartyCmdFollowMe ( CCharacter* ) override;
			void PartyCmdMoveTo ( const Vector3d& ) override;
		private:
			arstring<128> m_environment;
		};
		class AIF_GeneralLuaBehavior : public AIFocus
		{
		public:
			explicit AIF_GeneralLuaBehavior ( AIState* n_ai, const char* n_behaviorName ) : AIFocus(n_ai), m_behavior(n_behaviorName) {;}

			void Initialize ( void ) override;
			void Destruction ( void ) override;
			bool Execute ( void ) override;

		private:
			arstring<128> m_environment;
			arstring<128> m_behavior;
		};

	};

	class AIAccessor 
	{
	public:
		explicit AIAccessor ( AIState* n_ai ) : ai(n_ai) {}

		// Set character to follow
		FORCE_INLINE void	SetFollowTarget ( CCharacter* nCharacter ) {
			ai->ai_talk.followTarget = nCharacter;
		}
		// Moves the character to the target position
		// returns true when target reached
		FORCE_INLINE bool	MoveTo ( const Vector3d& vTargetPos, bool nAllowSprint ) {
			return ai->AI_MoveTo( vTargetPos, nAllowSprint );
		}

		// Topical AI States
		FORCE_INLINE void	Wander ( void ) {
			ai->AI_Wander();
		}
		FORCE_INLINE void	Follow ( void ) {
			ai->AI_Follow();
		}

		// Custom common routines
		void PerformAttack ( void );
		void PerformDefend ( void );
		void FaceAt ( const Vector3d& vTargetPos );

		void SetFocus ( const int, const char* );

	private:
		AIState*	ai;
	};
};

#endif//_NPC_AI_STATE_H_