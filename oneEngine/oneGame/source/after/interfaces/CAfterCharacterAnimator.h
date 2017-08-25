
#ifndef _C_AFTER_CHARACTER_ANIMATOR_H_
#define _C_AFTER_CHARACTER_ANIMATOR_H_

#include "after/states/NPC_CombatState.h"
#include "after/states/CharacterStats.h"
#include "after/types/character/Animation.h"
#include "after/types/character/Dialogue.h"

class CCharacterModel;
class CRacialStats;
class CPlayerStats;
class CPlayerInventory;

#include <string>

// ============================================================================
//			MCC Character Animator
// Class for unified interface of state based character animation.
//
// This can't go into CMccCharacterModel because the animator needs to work for
// the general case CCharacterModel. This cannot go into CCharacterModel
// because CCharacterModel is so general case, it has no visibility in regards
// to race stats, which are MCC specific.
// Thus, this class was born. It is used for animation for both player and the
// NPCs. All use this same interface, which keeps consistent animation across
// characters.
// It is also a lot more net-friendly than sending animation names. Instead, a
// eMoveAnimType value is the bare minimum that can be sent. The client takes
// care of the rest.
// ============================================================================

// Animator class definition
class CAfterCharacterAnimator
{
public:
	explicit				CAfterCharacterAnimator ( void );

	// Set move animation type
	void					SetMoveAnimation ( const NPC::eMoveAnimType );
	// Convert animation type to actual playermodel animation
	void					DoMoveAnimation ( void );

	// Play item animation
	void					PlayItemAnimation ( const NPC::eItemAnimType nActionType, const int nVariation, const int nHand, const float fArg, const float fAnimSpeed=0, const float fAttackSkip=0 );
	Real					GetItemAnimationLength ( const NPC::eItemAnimType nActionName, const int nSubset, const int nHand );

	// Play expressions
	void					PerformExpressionList ( const char* nExpressionList );

	// Play speech
	void					DoSpeech ( const NPC::eGeneralSpeechType& speechType );
	void					SpeakDialogue ( const std::string& soundFile );

public:
	CCharacterModel*	m_model;
	NPC::sCombatInfo*	m_combat_info;
	CRacialStats*		m_race_stats;
	CharStats*			m_stats;
	CPlayerInventory*	m_inventory;
private:
	// =Constants for State Variables=
	enum eCombatAnimStyle {
		CombatAnimUnarmed,
		CombatAnimDefault,
		CombatAnimArcher
	};
	enum eCombatAnimState_t {
		CombatState_LeftFoot,
		CombatState_RightFoot
	};

	// =Variables=
	NPC::eMoveAnimType	iMoveAnim;
	Real				fMoveAnimSwapTimer;
	int					iMoveAnimSwapValue;
	int					iCrouchAnimVariation;
	Real				fIdleAnimSwapTimer;
	int					iIdleAnimSwapValue;

	eCombatAnimStyle	iCombatAnimStyle;
	eCombatAnimState_t	iCombatAnimState;

	// =Speech variables=
	Real				f_speech_HuffTime;
	Real				f_speech_FallhardTime;
};


#endif//_C_AFTER_CHARACTER_ANIMATOR_H_