
#include "CCharacterModel.h"
#include "after/animation/Sequence.h"

#include "renderer/logic/model/CModel.h"
#include "renderer/logic/model/CSkinnedModel.h"
//#include "CCharacter.h"
#include "engine-common/entities/CActor.h"

//#include "CHKAnimation.h"
#include "engine-common/physics/motion/CRagdollCollision.h"

#include "engine-common/utils/AnimationMixing.h"

#include "engine/physics/raycast/Raycaster.h"

#include "core/math/Math.h"


void CCharacterModel::ConfigureAnimations ( void )
{
	CAnimation* pAnim = charModel->GetAnimation();
	CAnimation& anim = *pAnim;

	// Define functors for easy animation setup
	struct _setupanim_upperbody_t {
		CAnimation& anim;
		explicit _setupanim_upperbody_t ( CAnimation& refanim ) : anim(refanim) {}
		void operator()( const string& name, const int layer ) {
			anim[name].layer = layer;
			//anim[name].AddMixingTransform( "Spine1", true );
			Animation::AddMixingTransform( anim, anim[name], "Spine1", true );
			//anim[name].AddMixingTransform( "Spine", false );
			//anim[name].AddMixingTransform( "Pelvis", false );
			anim[name].tag = CAnimAction::TAG_ITEM;
			anim[name].prop_override[0] = 1;
			anim[name].prop_override[1] = 1;
			anim[name].prop_override[2] = 1;
			anim[name].prop_override[3] = 1;
		}
	} SetupAnim_UpperBody(anim);
	struct _setupanim_rightarm_t {
		CAnimation& anim;
		explicit _setupanim_rightarm_t ( CAnimation& refanim ) : anim(refanim) {}
		void operator()( const string& name, const int layer ) {
			anim[name].layer = layer;
			//anim[name].AddMixingTransform( "R UpperArm", true );
			Animation::AddMixingTransform( anim, anim[name], "R UpperArm", true );
			//anim[name].AddMixingTransform( "Prop1", false );
			Animation::AddMixingTransform( anim, anim[name], "Prop1", false );
			anim[name].tag = CAnimAction::TAG_ITEM;
			anim[name].prop_override[0] = 1;
			anim[name].prop_override[2] = 1;
		}
	} SetupAnim_RightArm(anim);
	struct _setupanim_leftarm_t {
		CAnimation& anim;
		explicit _setupanim_leftarm_t ( CAnimation& refanim ) : anim(refanim) {}
		void operator()( const string& name, const int layer ) {
			anim[name].layer = layer;
			//anim[name].AddMixingTransform( "L UpperArm", true );
			Animation::AddMixingTransform( anim, anim[name], "L UpperArm", true );
			//anim[name].AddMixingTransform( "Prop2", false );
			Animation::AddMixingTransform( anim, anim[name], "Prop2", false );
			anim[name].tag = CAnimAction::TAG_ITEM;
			anim[name].prop_override[1] = 1;
			anim[name].prop_override[3] = 1;
		}
	} SetupAnim_LeftArm(anim);

	// Idle animations
	anim["idle_relaxed_hover_02"].layer = 1;
	Animation::AddMixingTransform( anim, anim["idle_relaxed_hover_02"], "Bip001", false );
	Animation::AddMixingTransform( anim, anim["idle_relaxed_hover_02"], "Bip001 Pelvis", false );
	Animation::AddMixingTransform( anim, anim["idle_relaxed_hover_02"], "Bip001 L Thigh", true );
	Animation::AddMixingTransform( anim, anim["idle_relaxed_hover_02"], "Bip001 R Thigh", true );

	// Basic one-handed animations
	SetupAnim_RightArm( "hold_onehanded_draw",4 );
	SetupAnim_RightArm( "hold_onehanded_draw_fast",4 );
	SetupAnim_RightArm( "hold_onehanded_smack",4 );
	SetupAnim_RightArm( "hold_onehanded",3 );
	SetupAnim_RightArm( "hold_onehanded_toss",4 );

	// Basic one-handed animations
	SetupAnim_RightArm( "hold_torch_draw", 4 );
	SetupAnim_RightArm( "hold_torch_draw_02", 4 );
	SetupAnim_RightArm( "hold_torch", 3 );

	// Basic one-handed animations
	SetupAnim_RightArm( "hold_oneblade_draw", 4 );
	SetupAnim_RightArm( "hold_oneblade_draw_top", 4 );

	SetupAnim_UpperBody( "hold_oneblade_01_idle", 3 );

	SetupAnim_UpperBody( "hold_oneblade_01_block-1", 3 );
	SetupAnim_UpperBody( "hold_oneblade_01_block-2", 3 );
	SetupAnim_UpperBody( "hold_oneblade_01_block-3", 3 );
	SetupAnim_UpperBody( "hold_oneblade_01_block-4", 3 );
	SetupAnim_UpperBody( "hold_oneblade_01_block-5", 3 );

	SetupAnim_UpperBody( "hold_oneblade_01_parry", 4 );

	SetupAnim_UpperBody( "hold_oneblade_01_prepare-0", 4 );
	SetupAnim_UpperBody( "hold_oneblade_01_prepare-2", 4 );
	SetupAnim_UpperBody( "hold_oneblade_01_prepare-4", 4 );
	SetupAnim_UpperBody( "hold_oneblade_01_prepare-6", 4 );

	SetupAnim_UpperBody( "hold_oneblade_01_swing-0", 4 );
	SetupAnim_UpperBody( "hold_oneblade_01_swing-1", 4 );
	SetupAnim_UpperBody( "hold_oneblade_01_swing-2", 4 );
	SetupAnim_UpperBody( "hold_oneblade_01_swing-3", 4 );
	SetupAnim_UpperBody( "hold_oneblade_01_swing-4", 4 );
	SetupAnim_UpperBody( "hold_oneblade_01_swing-5", 4 );
	SetupAnim_UpperBody( "hold_oneblade_01_swing-6", 4 );
	SetupAnim_UpperBody( "hold_oneblade_01_swing-7", 4 );
	SetupAnim_UpperBody( "hold_oneblade_01_swing-8", 4 );

	SetupAnim_UpperBody( "hold_oneblade_01_ministun-0-4", 5 );

	// Dual wielding one-handed animations
	SetupAnim_UpperBody( "hold_twoblade_01_idle", 3 );

	SetupAnim_UpperBody( "hold_twoblade_01_block-3", 3 );

	SetupAnim_UpperBody( "hold_twoblade_01_parry", 4 );

	SetupAnim_LeftArm( "hold_twoblade_01_swing-left-0-4", 4 );
	SetupAnim_RightArm( "hold_twoblade_01_swing-right-0-4", 4 );

	// Basic two-handed animations
	Animation::AddMixingTransform( anim, anim["hold_twohanded_smack"], "L UpperArm", true );
	Animation::AddMixingTransform( anim, anim["hold_twohanded_smack"], "R UpperArm", true );
	Animation::AddMixingTransform( anim, anim["hold_twohanded_smack"], "Prop1", false );
	anim["hold_twohanded_smack"].layer = 4;

	Animation::AddMixingTransform( anim, anim["hold_twohanded_draw"], "L UpperArm", true );
	Animation::AddMixingTransform( anim, anim["hold_twohanded_draw"], "R UpperArm", true );
	Animation::AddMixingTransform( anim, anim["hold_twohanded_draw"], "Prop1", false );
	anim["hold_twohanded_draw"].layer = 4;

	Animation::AddMixingTransform( anim, anim["hold_twohanded"], "L UpperArm", true );
	Animation::AddMixingTransform( anim, anim["hold_twohanded"], "R UpperArm", true );
	Animation::AddMixingTransform( anim, anim["hold_twohanded"], "Prop1", false );
	anim["hold_twohanded"].layer = 4;

	SetupAnim_RightArm( "hold_general_push", 4 );
	SetupAnim_RightArm( "hold_general_shut", 4 );

	SetupAnim_UpperBody( "hold_anim_googleson", 4 );
	SetupAnim_UpperBody( "hold_anim_googlesoff", 4 );

	// Casting animations
	SetupAnim_RightArm( "spell01_snap", 4 );
	SetupAnim_RightArm( "spell01_idle", 3 );
	SetupAnim_RightArm( "spell02_idle", 3 );

	// Punching animations
	SetupAnim_LeftArm( "fisticuffs_left", 4 );
	anim["fisticuffs_left"].framesPerSecond = 37.0f;
	anim["fisticuffs_left"].tag = CAnimAction::TAG_ITEM;

	SetupAnim_RightArm( "fisticuffs_right", 4 );
	anim["fisticuffs_right"].framesPerSecond = 37.0f;
	anim["fisticuffs_right"].tag = CAnimAction::TAG_ITEM;

	SetupAnim_LeftArm( "fisticuffs_left_hit", 4 );
	anim["fisticuffs_left_hit"].framesPerSecond = 37.0f;
	anim["fisticuffs_left_hit"].tag = CAnimAction::TAG_ITEM;

	SetupAnim_RightArm( "fisticuffs_right_hit", 4 );
	anim["fisticuffs_right_hit"].framesPerSecond = 37.0f;
	anim["fisticuffs_right_hit"].tag = CAnimAction::TAG_ITEM;

	SetupAnim_LeftArm( "fisticuffs_left_hit_broke", 4 );
	SetupAnim_RightArm( "fisticuffs_right_hit_broke", 4 );

	// H2H Combat
	anim["h2h_punch_r"].layer = 4;
	AddAnimationBlendoutPct("h2h_punch_r", 0.10f, 0.25f );
	AddAnimationBlendstopped("h2h_punch_r", 0.10f);

	anim["h2h_punch_l"].layer = 4;
	AddAnimationBlendoutPct("h2h_punch_l", 0.10f, 0.25f );
	AddAnimationBlendstopped("h2h_punch_l", 0.10f);

	anim["h2h_kick_r"].layer = 4;
	AddAnimationBlendoutPct("h2h_kick_r", 0.10f, 0.25f );
	AddAnimationBlendstopped("h2h_kick_r", 0.10f);

	anim["h2h_kick_l"].layer = 4;
	AddAnimationBlendoutPct("h2h_kick_l", 0.10f, 0.25f );
	AddAnimationBlendstopped("h2h_kick_l", 0.10f);

	// Bow animations
	/*anim["hold_bow"].AddMixingTransform( "Spine", false );
	anim["hold_bow"].AddMixingTransform( "Spine1", false );
	anim["hold_bow"].AddMixingTransform( "Spine2", false );
	anim["hold_bow"].AddMixingTransform( "Spine3", true );
	anim["hold_bow"].AddMixingTransform( "Pelvis", false );*/
	anim["hold_bow_idle"].layer = 0;
	anim["hold_bow_idle"].prop_override[0] = 1;
	anim["hold_bow_idle"].prop_override[1] = 1;
	Animation::AddMixingTransform( anim, anim["hold_bow_aim"], "Spine", false );
	Animation::AddMixingTransform( anim, anim["hold_bow_aim"], "Spine1", true );
	Animation::AddMixingTransform( anim, anim["hold_bow_aim"], "Pelvis", false );
	Animation::AddMixingTransform( anim, anim["hold_bow_aim"], "Bip001", false );
	anim["hold_bow_aim"].layer = 1;
	anim["hold_bow_aim"].prop_override[0] = 1;
	anim["hold_bow_aim"].prop_override[1] = 1;
	anim["hold_bow_aim"].tag = CAnimAction::TAG_ITEM;

	anim["walk_bow_forward"].layer = 0;
	anim["walk_bow_forward"].prop_override[0] = 1;
	anim["walk_bow_forward"].prop_override[1] = 1;

	// Jumping animations
	anim["jump"].layer = 1;
	anim["jump_03"].layer = 1;

	anim["land"].layer = 1; // put landing over falling

	// Stun animations
	anim["fall_hard"].layer = 5;
	anim["fall_hard"].framesPerSecond = 20.0f;

	//anim["stun_stagger_hard"].layer = 5;
	anim["stun_stagger_hard"].framesPerSecond = 25.0f;
	anim["stun_stagger_twist"].framesPerSecond = 31.0f;

	// Action animations
	//anim["hang_ledge_climb"].layer = 4;
	anim["slide"].layer = 4;
	anim["roll"].layer = 4;

	anim["vault_4h"].layer = 4;

	anim["hang_climb"].layer = 4;

	anim["falldown_start"].layer = 4;
	anim["falldown_idle_0"].layer = 3;
	anim["falldown_idle_1"].layer = 3;
	anim["falldown_end"].layer = 4;

	// Walking, running, and other sync layer animation
	// TODO: edit sync track code (CHKAnimation.cpp) to work off of percentages.
	anim["walk01_backward"].sync_track = 1;
	anim["walk01_forward"].sync_track = 1;
	anim["walk01_left"].sync_track = 1;
	anim["walk01_right"].sync_track = 1;
	anim["run02_backward"].sync_track = 2;
	anim["run02_forward"].sync_track = 2;
	anim["run02_left"].sync_track = 2;
	anim["run02_right"].sync_track = 2;


	// Set up animation fade in/outs
	AddAnimationBlendoutPct("land", 0.125f, 0.50f );
	AddAnimationBlendoutPct("jump", 0.125f, 0.50f );
	AddAnimationBlendoutPct("jump_03", 0.125f, 0.50f );
	AddAnimationBlendoutPct("fall_hard", 0.0f, 0.25f );
	AddAnimationBlendoutPct("stun_stagger_hard", 0.17f, 0.25f );
	AddAnimationBlendoutPct("stun_stagger_twist", 0.17f, 0.25f );
	AddAnimationBlendoutPct("falldown_end", 0.0f, 0.25f );

}

void CCharacterModel::ConfigureCommonAnimations ( void )
{
	CAnimation* pAnim = charModel->GetAnimation();
	CAnimation& anim = *pAnim;
	CAnimAction* action;

	action = anim.FindAction( "jump" );
	if ( action ) action->layer = 1;

	action = anim.FindAction( "stagger" );
	if ( action ) action->layer = 2;

	action = anim.FindAction( "bite" );
	if ( action ) {
		action->layer = 2;
		Animation::AddMixingTransform( anim, *action, "Neck", true );
	}

	action = anim.FindAction( "walk01_forward" );
	if ( !action ) {
		useDirectionalMovementAnimation = false;
	}
}


// == Animations ==
bool CCharacterModel::PlayScriptedAnimation ( const string& sAnimName, const float fFramesPerSecond )
{
	CAnimation* anim = charModel->GetAnimation();
	CAnimAction* action;
	if ( action = anim->FindAction( sAnimName ) ) {
		if ( !action->isPlaying ) {
			action->layer = anim->maxLayers-1;
			anim->Play( sAnimName );
			//action->end_behavior = 2; //hold end and fade out
			action->framesPerSecond = fFramesPerSecond;
			SetMoveAnimation( "anim" );
		}
		return true;
	}
	return false;
}

bool CCharacterModel::SetNextBlendingValue ( const float nNextBlendSpeed )
{
	fNextBlendSpeed = nNextBlendSpeed;
	return true;
}

bool CCharacterModel::PlayAnimation ( const string& sAnimType, const float fArg, const float fAnimSpeed, const float fAttackSkip, const int nArm )
{
	bool validAnim = false;
	CAnimation& anim = (*(charModel->GetAnimation()));

	if ( iFallType < 0 )
		iFallType = 0;

	// Check for skip animation playing.
	if ( sAnimType.length() == 0 ) {
		return false;
	}

	// Check for reference
	if ( sAnimType == "ref" ) {
		anim.Play( "ref" );
		anim["ref"].loop = true;
		anim["ref"].framesPerSecond = 0.01f;
		anim["ref"].frame = 0;
		validAnim = true;
	}
	else {
		// Playing an animation. Interrupt current SEQUENCE if layer 0.
		if ( bPerformSequenceInterrupts )
		{
			if ( currentSequence && currentSequence->m_layer == 0 )
			{
				currentSequence->Reset();
			}
		}
	}

	// Based on animation type, work it.
	if ( sAnimType == "anim" ) {
		iIdleType = LockAll;
	}
	//else if (( sAnimType == "walk" )||( sAnimType == "run" )||( sAnimType == "walk_hover" )||( sAnimType == "run_hover" )||( sAnimType == "walk_bow_forward" ))
	else if ( (sAnimType.find("walk") != string::npos) || (sAnimType.find("run") != string::npos) || (sAnimType.find("sprint") != string::npos) )
	{
		Vector3d unbiasedMovement = !charModel->transform.rotation * vMotionVelocity;
		unbiasedMovement.z = 0;
		unbiasedMovement.normalize();
		if ( useDirectionalMovementAnimation )
		{
			if ( sAnimType == "walk" )
			{
				string main_anim = "walk01_forward";
				string sub_anim = "walk01_right";
				if ( unbiasedMovement.y > 0 ) {
					main_anim = "walk01_backward";
				}
				if ( unbiasedMovement.x > 0 ) {
					sub_anim = "walk01_left";
				}

				anim[main_anim].isPlaying = true;
				anim[sub_anim].isPlaying = true;

				anim[main_anim].weight	= std::min<ftype>( sqrt(fabs(unbiasedMovement.y)),	anim[main_anim].weight + 2*Time::deltaTime );
				anim[sub_anim].weight	= std::min<ftype>( fabs(unbiasedMovement.x),		anim[sub_anim].weight + 2*Time::deltaTime );

				anim[main_anim].reset_on_stop = false;
				anim[sub_anim].reset_on_stop = false;

				anim[main_anim].framesPerSecond = 37.0f;
				anim[sub_anim].framesPerSecond = 37.0f;

				validAnim = true;
			}
			else if ( true )
			{
				string main_anim = sAnimType;
				/*if ( unbiasedMovement.y < -0.383f ) {
					if ( unbiasedMovement.x > 0.383f ) {
						main_anim += "_forward_left";
					}
					else if ( unbiasedMovement.x < -0.383f ) {
						main_anim += "_forward_right";
					}
					else {
						main_anim += "_forward";
					}
				}
				else if ( unbiasedMovement.y > 0.383f ) {
					if ( unbiasedMovement.x > 0.383f ) {
						main_anim += "_backward_left";
					}
					else if ( unbiasedMovement.x < -0.383f ) {
						main_anim += "_backward_right";
					}
					else {
						main_anim += "_backward";
					}
				}
				else {
					if ( unbiasedMovement.x > 0.0f ) {
						main_anim += "_left";
					}
					else {
						main_anim += "_right";
					}
				}*/
				if ( unbiasedMovement.y < -0.707f ) {
					main_anim += "_forward";
				}
				else if ( unbiasedMovement.y > 0.707f ) {
					main_anim += "_backward";
				}
				else {
					if ( unbiasedMovement.x > 0.0f ) {
						main_anim += "_left";
					}
					else {
						main_anim += "_right";
					}
				}

				CAnimAction* action = anim.FindAction(main_anim);
				if ( action )
				{
					anim.PlaySmoothed( main_anim, 0.6f );
					action->reset_on_stop = false;
					if ( sAnimType.find("sprint") != string::npos ) {
						//std::cout << (vMotionVelocity.magnitude() / 11.667f) << std::endl;
						action->framesPerSecond = 30.0f * (vMotionVelocity.magnitude() / 17.0f );//11.667f);
					}
					else if ( sAnimType.find("run") != string::npos ) {
						//std::cout << (vMotionVelocity.magnitude() / 8.333f) << std::endl;
						action->framesPerSecond = 30.0f * (vMotionVelocity.magnitude() / 11.0f );//8.333f );
					}
					else {
						//std::cout << (vMotionVelocity.magnitude() / 5.0f) << std::endl;
						action->framesPerSecond = 30.0f * (vMotionVelocity.magnitude() / 5.0f );
					}
					action->enableMotionExtrapolation[0] = false;
					action->enableMotionExtrapolation[1] = false;
					action->enableMotionExtrapolation[2] = false;

					validAnim = true;
				}
			}
			// Animation name fallback
			if ( !validAnim )
			{
				anim.PlaySmoothed( sAnimType, 0.35f );
				anim[sAnimType].reset_on_stop = false;
				//anim["run"].framesPerSecond = 37.5f;
				if ( sAnimType.find("sprint") != string::npos ) {
					anim[sAnimType].framesPerSecond = 30.0f * (vMotionVelocity.magnitude() / 17.0f );
				}
				if ( sAnimType.find("run") != string::npos ) {
					//anim[sAnimType].framesPerSecond = 30.0f * (vMotionVelocity.magnitude() / 9.625);
					anim[sAnimType].framesPerSecond = 30.0f * (vMotionVelocity.magnitude() / 11.0f );
				}
				else {
					anim[sAnimType].framesPerSecond = 30.0f * (vMotionVelocity.magnitude() / 5.0f );
				}
				anim[sAnimType].enableMotionExtrapolation[0] = false;
				anim[sAnimType].enableMotionExtrapolation[1] = false;
				anim[sAnimType].enableMotionExtrapolation[2] = false;
				/*anim.PlaySmoothed( "mv2_run", 0.5f );
				anim["mv2_run"].reset_on_stop = false;
				anim["mv2_run"].framesPerSecond = 30.0f * (vMotionVelocity.magnitude() / 9.625);
				anim["mv2_run"].enableMotionExtrapolation[0] = false;
				anim["mv2_run"].enableMotionExtrapolation[1] = false;
				anim["mv2_run"].enableMotionExtrapolation[2] = false;*/
				validAnim = true;
			}
			if (!( anim["jump"].isPlaying || anim["jump_03"].isPlaying )) {
				iFallType = rand() % 2;
			}
		}
		else
		{
			anim.PlaySmoothed( sAnimType, 0.5f );
			anim[sAnimType].reset_on_stop = false;
			validAnim = true;
		}
		iIdleType = LockNone;
	}
	else if (( sAnimType == "crouch_move" )||( sAnimType == "prone_move" || sAnimType == "crawl_forward_1" || sAnimType == "crawl_sprint_1" ))
	{
		anim.PlaySmoothed( sAnimType, 0.5f );
		anim[sAnimType].reset_on_stop = false;
		anim["crouch_move"].framesPerSecond = 45.0f;
		anim["crawl_forward_1"].framesPerSecond = 65.0f;
		anim["crawl_sprint_1"].framesPerSecond = 70.0f;
		validAnim = true;

		if ( sAnimType == "crouch_move" ) {
			iIdleType = LockMajor;
		}
		else if ( sAnimType == "prone_move" || sAnimType == "crawl_forward_1" || sAnimType == "crawl_sprint_1" ) {
			iIdleType = LockAllButNeck;
		}
	}
	// Todo: move this if-statement to a PlayAnimationIdle() type
	/*else if (( sAnimType == "idle" )||( sAnimType == "idle_02" )
		||( sAnimType == "crouch_idle" )||( sAnimType == "crouch_idle_2" )||( sAnimType == "prone_idle" )||( sAnimType == "crawl_idle_1" )
		||( sAnimType == "fly_idle" )||( sAnimType == "idle_combat_01" )||( sAnimType == "idle_gaurded_01" )
		||( sAnimType == "idle_magic_showoff" )||( sAnimType == "idle_relaxed_hover_01" )||( sAnimType == "idle_relaxed_hover_02" )
		||( sAnimType == "idle_relaxed_01" )||( sAnimType == "idle_relaxed_02" )
		||( sAnimType == "idle_relaxed_jolly_01" )||( sAnimType == "idle_relaxed_jolly_02" )
		||( sAnimType == "idle_relaxed_gloomy_01" )||( sAnimType == "idle_relaxed_gloomy_02" )||( sAnimType == "idle_relaxed_gloomy_03" )
		||( sAnimType == "idle_relaxed_hammy_02" )
		||( sAnimType == "h2h_idle_l" )||( sAnimType == "h2h_idle_r" )
		)
	{
		if ( fArg < -0.1f ) {
			anim.Play( sAnimType );
		}
		else {
			anim.PlaySmoothed( sAnimType, 0.3f );
		}
		anim[sAnimType].reset_on_stop = false;
		validAnim = true;

		if (!( anim["jump"].isPlaying || anim["jump_03"].isPlaying )) {
			iFallType = 0;
		}

		if ( sAnimType == "crouch_idle" ) {
			iIdleType = LockMajor;
		}
		else if ( sAnimType == "prone_idle" || sAnimType == "crawl_idle_1" || sAnimType == "crouch_idle_2" ) {
			iIdleType = LockAllButNeck;
		}
		else {
			iIdleType = LockNone;
		}
	}*/
	else if ( sAnimType == "swim" )
	{
		anim.PlaySmoothed( "swim", 0.4f );
		anim["swim"].framesPerSecond = 23.0f;
		validAnim = true;

		iFallType = rand() % 2;
		m_IKFootEnable = false;

		iIdleType = LockAllButNeck;
	}
	else if ( sAnimType == "swim_tread" )
	{
		anim.PlaySmoothed( "swim_tread", 0.7f );
		anim["swim_tread"].framesPerSecond = 13.0f;
		validAnim = true;

		iFallType = rand() % 2;
		m_IKFootEnable = false;

		iIdleType = LockAllButNeck;
	}
	else if ( sAnimType == "hang_idle" )
	{
		/*anim.PlaySmoothed( "hang_ledge", 0.4f );
		anim["hang_ledge"].framesPerSecond = 23.0f;*/
		anim.PlaySmoothed( "hang_idle", 0.4f );
		anim["hang_idle"].playSpeed = 0.9f; // Slightly slow down the animation

		iFallType = rand() % 2; // Randomize fall type between type 0 and 1
		iIdleType = LockAllButHead; // Lock all the body but the head
		m_IKFootEnable = false;

		validAnim = true; // Animation is valid
	}

	if ( sAnimType == "jump" )
	{
		anim.Play( sAnimType );
		validAnim = true;

		// Randomize fall animations
		if ( rand() % 4 == 0 ) {
			iFallType = 2;
		}
		else {
			iFallType = 1;
		}
	}
	else if ( sAnimType == "jump_sprint" )
	{
		anim.Play( "jump_03" );
		validAnim = true;

		iFallType = 3;
	}
	else if ( sAnimType == "land" )
	{
		anim.Play( sAnimType );
		anim["land"].framesPerSecond = 20.0f;
		anim["land"].end_behavior = 1;
		validAnim = true;

		iFallType = 0;
	}
	else if ( sAnimType == "fall" )
	{
		string targetAnimName = "fall_reg";
		if ( iFallType == 2 ) {
			targetAnimName = "fall";
		}
		else if ( iFallType == 1 ) {
			targetAnimName = "fall_03";
		}
		else if ( iFallType == 3 ) {
			targetAnimName = "fall_sprint";
		}
		else {
			targetAnimName = "fall_reg";
		}

		if ( anim["jump"].isPlaying || anim["jump_03"].isPlaying ) {
			anim.PlaySmoothed( targetAnimName, 0.1f );
		}
		else {
			anim.PlaySmoothed( targetAnimName, 0.7f );
		}
		if ( iFallType != 3 ) {
			anim[targetAnimName].frame = std::max( 0.0f, (1.0f-vMotionVelocity.z)*0.4f );
			if ( anim[targetAnimName].frame >= anim[targetAnimName].Length()-2 ) {
				anim[targetAnimName].frame = anim[targetAnimName].Length()-2;
			}
			anim[targetAnimName].framesPerSecond = 0.001f;
		}
		else {
			anim[targetAnimName].framesPerSecond = std::min( 24.0f, std::max( 7.0f, -vMotionVelocity.z*1.1f - 3.7f ) );
		}

		validAnim = true;

		iIdleType = LockNone;
	}

	if ( sAnimType == "roll" )
	{
		anim.PlaySmoothed( "roll", 0.2f );
		anim["roll"].framesPerSecond = 76.0f;

		iIdleType = LockAllButNeck;
		m_IKFootEnable = false;

		validAnim = true;
	}
	else if ( sAnimType == "slide" )
	{
		anim.PlaySmoothed( "slide", 0.4f );
		anim["slide"].framesPerSecond = 76.0f;

		iIdleType = LockMajor;

		validAnim = true;
	}
	else if ( sAnimType == "falldown_idle_0" )
	{
		anim.PlaySmoothed( "falldown_idle_0", 0.3f );
		anim["falldown_idle_0"].framesPerSecond = 14.0f;
		iIdleType = LockAll;

		validAnim = true;
	}
	else if ( sAnimType == "falldown_idle_1" )
	{
		anim.PlaySmoothed( "falldown_idle_1", 0.4f );
		anim["falldown_idle_0"].framesPerSecond = 14.0f;
		iIdleType = LockMajor;

		validAnim = true;
	}
	else if ( sAnimType == "vault_4h" )
	{
		anim.PlaySmoothed( "vault_4h", 0.2f ); // Smooth the vaulting animation in
		if ( fArg > 0.05f )
		{
			anim["vault_4h"].framesPerSecond = fAnimSpeed;
			anim["vault_4h"].frame = fArg;
			anim["vault_4h"].end_behavior = 0;
		}
		m_IKFootEnable = false;

		validAnim = true;
	}
	else if ( sAnimType == "hang_climb" )
	{
		anim.PlaySmoothed( "hang_climb", 0.1f );
		if ( anim["hang_climb"].end_behavior == 2 ) {
			anim["hang_climb"].frame = 0;
			anim["hang_climb"].framesPerSecond = 30.0f;
			anim["hang_climb"].end_behavior = 0;
		}
		iIdleType = LockAllButHead; // Lock all the body but the head
		m_IKFootEnable = false;

		validAnim = true;
	}


	// DRAW
	if ( sAnimType == "hold_onehanded_draw" )
	{
		if ( rand() % 4 == 0 )
			anim.Play( "hold_onehanded_draw" );
		else
			anim.Play( "hold_onehanded_draw_fast" );
		validAnim = true;
	}
	else if ( sAnimType == "hold_oneblade_draw" )
	{
		if ( rand() % 4 == 0 )
			anim.Play( "hold_oneblade_draw" );
		else
			anim.Play( "hold_oneblade_draw_top" );
		validAnim = true;
	}
	else if ( sAnimType == "hold_torch_draw" )
	{
		if ( rand() % 4 == 0 )
			anim.Play( "hold_torch_draw" );
		else
			anim.Play( "hold_torch_draw_02" );
		validAnim = true;
	}
	else if ( sAnimType == "hold_twohanded_draw" )
	{
		anim.Play( "hold_twohanded_draw" );
		validAnim = true;
	}
	// IDLE
	else if (( sAnimType == "hold_onehanded" )||( sAnimType == "hold_torch" )||( sAnimType == "hold_twohanded" ))
	{
		anim.Play( sAnimType );
		validAnim = true;
	}
	else if ( sAnimType == "hold_oneblade" ) {
		anim.Play( "hold_oneblade_01_idle" );
		validAnim = true;
	}
	else if ( sAnimType == "hold_twoblade" ) {
		anim.Play( "hold_twoblade_01_idle" );
		validAnim = true;
	}
	else if ( sAnimType == "hold_bow_idle" )
	{
		anim.Play( sAnimType );
		/*CAnimAction* action = anim.FindAction( "hold_bow_aim" );
		if ( action ) {
			action->weight -= Time::deltaTime * 4.0f;
			if ( action->weight <= Time::deltaTime * 4.0f ) {
				action->Stop();
			}
		}*/
		if ( fAnimCooldown < 0 ) {
			CAnimAction* action = anim.FindAction( "hold_bow_aim" );
			if ( action ) {
				action->weight -= Time::deltaTime * 4.0f;
				if ( action->weight <= Time::deltaTime * 4.0f ) {
					action->Stop();
				}
			}
		}
		validAnim = true;
	}
	else if ( sAnimType == "hold_bow_aim" )
	{
		CAnimAction* action = anim.FindAction( "hold_bow_aim" );
		if ( action ) {
			action->Play( 1.0f, 0.2f );
			action->framesPerSecond = 0.01f;
			action->frame = (action->GetLength()-1.3f) * fArg;
		}
		fAnimCooldown = 0.2f;
		validAnim = true;
	}
	// ATTACKS
	else if ( sAnimType == "hold_onehanded_toss" )
	{
		anim.Stop( "hold_onehanded_toss" );
		anim.Play( "hold_onehanded_toss" );
		anim["hold_onehanded_toss"].frame = fArg;
		validAnim = true;
	}
	else if ( sAnimType == "hold_onehanded_smack" ) // ATK 
	{
		anim.Stop( "hold_onehanded_smack" );
		anim.Play( sAnimType );
		anim[sAnimType].framesPerSecond = 32.0f + random_range( 0.0f,7.0f );
		validAnim = true;
	}
	else if ( sAnimType == "hold_twohanded_smack" ) // ATK 
	{
		anim.Stop( "hold_twohanded_smack" );
		anim.Play( sAnimType );
		anim[sAnimType].framesPerSecond = 36.0f + random_range( 0.0f,7.0f );
		validAnim = true;
	}
	else if ( sAnimType == "hold_onebladed_charge" )
	{
		int choice;
		choice = (int)(fArg+0.4f);
		CAnimAction* action = NULL;
		CAnimAction* atk_actions[4];
		atk_actions[0] = anim.FindAction( "hold_oneblade_01_prepare-0" );
		atk_actions[1] = anim.FindAction( "hold_oneblade_01_prepare-2" );
		atk_actions[2] = anim.FindAction( "hold_oneblade_01_prepare-4" );
		atk_actions[3] = anim.FindAction( "hold_oneblade_01_prepare-6" );
		switch ( choice ) {
			case 0: 
			case 1: action = atk_actions[0]; break;
			case 2: 
			case 3: action = atk_actions[1]; break;
			case 8:
			case 4: 
			case 5: action = atk_actions[2]; break;
			case 6: 
			case 7: action = atk_actions[3]; break;
		}

		// sync up with other animations when charging up for first time
		ftype maxframe = 0;
		bool isPlaying = false;
		if ( !action->isPlaying ) {
			for ( uint i = 0; i < 4; ++i ) {
				if ( atk_actions[i]->isPlaying ) {
					if ( atk_actions[i]->frame <= 25 ) {
						maxframe = std::max<ftype>( maxframe, atk_actions[i]->frame );
						isPlaying = true;
					}
				}
			}
		}
		action->Play( 1.0f, 0.2f );
		if ( isPlaying ) { // apply next frame if similar in set already playing
			action->frame = maxframe;
		}
		if ( action->frame >= action->GetLength()-4 ) {
			action->framesPerSecond = 0;
			action->frame = action->GetLength()-3;
		}
		else {
			action->framesPerSecond = 30.0f;
		}
		validAnim = true;
	}
	else if (( sAnimType == "hold_onebladed_attack"  )||( sAnimType == "hold_onebladed_attack_quick" ))
	{
		int choice;
		choice = (int)(fArg+0.4f);
		CAnimAction* action = NULL;
		switch ( choice ) {
			case 0: action = anim.FindAction( "hold_oneblade_01_swing-0" ); break;
			case 1: action = anim.FindAction( "hold_oneblade_01_swing-1" ); break;
			case 2: action = anim.FindAction( "hold_oneblade_01_swing-2" ); break;
			case 3: action = anim.FindAction( "hold_oneblade_01_swing-3" ); break;
			case 4: action = anim.FindAction( "hold_oneblade_01_swing-4" ); break;
			case 5: action = anim.FindAction( "hold_oneblade_01_swing-5" ); break;
			case 6: action = anim.FindAction( "hold_oneblade_01_swing-6" ); break;
			case 7: action = anim.FindAction( "hold_oneblade_01_swing-7" ); break;
			case 8: action = anim.FindAction( "hold_oneblade_01_swing-8" ); break;
		}
		if ( action->isPlaying ) {
			action->Stop();
		}
		action->Play( 1.0f+fAnimSpeed );
		if ( sAnimType == "hold_onebladed_attack_quick" ) {
			action->frame = std::max<ftype>( 0.34f, fAttackSkip ) * action->GetEvent(Animation::Event_Attack);
			action->framesPerSecond = 38.0f;
		}
		else {
			action->frame = fAttackSkip * action->GetEvent(Animation::Event_Attack);
			action->framesPerSecond = 30.0f;
		}
		validAnim = true;
	}
	else if (( sAnimType == "hold_twobladed_attack"  )||( sAnimType == "hold_twobladed_attack_quick" ))
	{
		/*int choice;
		choice = (int)(fArg+0.4f);
		CAnimAction* action;
		switch ( choice ) {
			case 0: action = anim.FindAction( "hold_oneblade_01_swing-0" ); break;
			case 1: action = anim.FindAction( "hold_oneblade_01_swing-1" ); break;
			case 2: action = anim.FindAction( "hold_oneblade_01_swing-2" ); break;
			case 3: action = anim.FindAction( "hold_oneblade_01_swing-3" ); break;
			case 4: action = anim.FindAction( "hold_oneblade_01_swing-4" ); break;
			case 5: action = anim.FindAction( "hold_oneblade_01_swing-5" ); break;
			case 6: action = anim.FindAction( "hold_oneblade_01_swing-6" ); break;
			case 7: action = anim.FindAction( "hold_oneblade_01_swing-7" ); break;
			case 8: action = anim.FindAction( "hold_oneblade_01_swing-8" ); break;
		}*/
		CAnimAction* action = NULL;
		if ( nArm == 1 ) action = anim.FindAction( "hold_twoblade_01_swing-left-0-4" );
		if ( nArm == 0 ) action = anim.FindAction( "hold_twoblade_01_swing-right-0-4" );

		if ( action->isPlaying ) {
			action->Stop();
		}
		action->Play( 1.0f+fAnimSpeed );
		if ( sAnimType == "hold_twobladed_attack_quick" ) {
			action->frame = std::max<ftype>( 0.34f, fAttackSkip ) * action->GetEvent(Animation::Event_Attack);
			action->framesPerSecond = 38.0f;
		}
		else {
			action->frame = fAttackSkip * action->GetEvent(Animation::Event_Attack);
			action->framesPerSecond = 30.0f;
		}
		validAnim = true;
	}
	// DEFEND
	else if ( sAnimType == "hold_onebladed_defend" )
	{
		int choice;
		choice = (int)(fArg+0.4f);
		CAnimAction* action = NULL;
		switch ( choice ) {
			case 0: action = anim.FindAction( "hold_oneblade_01_block-4" ); break;
			case 1: action = anim.FindAction( "hold_oneblade_01_block-1" ); break;
			case 2: action = anim.FindAction( "hold_oneblade_01_block-2" ); break;
			case 3: action = anim.FindAction( "hold_oneblade_01_block-3" ); break;
			case 4: action = anim.FindAction( "hold_oneblade_01_block-4" ); break;
			case 5: action = anim.FindAction( "hold_oneblade_01_block-5" ); break;
			case 6: action = anim.FindAction( "hold_oneblade_01_block-2" ); break;
			case 7: action = anim.FindAction( "hold_oneblade_01_block-3" ); break;
			case 8: action = anim.FindAction( "hold_oneblade_01_block-4" ); break;
		}
		//anim.Play( "hold_oneblade_defend_base" );
		//anim["hold_oneblade_defend_base"].Play( 1.0f,0.6f );
		action->Play( 1.0f,0.8f );

		if ( anim["hold_oneblade_01_parry"].isPlaying ) {
			anim["hold_oneblade_01_parry"].weight = std::min<ftype>( 1, (1-(anim["hold_oneblade_01_parry"].frame / (anim["hold_oneblade_01_parry"].GetLength()-8) ))*2 );
			if ( anim["hold_oneblade_01_parry"].weight <= 0 ) {
				anim["hold_oneblade_01_parry"].Stop();
			}
		}
		validAnim = true;
	}
	else if ( sAnimType == "hold_twobladed_defend" )
	{
		CAnimAction* action;
		action = anim.FindAction( "hold_twoblade_01_block-3" );
		action->Play( 1.0f,0.8f );

		if ( anim["hold_twoblade_01_parry"].isPlaying ) {
			anim["hold_twoblade_01_parry"].weight = std::min<ftype>( 1, (1-(anim["hold_twoblade_01_parry"].frame / (anim["hold_twoblade_01_parry"].GetLength()-8) ))*2 );
			if ( anim["hold_twoblade_01_parry"].weight <= 0 ) {
				anim["hold_twoblade_01_parry"].Stop();
			}
		}
	}
	// PARRY
	else if ( sAnimType == "hold_onebladed_parry" )
	{
		anim.Stop( "hold_oneblade_01_parry" );
		anim.Play( "hold_oneblade_01_parry" );
		anim["hold_oneblade_01_parry"].frame = 1;
		anim["hold_oneblade_01_parry"].framesPerSecond = 31.0f;
		validAnim = true;
	}
	else if ( sAnimType == "hold_twobladed_parry" )
	{
		anim.Stop( "hold_twoblade_01_parry" );
		anim.Play( "hold_twoblade_01_parry" );
		anim["hold_twoblade_01_parry"].frame = 1;
		anim["hold_twoblade_01_parry"].framesPerSecond = 31.0f;
		validAnim = true;
	}
	else if ( sAnimType == "hold_onebladed_defend_stop" )
	{
		anim.Stop("hold_oneblade_defend_base");
	}
	// HOLSTER
	else if ( sAnimType == "hold_onehanded_holster" )
	{
		anim.Stop( "hold_oneblade_01_idle" );
		anim.Stop( "hold_onehanded_draw" );
		anim.Stop( "hold_onehanded_draw_fast" );
		anim.Stop( "hold_oneblade_01_block-1" );
		anim.Stop( "hold_oneblade_01_block-2" );
		anim.Stop( "hold_oneblade_01_block-3" );
		anim.Stop( "hold_oneblade_01_block-4" );
		anim.Stop( "hold_oneblade_01_block-5" );
		anim.Stop( "hold_torch" );
		anim.Stop( "hold_torch_draw" );
		anim.Stop( "hold_torch_draw_02" );
		//anim.Stop( "hold_oneblade" );
		anim.Stop( "hold_oneblade_draw" );
		anim.Stop( "hold_oneblade_draw_top" );
		anim.Stop( "hold_onehanded" );
		anim.Stop( "hold_onehanded_smack" );
		validAnim = true;
	}
	else if ( sAnimType == "hold_twohanded_holster" )
	{
		anim.Stop( "hold_twohanded" );
		anim.Stop( "hold_twohanded_draw" );
		anim.Stop( "hold_twohanded_smack" );
		validAnim = true;
	}
	else if ( sAnimType == "hold_bow_holster" )
	{
		anim.Stop( "hold_bow" );
		anim.Stop( "hold_bow_aim" );
		validAnim = true;
	}

	if ( sAnimType == "fisticuffs_punch" )
	{
		anim.Stop( "fisticuffs_left" );
		anim.Stop( "fisticuffs_right" );
		anim.Stop( "fisticuffs_left_hit" );
		anim.Stop( "fisticuffs_right_hit" );
		string targetAnimName = "fisticuffs_left";
		if ( rand() % 2 == 0 ) {
			targetAnimName = "fisticuffs_right";
		}
		anim.Play( targetAnimName );
		validAnim = true;
	}
	else if ( sAnimType == "fisticuffs_punch_hit" )
	{
		anim.Stop( "fisticuffs_left" );
		anim.Stop( "fisticuffs_right" );
		anim.Stop( "fisticuffs_left_hit" );
		anim.Stop( "fisticuffs_right_hit" );
		string targetAnimName = "fisticuffs_left_hit";
		if ( rand() % 2 == 0 ) {
			targetAnimName = "fisticuffs_right_hit";
		}
		anim.Play( targetAnimName );
		validAnim = true;
	}
	else if ( sAnimType == "punch_neutral_break" )
	{
		if ( anim["fisticuffs_left"].isPlaying || anim["fisticuffs_left_hit"].isPlaying ) {
			anim.Play( "fisticuffs_left_hit_broke" );
		}
		else {
			anim.Play( "fisticuffs_right_hit_broke" );
		}
		validAnim = true;
	}

	if ( sAnimType == "spell01_draw" )
	{
		validAnim = true;
	}
	if ( sAnimType == "spell01_idle" )
	{
		anim.Play( "spell01_idle" );
		validAnim = true;
	}
	else if ( sAnimType == "spell01_snap" )
	{
		anim.Play( "spell01_snap" );
		validAnim = true;
	}
	else if ( sAnimType == "spell01_holster" )
	{
		//anim.Stop( "spell01_snap" );
		anim.Stop( "spell01_idle" );
		validAnim = true;
	}

	if ( sAnimType == "spell02_draw" )
	{
		validAnim = true;
	}
	if ( sAnimType == "spell02_idle" )
	{
		anim.Play( "spell02_idle" );
		validAnim = true;
	}
	else if ( sAnimType == "spell02_cast" )
	{
		anim.Play( "spell02_cast" );
		validAnim = true;
	}
	else if ( sAnimType == "spell02_holster" )
	{
		anim.Stop( "spell02_idle" );
		validAnim = true;
	}
	

	if ( sAnimType == "anim" ) {
		validAnim = true;
	}

	if ( !validAnim ) 
	{
		CAnimAction* action = anim.FindAction(sAnimType);
		if ( action )
		{
			if ( fNextBlendSpeed < FTYPE_PRECISION )
			{
				// Check for action in the blendoutList
				for ( auto blend = blendstoppedList.begin(); blend != blendstoppedList.end(); ++blend )
				{
					if ( blend->action == action )
					{	// Zero out its weight
						action->weight = 0.0f;
						break;
					}
				}
				// Instant play
				if ( action->weight < 0.5f )
				{
					action->end_behavior = 1;
					action->Play();
					// Set animation playback speed
					if ( fAnimSpeed > FTYPE_PRECISION ) {
						action->playSpeed = fAnimSpeed;
					}
					// Check for the action in the blendlist
					for ( auto blend = blendinoutList.begin(); blend != blendinoutList.end(); ++blend )
					{
						if ( blend->action == action )
						{ // If it is, set the weight to nearly zero
							action->weight = (Real) FTYPE_PRECISION * 2.0f;
							break;
						}
					}
				}
			}
			else
			{	// Smooth blend in
				action->end_behavior = 1;
				action->Play( 1.0f, fNextBlendSpeed );
				fNextBlendSpeed = 0;
			}
			// Mark animation as valid now
			validAnim = true;
		}
	}

#ifdef _ENGINE_DEBUG
	if ( !validAnim )
	{
		//charModel->GetAnimation()->PlaySmoothed( "hold_onehanded", 0.1f );
		std::cout << "Could not find an action for animation " << sAnimType << std::endl;
	}
#endif
	return validAnim;
}


bool CCharacterModel::PlayAnimationIdle ( const string &sAnimType, const float fArg, const float fAnimSpeed, const float fAttackSkip, const int nArm )
{
	CAnimation& anim = (*(charModel->GetAnimation()));

	// Reset fall type
	if ( iFallType < 0 )
		iFallType = 0;

	CAnimAction* action = anim.FindAction( sAnimType );
	if ( action != NULL )
	{
		// Play the animation
		if ( fArg < -0.1f ) {
			action->Play( 1.0f );
		}
		else {
			action->Play( 1.0f, 0.3f );
		}
		action->reset_on_stop = false;

		// Set falltype based on the input
		if (!( anim["jump"].isPlaying || anim["jump_03"].isPlaying )) {
			iFallType = 0;
		}

		// Set body backbend locktype
		if ( sAnimType == "crouch_idle" ) {
			iIdleType = LockMajor;
		}
		else if ( sAnimType == "prone_idle" || sAnimType == "crawl_idle_1" || sAnimType == "crouch_idle_2" ) {
			iIdleType = LockAllButNeck;
		}
		else {
			iIdleType = LockNone;
		}

		// Return success
		return true;
	}
	else
	{
#ifdef _ENGINE_DEBUG
		std::cout << "Could not find an action for animation " << sAnimType << std::endl;
#endif
		return false;
	}
}

void CCharacterModel::UpdateAnimations ( void )
{
	if ( currentSequence )
	{
		bPerformSequenceInterrupts = false;
		currentSequence->Update();
		bPerformSequenceInterrupts = true;
		if ( currentSequence->isPlaying == false ) {
			currentSequence = NULL;
		}
	}


	CAnimation& anim = (*(charModel->GetAnimation()));

	/*if ( anim.FindAction("land") )
	{
		if ( anim["land"].isPlaying )
		{
			Real framePercentage = (anim["land"].frame/anim["land"].GetLength());
			Real calculatedWeight = 1;
			calculatedWeight *= std::min( 1.0f, (1-framePercentage)*2 ); // 50% fade out
			calculatedWeight *= std::min( 1.0f, (framePercentage)*8 ); // 12.5% fade in
			anim["land"].weight = std::max( 0.0f, std::min( 1.0f, calculatedWeight ) );
		}
	}
	if ( anim.FindAction("jump") && anim["jump"].isPlaying )
	{
		Real framePercentage = (anim["jump"].frame/anim["jump"].GetLength());
		Real calculatedWeight = 1;
		calculatedWeight *= std::min( 1.0f, (1-framePercentage)*2 );
		calculatedWeight *= std::min( 1.0f, (framePercentage)*8 );
		anim["jump"].weight = std::max( 0.0f, std::min( 1.0f, calculatedWeight ) );
	}
	if ( anim.FindAction("jump_03") && anim["jump_03"].isPlaying )
	{
		Real framePercentage = (anim["jump_03"].frame/anim["jump_03"].GetLength());
		Real calculatedWeight = 1;
		calculatedWeight *= std::min( 1.0f, (1-framePercentage)*2 );
		calculatedWeight *= std::min( 1.0f, (framePercentage)*8 );
		anim["jump_03"].weight = std::max( 0.0f, std::min( 1.0f, calculatedWeight ) );
	}
	if ( anim.FindAction("fall_hard") && anim["fall_hard"].isPlaying )
	{
		anim["fall_hard"].weight = ( 1 - (anim["fall_hard"].frame/anim["fall_hard"].GetLength()) ) * 4;
		if ( anim["fall_hard"].weight > 1 ) {
			anim["fall_hard"].weight = 1;
		}
	}
	if ( anim.FindAction("stun_stagger_hard") && anim["stun_stagger_hard"].isPlaying )
	{
		Real framePercentage = (anim["stun_stagger_hard"].frame/anim["stun_stagger_hard"].GetLength());
		Real calculatedWeight = 1;
		calculatedWeight *= std::min( 1.0f, (1-framePercentage)*4 );
		calculatedWeight *= std::min( 1.0f, (framePercentage)*6 );
		anim["stun_stagger_hard"].weight = std::max( 0.0f, std::min( 1.0f, calculatedWeight ) );
	}
	if ( anim.FindAction("stun_stagger_twist") && anim["stun_stagger_twist"].isPlaying )
	{
		Real framePercentage = (anim["stun_stagger_twist"].frame/anim["stun_stagger_twist"].GetLength());
		Real calculatedWeight = 1;
		calculatedWeight *= std::min( 1.0f, (1-framePercentage)*4 );
		calculatedWeight *= std::min( 1.0f, (framePercentage)*6 );
		anim["stun_stagger_twist"].weight = std::max( 0.0f, std::min( 1.0f, calculatedWeight ) );
	}
	if ( anim.FindAction("falldown_end") && anim["falldown_end"].isPlaying )
	{
		anim["falldown_end"].weight = ( 1 - (anim["falldown_end"].frame/anim["falldown_end"].GetLength()) ) * 4;
		if ( anim["falldown_end"].weight > 1 ) {
			anim["falldown_end"].weight = 1;
		}
	}*/

	// Fadeout animations
	if ( anim.FindAction("vault_4h") )
	{
		if ( anim["vault_4h"].isPlaying && anim["vault_4h"].end_behavior == 2 )
		{
			m_IKFootEnable = false;
			anim["vault_4h"].weight -= Time::deltaTime;
			if ( anim["vault_4h"].weight <= 0.0f ) {
				anim["vault_4h"].isPlaying = false;
				anim["vault_4h"].weight = 0.0f;
			}
		}
	}
	if ( anim.FindAction("hang_climb") )
	{
		if ( anim["hang_climb"].isPlaying && anim["hang_climb"].end_behavior == 2 ) 
		{
			m_IKFootEnable = false;
			anim["hang_climb"].weight -= Time::deltaTime;
			if ( anim["hang_climb"].weight <= 0.0f ) {
				anim["hang_climb"].isPlaying = false;
				anim["hang_climb"].weight = 0.0f;
			}
		}
	}

	// Perform auto blend-in/out system.
	for ( auto blend = blendinoutList.begin(); blend != blendinoutList.end(); ++blend )
	{
		if ( blend->action && blend->action->isPlaying )
		{
			Real calculatedWeight = 1.0f;
			Real t_frame = blend->action->frame;
			Real t_length = blend->action->GetLength();

			// Perform blend in based on frame count
			if ( blend->in > FTYPE_PRECISION ) {
				calculatedWeight *= std::min<Real>( (t_frame)/blend->in, 1.0f );
			}
			if ( blend->out > FTYPE_PRECISION ) {
				calculatedWeight *= std::min<Real>( (t_length - t_frame)/blend->out, 1.0f );
			}

			// Set the final weight
			blend->action->weight = Math.Clamp( calculatedWeight, 0.0f, 1.0f );
			// Stop animation if done playing
			if ( blend->action->weight <= 0.0f ) {
				blend->action->isPlaying = false;
			}
		}
		//
	}
	// Perform auto blend-out-stopped system.
	for ( auto blend = blendstoppedList.begin(); blend != blendstoppedList.end(); ++blend )
	{
		if ( blend->action && !blend->action->isPlaying && blend->action->weight > 0 )
		{
			blend->action->weight = std::max<Real>( 0.0f, blend->action->weight - Time::deltaTime * blend->out );
		}
	}

}


void CCharacterModel::AddAnimationBlendout ( const char* n_anim, const Real n_fadeIn, const Real n_fadeOut )
{
	CAnimation& anim = (*(charModel->GetAnimation()));
	CAnimAction* action = anim.FindAction( n_anim );
	if ( action ) 
	{
		blendoutAnimation_t blend;
		blend.action = action;
		blend.in = n_fadeIn;
		blend.out = n_fadeOut;
		blendinoutList.push_back( blend );
	}
}
void CCharacterModel::AddAnimationBlendoutPct ( const char* n_anim, const Real n_fadeInPct, const Real n_fadeOutPct )
{
	CAnimation& anim = (*(charModel->GetAnimation()));
	CAnimAction* action = anim.FindAction( n_anim );
	if ( action ) 
	{
		// Get the animation length and create the frame counts from that 
		Real fadeIn_frames = action->GetLength() * n_fadeInPct;
		Real fadeOut_frames = action->GetLength() * n_fadeOutPct;
		AddAnimationBlendout( n_anim, fadeIn_frames, fadeOut_frames );
	}
}

void CCharacterModel::AddAnimationBlendstopped ( const char* n_anim, const Real n_fadeOutTime )
{
	CAnimation& anim = (*(charModel->GetAnimation()));
	CAnimAction* action = anim.FindAction( n_anim );
	if ( action ) 
	{
		blendoutAnimation_t blend;
		blend.action = action;
		blend.out = 1 / n_fadeOutTime;
		blendstoppedList.push_back( blend );
	}
}


// Interface to edit sequences
void CCharacterModel::AddSequence ( const char* n_filename, const char* n_sequencename, const uchar n_layer, const bool n_loopoverride )
{
	Animation::Sequence* sequence = new Animation::Sequence();

	if ( !sequence->LoadFromFile( n_filename ) ) {
		throw std::exception();
	}

	if ( n_loopoverride ) {
		sequence->m_loop = n_loopoverride;
	}
	sequence->m_layer = n_layer;
	sequence->m_name = n_sequencename;
	sequence->m_target = this;

	sequences.push_back( sequence );
}


// SetMoveAnimation()
// Sets the animation to play every step automatically.
bool CCharacterModel::SetMoveAction ( string const& sActionType )
{
	return SetMoveAnimation( sActionType );
}
bool CCharacterModel::SetMoveAnimation ( string const& sAnimType )
{
	// Reset idle mode
	bIsIdling = false;

	// Check if the input is an SEQUENCE.
	int sequenceInfo = -1;
	for ( unsigned int i = 0; i < sequences.size(); ++i ) {
		if ( sequences[i]->m_name == sAnimType ) {
			sequenceInfo = i;
			break;
		}
	}
	if ( sequenceInfo >= 0 )
	{
		if ( currentSequence && currentSequence->isPlaying )
		{
			if ( currentSequence->m_layer > sequences[sequenceInfo]->m_layer )
			{
				return false;
			}
		}
		// Set the current target sequence
		currentSequence = sequences[sequenceInfo];
		currentSequence->Play();
		// Sequence, so set empty anim.
		sCurrentMoveAnim = "";
		return true;
	}
	// If it's not an SEQUENCE, then assume ANIMATION.
	else {
		sCurrentMoveAnim = sAnimType;
		CAnimation& anim = (*(charModel->GetAnimation()));
		if ( anim["idle_relaxed_hover_02"].isPlaying )
		{
			if ( sAnimType.find("idle") != string::npos )
			{
				anim.StopSmoothed( "idle_relaxed_hover_02", 0.5f );
			}
		}
		return true;
	}
}
bool CCharacterModel::SetIdleAnimation ( string const& sAnimType )
{
	bool result = SetMoveAnimation(sAnimType);
	// Mark as idling
	bIsIdling = true;
	return result;
}
// Stops an animation entirely
void CCharacterModel::StopAnimation ( string const& sAnimType )
{
	CAnimation& anim = (*(charModel->GetAnimation()));
	anim[sAnimType].Stop();
}
// Stop an animation's playing
void CCharacterModel::StopAnimationPlaying ( const string& sAnimType )
{
	CAnimation& anim = (*(charModel->GetAnimation()));
	anim[sAnimType].isPlaying = false; // don't remove weight, as it will fade out automatically if not playing (this way can blend properly from interrupts into stuns)
}
//
void CCharacterModel::StopMoveAnimation ( string const& sAnimType )
{
	CAnimation& anim = (*(charModel->GetAnimation()));

	if ( sAnimType == "roll" ) {
		anim.StopSmoothed("roll",0.4f);
	}
	else if ( sAnimType == "slide" ) {
		anim.StopSmoothed("slide",0.4f);
	}
	else if ( sAnimType == "falldown_idle_0" ) {
		anim.StopSmoothed("falldown_idle_0",0.6f);
	}
	else if ( sAnimType == "falldown_idle_1" ) {
		anim.StopSmoothed("falldown_idle_1",0.4f);
	}
	else if ( sAnimType == "idle_relaxed_hover_02" ) {
		anim.StopSmoothed("idle_relaxed_hover_02",0.4f);
	}
	else if ( sAnimType == "vault_4h" ) {
		anim["vault_4h"].end_behavior = 2;
	}
	else if ( sAnimType == "hang_climb" ) {
		anim["hang_climb"].end_behavior = 2;
	}
}
bool CCharacterModel::SetAnimationMotion ( Vector3d const& velocity )
{
	vMotionVelocity = velocity;
	return true;
}
void CCharacterModel::SetMoveAnimationSpeed ( const ftype playbackSpeed )
{
	CAnimAction* mAction = charModel->GetAnimation()->FindAction( sCurrentMoveAnim );
	if ( mAction ) {
		//mAction->playSpeed = playbackSpeed;
		mAction->framesPerSecond = 30.0f * playbackSpeed;
	}
}