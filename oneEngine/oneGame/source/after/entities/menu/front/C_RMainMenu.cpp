
//
// -- C_RMainMenu --
// Class for the main menu
// Manages camera angles, time of day in the background
// CMainMenu is outclassed by this class. Not that CMainMenu ever worked in the first place.
//

#include "C_RMainMenu.h"

#include "renderer/camera/CCamera.h"
#include "after/entities/world/environment/DayAndNightCycle.h"
#include "after/entities/world/environment/CloudSphere.h"

#include "core/time/time.h"
#include "core/math/Math.h"
#include "core/math/random/Random.h"

#include "core/settings/CGameSettings.h"

//#include "gmsceneMainGame.h"

#include "after/states/CharacterStats.h"
#include "after/states/player/CPlayerStats.h"
#include "after/entities/character/npc/CNpcBase.h"
#include "after/entities/character/npc/zoned/CZonedCharacter.h"
#include "after/interfaces/io/CZonedCharacterIO.h"

//#include "boost/filesystem.hpp"

//#include "CTerrainCollisionLoader.h"
#include "engine-common/lua/CLuaController.h"

#include "engine/audio/CAudioInterface.h"

#include "renderer/texture/CRenderTexture.h"
#include "renderer/object/sprite/CSprite.h"
#include "renderer/object/screen/CScreenFade.h"

#include "engine/state/CGameState.h"

#include "after/interfaces/world/CWorldCartographer.h"
#include "after/states/world/CMCCRealm.h"

#include "after/interfaces/CAfterCharacterAnimator.h"

#include "after/terrain/generation/CWorldGenerator.h"
#include "after/terrain/generation/CWorldGen_Terran.h"

#include "after/terrain/VoxelTerrain.h"
#include "../COctreePauseloader.h"
#include "after/terrain/system/TerrainRenderer.h"
//#include "COctreeRenderer.h"

#include "after/entities/gametype/gametypeMenu.h"
#include "after/entities/gametype/gametypeNotDying.h"

#include "after/states/CWorldState.h"
#include "after/entities/world/environment/CEnvironmentEffects.h"
#include "after/entities/character/npc/zoned/CZonedCharacterController.h"
#include "after/entities/world/CNpcSpawner.h"

#include "renderer/material/glMaterial.h"

#include <filesystem>
namespace fs = std::tr2::sys;

CSoundBehavior* mb = NULL;

C_RMainMenu::C_RMainMenu ( CGameType* n_gametype )
	: CGameBehavior()
{
	gui = new CDuskGUI ();
	CreateGUIElements();

	pCamera = new CCamera();
	
	//CEnvironmentEffects* n_env_effects = new CEnvironmentEffects();
	//n_env_effects->active = false;
	m_gametype = new gametypeMenu( n_gametype ); // Create menu gametype
	pDaycycle	= m_gametype->m_worldeffects->pWorldDaycycle;
	pMooncycle	= m_gametype->m_worldeffects->pWorldMooncycle;
	pCloudcycle	= m_gametype->m_worldeffects->pWorldCloudsphere;

	pl_stats = new CPlayerStats();
	pl_stats->SetDefaults();

	m_playercharacter = NULL;
	p_char_creator = NULL;
	p_model	= NULL;

	tRealmSelect.terra_rt = new CRenderTexture( RGBA8, 512,512, Repeat, Repeat );
	tRealmSelect.terra_sprite = new CSprite();
	tRealmSelect.terra_sprite->GetMaterial()->setTexture( 0, tRealmSelect.terra_rt );
	tRealmSelect.terra_sprite->scale = Vector2d( 8,8 );
	tRealmSelect.terra_sprite->visible = false;
	//tRealmSelect.terra_sprite->position = Vector2d( 

	pPauseLoader = new COctreePauseloader();
	pPauseLoader->active = false;
	pPauseLoader->SetVisible( false );

	iMenuState = M_load;
	//mb = Audio.playSound( "Music.MainMenu" );

	// Load up lua
	m_environment = "__menu_object_";
	m_behavior = "menus/rmain_menu.lua";
	Lua::Controller->LoadLuaFile( m_behavior.c_str(), m_environment.c_str(), m_environment.c_str() );
	Lua::Controller->SetEnvironment( m_environment.c_str() );
	Lua::Controller->ResetEnvironment();
}

C_RMainMenu::~C_RMainMenu ( void )
{
	if ( CGameState::Active()->ObjectExists( mb ) ) {
		delete_safe( mb );
	}

	delete_safe( gui );
//
//	delete_safe( pCloudcycle );
//	delete_safe( pDaycycle );
//	delete_safe( pMooncycle );
	delete_safe( pCamera );

	delete_safe( tMainMenu.title );

	delete_safe( tRealmSelect.terra_rt );
	delete_safe( tRealmSelect.terra_sprite );
	delete_safe( tRealmSelect.main_planetoid );

	if ( pl_stats ) {
		delete pl_stats->stats;
		delete pl_stats;
		pl_stats = NULL;
	}
	/*if ( p_model ) {
		delete p_model;
		p_model = NULL;
	}*/

	delete_safe( pPauseLoader );

	Lua::Controller->FreeLuaFile( m_behavior.c_str(), m_environment.c_str() );
}


void C_RMainMenu::Update ( void )
{
	eMenuState prevMenuState = iMenuState;
	pCloudcycle->SetCloudDensity( -0.2f );
	// Do menu states
	switch ( iMenuState )
	{
		case M_load:		iMenuState = stateLoad();	break;
		case M_intro:		iMenuState = stateIntro();	break;
		case M_mainmenu:	iMenuState = stateMainMenu();	break;
		case M_options:		iMenuState = stateOptions();	break;
		case M_realmselect:	iMenuState = stateRealmSelect();	break;
		case M_blendtocharselect:	iMenuState = stateBlendToCharSelect();	break;
		case M_charselect:			iMenuState = stateCharSelect();	break;
		case M_blendtogameplay:		iMenuState = stateBlendToGameplay();	break;
		case M_blendtocharcreation:	iMenuState = stateBlendToCharCreation();	break;
		case M_charcreation:		iMenuState = stateCharCreation();	break;
		case M_blendtogameplaystart:iMenuState = stateBlendToGameplayNewChar(); break;
	};
	pMooncycle->SetTimeOfCycle( stateCurrent.timeOfDay );
	pCloudcycle->SetTimeOfDay( stateCurrent.timeOfDay );
	pDaycycle->SetTimeOfDay( stateCurrent.timeOfDay );
	pDaycycle->SetSpaceEffect( stateCurrent.spaceEffect );
	if ( tRealmSelect.main_planetoid ) {
		tRealmSelect.main_planetoid->GetMaterial()->m_diffuse.alpha = Math.Clamp( (stateCurrent.spaceEffect-0.5f) * 3, 0, 1 );
		if ( tRealmSelect.main_planetoid->GetMaterial()->m_diffuse.alpha <= 0.02 ) {
			//tRealmSelect.main_planetoid->visible = false;
			tRealmSelect.main_planetoid->SetVisibility( false );
		}
		else {
			//tRealmSelect.main_planetoid->visible = true;
			tRealmSelect.main_planetoid->SetVisibility( true );
		}
	}

	// Camera movement
	gui->SetVisible( true );
	if ( iMenuState != prevMenuState ) {
		statePrevious = stateCurrent;
		fLerpValue = 0.0f;
	}
	else {
		if ( p_char_creator && p_char_creator->DoCameraOverview() )
		{
			p_char_creator->GetCameraOverview( pCamera );
			if ( !bSubstateBlend )
			{
				bSubstateBlend = true;
				statePrevious = stateCurrent;
				fLerpValue = 0.0f;
			}
			stateTarget.cameraPos = pCamera->transform.position;
			stateTarget.cameraRot = pCamera->transform.rotation;
			stateTarget.cameraFoV = pCamera->fov;

			gui->SetVisible( false );
		}
		else {
			if ( bSubstateBlend )
			{
				bSubstateBlend = false;
				statePrevious = stateCurrent;
				fLerpValue = 0.0f;
			}
		}
		// 
		{	// Output positions, but clamp to stop the jitter on the last frame
			Real t_lerpValue = Math.Clamp( fLerpValue,0,1 );
			stateCurrent.cameraPos = statePrevious.cameraPos.lerp( stateTarget.cameraPos, Math.Smoothlerp(t_lerpValue) );
			stateCurrent.cameraRot = statePrevious.cameraRot.Slerp( stateTarget.cameraRot, Math.Smoothlerp(t_lerpValue) );
			stateCurrent.cameraFoV = Math.Lerp( Math.Smoothlerp(t_lerpValue), statePrevious.cameraFoV, stateTarget.cameraFoV );
			stateCurrent.cameraRoll = Math.Lerp( Math.Smoothlerp(t_lerpValue), statePrevious.cameraRoll, stateTarget.cameraRoll );

			if ( m_gametype->m_worldeffects->active == false ) {
				stateCurrent.timeOfDay = Math.Lerp( Math.Smoothlerp(t_lerpValue), statePrevious.timeOfDay, stateTarget.timeOfDay );
				stateCurrent.spaceEffect = Math.Lerp( Math.Smoothlerp(t_lerpValue), statePrevious.spaceEffect, stateTarget.spaceEffect );
			}

			pCamera->transform.position = stateCurrent.cameraPos;
			pCamera->transform.rotation = Vector3d( stateCurrent.cameraRoll, 0, 0 );
			pCamera->transform.rotation = stateCurrent.cameraRot * pCamera->transform.rotation;
			pCamera->fov = stateCurrent.cameraFoV;
		}
	}

	// Create model
	if ( p_model == NULL || m_playercharacter == NULL )
	{
		tCharModel.glanceAngle = Vector3d( 0,0,0 );
		tCharModel.glanceFinalAngle = Vector3d( 0,0,0 );
		tCharModel.glanceShiftTime = 0;
		tCharModel.lookatCameraOffset = Vector3d( 0,0,0 );
		tCharModel.lookatCameraFinalOffset = Vector3d( 0,0,0 );
	}
	else
	{
		if ( iMenuState >= M_blendtocharselect ) {
			//p_model->SetVisibility( p_model->GetVisibility() );
		}
		else {
			if ( p_model ) {
				p_model->SetVisibility( false );
			}
		}

		// Do the glance effect
		tCharModel.glanceShiftTime -= Time::deltaTime;
		if ( tCharModel.glanceShiftTime < 0 ) {
			tCharModel.glanceAngle += Vector3d( 0,random_range(-20.0f,20.0f),random_range(-20.0f,20.0f) );
			if ( tCharModel.glanceAngle.magnitude() > 40 ) {
				tCharModel.glanceAngle = tCharModel.glanceAngle.normal() * 40;
			}
			if ( tCharModel.glanceAngle.z < -10 ) {
				tCharModel.glanceAngle.z = -10;
			}
			if ( tCharModel.glanceAngle.z > 20 ) {
				tCharModel.glanceAngle.z = 20;
			}
			tCharModel.glanceShiftTime = random_range( 0.2f, 3.0f );

			if ( iMenuState != M_charcreation )
			{
				if ( rand()%6 == 0 )
				{
					Matrix4x4 rotMatx;
					rotMatx.setRotation( pCamera->transform.rotation );
					tCharModel.lookatCameraOffset = rotMatx * Vector3d( 0, tCharModel.glanceAngle.y, tCharModel.glanceAngle.z*0.3f - 1.1f ) * 0.2f;
				}
			}
		}
		else {
			tCharModel.glanceAngle += Vector3d( 0,random_range(-4.0f,4.0f),random_range(-4.0f,4.0f) ) * Time::deltaTime;
		}
		tCharModel.glanceFinalAngle = tCharModel.glanceFinalAngle.lerp( tCharModel.glanceAngle, Time::TrainerFactor( 0.3f ) );
		tCharModel.lookatCameraFinalOffset = tCharModel.lookatCameraFinalOffset.lerp( tCharModel.lookatCameraOffset, Time::deltaTime * 0.01f );

		// Set the model angles and look places
		if ( p_model ) {
			p_model->SetEyeRotation( Rotator( tCharModel.glanceFinalAngle ) );
			p_model->SetLookAtPosition( pCamera->transform.position + tCharModel.lookatCameraOffset );
			//p_model->SetLookAtPosition( pCamera->transform.position );
		}
	}

	if ( !gui->GetMouseInGUI() && (!p_char_creator || (p_char_creator && !p_char_creator->gui->GetMouseInGUI() && !p_char_creator->DoCameraOverview())) )
	{
		//if ( (CInput::mouse[CInput::MBLeft] && CInput::mouse[CInput::MBRight])||(CInput::mouse[CInput::MBMiddle])||(CInput::mouse[CInput::MBRight]&&CInput::Key(Keys.Alt)) ) {
		if ( CInput::Mouse( CInput::MBLeft ) )
		{
			Matrix4x4 rotMatx;
			rotMatx.setRotation( pCamera->transform.rotation );
			tCharModel.lookatCameraOffset -= rotMatx * Vector3d( 0, (ftype)CInput::DeltaMouseX(), (ftype)CInput::DeltaMouseY() ) * 0.02f;
		}
	}
	else if ( p_char_creator && p_char_creator->DoCameraOverview() ) {
		tCharModel.lookatCameraOffset = Vector3d::zero;
	}

	if ( p_model && pl_stats && !p_char_creator )
	{
		/*if ( pl_stats->race_stats->iRace == CRACE_FLUXXOR ) {
			p_model->SetMoveAnimation( "idle_relaxed_hover_01" );
		}
		else if ( pl_stats->race_stats->iRace == CRACE_HUMAN ) {
			p_model->SetMoveAnimation( "idle_relaxed_01" );
		}
		else if ( pl_stats->race_stats->iRace == CRACE_KITTEN ) {
			p_model->SetMoveAnimation( "idle_relaxed_01" );
		}
		else {
			p_model->SetMoveAnimation( "idle" );
		}*/
		static CAfterCharacterAnimator anim;
		anim.m_model = p_model;
		anim.m_combat_info = NULL;
		anim.m_inventory = NULL;
		anim.m_race_stats = pl_stats->race_stats;
		anim.m_stats = pl_stats->stats;
		pl_stats->stats->fMana = pl_stats->stats->fManaMax;

		anim.SetMoveAnimation( NPC::MoveAnimWalkIdle );
		anim.DoMoveAnimation();
	}
}

//#include <boost/lexical_cast.hpp>

void C_RMainMenu::CreateGUIElements ( void )
{
	// Main menu
	{
		// Create container
		tMainMenu.container = gui->CreatePanel();
		gui->SetElementRect( tMainMenu.container, Rect( -1,-1,-1,-1 ) );
		gui->SetElementVisible( tMainMenu.container, false );

		// Create the buttons
		tMainMenu.btnrealm = gui->CreateButton( tMainMenu.container );
		gui->SetElementRect( tMainMenu.btnrealm, Rect( 0.4f,0.4f,0.2f,0.05f ) );
		gui->SetElementText( tMainMenu.btnrealm, "Select Realm" );

		tMainMenu.btnoptions = gui->CreateButton( tMainMenu.container );
		gui->SetElementRect( tMainMenu.btnoptions, Rect( 0.4f,0.5f,0.2f,0.05f ) );
		gui->SetElementText( tMainMenu.btnoptions, "Options" );

		tMainMenu.btnquit = gui->CreateButton( tMainMenu.container );
		gui->SetElementRect( tMainMenu.btnquit, Rect( 0.4f,0.6f,0.2f,0.05f ) );
		gui->SetElementText( tMainMenu.btnquit, "Quit" );

		// Create the title
		tMainMenu.title = new C_RMainMenuTitle;
		tMainMenu.title->SetVisible( false );
	}

	// Options menu
	{
		// Create container
		tOptions.container = gui->CreatePanel();
		gui->SetElementRect( tOptions.container, Rect( 0.03f,0.04f,0.4f,0.92f ) );
		gui->SetElementVisible( tOptions.container, false );

		// Create the buttons
		tOptions.btnback = gui->CreateButton( tOptions.container );
		gui->SetElementRect( tOptions.btnback, Rect( 0.05f,0.8f,0.2f,0.05f ) );
		gui->SetElementText( tOptions.btnback, "Back" );

		tOptions.lbl_fullscreen_choice = gui->CreateText( tOptions.container, "Fullscreen resolution" );
		gui->SetElementRect( tOptions.lbl_fullscreen_choice, Rect( 0.09f, 0.15f, 0.2f, 0.027f ) );
		tOptions.ddl_fullscreen_choice = gui->CreateDropdownList( tOptions.container );
		gui->SetElementRect( tOptions.ddl_fullscreen_choice, Rect( 0.11f, 0.19f, 0.15f, 0.04f ) );
		{	// TODO: Resolution
			/*uint currentTarget = 0;
			char res_str [256];
			const std::vector<CGameSettings::resolution_t> & reslist = CGameSettings::Active()->GetResolutionList();
			for ( uint i = 0; i < reslist.size(); ++i ) {
				sprintf( res_str, "%dx%d", reslist[i].w, reslist[i].h );
				gui->AddDropdownOption( tOptions.ddl_fullscreen_choice, res_str, i );

				if ( abs(reslist[i].w - CGameSettings::Active()->i_ro_TargetResX)+abs(reslist[i].h - CGameSettings::Active()->i_ro_TargetResY)
					< abs(reslist[currentTarget].w - CGameSettings::Active()->i_ro_TargetResX)+abs(reslist[currentTarget].h - CGameSettings::Active()->i_ro_TargetResY) )
				{
					currentTarget = i;
				}
			}
			gui->SetDropdownOption( tOptions.ddl_fullscreen_choice, currentTarget );*/
		}
		
	}

	// Realm select menu
	{
		// Create container
		tRealmSelect.container = gui->CreatePanel();
		gui->SetElementRect( tRealmSelect.container, Rect( 0.04f,0.03f,0.43f,0.94f ) );
		gui->SetElementVisible( tRealmSelect.container, false );

		// Create the buttons
		tRealmSelect.btnback = gui->CreateButton( tRealmSelect.container );
		gui->SetElementRect( tRealmSelect.btnback, Rect( 0.05f,0.88f,0.18f,0.05f ) );
		gui->SetElementText( tRealmSelect.btnback, "Back" );

		// Initial states
		tRealmSelect.state = tRealmSelect.S_SELECT_REALM;
		tRealmSelect.selection = -1;

		// Create selection container
		tRealmSelect.cont_selection = gui->CreatePanel( tRealmSelect.container );
		gui->SetElementRect( tRealmSelect.cont_selection, Rect( -1,-1,-1,-1 ) );
		gui->SetElementVisible( tRealmSelect.cont_selection, true );
		{
			// Create header
			tRealmSelect.lblheader = gui->CreateText( tRealmSelect.cont_selection, "Realm List" );
			gui->SetElementRect( tRealmSelect.lblheader, Rect( 0.05f,0.05f,0.2f, 0.05f ) );

			// Create buttons
			tRealmSelect.btnselectrealm = gui->CreateButton( tRealmSelect.cont_selection );
			gui->SetElementRect( tRealmSelect.btnselectrealm, Rect( 0.27f,0.82f,0.18f,0.05f ) );
			gui->SetElementText( tRealmSelect.btnselectrealm, "Select Realm" );

			tRealmSelect.btnnewrealm = gui->CreateButton( tRealmSelect.cont_selection );
			gui->SetElementRect( tRealmSelect.btnnewrealm, Rect( 0.05f,0.82f,0.18f,0.05f ) );
			gui->SetElementText( tRealmSelect.btnnewrealm, "New Realm" );

			tRealmSelect.btndeleterealm = gui->CreateButton( tRealmSelect.cont_selection );
			gui->SetElementRect( tRealmSelect.btndeleterealm, Rect( 0.27f,0.88f,0.18f,0.05f ) );
			gui->SetElementText( tRealmSelect.btndeleterealm, "Delete Realm" );

			tRealmSelect.btn_refresh = gui->CreateButton( tRealmSelect.cont_selection );
			gui->SetElementRect( tRealmSelect.btn_refresh, Rect( 0.38f,0.05f,0.04f,0.04f ) );
			gui->SetElementText( tRealmSelect.btn_refresh, "r" );

			// Create delete dialogue
			tRealmSelect.dlgdeleteconfirm = gui->CreateYesNoDialogue( tRealmSelect.container );
			gui->SetElementRect( tRealmSelect.dlgdeleteconfirm, Rect( 0.3f,0.4f,0.4f,0.15f ) );
			gui->SetElementText( tRealmSelect.dlgdeleteconfirm, "Really delete this realm?\n  This is permanent!)" );

			// Create listview
			tRealmSelect.slsListview = gui->CreateListview( tRealmSelect.cont_selection );
			gui->SetElementRect( tRealmSelect.slsListview, Rect( 0.07f, 0.095f, 0.35f, 0.68f ) );
			gui->SetListviewFieldHeight( tRealmSelect.slsListview, 0.04f );

			// Create info label
			tRealmSelect.lblinfo = gui->CreateParagraph( tRealmSelect.cont_selection );
			gui->SetElementRect( tRealmSelect.lblinfo, Rect( 0.5f, 0.06f, 0.4f, 0.05f ) );
			//string sCurrent = "Selected file properties:\nworld count: 1\n";
			//sCurrent += "seed: " + boost::lexical_cast<string>(CGameSettings::Active()->i_cl_DefaultSeed);
			char sCurrent [1024];
			sprintf( sCurrent, "Selected file properties:\nworld count: 1\nseed: %d", CGameSettings::Active()->i_cl_DefaultSeed );
			gui->SetElementText( tRealmSelect.lblinfo, sCurrent );
		}

		// Create creation container
		tRealmSelect.cont_creation = gui->CreatePanel( tRealmSelect.container );
		gui->SetElementRect( tRealmSelect.cont_creation, Rect( -1,-1,-1,-1 ) );
		gui->SetElementVisible( tRealmSelect.cont_creation, false );
		{
			// Create header
			tRealmSelect.lbl_creation_header = gui->CreateText( tRealmSelect.cont_creation, "Create New Realm" );
			gui->SetElementRect( tRealmSelect.lbl_creation_header, Rect( 0.05f,0.05f,0.2f, 0.05f ) );

			tRealmSelect.lbl_realmname = gui->CreateText( tRealmSelect.cont_creation, "Realm Name" );
			gui->SetElementRect( tRealmSelect.lbl_realmname, Rect( 0.07f,0.315f,0.2f,0.05f ) );
			tRealmSelect.fld_realmname = gui->CreateTextfield( tRealmSelect.cont_creation );
			gui->SetElementRect( tRealmSelect.fld_realmname, Rect( 0.07f,0.35f,0.3f,0.06f ) );

			tRealmSelect.lbl_seed = gui->CreateText( tRealmSelect.cont_creation, "Seed" );
			gui->SetElementRect( tRealmSelect.lbl_seed, Rect( 0.07f,0.415f,0.2f,0.05f ) );
			tRealmSelect.fld_seed = gui->CreateTextfield( tRealmSelect.cont_creation );
			gui->SetElementRect( tRealmSelect.fld_seed, Rect( 0.07f,0.45f,0.3f,0.06f ) );

			tRealmSelect.btn_confirmnew = gui->CreateButton( tRealmSelect.cont_creation );
			gui->SetElementRect( tRealmSelect.btn_confirmnew, Rect( 0.16f,0.68f,0.18f,0.05f ) );
			gui->SetElementText( tRealmSelect.btn_confirmnew, "Create Realm" );
		}

		// Init the background map state
		tRealmSelect.renderMap = false;
		tRealmSelect.mapRenderState = CWorldCartographer::sRenderProgressState();
		tRealmSelect.cartographer = NULL;
		tRealmSelect.generator = NULL;
		tRealmSelect.main_planetoid = NULL;

		tRealmSelect.gui = gui;
	}

	// Character select menu
	{
		// Create container
		tCharSelect.container = gui->CreatePanel();
		gui->SetElementRect( tCharSelect.container, Rect( 0.04f,0.03f,0.43f,0.94f ) );
		gui->SetElementVisible( tCharSelect.container, false );

		// Create header
		tCharSelect.lblheader = gui->CreateText( tCharSelect.container, "Character Selection" );
		gui->SetElementRect( tCharSelect.lblheader, Rect( 0.1f,0.07f,0.2f, 0.05f ) );

		// Create the buttons
		tCharSelect.btnback = gui->CreateButton( tCharSelect.container );
		gui->SetElementRect( tCharSelect.btnback, Rect( 0.05f,0.8f,0.1f,0.05f ) );
		gui->SetElementText( tCharSelect.btnback, "Back" );

		tCharSelect.btnselectchar = gui->CreateButton( tCharSelect.container );
		gui->SetElementRect( tCharSelect.btnselectchar, Rect( 0.18f,0.75f,0.2f,0.05f ) );
		gui->SetElementText( tCharSelect.btnselectchar, "Select Character" );

		tCharSelect.btnnewchar = gui->CreateButton( tCharSelect.container );
		gui->SetElementRect( tCharSelect.btnnewchar, Rect( 0.18f,0.89f,0.2f,0.05f ) );
		gui->SetElementText( tCharSelect.btnnewchar, "New Character" );

		tCharSelect.btndeletechar = gui->CreateButton( tCharSelect.container );
		gui->SetElementRect( tCharSelect.btndeletechar, Rect( 0.18f,0.82f,0.2f,0.05f ) );
		gui->SetElementText( tCharSelect.btndeletechar, "Delete Character" );

		tCharSelect.pghinfo = gui->CreateParagraph( tCharSelect.container );
		gui->SetElementRect( tCharSelect.pghinfo, Rect( 0.33f,0.18f,0.4f,0.05f ) );
		gui->SetElementText( tCharSelect.pghinfo, "" );

		tCharSelect.dlgdeleteconfirm = gui->CreateYesNoDialogue( tCharSelect.container );
		gui->SetElementRect( tCharSelect.dlgdeleteconfirm, Rect( 0.3f,0.4f,0.4f,0.15f ) );
		gui->SetElementText( tCharSelect.dlgdeleteconfirm, "Really delete this character?\n  (This cannot be undone, ever!)" );

		// Create listview
		tCharSelect.slsCharacterList = gui->CreateListview( tCharSelect.container );
		gui->SetElementRect( tCharSelect.slsCharacterList, Rect( 0.07f, 0.12f, 0.25f, 0.62f ) );
		gui->SetListviewFieldHeight( tCharSelect.slsCharacterList, 0.04f );

		//tCharSelect.list_chars = NULL;
		tCharSelect.gui = gui;
	}

	// Character create menu
	{
		// Create container
		tCharCreation.container = gui->CreatePanel();
		gui->SetElementRect( tCharCreation.container, Rect( -1,-1,-1,-1 ) );
		gui->SetElementVisible( tCharCreation.container, false );

		// Create the buttons
		tCharCreation.btnback = gui->CreateButton( tCharCreation.container );
		gui->SetElementRect( tCharCreation.btnback, Rect( 0.05f,0.9f,0.1f,0.05f ) );
		gui->SetElementText( tCharCreation.btnback, "Cancel" );

		// Create the soulname prompt
		tCharCreation.fld_soulname = gui->CreateTextfield( tCharCreation.container );
		gui->SetElementRect( tCharCreation.fld_soulname, Rect( 0.35f,0.45f,0.3f,0.06f ) );

		tCharCreation.btn_soulnamecontinue = gui->CreateButton( tCharCreation.container );
		gui->SetElementRect( tCharCreation.btn_soulnamecontinue, Rect( 0.4f, 0.6f, 0.2f, 0.05f ) );
		gui->SetElementText( tCharCreation.btn_soulnamecontinue, "Use soulname" );

		tCharCreation.pgh_soulname = gui->CreateParagraph( tCharCreation.container );
		gui->SetElementRect( tCharCreation.pgh_soulname, Rect( 0.04f,0.35f,0.31f,0.25f ) );
		gui->SetElementText( tCharCreation.pgh_soulname, "Enter your new character's soulname.\nA soulname is the name used by the entities that control and run the universes. It doesn't have to be the actual name of the character.\nOnly alphabetical and hyphenation characters are allowed, and the name must be at least two letters." );


		/*tCharCreation.btncontinue = gui->CreateButton( tCharCreation.container );
		gui->SetElementRect( tCharCreation.btncontinue, Rect( 0.55f,0.7f,0.2f,0.05f ) );
		gui->SetElementText( tCharCreation.btncontinue, "CONTINUE" );

		// Now these are temp, since we want the character creation to be in its own object
		// But they serve enough purpose for now
		tCharCreation.fld_character_name = gui->CreateText( tCharCreation.container, "Name: Clara" );
		gui->SetElementRect( tCharCreation.fld_character_name, Rect( 0.08f, 0.2f, 0.2f, 0.05f ) );

		tCharCreation.lbl_gametype = gui->CreateText( tCharCreation.container, "Gametype" );
		gui->SetElementRect( tCharCreation.lbl_gametype, Rect( 0.09f, 0.66f, 0.2f, 0.027f ) );
		tCharCreation.lbl_gametype_choice = gui->CreateText( tCharCreation.container, "Not Dying" );
		gui->SetElementRect( tCharCreation.lbl_gametype_choice, Rect( 0.11f, 0.69f, 0.2f, 0.02f ) );
		tCharCreation.btn_gametype_left = gui->CreateButton( tCharCreation.container );
		gui->SetElementRect( tCharCreation.btn_gametype_left, Rect( 0.08f, 0.70f, 0.02f, 0.03f ) );
		tCharCreation.btn_gametype_right = gui->CreateButton( tCharCreation.container );
		gui->SetElementRect( tCharCreation.btn_gametype_right, Rect( 0.23f, 0.70f, 0.02f, 0.03f ) );

		tCharCreation.lbl_gender = gui->CreateText( tCharCreation.container, "Gender" );
		gui->SetElementRect( tCharCreation.lbl_gender, Rect( 0.09f, 0.36f, 0.2f, 0.027f ) );
		tCharCreation.lbl_gender_choice = gui->CreateText( tCharCreation.container, "Female" );
		gui->SetElementRect( tCharCreation.lbl_gender_choice, Rect( 0.11f, 0.39f, 0.2f, 0.02f ) );
		tCharCreation.btn_gender_left = gui->CreateButton( tCharCreation.container );
		gui->SetElementRect( tCharCreation.btn_gender_left, Rect( 0.08f, 0.40f, 0.02f, 0.03f ) );
		tCharCreation.btn_gender_right = gui->CreateButton( tCharCreation.container );
		gui->SetElementRect( tCharCreation.btn_gender_right, Rect( 0.23f, 0.40f, 0.02f, 0.03f ) );

		tCharCreation.lbl_race = gui->CreateText( tCharCreation.container, "Race" );
		gui->SetElementRect( tCharCreation.lbl_race, Rect( 0.09f, 0.26f, 0.2f, 0.027f ) );
		tCharCreation.lbl_race_choice = gui->CreateText( tCharCreation.container, "Human" );
		gui->SetElementRect( tCharCreation.lbl_race_choice, Rect( 0.11f, 0.29f, 0.2f, 0.02f ) );
		tCharCreation.btn_race_left = gui->CreateButton( tCharCreation.container );
		gui->SetElementRect( tCharCreation.btn_race_left, Rect( 0.08f, 0.30f, 0.02f, 0.03f ) );
		tCharCreation.btn_race_right = gui->CreateButton( tCharCreation.container );
		gui->SetElementRect( tCharCreation.btn_race_right, Rect( 0.23f, 0.30f, 0.02f, 0.03f ) );
		tCharCreation.race = CRACE_HUMAN;*/
	}
}

C_RMainMenu::eMenuState	C_RMainMenu::stateLoad ( void )
{
	fTimer = 0;
	return M_intro;
}
C_RMainMenu::eMenuState	C_RMainMenu::stateIntro ( void )
{
	fTimer += Time::deltaTime*0.6f;
	//fTimeOfDay = fTimer * 60 * 60 * 2;
	//pDaycycle->
	//fTimeOfDay = Math.Smoothlerp( fTimer, 60*60*17, 60*60*(24+5) );
	//fTimeOfDay = Math.Smoothlerp( fTimer, 60*60*13, 60*60*(24+5) );
	stateCurrent.timeOfDay = Math.Smoothlerp( fTimer, 60*60*6, 60*60*(24+4) );
	//pDaycycle->SetTimeOfDay( fTimeOfDay );
	pCloudcycle->SetCloudDensity( 1.2f - 1.4f*fTimer );

	stateCurrent.cameraPos = Vector3d( 0,0,3030 ) + Vector3d( 6500,0,0 );
	stateCurrent.cameraRot = -Vector3d( 0,20,10 );
	stateCurrent.cameraFoV = 100.0f;
	stateCurrent.cameraRoll = -160+180*(1-fTimer);
	stateCurrent.spaceEffect = 1;

	statePrevious = stateCurrent;
	stateTarget = statePrevious;

	fLerpValue = 0;

	if ( fTimer > 1.0f )
	{
		// Terrain collision background loader to speed up getting into game
		/*{
			CTerrainCollisionLoader* loader = new CTerrainCollisionLoader();
			loader->RemoveReference();
		}*/


		fTimer = 0;
		fLerpValue = 0;
		return M_mainmenu;
	}
	return iMenuState;
}
FORCE_INLINE void C_RMainMenu::BlendWithSpeed ( const Real speed )
{
	if ( fLerpValue < 1.0f )
		fLerpValue += Time::deltaTime*speed;
	else
		fLerpValue = 1.0f;
}
C_RMainMenu::eMenuState	C_RMainMenu::stateMainMenu ( void )
{
	// Continue day cycle
	stateTarget.timeOfDay += Time::deltaTime * 60 * 15;
	// Move camera
	BlendWithSpeed(1.5f);
	// Set new camera pos
	stateTarget.cameraPos = Vector3d( -10,0,3010 ) + Vector3d( 6500,0,0 );
	stateTarget.cameraRot = Vector3d( 0,0,-10 );
	stateTarget.cameraFoV = 100.0f;
	stateTarget.cameraRoll = -180.0f;
	stateTarget.spaceEffect = 1;

	// Set main menu visible
	tMainMenu.title->SetVisible( true );

	// Enable menu and work on shit
	gui->SetElementVisible( tMainMenu.container, true );
	if ( gui->GetButtonClicked( tMainMenu.btnquit ) )
	{
		CGameState::Active()->EndGame();
	}
	else if ( gui->GetButtonClicked( tMainMenu.btnoptions ) )
	{
		Audio.playSound( "Menu.Click" );

		tMainMenu.title->SetVisible( false );
		gui->SetElementVisible( tMainMenu.container, false );
		return M_options;
	}
	else if ( gui->GetButtonClicked( tMainMenu.btnrealm ) )
	{
		Audio.playSound( "Menu.Click" );

		tMainMenu.title->SetVisible( false );
		gui->SetElementVisible( tMainMenu.container, false );
		return M_realmselect;
	}
	
	return iMenuState;
}
C_RMainMenu::eMenuState	C_RMainMenu::stateOptions ( void )
{
	// Move camera
	BlendWithSpeed( 1.2f );
	// Set new camera pos
	stateTarget.cameraPos = Vector3d( -15,0,20 ) + Vector3d( 6500,0,0 );
	stateTarget.cameraRot = Vector3d( -15,-90,0 );
	stateTarget.cameraFoV = 120.0f;
	// Move time of day
	stateTarget.timeOfDay = 60*60*(24+4);
	stateTarget.spaceEffect = 0;

	// Enable options menu
	gui->SetElementVisible( tOptions.container, true );
	if ( gui->GetButtonClicked( tOptions.btnback ) )
	{
		Audio.playSound( "Menu.Click" );

		gui->SetElementVisible( tOptions.container, false );
		return M_mainmenu;
	}
	else
	{	// TODO: Resolution
		/*const std::vector<CGameSettings::resolution_t> & reslist = CGameSettings::Active()->GetResolutionList();
		int res_index = gui->GetDropdownOption( tOptions.ddl_fullscreen_choice );
		CGameSettings::Active()->i_ro_TargetResX = reslist[res_index].w;
		CGameSettings::Active()->i_ro_TargetResY = reslist[res_index].h;*/
	}

	return iMenuState;
}
C_RMainMenu::eMenuState	C_RMainMenu::stateRealmSelect ( void )
{
	// Move camera
	BlendWithSpeed( 0.8f );
	// Set new camera pos
	stateTarget.cameraPos = Vector3d( -25,0,3010 );
	stateTarget.cameraRot = Vector3d( 0,0,-170 );
	stateTarget.cameraFoV = 100.0f;
	stateTarget.cameraRoll = -360;
	// Move time of day
	stateTarget.timeOfDay = 60*60*(24+10);
	stateTarget.spaceEffect = 0.9f;

	pCamera->zFar = 2000; // Change Z far for this area

	// Enable realm select menu
	gui->SetElementVisible( tRealmSelect.container, true );
	// Depending on state, toggle views
	gui->SetElementVisible( tRealmSelect.cont_selection, tRealmSelect.state == tRealmSelect.S_SELECT_REALM );
	gui->SetElementVisible( tRealmSelect.cont_creation, tRealmSelect.state == tRealmSelect.S_CREATE_REALM );

	// Generate realm list
	tRealmSelect.CreateRealmList();
	// Get realm selection
	int prevselection = tRealmSelect.selection;
	tRealmSelect.selection = gui->GetListviewSelection( tRealmSelect.slsListview );

	// Create planetoid
	if ( tRealmSelect.main_planetoid == NULL ) {
		tRealmSelect.main_planetoid = new CModel ( string("models/smoothsphere.FBX") );
		tRealmSelect.main_planetoid->transform.scale = Vector3d(1,1,1) * 800;

		glMaterial* skymat = new glMaterial();
		skymat->loadFromFile( "sky/planetoid_terra" );
		skymat->setTexture( 0, tRealmSelect.terra_rt );

		tRealmSelect.main_planetoid->SetMaterial( skymat );
	}
	else {
		tRealmSelect.main_planetoid->transform.position = stateTarget.cameraPos + Vector3d( -10,1.8f,-3.1f )*(110+50);
	}
	// Set sprite
	tRealmSelect.terra_sprite->position.x = Screen::Info.width * 0.5f;
	tRealmSelect.terra_sprite->position.y = Screen::Info.height * 0.1f - Screen::Info.height;
	tRealmSelect.terra_sprite->visible = false;

	// Generate BG
	if ( tRealmSelect.renderMap ) {
		ftype startTime = Time::CurrentTime();
		do {
			tRealmSelect.cartographer->StutterRenderWorldTextureMap( tRealmSelect.mapRenderState );
		} while ( Time::CurrentTime()-startTime < 1/40.0f && !tRealmSelect.mapRenderState.done );
	}

	// Text fields
	if ( tRealmSelect.state == tRealmSelect.S_CREATE_REALM )
	{
		// Hide sprite
		tRealmSelect.terra_sprite->visible = false;

		// Work on the textbox
		gui->UpdateTextfield( tRealmSelect.fld_seed, tRealmSelect.cur_seed );
		gui->UpdateTextfield( tRealmSelect.fld_realmname, tRealmSelect.cur_realmname );
		// Check if the name is valid. If it's not valid, hide the freaking thing
		int c_count = 0;
		for ( unsigned int i = 0; i < tRealmSelect.cur_realmname.size(); ++i )
		{
			unsigned char c = tRealmSelect.cur_realmname[i];
			if ( tRealmSelect.cur_realmname[0] == ' ' ) {
				c_count -= 16;
			}
			if ( isalnum(c) ) {
				c_count += 1;
			}
			else if ( (c=='\'') || (c=='`') || (c=='-') || (c==' ') ) {
				;
			}
			else {
				c_count -= 100; // Invalid name
			}
		}

		fs::path realmdir( CGameSettings::Active()->GetWorldSaveDir(tRealmSelect.cur_realmname) );
		gui->SetElementVisible( tRealmSelect.btn_confirmnew, ((c_count >= 1)&&( !fs::exists( realmdir ) )) );

		if ( gui->GetButtonClicked( tRealmSelect.btn_confirmnew ) )
		{
			Audio.playSound( "Menu.Click" );

			// Set file to given name
			CGameSettings::Active()->SetWorldSaveFile( tRealmSelect.cur_realmname );
			// Create player folder
			CGameSettings::Active()->GetWorldSaveDir();

			// Load the realm
			CMCCRealm newRealm ( tRealmSelect.cur_realmname.c_str() );
			// Set the realm seed
			newRealm.SetSeed( tRealmSelect.cur_seed.c_str() );
			// Realm will be saved automatically

			// Ready up the character controller
			m_gametype->m_charactercontroller->ReadyUp();

			// Clear realm list
			tRealmSelect.list_realms.clear();
			// Reset selection
			tRealmSelect.selection = -1;

			// Go to select state
			tRealmSelect.state = tRealmSelect.S_SELECT_REALM;
		}
	}
	else if ( tRealmSelect.state == tRealmSelect.S_SELECT_REALM )
	{
		// Work refresh button
		if ( gui->GetButtonClicked( tRealmSelect.btn_refresh ) ) {
			// Clear realm list
			tRealmSelect.list_realms.clear();
			// Reset selection
			tRealmSelect.selection = -1;
		}

		// Toggle selection button visibility based on selection
		if ( tRealmSelect.selection != -1 ) {
			// Get the name of the realm and load its options, if possible

			gui->SetElementVisible( tRealmSelect.btnselectrealm, true );
			// Update the realm EVERYTHING but only on change!
			if ( prevselection != tRealmSelect.selection || (CGameSettings::Active()->GetWorldSaveFile() != tRealmSelect.list_realms[tRealmSelect.selection]) ) {
				Audio.playSound( "Menu.Click" );

				// Select that realm
				CGameSettings::Active()->SetWorldSaveFile( tRealmSelect.list_realms[tRealmSelect.selection] );
				CGameSettings::Active()->SetTerrainSaveFile( "terra" );

				// Load the realm
				CMCCRealm targetRealm ( tRealmSelect.list_realms[tRealmSelect.selection].c_str() );
				// Set info string
				char temp [512];
				sprintf( temp, "Realm name: %s\nWorld Seed: %X", tRealmSelect.list_realms[tRealmSelect.selection].c_str(), targetRealm.GetSeed() ); 
				gui->SetElementText( tRealmSelect.lblinfo, temp );

				// Create generator
				if ( tRealmSelect.generator == NULL ) {
					tRealmSelect.generator = new Terrain::CWorldGen_Terran( 5 );
				}
				else {
					tRealmSelect.generator->Cleanup();
				}
				tRealmSelect.generator->SetSeed( targetRealm.GetSeed() );
				tRealmSelect.generator->Initialize();

				// Generator is ready, start up the terrain system
				CVoxelTerrain* currentTerrain = CVoxelTerrain::GetActive();
				if ( currentTerrain ) {
					//currentTerrain->State_ClearInformation(); // Clear information
					currentTerrain->SetGenerator(tRealmSelect.generator);
					currentTerrain->ResetSystem(); // Clear information
					currentTerrain->SetSystemPaused( false ); // Run the terrain.
				}

				/*
				// (Use the generator to generate a new spawnpoint, and set it.).
				Vector3d_d newSpawnpoint = tRealmSelect.generator->GetSpawnPoint( 1, 0, targetRealm.GetPersonCount() );
				// Tell where the terrain should be generating
				COctreeTerrain::GetActive()->SetStateWorldOffset( newSpawnpoint );
				*/
				{
					// Force code to run
					tCharSelect.selection = -1;
					// Create character list
					if ( tCharSelect.list_chars.empty() ) {
						tCharSelect.CreateCharList();
					}
					// Save the selected character name
					tCharSelect.last_selected = targetRealm.GetSavedSoulname();
					// Check for previous selection on this realm
					if ( gui->GetListviewSelection( tCharSelect.slsCharacterList ) == -1 ) {
						// With the realm, check for match
						for ( uint i = 0; i < tCharSelect.list_chars.size(); ++i )
						{
							if ( tCharSelect.last_selected.compare( tCharSelect.list_chars[i].c_str() ) )
							{	// Name matches, start up with this character
								gui->SetListviewSelection( tCharSelect.slsCharacterList, i );
							}
						}
					}
					// Check for character select buttons
					int nextSelection = gui->GetListviewSelection( tCharSelect.slsCharacterList );
					if ( nextSelection != tCharSelect.selection ) // If character selection has changed, load that character.
					{
						tCharSelect.selection = (int)(nextSelection);
						// Set game settings
						CGameSettings::Active()->SetPlayerSaveFile( tCharSelect.list_chars[ tCharSelect.selection ] );
						// Load stats
						pl_stats->LoadFromFile();
						// Temporarily load up the current realm.
						SetTerrainPlacementFromStats();
					}
					else
					{
						m_next_spawnpoint =  tRealmSelect.generator->GetSpawnPoint( 1, 0, targetRealm.GetPersonCount() );
						CVoxelTerrain::GetActive()->SetCenterPosition( m_next_spawnpoint );
					}
					// 
				}


				// Create maps
				//CWorldCartographer cartographer ( "terra", tRealmSelect.terra_rt );
				//cartographer.RenderTerrainMap();
				if ( tRealmSelect.cartographer == NULL ) {
					tRealmSelect.cartographer = new CWorldCartographer( "terra", tRealmSelect.terra_rt );
					tRealmSelect.cartographer->GiveGenerator( tRealmSelect.generator );
				}
				else {
					tRealmSelect.cartographer->ForceReload();
					tRealmSelect.cartographer->GiveGenerator( tRealmSelect.generator );
				}// TRY RenderTerrainMap to check code
				tRealmSelect.mapRenderState = CWorldCartographer::sRenderProgressState(); // Reset map state
				tRealmSelect.renderMap = true;
				//tRealmSelect.cartographer->RenderTerrainMap();
			}
		}
		else {
			gui->SetElementVisible( tRealmSelect.btnselectrealm, false );
			gui->SetElementText( tRealmSelect.lblinfo, "" );
		}
	}

	// Work on buttons
	if ( gui->GetButtonClicked( tRealmSelect.btnback ) )
	{
		Audio.playSound( "Menu.Click" );

		if ( tRealmSelect.state == tRealmSelect.S_SELECT_REALM ) {
			gui->SetElementVisible( tRealmSelect.container, false );

			// Hide sprite
			tRealmSelect.terra_sprite->visible = false;

			// If hit back, also stop the terrain object
			CVoxelTerrain* currentTerrain = CVoxelTerrain::GetActive();
			if ( currentTerrain ) {
				currentTerrain->SetSystemPaused( true );
			}

			return M_mainmenu;
		}
		else if ( tRealmSelect.state == tRealmSelect.S_CREATE_REALM ) {
			tRealmSelect.state = tRealmSelect.S_SELECT_REALM;
		}
	}
	else if ( gui->GetButtonClicked( tRealmSelect.btnnewrealm ) )
	{
		Audio.playSound( "Menu.Click" );

		// Reset realm name and seed
		tRealmSelect.cur_realmname = "";
		tRealmSelect.cur_seed = "";
		// Generate a random seed
		{
			static const char randomset[] =
				"0123456789"
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"abcdefghijklmnopqrstuvwxyz"
				"!@#$%^&*()_+-={}[]:\";'<>,.?/|\\`~";
			int rand_len = Random.Next() % 8 + 8;
			int set_len = strlen( randomset );
			for ( int i = 0; i < rand_len; ++i ) {
				tRealmSelect.cur_seed += randomset[Random.Next() % set_len];
			}
		}
		// Go to creation state
		tRealmSelect.state = tRealmSelect.S_CREATE_REALM;

		// Make sure the terrain system is stopped
		CVoxelTerrain* currentTerrain = CVoxelTerrain::GetActive();
		if ( currentTerrain ) {
			currentTerrain->SetSystemPaused( true );
		}
	}
	else if ( gui->GetButtonClicked( tRealmSelect.btnselectrealm ) && ( tRealmSelect.selection != -1 ) )
	{
		Audio.playSound( "Menu.Click" );

		// Select that realm and go to next
		CGameSettings::Active()->SetWorldSaveFile( tRealmSelect.list_realms[tRealmSelect.selection] );
		CGameSettings::Active()->SetTerrainSaveFile( "terra" );
		// Load and save settings
		CMCCRealm targetRealm;

		// Reset selection
		tRealmSelect.selection = -1;
		// Save the selected character name
		tCharSelect.selection = -1;
		tCharSelect.last_selected = targetRealm.GetSavedSoulname();
		// Increment the selection counter
		targetRealm.IncSelectCount();

		// Make sure the terrain system is running
		CVoxelTerrain* currentTerrain = CVoxelTerrain::GetActive();
		if ( currentTerrain ) {
			currentTerrain->SetSystemPaused( false );
		}

		// Start up the load checker
		pPauseLoader->active = true;
		pPauseLoader->Reset();
		pPauseLoader->m_targetLoadcheckPosition = Vector3d(0,0,0);

		// Hide sprite
		tRealmSelect.terra_sprite->visible = false;
		// Go to next state
		gui->SetElementVisible( tRealmSelect.container, false );
		
		return M_blendtocharselect;
	}
	else if ( gui->GetButtonClicked( tRealmSelect.btndeleterealm ) && ( tRealmSelect.selection != -1 ) )
	{
		Audio.playSound( "Menu.Click" );

		if ( tRealmSelect.selection >= 0 ) {
			CGameSettings::Active()->SetWorldSaveFile( tRealmSelect.list_realms[tRealmSelect.selection] );
			CGameSettings::Active()->SetTerrainSaveFile( "terra" );
		}

		gui->ShowYesNoDialogue( tRealmSelect.dlgdeleteconfirm );
	}
	else
	{
		// Check for dialogue buttons
		int dlgresult = gui->GetDialogueResponse( tRealmSelect.dlgdeleteconfirm );
		if ( dlgresult == 0 )
		{
			// Make sure the terrain system is stopped before deleting worlds
			CVoxelTerrain* currentTerrain = CVoxelTerrain::GetActive();
			if ( currentTerrain ) {
				currentTerrain->SetSystemPaused( true ); // Will block until completely paused
			}
			
			string dir = CGameSettings::Active()->GetWorldSaveDir();
			fs::path realmdir( dir );
			fs::remove_all( realmdir );

			gui->HideDialogue( tRealmSelect.dlgdeleteconfirm );

			// Clear realm list
			tRealmSelect.list_realms.clear();
			// Reset selection
			tRealmSelect.selection = -1;
		}
		else if ( dlgresult == 1 )
		{
			gui->HideDialogue( tRealmSelect.dlgdeleteconfirm );
		}
	}

	return iMenuState;
}
void C_RMainMenu::sRealmSelectElements_t::CreateRealmList ( void )
{
	if ( list_realms.size() > 0 ) {
		return;
	}

	// Reset selection
	selection = -1;

	// Need to loop through all the found folders in the character directory
	// From game settings, get a list of all names in .games\.players
	std::vector<string> realms;
	CGameSettings::Active()->GetRealmFileList( realms );

	// If there are characters, create the list of selectors
	/*char_num = characters.size();
	if ( !characters.empty() )
	{
		list_chars	= new Handle [char_num];
		list_strs	= new string [char_num];
		for ( uint i = 0; i < char_num; ++i )
		{
			list_chars[i] = gui->CreateButton( container );
			gui->SetElementRect( list_chars[i], Rect( 0.06f,0.13f + i*0.06f,0.2f,0.05f ) );
			gui->SetElementText( list_chars[i], characters[i] );

			list_strs[i] = characters[i];
		}
	}*/
	gui->ClearListview( slsListview );
	for ( uint i = 0; i < realms.size(); ++i )
	{
		list_realms.push_back( realms[i] );
		gui->AddListviewOption( slsListview, realms[i], i );
	}
}

C_RMainMenu::eMenuState	C_RMainMenu::stateBlendToCharSelect ( void )
{
	// Move camera
	BlendWithSpeed( 1.1f );
	// Set new camera pos
	stateTarget.cameraPos = Vector3d( -25,0,3010 ) + (Vector3d( -10,1.8f,-3.1f )*(110+50-82+30));
	stateTarget.cameraRot = Vector3d( 0,20,-190 );
	stateTarget.cameraFoV = 100.0f;
	stateTarget.cameraRoll = -260;
	// Move time of day
	stateTarget.timeOfDay =  60*60*(24+10);
	stateTarget.spaceEffect = 0.6f;

	// If lerp at certain point, then go to char select, starting in clouds
	if ( fLerpValue > 0.7f )
	{
		// At this point, wait for the terrain to finish loading.
		if ( !pPauseLoader->GetDoneLoad() ) {
			fLerpValue = 0.7f;

			// Print terrain loading information
			pPauseLoader->SetVisible( true );
		}
		else
		{
			stateCurrent = stateTarget;
			stateCurrent.spaceEffect = 0;
			stateCurrent.cameraRoll = -30;
			stateCurrent.cameraPos = Vector3d( 800,-200,800 );
			stateCurrent.cameraRot = Vector3d( 0,74,-170 );
			stateCurrent.timeOfDay =  60*60*(24+0);

			tCharModel.lookatCameraOffset *= 0.01f;

			new CScreenFade( true, 0.4f, 0.1f, Color(1,1,1) );

			// Turn off load checker
			pPauseLoader->active = false;
			pPauseLoader->SetVisible( false );

			// Get the gamestate into game mode
			((gametypeMenu*)m_gametype)->InitWorld();

			return M_charselect;
		}
	}
	return iMenuState;
}

C_RMainMenu::eMenuState	C_RMainMenu::stateCharSelect ( void )
{
	/*
	p_model->transform.position = Vector3d( -17.1f,0.9f,16.3f );
		p_model->SetFaceAtRotation( Rotator(0,0,-40) );
	*/
	if ( (m_playercharacter == NULL) && (gui->GetListviewSelection( tCharSelect.slsCharacterList ) != -1) )
	{
		uint64_t player_id = NPC::Manager->RequestNPC( NPC::npcid_SETPIECE, 0 );
		{
			NPC::characterFile_t characterFile;
			characterFile.rstats = pl_stats->race_stats;
			
			NPC::sWorldState worldstate;
			characterFile.worldstate = &worldstate;
			worldstate.mFocus = NPC::AIFOCUS_None;
			worldstate.mFocusName = "WUPWUPWUPWUP";
			worldstate.worldPosition = Vector3d( -17.1f,0.9f,16.3f );//pPlayerStats->vPlayerInitSpawnPoint;
			worldstate.partyHost = 1024;

			NPC::sOpinions opinions;
			characterFile.opinions = &opinions;
			NPC::sPreferences prefs;
			characterFile.prefs = &prefs;

			// Save generated stats
			NPC::CZonedCharacterIO io;
			io.CreateCharacterFile( player_id, characterFile );
		}
		m_playercharacter = (NPC::CNpcBase*) NPC::Manager->SpawnNPC(player_id);

		if ( m_playercharacter ) {
			Vector3d_d delta = m_next_spawnpoint - CVoxelTerrain::GetActive()->GetCenterPosition();
			m_playercharacter->transform.position = Vector3d((Real)delta.x,(Real)delta.y,(Real)delta.z);
			m_playercharacter->transform.SetDirty();

			p_model = (CMccCharacterModel*)m_playercharacter->GetCharModel();
		}
	}
	
	// Get values from lua file
	Lua::Controller->SetEnvironment( m_environment.c_str() );
	Vector3d mCameraOffset;
	Vector3d mCameraOffsetAngle;
	Vector3d mCameraAngle;
	int returns;
	lua_State* L = Lua::Controller->GetState();
	returns = Lua::Controller->Call( "GetCameraOffset", 0 );
	if ( returns == 1 && lua_istable(L,-1) ) {
		mCameraOffset.x = (Real)Lua::Controller->TableGetNumber("x");
		mCameraOffset.y = (Real)Lua::Controller->TableGetNumber("y");
		mCameraOffset.z = (Real)Lua::Controller->TableGetNumber("z");
		lua_pop(L,1);
	}
	returns = Lua::Controller->Call( "GetCameraOffsetAngle", 0 );
	if ( returns == 1 && lua_istable(L,-1) ) {
		mCameraOffsetAngle.x = (Real)Lua::Controller->TableGetNumber("x");
		mCameraOffsetAngle.y = (Real)Lua::Controller->TableGetNumber("y");
		mCameraOffsetAngle.z = (Real)Lua::Controller->TableGetNumber("z");
		lua_pop(L,1);
	}
	returns = Lua::Controller->Call( "GetCameraAngles", 0 );
	if ( returns == 1 && lua_istable(L,-1) ) {
		mCameraAngle.x = (Real)Lua::Controller->TableGetNumber("x");
		mCameraAngle.y = (Real)Lua::Controller->TableGetNumber("y");
		mCameraAngle.z = (Real)Lua::Controller->TableGetNumber("z");
		lua_pop(L,1);
	}
	Lua::Controller->ResetEnvironment();


	// Move camera
	BlendWithSpeed( 0.5f );
	// Set new camera pos
	//////stateTarget.cameraPos = Vector3d( -14.4f,-3.4f,20.1f )-Vector3d( -17.1f,0.9f,16.3f ) + p_model->transform.position;
	////stateTarget.cameraPos = Vector3d( -14.4f,-3.4f,20.1f )-Vector3d( -17.1f,0.9f,16.3f ) + m_playercharacter->transform.position;
	////stateTarget.cameraRot = Rotator( Vector3d( 0,-6,-150 ) ).getQuaternion();
	//stateTarget.cameraPos = (m_playercharacter->GetCharModel()->transform.rotation * Rotator(0,0,-40) ) * (Vector3d( -14.4f,-3.4f,20.1f )-Vector3d( -17.1f,0.9f,16.3f )) + m_playercharacter->transform.position;
	//stateTarget.cameraRot = (m_playercharacter->GetCharModel()->transform.rotation * Rotator( Vector3d( 0,-6,-150 ) ) ).getQuaternion();
	if ( m_playercharacter && m_playercharacter->GetCharModel() )
	{
		stateTarget.cameraPos = ( (m_playercharacter->GetCharModel()->GetModelRotation() * Rotator( mCameraOffsetAngle ) ) * mCameraOffset )+ m_playercharacter->transform.position;
		stateTarget.cameraRot = (m_playercharacter->GetCharModel()->GetModelRotation() * Rotator( mCameraAngle ) ).getQuaternion();
		// Spin character
		//m_playercharacter->GetAI()->ai_lookat.facingPos = m_playercharacter->GetCharModel()->transform.position + Rotator(0,0,Time::currentTime*60)*Vector3d(20,0,0);
		// Have character look at the camera for now
		//m_playercharacter->GetAI()->ai_lookat.lookatPos = stateTarget.cameraPos + tCharModel.lookatCameraOffset;
		//m_playercharacter->GetAI()->ai_lookat.lookatOverride = 1;
		m_playercharacter->GetAI()->RequestLookatOverride ( stateTarget.cameraPos + tCharModel.lookatCameraOffset );
	}
	else
	{
		Vector3d_d delta = m_next_spawnpoint - CVoxelTerrain::GetActive()->GetCenterPosition();
		stateTarget.cameraPos = Vector3d((Real)delta.x,(Real)delta.y,(Real)delta.z) + Rotator( mCameraOffsetAngle ) * mCameraOffset;
		stateTarget.cameraRot = Rotator( mCameraAngle ).getQuaternion();
	}
	stateTarget.cameraFoV = 77.0f;
	stateTarget.cameraRoll = 0.0f;
	// Move time of day
	//stateTarget.timeOfDay = 60*60*(24+6);
	stateTarget.spaceEffect = 0;

	pCamera->zFar = 1000; // Change Z far back to default for this area

	// Enable character select menu
	gui->SetElementVisible( tCharSelect.container, true );

	// Enable/disable select and delete button based on selection
	gui->SetElementVisible( tCharSelect.btnselectchar, tCharSelect.selection >= 0 );
	gui->SetElementVisible( tCharSelect.btndeletechar, tCharSelect.selection >= 0 );

	// Create character list
	if ( tCharSelect.list_chars.empty() ) {
		tCharSelect.CreateCharList();
	}
	if ( gui->GetButtonClicked( tCharSelect.btnback ) )
	{
		Audio.playSound( "Menu.Click" );

		// Remove character list
		tCharSelect.FreeCharList();
		// Clear char info
		gui->SetElementText( tCharSelect.pghinfo, "" );

		gui->SetElementVisible( tCharSelect.container, false );

		// Stop game mode
		((gametypeMenu*)m_gametype)->StopWorld();
		// Delete the character
		if ( m_playercharacter ) {
			DeleteObject( m_playercharacter );
		}
		m_playercharacter = NULL;
		return M_realmselect;
	}
	else if ( gui->GetButtonClicked( tCharSelect.btnnewchar ) )
	{
		Audio.playSound( "Menu.Click" );
		
		// Remove character list
		tCharSelect.FreeCharList();

		//CGameSettings::Active()->SetPlayerSaveFile( "clara" );
		//tCharSelect.selection = -1;
		
		// Hide this dialogue
		gui->SetElementVisible( tCharSelect.container, false );

		// Have another blender. Blend to white, turn the terrain off.
		// Create a specific area for the new character.
		// Stop game mode
		((gametypeMenu*)m_gametype)->StopWorld();

		// Goto character creation
		//return M_charcreation;
		return M_blendtocharcreation;
	}
	else if ( gui->GetButtonClicked( tCharSelect.btnselectchar ) )
	{
		Audio.playSound( "Menu.Click" );

		if ( tCharSelect.selection >= 0 ) {
			// Set the savefile
			CGameSettings::Active()->SetPlayerSaveFile( tCharSelect.list_chars[ tCharSelect.selection ] );

			// Check if this character has been on this world before.
			string spawnpointFile = CGameSettings::Active()->GetWorldSaveFile()+CGameSettings::Active()->GetTerrainSaveFile();
			auto worldSpawnpoint = pl_stats->mPlayerSpawnpointMap.find( spawnpointFile );
			if ( worldSpawnpoint == pl_stats->mPlayerSpawnpointMap.end() ) {
				// Has not been on this world before, so generate spawnpoint now.
				// Set the spawn point
				//Vector3d_d newSpawnpoint = tRealmSelect.generator->GetSpawnPoint( 1, 0, targetRealm.GetPersonCount() );
				Vector3d_d newSpawnpoint = pl_stats->MakeSpawnpoint( tRealmSelect.generator );
				pl_stats->mPlayerSpawnpointMap[spawnpointFile] = newSpawnpoint;
				// Set the follow target a second time.
				//COctreeTerrain::GetActive()->SetStateFollowTarget( Vector3d(newSpawnpoint.x,newSpawnpoint.y,newSpawnpoint.z) );
				CVoxelTerrain::GetActive()->SetCenterPosition( newSpawnpoint );
				// Increment person count.
				CMCCRealm targetRealm;
				targetRealm.SetSavedSoulname( tCharSelect.list_chars[tCharSelect.selection].c_str() );
				targetRealm.IncPersonCount();
				// Save spawnpoints to file
				pl_stats->SaveToFile();
			}
			else {
				CMCCRealm targetRealm;
				targetRealm.SetSavedSoulname( tCharSelect.list_chars[tCharSelect.selection].c_str() );
			}
			gui->SetVisible( false );
			gui->active = false;
			return M_blendtogameplay;
		}
		else {
			// Need to select a character, fool!
		}
	}
	else if ( gui->GetButtonClicked( tCharSelect.btndeletechar ) )
	{
		Audio.playSound( "Menu.Click" );

		if ( tCharSelect.selection >= 0 ) {
			CGameSettings::Active()->SetPlayerSaveFile( tCharSelect.list_chars[ tCharSelect.selection ] );

			gui->ShowYesNoDialogue( tCharSelect.dlgdeleteconfirm );
		}
	}
	else
	{
		// Check for dialogue buttons
		int dlgresult = gui->GetDialogueResponse( tCharSelect.dlgdeleteconfirm );
		if ( dlgresult == 0 )
		{
			string dir = CGameSettings::Active()->GetPlayerSaveDir();
			fs::path playerdir( dir );
			fs::remove_all( playerdir );

			gui->SetElementText( tCharSelect.pghinfo, "" );

			tCharSelect.FreeCharList();
			tCharSelect.CreateCharList();

			gui->HideDialogue( tCharSelect.dlgdeleteconfirm );
		}
		else if ( dlgresult == 1 )
		{
			gui->HideDialogue( tCharSelect.dlgdeleteconfirm );
		}

		// Check for previous selection on this realm
		if ( gui->GetListviewSelection( tCharSelect.slsCharacterList ) == -1 ) {
			bool hasMatch = false;
			// With the realm, check for match
			for ( uint i = 0; i < tCharSelect.list_chars.size(); ++i )
			{
				if ( tCharSelect.last_selected.compare( tCharSelect.list_chars[i].c_str() ) )
				{	// Name matches, start up with this character
					gui->SetListviewSelection( tCharSelect.slsCharacterList, i );
					hasMatch = true;
				}
			}
			// If no match, then have a problem
			if ( !hasMatch )
			{
				// Look for a character that's been on this planet
				for ( uint c = 0; c < tCharSelect.list_chars.size(); ++c )
				{
					// Set game settings
					CGameSettings::Active()->SetPlayerSaveFile( tCharSelect.list_chars[c] );
					// Load stats
					pl_stats->LoadFromFile();
					// Search stats for this world
					string spawnpointFile = CGameSettings::Active()->GetWorldSaveFile()+CGameSettings::Active()->GetTerrainSaveFile();
					auto worldSpawnpoint = pl_stats->mPlayerSpawnpointMap.find( spawnpointFile );
					if ( worldSpawnpoint != pl_stats->mPlayerSpawnpointMap.end() ) {
						hasMatch = true;
						// World matches, use this character
						gui->SetListviewSelection( tCharSelect.slsCharacterList, c );
						break;
					}
				}
				// If still no match, stick character as ground
				if ( !hasMatch && m_playercharacter )
				{
					Vector3d_d delta = m_next_spawnpoint - CVoxelTerrain::GetActive()->GetCenterPosition();
					m_playercharacter->transform.position = Vector3d((Real)delta.x,(Real)delta.y,(Real)delta.z);
					m_playercharacter->transform.SetDirty();
				}
			}
		}
		// Check for character select buttons
		int nextSelection = gui->GetListviewSelection( tCharSelect.slsCharacterList );
		if ( nextSelection != tCharSelect.selection ) // If character selection has changed, load that character.
		{
			// Set game settings
			CGameSettings::Active()->SetPlayerSaveFile( tCharSelect.list_chars[ nextSelection ] );
			// Load stats
			pl_stats->LoadFromFile();

			// If have a character, update the model
			if ( m_playercharacter )
			{
				tCharSelect.selection = (int)(nextSelection);
				if ( p_model ) {
					p_model->SetVisualsFromStats( pl_stats->race_stats ); // Set visuals from stats
					p_model->SetVisibility ( true );
					p_model->UpdateTattoos();
				}
			}

			// Set stats of NPC in view
			if ( m_playercharacter ) {
				m_playercharacter->SetRaceStats( pl_stats->race_stats );
			}

			// Generate stat list (TODO, MAKE THIS A FUNCTION)
			string s_racetype = Races::GetCharacterDescriptor( pl_stats->race_stats );
			static char char_info [2048];
			{
				memset( char_info, 0, 2048 );
				sprintf( char_info,
					"%s %s\n Level %d\n %s\n\nStats:\n STR:%d\n AGI:%d\n INT:%d\n",
					pl_stats->race_stats->sPlayerName.c_str(), pl_stats->race_stats->sLastName.c_str(), pl_stats->stats->iLevel, s_racetype.c_str(),
					pl_stats->stats->iStrength,pl_stats->stats->iAgility,pl_stats->stats->iIntelligence );
				gui->SetElementText( tCharSelect.pghinfo, char_info );
			}

			// Temporarily load up the current realm.
			SetTerrainPlacementFromStats();
			
			// Set new position for the character
			if ( m_playercharacter ) {
				Vector3d_d delta = m_next_spawnpoint - CVoxelTerrain::GetActive()->GetCenterPosition();
				m_playercharacter->transform.position = Vector3d((Real)delta.x,(Real)delta.y,(Real)delta.z);
				m_playercharacter->transform.SetDirty();
			}
		}
		// 
	}
	

	return iMenuState;
}
void C_RMainMenu::SetTerrainPlacementFromStats ( void )
{
	// Load player's last position (if even applicable)
	auto worldSpawnpoint = pl_stats->mPlayerSpawnpointMap.find( CGameSettings::Active()->GetWorldSaveFile()+CGameSettings::Active()->GetTerrainSaveFile() );
	if ( worldSpawnpoint == pl_stats->mPlayerSpawnpointMap.end() ) {
		// (Use the generator to generate a new spawnpoint, and set it. Eventually have a button to rechoose spawnpoint).
		Vector3d_d newSpawnpoint = pl_stats->MakeSpawnpoint( tRealmSelect.generator );
		m_next_spawnpoint = newSpawnpoint;
		// (BUT DO NOT SET THE SPAWNPOINT)
		CVoxelTerrain::GetActive()->SetCenterPosition( newSpawnpoint );
	}
	else {
		// Load up that spawnpoint. Have the terrain follow it.
		CVoxelTerrain::GetActive()->SetCenterPosition( worldSpawnpoint->second );
		m_next_spawnpoint = worldSpawnpoint->second;
	}
}

void C_RMainMenu::sCharacterSelectElements_t::CreateCharList ( void )
{
	if ( !list_chars.empty() ) {
		return;
	}

	// Reset selection
	selection = -1;

	// Need to loop through all the found folders in the character directory
	// From game settings, get a list of all names in .games\.players
	std::vector<string> characters;
	CGameSettings::Active()->GetPlayerFileList( characters );

	// If there are characters, create the list of selectors
	char_num = characters.size();
	if ( !characters.empty() )
	{
		/*list_chars	= new Handle [char_num];
		list_strs	= new string [char_num];
		for ( uint i = 0; i < char_num; ++i )
		{
			list_chars[i] = gui->CreateButton( container );
			gui->SetElementRect( list_chars[i], Rect( 0.06f,0.13f + i*0.06f,0.2f,0.05f ) );
			gui->SetElementText( list_chars[i], characters[i] );

			list_strs[i] = characters[i];
		}*/
		for ( uint i = 0; i < char_num; ++i )
		{
			list_chars.push_back( characters[i] );
			gui->AddListviewOption( slsCharacterList, characters[i], i );
		}
	}
}
void C_RMainMenu::sCharacterSelectElements_t::FreeCharList ( void )
{
	gui->ClearListview( slsCharacterList );
	list_chars.clear();

	// Reset selection
	selection = -1;
}

C_RMainMenu::eMenuState	C_RMainMenu::stateBlendToGameplay ( void )
{
	// Move camera
	BlendWithSpeed( 2.5f );
	// Lerp to the position of the character's head
	stateTarget.cameraPos = m_playercharacter->GetEyeRay().pos; //Vector3d( -25,0,3010 ) + (Vector3d( -10,1.8,-3.1 )*(110+50-82+30));
	stateTarget.cameraRot = Quaternion::CreateRotationTo( Vector3d::forward, m_playercharacter->GetEyeRay().dir );
	stateTarget.cameraFoV = 100.0f;
	stateTarget.cameraRoll= 0;
	//stateTarget.cameraRot = //Vector3d( 0,20,-190 );
	//stateTarget.cameraFoV = 100.0f;
	//stateTarget.cameraRoll = -260;

	if ( fLerpValue > 0.95 )
	{
		// Save and delete stats
		pl_stats->SaveToFile();
		delete pl_stats->stats;
		delete pl_stats;
		pl_stats = NULL;

		// Remove character list
		tCharSelect.FreeCharList(); 

		// Save character position
		Vector3d tPosition = m_playercharacter->transform.position;

		// Need to delete self and go into game mode.
		DeleteObject( this );
		if ( m_playercharacter ) {
			DeleteObject( m_playercharacter );
		}
		m_playercharacter = NULL;

		// Create gametype
		m_gametype = new gametypeNotDying( m_gametype, tPosition );
	}

	return M_blendtogameplay;
}

C_RMainMenu::eMenuState	C_RMainMenu::stateBlendToGameplayNewChar ( void )
{
	BlendWithSpeed( 2.5f );

	if ( fLerpValue > 0.7f )
	{
		// At this point, wait for the terrain to finish loading.
		if ( !pPauseLoader->GetDoneLoad() ) {
			fLerpValue = 0.7f;

			// Print terrain loading information
			pPauseLoader->SetVisible( true );
		}
		else
		{
			// Save and delete stats
			pl_stats->SaveToFile();
			delete pl_stats->stats;
			delete pl_stats;
			pl_stats = NULL;

			// Remove character list
			tCharSelect.FreeCharList(); 

			// Save character position
			//Vector3d tPosition = m_playercharacter->transform.position;
			// Delete character
			if ( m_playercharacter ) {
				DeleteObject( m_playercharacter );
			}
			m_playercharacter = NULL;

			// Get the gamestate into game mode
			((gametypeMenu*)m_gametype)->InitWorld();

			// Need to delete self and go into game mode.
			DeleteObject( this );

			// Set everything to visible
			CVoxelTerrain* currentTerrain = CVoxelTerrain::GetActive();
			if ( currentTerrain ) {
				currentTerrain->Renderer->SetVisible( true );
			}

			// Create gametype
			//m_gametype = new gametypeNotDying( m_gametype, tPosition );
			//m_gametype = new gametypeNotDying( m_gametype, Vector3d( -17.1f,0.9f,16.3f ) );
			Vector3d_d localSpawnpoint = m_next_spawnpoint - currentTerrain->GetCenterPosition();
			m_gametype = new gametypeNotDying( m_gametype, Vector3d( (Real)localSpawnpoint.x,(Real)localSpawnpoint.y,(Real)localSpawnpoint.z ) );

			// Now fade
			new CScreenFade( true, 0.4f, 0.1f, Color(1,1,1) );

			// Turn off load checker
			pPauseLoader->active = false;
			pPauseLoader->SetVisible( false );

			return M_charselect;
		}
	}

	return M_blendtogameplaystart;
}

C_RMainMenu::eMenuState	C_RMainMenu::stateBlendToCharCreation ( void )
{
	// Move camera
	BlendWithSpeed(1.2f);
	
	// Set new camera pos
	stateTarget.cameraPos = Vector3d( -25,0,210 );
	stateTarget.cameraRot = Vector3d( 0,0,-170 );
	stateTarget.cameraFoV = 100.0f;
	stateTarget.cameraRoll = -360;
	// Move time of day
	stateTarget.timeOfDay = 60*60*(6);
	stateTarget.spaceEffect = 0.5;

	// Go to actual thing at end
	if ( fLerpValue > 0.97 )
	{
		tCharModel.lookatCameraOffset *= 0.0f;
		// Disable the terrain
		CVoxelTerrain* currentTerrain = CVoxelTerrain::GetActive();
		if ( currentTerrain ) {
			currentTerrain->SetSystemPaused( true ); // Run the terrain.
			currentTerrain->Renderer->SetVisible( false );
			currentTerrain->Renderer->ClearAll(); // Clear the collision and all meshes
		}
		// Delete the character
		if ( m_playercharacter ) {
			DeleteObject( m_playercharacter );
		}
		m_playercharacter = NULL;
		// Reset char creation state
		tCharCreation.state = 0;

		return M_charcreation;
	}
	return M_blendtocharcreation;
}


C_RMainMenu::eMenuState	C_RMainMenu::stateCharCreation ( void )
{
	// Move camera
	BlendWithSpeed(1.2f);
	// Set new camera pos
	if ( p_char_creator )
	{
		p_char_creator->GetCameraOverview( pCamera );
		stateTarget.cameraPos = pCamera->transform.position;
		stateTarget.cameraRot = pCamera->transform.rotation;
	}
	else
	{
		/*if ( m_playercharacter ) {
			stateTarget.cameraPos = Vector3d( -16,0,21.7f )-Vector3d( -17.1f,0.9f,16.3f ) + m_playercharacter->transform.position;//p_model->transform.position;
			stateTarget.cameraRot = Rotator(Vector3d( 0,-3,-150 ));
		}*/
	}
	stateTarget.cameraRoll = 0.0f;
	stateTarget.cameraFoV = 90.0f;
	// Move time of day
	stateTarget.timeOfDay = 60*60*(24+3.5f);

	// Toggle object visibility based on state
	gui->SetElementVisible( tCharCreation.pgh_soulname, 0 );
	gui->SetElementVisible( tCharCreation.fld_soulname, 0 );
	gui->SetElementVisible( tCharCreation.btn_soulnamecontinue, 0 );

	if ( p_char_creator == NULL )
	{
		p_char_creator = new C_RCharacterCreator();
	}

	// Create a random character to start with.
	if ( m_playercharacter == NULL )
	{
		uint64_t player_id = NPC::Manager->RequestNPC( NPC::npcid_SETPIECE, 0 );
		{
			NPC::characterFile_t characterFile;
			// Create new stats
			/*pl_stats->SetDefaults();
			pl_stats->race_stats->iRace = (eCharacterRace) (rand()%5 + 1);
			pl_stats->race_stats->RerollColors();*/
			characterFile.rstats = p_char_creator->GetPlayerStats()->race_stats;//pl_stats->race_stats;
			
			NPC::sWorldState worldstate;
			characterFile.worldstate = &worldstate;
			worldstate.mFocus = NPC::AIFOCUS_None;
			worldstate.mFocusName = "WUPWUPWUPWUP";
			worldstate.worldPosition = Vector3d( -17.1f,0.9f,16.3f );//pPlayerStats->vPlayerInitSpawnPoint;
			worldstate.partyHost = 1024;

			NPC::sOpinions opinions;
			characterFile.opinions = &opinions;
			NPC::sPreferences prefs;
			characterFile.prefs = &prefs;

			// Save generated stats
			NPC::CZonedCharacterIO io;
			io.CreateCharacterFile( player_id, characterFile );
		}
		m_playercharacter = (NPC::CNpcBase*) NPC::Manager->SpawnNPC(player_id);
	}

	// Enable character create menu
	gui->SetElementVisible( tCharCreation.container, p_char_creator->GetCanCancel() );
	// Press back
	if ( p_char_creator->GetCanCancel() && gui->GetButtonClicked( tCharCreation.btnback ) )
	{
		Audio.playSound( "Menu.Click" );

		gui->SetElementVisible( tCharCreation.container, false );
		if ( p_char_creator != NULL ) {
			delete_safe( p_char_creator );
		}
		if ( tCharCreation.state == 1 ) {
			// Remove created player
			string dir = CGameSettings::Active()->GetPlayerSaveDir();
			fs::path playerdir( dir );
			if ( fs::exists( playerdir ) ) {
				fs::remove_all( playerdir );
			}
		}

		// Reenable the terrain
		CVoxelTerrain* currentTerrain = CVoxelTerrain::GetActive();
		if ( currentTerrain ) {
			currentTerrain->ResetSystem();
			currentTerrain->SetSystemPaused( false ); // Stop the terrain.
			currentTerrain->Renderer->SetVisible( true );
		}
		
		// Delete character created
		delete_safe( m_playercharacter );
		m_playercharacter = NULL;

		// Delete character creator effects

		//return M_charselect;
		return M_realmselect;
	}

	/*if ( tCharCreation.state == 0 )
	{
		// Work on the textbox
		gui->UpdateTextfield( tCharCreation.fld_soulname, tCharCreation.cur_soulname );
		// Check if the name is valid. If it's not valid, hide the freaking thing
		int c_count = 0;
		for ( unsigned int i = 0; i < tCharCreation.cur_soulname.size(); ++i )
		{
			unsigned char c = tCharCreation.cur_soulname[i];
			if ( tCharCreation.cur_soulname[0] == ' ' ) {
				c_count -= 16;
			}
			if ( isalpha(c) ) {
				c_count += 1;
			}
			else if ( (c=='\'') || (c=='`') || (c=='-') || (c==' ') ) {
				;
			}
			else {
				c_count -= 100; // Invalid name
			}
		}
		boost::filesystem::path playerdir( CGameSettings::Active()->GetPlayerSaveDir(tCharCreation.cur_soulname) );
		gui->SetElementVisible( tCharCreation.btn_soulnamecontinue, ((c_count >= 2)&&( !boost::filesystem::exists( playerdir ) )) );

		if ( gui->GetButtonClicked( tCharCreation.btn_soulnamecontinue ) )
		{
			// Set file to given name
			CGameSettings::Active()->SetPlayerSaveFile( tCharCreation.cur_soulname );
			// Create player folder
			CGameSettings::Active()->GetPlayerSaveDir();

			// Go to next state
			tCharCreation.state = 1;
		}
	}
	else if ( tCharCreation.state == 1 )
	{*/
	// Create editor
	if ( p_char_creator )
	{
		/*if ( p_model )
		{
			p_char_creator->SetVisualModel( p_model );
			p_model->SetVisibility( true );
		}*/
		p_char_creator->SetVisualModel( m_playercharacter );
		// Check for finished editing
		/*if ( p_char_creator->IsDoneEditing() )
		{
			gui->SetElementVisible( tCharCreation.container, false );
			if ( p_char_creator != NULL ) {
				delete_safe( p_char_creator );
			}

			// Reenable the terrain
			COctreeTerrain* currentTerrain = COctreeTerrain::GetActive();
			if ( currentTerrain ) {
				currentTerrain->SetSystemPaused( false ); // Run the terrain.
				currentTerrain->GetRenderer()->visible = true;
			}

			return M_charselect;
		}*/
		// If done editing, then go directly to the game with the new character
		if ( p_char_creator->IsDoneEditing() )
		{
			// Go to game

			// Turn off load checker
			pPauseLoader->active = false;
			pPauseLoader->SetVisible( false );

			// Get the gamestate into game mode
			((gametypeMenu*)m_gametype)->InitWorld();

			// Start up the load checker
			pPauseLoader->active = true;
			pPauseLoader->Reset();
			pPauseLoader->m_targetLoadcheckPosition = Vector3d(0,0,0);

			// Delete char creator
			delete_safe( p_char_creator );

			// Load player stats
			pl_stats->LoadFromFile();

			// System is ready, start up the terrain system
			CVoxelTerrain* currentTerrain = CVoxelTerrain::GetActive();
			if ( currentTerrain ) {
				//currentTerrain->State_ClearInformation(); // Clear information
				currentTerrain->SetGenerator(tRealmSelect.generator);
				currentTerrain->ResetSystem(); // Clear information
				currentTerrain->SetSystemPaused( false ); // Run the terrain.
			}

			// Check if this character has been on this world before.
			string spawnpointFile = CGameSettings::Active()->GetWorldSaveFile()+CGameSettings::Active()->GetTerrainSaveFile();
			{
				// Has not been on this world before, so generate spawnpoint now.
				// Set the spawn point
				Vector3d_d newSpawnpoint = pl_stats->MakeSpawnpoint( tRealmSelect.generator );
				pl_stats->mPlayerSpawnpointMap[spawnpointFile] = newSpawnpoint;
				m_next_spawnpoint = newSpawnpoint;
				// Set the follow target a second time.
				CVoxelTerrain::GetActive()->SetCenterPosition( newSpawnpoint );
				// Increment person count.
				CMCCRealm targetRealm;
				targetRealm.SetSavedSoulname( CGameSettings::Active()->GetPlayerSaveFile().c_str() );
				targetRealm.IncPersonCount();
				// Save spawnpoints to file
				pl_stats->SaveToFile();
			}

			{
				// Load stats
				pl_stats->LoadFromFile();
				// Temporarily load up the current realm. Again.
				SetTerrainPlacementFromStats();
			}

			// And turn gui off
			gui->SetElementVisible( tCharCreation.container, false );

			// UGH THIS IS GONNA BE BITCHY. NEED ANOTHER STEP FOR THE LOADING WORLD
			return M_blendtogameplaystart;
		}
	}
	//}

	return iMenuState;
}