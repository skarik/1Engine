
#include "CNpcBase.h"

#include "core/math/random/Random.h"

#include "core-ext/input/emulated/CEmulatedInputControl.h"
#include "core-ext/animation/CAnimation.h"

#include "engine/physics/raycast/Raycaster.h"

#include "renderer/debug/CDebugDrawer.h"
#include "renderer/logic/model/CModel.h"

#include "after/entities/CCharacterModel.h"
#include "after/entities/character/CMccCharacterModel.h"
#include "after/entities/character/CRfCharacterModel.h"
#include "after/entities/character/CAfterPlayer.h"
#include "after/entities/item/weapon/CBaseRandomMelee.h"
#include "after/types/terrain/BlockType.h"
#include "after/types/terrain/BlockData.h"
#include "after/states/inventory/CPlayerInventory.h"

#include "after/terrain/edit/CTerrainAccessor.h"

// For damage
#include "after/entities/item/skill/skills/defensive/SkillShield.h"

using namespace NPC;

REGISTER_ZCC(CNpcBase);

CNpcBase::CONSTRUCTOR_ZCC_V2(CNpcBase)
{	ZCC_AddInstance();
	// Set default stats

	// Create Racial stats for a Level 1 Dark Elf
	m_stats = new CRacialStats();
	base_race_stats = new CRacialStats();
	*base_race_stats = *m_stats;
	// Give the new stats our own stats (TODO: constructor for CRacialStats to take stats)
	//delete m_stats->stats;
	m_stats->stats = &stats;
	m_stats->pOwner = this;
	// Set defaults
	m_stats->SetDefaults();
	m_stats->iRace = CRACE_ELF;
	//m_stats->iRace = CRACE_FLUXXOR;
	m_stats->SetLuaDefaults();
	m_stats->RerollColors();

	// Set the name to wanderer
	strcpy( charName, "Wanderer" );
	//m_stats->sPlayerName = charName;

	// Set the proper points for the character file
	characterFile.rstats	= m_stats;
	characterFile.worldstate= &m_worldstate;
	characterFile.prefs		= &m_prefs;
	characterFile.opinions	= &m_opinions;
}
//	PostLoad()
// Initialize all values that require stats
void CNpcBase::PostLoad ( void )
{
	// Set proper defaults again
	m_stats->SetLuaDefaults();
	*base_race_stats = *m_stats;

	// Set proper name
	strcpy( charName, m_stats->sPlayerName.c_str() );

	// Character Model
	pCharModel = new CMccCharacterModel( this );
	pCharModel->LoadBase( "clara" );

	CMccCharacterModel* mccModel = (CMccCharacterModel*)pCharModel;
	mccModel->SetVisualsFromStats( m_stats );

	model = pCharModel;

	// Init collision
	ai.hull.height = m_stats->fStandingHeight;
	ai.hull.radius = m_stats->fPlayerRadius;
	ai.info_combat.meleeRange = (m_stats->fPlayerRadius + 2.0f)*2.0f;

	pCollision = new CCapsuleCollider ( ai.hull.height, ai.hull.radius );
	pRigidBody = NULL;
	
	// create inventory
	inventory = new CPlayerInventory( this, 4,6, m_stats->iWieldCount, true );
	
	// Init move info
	MvtInit();

	// Create input emulator
	input = new CEmulatedInputControl();

	// Set up animator
	animator.m_model = mccModel;
	animator.m_race_stats = m_stats;
	animator.m_stats = &stats;
	animator.m_combat_info = &combatInfo;
	animator.m_inventory = inventory;

	// muck with inventory
	/*inventory->SwitchSpotHotbar( 3, 0 );
	//inventory->SwitchSpotHotbar( 1, 0 );
	inventory->ChangeEquippedTo( 0, false ); // Equip a sword.

	// Equip some clothes
	if ( Random.Range(0,1) < 0.4 ) {
		inventory->EquipArmorPiece( (CWearableItem*)inventory->GetItem(11), CPlayerInventory::GEAR_LEGS );
	}
	if ( Random.Range(0,1) < 0.4 ) {
		inventory->EquipArmorPiece( (CWearableItem*)inventory->GetItem(15), CPlayerInventory::GEAR_SHOULDER );
	}
	if ( Random.Range(0,1) < 0.3 ) {
		inventory->EquipArmorPiece( (CWearableItem*)inventory->GetItem(18), CPlayerInventory::GEAR_CHEST );
	}
	if ( Random.Range(0,1) < 0.3 ) {
		inventory->EquipArmorPiece( (CWearableItem*)inventory->GetItem(16), CPlayerInventory::GEAR_HEAD );
	}
	if ( m_stats->iRace != CRACE_KITTEN && m_stats->iRace != CRACE_MERCHANT )
	{
		if ( Random.Range(0,1) < 0.3 ) {
			inventory->EquipArmorPiece( (CWearableItem*)inventory->GetItem(10), CPlayerInventory::GEAR_LEFTFOOT );
		}	
		else if ( Random.Range(0,1) < 0.5 ) {
			inventory->EquipArmorPiece( (CWearableItem*)inventory->GetItem(19), CPlayerInventory::GEAR_LEFTFOOT );
		}
	}*/

	// init AI state values
	ai.owner = this;
	ai.rigidbody = pRigidBody;

	// NPC aggro type
	ai.SetAggroType( NPC::AGGRO_CHARACTER );

	// For now, set AI to companion
	//ai.SetFocus( NPC::AIFOCUS_Companion );
	ai.SetFocus( characterFile.worldstate->mFocus, characterFile.worldstate->mFocusName );
	SetPartyHost( characterFile.worldstate->partyHost );

	// Now mark AI as ready
	ai.routine_alert = "stand_alert";
	ai.routine_angry = "human_melee_angry";
	ai.Initialize();

	// Init move
	fStunTimer = 0;
	bIsStunned = 0;

	fMovementMultiplier = 1.0f;
	
	// ===Attachers===
	m_shield = NULL;
}
//  Stats
// Set new race stats. This is very dangerous, and a time consuming task for the engine.
void CNpcBase::SetRaceStats ( CRacialStats* n_stats_to_copy )
{
	// Set stats
	if ( m_stats != n_stats_to_copy ) {
		m_stats->CopyFrom( n_stats_to_copy );
		*base_race_stats = *m_stats;
	}
	// Load new visuals
	CMccCharacterModel* mccModel = (CMccCharacterModel*)pCharModel;
	mccModel->SetVisualsFromStats( m_stats );
	mccModel->UpdateTattoos();
}

CNpcBase::~CNpcBase ( void )
{
	model = NULL;
	delete_safe_decrement( pCharModel );
	//

	delete_safe_decrement( pRigidBody );
	delete_safe( pCollision );

	delete_safe( pModel );

	delete_safe( inventory );

	delete_safe( input );

	delete_safe( base_race_stats );
}

void CNpcBase::OnInteract ( CActor* interactingActor )
{
	if ( IsAlive() ) 
	{
		if ( interactingActor->GetTypeName() == "CPlayer" )
		{
			ai.RequestActorInteract( (CCharacter*)interactingActor, true );
		}
		else if ( interactingActor->IsCharacter() && false )
		{
			ai.RequestActorInteract( (CCharacter*)interactingActor, true );
		}
	}
	else
	{
		if ( interactingActor->GetTypeName() == "CPlayer" )
		{
			((CAfterPlayer*)interactingActor)->OpenInventoryForSwapping(this->inventory);
		}
	}
}

void CNpcBase::OnTalkTo ( CCharacter* talkingCharacter )
{
	// TODO: Don't talk if angry at target or busy as fuck.
	// Put talker code here
	CAfterPlayer* player = (CAfterPlayer*)(talkingCharacter); // todo: cast this safely. not always going to be the player that is talking
	std::vector<CCharacter*> temp;
	temp.push_back ((CCharacter*)(this));
	player->StartConversation("dialogue/test2_2.txt", temp); //please don't name variables that make the code hard to read. "Awkward_Guy" is a fucking dumb name.

	//SetPartyHost( 1024 );
	//m_worldstate.partyHost = 1024; // Join player's party (1024 is companion, but companion is companion, so 1024 is player party)
	// other possible partHost info could be that if partyHost == npc_id, then player party.
}

// SpeakDialogue makes this character speak dialogue. It takes either a raw path or a sound file
#include "after/states/model/CLipsyncSequence.h"
#include "engine/audio/CAudioInterface.h"
#include "audio/CAudioSource.h"
#include "engine/audio/CSoundBehavior.h"
#include "core-ext/system/io/Resources.h"
void CNpcBase::SpeakDialogue ( const string& soundFile )
{
	Real length;
	string playedFile;
	if ( soundFile.find(".mp3") == string::npos && soundFile.find(".ogg") == string::npos && soundFile.find(".wav") == string::npos )
	{	// Load in a sound, set lipsync with the feedback sound file name
		CSoundBehavior* behavior = Audio.playSound( soundFile.c_str() );
		if ( !behavior ) throw std::exception("Could not play sound");
		playedFile = behavior->filename;
		length = (Real) behavior->mySource->GetSoundLength();

		behavior->position = Vector3d( 0,0,5 );
		behavior->parent = &transform;
		behavior->RemoveReference();
	}
	else
	{	// Load in the raw sound.
		playedFile = Core::Resources::PathTo( "sounds/" + soundFile );
		CAudioSource* t_source = Audio.PlayWaveFile( playedFile );
		if ( !t_source ) throw std::exception("Could not find file");
		length = (Real) t_source->GetSoundLength();
		CSoundBehavior* behavior = new CSoundBehavior;
		behavior->mySource			= t_source;
		behavior->ai_alert_amount	= AudioStructs::AI_SPEECH;
		behavior->channel			= AudioStructs::CHAN_SPEECH;
		behavior->deleteWhenDone	= true;

		behavior->position = Vector3d( 0,0,5 );
		behavior->parent = &transform;
		behavior->RemoveReference();
	}

	string morphFile = playedFile.substr( 0, playedFile.find_last_of('.') );
	morphFile += ".mph";
	CLipsyncSequence* syncer = ((CMccCharacterModel*)pCharModel)->GetLipSyncer();
	syncer->Load( morphFile );
	syncer->m_position = 0;
	syncer->m_sequence_length = length;
	((CMccCharacterModel*)pCharModel)->SetLipsyncPlay();
}
// PerformActionList performs the actions given in the list string. See dialogue documentation.
void CNpcBase::PerformActionList ( const string& actionList ) 
{
	// Nothing for now
}
// PerformExpressionList performs the expressions given in the list string. See dialogue documentation.
void CNpcBase::PerformExpressionList ( const string& expressionList ) 
{
	animator.PerformExpressionList( expressionList.c_str() );
}


bool CNpcBase::PerformAttack ( void ) 
{
	CWeaponItem* currentEquipped = inventory->GetCurrentEquipped();
	if ( currentEquipped )
	{
		if ( currentEquipped->CanUse(0) )
		{
			// Should change attack direction
			m_aiming_angle = Random.Range( -180, 180 );

			input->SimulatePress( input->InputPrimary );
			return true;
		}
	}
	return false;
}
bool CNpcBase::PerformDefend ( void ) 
{
	CWeaponItem* currentEquipped = inventory->GetCurrentEquipped();
	if ( currentEquipped )
	{
		if ( currentEquipped->CanUse(Item::UDefend) )
		{
			// Should change attack direction
			m_aiming_angle += Random.Range( -15, 15 ) * Time::deltaTime;
			
			// Should change defend direction
			input->SimulatePress( input->InputDefend );
			return true;
		}
	}
	return false;
}

// Get if character is allied
bool CNpcBase::IsAlliedToCharacter ( CCharacter* character )
{ 
	if ( character->GetTypeName() == "CPlayer" )
	{	// If companion to player, return allied
		if ( characterId == 1024 ) {
			return true;
		}
	}
	else if ( character->GetTypeName() == "CNpcBase" )
	{	// If in same party, return allied
		if ( GetPartyHost() == ((CNpcBase*)character)->GetPartyHost() ) {
			return true;
		}
	}

	return false;
}

// Get if character is enemy (use faction system)
bool CNpcBase::IsEnemyToCharacter ( CCharacter* character )
{
	NPC::eCharacterFaction targetFaction = FactionInvalid;
	if ( character->GetTypeName() == "CPlayer" ) {
		targetFaction = NPC::FactionNone; // Player starts as no faction
	}
	else if ( character->GetTypeName() == "CNpcBase" ) {
		targetFaction = ((CNpcBase*)character)->GetFaction();
	}

	if ( targetFaction != FactionInvalid )
	{
		switch ( m_worldstate.mFaction ) {
		case FactionNone:
			return targetFaction==FactionBandit;
			break;
		case FactionBandit:
			return targetFaction==FactionNone;
			break;
		case FactionBanditRunner:
			return false;
			break;
		}
	}

	return false;
}

CCharacter*	CNpcBase::GetCombatTarget ( void )
{
	return ai.QueryAggroTarget();
}

void CNpcBase::UpdateActive ( void )
{
	// Copy current focus from AI to the world info to save
	// (should move to unload)
	ai.GetFocus( characterFile.worldstate->mFocus, characterFile.worldstate->mFocusName );

	// Now update combat state
	combatInfo.Update();

	if ( !pRigidBody )
	{
		pRigidBody = new CRigidBody ( pCollision, this, 30 );
		pRigidBody->SetRotationEnabled( false );
		pRigidBody->SetQualityType( HK_COLLIDABLE_QUALITY_CHARACTER );
		pRigidBody->SetRestitution( 0.01f );
		pRigidBody->SetCollisionLayer( Layers::PHYS_SWEPTCOLLISION, 1 );
		ai.rigidbody = pRigidBody;
	}
	else {
		//pRigidBody->SetRotation( transform.rotation.getQuaternion() ); // Set hull to face
	}

	// Work the stun timer
	if ( bIsStunned )
	{
		fStunTimer -= Time::deltaTime;
		if ( fStunTimer <= 0 ) {
			ai.RequestStunPanic();
			bIsStunned = false;
		}
	}

	ai.SetIsStunned( bIsStunned );
	ai.SetIsDead( !IsAlive() );

	// Update AI
	ai.Think();

	if ( pCharModel )
	{
		if ( IsAlive() )
		{
			pCharModel->SetSplitFacing( true );
			if ( !bIsStunned ) {
				pCharModel->SetFaceAtPosition( ai.QueryFacingPosition() );
			}
			pCharModel->SetLookAtSpeed( ai.info_lookat.headTurnSpeed, ai.info_lookat.eyeTurnSpeed );
			pCharModel->SetLookAtPosition( ai.QueryLookatPosition() );
			pCharModel->SetEyeRotation( Rotator( ai.QueryEyeGlanceAngles() ) );
	
			MvtPerform();

			// Animation motion extrapolation
			if ( pRigidBody )
			{
				// Get model offset as base
				Vector3d modelOffset = model->GetAnimationState()->GetExtrapolatedMotion();
				model->GetAnimationState()->ResetExtrapolatedMotion();
				// Add linear motions
				if ( bAlive )
				{
					for ( auto it = vLinearMotions.begin(); it != vLinearMotions.end(); )
					{
						it->w -= Time::deltaTime;
						modelOffset += Vector3d( it->x, it->y, it->z ) * Time::deltaTime;
						if ( it->w <= 0 ) {
							it = vLinearMotions.erase(it);
						}
						else {
							++it;
						}
					}
				}
				// Add linear motions to the current position
				pRigidBody->AddToPosition( model->GetModelRotation() * modelOffset );
			}
		}
		else
		{
			/*fadeOutValue -= Time::deltaTime * 0.5f;
			if ( fadeOutValue < -0.5f ) {
				fadeOutValue = -0.5f;
				DeleteObject( this );
			}*/
		}
		pCharModel->SetAnimationMotion( pRigidBody->GetVelocity() );
		// Update animator
		animator.DoMoveAnimation();

		//pCharModel->SetModelAlpha( fadeOutValue );

		if ( IsAlive() )
		{
			// Set isBusy state
			CAnimation* anim = pCharModel->GetAnimationState();
			if ( anim ) {
				ai.SetIsBusy( (*anim)["stagger"].isPlaying );
			}
		}
	}

	if ( input ) {
		input->Update();
	}

	if ( !bIsStunned )
	{
		// Player item usage
		bool usePrim = true;
		CWeaponItem* currentEquipped = inventory->GetCurrentEquipped();
		if ( currentEquipped )
		{
			if ( input->axes.primary )
			{
				if ( input->axes.primary.pressed() ) {
					currentEquipped->StartUse( bIsSprinting ? Item::UPrimarySprint : Item::UPrimary );
				}
				if ( currentEquipped->Use( bIsSprinting ? Item::UPrimarySprint : Item::UPrimary ) ) {
					usePrim = false;
				}
			}
			else if ( input->axes.primary.released() ) {
				currentEquipped->EndUse( bIsSprinting ? Item::UPrimarySprint : Item::UPrimary );
			}

			if ( input->axes.secondary )
			{
				if ( input->axes.secondary.pressed() ) {
					currentEquipped->StartUse( bIsSprinting ? Item::USecondarySprint : Item::USecondary );
				}
				if ( currentEquipped->Use( bIsSprinting ? Item::USecondarySprint : Item::USecondary ) ) {
					usePrim = false;
				}
			}
			else if ( input->axes.secondary.released() ) {
				currentEquipped->EndUse( bIsSprinting ? Item::USecondarySprint : Item::USecondary );
			}

			if ( input->axes.tertiary )
			{
				if ( input->axes.tertiary.pressed() ) {
					currentEquipped->StartUse( bIsSprinting ? Item::UOptionalSprint : Item::UTertiary );
				}
				if ( currentEquipped->Use( bIsSprinting ? Item::UOptionalSprint : Item::UTertiary ) ) {
					usePrim = false;
				}
			}
			else if ( input->axes.tertiary.released() ) {
				currentEquipped->EndUse( bIsSprinting ? Item::UOptionalSprint : Item::UTertiary );
			}

			if ( input->axes.defend )
			{
				if ( input->axes.defend.pressed() ) {
					currentEquipped->StartUse( bIsSprinting ? Item::UDefendSprint : Item::UDefend );
				}
				if ( currentEquipped->Use( bIsSprinting ? Item::UDefendSprint : Item::UDefend ) ) {
					usePrim = false;
				}
			}
			else if ( input->axes.defend.released() ) {
				currentEquipped->EndUse( bIsSprinting ? Item::UDefendSprint : Item::UDefend );
			}
		}
	}

	// Over here, for now, do the race stats
	*m_stats = *base_race_stats;
	{
		m_stats->fRunSpeed *= fMovementMultiplier;
		m_stats->fSprintSpeed *= fMovementMultiplier;
		m_stats->fCrouchSpeed *= fMovementMultiplier;
		m_stats->fProneSpeed *= fMovementMultiplier;
		m_stats->fSwimSpeed *= fMovementMultiplier;
	}

	MoveUnstuck();
}

// Inactive update (character is out of range)
void CNpcBase::UpdateInactive ( void )
{
	// Since we're out of range, delete our rigidbody.
	if ( pRigidBody )
	{
		delete pRigidBody;
		pRigidBody = NULL;
		ai.rigidbody = NULL;
	}
}

//void CNpcBase::PlayItemAnimation ( const string& sActionName, const float fArg )
void CNpcBase::PlayItemAnimation ( const NPC::eItemAnimType nActionName, const int nSubset, const int nHand, const float fArg, const float fAnimSpeed, const float fAttackSkip )
{
	//animator.PlayItemAnimation( sActionName, fArg );
	animator.PlayItemAnimation( nActionName, nSubset, nHand, fArg, fAnimSpeed, fAttackSkip );
}
Real CNpcBase::GetItemAnimationLength ( const NPC::eItemAnimType nActionName, const int nSubset, const int nHand ) 
{
	return animator.GetItemAnimationLength( nActionName, nSubset, nHand );
}
void CNpcBase::PlayAnimation ( const string& sActionName )
{
	std::cout << "NO ANIMATION FOR " << sActionName << " IS AVAILABLE" << std::endl;
}

// Get aiming arc. Used for melee weapons.
Vector4d CNpcBase::GetAimingArc ( void )
{
	// Returns a Vector4d indicating the following aiming properties:
	// X is the width of the arc in degrees.
	// Y is the vertical rotation offset. (non-zero means not aiming down center of screen).
	// Z is the depth rotation.
	// W is ~0 for zero depth, ~1 for deep depth
	return Vector4d( 40,0,m_aiming_angle,0 );
}


// Returns a pointer to the shield. Returns NULL if no shield.
SkillShield*	CNpcBase::GetShield ( void ) 
{
	return m_shield;
}
void	CNpcBase::SetShield ( SkillShield* n_shield )
{
	m_shield = n_shield;
}
// Returns a list of all the casting spells in a vector. Default implemenation returns a list of empty skills.
std::vector<CSkill*>	CNpcBase::GetCastingSkills ( void ) 
{
	std::vector<CSkill*> skills;
	// Loop through all the hands and get the skills
	short handCount = inventory->GetCurrentEquippedCount();
	for ( short i = 0; i < handCount; ++i ) 
	{
		CWeaponItem* t_itemInHand = inventory->GetCurrentEquipped(i);
		// Check if equipped is a skill
		if ( t_itemInHand->GetIsSkill() ) {
			skills.push_back( (CSkill*)t_itemInHand );
		}
	}
	return skills;
}

void CNpcBase::OnDealDamage( Damage& damage, CActor* receivingCharacter )
{
	if ( damage.amount > 0 )
	{
		if ( ai.response_atk.atk_waitForResult )
		{
			ai.response_atk.atk_waitForResult = false;
			ai.response_atk.atk_hit = true;
		}
	}
}

void CNpcBase::OnDamaged ( Damage const& hitDamage, DamageFeedback* dmgFeedback )
{
	// Do AI stun
	/*CAnimation* anim = pCharModel->GetAnimationState();
	CAnimAction* stagger;
	if ( stagger = anim->FindAction( "stagger" ) ) {
		if ( !stagger->isPlaying ) {
			anim->Play( "stagger" );
			
			ai.ai_think.isStunned = true;
			ai.ai_think.stunTimer = (stagger->GetLength() / 25.0f);
		}
	}*/

	ai.OnGetDamaged( hitDamage );

	//cout << "Komodo took " << hitDamage.amount << " damage from " << (ftype)charHealth;
	//CCharacter::OnDamaged( hitDamage );
	//cout << " to " << (ftype)charHealth << endl;

	// Run the damage first through the buffs system
	Damage finalDamage = debuffs.OnDamaged( hitDamage );
	// Now run it through the shield
	if ( GetShield() ) {
		finalDamage = GetShield()->OnDamaged( finalDamage, dmgFeedback );
	}

	// After running it through the buffs, check for the reflect damage type
	if ( finalDamage.type & DamageType::Reflect ) {
		if ( finalDamage.amount > 80.0f )	// Reduce damage if above 80%
			finalDamage.amount = finalDamage.amount * 0.5f + 40.0f;
		// Reflect damage must be turned into percentage amounts
		finalDamage.amount = (finalDamage.amount/100.0f) * stats.fHealth;	// Subtract a percentage of the CURRENT health.
	}

	// ==Defense Calculations==
	int defenseCount = 0;
	// For physical damage
	if ( ((finalDamage.type & DamageType::Physical) || !(finalDamage.type & DamageType::Magical)) && (!(finalDamage.type & DamageType::Reflect)) )
	{
		bool canBlock = true;
		if ( bIsStunned ) {
			canBlock = false;
		}
		else {
			Matrix4x4 rotMatrix;
			rotMatrix.setRotation( GetAimRotator() );
			if ( finalDamage.direction.dot( rotMatrix*Vector3d::forward ) > -0.1f ) {
				canBlock = false;
			}
		}
		std::cout << "Blocking: " << canBlock << std::endl;
		if ( canBlock )
		{
			// Run the damage through the blocking calculations (get stances)
			for ( ushort i = 0; i < m_stats->iWieldCount; ++i )
			{
				CWeaponItem* currentEquipped = inventory->GetCurrentEquipped(i);
				if ( currentEquipped && currentEquipped->GetStance() == Item::StanceDefense ) {
					defenseCount += 1;
				}
			}
			std::cout << "NPC defense count: " << defenseCount << std::endl;
			// If definitely defending...
			if ( defenseCount >= 1 )
			{
				// Run the damage through the weapons first
				for ( ushort i = 0; i < m_stats->iWieldCount; ++i )
				{
					CWeaponItem* currentEquipped = inventory->GetCurrentEquipped(i);
					if ( currentEquipped ) {
						currentEquipped->OnBlockAttack( finalDamage ); // If parrying, this will set damage to zero.
					}
				}
				// If still dealing damage...
				if ( finalDamage.amount > FTYPE_PRECISION )
				{
					// There are 3 types of blocks based on how successful the block was. Figure out what type of block is being used.
					Vector4d tAimingArc = GetAimingArc();
					int		blockType;
					Vector3d swordVector = Vector3d( (ftype)cos(degtorad(tAimingArc.z)),(ftype)sin(degtorad(tAimingArc.z)),tAimingArc.w ).normal();
					ftype	blockAngle = finalDamage.applyDirection.dot( swordVector );
					if ( fabs( blockAngle ) < 0.5f ) {
						blockType = 1; // successful block (30 degrees)
					}
					else if ( fabs( blockAngle ) < 0.87f ) {
						blockType = 2; // okay block (60 degrees)
					}
					else {
						blockType = 3; // bad block (90 degrees or greater)
					}
					std::cout << "Blocked damage at angle: " << blockAngle << std::endl;
					std::cout << "Using blocktype: " << blockType << std::endl;
					// Punch view
					//PunchView( Vector3d( Random.PointOnUnitCircle() ) * sqr(finalDamage.amount*0.32f) );
					// Do blocking modifiers
					switch ( blockType )
					{
					case 1:
						stats.fStamina -= finalDamage.amount * 0.1f;
						finalDamage.amount = 0;
						if ( stats.fStamina <= 1 )
							finalDamage.stagger_chance += 0.2f;
						else 
							finalDamage.stagger_chance *= 0.1f;
						// Perform a ministun on the source
						if ( finalDamage.actor && finalDamage.actor->IsCharacter() ) {
							std::cout << "...npc excellent block reflect..." << std::endl;
							((CCharacter*)finalDamage.actor)->ApplyStun("stun_stagger_twist",1.0);
							std::cout << "npc excellent block reflect done" << std::endl;
							((CCharacter*)finalDamage.actor)->ApplyLinearMotion( Vector3d( -7.0f,0,0 ), 0.1f );
						}
						// Don't move back too much
						ApplyLinearMotion( Vector3d( -1.0f,0,0 ), 0.1f );
						// Set feedback damage
						if ( dmgFeedback ) {
							dmgFeedback->material = PhysMats::Get(PhysMats::MAT_Wood);
						}
						break;
					case 2:
						stats.fStamina -= finalDamage.amount * 0.2f;
						finalDamage.amount *= 0.15f;
						if ( stats.fStamina <= 1 )
							finalDamage.stagger_chance += 0.5f;
						else 
							finalDamage.stagger_chance *= 0.3f;
						// Perform a ministun regardless (not a perfect block)
						if ( StunWithAnimation( "stun_stagger_twist", 1.2f ) ) {
							//fScreenBlurAmount += 0.2f;
						}
						// Don't move back too much
						ApplyLinearMotion( Vector3d( -1.5f,0,0 ), 0.2f );
						// Perform a ministun on the source
						if ( finalDamage.actor && finalDamage.actor->IsCharacter() ) {
							std::cout << "...npc good block reflect..." << std::endl;
							((CCharacter*)finalDamage.actor)->ApplyStun("hold_oneblade_01_ministun-0-4",1.0);
							std::cout << "npc good block reflect done" << std::endl;
						}
						// Set feedback damage
						if ( dmgFeedback ) {
							dmgFeedback->material = PhysMats::Get(PhysMats::MAT_Wood);
						}
						break;
					case 3:
						stats.fStamina -= finalDamage.amount * 0.3f;
						finalDamage.amount *= 0.25f;
						if ( stats.fStamina <= 1 ) 
							finalDamage.stagger_chance += 0.75f;
						else 
							finalDamage.stagger_chance *= 0.3f;
						// Perform a ministun on self for fucking up block
						if ( StunWithAnimation( "stun_stagger_twist" ) ) {
							//fScreenBlurAmount += 0.5f;
						}
						// Fall back a full foot
						ApplyLinearMotion( Vector3d( -2.0f,0,0 ), 0.3f );
						break;
					}
				}
				else
				{	// If not dealing damage, means that parried. Deal pure stun to the damage source
					if ( finalDamage.actor ) {
						std::cout << "...npc parry reflect..." << std::endl;
						Damage stunDamage;
						stunDamage.type = DamageType::Reflect;
						stunDamage.amount = 0;
						stunDamage.stagger_chance = 999999;
						finalDamage.actor->OnDamaged( stunDamage );
						std::cout << "npc parry reflect done" << std::endl;
						if ( dmgFeedback ) { // Don't do damage effect when parried
							dmgFeedback->do_effect = false;
						}
					}
				}
				// End defend code
			}
		}
		// End melee defend code
	}
	else if ( finalDamage.type & DamageType::Magical )
	{
		// Do magic defense
	}

	// Perform damage calculations
	stats.fHealth -= finalDamage.amount;
	// Knock the view
	if ( !(finalDamage.type & DamageType::Fall) || !(finalDamage.type & DamageType::Burn) ) {
		//PunchView( Vector3d( Random.PointOnUnitCircle() ) * sqr(finalDamage.amount*0.24f) );
	}
	// Cut the model
	if ( finalDamage.type & DamageType::Slash ) {
		//
		//model->tats_cuts
		CMccCharacterModel* t_charmodel = (CMccCharacterModel*)pCharModel;
		//t_charmodel->AddDecal( hitDamage.source+Vector3d( -hitDamage.direction.x, -hitDamage.direction.y, hitDamage.direction.z*2 ), DamageType::Slash );
		t_charmodel->AddDecal( hitDamage.source, DamageType::Slash );
		t_charmodel->UpdateTattoos();
	}
	// Slightly blur the screen
	//fScreenBlurAmount += (finalDamage.amount/charHealth.GetMax())*1.2f;
	// Knock the breath out when falling
	if ( finalDamage.type & DamageType::Fall ) {
		stats.fStamina -= finalDamage.amount * 0.25f;
	}

	// Remove stun chances from certain type of damage
	if ( finalDamage.type & DamageType::Fall || finalDamage.type & DamageType::Burn ) {
		finalDamage.stagger_chance = 0;
	}

	// If this damage killed, set kill damage
	if ( stats.fHealth <= 0 ) {
		killDamage = hitDamage;
	}
	else
	{
		if ( Random.Chance(finalDamage.stagger_chance) )
		{
			// Do AI stun
			if ( StunWithAnimation( "stun_stagger_hard" ) ) {
				//fScreenBlurAmount += 0.2f;
			}
			//
		}
		// Stun the player otherwise (if high enough damage)
		else if ( finalDamage.amount > stats.fHealthMax*0.3f )
		{
			if ( finalDamage.type & DamageType::Fall )
			{
				if ( !(finalDamage.type & DamageType::Drown) )
				{
					//if ( (forwardAmount > 0) || (movementSpeed < 4) ) {
						// Do fall hard stun
						if ( StunWithAnimation( "fall_hard" ) ) {
							// Blur the screen hardcore
							//fScreenBlurAmount += 0.6f;
						}
					//}
					//else {
						// Do fall backwards
						//m_queuedMoveType	= (stateFunc_t)&CMccPlayer::mvt_FellOnBackStart;
						// Blur the screen hardcore
						//fScreenBlurAmount += 0.6f;
					//}
				}
				else
				{
					// Do fall hard stun
					StunWithAnimation( "fall_hard" );
					//fScreenBlurAmount += 0.7f;
					model->BlendToRagdoll( 0 );
				}
			}
		}
	}
}

// Sets the movement scaling.
void CNpcBase::SetMovementSpeedScale ( float nSpeedScale )
{
	fMovementMultiplier = nSpeedScale;
}


// Apply stun. Returns true on successful stun. Ignores the second argument.
bool CNpcBase::ApplyStun ( const char* n_stunname, const ftype n_stunlength )
{
	return StunWithAnimation( n_stunname );
}

//  StunWithAnimation()
// Stuns the player with the animation. Will stun for entire duration of animation.
// Returns true when animation is found.
bool	CNpcBase::StunWithAnimation ( const string& nStunName, const ftype nTimescale )
{
	bool hasStun = false;
	// Do AI stun
	//
	CAnimation* anim = model->GetAnimationState();
	CAnimAction* stagger;
	if ( stagger = anim->FindAction( nStunName ) ) {
		if ( !stagger->isPlaying ) {
			hasStun = true;

			anim->Play( nStunName );

			stagger->framesPerSecond = 30.0f * nTimescale;

			bIsStunned = true;
			fStunTimer = (stagger->GetLength() / stagger->framesPerSecond);
		}
	}
	//
	if ( hasStun ) {
		//m_queuedMoveType	= (stateFunc_t)&CMccPlayer::mvt_Stunned;	
	}
	return hasStun;
}

bool CNpcBase::ApplyLinearMotion ( const Vector3d& n_motion, const ftype n_motionlength ) 
{ 
	if ( IsAlive() )
	{
		vLinearMotions.push_back( Vector4d( n_motion.y, -n_motion.x, n_motion.z, n_motionlength ) );
		return true;
	}
	return false;
}


void CNpcBase::OnDeath ( Damage const&  killDamage )
{
	ai.SetIsDead( true );

	/*ai.ai_think.isAlerted	= false;
	ai.ai_think.isAngry		= false;
	ai.ai_think.isCautious	= false;*/
	ai.RequestInfoState( NPC::ai_think_state_t::AI_RELAXED );

	ai.SetIsBusy( true );

	ai.GetMoveFeedback().isRunning	= false;
	ai.GetMoveFeedback().isWalking	= false;

	{
		string deadAnim = "die_forward";
		if ( Random.Next()%2 == 0 ) {
			deadAnim = "die_backward";
		}
		CAnimation* anim = pCharModel->GetAnimationState();
		CAnimAction* death;
		if ( death = anim->FindAction( deadAnim ) ) {
			death->end_behavior = 1;
			death->layer = 6;
			anim->Play( deadAnim );
		}
	}
	{
		pCharModel->BlendToRagdoll( Random.Range( 0.6f, 2.6f ) );
	}
	{
		// Also drop equipped items
		for ( ushort i = 0; i < m_stats->iWieldCount; ++i )
		{
			CWeaponItem* currentEquipped = inventory->GetCurrentEquipped(i);
			if ( currentEquipped ) {
				currentEquipped->SetOwner( NULL );
			}
		}

		// Update inventory owning
		inventory->Update();
	}

	ZCC_DisableSave(); // disable saving
	NPC::Manager->FreeNPCID( GetNPCID() ); // remove off disk
}



// ===Transform Getters===
XTransform	CNpcBase::GetHoldTransform ( char i_handIndex )
{
	/*Matrix4x4 resultT, resultR;
	Matrix4x4 rotMatrix, rotMatrix2;

	resultR.setRotation( -pCamera->transform.rotation.getEulerAngles() );

	rotMatrix.setRotation( pCamera->transform.rotation );
	rotMatrix2.setRotation( Vector3d( 0,-27,29 ) );
	resultT.setTranslation( pCamera->transform.position + (rotMatrix*(rotMatrix2*Vector3d(1.0f,0,0))) );

	return resultT*resultR;*/

	/*Matrix4x4 resultT, resultR;
	XTransform prop01Transform;

	model->GetProp01Transform( prop01Transform );

	resultT.setTranslation( prop01Transform.position );
	resultR.setRotation( prop01Transform.rotation );

	return resultT*resultR;*/

	XTransform prop01Transform;

	if ( i_handIndex == 0 ) {
		model->GetProp01Transform( prop01Transform );
	}
	else if ( i_handIndex == 1 ) {
		model->GetProp02Transform( prop01Transform );
	}
	else {
		model->GetEyecamTransform( prop01Transform );
	}

	return prop01Transform;
}
XTransform	CNpcBase::GetBeltTransform ( char i_beltIndex )
{
	/*Matrix4x4 resultT, resultR;
	resultR.setRotation( !transform.rotation );
	resultT.setTranslation( transform.position + transform.Forward() + Vector3d( 0,0,fPlayerHeight*0.5f ) );
	return resultT*resultR;*/
	XTransform result;
	result.rotation = transform.rotation.getQuaternion();
	result.position = transform.position + transform.Forward() + Vector3d( 0,0,m_stats->fStandingHeight*0.5f );
	return result;
}

// Get aiming direction. Used for melee weapons.
Rotator		CNpcBase::GetAimRotator ( void )
{
	/*XTransform transCamPos;
	model->GetEyecamTransform( transCamPos );
	return Quaternion::CreateRotationTo( Vector3d::forward, transCamPos.rotation * Vector3d::left );*/
	Quaternion rot;
	Ray eye = GetEyeRay();
	rot.RotationTo( Vector3d::forward, (ai.QueryLookatPosition()-eye.pos).normal() );
	Rotator result ( rot );
	return Rotator( rot );
}


// == Animation Events ==
#include "engine-common/entities/effects/CFXMaterialHit.h"

// == Character State Getters ==
// Is this character performing a melee attack? The input to the function is the hand to check.
// If the hand is being used with a melee attack, then return true.
bool CNpcBase::IsAttackingMelee ( const short hand )
{
	int frame = model->GetMeleeAttackFrame( hand );

	return frame>=0;
}
// Get the frame of the melee attack that the character is attacking. This is used for determining attack cancelling during melee attacks.
// Attack cancelling is used for both combos and for sword clashing.
int	CNpcBase::GetAttackingFrame ( const short hand )
{
	return model->GetMeleeAttackFrame( hand );
}

void CNpcBase::OnAnimationEvent ( const Animation::eAnimSystemEvent eventType, const Animation::tag_t tag )
{
	/*if ( eventType == Animation::Event_Attack ) {
		CWeaponItem* currentEquipped = inventory->GetCurrentEquipped();
		if ( currentEquipped ) {
			currentEquipped->Attack();	// Call attack event
		}
	}
	else if ( eventType == Animation::Event_Footstep ) {
		//Audio.playSound( "Char.Footstep" )->position = transform.position;
		Ray castRay;
		castRay.pos = transform.position + Vector3d( 0,0,1.8f );
		castRay.dir = Vector3d( 0,0,-1 );
		RaycastHit rhLastHit;
		if ( Raycaster.Raycast( castRay, 2.4f, &rhLastHit, Physics::GetCollisionFilter(Layers::PHYS_BULLET_TRACE,0,31) ) )
		{
			// Do material effects
			CFXMaterialHit* newHitEffect = new CFXMaterialHit(
				Terrain::MaterialOf( Raycaster.HitBlock().block.block ),
				rhLastHit, CFXMaterialHit::HT_STEP );
			newHitEffect->RemoveReference();
		}
	}*/
	switch ( eventType )
	{
	case Animation::Event_Attack:
		{
			CWeaponItem* currentEquipped = inventory->GetCurrentEquipped();
			if ( currentEquipped ) {
				XTransform t_targetTransform ( currentEquipped->transform.position, currentEquipped->transform.rotation );
				if ( tag != Animation::EventTag_NoTag && tag != Animation::EventTag_Default ) {
					model->GetBoneTransform( tag, t_targetTransform );
				}
				currentEquipped->Attack(t_targetTransform);	// Call attack event
			}
		}
		break;
	case Animation::Event_ClangCheck:
		{
			// Run CLANG CHECK on the weapon
			CWeaponItem* currentEquipped = inventory->GetCurrentEquipped();
			if ( currentEquipped ) {
				if ( currentEquipped->GetBaseClassName() == "RandomBaseMelee" ) {
					((CBaseRandomMelee*)(currentEquipped))->ClangCheck();
				}
			}
		}
		break;
	case Animation::Event_Footstep:
		{
			//Audio.playSound( "Char.Footstep" )->position = transform.position;
			Ray castRay;
			castRay.pos = transform.position + Vector3d( 0,0,1.8f );
			castRay.dir = Vector3d( 0,0,-1 );
			RaycastHit rhLastHit;
			if ( Raycaster.Raycast( castRay, 2.4f, &rhLastHit, Physics::GetCollisionFilter(Layers::PHYS_BULLET_TRACE,0,31) ) )
			{
				// Do material effects
				CFXMaterialHit* newHitEffect = new CFXMaterialHit(
					//Terrain::MaterialOf( Raycaster.HitBlock().block.block ),
					Terrain::MaterialOf( TerrainAccess.GetBlockAtPosition(rhLastHit).block ),
					rhLastHit, CFXMaterialHit::HT_STEP );
				newHitEffect->RemoveReference();
			}
		}
		break;
	}
}


void	CNpcBase::OnEquip ( CItemBase* pItem )
{
	if ( pItem == NULL ) {
		return;
	}
	

	if ( pItem->GetBaseClassName() != "WearableItem" )
	{
		//pl_inventoryGUI->SetDrawName (true);
	}
	else
	{
		switch ( ((CWeaponItem*)pItem)->GetItemType() ) {
			case WeaponItem::TypeArmorBoots:
				m_stats->sGearRightfoot = ((CWearableItem*)pItem)->GetPlayerModel();
				((CMccCharacterModel*)model)->SetPartHidden( CMccCharacterModel::PART_FEET, ((CWearableItem*)pItem)->HidesBody() );
				((CMccCharacterModel*)model)->SetBootsModel( m_stats->sGearRightfoot );
				break;
			case WeaponItem::TypeArmorChest:
				m_stats->sGearChest = ((CWearableItem*)pItem)->GetPlayerModel();
				((CMccCharacterModel*)model)->SetPartHidden( CMccCharacterModel::PART_TORSO, ((CWearableItem*)pItem)->HidesBody() );
				((CMccCharacterModel*)model)->SetShirtModel( m_stats->sGearChest );
				break;
			case WeaponItem::TypeArmorGreaves:
				m_stats->sGearLegs = ((CWearableItem*)pItem)->GetPlayerModel();
				((CMccCharacterModel*)model)->SetPartHidden( CMccCharacterModel::PART_HIPS, ((CWearableItem*)pItem)->HidesBody() );
				((CMccCharacterModel*)model)->SetShortsModel( m_stats->sGearLegs );
				break;
			case WeaponItem::TypeArmorHead:
				m_stats->sGearHead = ((CWearableItem*)pItem)->GetPlayerModel();
				((CMccCharacterModel*)model)->SetPartHidden( CMccCharacterModel::PART_HEAD, ((CWearableItem*)pItem)->HidesBody() );
				((CMccCharacterModel*)model)->SetGlassesModel( m_stats->sGearHead );
				break;
			case WeaponItem::TypeArmorShoulder:
				m_stats->sGearShoulder = ((CWearableItem*)pItem)->GetPlayerModel();
				((CMccCharacterModel*)model)->SetPartHidden( CMccCharacterModel::PART_SHOULDERS, ((CWearableItem*)pItem)->HidesBody() );
				((CMccCharacterModel*)model)->SetShoulderModel( m_stats->sGearShoulder );
				((CMccCharacterModel*)model)->SetHair( ((CWearableItem*)pItem)->HidesHair() ? -1 : m_stats->iHairstyle );
				break;
			default:
				Debug::Console->PrintError( "Unhandled armor type equipped!" );
				break;
		}
	}
}
void	CNpcBase::OnUnequip ( CItemBase* pItem )
{
	if ( pItem->GetBaseClassName() != "WearableItem" )
	{
		;
	}
	else
	{
		switch ( ((CWeaponItem*)pItem)->GetItemType() ) {
			case WeaponItem::TypeArmorBoots:
				m_stats->sGearRightfoot = "";
				((CMccCharacterModel*)model)->SetPartHidden( CMccCharacterModel::PART_FEET, false );
				((CMccCharacterModel*)model)->SetBootsModel( "" );
				break;
			case WeaponItem::TypeArmorChest:
				m_stats->sGearChest = "";
				((CMccCharacterModel*)model)->SetPartHidden( CMccCharacterModel::PART_TORSO, false );
				((CMccCharacterModel*)model)->SetShirtModel( "wraps" );
				break;
			case WeaponItem::TypeArmorGreaves:
				m_stats->sGearLegs = "";
				((CMccCharacterModel*)model)->SetPartHidden( CMccCharacterModel::PART_HIPS, false );
				((CMccCharacterModel*)model)->SetShortsModel( "shorts" );
				break;
			case WeaponItem::TypeArmorHead:
				m_stats->sGearHead = "";
				((CMccCharacterModel*)model)->SetPartHidden( CMccCharacterModel::PART_HEAD, false );
				((CMccCharacterModel*)model)->SetGlassesModel( "" );
				break;
			case WeaponItem::TypeArmorShoulder:
				m_stats->sGearShoulder = "";
				((CMccCharacterModel*)model)->SetPartHidden( CMccCharacterModel::PART_SHOULDERS, false );
				((CMccCharacterModel*)model)->SetShoulderModel( "" );
				((CMccCharacterModel*)model)->SetHair( m_stats->iHairstyle );
				break;
			default:
				Debug::Console->PrintWarning( "Unhandled armor type unequipped!" );
				break;
		}
	}	
}



//#include "unused/CVoxelTerrain.h"
// Move Unstuck
void CNpcBase::MoveUnstuck ( void )
{
	return;
	/*ushort block = Zones.GetBlockAtPosition(transform.position + Vector3d(0,0,0.05f)).block;
	if (( block == Terrain::Terrain::EB_NONE )||( block == Terrain::Terrain::EB_WATER ))
	{

	}
	else
	{
		// Issues when z is negative, still
		transform.position.z += 2 - (transform.position.z - floor( transform.position.z / 2 )*2.0f) + 0.02f;
		if ( pRigidBody ) {
			transform.SetDirty();
			pRigidBody->SetVelocity( pRigidBody->GetVelocity() * 0.5f );
		}
	}*/
	// Raytrace downward for the check
	Ray			downCheckRayLower ( transform.position + Vector3d(0,0,0.05f), -Vector3d::up );
	RaycastHit	downCheckHitInfoLower;
	Raycaster.Raycast( downCheckRayLower, 2.0f, &downCheckHitInfoLower,  Physics::GetCollisionFilter(Layers::PHYS_WORLD_TRACE) );
	Ray			downCheckRayUpper ( transform.position + Vector3d(0,0,2.05f), -Vector3d::up );
	RaycastHit	downCheckHitInfoUpper;
	Raycaster.Raycast( downCheckRayUpper, 4.0f, &downCheckHitInfoUpper,  Physics::GetCollisionFilter(Layers::PHYS_WORLD_TRACE) );

	Vector3d samplePositionUpper = transform.position + Vector3d(0,0,0.05f);
	Terrain::terra_b blockUpper = Zones.GetBlockAtPosition(samplePositionUpper);
	Vector3d samplePositionLower = transform.position + Vector3d(0,0,-1.95f);
	Terrain::terra_b blockLower = Zones.GetBlockAtPosition(samplePositionLower);

	if (( blockLower.block == Terrain::EB_NONE )||( blockLower.block == Terrain::EB_WATER ))
	{

	}
	else
	{
		// Calculate deltaZ to the ground.
		float deltaZ = -0.1f;
		if ( !downCheckHitInfoLower.hit && downCheckHitInfoUpper.hit )
		{
			if (( blockUpper.block == Terrain::EB_NONE )||( blockUpper.block == Terrain::EB_WATER ))
			{
				// Guess the current Z position
				float deltaZ = (floor(samplePositionLower.z/2)*2.0f) + (2*Terrain::_depth_bias(blockLower.normal_z_w)) + 0.05f;
				// Target delta = Target - Current
				deltaZ = deltaZ - transform.position.z;
			}
			else
			{	// In solid block, move up
				deltaZ = 2;
			}
		}
		else if ( !downCheckHitInfoLower.hit && !downCheckHitInfoUpper.hit )
		{
			// Both traces missed
			if (( blockUpper.block == Terrain::EB_NONE )||( blockUpper.block == Terrain::EB_WATER ))
			{
				// Nothing
			}
			{	// In solid block, move up
				deltaZ = 2;
			}
		}
		// If positive movement, move the player upwards
		if ( deltaZ > 0.0 )
		{
			transform.position.z += deltaZ;

			if ( pRigidBody ) { // Set dirty body and slow movement
				transform.SetDirty();
				pRigidBody->SetVelocity( pRigidBody->GetVelocity() * 0.5f );
			}
		}
	}
}

/*
Ray	CNpcBase::GetEyeRay ( void )
{
	Ray newRay;
	XTransform transCamPos;
	pCharModel->GetEyecamTransform( transCamPos );

	newRay.pos = transCamPos.position;
	//newRay.dir = transCamPos.rotation * Vector3d::left;
	newRay.dir = (vCurrentFacingPos-transCamPos.position).normal();

	return newRay;
};*/

Ray		CNpcBase::GetEyeRay ( void )
{
	if ( !pCharModel ) {
		return CZonedCharacter::GetEyeRay();
	}

	Ray newRay;
	XTransform transCamPos;
	pCharModel->GetEyecamTransform( transCamPos );

	newRay.pos = transCamPos.position;
	newRay.dir = (ai.QueryFacingPosition()-transCamPos.position).normal();

	return newRay;
}


void CNpcBase::MvtInit ( void )
{
	bIsProne		= false;
	bIsCrouching	= false;
	bCanUncrouch	= false;
	bCanUnprone		= false;
	bWantsCrouch	= false;
	bWantsProne		= false;
	bIsSprinting	= false;
	bCanSprint		= false;

	fSlideCounter	= 0.0f;

	fCurrentHeight = m_stats->fStandingHeight;
}

void CNpcBase::MvtCommonCrouching ( void ) //Common movement routine, such as crouching
{
	auto ai_moves = ai.GetMoveFeedback();

	if ( fCurrentHeight < 1.0f )
		fCurrentHeight = m_stats->fStandingHeight;
	if ( ai_moves.wantsCrouch ) //if ( fInput[iCrouch] )
		bWantsCrouch = true;
	else
		bWantsCrouch = false;

	if ( ai_moves.wantsProne ) //if ( fInput[iProne] )
		bWantsProne = !bWantsProne;
	if ( bWantsCrouch )
		bWantsProne = false;
	/*float groundDistance = 0.0f;
	bool onGround = OnGround( groundDistance );
	if ( !onGround )
		bWantsProne = false;*/

	// Raytrace upwards to check if can uncrouch
	RaycastHit upHitResult;
	Ray ray;
	ray.pos = transform.position + Vector3d( 0,0,1 );
	ray.dir = Vector3d(0,0,1);
	Raycaster.Raycast( ray, 6.0f, &upHitResult, Physics::GetCollisionFilter(Layers::PHYS_WORLD_TRACE) ); 
	if ( upHitResult.hit )
	{
		if ( upHitResult.distance < (m_stats->fCrouchingHeight-1) )
		{
			bCanUncrouch = false;
			bCanUnprone = false;
		}
		else if ( upHitResult.distance < (m_stats->fStandingHeight-1) )
		{
			bCanUncrouch = false;
			bCanUnprone = true;
		}
		else
		{
			bCanUncrouch = true;
			bCanUnprone = true;
		}
	}
	else
	{
		bCanUncrouch = true;
		bCanUnprone = true;
	}

	// Set the height
	if (( bWantsProne )||( !bCanUnprone ))
	{
		bIsProne = true;
		bIsCrouching = false;
		MvtSetPlayerHeightStick( m_stats->fProneHeight );
	} 
	else if (( bWantsCrouch )||( !bCanUncrouch ))
	{
		bIsProne = false;
		bIsCrouching = true;
		//if ( bOnGround )
		MvtSetPlayerHeightStick( m_stats->fCrouchingHeight );
		//else
		//	MvtSetPlayerHeight( fCrouchPlayerHeight );
	}
	else
	{
		bIsProne = false;
		bIsCrouching = false;
		MvtSetPlayerHeightStick( m_stats->fStandingHeight );
	}

	// Check if just started crouching
	/*bStartedCrouching = false;
	if ( bIsCrouching )
		if ( input->axes.crouch.PreviousValue < 0.5f )//	if ( fInputPrev[iCrouch] < 0.5f )
			bStartedCrouching = true;*/
	//
}

// Set player's height. This doesn't do any duck-jump functionality
void CNpcBase::MvtSetPlayerHeightStick ( float fnPlayerHeight )
{
	// Check for a default player size value
	if ( fnPlayerHeight <= 0 ) {
		fnPlayerHeight = m_stats->fStandingHeight;
	}

	// Calculate delta amount
	//float deltaHeight = (fnPlayerHeight-fPlayerHeight)*Time::TrainerFactor( 0.4f );
	float deltaHeight;
	deltaHeight = (((fnPlayerHeight-fCurrentHeight)<0) ? -12.0f : 12.0f ) * Time::deltaTime;

	// Check the ceiling first
	if ( fabs(fnPlayerHeight-fCurrentHeight) > fabs(deltaHeight) )
	{
		if ( deltaHeight > 0 ) {
			RaycastHit upHitResult;
			Ray ray;
			ray.pos = transform.position + Vector3d( 0,0,fCurrentHeight );
			ray.dir = Vector3d( 0,0,1 );
			if ( Raycaster.Raycast( ray, deltaHeight*2.0f, &upHitResult, Physics::GetCollisionFilter(Layers::PHYS_WORLD_TRACE) ) ) {
				pRigidBody->AddToPosition( Vector3d( 0,0,-deltaHeight ) );
			}
		}
	}

	// Change the player size
	if ( fabs(fnPlayerHeight-fCurrentHeight) < fabs(deltaHeight) ) {
		deltaHeight = fnPlayerHeight-fCurrentHeight;
	}
	fCurrentHeight += deltaHeight;

	// Set the height
	pCollision->SetHeight( fCurrentHeight );
}

// Do actual movement
void CNpcBase::MvtPerform ( void )
{
	MvtCommonCrouching();

	auto ai_moves = ai.GetMoveFeedback();

	bCanSprint = true;
	// Disallow sprint if no stamina is left
	if ( stats.fStamina < 1.0f )
		bCanSprint = false;

	// TODO moveto movetype
	if (( bCanSprint )&&( ai_moves.isRunning )&&( !bIsCrouching || m_stats->bCanCrouchSprint )) //if (( bCanSprint )&&( fInput[iSprint] ))
	{
		bIsSprinting = true;
	}
	else {
		bIsSprinting = false;
	}

	if ( true )
	{
		Vector3d moveVector(0,0,0);
		if ( ai_moves.isRunning || ai_moves.isWalking ) {
			moveVector = ai_moves.final_target - transform.position;
			if ( moveVector.sqrMagnitude() < 0.1f ) {
				ai_moves.isRunning = false; // turn off movement if at target
				ai_moves.isWalking = false;
				moveVector = Vector3d::zero;
			}
			else {
				moveVector.z = 0;
				moveVector.normalize();
			}
		}

		// Get rigidbody's velocity
		Vector3d vMoveVelocity;
		vMoveVelocity = pRigidBody->GetVelocity();

		
		int iTerrainCurrentBlock = Terrain::EB_DIRT;
		// Change friction based on block
		ftype slowdownMultiplier = 1.0f;
		switch ( iTerrainCurrentBlock )
		{
			case Terrain::EB_SAND:
			case Terrain::EB_MUD:	slowdownMultiplier = 0.3f; break;
			case Terrain::EB_ICE:	slowdownMultiplier = 0.08f; break;
		}
		// Slow down movement
		if ( moveVector.sqrMagnitude() < 0.1f ) {
			vMoveVelocity += (-vMoveVelocity.normal()) * Time::deltaTime * m_stats->fGroundAccelBase * slowdownMultiplier;
			if ( vMoveVelocity.magnitude() < Time::deltaTime * m_stats->fGroundAccelBase * slowdownMultiplier * 1.2f ) {
				vMoveVelocity = Vector3d( 0,0,0 );
			}
		}
		// Add to move velocity
		else {
			vMoveVelocity += moveVector * Time::deltaTime * m_stats->fGroundAccelBase * slowdownMultiplier;
		}
		
		// Get the move speed limit
		ftype limit;
		if ( bIsProne )
			limit = m_stats->fProneSpeed;
		else if ( bIsCrouching )
			limit = m_stats->fCrouchSpeed;
		else
			limit = m_stats->fRunSpeed;
		if ( bIsSprinting )
			limit *= m_stats->fSprintSpeed/m_stats->fRunSpeed;
		
		limit *= 0.7f;

		// Limit the xy movement while moving
		Vector3d testCase = Vector3d( vMoveVelocity.x, vMoveVelocity.y, 0 );
		if ( testCase.magnitude() > limit )
		{
			testCase = testCase.normal() * limit;
			vMoveVelocity.x = testCase.x;
			vMoveVelocity.y = testCase.y;
		}

		// Vertical movement (Jumping)
		vMoveVelocity.z = pRigidBody->GetVelocity().z;
		//if ( input->axes.jump.pressed() ) //if ( fInput[iJump] )
		if ( false )
		{
			if ( bIsProne )
				bWantsProne = false;
			else
			{
				vMoveVelocity.z = m_stats->fJumpVelocity;

				// Subtract stamina if sprinting
				if ( bIsSprinting )
				{
					stats.fStamina -= 2.0f;
				}

				if ( !bIsSprinting ) {
					model->PlayAnimation( "jump" );
				}
				else {
					model->PlayAnimation( "jump_sprint" );
				}

				// Reset slide timer
				fSlideCounter = -0.01f;
			}
		}

		// Autojump (blockhop)
		//if (( bAutojumpEnabled )&&( !bIsCrouching )&&( bIsSprinting ))
		//if (( bAutojumpEnabled )&&( !bIsCrouching )&&( !bIsProne )&&( vMoveVelocity.sqrMagnitude() > 2 ))
		if (( true )&&( !bIsCrouching )&&( !bIsProne )&&( moveVector.sqrMagnitude() > 0.8f ))
		{
			// Raytrace forward
			RaycastHit hitResult;
			Ray ray;
			ray.pos = transform.position + Vector3d( 0,0,0.08f );
			ray.dir = testCase.normal();
			if ( Raycaster.Raycast( ray, 3.0f, &hitResult, 1|2|4 ) && ( fabs( hitResult.hitNormal.z ) < 0.2f ) 
				&& ( hitResult.hitNormal.dot(ray.dir) < -0.5f ) ) // Also check the normal of this wall face
			{
				ray.pos.z += 2; // Increase trace height for trace that looks for upper wall
				ftype distanceToStep = hitResult.distance;
				// Create upwards ray trace to look for open space
				Ray nextRay ( hitResult.hitPos-hitResult.hitNormal, Vector3d(0,0,1) );
				nextRay.pos.z += 2;
				// Check for first open block
				bool blockOpen = !Raycaster.Raycast( ray, hitResult.distance + 1.3f, &hitResult, 1|2|4 ); // Check horizontal
				if ( blockOpen && !Raycaster.Raycast( nextRay, std::max<ftype>( 0.5f, fCurrentHeight-1.9f ), &hitResult, 1|2|4 ) ) // Check upwards
				{
					// Jump if far enough
					if ( distanceToStep > ai.hull.radius+0.4f )
					{
						vMoveVelocity.z = 9.4f + testCase.sqrMagnitude() * 0.012f;

						// Subtract stamina if sprinting
						if ( bIsSprinting )
						{
							stats.fStamina -= 2.0f;
						}
					}
					else // If too close, then just rise up
					{
						//vMoveVelocity.z = 0.0f;
						//pMainRigidBody->SetPosition( pMainRigidBody->GetPosition() + (Vector3d( -vMoveVelocity.x,-vMoveVelocity.y,8.0f ) * Time::deltaTime) );
						vMoveVelocity.x *= 0.6f;
						vMoveVelocity.y *= 0.6f;
						vMoveVelocity.z = 3.7f; // Manually set velocity
					}
				}
			}
		}

		// Remove stamina if sprinting
		if (( bIsSprinting )&&( vMoveVelocity.sqrMagnitude() > 1 ))
		{
			stats.fStamina -= Time::deltaTime * 8.0f;
		}

		// Set rigidbody's velocity
		pRigidBody->SetVelocity( vMoveVelocity );

		// Animation set!
		ftype sqrSpeed = vMoveVelocity.sqrMagnitude();
		if ( bIsProne )
		{
			if ( sqrSpeed > sqr(m_stats->fProneSpeed*0.8f*0.7f) ) {
				animator.SetMoveAnimation( NPC::MoveAnimProneMove );
			}
			else {
				animator.SetMoveAnimation( NPC::MoveAnimProneIdle );
			}
		}
		else if ( bIsCrouching )
		{
			if ( sqrSpeed > sqr(m_stats->fCrouchSpeed*(m_stats->fSprintSpeed/m_stats->fRunSpeed)*0.8f*0.7f) ) {
				animator.SetMoveAnimation( NPC::MoveAnimCrouchSprint );
			}
			else if ( sqrSpeed > sqr(m_stats->fCrouchSpeed*0.8f*0.4f) ) {
				animator.SetMoveAnimation( NPC::MoveAnimCrouchMove );
			}
			else {
				animator.SetMoveAnimation( NPC::MoveAnimCrouchIdle );
			}
		}
		else
		{
			if ( sqrSpeed > sqr(m_stats->fSprintSpeed*0.8f*0.7f) ) {
				animator.SetMoveAnimation( NPC::MoveAnimWalkSprint );
			}
			else if ( sqrSpeed > sqr(m_stats->fRunSpeed*0.8f*0.4f) ) {
				animator.SetMoveAnimation( NPC::MoveAnimWalkMove );
			}
			else {
				animator.SetMoveAnimation( NPC::MoveAnimWalkIdle );
			}
		}


	}

}