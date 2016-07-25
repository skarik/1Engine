
// Includes
#include "CAfterPlayer.h"
#include "CMccCharacterModel.h"
#include "core/settings/CGameSettings.h"

// motion
#include "after/interfaces/CCharacterMotion.h"
#include "core/input/CInput.h"
#include "core-ext/input/CInputControl.h"
#include "physical/physics/water/Water.h"
#include "after/terrain/Zones.h"
#include "after/physics/wind/WindMotion.h"
#include "core/math/random/Random.h"
#include "engine/physics/raycast/Raycaster.h"
#include "Math.h"

#include "after/states/skilltree/CSkillTree.h"

#include "after/entities/hud/player/CPlayerHudStatus.h"
#include "after/entities/hud/player/CPlayerInventoryGUI.h"
#include "after/entities/hud/player/CSkillTreeGUI.h"
#include "after/entities/hud/player/CPlayerLogbook.h"
#include "after/entities/hud/player/CCharacterScreen.h"
#include "after/entities/hud/player/CPlayerMenu.h"
#include "after/entities/hud/player/Plhud_RadialMenu.h"
#include "after/renderer/objects/hud/Plhud_TextNotifier.h"
#include "after/entities/hud/player/CInventoryGUI.h"

#include "after/entities/cutscene/CDialogueGUI.h"

#include "after/entities/world/client/CQuestSystem.h"

#include "after/entities/item/CRecipeLibrary.h"

// Audio and effects
#include "engine/audio/CAudioInterface.h"
#include "engine-common/entities/effects/CFXMaterialHit.h"

#include "engine-common/entities/CRendererHolder.h"
#include "renderer/object/screenshader/effects/CBloomShader.h"

//#include "unused/COctreeTerrain.h"
//#include "unused/CVoxelTerrain.h"
#include "after/terrain/VoxelTerrain.h"

#include "after/renderer/objects/hud/Ploverlay_Icing.h"
#include "after/renderer/objects/hud/Ploverlay_Hurt01.h"
#include "after/renderer/objects/hud/Plhud3d_Hurt01.h"
#include "after/renderer/objects/hud/Ploverlay_Goggles01.h"

#include "renderer/object/screen/CScreenFade.h"

// for testing
#include "after/entities/item/weapon/CWeaponItemGenerator.h"
#include "after/entities/item/props/ItemCampfire.h"

// For damage
#include "after/entities/item/skill/skills/defensive/SkillShield.h"

#include "after/states/CWorldState.h"

#include "engine-common/entities/CPointSpawn.h"

#include "engine-common/physics/motion/CRagdollCollision.h"

#include "engine/state/CGameState.h"

#include "engine/physics/motion/CRigidbody.h"

#include "engine/system/GameMessages.h"

#include "physical/skeleton/skeletonBone.h"

#include "after/types/terrain/BlockType.h"
#include "after/types/terrain/WeatherTypes.h"

#include "core-ext/animation/CAnimAction.h"
#include "core-ext/animation/CAnimation.h"

#include "after/physics/water/Water.h"


#include "after/entities/character/npc/zoned/CZonedCharacterController.h"
#include "after/entities/world/CNpcSpawner.h"
#include "after/types/character/NPC_WorldState.h"
#include "after/interfaces/io/CZonedCharacterIO.h"
#include "after/entities/character/npc/CNpcBase.h"

#include "after/renderer/objects/hud/Plhud_AreaNotifier.h"

#include "after/terrain/edit/CTerrainAccessor.h"

#include "core/system/Screen.h"

// == Constructor ==
CAfterPlayer::CAfterPlayer ( CPlayerStats* newStats )
	: CCharacter(), pl_stats( newStats ), isLoaded(false)
{
	//=========================================//
	// Input

	// Create input controller
	input = new CInputControl( this );
	input->Capture();
	// Set input to default values
	bCanMove		= true;
	bOverrideInput	= false;
	bHasInput		= true;
	bCanMouseMove	= true;

	//=========================================//
	// Camera

	pCamera	= new CCamera ();
	pCamera->SetActive();
	//pCamera->orthographic = true;
	pCamera->ortho_size.x = Screen::Info.width * 0.1f;
	pCamera->ortho_size.y = Screen::Info.height * 0.1f;
	fTurnSensitivity	= 1;

	//=========================================//
	// Audio

	pListener = Audio.CreateListener();

	//=========================================//
	// Character Stats and Info

	// Create a random character if no stats passed in.
	if ( newStats == NULL )
	{
		pl_stats = new CPlayerStats();
		pl_stats->Randomize();
		pl_stats->race_stats->SetLuaDefaults();
	}
	pl_race_stats = pl_stats->race_stats;
	base_race_stats = new CRacialStats();
	*base_race_stats = *pl_race_stats;
	// STATS
	// take the stats from the player stats and assign the stats of type stats to those stats.
	stats = *(pl_stats->stats);	// give player's stats the values stored in the player stats loaded from file
	//delete pl_stats->stats;	// delete player stats' stats
	pl_stats->stats = &stats;	// tell player stats to look at player's stats from now on
	pl_race_stats->stats = &stats;
	pl_stats->pOwner = this;
	pl_race_stats->pOwner = this;

	//=========================================//
	// Motion and Camera State

	m_motion = new CCharacterMotion();
	m_motion->m_character = this;
	m_motion->m_stats = pl_race_stats;
	m_motion->InitMovement();

	m_cameraUpdateType	= (stateFunc_t)&CAfterPlayer::cam_Default;
	//m_cameraUpdateType	= (stateFunc_t)&CMccPlayer::cam_ThirdPerson;
	//m_cameraUpdateType	= (stateFunc_t)&CMccPlayer::cam_Inventory;

	//=========================================//
	// Camera State

	bUseThirdPersonMode	= false;
	bUseBeautyCamMode	= false;
	bUseBeautyMontage	= false;
	fCamAnimBlend	= 0;
	bUseAnimCamMode = false;

	vCameraRotationPrev	= Vector3d();
	vCameraPositionPrev	= Vector3d();
	fCamSwapBlend		= 0;

	//pMainRigidBody->SetFriction( 0.01f );
	//InitMovetypeConstants();
	//SetMovetypeConstants();

	bIsStunned = false;
	//bAllowingSprint = true;
	bSprintDisabled = false;

	fMovementMultiplier = 1.0;

	//=========================================//
	// Lookat Setup

	pCurrentLookedAt	= NULL;
	fMaxUseDistance	= 4.0f;

	//=========================================//
	// Melee Input

	vPreviousAimRotation	= Vector3d();
	vNextAimRotation		= Vector3d();

	vAimingArc	= Vector4d( 50,0,3,0 );

	//=========================================//
	// Inventory and Item Pickup

	pMyInventory	= new CPlayerInventory( this, 4,6, (pl_race_stats->iRace==CRACE_MERCHANT)?4:2, true );
	
	fPickupTimer	= 0.0f;
	bPickupDone		= false;

	//=========================================//
	// Screen Effect Information

	pBloomTarget	= NULL;
	fScreenBlurAmount	= 1.0f;
	pScreenIcing	= new Ploverlay_Icing;
	pScreenIcing->mIcingValue = -0.1f;

	// ===Attachers===
	m_shield = NULL;

	// ===Heads Up Display===
	pMyHud			= new CPlayerHud( this, pMyInventory );

	//=========================================//
	// Inventory, Library, and Skilltree

	// Create player inventory
	pl_bags = new CInventory* [3];
	pl_bags[0] = pMyInventory;
	pl_bags[1] = NULL;
	pl_bags[2] = NULL;
	// Create recipe library
	pl_itemlib = new CRecipeLibrary ();
	pMyCrafting		= new CCrafting (4, 4, pl_itemlib, this);
	m_skilltree		= new CSkillTree();
	m_skilltree_race= new CSkillTree();

	//Create a QuestSystem
	pMyQuests = new CQuestSystem();

	//=========================================//
	// Graphical Interfaces

	// Create player huds
	pl_hud = new CPlayerHudStatus ( this );
	pl_logbookGUI	= new CPlayerLogbook ( pl_itemlib );
	pl_chestGUI = new CInventoryGUI (this, pl_bags);
	pl_inventoryGUI = new CPlayerInventoryGUI ( this, pl_bags, pl_itemlib, pMyCrafting, pl_logbookGUI, pl_chestGUI );
	pl_characterGUI	= new CCharacterScreen ( this, pl_stats );
	pl_skilltreeGUI = new CSkillTreeGUI ( this, m_skilltree, m_skilltree_race, pl_inventoryGUI);
	pl_questGUI = new CQuestSysRenderer(pMyQuests);
	pl_dialogueGUI	= new CDialogueGUI ();
	pl_gui_menu = new CPlayerMenu( this, pl_inventoryGUI, pl_logbookGUI, pl_questGUI, pl_skilltreeGUI, pl_characterGUI, pl_dialogueGUI, pl_chestGUI );

	//pl_radial_menu = new Plhud_RadialMenu( this );
	pl_radial_menu = NULL;

	//=========================================//
	// Settings

	// Initialize field of view
	fViewAngle	= 80;

	//=========================================//
	// Player Model and Animation

	// Create animation
	animator.m_combat_info = &combatInfo;
	animator.m_race_stats = pl_race_stats;
	animator.m_stats = &stats;
	animator.m_inventory = pMyInventory;
	// Create player model
	model = new CMccCharacterModel( this );
	model->LoadBase( "clara" );
	((CMccCharacterModel*)model)->SetVisualsFromStats( pl_race_stats );
	// Set animator's info
	animator.m_model = model;
	// Set motion info
	m_motion->m_model = model;
	m_motion->m_animator = &animator;

	//=========================================//
	// World Sampling

	// Initialize local terrain sampler
	InitAreaInfo();

	//=========================================//
	// Game Setup

	// Spawn character
	//GenerateSpawnpoint();
	if ( CVoxelTerrain::GetActive() ) 
	{
		transform.position = pl_stats->vPlayerCalcSpawnPoint;
		transform.rotation.Euler( Vector3d(0,0,0) );
	}
	else
	{
		CPointSpawn* spawnPoint = (CPointSpawn*)CGameState::Active()->FindFirstObjectWithTypename( "CPointSpawn" );
		if ( spawnPoint ) {
			transform.position = spawnPoint->m_position;
			transform.rotation.Euler( Vector3d(0,0,0) );
		}
	}
	transform.SetDirty();
	OnSpawn();
}

// == Destructor ==
CAfterPlayer::~CAfterPlayer ( void )
{
	// Set animator to no target
	animator.m_combat_info		= NULL;
	animator.m_inventory		= NULL;
	animator.m_model			= NULL;
	animator.m_race_stats		= NULL;
	animator.m_stats			= NULL;

	// Remove movement
	m_motion->FreeMovement();
	delete_safe(m_motion);
	
	// Move items in crafting to the inventory
	//pMyCrafting->
	pl_inventoryGUI->EmptyCrafting();

	arstring<256> sSaveLocation;

	// Save inventory
	sprintf( sSaveLocation, "%s/inventory", CGameSettings::Active()->MakePlayerSaveDirectory().c_str() );
	pMyInventory->SaveInventory(sSaveLocation);
	// Save skill trees
	sprintf( sSaveLocation, "%s/skills/aggregate", CGameSettings::Active()->MakePlayerSaveDirectory().c_str() );
	m_skilltree->Save(sSaveLocation);
	m_skilltree_race->Save(sSaveLocation);

	pl_stats->SaveToFile();

	pMyQuests->saveQuests();
	delete_safe_decrement(pMyQuests);

	// Free child objects
	delete_safe_decrement(pMyHud);

	delete_safe( pMyInventory );
	delete_safe( pMyCrafting );

	delete_safe_decrement(pl_hud);
	delete_safe_decrement(pl_logbookGUI);
	delete_safe_decrement(pl_gui_menu);
	delete_safe_decrement(pl_chestGUI);
	delete_safe_decrement(pl_inventoryGUI);
	delete_safe_decrement(pl_characterGUI);
	delete_safe_decrement(pl_skilltreeGUI);
	delete_safe_decrement(pl_questGUI);
	delete_safe_decrement(pl_dialogueGUI);

	delete_safe_decrement(model);

	pBloomTarget = NULL; // Not ownership
	delete_safe_decrement( pScreenIcing );

	// Delete playerStats last
	delete base_race_stats;
	delete pl_stats;

	// Force world to save
	if ( ActiveGameWorld ) {
		ActiveGameWorld->Save();
	}
}

void CAfterPlayer::GenerateSpawnpoint ( void )
{
	// Make proper spawnpoint.
	//pl_stats->MakeSpawnpoint();
	//cout << "spawn point: " << pl_stats->vPlayerCalcSpawnPoint.x << " " << pl_stats->vPlayerCalcSpawnPoint.y << " " << pl_stats->vPlayerCalcSpawnPoint.z << endl;
	if ( CVoxelTerrain::GetActive() ) 
	{
		transform.position = pl_stats->vPlayerCalcSpawnPoint; // This is now old. Should reset it, though.
		transform.rotation.Euler( Vector3d( 0,0,random_range( 0.0f, 360.0f ) ) );
	}
	else
	{
		CPointSpawn* spawnPoint = (CPointSpawn*)CGameState::Active()->FindFirstObjectWithTypename( "CPointSpawn" );
		if ( spawnPoint ) {
			transform.position = spawnPoint->m_position;
			transform.rotation.Euler( Vector3d(0,0,0) );
		}
	}
	transform.SetDirty();

	// Stop moving
	if ( m_motion->m_rigidbody ) {
		m_motion->m_rigidbody->SetVelocity( Vector3d(0,0,0) );
	}
}

void CAfterPlayer::OnSpawn ( void )
{
	// Stop moving
	/*if ( m_motion->m_rigidbody ) {
		m_motion->m_rigidbody->SetVelocity( Vector3d(0,0,0) );
		vFallingVelocity = Vector3d(0,0,0);
	}*/
	m_motion->StopMotion();

	// Reset bleed timers
	fBleedTime		= 0.0f;
	fBleedDamage	= 0.0f;
	fBleedDamageThreshold = 30.0f; // for now, a flat 30 damage.
	bBleeding		= false;

	// Blur screen
	fScreenBlurAmount = 2.0f;

	// Load proper skill trees now
	OnReceiveSignal( Game::MSG_GAME_SKILLTREE_RELOAD );

	// Possibly causing memory problems.
	if ( !isLoaded ) {
		arstring<256> sLoadLocation;
		sprintf( sLoadLocation, "%s/inventory", CGameSettings::Active()->MakePlayerSaveDirectory().c_str() );
		pMyInventory->LoadInventory (sLoadLocation);
		
		isLoaded = true;
	}

	// Reenable input
	bHasInput = true;

	// Capture input
	input->Capture();
}

// == Player Death ==
void CAfterPlayer::OnDeath ( Damage const& dmg )
{
	pl_inventoryGUI->EmptyCrafting();

	// Save all data NOW
	arstring<256> sSaveLocation;
	sprintf( sSaveLocation, "%s/inventory", CGameSettings::Active()->MakePlayerSaveDirectory().c_str() );
	pMyInventory->SaveInventory(sSaveLocation);
	// Save skill trees
	sprintf( sSaveLocation, "%s/skills/aggregate", CGameSettings::Active()->MakePlayerSaveDirectory().c_str() );
	m_skilltree->Save(sSaveLocation);
	m_skilltree_race->Save(sSaveLocation);
	// Save stats
	pl_stats->SaveToFile();

	// Go to death mode
	std::cout << "DEATHU" << std::endl;
	//m_moveType			= (stateFunc_t)&CMccPlayer::mvt_Dead;
	m_motion->SetMovementModeImmediate( NPC::MOVEMENT_DEAD );

	// Load death spawn point
	pl_stats->vPlayerInitSpawnPoint = Vector3d( 1,1,1 );

	// Increment death count (heheh, nub)
	pl_stats->fDeathCount += 1.0f;
	// Stop fall damage (dgaf!)
	//vFallingVelocity = Vector3d(0,0,0);

	// Play death animation
	{
		string deadAnim = "die_forward";
		if ( Random.Next()%2 == 0 ) {
			deadAnim = "die_backward";
		}
		CAnimation* anim = model->GetAnimationState();
		CAnimAction* death;
		if ( death = anim->FindAction( deadAnim ) ) {
			death->end_behavior = CAnimAction::END_HOLD_END;
			death->layer = 6;
			anim->Play( deadAnim );
		}
	}
}

// == Signals ==
void CAfterPlayer::OnReceiveSignal ( const uint64_t n_signal ) 
{
	if ( n_signal == Game::MSG_GAME_SKILLTREE_RELOAD )
	{
		m_skilltree->ManagedLoad( "system/v_skilltree_skills.txt" );
		switch ( pl_race_stats->iRace )
		{
		case CRACE_HUMAN:
			m_skilltree_race->ManagedLoad( "system/v_skilltree_human.txt" );
			break;
		case CRACE_ELF:
			m_skilltree_race->ManagedLoad( "system/v_skilltree_elf.txt" );
			break;
		case CRACE_FLUXXOR:
			m_skilltree_race->ManagedLoad( "system/v_skilltree_fluxxor.txt" );
			break;
		case CRACE_DWARF:
			m_skilltree_race->ManagedLoad( "system/v_skilltree_dwarf.txt" );
			break;
		case CRACE_KITTEN:
			m_skilltree_race->ManagedLoad( "system/v_skilltree_mayang.txt" );
			break;
		case CRACE_MERCHANT:
			m_skilltree_race->ManagedLoad( "system/v_skilltree_bani.txt" );
			break;
		}

		arstring<256> sLoadLocation;
		sprintf( sLoadLocation, "%s/skills/aggregate", CGameSettings::Active()->MakePlayerSaveDirectory().c_str() );
		m_skilltree->Load( sLoadLocation );
		m_skilltree_race->Load( sLoadLocation );
		// Sync up levels after loading (also updates the passives)
		pl_skilltreeGUI->SyncLevels();
	}
}

// == Updates ==
void CAfterPlayer::Update ( void )
{
	// Update the motion's state
	m_motion->m_input = this->input;

	// Update stats
	{
		pl_stats->pOwner = this;
		pl_stats->UpdateStats();
			
		// Do leveling up
		if ( stats.fExperience >= stats.fExperienceMax ) {
			stats.fExperience -= stats.fExperienceMax;
			pl_stats->LevelUp();
			//pl_race_stats->LevelUp(); //is called in CPlayerStats
		}
	}
	// Do bleeding damage and logic
	if ( bBleeding ) 
	{
		// Healed above 15% health, so stop bleeding.
		if ( stats.fHealth > stats.fHealthMax * 0.15f )
		{
			bBleeding = false;
		}
		else
		{
			// Stuck at 1 HP.
			stats.fHealth = 1.0f;
			// Keep track of time bleeding out.
			fBleedTime += Time::deltaTime;

			// Deal 1 bleed damage every 4 seconds
			if ( fBleedDamage + 1 < fBleedDamageThreshold ) {
				fBleedDamage += Time::deltaTime / 4.0f;
			}
		}
	}

	// Now apply effects

	// Now update combat state
	combatInfo.Update();

	// Set facing
	Vector3d lookAtPos = pCamera->transform.position + pCamera->transform.Forward();
	//model->SetLookAtPosition( lookAtPos );
	//model->FixAimingAnglesHack( pCamera->transform.rotation );
	//model->SetLookAtRotations( pCamera->transform.rotation, Rotator() );
	model->SetLookAtRotations( vCameraRotation, Rotator() );

	Rotator rotMatrix;
	rotMatrix.setRotation( vPlayerRotation - Vector3d( 0,0,90 ) );
	model->SetSplitFacing( true );
	model->SetFaceAtRotation( rotMatrix );
	//DoMoveAnimation();
	animator.DoMoveAnimation();
	if ( m_motion->m_rigidbody ) {
		model->SetAnimationMotion( m_motion->m_rigidbody->GetVelocity() );
	}
	if ( pMyInventory->GetCurrentEquipped() ) {
		model->SetAimerIK( 0, pMyInventory->GetCurrentEquipped()->GetHoldType() );
	}
	else {
		model->SetAimerIK( 0, Item::EmptyHanded );
	}
	// look at nearest NPC too

	// Grab nearby terrain information
	UpdateTerrainSample();
	UpdateAreaInfo();

	// Move unobstructed
	/*if ( bInActiveArea && bMoveUnobstructed ) {
		MoveUnstuck();
	}*/
	if ( bInActiveArea ) {
		m_motion->Update();
	}

	// Toggle cameras
	if ( Input::Keydown('7') ) {
		//bUseThirdPersonMode = !bUseThirdPersonMode;
		if ( bUseThirdPersonMode ) {
			bUseThirdPersonMode = false;
			bUseBeautyCamMode = true;
		}
		else if ( bUseBeautyCamMode ) {
			bUseBeautyCamMode = false;
			bUseBeautyMontage = true;
		}
		else if ( bUseBeautyMontage ) {
			bUseBeautyMontage = false;
		}
		else {
			bUseThirdPersonMode = true;
		}
	}
	// Toggle test ragdoll
	if ( Input::Keydown('8') ) {
		//bUseThirdPersonMode = !bUseThirdPersonMode;
	}
	// Toggle test ragdoll
	if ( Input::Keydown('9') ) {
		if ( ((glBone*)model->GetSkeletonRoot()->owner)->ragdollStrength > 0.5 ) {
			model->BlendToAnimation( 2.0f );
		}
		else {
			model->BlendToRagdoll( 2.0f );
		}
	}

	// If stamina low, do Out of Breath huff
	if ( stats.fStamina <= std::max<Real>( stats.fStaminaMax*0.02f, 5.0f ) )
	{
		 DoSpeech( NPC::SpeechType_OutOfBreath );
	}

	// Swap vision modes
	static Ploverlay_Goggles01* gogov = NULL;
	{
		static bool blending = false;
		static Real blendTime = 0;
		if ( input->axes.tglEyewear.pressed() )
		{
			if ( glMaterial::special_mode == Renderer::SP_MODE_NORMAL ) {
				glMaterial::special_mode = Renderer::SP_MODE_ECHO;
			}
			else if ( glMaterial::special_mode == Renderer::SP_MODE_ECHO ) {
				glMaterial::special_mode = Renderer::SP_MODE_SHAFT;
			}
			else {
				glMaterial::special_mode = Renderer::SP_MODE_NORMAL;
			}
			/*if ( gogov == NULL )
			{
				//gogov = new Ploverlay_Goggles01();

				model->PlayScriptedAnimation( "hold_anim_googleson" );
				blending = true;
				blendTime = 0;

				//new CScreenFade ( false, 0.4f, 0.4f );
				//new CScreenFade ( true, 0.2f, 0.8f );
				new CScreenFade ( false, 0.4f, 0.5f );
			}
			else
			{
				//delete gogov;
				//gogov = NULL;

				model->PlayScriptedAnimation( "hold_anim_googlesoff" );
				blending = true;
				blendTime = 0;

				//new CScreenFade ( false, 0.2f, 0.3f );
				//new CScreenFade ( true, 0.4f, 0.5f );
				new CScreenFade ( false, 0.4f, 0.4f );
			}*/
		}
		if ( blending )
		{
			blendTime += Time::deltaTime;
			if ( gogov == NULL ) {
				if ( blendTime >= 0.9f ) {
					blending = false;
					gogov = new Ploverlay_Goggles01();
					new CScreenFade ( true, 0.2f );
				}
			}
			else {
				if ( blendTime >= 0.8f ) {
					blending = false;
					delete gogov;
					gogov = NULL;
					new CScreenFade ( true, 0.2f );
				}
			}
		}
	}

	// Now work on the player
	*pl_race_stats = *base_race_stats;
	Real movetypeMultiplier = fMovementMultiplier;
	if ( bBleeding ) {
		movetypeMultiplier *= 0.5;
	}
	if ( WindMotion.GetWeather( transform.position ) == Terrain::WTH_DUST_STORM ) {
		movetypeMultiplier = std::min<Real>( movetypeMultiplier, std::max<Real>( 0.7f, movetypeMultiplier*0.7f ) );
	}
	{
		pl_race_stats->fRunSpeed *= movetypeMultiplier;
		pl_race_stats->fSprintSpeed *= movetypeMultiplier;
		pl_race_stats->fCrouchSpeed *= movetypeMultiplier;
		pl_race_stats->fProneSpeed *= movetypeMultiplier;
		pl_race_stats->fSwimSpeed *= movetypeMultiplier;
	}
	//SetMovetypeConstants();

	CCharacter::Update();//CPlayer::Update();

	if ( gogov != NULL ) {
		ActiveGameWorld->cBaseAmbient = ActiveGameWorld->cBaseAmbient - Color( 0.4f,0.43f,0.5f )*1.4f;

		/*pBloomTarget = (CBloomShader*)((CRendererHolder*)CGameState::pActive->FindFirstObjectWithName( "Bloom Shader Holder" ))->GetRenderer();
				CRendererHolder* shaderHolder5 = new CRendererHolder( new CDepthSplitShader() );
		shaderHolder5->name = "Depth Split Shader Holder";
		shaderHolder5->RemoveReference();*/
		
	}

	// Stop movement if going into unloaded area
	if ( !bInActiveArea && m_motion->m_rigidbody ) {
		m_motion->m_rigidbody->SetVelocity( Vector3d(0,0,0) );
	}

	// Set screen blur amount
	if ( !pBloomTarget ) { // Get reference to current bloom
		CRendererHolder* foundHolder = (CRendererHolder*)(CGameState::Active()->FindFirstObjectWithName( "Bloom Shader Holder" ));
		if ( foundHolder != NULL ) {
			pBloomTarget = (CBloomShader*)(foundHolder->GetRenderer());
		}
	}
	else
	{
		if ( fScreenBlurAmount > 0 ) { // Decrease screen blur over time
			fScreenBlurAmount -= Time::deltaTime*0.707f;
		}
		else {
			fScreenBlurAmount = 0.0f; // Limit to 0
		}
		fScreenBlurAmount = std::max<ftype>( (0.25f - (stats.fStamina/stats.fStaminaMax))*3, fScreenBlurAmount ); // stamina blur
		fScreenBlurAmount = std::max<ftype>( std::min<ftype>( 0.1f, 0.66f - (stats.fHealth/stats.fHealthMax) ), fScreenBlurAmount ); // damage blur
		if ( bBleeding ) {
			fScreenBlurAmount += std::max<ftype>( -0.05f, sin( Time::currentTime )*0.3f ) * Time::deltaTime; // Bleeding blur.
		}
		// Set screen blur
		pBloomTarget->SetBlur( std::min<ftype>( fScreenBlurAmount, 1 ) );
		// Set red desaturation
		pBloomTarget->SetRed( std::max<ftype>( (1.0f+(fBleedDamage/fBleedDamageThreshold)) - (stats.fHealth/stats.fHealthMax)*3, 0 ) ); 
	}

	// Do freezing screen effects
	if ( pScreenIcing ) {
		// .5 is freezing, 1.5 is hot, 1.0 is comfortable
		ftype temperature = WindMotion.GetTemperatureFast( transform.position );
		ftype targetValue = (-(temperature-0.7f))/0.4f; // 0.6 where frost starts, 0.2 where frost is full
		pScreenIcing->mIcingValue += (targetValue-pScreenIcing->mIcingValue) * Time::TrainerFactor(0.01f);
	}

	// Do movement based gameplay effects
	if ( m_motion->m_rigidbody )
	{
		// Grab angles
		vPreviousAimRotation = vNextAimRotation;
		vNextAimRotation = Vector3d( 0, vCameraRotation.y, vPlayerRotation.z );
		// Now do the aiming arcs!
		Vector3d initialVect ( (ftype)cos(degtorad(vAimingArc.z)),(ftype)sin(degtorad(vAimingArc.z)), vAimingArc.w );
		Vector3d delta ( vNextAimRotation.z-vPreviousAimRotation.z + Random.Range(-0.005f,0.005f), vPreviousAimRotation.y-vNextAimRotation.y + Random.Range(-0.005f,0.005f), 0 );
		initialVect += (delta*0.24f)/fTurnSensitivity; // Change aiming based on aiming angle
		{	// Do aiming changes based on movement
			Matrix4x4 rotMatrix;
			rotMatrix.setRotation( vPlayerRotation );
			initialVect.x -= m_motion->m_rigidbody->GetVelocity().dot( rotMatrix*Vector3d::left ) * 0.06f;
			initialVect.z += m_motion->m_rigidbody->GetVelocity().dot( rotMatrix*Vector3d::forward ) * 0.05f;
		}
		initialVect.normalize(); // Limit values
		vAimingArc.z = (ftype)radtodeg( atan2( initialVect.y,initialVect.x ) );
		vAimingArc.w = Math.Clamp( initialVect.z, 0,1 );

		// Add to distance traveled
		pl_stats->fFeetTravelled += m_motion->m_rigidbody->GetVelocity().magnitude() * Time::deltaTime;

		// Do spawn chances for walking in sand.
		if ( iTerrainCurrentBlock == Terrain::EB_SAND )
		{
			if ( m_motion->m_rigidbody->GetVelocity().sqrMagnitude() > sqr(pl_race_stats->fRunSpeed*0.7f) )
			{
				static ftype lastDistance = pl_stats->fFeetTravelled;
				if ( (pl_stats->fFeetTravelled-lastDistance) > 2.2f ) { // Check every new block
					if ( ItemGenerator.ChanceDesertTripDrops() ) {
						ItemGenerator.DropDesertTrip( transform.position - m_motion->m_rigidbody->GetVelocity().normal() );
						// TODO: Make stun a dedicated function to play the animation!
						// example:
						// if ( StunWithAnimation( "fall_hard", &stunTime ) ) {
						//		fStunTimer = stunTime;
						//		fScreenBlurAmount += 0.6f;
						// }
						CAnimation* anim = model->GetAnimationState();
						CAnimAction* stagger;
						if ( stagger = anim->FindAction( "fall_hard" ) ) { // TODO: Different animations for different places! (water stun is different)
							if ( !stagger->isPlaying ) {
								anim->Play( "fall_hard" );
								
								//m_moveType = (stateFunc_t)&CMccPlayer::mvt_Stunned;
								m_motion->fStunTimer = (stagger->GetLength() / stagger->framesPerSecond);

								// Blur the screen hardcore
								fScreenBlurAmount += 0.6f;
							}
						}
						//m_moveType	= (stateFunc_t)&CMccPlayer::mvt_Stunned;	
						m_motion->SetMovementModeImmediate( NPC::MOVEMENT_STUN_GENERAL );
						//
						bIsStunned = true;
					}
					lastDistance = pl_stats->fFeetTravelled;
				}
			}
		}
	}// End rigidbody check


	// Do tooltips check
	{
		static Real tooltipWeapons [5] = {0,0,0,0};
		static Real tooltipCooldown [5] = {0,0,0,0};	// todo: move to class?
		static int previousTarget = -1;
		CWeaponItem* mItem = pMyInventory->GetCurrentEquipped();
		for ( uint i = 0; i < 5; i += 1 ) {
			tooltipCooldown[i] -= Time::deltaTime;
		}
		if ( mItem ) {
			int target = -1;
			if ( mItem->GetItemData()->eTopType == WeaponItem::ItemBlade ) {
				target = 0;
			}
			else if ( mItem->GetItemData()->iItemId == 2 ) {
				target = 1;
			}
			else if ( mItem->GetIsSkill() ) {
				target = 2;
			}
			else if ( mItem->GetItemData()->eTopType == WeaponItem::ItemBow ) {
				target = 3;
			}
			else if ( mItem->GetBaseClassName() == "CItemJunkBase" ) {
				target = 4;
			}

			if ( target != -1 ) {
				if ( tooltipCooldown[target] <= 0 )
				{
					tooltipWeapons[target] += Time::deltaTime;
					if ( target != previousTarget && tooltipWeapons[target] < 14.0f ) {
						const Vector2d messagePosition = Vector2d( 0.75f,0.6f );
						Plhud_TextNotifier* notifier = NULL;
						if ( target == 0 ) {
							notifier = new Plhud_TextNotifier( "LMB  Attack\nQ     Parry\nQ(hold) Defend", messagePosition, false );
						}
						else if ( target == 1 ) {
							notifier = new Plhud_TextNotifier( "RMB  Place block\n", messagePosition, false );
						}
						else if ( target == 2 ) {
							notifier = new Plhud_TextNotifier( "LMB  Cast\nSome Western magic\ncan be mixed with\nother western spells.", messagePosition, false );
						}
						else if ( target == 3 ) {
							notifier = new Plhud_TextNotifier( "RMB  Aim\nLMB  Shoot", messagePosition, false );
						}
						else if ( target == 4 ) {
							notifier = new Plhud_TextNotifier( "LMB  Toss", messagePosition, false );
						}
						else if ( target == 5 ) {
							notifier = new Plhud_TextNotifier( "LMB  Attack\nRMB  Attack\nLMB+RMB Hard Attack", messagePosition, false );
						}
						if ( notifier ) {
							notifier->RemoveReference();
						}
					}
				}
				tooltipCooldown[target] = 6.0f;
			}

			previousTarget = target;
		}
	}
	
	DoPlayerActions();
}
void CAfterPlayer::LateUpdate ( void )
{
	CCharacter::LateUpdate();

	GrabInput();
	(this->*m_cameraUpdateType)();
	
	// Update listener position
	pListener->velocity = pCamera->transform.position - pListener->position;
	pListener->position = pCamera->transform.position;
	pListener->orient_forward = pCamera->transform.Forward();
	pListener->orient_up = pCamera->transform.Up();
}


// ==Fixed update, physics check==
void CAfterPlayer::FixedUpdate ( void )
{
	//CPlayer::FixedUpdate();
	CCharacter::FixedUpdate();

	m_motion->FixedUpdate( &vTurnInput, &vPlayerRotation, &vCameraRotation );
}

bool CAfterPlayer::ApplyLinearMotion ( const Vector3d& n_motion, const ftype n_motionlength ) 
{ 
	if ( bAlive )
	{
		//vLinearMotions.push_back( Vector4d( n_motion.y, -n_motion.x, n_motion.z, n_motionlength ) );
		m_motion->vLinearMotions.push_back( Vector4d( n_motion.y, -n_motion.x, n_motion.z, n_motionlength ) );
		return true;
	}
	return false;
}

// Get aiming arc. Used for melee weapons.
Vector4d CAfterPlayer::GetAimingArc ( void )
{
	// Returns a Vector4d indicating the following aiming properties:
	// X is the width of the arc in degrees.
	// Y is the vertical rotation offset. (non-zero means not aiming down center of screen).
	// Z is the depth rotation.
	// W is ~0 for zero depth, ~1 for deep depth
	return vAimingArc;
}

void DebugSpawnSitter( const Vector3d& spawnPosition );


// ==Input==
// Receive all input and save in the player object
void CAfterPlayer::GrabInput ( void )
{
	// If there's input, grab it.
	input->Update( this, Time::deltaTime );
	if ( !bHasInput )
	{
		// If there's no user input and there's no override present
		// Then reset the object's input
		if ( !bOverrideInput )
		{
			//input->ZeroValues( this );
			vDirInput	= Vector3d( 0,0,0 );
			vTurnInput	= Vector3d( 0,0,0 );

			input->axes.prone.Value				= input->axes.prone.PreviousValue;
			input->axes.prone.PreviousValue		= 0.0f;

			input->axes.crouch.Value			= input->axes.crouch.PreviousValue;
			input->axes.crouch.PreviousValue	= 0.0f;

			//input->axes.jump.Value				= input->axes.jump.PreviousValue;
			input->axes.jump.Value				= 0.0f;
			input->axes.jump.PreviousValue		= 0.0f;

			input->axes.sprint.Value			= input->axes.sprint.PreviousValue;
			input->axes.sprint.PreviousValue	= 0.0f;

			//input->Update( this );
		}
		input->axes.primary.Value			= 0.0f;
		input->axes.primary.PreviousValue	= 0.0f;

		input->axes.secondary.Value			= 0.0f;
		input->axes.secondary.PreviousValue	= 0.0f;

		input->axes.use.Value				= 0.0f;
		input->axes.use.PreviousValue		= 0.0f;

		input->axes.defend.Value				= 0.0f;
		input->axes.defend.PreviousValue		= 0.0f;

		input->axes.tertiary.Value				= 0.0f;
		input->axes.tertiary.PreviousValue		= 0.0f;

		input->axes.tglCompanion.Value			= 0.0f;
		input->axes.tglCompanion.PreviousValue	= 0.0f;
	}
	else
	{
		vDirInput	= input->vDirInput;
		if ( bCanMouseMove ) {
			vTurnInput	+= input->vMouseInput;
		}
		else {
			vTurnInput	= Vector3d( 0,0,0 );
		}
	}
}



void CAfterPlayer::DoPlayerActions ( void )
{
	if ( !bIsStunned )
	{
		// Player item usage
		bool usePrim = true;
		// Check for secondary eqiupment info
		CWeaponItem* currentEquipped = pMyInventory->GetCurrentEquipped(0);
		CWeaponItem* currentEquipped_Offset = pMyInventory->GetCurrentEquipped(1);
		// Do we have a valid offhand?
		bool hasOffset = (currentEquipped_Offset != NULL
			&& currentEquipped_Offset->GetTypeName() != "ItemBlockPuncher"
			&& currentEquipped_Offset->GetTypeName() != "ItemMeleeCombat"
			&& currentEquipped_Offset->GetHands() > 0 );
		if ( currentEquipped == NULL
			|| currentEquipped->GetTypeName() == "ItemBlockPuncher"
			|| currentEquipped->GetTypeName() == "ItemMeleeCombat" )
		{
			// If nothing in the main hand but something valid in the offhand, set the main hand to the offhand.
			if ( hasOffset ) {
				currentEquipped = currentEquipped_Offset;
				currentEquipped_Offset = NULL;
				hasOffset = false;
			}
		}

		// AAAAA
		if ( currentEquipped )
		{
			if ( input->axes.primary )
			{
				if ( input->axes.primary.pressed() ) {
					currentEquipped->StartUse( m_motion->bIsSprinting ? Item::UPrimarySprint : Item::UPrimary );
				}
				if ( currentEquipped->Use( m_motion->bIsSprinting ? Item::UPrimarySprint : Item::UPrimary ) ) {
					usePrim = false;
				}
			}
			else if ( input->axes.primary.released() ) {
				currentEquipped->EndUse( m_motion->bIsSprinting ? Item::UPrimarySprint : Item::UPrimary );
			}

			if ( input->axes.secondary )
			{
				if ( !hasOffset )
				{
					if ( input->axes.secondary.pressed() ) {
						currentEquipped->StartUse( m_motion->bIsSprinting ? Item::USecondarySprint : Item::USecondary );
					}
					if ( currentEquipped->Use( m_motion->bIsSprinting ? Item::USecondarySprint : Item::USecondary ) ) {
						usePrim = false;
					}
				}
				else
				{
					if ( input->axes.secondary.pressed() ) {
						currentEquipped_Offset->StartUse( m_motion->bIsSprinting ? Item::UPrimarySprint : Item::UPrimary );
					}
					if ( currentEquipped_Offset->Use( m_motion->bIsSprinting ? Item::UPrimarySprint : Item::UPrimary ) ) {
						usePrim = false;
					}
				}
			}
			else if ( input->axes.secondary.released() ) {
				if ( !hasOffset ) {
					currentEquipped->EndUse( m_motion->bIsSprinting ? Item::USecondarySprint : Item::USecondary );
				}
				else {
					currentEquipped_Offset->EndUse( m_motion->bIsSprinting ? Item::UPrimarySprint : Item::UPrimary );
				}
			}

			if ( input->axes.tertiary )
			{
				if ( input->axes.tertiary.pressed() ) {
					currentEquipped->StartUse( m_motion->bIsSprinting ? Item::UOptionalSprint : Item::UTertiary );
				}
				if ( currentEquipped->Use( m_motion->bIsSprinting ? Item::UOptionalSprint : Item::UTertiary ) ) {
					usePrim = false;
				}
				if ( hasOffset ) {
					if ( input->axes.defend.pressed() ) {
						currentEquipped_Offset->StartUse( m_motion->bIsSprinting ? Item::UOptionalSprint : Item::UTertiary );
					}
					if ( currentEquipped_Offset->Use( m_motion->bIsSprinting ? Item::UOptionalSprint : Item::UTertiary ) ) {
						usePrim = false;
					}
				}
			}
			else if ( input->axes.tertiary.released() ) {
				currentEquipped->EndUse( m_motion->bIsSprinting ? Item::UOptionalSprint : Item::UTertiary );
				if ( hasOffset ) {
					currentEquipped_Offset->EndUse( m_motion->bIsSprinting ? Item::UOptionalSprint : Item::UTertiary );
				}
			}

			if ( input->axes.defend )
			{
				if ( input->axes.defend.pressed() ) {
					currentEquipped->StartUse( m_motion->bIsSprinting ? Item::UDefendSprint : Item::UDefend );
				}
				if ( currentEquipped->Use( m_motion->bIsSprinting ? Item::UDefendSprint : Item::UDefend ) ) {
					usePrim = false;
				}
				if ( hasOffset ) {
					if ( input->axes.defend.pressed() ) {
						currentEquipped_Offset->StartUse( m_motion->bIsSprinting ? Item::UDefendSprint : Item::UDefend );
					}
					if ( currentEquipped_Offset->Use( m_motion->bIsSprinting ? Item::UDefendSprint : Item::UDefend ) ) {
						usePrim = false;
					}
				}
			}
			else if ( input->axes.defend.released() ) {
				currentEquipped->EndUse( m_motion->bIsSprinting ? Item::UDefendSprint : Item::UDefend );
				if ( hasOffset ) {
					currentEquipped_Offset->EndUse( m_motion->bIsSprinting ? Item::UDefendSprint : Item::UDefend );
				}
			}
		}
		// Do character events
		/*{
			// Handled in CCharacterModel
		}*/

		// Change the equipped item when the mouse wheel is scrolled
		if ( bHasInput ) {
			if ( CInput::DeltaMouseW() != 0 )
			{
				pMyInventory->ChangeEquipped(CInput::DeltaMouseW());
			}
		}
	}

	// Calling the lookAt functions
	DoLookAtCommands();

	bool t_canSprint = !bSprintDisabled;
	if ( !bIsStunned )
	{
		// Pickup or use items when E is pressed
		if ( input->axes.use.pressed() ) //if ( CInput::keydown['E'] )
		{
			DoUseCommand();
		}
		// Pickup items in the area if E is held down for 1 second
		else if ( (bool)(input->axes.use) ) //else if ( CInput::key['E'] )
		{
			if ( !bPickupDone )
			{
				fPickupTimer += Time::smoothDeltaTime;
				if ( fPickupTimer > 0.6f )
				{
					//bPickupDone = true;
					PickupItemsRadius();
				}
			}
			// No sprinting allowed
			t_canSprint = false;
		}
		// Reset states when E is released.
		else
		{
			fPickupTimer = 0;
			bPickupDone = false;
			// Allow sprinting
			//bCanSprint = true;
		}
	}

	// Disallow sprint if no stamina is left
	if ( stats.fStamina <= 0.0f ) {
		//bCanSprint = false;
		t_canSprint = false;
	}

	m_motion->m_canSprint = t_canSprint;

	// Subtract stamina if the head is underwater
	//if ( Water.PositionInside( transform.position + Vector3d( 0,0,m_motion->fPlayerHeight ) ) )
	XTransform t_headPosition;
	model->GetHeadTransform( t_headPosition );
	if ( WaterTester::Get()->PositionInside( t_headPosition.position ) )
	{
		stats.fStamina -= Time::deltaTime;
		// If no stamina left, then slowly drown
		if ( stats.fStamina <= 0.0f )
		{
			Damage drowndamage;
			drowndamage.amount = stats.iStrength * 0.5f * Time::deltaTime;
			drowndamage.type = DamageType::Drown;
			OnDamaged( drowndamage );
		}
	}


	// Toggle radial menu
	/*if ( input->axes.tglCompanion ) {
		pl_radial_menu->visible = true;
	}
	else {
		pl_radial_menu->visible = false;
	}*/
	if ( !bIsStunned )
	{
		if ( input->axes.tglCompanion ) {
			if ( !pl_radial_menu ) {
				pl_radial_menu = new Plhud_RadialMenu( this );
			}
		}
		else {
			if ( pl_radial_menu ) {
				pl_radial_menu->NotifyKill();
				pl_radial_menu = NULL;
			}
		}
	}

	// Debug Spawning
	/*if ( bHasInput ) {
		if ( Input::Keydown('M') )
		{
			// == DEBUG ==
			Ray viewRay;
			viewRay.dir = pCamera->transform.Forward();
			viewRay.pos = pCamera->transform.position;

			RaycastHit result;
			BlockTrackInfo block;

			if ( Raytracer.Raycast( viewRay, 10.0f, &result, &block, 1|2|4, this ) )
			{
				ItemGenerator.MakeShittySword( result.hitPos + result.hitNormal*2.0f );
			}
		}
		if ( Input::Keydown('K') )
		{
			// == DEBUG ==
			Ray viewRay;
			viewRay.dir = pCamera->transform.Forward();
			viewRay.pos = pCamera->transform.position;

			RaycastHit result;
			BlockTrackInfo block;

			if ( Raytracer.Raycast( viewRay, 10.0f, &result, &block, 1|2|4, this ) )
			{
				ItemGenerator.MakeBasicDrill( result.hitPos + result.hitNormal*2.0f );
			}
		}
		if ( Input::Keydown('J') )
		{
			// == DEBUG ==
			Ray viewRay;
			viewRay.dir = pCamera->transform.Forward();
			viewRay.pos = pCamera->transform.position;

			RaycastHit result;
			BlockTrackInfo block;

			if ( Raytracer.Raycast( viewRay, 10.0f, &result, &block, 1|2|4, this ) )
			{
				ItemGenerator.MakeDynamite( result.hitPos + result.hitNormal*2.0f );
			}
		}
		if ( Input::Keydown('N') )
		{
			// == DEBUG ==
			Ray viewRay;
			viewRay.dir = pCamera->transform.Forward();
			viewRay.pos = pCamera->transform.position;

			RaycastHit result;
			BlockTrackInfo block;

			if ( Raytracer.Raycast( viewRay, 10.0f, &result, &block, 1|2|4, this ) )
			{
				ItemGenerator.MakeTestBow( result.hitPos + result.hitNormal*2.0f );
			}
		}
		if ( Input::Keydown('B') )
		{
			// == DEBUG ==
			Ray viewRay;
			viewRay.dir = pCamera->transform.Forward();
			viewRay.pos = pCamera->transform.position;

			RaycastHit result;
			BlockTrackInfo block;

			if ( Raytracer.Raycast( viewRay, 10.0f, &result, &block, 1|2|4, this ) )
			{
				//ItemGenerator.MakeTestBow( result.hitPos + result.hitNormal*2.0f );
				ItemCampfire* temp = new ItemCampfire();
				temp->transform.position = result.hitPos + result.hitNormal*2.0f;
				temp->transform.SetDirty();
			}
		}
		if ( Input::Keydown('H') )
		{
			// == DEBUG ==
			Ray viewRay;
			viewRay.dir = pCamera->transform.Forward();
			viewRay.pos = pCamera->transform.position;

			RaycastHit result;
			BlockTrackInfo block;

			if ( Raytracer.Raycast( viewRay, 10.0f, &result, &block, 1|2|4, this ) )
			{
				//ItemGenerator.MakeTestBow( result.hitPos + result.hitNormal*2.0f );
				DebugSpawnSitter ( result.hitPos + result.hitNormal*2.0f );
			}
		}
	}*/
}


void CAfterPlayer::DoUseCommand ( void )
{
	CGameBehavior* pHitObject = NULL;
	// First check for an actor.
	// Waaaiittt how the hell do we check for an actor or an item? They're all CGameObjects!
	// Well, simple. We just need to give game objects a "layer." With that layer, we can classify things easily.
	// So we raycast from the eye position, and activate whatever is in the way
	Ray viewRay = GetEyeRay();

	RaycastHit result;
	//BlockTrackInfo block;

	CInventory* targetBag = GetInventory();

	// Do the raycast
	Raycaster.Raycast( viewRay, fMaxUseDistance*2.5f, &result, Physics::GetCollisionFilter(Layers::PHYS_BULLET_TRACE,0,31), this );
	// Creat weighted distance
	Vector3d weightedDistance = rhLookAtResult.hitPos - viewRay.pos;
	weightedDistance.z *= 0.5f;
	if (( rhLookAtResult.hit )&&( rhLookAtResult.distance <= fMaxUseDistance || weightedDistance.magnitude() <= fMaxUseDistance ))
	{
		pHitObject = result.pHitBehavior;
		// Get parent object if the object is a rigidbody
		if ( pHitObject && ((pHitObject->layer & Layers::Rigidbody) != 0) ) {
			pHitObject = ((CRigidBody*)result.pHitBehavior)->GetOwner();
		}
		else if ( pHitObject && ((pHitObject->layer & Layers::Hitboxes) != 0) ) {
			pHitObject = ((CRagdollCollision*)result.pHitBehavior)->GetActor();
		}
		
		if ( pHitObject )
		{
			if ( (pHitObject->layer & Layers::Actor) != 0 ) // Looking at an actor
			{
				// Work on the actor
				std::cout << "I think this is what I'm looking for?" << std::endl; //DTAKEOUT
				((CActor*)pHitObject)->OnInteract( this );
			}
			else if ( (pHitObject->layer & Layers::WeaponItem) != 0 ) // Looking at a weaponItem
			{
				// Work on the item
				((CWeaponItem*)pHitObject)->OnInteract( this );
				// Pick it up if possible
				if ( targetBag && targetBag->CanAddItem( (CWeaponItem*)pHitObject ) != -1 )
				{
					targetBag->AddItem( (CWeaponItem*)pHitObject );
				}
			}
		}
	}
	// And that's it. Really.
}
void CAfterPlayer::PickupItemsRadius ( void )
{
	CInventory* targetBag = GetInventory();

	if ( targetBag )
	{
		// Loop through all the items, if they're nearby, then pick them up
		vector<CGameBehavior*>* pNewList = CGameState::Active()->FindObjectsWithLayer( Layers::WeaponItem );

		Vector3d vGrabSource = transform.position + Vector3d( 0,0,pl_race_stats->fStandingHeight*0.7f );
		for ( unsigned int i = 0; i < pNewList->size(); ++i )
		{
			CWeaponItem* pItem = ((CWeaponItem*)((*pNewList)[i]));
			if ( pItem->GetIsSkill() ) continue;
			if (( pItem->GetHoldState() == Item::None )||( pItem->GetHoldState() == Item::Hover ))
			{
				ftype fCurrentDist = (pItem->transform.position-vGrabSource).sqrMagnitude();
				if ( fCurrentDist < (fMaxUseDistance*fMaxUseDistance*1.8f) )
				{
					// Pick it up if possible
					if ( targetBag->CanAddItem( (CWeaponItem*)pItem ) != -1 )
					{
						// If can pick it up, move it closer to the player
						pItem->transform.position -= (pItem->transform.position-vGrabSource).normal() * 35.0f * Time::deltaTime;
						pItem->transform.rotation *= Rotator( (pItem->transform.position-vGrabSource) * Time::deltaTime );
						pItem->transform.SetDirty();
						// At a certain range, add it to the inventory
						if ( fCurrentDist < fMaxUseDistance*1.2f )
						{
							targetBag->AddItem( (CWeaponItem*)pItem );
						}
					}
				}
			}
		}

		delete pNewList;
	}
}


void CAfterPlayer::DoLookAtCommands ( void )
{
	CGameBehavior* pHitObject = NULL;

	// Raycast from eye position
	Ray viewRay = GetEyeRay();

	//RaycastHit result; //rhLookAtResult;
	BlockTrackInfo block;

	// Raycasting
	Raycaster.Raycast( viewRay, pCamera->zFar, &rhLookAtResult, Physics::GetCollisionFilter(Layers::PHYS_BULLET_TRACE,0,31), this );
	TerrainAccess.GetBlockAtPosition( rhLookAtResult, block );
	RaycastHit result = rhLookAtResult;
	// Do the raycast
	Vector3d weightedDistance = rhLookAtResult.hitPos - viewRay.pos;
	weightedDistance.z *= 0.5f;
	//if ( Raytracer.Raycast( viewRay, fMaxUseDistance, &result, &block, 1|2|4 ) )
	if (( rhLookAtResult.hit )&&( rhLookAtResult.distance <= fMaxUseDistance || weightedDistance.magnitude() <= fMaxUseDistance ))
	{
		pHitObject = result.pHitBehavior;
		// Get parent object if the object is a rigidbody
		if ( (pHitObject->layer & Layers::Rigidbody) != 0 ) {
			pHitObject = ((CRigidBody*)result.pHitBehavior)->GetOwner();
		}
		else if ( (pHitObject->layer & Layers::Hitboxes) != 0 ) {
			pHitObject = ((CRagdollCollision*)result.pHitBehavior)->GetActor();
		}

		// Depending on the layer of the hit object, call the lookAt functions
		if ( pHitObject && (pHitObject->layer & Layers::WeaponItem) != 0 ) // Looking at a weaponItem
		{
			if ( pCurrentLookedAt != pHitObject )
			{
				// Set old looked at
				if (( pCurrentLookedAt != NULL )&&( CGameState::Active()->ObjectExists( pCurrentLookedAt ) ))
				{
					// Perform the proper routine
					if ( (pCurrentLookedAt->layer & Layers::WeaponItem) != 0 )
						((CWeaponItem*)pCurrentLookedAt)->OnInteractLookAway( this );
					else if ( (pCurrentLookedAt->layer & Layers::Actor) != 0 )
						((CActor*)pCurrentLookedAt)->OnInteractLookAway( this );
				}
				// Set new looked at
				pCurrentLookedAt = pHitObject;
				// Perform the proper routine
				((CWeaponItem*)pCurrentLookedAt)->OnInteractLookAt( this );
			}
		}
		else if ( pHitObject && (pHitObject->layer & Layers::Actor) != 0 ) // Looking at an actor
		{
			if ( pCurrentLookedAt != pHitObject )
			{
				// Set old looked at
				if (( pCurrentLookedAt != NULL )&&( CGameState::Active()->ObjectExists( pCurrentLookedAt ) ))
				{
					// Perform the proper routine
					if ( (pCurrentLookedAt->layer & Layers::WeaponItem) != 0 )
						((CWeaponItem*)pCurrentLookedAt)->OnInteractLookAway( this );
					else if ( (pCurrentLookedAt->layer & Layers::Actor) != 0 )
						((CActor*)pCurrentLookedAt)->OnInteractLookAway( this );
				}
				// Set new looked at
				pCurrentLookedAt = pHitObject;
				// Perform the proper routine
				((CActor*)pCurrentLookedAt)->OnInteractLookAt( this );
			}
		}
		else if ( pCurrentLookedAt != pHitObject ) // Not looking at anything
		{
			// Set old looked at
			if (( pCurrentLookedAt != NULL )&&( CGameState::Active()->ObjectExists( pCurrentLookedAt ) ))
			{
				// Perform the proper routine
				if ( (pCurrentLookedAt->layer & Layers::WeaponItem) != 0 )
					((CWeaponItem*)pCurrentLookedAt)->OnInteractLookAway( this );
				else if ( (pCurrentLookedAt->layer & Layers::Actor) != 0 )
					((CActor*)pCurrentLookedAt)->OnInteractLookAway( this );
			}
			// Set new looked at
			pCurrentLookedAt = NULL;
		}
	}
	else // Use cast didn't even touch
	{
		// Set old looked at
		if (( pCurrentLookedAt != NULL )&&( CGameState::Active()->ObjectExists( pCurrentLookedAt ) ))
		{
			// Perform the proper routine
			if ( (pCurrentLookedAt->layer & Layers::WeaponItem) != 0 )
				((CWeaponItem*)pCurrentLookedAt)->OnInteractLookAway( this );
			else if ( (pCurrentLookedAt->layer & Layers::Actor) != 0 )
				((CActor*)pCurrentLookedAt)->OnInteractLookAway( this );
		}
		// Set new looked at
		pCurrentLookedAt = NULL;
	}

	// Set camera's focal distance based on the LookAt result
	ftype targetFocalDistance = pCamera->focalDistance;
	if ( rhLookAtResult.hit ) {
		targetFocalDistance = rhLookAtResult.distance;
	}
	else {
		targetFocalDistance = pCamera->zFar * 0.5f;
	}
	pCamera->focalDistance += ( targetFocalDistance - pCamera->focalDistance ) * Time::TrainerFactor( 0.3f );
}


void DebugSpawnSitter( const Vector3d& spawnPosition )
{
	NPC::npcid_t banditId = NPC::Manager->RequestNPC( NPC::npcid_UNIQUE );
	{
		NPC::characterFile_t characterFile;

		CRacialStats rstats;
		rstats.stats = new CharacterStats;
		rstats.SetDefaults();
		characterFile.rstats = &rstats;
		{
			// Set race
			rstats.iRace = CRACE_ELF;
			/*switch ( Random.Next()%4 ) {
			case 0:
				rstats.iRace = CRACE_FLUXXOR; break;
			case 1:
				rstats.iRace = CRACE_ELF; break;
			case 2:
				rstats.iRace = CRACE_KITTEN; break;
			case 3:
				rstats.iRace = CRACE_HUMAN; break;
			}*/
			// Set gender
			rstats.iGender = eCharacterGender(Random.Next()%2);
			// Randomize hairstyle
			rstats.iHairstyle = Random.Next()%3;
			// Set companion colors
			rstats.RerollColors();
			
			// Set facial tattoo
			if ( Random.Next()%3 == 0 )
			{
				CRacialStats::tattoo_t tattoo;
				tattoo.color = Color( Random.Range(0.1f,0.5f),Random.Range(0.1f,0.5f),Random.Range(0.1f,0.5f),1.0f );
				tattoo.mirror = false;
				tattoo.type = TATT_CLANMARK;
				tattoo.pattern = "clan_skullblack";
				if ( Random.Next()%2 == 0 ) {
					tattoo.pattern = "clan_mask2";
				}
				tattoo.projection_angle = Random.Range( 150.0f, 180.0f );
				tattoo.projection_dir = Vector3d( 0,1,0 );
				tattoo.projection_pos = Vector3d( 0,-15,28 ) / 12.0f;
				tattoo.projection_scale = Vector3d(1,1,1) * Random.Range( 0.77f, 0.92f );
				if ( Random.Chance(0.5f) ) tattoo.projection_scale.x *= -1;
				rstats.tattooList.push_back( tattoo );
			}
			rstats.iTattooCount = rstats.tattooList.size();

			// Set name
			rstats.sPlayerName = "Bandit";
			rstats.sLastName = "";
		}

		NPC::sWorldState worldstate;
		{
			characterFile.worldstate = &worldstate;
			worldstate.mFocus = NPC::AIFOCUS_LuaGeneral; // should be bandit focus, so can turn to BanditRunner faction when 
			worldstate.mFocusName = "sitter";
			worldstate.travelDirection = Vector3d( 1,0,0 );
			bool success = false;
			while ( !success ) {
				worldstate.worldPosition = NPC::Spawner->GetSpawnPosition( spawnPosition, success );//= pPlayerStats->vPlayerInitSpawnPoint;
			}
			//worldstate.worldPosition = spawnPosition;
			worldstate.partyHost = banditId;

			worldstate.mFaction = NPC::FactionBanditRunner;
		}

		NPC::sOpinions opinions;
		characterFile.opinions = &opinions;

		NPC::sPreferences prefs;
		characterFile.prefs = &prefs;

		// Save generated stats
		NPC::CZonedCharacterIO io;
		io.CreateCharacterFile( banditId, characterFile );
		// Clean up
		delete rstats.stats;
	}

	// Create bandit :D
	NPC::CNpcBase* bandit = (NPC::CNpcBase*) NPC::Manager->SpawnNPC( banditId );
	bandit->transform.position = spawnPosition;
	bandit->transform.SetDirty();
	// Now, change bandit's equipment 
}


void CAfterPlayer::SetCanSprint ( bool enableSprint ) 
{
	bSprintDisabled = !enableSprint;
}
// Sets the movement scaling.
void CAfterPlayer::SetMovementSpeedScale ( float nSpeedScale )
{
	fMovementMultiplier = nSpeedScale;
}

// Update terrain sample
void CAfterPlayer::UpdateTerrainSample ( void )
{
	//bInActiveArea = Zones.IsActiveArea( transform.position );
	bInActiveArea = Zones.IsCollidableArea( transform.position );

	// Need to sample the nearby areas to get a 3d normal
	vTerrainNormal = Vector3d( 0,0,0.01f );
	if ( CVoxelTerrain::GetActive() )
	{
		for ( char i = -3; i <= 3; ++i )
		{
			for ( char j = -3; j <= 3; ++j )
			{
				for ( char k = -3; k <= 3; ++k )
				{
					if ( Vector3d(ftype(i),ftype(j),ftype(k)).sqrMagnitude() > sqr(4) )
						continue;

					ushort block = Zones.GetBlockAtPosition( transform.position + Vector3d( i*2.0f,j*2.0f,k*2.0f + 0.3f ) ).block;
					if (( block == Terrain::EB_NONE )||( block == Terrain::EB_WATER ))
					{
						vTerrainNormal += Vector3d( i,j,k );
					}
				}
			}
		}
		// While we're at it, get the block under the player's feet
		//iTerrainCurrentBlock = CVoxelTerrain::GetActive()->GetBlockAtPosition( transform.position + Vector3d( 0,0,-1.0f ) ).block;
		iTerrainCurrentBlock = Zones.GetBlockAtPosition( transform.position + Vector3d( 0,0,-1.0f ) ).block;
	}
	vTerrainNormal = vTerrainNormal.normal(); // Normalize the normal

	// First need the tangent of the slope
	{
		Vector3d up = Vector3d( 0,0,1 );
		Vector3d horizontal = vTerrainNormal.cross( up );
		vTerrainTangent = horizontal.cross( vTerrainNormal );
		if ( vTerrainTangent.z > 0 )
		{
			vTerrainTangent = -vTerrainTangent;
		}
	}
	vTerrainTangent = vTerrainTangent.normal(); // Normalize the tangent

	//
	m_motion->vTerrainTangent = vTerrainTangent;
	m_motion->iTerrainCurrentBlock = iTerrainCurrentBlock;
}


// Returns a pointer to the shield. Returns NULL if no shield.
SkillShield*	CAfterPlayer::GetShield ( void ) 
{
	return m_shield;
}
void	CAfterPlayer::SetShield ( SkillShield* n_shield )
{
	m_shield = n_shield;
}
// Returns a list of all the casting spells in a vector. Default implemenation returns a list of empty skills.
std::vector<CSkill*>	CAfterPlayer::GetCastingSkills ( void ) 
{
	std::vector<CSkill*> skills;
	// Loop through all the hands and get the skills
	short handCount = pMyInventory->GetCurrentEquippedCount();
	for ( short i = 0; i < handCount; ++i ) 
	{
		CWeaponItem* t_itemInHand = pMyInventory->GetCurrentEquipped(i);
		
		//If the hand is empty, don't do anything
		if (t_itemInHand == NULL)
			continue;
		// Check if equipped is a skill
		if ( t_itemInHand->GetIsSkill() ) {
			skills.push_back( (CSkill*)t_itemInHand );
		}
	}
	return skills;
}

//  StunWithAnimation()
// Stuns the player with the animation. Will stun for entire duration of animation.
// Returns true when animation is found.
bool	CAfterPlayer::StunWithAnimation ( const string& nStunName, const ftype nTimescale )
{
	bool hasStun = false;
	// Do AI stun
	//
	CAnimation* anim = model->GetAnimationState();
	CAnimAction* stagger;
	if ( stagger = anim->FindAction( nStunName ) ) {
		if ( !stagger->isPlaying ) {
			anim->Play( nStunName );

			stagger->framesPerSecond = 30.0f * nTimescale;

			hasStun = true;

			m_motion->fStunTimer = (stagger->GetLength() / stagger->framesPerSecond);
		}
	}
	//
	if ( hasStun ) {
		//m_queuedMoveType	= (stateFunc_t)&CMccPlayer::mvt_Stunned;	
		m_motion->SetMovementModeQueued( NPC::MOVEMENT_STUN_GENERAL );
	}
	return hasStun;
}

// ===Callbacks===
void	CAfterPlayer::OnDamaged	( Damage const& hitDamage, DamageFeedback* dmgFeedback )
{
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
	// For physical damage (skip if reflect damage)
	if ( ((finalDamage.type & DamageType::Physical) || !(finalDamage.type & DamageType::Magical)) && (!(finalDamage.type & DamageType::Reflect)) )
	{
		bool canBlock = true;
		if ( bIsStunned ) {
			canBlock = false;
		}
		else {
			Matrix4x4 rotMatrix;
			rotMatrix.setRotation( vPlayerRotation );
			if ( finalDamage.direction.dot( rotMatrix*Vector3d::forward ) > -0.1f ) {
				canBlock = false;
			}
		}
		if ( canBlock )
		{
			// Run the damage through the blocking calculations (get stances)
			for ( ushort i = 0; i < pl_race_stats->iWieldCount; ++i )
			{
				CWeaponItem* currentEquipped = pMyInventory->GetCurrentEquipped(i);
				if ( currentEquipped && currentEquipped->GetStance() == Item::StanceDefense ) {
					defenseCount += 1;
				}
			}
			// If definitely defending...
			if ( defenseCount >= 1 )
			{
				// Run the damage through the weapons first
				for ( ushort i = 0; i < pl_race_stats->iWieldCount; ++i )
				{
					CWeaponItem* currentEquipped = pMyInventory->GetCurrentEquipped(i);
					if ( currentEquipped ) {
						currentEquipped->OnBlockAttack( finalDamage ); // If parrying, this will set damage to zero.
					}
				}
				// If still dealing damage...
				if ( finalDamage.amount > FTYPE_PRECISION )
				{
					// There are 3 types of blocks based on how successful the block was. Figure out what type of block is being used.
					int		blockType;
					Vector3d swordVector = Vector3d( (ftype)cos(degtorad(vAimingArc.z)),(ftype)sin(degtorad(vAimingArc.z)),vAimingArc.w ).normal();
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
					// Punch view
					PunchView( Vector3d( Random.PointOnUnitCircle() ) * sqr(finalDamage.amount*0.32f) );
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
							std::cout << "...excellent block reflect..." << std::endl;
							((CCharacter*)finalDamage.actor)->ApplyStun("stun_stagger_twist",1.0);
							std::cout << "excellent block reflect done" << std::endl;
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
							fScreenBlurAmount += 0.2f;
						}
						// Don't move back too much
						ApplyLinearMotion( Vector3d( -1.5f,0,0 ), 0.2f );
						// Perform a ministun on the source
						if ( finalDamage.actor && finalDamage.actor->IsCharacter() ) {
							std::cout << "...good block reflect..." << std::endl;
							((CCharacter*)finalDamage.actor)->ApplyStun("hold_oneblade_01_ministun-0-4",1.0);
							std::cout << "good block reflect done" << std::endl;
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
							fScreenBlurAmount += 0.5f;
						}
						// Fall back a full foot
						ApplyLinearMotion( Vector3d( -2.0f,0,0 ), 0.3f );
						break;
					}
				}
				else
				{	// If not dealing damage, means that parried. Deal pure stun to the damage source
					if ( finalDamage.actor ) {
						std::cout << "...parry reflect..." << std::endl;
						Damage stunDamage;
						stunDamage.type = DamageType::Reflect;
						stunDamage.amount = 0;
						stunDamage.stagger_chance = 999999;
						finalDamage.actor->OnDamaged( stunDamage );
						std::cout << "parry reflect done" << std::endl;
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

	// Get movement vs facing to do slipping directions
	ftype forwardAmount = 0;
	ftype movementSpeed = 0;
	if ( m_motion->m_rigidbody )
	{
		Matrix4x4 rotMatrix;
		rotMatrix.setRotation( vPlayerRotation );
		Vector3d facingDirection = rotMatrix*Vector3d::forward;
		Vector3d velocity = m_motion->m_rigidbody->GetVelocity();
		movementSpeed = velocity.magnitude();
		forwardAmount = facingDirection.dot( velocity/movementSpeed );
	}

	// Perform damage calculations
	//charHealth -= finalDamage.amount;
	stats.fHealth -= finalDamage.amount;
	// Knock the view
	if ( !(finalDamage.type & DamageType::Fall) || !(finalDamage.type & DamageType::Burn) ) {
		PunchView( Vector3d( Random.PointOnUnitCircle() ) * sqr(finalDamage.amount*0.24f) );
	}
	// Slightly blur the screen
	//fScreenBlurAmount += (finalDamage.amount/charHealth.GetMax())*1.2f;
	fScreenBlurAmount += (finalDamage.amount/stats.fHealthMax)*1.2f;
	// Knock the breath out when falling
	if ( finalDamage.type & DamageType::Fall ) {
		stats.fStamina -= finalDamage.amount * 0.25f;
	}

	// Remove stun chances from certain type of damage
	if ( finalDamage.type & DamageType::Fall || finalDamage.type & DamageType::Reflect ) {
		finalDamage.stagger_chance = 0;
	}

	// Do stuns if haven't killed
	if ( stats.fHealth > 0 )
	{
		if ( Random.Chance(finalDamage.stagger_chance) )
		{
			// Do AI stun
			if ( StunWithAnimation( "stun_stagger_hard" ) ) {
				fScreenBlurAmount += 0.2f;
			}
			//
		}
		// Stun the player otherwise (if high enough damage)
		else if ( finalDamage.amount > stats.fHealthMax*0.3f )
		{
			if ( finalDamage.type & DamageType::Fall ) {
				if ( !(finalDamage.type & DamageType::Drown) )
				{
					if ( (forwardAmount > 0) || (movementSpeed < 4) ) {
						// Do fall hard stun
						if ( StunWithAnimation( "fall_hard" ) ) {
							// Blur the screen hardcore
							fScreenBlurAmount += 0.6f;
						}
					}
					else {
						// Do fall backwards
						//m_queuedMoveType	= (stateFunc_t)&CMccPlayer::mvt_FellOnBackStart;
						m_motion->SetMovementModeQueued( NPC::MOVEMENT_STUN_FALL_BACKWARDS );
						// Blur the screen hardcore
						fScreenBlurAmount += 0.6f;
					}
				}
				else
				{
					// Do fall hard stun
					StunWithAnimation( "fall_hard" );
					fScreenBlurAmount += 0.7f;
					model->BlendToRagdoll( 0 );
				}
			}
		}
	}
	// If this damage killed, set "kill" damage
	else
	{
		killDamage = hitDamage;

		// If not bleeding, go to bleeding state
		if ( !bBleeding )
		{
			bBleeding = true;
			fBleedDamage = std::max<ftype>( 0, hitDamage.amount*0.5f - fBleedDamageThreshold );
			fBleedTime = 0;

			// Go to bleeding out
			std::cout << "BLEEDING OUT" << std::endl;
			//m_NextMoveType	= (stateFunc_t)&CMccPlayer::mvt_Saviorize;
			m_motion->SetMovementModeNext( NPC::MOVEMENT_SAVIORIZE );

			// Stun the character with fall down
			model->PlayScriptedAnimation( "falldown_idle_1",30.0f );
			StunWithAnimation( "falldown_start" );
		}
		else
		{
			// Add incoming damage to bleed damage
			fBleedDamage += hitDamage.amount;
		}

		if ( fBleedDamage > fBleedDamageThreshold )
		{
			// Hit damage threshold, time to die.
			//bBleeding = false;
		}
		else
		{
			// Not above damage threshold, so don't die.
			stats.fHealth = 1.0f;
		}
		// Play death animation
		/*{
			string deadAnim = "die_forward";
			if ( Random.Next()%2 == 0 ) {
				deadAnim = "die_backward";
			}
			CAnimation* anim = model->GetAnimationState();
			CAnimAction* death;
			if ( death = anim->FindAction( deadAnim ) ) {
				death->end_behavior = 1;
				death->layer = 6;
				anim->Play( deadAnim );
			}
		}*/
	}
	// Do hurt overlays and hud effects
	if ( finalDamage.type & DamageType::Fall && finalDamage.amount > 2.0f )
	{
		Ploverlay_Hurt01* overlay = new Ploverlay_Hurt01;
		overlay->mHurtValue = Math.Clamp( finalDamage.amount*0.2f, 0.1f, 1.4f );
		overlay->RemoveReference();

		Plhud3d_Hurt01* hurtRing = new Plhud3d_Hurt01 ( finalDamage.amount, Rotator() );
		hurtRing->RemoveReference();
	}
	else
	{
		Rotator hurtAngle;
		if ( m_motion )
		{
			hurtAngle.RotationTo( Vector3d( 0,-1,0 ), ( finalDamage.source - (transform.position + Vector3d(0,0,m_motion->fPlayerHeight*0.92f)) ).normal() );
		}
		else
		{
			hurtAngle.RotationTo( Vector3d( 0,-1,0 ), finalDamage.direction.normal() );
		}
		Plhud3d_Hurt01* hurtRing = new Plhud3d_Hurt01 ( finalDamage.amount, hurtAngle );
		hurtRing->RemoveReference();
	}

	// Perform hurt dialogue
	if ( finalDamage.amount > 2.0f )
	{
		// Don't do sound for fall damage
		if ( (finalDamage.type & DamageType::Fall) == 0 )
		{
			DoSpeech( NPC::SpeechType_Hurt );
		}
	}
}
// Apply stun. Returns true on successful stun. Ignores the second argument.
bool CAfterPlayer::ApplyStun ( const char* n_stunname, const ftype n_stunlength )
{
	return StunWithAnimation( n_stunname );
}

void	CAfterPlayer::OnDealDamage ( Damage& hitDamage, CActor* receivingCharacter )
{
	// For now, don't touch the hitDamage.
	// We want to use the receivingCharacter and show their health bar.

	pl_hud->UpdateKillTarget( receivingCharacter );
}


void	CAfterPlayer::OnEquip ( CItemBase* pItem )
{
	if ( pItem == NULL ) {
		return;
	}
	

	if ( pItem->GetBaseClassName() != "WearableItem" )
	{
		pl_inventoryGUI->SetDrawName (true);
	}
	else
	{
		switch ( ((CWeaponItem*)pItem)->GetItemType() ) {
			case WeaponItem::TypeArmorBoots:
				pl_race_stats->sGearRightfoot = ((CWearableItem*)pItem)->GetPlayerModel();
				((CMccCharacterModel*)model)->SetPartHidden( CMccCharacterModel::PART_FEET, ((CWearableItem*)pItem)->HidesBody() );
				((CMccCharacterModel*)model)->SetBootsModel( pl_race_stats->sGearRightfoot );
				break;
			case WeaponItem::TypeArmorChest:
				pl_race_stats->sGearChest = ((CWearableItem*)pItem)->GetPlayerModel();
				((CMccCharacterModel*)model)->SetPartHidden( CMccCharacterModel::PART_TORSO, ((CWearableItem*)pItem)->HidesBody() );
				((CMccCharacterModel*)model)->SetShirtModel( pl_race_stats->sGearChest );
				break;
			case WeaponItem::TypeArmorGreaves:
				pl_race_stats->sGearLegs = ((CWearableItem*)pItem)->GetPlayerModel();
				((CMccCharacterModel*)model)->SetPartHidden( CMccCharacterModel::PART_HIPS, ((CWearableItem*)pItem)->HidesBody() );
				((CMccCharacterModel*)model)->SetShortsModel( pl_race_stats->sGearLegs );
				break;
			case WeaponItem::TypeArmorHead:
				pl_race_stats->sGearHead = ((CWearableItem*)pItem)->GetPlayerModel();
				((CMccCharacterModel*)model)->SetPartHidden( CMccCharacterModel::PART_HEAD, ((CWearableItem*)pItem)->HidesBody() );
				((CMccCharacterModel*)model)->SetGlassesModel( pl_race_stats->sGearHead );
				break;
			case WeaponItem::TypeArmorShoulder:
				pl_race_stats->sGearShoulder = ((CWearableItem*)pItem)->GetPlayerModel();
				((CMccCharacterModel*)model)->SetPartHidden( CMccCharacterModel::PART_SHOULDERS, ((CWearableItem*)pItem)->HidesBody() );
				((CMccCharacterModel*)model)->SetShoulderModel( pl_race_stats->sGearShoulder );
				((CMccCharacterModel*)model)->SetHair( ((CWearableItem*)pItem)->HidesHair() ? -1 : pl_race_stats->iHairstyle );
				break;
			default:
				Debug::Console->PrintError( "Unhandled armor type equipped!" );
				break;
		}
	}
}
void	CAfterPlayer::OnUnequip	(CItemBase* pItem)
{
	if ( pItem->GetBaseClassName() != "WearableItem" )
	{
		;
	}
	else
	{
		switch ( ((CWeaponItem*)pItem)->GetItemType() ) {
			case WeaponItem::TypeArmorBoots:
				pl_race_stats->sGearRightfoot = "";
				((CMccCharacterModel*)model)->SetPartHidden( CMccCharacterModel::PART_FEET, false );
				((CMccCharacterModel*)model)->SetBootsModel( "" );
				break;
			case WeaponItem::TypeArmorChest:
				pl_race_stats->sGearChest = "";
				((CMccCharacterModel*)model)->SetPartHidden( CMccCharacterModel::PART_TORSO, false );
				((CMccCharacterModel*)model)->SetShirtModel( "wraps" );
				break;
			case WeaponItem::TypeArmorGreaves:
				pl_race_stats->sGearLegs = "";
				((CMccCharacterModel*)model)->SetPartHidden( CMccCharacterModel::PART_HIPS, false );
				((CMccCharacterModel*)model)->SetShortsModel( "shorts" );
				break;
			case WeaponItem::TypeArmorHead:
				pl_race_stats->sGearHead = "";
				((CMccCharacterModel*)model)->SetPartHidden( CMccCharacterModel::PART_HEAD, false );
				((CMccCharacterModel*)model)->SetGlassesModel( "" );
				break;
			case WeaponItem::TypeArmorShoulder:
				pl_race_stats->sGearShoulder = "";
				((CMccCharacterModel*)model)->SetPartHidden( CMccCharacterModel::PART_SHOULDERS, false );
				((CMccCharacterModel*)model)->SetShoulderModel( "" );
				((CMccCharacterModel*)model)->SetHair( pl_stats->race_stats->iHairstyle );
				break;
			default:
				Debug::Console->PrintWarning( "Unhandled armor type unequipped!" );
				break;
		}
	}	
}

void CAfterPlayer::OpenInventoryForSwapping ( CInventory* nTargetInventory )
{
	std::cout << "NYYYIIIGGAAHHH" << std::endl;
	pl_chestGUI->OpenInventory (nTargetInventory);
	pl_chestGUI->SetVisibility (true);
	pl_inventoryGUI->SetVisibility (true);
	bool yes = true;
	pl_inventoryGUI->SetInventoryVisibility (yes);
}


void CAfterPlayer::InitAreaInfo ( void )
{
	iCurrentRegion = (uint32_t) -1;
	iLastRegion = (uint32_t) -1;
	fRegionTime = 0.0f;
}

#include "after/states/world/ProvinceManager.h"
void CAfterPlayer::UpdateAreaInfo ( void )
{
	if ( !World::ProvinceManager ) {
		return;
	}

	uint32_t nextRegion = World::ProvinceManager->GetRegion( rangeint(transform.position.x/64.0f), rangeint(transform.position.y/64.0f) );

	bool hadRegionChange = false;

	if ( nextRegion != iCurrentRegion ) {
		if ( nextRegion == iLastRegion ) {
			fRegionTime += Time::deltaTime; // Only swap region after a time
			if ( fRegionTime > 10.0f ) {
				iLastRegion = iCurrentRegion;
				iCurrentRegion = nextRegion;
				hadRegionChange = true;
			}
		}
		else {
			// Save prev region
			iLastRegion = iCurrentRegion;
			iCurrentRegion = nextRegion; // Swap region if it wasn't a previous region
			hadRegionChange = true;
		}
	}
	else {
		fRegionTime = 0.0f; // Reset timer
	}

	if ( hadRegionChange ) {
		// Print out new region's name
		char regionName [256];
		if ( World::ProvinceManager->GetProvinceName( iCurrentRegion,regionName ) ) {
			//cout << "new region: " << iCurrentRegion << endl;
			//cout << " prev regi: " << iLastRegion << endl;
			new Plhud_AreaNotifier ( regionName, "", "province" );
		}
		else {
			std::cout << "Could not get region's next name!" << std::endl;
		}
	}
}

void CAfterPlayer::StartConversation (string conversation, vector<CCharacter*> person)
{
	vector<CCharacter*> temp;
	temp.push_back(this);
	for (uint i = 0; i < person.size(); i++)
		temp.push_back(person[i]);
	
	pl_dialogueGUI->OpenConversation (conversation, temp);
	pl_dialogueGUI->SetActive(true);
}

void CAfterPlayer::PassSelected (CWeaponItem* selected)
{
	//if (selected->GetOwner() != this)
		//selected->SetOwner (this);
	pl_inventoryGUI->ReceiveSelected(selected);
}

void CAfterPlayer::TurnOffInventoryGUI (void)
{
	//pl_inventoryGUI->SetVisibility (false);
	pl_gui_menu->TurnOffInventory ();
}


// OnGainExperience doesn't actually mean that experience has been gained, similar to OnDamaged.
//  Based on the input's discipline, you can give certain experience types a special effects.
// The default behavior is to just add the experience to the stats.
void CAfterPlayer::OnGainExperience ( const Experience& incomingExperience )
{
	Experience finalExperience = debuffs.OnGainExperience( incomingExperience );
	stats.fExperience += finalExperience.amount;

	int t_targetDiscipline = int(finalExperience.discipline) + int(finalExperience.subdiscipline);
	if ( t_targetDiscipline >= 0 && t_targetDiscipline < 32 ) {
		pl_stats->skillpoints_experience[t_targetDiscipline] += finalExperience.amount;
	}
}
