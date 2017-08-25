
#include "CAfterCharacterAnimator.h"
#include "after/entities/CCharacterModel.h"
#include "after/states/CRacialStats.h"
#include "after/states/inventory/CPlayerInventory.h"
#include "renderer/logic/model/morpher/CMorpher.h" //Maybe should be CMorphAction?

#include "core/math/random/Random.h"

using namespace NPC;

// Constructor for default values
CAfterCharacterAnimator::CAfterCharacterAnimator ( void )
{
	m_model			= NULL;
	m_combat_info	= NULL;
	m_race_stats	= NULL;
	m_stats			= NULL;

	iMoveAnim = MoveAnimWalkIdle;
	fMoveAnimSwapTimer = 10;
	iMoveAnimSwapValue = 0;
	iCrouchAnimVariation = 0;
	fIdleAnimSwapTimer = 5;
	iIdleAnimSwapValue = 0;
	iCombatAnimStyle = CombatAnimUnarmed;
	iCombatAnimState = CombatState_LeftFoot;

	Real startTime = Time::CurrentTime()  - 2000.0f;
	f_speech_HuffTime		= startTime;
	f_speech_FallhardTime	= startTime;
}


// == Animation Movement ==
void CAfterCharacterAnimator::SetMoveAnimation ( const eMoveAnimType newMoveType )
{
	iMoveAnim = newMoveType;
}

// DoMoveAnimation()
// Plays the proper animations based on race and other misc stats
void CAfterCharacterAnimator::DoMoveAnimation ( void )
{
	// Check for invalid values
	bool failed = false;
	if ( m_model == NULL ) {
		Debug::Console->PrintError( "Charactor animator has no model" );
		failed = true;
	}
	if ( m_race_stats == NULL ) {
		Debug::Console->PrintError( "Charactor animator has no racial stats" );
		failed = true;
	}
	if ( m_stats == NULL ) {
		Debug::Console->PrintError( "Charactor animator has no point stats" );
		failed = true;
	}
	if ( failed ) return;

	// Perform actual animation now
	switch ( iMoveAnim )
	{
	case MoveAnimFall:
		m_model->SetMoveAnimation( "fall" );
		break;
	case MoveAnimFallSlide:
		m_model->SetMoveAnimation( "fall" );
		break;
	case MoveAnimSwimIdle:
		m_model->SetMoveAnimation( "swim_tread" );
		break;
	case MoveAnimSwimMove:
		m_model->SetMoveAnimation( "swim" );
		break;
	case MoveAnimSwimTread:
		m_model->SetMoveAnimation( "swim_tread" );
		break;
	case MoveAnimProneIdle:
		m_model->SetIdleAnimation( "prone_idle" );
		break;
	case MoveAnimProneMove:
		m_model->SetMoveAnimation( "prone_move" );
		break;

	case MoveAnimCrouchSprint:
		if ( m_race_stats->iRace == CRACE_KITTEN ) {
			m_model->SetIdleAnimation( "crawl_sprint_1" );
			iCrouchAnimVariation = 0;
		}
		else {
			m_model->SetIdleAnimation( "crouch_move" );
		}
		break;
	case MoveAnimCrouchMove:
		if ( m_race_stats->iRace == CRACE_KITTEN ) {
			m_model->SetIdleAnimation( "crawl_forward_1" );
			iCrouchAnimVariation = 0;
		}
		else {
			m_model->SetIdleAnimation( "crouch_move" );
		}
		break;
	case MoveAnimCrouchIdle:
		if ( iCrouchAnimVariation == 0 ) {
			if ( m_race_stats->iRace == CRACE_KITTEN ) {
				m_model->SetIdleAnimation( "crawl_idle_1" );
			}
			else {
				m_model->SetIdleAnimation( "crouch_idle" );
			}
		}
		else if ( iCrouchAnimVariation == 1 ) {
			m_model->SetIdleAnimation( "crouch_idle_2" );
		}
		break;

	case MoveAnimWalkSprint:
		if ( m_race_stats->iRace == CRACE_FLUXXOR ) {
			// Swap animations every 3 seconds
			fMoveAnimSwapTimer += Time::deltaTime;
			if ( fMoveAnimSwapTimer > 3.0f ) {
				// the chance to choose set 1 is (stamina+mana)/(total stamina+total mana)
				ftype randomChance = Random.Range(0,0.9f);
				randomChance = sqr(randomChance);
				if ( randomChance > (m_stats->fMana+m_stats->fStamina)/(m_stats->fManaMax+m_stats->fStaminaMax) ) {
					iMoveAnimSwapValue = 0;
				}
				else {
					iMoveAnimSwapValue = 1;
				}
				fMoveAnimSwapTimer = 0;
			}
		}

		// Base animation on combat state
		if ( !m_combat_info || m_combat_info->state == NPC::sCombatInfo::RELAXED ) {
			if ( m_race_stats->iRace == CRACE_FLUXXOR ) {
				// Depend on animation swap w/ fluxxors, change animation played between hover and normal set
				if ( iMoveAnimSwapValue == 0 ) {
					m_model->SetMoveAnimation( "sprint02" );
				}
				else {
					m_model->SetMoveAnimation( "sprint90" );
				}
			}
			else {
				// Normal race, do normal animation
				m_model->SetMoveAnimation( "sprint02" );
			}
		}
		else if ( m_combat_info->state == NPC::sCombatInfo::GUARDED ) {
			m_model->SetMoveAnimation( "sprint02" );
		}
		else if ( m_combat_info->state == NPC::sCombatInfo::COMBAT ) {
			m_model->SetMoveAnimation( "sprint02" );
		}
		if ( m_race_stats->iRace != CRACE_FLUXXOR && m_race_stats->iRace != CRACE_DWARF ) {
			// Go to dot crouch animation.
			iCrouchAnimVariation = 1;
		}
		break;

	case MoveAnimWalkMove:
		if ( m_race_stats->iRace == CRACE_FLUXXOR ) {
			// Swap animations every 3 seconds
			fMoveAnimSwapTimer += Time::deltaTime;
			if ( fMoveAnimSwapTimer > 3.0f ) {
				// the chance to choose set 1 is (stamina+mana)/(total stamina+total mana)
				ftype randomChance = Random.Range(0,0.9f);
				randomChance = sqr(randomChance);
				if ( randomChance > (m_stats->fMana+m_stats->fStamina)/(m_stats->fManaMax+m_stats->fStaminaMax) ) {
					iMoveAnimSwapValue = 0;
				}
				else {
					iMoveAnimSwapValue = 1;
				}
				fMoveAnimSwapTimer = 0;
			}
		}

		// Base animation on combat state
		if ( !m_combat_info || m_combat_info->state == NPC::sCombatInfo::RELAXED ) {
			if ( m_race_stats->iRace == CRACE_FLUXXOR ) {
				// Depend on animation swap w/ fluxxors, change animation played between hover and normal set
				if ( iMoveAnimSwapValue == 0 ) {
					m_model->SetMoveAnimation( "run02" );
				}
				else {
					m_model->SetMoveAnimation( "walk_hover" );
				}
			}
			else {
				// Normal race, do normal animation
				m_model->SetMoveAnimation( "run02" );
			}
		}
		else if ( m_combat_info->state == NPC::sCombatInfo::GUARDED ) {
			m_model->SetMoveAnimation( "run02" );
		}
		else if ( m_combat_info->state == NPC::sCombatInfo::COMBAT ) {
			m_model->SetMoveAnimation( "run02" );
		}
		if ( m_race_stats->iRace != CRACE_FLUXXOR && m_race_stats->iRace != CRACE_DWARF ) {
			// Go to dot crouch animation.
			iCrouchAnimVariation = 1;
		}
		break;

	case MoveAnimWalkWalk:
		if ( m_race_stats->iRace == CRACE_FLUXXOR ) {
			// Swap animations every 3 seconds
			fMoveAnimSwapTimer += Time::deltaTime;
			if ( fMoveAnimSwapTimer > 3.0f ) {
				// the chance to choose set 1 is (stamina+mana)/(total stamina+total mana)
				ftype randomChance = Random.Range(0,1.0f);
				if ( randomChance > (m_stats->fMana+m_stats->fStamina)/(m_stats->fManaMax+m_stats->fStaminaMax) ) {
					iMoveAnimSwapValue = 0;
				}
				else {
					iMoveAnimSwapValue = 1;
				}
				fMoveAnimSwapTimer = 0;
			}
		}

		// Base animation on combat state
		if ( !m_combat_info || m_combat_info->state == NPC::sCombatInfo::RELAXED ) {
			if ( m_race_stats->iRace == CRACE_FLUXXOR ) {
				// Depend on animation swap w/ fluxxors, change animation played between hover and normal set
				if ( iMoveAnimSwapValue == 0 ) {
					m_model->SetMoveAnimation( "walk" );
				}
				else {
					m_model->SetMoveAnimation( "walk_hover" );
				}
			}
			else {
				// Normal race, do normal animation
				if ( m_stats->fHealth < m_stats->fHealthMax*0.01f+2.0f ) {
					m_model->SetMoveAnimation( "walkHT_forward" );
				}
				else {
					m_model->SetMoveAnimation( "walk" ); // check for inclement weather debuff
				}
			}
		}
		else if ( m_combat_info->state == NPC::sCombatInfo::GUARDED ) {
			if ( iCombatAnimStyle == CombatAnimDefault || iCombatAnimStyle == CombatAnimUnarmed ) {
				m_model->SetMoveAnimation( "walk" );
			}
			else if ( iCombatAnimStyle == CombatAnimArcher ) {
				m_model->SetMoveAnimation( "walk_bow_forward" );
			}
		}
		else if ( m_combat_info->state == NPC::sCombatInfo::COMBAT ) {
			if ( iCombatAnimStyle == CombatAnimDefault || iCombatAnimStyle == CombatAnimUnarmed ) {
				m_model->SetMoveAnimation( "walk" );
			}
			else if ( iCombatAnimStyle == CombatAnimArcher ) {
				m_model->SetMoveAnimation( "walk_bow_forward" );
			}
		}
		// Set movespeed of animation based on the stats.
		m_model->SetMoveAnimationSpeed( std::max<ftype>( 1.0f, m_race_stats->fRunSpeed/11.0f ) );
		break;
	case MoveAnimWalkIdle:
		if ( m_race_stats->iRace == CRACE_FLUXXOR ) {
			// Swap animations every 3 seconds
			fMoveAnimSwapTimer += Time::deltaTime;
			if ( fMoveAnimSwapTimer > 3.0f ) {
				// the chance to choose set 1 is (stamina+mana)/(total stamina+total mana)
				ftype randomChance = Random.Range(0,1.1f);
				if ( randomChance > (m_stats->fMana+m_stats->fStamina)/(m_stats->fManaMax+m_stats->fStaminaMax) ) {
					iMoveAnimSwapValue = 0;
				}
				else {
					iMoveAnimSwapValue = 1;
				}
				fMoveAnimSwapTimer = 0;
			}
		}

		// Base animation on combat state
		if ( !m_combat_info || m_combat_info->state == NPC::sCombatInfo::RELAXED )
		{
			// Perform special racial
			if ( iMoveAnimSwapValue == 1 && m_race_stats->iRace == CRACE_FLUXXOR )
			{
				m_model->SetIdleAnimation( "idle_relaxed_hover_01" );
				m_model->StopMoveAnimation( "idle_relaxed_hover_02" );
			}
			else
			{
				// Perform general movesets
				switch ( m_race_stats->iMovestyle )
				{
				case CMOVES_GLOOMY:
					fIdleAnimSwapTimer += Time::deltaTime;
					if ( fIdleAnimSwapTimer > 7.0f ) {
						iIdleAnimSwapValue = Random.Next() % 3;
						fIdleAnimSwapTimer = 0.0f;
					}
					if ( iIdleAnimSwapValue == 0 )
						m_model->SetIdleAnimation( "idle_relaxed_gloomy_01" );
					else if ( iIdleAnimSwapValue == 1 )
						m_model->SetIdleAnimation( "idle_relaxed_gloomy_02" );
					else if ( iIdleAnimSwapValue == 2 )
						m_model->SetIdleAnimation( "idle_relaxed_gloomy_03" );
					break;
				case CMOVES_LARGE_HAM:
					m_model->SetIdleAnimation( "idle_relaxed_hammy_02" );
					break;
				/*case CMOVES_SASSY:
					m_model->SetMoveAnimation( "idle" );
					break;*/
				case CMOVES_JOLLY:
					fIdleAnimSwapTimer += Time::deltaTime;
					if ( fIdleAnimSwapTimer > 7.0f - iIdleAnimSwapValue * 4.0f ) {
						iIdleAnimSwapValue = Random.Next() % 2;
						fIdleAnimSwapTimer = 0.0f;
					}
					if ( iIdleAnimSwapValue == 0 )
						m_model->SetIdleAnimation( "idle_relaxed_jolly_01" );
					else
						m_model->SetIdleAnimation( "idle_relaxed_jolly_02" );
					break;
				case CMOVES_DEFAULT:
				default:
					// Normal race, do normal animation
					//m_model->SetMoveAnimation( "idle" ); //idle_relaxed_01
					fIdleAnimSwapTimer += Time::deltaTime;
					if ( fIdleAnimSwapTimer > 10.0f - iIdleAnimSwapValue * 4.0f ) {
						iIdleAnimSwapValue = Random.Next() % 2;
						fIdleAnimSwapTimer = 0.0f;
					}
					if ( iIdleAnimSwapValue == 0 )
						m_model->SetIdleAnimation( "idle_relaxed_01" );
					else
						m_model->SetIdleAnimation( "idle_relaxed_02" );
					break;
				}

				if ( m_race_stats->iRace == CRACE_FLUXXOR ) {
					m_model->PlayAnimation( "idle_relaxed_hover_02" );
				}
			}
		}
		else if ( m_combat_info->state == NPC::sCombatInfo::GUARDED ) {
			if ( iCombatAnimStyle == CombatAnimUnarmed ) {
				if ( iCombatAnimState == CombatState_LeftFoot ) {
					m_model->SetIdleAnimation( "h2h_idle_l" );
				}
				else {
					m_model->SetIdleAnimation( "h2h_idle_r" );
				}
			}
			else if ( iCombatAnimStyle == CombatAnimDefault ) {
				m_model->SetIdleAnimation( "idle_gaurded_01" );
			}
			else if ( iCombatAnimStyle == CombatAnimArcher ) {
				m_model->SetIdleAnimation( "hold_bow_idle" );
			}
		}
		else if ( m_combat_info->state == NPC::sCombatInfo::COMBAT ) {
			if ( iCombatAnimStyle == CombatAnimUnarmed ) {
				if ( iCombatAnimState == CombatState_LeftFoot ) {
					m_model->SetIdleAnimation( "h2h_idle_l" );
				}
				else {
					m_model->SetIdleAnimation( "h2h_idle_r" );
				}
			}
			else if ( iCombatAnimStyle == CombatAnimDefault ) {
				m_model->SetIdleAnimation( "idle_combat_01" );
			}
			else if ( iCombatAnimStyle == CombatAnimArcher ) {
				m_model->SetIdleAnimation( "hold_bow_idle" );
			}
		}
		
		// Choose a random dot crouch animation
		if ( m_race_stats->iRace == CRACE_KITTEN ) {
			iCrouchAnimVariation = 1; // Do kitty crouch, bitch.
		}
		else {
			iCrouchAnimVariation = 0;
		}
		break;

	case MoveAnimMoveWallrunLeft:
		m_model->SetMoveAnimation( "sprint02_wallrun_left" );
		// Set movespeed of animation based on the stats.
		m_model->SetMoveAnimationSpeed( std::max<ftype>( 1.0f, m_race_stats->fRunSpeed/17.0f ) );
		break;
	case MoveAnimMoveWallrunRight:
		m_model->SetMoveAnimation( "sprint02_wallrun_right" );
		// Set movespeed of animation based on the stats.
		m_model->SetMoveAnimationSpeed( std::max<ftype>( 1.0f, m_race_stats->fRunSpeed/17.0f ) );
		break;

	case MoveAnim_USER:
	default:
		// Do Nothing.
		break;
	};
}


// ===Base Animation Interface===
//void	CAfterCharacterAnimator::PlayItemAnimation ( const string& sActionName, const float fArg )
void	CAfterCharacterAnimator::PlayItemAnimation ( const NPC::eItemAnimType nActionType, const int nVariation, const int nArm, const float fArg, const float fAnimSpeed, const float fAttackSkip )
{
	// Check for invalid values
	bool failed = false;
	if ( !m_inventory ) {
		Debug::Console->PrintError( "Charactor animator has no inventory" );
		failed = true;
	}
	if ( failed ) return;
	// Perform animation
	CWeaponItem* currentEquipped = m_inventory->GetCurrentEquipped(nArm);
	if ( currentEquipped == NULL ) {
		return;
	}
	CWeaponItem* currentEquipped_Offset = m_inventory->GetCurrentEquipped((1-(nArm%2))+(nArm%2));
	bool hasOffset = (currentEquipped_Offset != NULL && currentEquipped_Offset->GetTypeName() != "ItemBlockPuncher");

	Item::HoldType holdType = currentEquipped->GetHoldType();
	if ( holdType == Item::SpellHanded )
	{
		iCombatAnimStyle = CombatAnimDefault;

		if ( nVariation == 1 ) {
			if ( nActionType == ItemAnim::Draw ) {
				m_model->PlayAnimation( "spell01_draw" );
			}
			else if ( nActionType == ItemAnim::Idle ) {
				//model->PlayAnimation( "spell01_holster" );
			}
			else if ( nActionType == ItemAnim::Holster ) {
				m_model->PlayAnimation( "spell01_holster" );
			}
			else if ( nActionType == ItemAnim::Cast ) {
				m_model->PlayAnimation( "spell01_snap" );
			}
			else if ( nActionType == ItemAnim::Precast ) {
				m_model->PlayAnimation( "spell01_idle" );
			}
		}
		else if ( nVariation == 2 ) {
	//}
	//else if ( holdType == CWeaponItem::SpellBallHanded )
	//{
			if ( nActionType == ItemAnim::Draw ) {
				m_model->PlayAnimation( "spell02_draw" );
			}
			else if ( nActionType == ItemAnim::Idle ) {
				m_model->PlayAnimation( "spell02_idle" );
			}
			else if ( nActionType == ItemAnim::Holster ) {
				m_model->PlayAnimation( "spell02_holster" );
			}
			else if ( nActionType == ItemAnim::Cast ) {
				m_model->PlayAnimation( "spell02_cast" );
			}
			else if ( nActionType == ItemAnim::Precast ) {
				m_model->PlayAnimation( "spell02_idle" );
			}
			else if ( nActionType == ItemAnim::Toss ) {
				m_model->PlayAnimation( "hold_onehanded_toss", fArg, fAnimSpeed );
			}
		}
	}
	else if ( holdType == Item::LightSourceMelee )
	{
		iCombatAnimStyle = CombatAnimDefault;

		if ( nActionType == ItemAnim::Draw ) {
			m_model->PlayAnimation( "hold_torch_draw" );
		}
		else if ( nActionType == ItemAnim::Idle ) {
			m_model->PlayAnimation( "hold_torch" );
		}
		else if ( nActionType == ItemAnim::Holster ) {
			m_model->PlayAnimation( "hold_onehanded_holster" );
		}
	}
	else if (( holdType == Item::Sword )||( holdType == Item::TwoHandedAxe ))
	{
		iCombatAnimStyle = CombatAnimDefault;

		if ( nActionType == ItemAnim::Draw ) {
			m_model->PlayAnimation( "hold_oneblade_draw" );
			if (m_combat_info) m_combat_info->OnDraw();
		}
		else if ( nActionType == ItemAnim::Idle ) {
			if (m_combat_info) {
				if ( m_combat_info->state >= sCombatInfo::GUARDED ) {
					m_model->PlayAnimation( hasOffset ? "hold_twoblade" : "hold_oneblade" );
				}
				else {
					m_model->PlayAnimation( "hold_onehanded_holster" );
				}
			}
			else {
				m_model->PlayAnimation( hasOffset ? "hold_twoblade" : "hold_oneblade" );
			}
		}
		else if ( nActionType == ItemAnim::Holster ) {
			m_model->PlayAnimation( "hold_onehanded_holster" );
			if (m_combat_info) m_combat_info->OnSheathe();
		}
		else if ( nActionType == ItemAnim::Attack ) {
			m_model->PlayAnimation( hasOffset ? "hold_twobladed_attack" : "hold_onebladed_attack", fArg, fAnimSpeed, fAttackSkip, nArm );
			if (m_combat_info) m_combat_info->OnAttack();
		}
		else if ( nActionType == ItemAnim::Quickstrike ) {
			m_model->PlayAnimation( hasOffset ? "hold_twobladed_attack_quick" : "hold_onebladed_attack_quick", fArg, fAnimSpeed, fAttackSkip, nArm );
			if (m_combat_info) m_combat_info->OnAttack();
		}
		else if ( nActionType == ItemAnim::Charge ) {
			m_model->PlayAnimation( "hold_onebladed_charge", fArg );
			if (m_combat_info) m_combat_info->OnAttack();
		}
		else if ( nActionType == ItemAnim::Defend ) {
			m_model->PlayAnimation( hasOffset ? "hold_twobladed_defend" : "hold_onebladed_defend", fArg );
			if (m_combat_info) m_combat_info->OnDefend();
		}
		else if ( nActionType == ItemAnim::DefendStop ) {
			m_model->PlayAnimation( "hold_onebladed_defend_stop", fArg );
		}
		else if ( nActionType == ItemAnim::Parry ) {
			m_model->PlayAnimation( hasOffset ? "hold_twobladed_parry" : "hold_onebladed_parry", fArg );
			if (m_combat_info) m_combat_info->OnDefend();
		}
	}
	else if ( holdType == Item::Bow )
	{
		if ( nActionType == ItemAnim::Draw ) {
			iCombatAnimStyle = CombatAnimArcher;
			m_model->PlayAnimation( "hold_oneblade_draw" );
			if (m_combat_info) m_combat_info->OnDraw();
		}
		else if ( nActionType == ItemAnim::Idle ) {
			iCombatAnimStyle = CombatAnimArcher;
			//m_model->PlayAnimation( "hold_bow_idle" );
		}
		else if ( nActionType == ItemAnim::Charge ) {
			iCombatAnimStyle = CombatAnimArcher;
			m_model->PlayAnimation( "hold_bow_aim", fArg );
			if (m_combat_info) m_combat_info->OnAttack();
		}
		else if ( nActionType == ItemAnim::Holster ) {
			iCombatAnimStyle = CombatAnimDefault;
			m_model->PlayAnimation( "hold_bow_holster" );
			if (m_combat_info) m_combat_info->OnSheathe();
		}
	}
	else if ( holdType != Item::EmptyHanded )
	{
		iCombatAnimStyle = CombatAnimDefault;

		if ( nActionType == ItemAnim::Draw ) {
			m_model->PlayAnimation( "hold_onehanded_draw" );
		}
		else if ( nActionType == ItemAnim::Idle ) {
			m_model->PlayAnimation( "hold_onehanded" );
		}
		else if ( nActionType == ItemAnim::Holster ) {
			m_model->PlayAnimation( "hold_onehanded_holster" );
		}
		else if ( nActionType == ItemAnim::Attack ) {
			m_model->PlayAnimation( "hold_onehanded_smack" );
		}
		else if ( nActionType == ItemAnim::Toss ) {
			m_model->PlayAnimation( "hold_onehanded_toss", fArg );
		}
	}
	else
	{
		iCombatAnimStyle = CombatAnimUnarmed;

		if ( nActionType == ItemAnim::PunchNeutral ) {
			m_model->PlayAnimation( "fisticuffs_punch_hit" );
		}
		else if ( nActionType == ItemAnim::PunchNeutralMiss ) {
			m_model->PlayAnimation( "fisticuffs_punch" );
		}
		else if ( nActionType == ItemAnim::Holster ) {
			m_model->PlayAnimation( "hold_onehanded_holster" );
		}
		else if ( nActionType == ItemAnim::PunchNeutralBreak ) {
			m_model->PlayAnimation( "punch_neutral_break" );
		}
		// H2H Combat
		else if ( nActionType == ItemAnim::h2hInterrupt ) {
			m_model->StopAnimationPlaying( "h2h_punch_r" );
			m_model->StopAnimationPlaying( "h2h_punch_l" );
			m_model->StopAnimationPlaying( "h2h_kick_r" );
			m_model->StopAnimationPlaying( "h2h_kick_l" );
		}
		else if ( nActionType == ItemAnim::h2hPunch_L ) {
			iCombatAnimState = CombatState_LeftFoot;
			m_model->StopAnimationPlaying( "h2h_punch_l" );
			m_model->PlayAnimation( "h2h_punch_l" );
			if (m_combat_info) m_combat_info->OnAttack();
		}
		else if ( nActionType == ItemAnim::h2hPunch_R ) {
			iCombatAnimState = CombatState_RightFoot;
			m_model->StopAnimationPlaying( "h2h_punch_r" );
			m_model->PlayAnimation( "h2h_punch_r" );
			if (m_combat_info) m_combat_info->OnAttack();
		}
		else if ( nActionType == ItemAnim::h2hKick_L ) {
			iCombatAnimState = CombatState_LeftFoot;
			m_model->StopAnimationPlaying( "h2h_kick_l" );
			m_model->PlayAnimation( "h2h_kick_l" );
			if (m_combat_info) m_combat_info->OnAttack();
		}
		else if ( nActionType == ItemAnim::h2hKick_R ) {
			iCombatAnimState = CombatState_LeftFoot;
			m_model->StopAnimationPlaying( "h2h_kick_r" );
			m_model->PlayAnimation( "h2h_kick_r" );
			if (m_combat_info) m_combat_info->OnAttack();
		}
	}
}

Real CAfterCharacterAnimator::GetItemAnimationLength ( const NPC::eItemAnimType nActionName, const int nSubset, const int nHand )
{

	return -1;
}

void CAfterCharacterAnimator::PerformExpressionList ( const char* nExpressionList )
{
	// Get the morpher
	CMorpher* morpher = NULL;
	morpher = m_model->GetFaceMorpher();
	if ( !morpher ) {
		return;
	}

	// Parse through the expression list and create the morphs
	struct expression_t
	{
		string expression;
		bool base;
	};
	std::vector<expression_t> expressions;

	// Tokenize string manually to build pieces
	{
		string usedExpressionList = string(nExpressionList) + "  "; // Append two spaces to ensure last value added
		expression_t nextExpression;
		nextExpression.base = true;
		nextExpression.expression = "";
		int mode = 0;
		for ( uint i = 0; i < usedExpressionList.size(); ++i )
		{
			char nextChar = usedExpressionList[i];
			// Searching for next expression
			if ( mode == 0 )
			{
				if ( isspace( nextChar ) ) {
					continue;
				}
				else if ( nextChar == '+' ) {
					mode = 1;
					nextExpression.base = false;
				}
				else {
					mode = 1;
					nextExpression.expression += nextChar;
				}
			}
			// Building expression
			else if ( mode == 1 )
			{
				if ( isspace( nextChar ) ) {
					// Next char is a space. We hit the end of the mode.
					mode = 0;
					expressions.push_back( nextExpression );
					nextExpression.base = true;
					nextExpression.expression = "";
				}
				else if ( nextChar == '+' ) {
					// Next char is a plus. We hit the start of a new expression.
					expressions.push_back( nextExpression );
					nextExpression.base = false;
					nextExpression.expression = "";
				}
				else {
					// Next char is an identifier
					nextExpression.expression += nextChar;
				}
			}
		}
		// "expressions" is now filled with data to work off of
	}

	// Grab needed morphs
	CMorphAction *leftEyebrow, *rightEyebrow, *smirk;

	leftEyebrow		= morpher->FindAction("leftEyebrow");
	rightEyebrow	= morpher->FindAction("rightEyebrow");
	smirk			= morpher->FindAction("smirk");

	// Loop through the expressions and perform them
	for ( uint i = 0; i < expressions.size(); ++i )
	{
		// A base zeros out all expressions
		if ( expressions[i].base ) {
			leftEyebrow->target_weight	= 0;
			rightEyebrow->target_weight	= 0;
			smirk->target_weight		= 0;

			leftEyebrow->auto_blend		= true;
			rightEyebrow->auto_blend	= true;
			smirk->auto_blend			= true;
		}

		// Based on input string, perform expression
		if ( expressions[i].expression == "default" )
		{
			// Nothing
		}
		else if ( expressions[i].expression == "smirk" )
		{
			smirk->target_weight += 1.0f;
		}
		else if ( expressions[i].expression == "leyebrowraise" )
		{
			leftEyebrow->target_weight += 1.0f;
		}
		else if ( expressions[i].expression == "reyebrowraise" )
		{
			rightEyebrow->target_weight += 1.0f;
		}
		else if ( expressions[i].expression == "leyebrowlower" )
		{
			leftEyebrow->target_weight -= 0.5f;
		}
		else if ( expressions[i].expression == "reyebrowlower" )
		{
			rightEyebrow->target_weight -= 0.5f;
		}
	}

}