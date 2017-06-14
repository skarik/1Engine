// Character class. Has health, can have debuffs.
// Typically uses a living model to represent itself
// Differ from actors in that they are usually living. They can communicate and usually have stats of some sort.
// Their interface is much more full in terms of items, communication, and AI.

#ifndef _C_CHARACTER_H_
#define _C_CHARACTER_H_

// Includes
#include "engine-common/entities/CActor.h"

#include "after/states/CharacterStats.h"
#include "after/types/CharHealth.h"
#include "after/states/debuffs/Debuffs.h"
#include "after/types/DebuffChance.h"
#include "after/states/NPC_CombatState.h"
#include "core/callbacks/Events.h"
#include "after/types/Experience.h"
#include "after/types/character/AggroEnum.h"
#include "after/types/character/Animation.h"
#include "after/types/character/TypeEnum.h"

#include "core-ext/animation/CAnimationEvents.h"

class CCharacterModel;
class CRacialStats;
class CPlayerStats;
class CSkill;
class SkillShield;
namespace NPC {
	enum eGeneralSpeechType;
}
class CCharacterMotion;

// Class Definition
class CCharacter : public CActor
{
	ClassName( "CCharacter" );
	BaseClass( "CActor_Character" );
public:
	// Mark the type as a character actor
	virtual eActorObjectType ActorType ( void ) override { return ACTOR_TYPE_CHARACTER; }
	virtual NPC::eCharacterType CharacterType ( void ) { return NPC::CHARACTER_TYPE_GENERAL; }

	// Constructor
	explicit CCharacter ( void )
		: CActor(), model( NULL )
	{
		layer = Layers::Actor | Layers::Character;
		bAlive = true;
		memset( charName, 0, 512 );

		msList.push_back( this ); // Add this to character list
	};
	// Destructor
	//~CCharacter ( void ) { /*delete stats;*/ };
	~CCharacter ( void )
	{
		msList.erase( find( msList.begin(), msList.end(), this ) ); // Remove this from character list
	};

	// Step functions
	void Update ( void )
	{
		if ( stats.fHealth <= 0 ) {
			if ( bAlive ) {
				bAlive = false;
				OnDeath( killDamage );
			}
		}

		//stats.fHealth = charHealth;
		debuffs.UpdateParent( this, &stats );
		debuffs.UpdatePreStats();
		stats.UpdateStats();
		debuffs.Update();
		stats.UpdatePostDebuff();
		debuffs.UpdatePostStats();
		//charHealth = stats.fHealth;
		//charHealth.SetMax( stats.fHealthMax );
	}
	void LateUpdate ( void ) {};

	void FixedUpdate ( void ) {};

	// Is this an actor or a character?
	bool IsCharacter ( void ) override
	{
		return true;
	}

	CCharacterModel*	model;
public:
	// ==='Callbacks'===
	// Note that OnDamaged doesn't actually mean that damage has been done. It just means that
	//  damage has been applied. If you override this function, you can give certain damages
	//  a special effect.
	// The default behavior is to run the damage through the debuffs.
	virtual void	OnDamaged	( const Damage& hitDamage, DamageFeedback* dmgFeedback=NULL ) override
	{
		Damage finalDamage = debuffs.OnDamaged( hitDamage );
		stats.fHealth -= finalDamage.amount;
		if ( stats.fHealth <= 0 )
		{
			killDamage = hitDamage;
		}
	}
	// OnAnimationEvent is called by associated character models or manually.
	// This is where footsteps, attacks, and other events are sent to be handled.
	// Event names are defined in the AD file of unpackaged models, so make sure you match names
	//  with the defined animations. If an event is not called, it is likely because the name in
	//  the AD file was misspelled.
	// This system will eventually be replaced with a hard-coded enumeration system to prevent this issue.
	virtual void	OnAnimationEvent ( const Animation::eAnimSystemEvent eventType, const Animation::tag_t tag ) {
		;
	}

	// OnMeleeAttack is meant called by the character's items. By default, it just notifies observers of an attack.
	virtual void	OnMeleeAttack ( void ) { NotifyAttack(); };
	// OnRangedAttack is meant called by the character's items. By default, it just notifies observers of an attack.
	virtual void	OnRangedAttack ( void ) { NotifyAttack(); };
	// NotifyAttack is used to go through attack notification list and call all the function pointers that have been added.
	void			NotifyAttack ( void );
	// Adds a function to call when this Character performs an attacking action.
	void			AddNotifyWhenAttack ( CCallbackObserver* nlink, arCallback_Clb_Ptr ncb );
	// Remove all notifications of attack with matching pointer.
	void			RemoveNotiftyWhenAttack ( CCallbackObserver* nlink );

	// OnGainExperience doesn't actually mean that experience has been gained, similar to OnDamaged.
	//  Based on the input's discipline, you can give certain experience types a special effects.
	// The default behavior is to just add the experience to the stats.
	virtual void	OnGainExperience ( const Experience& incomingExperience )
	{
		Experience finalExperience = debuffs.OnGainExperience( incomingExperience );
		stats.fExperience += finalExperience.amount;
	}

	// OnEquip is called when an item is equipped.
	virtual void	OnEquip ( CItemBase* pItem ) {}
	// OnUnequip is called when an item is put away.
	virtual void	OnUnequip ( CItemBase* pItem ) {}

	//==============================================================================================//
	// Animation/Speech/Expression/Action Interface
	//==============================================================================================//

	// Play item animation
	virtual void	PlayItemAnimation ( const NPC::ItemAnim nActionName, const int nSubset, const int nHand, const float fArg, const float fAnimSpeed=0, const float fAttackSkip=0 ) { ; }
	// Return item animation length
	virtual Real	GetItemAnimationLength ( const NPC::ItemAnim nActionName, const int nSubset, const int nHand ) { return -1; }
	//	SpeakDialogue
	// Make this character speak dialogue. It should be compatible with both raw files and the sound system.
	virtual void	SpeakDialogue ( const string& soundFile ) {}
	//	DoSpeech
	// Alternative approach to performing speech by being given a general response first.
	virtual void	DoSpeech ( const NPC::eGeneralSpeechType& speechType ) {}
	//	PerformActionList
	// Should perform the actions given in the list string. See dialogue documentation.
	virtual void	PerformActionList ( const string& actionList ) {}
	//	PerformExpressionList
	// Should perform the expressions given in the list string. See dialogue documentation.
	virtual void	PerformExpressionList ( const string& expressionList ) {}

	//==============================================================================================//
	// Common Getters
	//==============================================================================================//

	// Implementation should be self-explanatory. If the character is on the ground, then return true.
	// This could be used for certain weapon effects, such as a rifle that only stays zoomed in if the player is not jumping.
	virtual bool	OnGround ( void ) { return true; }
	// Implementation should be self-explanatory. If the character is doing a "busy"-type movement, then return true.
	// This could be used for certain weapon effects, such as a energy launcher that only tracks targets if the player is not sprinting.
	virtual bool	IsSprinting ( void ) { return false; }
	// Implementation should be self-explanatory. If the character is doing a "busy"-type action, like pressing a button or climbing a ladder, then return true.
	// The player should be the one to put down the weapons, but certain objects may require this. That, or Weeping Angels can only move while this is true.
	virtual bool	IsPreoccupied ( void ) { return false; }

	// Is this character being threatening?
	virtual bool	IsThreatening ( void ) { return false; }

	// Is this character shadowed (hard to follow)
	virtual bool	IsShadowed ( void ) { return false; }

	// Is this character performing a melee attack? The input to the function is the hand to check.
	// If the hand is being used with a melee attack, then return true.
	virtual bool	IsAttackingMelee ( const short hand ) { return false; }
	// Get the frame of the melee attack that the character is attacking. This is used for determining attack cancelling during melee attacks.
	// Attack cancelling is used for both combos and for sword clashing.
	virtual int		GetAttackingFrame ( const short hand ) { return -1; }

	// Returns a reference to the health
	//virtual CharHealth&		GetHealth ( void ) { return charHealth; }
	virtual ftype		GetHealth ( void ) { return stats.fHealth; }
	
	// Returns a pointer to the stats
	virtual CharacterStats*	GetCharStats ( void ) { return &stats; }
	// Returns a pointer to misc stats
	virtual CRacialStats*	GetRacialStats ( void ) { return NULL; }
	// Returns a pointer to the motion state
	virtual CCharacterMotion* GetMotionState ( void ) { return NULL; }
	// Returns a pointer to combat state
	virtual NPC::sCombatInfo*	GetCombatState ( void ) { return NULL; }
	// Returns a pointer to the combat target
	virtual CCharacter*		GetCombatTarget ( void ) { return NULL; }
	// Returns a pointer to the shield. Returns NULL if no shield.
	virtual SkillShield*	GetShield ( void ) { return NULL; }
	virtual void	SetShield ( SkillShield* ) { ; }
	// Returns a list of all the casting spells in a vector. Default implemenation returns a list of empty skills.
	virtual std::vector<CSkill*>	GetCastingSkills ( void ) { return std::vector<CSkill*>(); }
	
	// Returns type of character (0 animal, 1 monster, 2 sentinent)
	virtual NPC::eAggroAIType GetAggroType ( void ) { return NPC::AGGRO_ANIMAL; }

	// Returns a pointer to the character name
	virtual char*	GetName ( void ) { return charName; }

	// Returns the velocity of the character's motion
	virtual Vector3d GetMotionVelocity ( void ) { return Vector3d::zero; }

	// ===Common Setters===
	// Sets the view angle. Normally, this is something like the FOV (field of vision) of a character.
	virtual void	SetViewAngle ( float = 100 ) {}
	// Sets the offset for the view angle. This is the function you call when you want to temporarily change the view angle of a character
	// as when, for example, you want to zoom in with a Sniper Rifle.
	virtual void	SetViewAngleOffset ( float = 0 ) {}
	// Sets if can sprint. When being busy, you usually want to disable sprinting.
	virtual void	SetCanSprint ( bool enableSprint ) {}
	// Sets the movement scaling.
	virtual void	SetMovementSpeedScale ( float = 1 ) {};

	// ===Specific Modders===

	// Apply stun. Implementations should attempt to apply the stun passed in.
	// Implementations should return true when a stun is applied.
	virtual bool	ApplyStun ( const char* n_stunname, const ftype n_stunlength ) {
		return false;
	}
	// Apply linear motion offset. Implementations should attempt to apply the motion offset passed in.
	// Implementations should return true when the effect is applied.
	virtual bool	ApplyLinearMotion ( const Vector3d& n_motion, const ftype n_motionlength ) {
		return false;
	}

	// Adds the given debuff to the character.
	//virtual void AddBuff ( Debuffs::BaseEffect* newBuff )
	/*Debuffs::CCharacterBuff* AddBuff ( const char* newBuff ) {
		debuffs.Add( newBuff );
	}
	Debuffs::CCharacterBuff* AddBuff ( const short newBuff ) {
		debuffs.Add( newBuff );
	}
	//virtual bool AddBuffIfUnique ( Debuffs::BaseEffect* newBuff )
	Debuffs::CCharacterBuff* AddBuffIfUnique ( const char* newBuff, bool& wasUnique ) {
		return debuffs.AddUnique( newBuff );
	}
	Debuffs::CCharacterBuff* AddBuffIfUnique ( const short newBuff, bool& wasUnique ) {
		return debuffs.AddUnique( newBuff );
	}*/
	template <class BuffType>
	Debuffs::BaseEffect* AddBuff ( void ) {
		return debuffs.Add<BuffType>();
	}
	template <class BuffType>
	Debuffs::BaseEffect* AddBuffIfUnique ( bool& wasUnique ) {
		return debuffs.AddUnique<BuffType>(wasUnique);
	}

	// Returns the debuffs class
	Debuffs::Debuffs& GetDebuffs ( void ) {
		return debuffs;
	}
public:
	CharacterStats	stats;
protected:
	//CharHealth	charHealth;
	Debuffs::Debuffs	debuffs;
	// Damage type that killed the character
	Damage		killDamage; 
	// Character name
	char		charName [512];
private:

protected:
	std::vector<cbpObserverInfo>	observersAttack;

public:
	static std::vector<CCharacter*>	msList;
	
};

#endif