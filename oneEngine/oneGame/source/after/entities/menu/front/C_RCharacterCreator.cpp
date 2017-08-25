
#include "C_RCharacterCreator.h"

#include "after/states/CharacterStats.h"
#include "after/states/player/CPlayerStats.h"
#include "after/entities/character/npc/CNpcBase.h"
#include "after/entities/character/npc/zoned/CZonedCharacter.h"
#include "after/interfaces/io/CZonedCharacterIO.h"

//#include "boost/filesystem.hpp"
#include "core/settings/CGameSettings.h"

#include "after/interfaces/CAfterCharacterAnimator.h"

#include "renderer/camera/CCamera.h"
#include "core/math/Math.h"
#include "core/math/random/Random.h"

#include "engine-common/entities/CParticleSystem.h"
#include "after/entities/test/CTestPhysicsFloor.h"

#include "renderer/object/shapes/CCubeRenderablePrimitive.h"

#include "core-ext/system/io/Resources.h"

#include <filesystem>
namespace fs = std::tr2::sys;

C_RCharacterCreator::C_RCharacterCreator ( void )
	: CGameBehavior(), pl_stats( NULL ), m_playercharacter( NULL )//pl_model( NULL )
{
	Random.Seed( uint32_t(Time::currentTime*2000) + Random.Next() );

	// Setup player stats
	pl_stats = new CPlayerStats();
	pl_stats->SetDefaults();
	pl_stats->race_stats->iRace = (eCharacterRace) ((Random.Next()%5) + 1);
	pl_stats->race_stats->RerollColors();

	// Setup default stuff
	tMainAttr.str_soulname = "";

	// Create GUI and init elements
	gui = new CDuskGUI ();
	CreateGUIElements();

	// NOT done editing
	b_done_editing = false;

	//p_model->SetFaceAtRotation( Rotator(0,0,-40) );
	rModelRotation = Rotator(0,0,-40);
	//rCameraRotation = Rotator(0,0,10);
	vCameraRotation = Vector3d( 0,0,10 );

	// Magic showoff effect
	particlesLeftHand = NULL;
	particlesRightHand= NULL;

	// Create the stage
	stageFloor = new CTestPhysicsFloor;
	((CTestPhysicsFloor*)stageFloor)->pRenderable->SetVisible( false );
}

C_RCharacterCreator::~C_RCharacterCreator ( void )
{
	delete_safe( gui );

	if ( pl_stats ) {
		delete pl_stats->stats;
		delete pl_stats;
		pl_stats = NULL;
	}

	delete_safe( particlesLeftHand );
	delete_safe( particlesRightHand );

	delete_safe( stageFloor );
}

void C_RCharacterCreator::SetVisualModel ( CMccCharacterModel* model )
{
	//pl_model = model;
}
void C_RCharacterCreator::SetVisualModel ( NPC::CNpcBase* npc )
{
	m_playercharacter = npc;
}


bool C_RCharacterCreator::IsDoneEditing ( void )
{
	return b_done_editing;
}
bool C_RCharacterCreator::GetCanCancel ( void )
{
	return iMenuState == M_main;
}

void C_RCharacterCreator::CreateGUIElements ( void )
{
	iMenuState = M_main;

	// Main character creation panel
	{
		// Create container
		tMainAttr.container = gui->CreatePanel();
		//gui->SetElementRect( tMainAttr.container, Rect( -1,-1,-1,-1 ) );
		gui->SetElementRect( tMainAttr.container, Rect( 0.03f,0.03f,0.27f,0.84f ) );
		gui->SetElementText( tMainAttr.container, "New Character Creation" );
		gui->SetElementVisible( tMainAttr.container, false );

		tMainAttr.btncontinue = gui->CreateButton( tMainAttr.container );
		gui->SetElementRect( tMainAttr.btncontinue, Rect( 0.18f,0.9f,0.18f,0.05f ) );
		gui->SetElementText( tMainAttr.btncontinue, "Create Character" );
		gui->SetElementVisible( tMainAttr.btncontinue, false );

		// Soulname box
		tMainAttr.lbl_soulname = gui->CreateText( tMainAttr.container, "Soul Name" );
		gui->SetElementRect( tMainAttr.lbl_soulname, Rect( 0.09f, 0.11f, 0.2f, 0.027f ) );

		tMainAttr.fld_soulname = gui->CreateTextfield( tMainAttr.container, tMainAttr.str_name );
		gui->SetElementRect( tMainAttr.fld_soulname, Rect( 0.11f, 0.15f, 0.15f, 0.04f ) );

		// Name box
		tMainAttr.lbl_name = gui->CreateText( tMainAttr.container, "Personal Given Name" );
		gui->SetElementRect( tMainAttr.lbl_name, Rect( 0.09f, 0.19f, 0.2f, 0.027f ) );

		tMainAttr.fld_name = gui->CreateTextfield( tMainAttr.container, tMainAttr.str_name );
		gui->SetElementRect( tMainAttr.fld_name, Rect( 0.11f, 0.23f, 0.15f, 0.04f ) );


		// Cloud or no cloud choice
		tMainAttr.lbl_cloudmode = gui->CreateText( tMainAttr.container, "Synchronisation" );
		gui->SetElementRect( tMainAttr.lbl_cloudmode, Rect( 0.09f, 0.75f, 0.2f, 0.027f ) );

		tMainAttr.ddl_cloudmode_choice = gui->CreateDropdownList( tMainAttr.container );
		gui->SetElementRect( tMainAttr.ddl_cloudmode_choice, Rect( 0.11f, 0.79f, 0.15f, 0.04f ) );
		gui->AddDropdownOption( tMainAttr.ddl_cloudmode_choice, "Offline", 0 );
		gui->AddDropdownOption( tMainAttr.ddl_cloudmode_choice, "More Offline", 0 );

		// Gametype choice
		tMainAttr.lbl_gametype = gui->CreateText( tMainAttr.container, "Gametype" );
		gui->SetElementRect( tMainAttr.lbl_gametype, Rect( 0.09f, 0.65f, 0.2f, 0.027f ) );

		tMainAttr.ddl_gametype_choice = gui->CreateDropdownList( tMainAttr.container );
		gui->SetElementRect( tMainAttr.ddl_gametype_choice, Rect( 0.11f, 0.69f, 0.15f, 0.04f ) );
		gui->AddDropdownOption( tMainAttr.ddl_gametype_choice, "Not Dying", 0 );

		// Submenu boxes
		tMainAttr.btn_colors = gui->CreateButton( tMainAttr.container );
		gui->SetElementRect( tMainAttr.btn_colors, Rect( 0.04f, 0.45f, 0.12f, 0.04f ) );
		gui->SetElementText( tMainAttr.btn_colors, "Colors" );
		tMainAttr.btn_stats = gui->CreateButton( tMainAttr.container );
		gui->SetElementRect( tMainAttr.btn_stats, Rect( 0.17f, 0.45f, 0.12f, 0.04f ) );
		gui->SetElementText( tMainAttr.btn_stats, "Stats" );
		tMainAttr.btn_style = gui->CreateButton( tMainAttr.container );
		gui->SetElementRect( tMainAttr.btn_style, Rect( 0.04f, 0.50f, 0.12f, 0.04f ) );
		gui->SetElementText( tMainAttr.btn_style, "Style" );
		tMainAttr.btn_companion = gui->CreateButton( tMainAttr.container );
		gui->SetElementRect( tMainAttr.btn_companion, Rect( 0.17f, 0.50f, 0.12f, 0.04f ) );
		gui->SetElementText( tMainAttr.btn_companion, "Companion" );
		tMainAttr.btn_markings = gui->CreateButton( tMainAttr.container );
		gui->SetElementRect( tMainAttr.btn_markings, Rect( 0.04f, 0.55f, 0.12f, 0.04f ) );
		gui->SetElementText( tMainAttr.btn_markings, "Markings" );
		tMainAttr.btn_appearance = gui->CreateButton( tMainAttr.container );
		gui->SetElementRect( tMainAttr.btn_appearance, Rect( 0.17f, 0.55f, 0.12f, 0.04f ) );
		gui->SetElementText( tMainAttr.btn_appearance, "Appearance" );

		tMainAttr.btn_randomize = gui->CreateButton( tMainAttr.container );
		gui->SetElementRect( tMainAttr.btn_randomize, Rect( 0.04f, 0.60f, 0.12f, 0.04f ) );
		gui->SetElementText( tMainAttr.btn_randomize, "Radomize Marks" );
		tMainAttr.btn_reroll = gui->CreateButton( tMainAttr.container );
		gui->SetElementRect( tMainAttr.btn_reroll, Rect( 0.17f, 0.60f, 0.12f, 0.04f ) );
		gui->SetElementText( tMainAttr.btn_reroll, "Reroll Colors" );


		// Gender choice
		tMainAttr.lbl_gender = gui->CreateText( tMainAttr.container, "Gender" );
		gui->SetElementRect( tMainAttr.lbl_gender, Rect( 0.09f, 0.35f, 0.2f, 0.027f ) );

		tMainAttr.ddl_gender_choice = gui->CreateDropdownList( tMainAttr.container );
		gui->SetElementRect( tMainAttr.ddl_gender_choice, Rect( 0.11f, 0.39f, 0.15f, 0.04f ) );
		gui->AddDropdownOption( tMainAttr.ddl_gender_choice, "Female", CGEND_FEMALE );
		gui->AddDropdownOption( tMainAttr.ddl_gender_choice, "Male", CGEND_MALE );
		gui->SetDropdownValue( tMainAttr.ddl_gender_choice, pl_stats->race_stats->iGender );

		// Race choice
		tMainAttr.lbl_race = gui->CreateText( tMainAttr.container, "Race" );
		gui->SetElementRect( tMainAttr.lbl_race, Rect( 0.09f, 0.27f, 0.2f, 0.027f ) );

		tMainAttr.ddl_race_choice = gui->CreateDropdownList( tMainAttr.container );
		gui->SetElementRect( tMainAttr.ddl_race_choice, Rect( 0.11f, 0.31f, 0.15f, 0.04f ) );
		gui->AddDropdownOption( tMainAttr.ddl_race_choice, "Human", CRACE_HUMAN );
		gui->AddDropdownOption( tMainAttr.ddl_race_choice, "Dark Elf", CRACE_ELF );
		gui->AddDropdownOption( tMainAttr.ddl_race_choice, "Dwarf", CRACE_DWARF );
		gui->AddDropdownOption( tMainAttr.ddl_race_choice, "Catperson", CRACE_KITTEN );
		gui->AddDropdownOption( tMainAttr.ddl_race_choice, "Fluxxor", CRACE_FLUXXOR );
		gui->SetDropdownValue( tMainAttr.ddl_race_choice, pl_stats->race_stats->iRace );

	}
	
	//sColorElements_t
	{
		// Create container
		tCharColors.container = gui->CreatePanel();
		gui->SetElementRect( tCharColors.container, Rect( 0.03f,0.03f,0.27f,0.84f ) );
		gui->SetElementVisible( tCharColors.container, false );
		gui->SetElementText( tCharColors.container, "Change Colors" );

		tCharColors.btndone = gui->CreateButton( tCharColors.container );
		gui->SetElementRect( tCharColors.btndone, Rect( 0.18f,0.81f,0.1f,0.05f ) );
		gui->SetElementText( tCharColors.btndone, "Done" );

		tCharColors.lbl_eyecolor = gui->CreateText( tCharColors.container, "Eyes" );
		gui->SetElementRect( tCharColors.lbl_eyecolor, Rect( 0.09f, 0.25f, 0.2f, 0.027f ) );
		tCharColors.cs_eyecolor = gui->CreateColorPicker( tCharColors.container, Color( 0.1f,0.5f,0.7f,1.0f ) );
		gui->SetElementRect( tCharColors.cs_eyecolor, Rect( 0.16f, 0.25f, 0.07f, 0.04f ) );

		tCharColors.lbl_focuscolor = gui->CreateText( tCharColors.container, "Focus" );
		gui->SetElementRect( tCharColors.lbl_focuscolor, Rect( 0.09f, 0.35f, 0.2f, 0.027f ) );
		tCharColors.cs_focuscolor = gui->CreateColorPicker( tCharColors.container, Color( 0.2f,0.8f,1.0f,1.0f ) );
		gui->SetElementRect( tCharColors.cs_focuscolor, Rect( 0.16f, 0.35f, 0.07f, 0.04f ) );

		tCharColors.lbl_skincolor = gui->CreateText( tCharColors.container, "Skin" );
		gui->SetElementRect( tCharColors.lbl_skincolor, Rect( 0.09f, 0.55f, 0.2f, 0.027f ) );
		tCharColors.cs_skincolor = gui->CreateColorPicker( tCharColors.container );
		gui->SetElementRect( tCharColors.cs_skincolor, Rect( 0.16f, 0.55f, 0.07f, 0.04f ) );

		tCharColors.lbl_haircolor = gui->CreateText( tCharColors.container, "Hair" );
		gui->SetElementRect( tCharColors.lbl_haircolor, Rect( 0.09f, 0.65f, 0.2f, 0.027f ) );
		tCharColors.cs_haircolor = gui->CreateColorPicker( tCharColors.container, Color( 0.67f,0.34f,0.09f,1.0f ) );
		gui->SetElementRect( tCharColors.cs_haircolor, Rect( 0.16f, 0.65f, 0.07f, 0.04f ) );
	}

	//sStatElements_t
	{
		// Create container
		tCharStats.container = gui->CreatePanel();
		gui->SetElementRect( tCharStats.container, Rect( 0.03f,0.03f,0.27f,0.84f ) );
		gui->SetElementVisible( tCharStats.container, false );
		gui->SetElementText( tCharStats.container, "Character Stats" );
		
		tCharStats.btndone = gui->CreateButton( tCharStats.container );
		gui->SetElementRect( tCharStats.btndone, Rect( 0.18f,0.81f,0.1f,0.05f ) );
		gui->SetElementText( tCharStats.btndone, "Back" );

		tCharStats.lbl_txt_str = gui->CreateText( tCharStats.container );
		gui->SetElementRect( tCharStats.lbl_txt_str, Rect( 0.04f, 0.35f, 0.2f,0.027f ) );
		tCharStats.lbl_txt_agi = gui->CreateText( tCharStats.container );
		gui->SetElementRect( tCharStats.lbl_txt_agi, Rect( 0.04f, 0.40f, 0.2f,0.027f ) );
		tCharStats.lbl_txt_int = gui->CreateText( tCharStats.container );
		gui->SetElementRect( tCharStats.lbl_txt_int, Rect( 0.04f, 0.45f, 0.2f,0.027f ) );

		tCharStats.lbl_txt_hp = gui->CreateText( tCharStats.container );
		gui->SetElementRect( tCharStats.lbl_txt_hp, Rect( 0.12f, 0.35f, 0.2f,0.027f ) );
		tCharStats.lbl_txt_sp = gui->CreateText( tCharStats.container );
		gui->SetElementRect( tCharStats.lbl_txt_sp, Rect( 0.12f, 0.40f, 0.2f,0.027f ) );
		tCharStats.lbl_txt_mp = gui->CreateText( tCharStats.container );
		gui->SetElementRect( tCharStats.lbl_txt_mp, Rect( 0.12f, 0.45f, 0.2f,0.027f ) );

		tCharStats.pgh_race_desc = gui->CreateParagraph( tCharStats.container );
		gui->SetElementRect( tCharStats.pgh_race_desc, Rect( 0.05f,0.08f,0.23f,0.2f ) );
		tCharStats.pgh_race_buff = gui->CreateParagraph( tCharStats.container );
		gui->SetElementRect( tCharStats.pgh_race_buff, Rect( 0.05f,0.50f,0.23f,0.2f ) );
	}

	//sStyleElements_t
	{
		// Create container
		tCharStyle.container = gui->CreatePanel();
		gui->SetElementRect( tCharStyle.container, Rect( 0.03f,0.03f,0.27f,0.84f ) );
		gui->SetElementVisible( tCharStyle.container, false );
		gui->SetElementText( tCharStyle.container, "Change style" );

		tCharStyle.btndone = gui->CreateButton( tCharStyle.container );
		gui->SetElementRect( tCharStyle.btndone, Rect( 0.18f,0.81f,0.1f,0.05f ) );
		gui->SetElementText( tCharStyle.btndone, "Done" );

		// Hairstyle
		tCharStyle.lbl_hair = gui->CreateText( tCharStyle.container, "Hair" );
		gui->SetElementRect( tCharStyle.lbl_hair, Rect( 0.09f, 0.25f, 0.2f, 0.027f ) );

		tCharStyle.ddl_hair_choice = gui->CreateDropdownList( tCharStyle.container );
		gui->SetElementRect( tCharStyle.ddl_hair_choice, Rect( 0.11f, 0.29f, 0.15f, 0.04f ) );
		gui->AddDropdownOption( tCharStyle.ddl_hair_choice, "Sidekick", 0 );
		gui->AddDropdownOption( tCharStyle.ddl_hair_choice, "Spikes", 1 );
		gui->AddDropdownOption( tCharStyle.ddl_hair_choice, "Kude", 2 );
		gui->SetDropdownValue( tCharStyle.ddl_hair_choice, pl_stats->race_stats->iHairstyle );

		// Motion style
		tCharStyle.lbl_movement = gui->CreateText( tCharStyle.container, "Motion and Combat" );
		gui->SetElementRect( tCharStyle.lbl_movement, Rect( 0.09f, 0.35f, 0.2f, 0.027f ) );

		tCharStyle.ddl_movement_choice = gui->CreateDropdownList( tCharStyle.container );
		gui->SetElementRect( tCharStyle.ddl_movement_choice, Rect( 0.11f, 0.39f, 0.15f, 0.04f ) );
		gui->AddDropdownOption( tCharStyle.ddl_movement_choice, "Normal", CMOVES_DEFAULT );
		gui->AddDropdownOption( tCharStyle.ddl_movement_choice, "Gloomy", CMOVES_GLOOMY );
		gui->AddDropdownOption( tCharStyle.ddl_movement_choice, "Jolly", CMOVES_JOLLY );
		gui->AddDropdownOption( tCharStyle.ddl_movement_choice, "Sassy", CMOVES_SASSY );
		gui->AddDropdownOption( tCharStyle.ddl_movement_choice, "Large Ham", CMOVES_LARGE_HAM );
		//gui->AddDropdownOption( tCharStyle.ddl_movement_choice, "Primal",  );
		gui->SetDropdownValue( tCharStyle.ddl_movement_choice, pl_stats->race_stats->iMovestyle );

		// Speech style
		tCharStyle.lbl_speech = gui->CreateText( tCharStyle.container, "Speech and Replies" );
		gui->SetElementRect( tCharStyle.lbl_speech, Rect( 0.09f, 0.45f, 0.2f, 0.027f ) );

		tCharStyle.ddl_speech_choice = gui->CreateDropdownList( tCharStyle.container );
		gui->SetElementRect( tCharStyle.ddl_speech_choice, Rect( 0.11f, 0.49f, 0.15f, 0.04f ) );
		gui->AddDropdownOption( tCharStyle.ddl_speech_choice, "Normal", 0 );
		gui->AddDropdownOption( tCharStyle.ddl_speech_choice, "Arrogant", 1 );
		gui->AddDropdownOption( tCharStyle.ddl_speech_choice, "Nyormal", 2 );
		gui->AddDropdownOption( tCharStyle.ddl_speech_choice, "Santa", 3 );
		gui->SetDropdownValue( tCharStyle.ddl_speech_choice, pl_stats->race_stats->iTalkstyle );
	}

	//sCompanionElements_t
	{
		// Create container
		tCompAttr.container = gui->CreatePanel();
		gui->SetElementRect( tCompAttr.container, Rect( 0.03f,0.03f,0.27f,0.84f ) );
		gui->SetElementVisible( tCompAttr.container, false );
		gui->SetElementText( tCompAttr.container, "Edit Companion" );

		tCompAttr.btndone = gui->CreateButton( tCompAttr.container );
		gui->SetElementRect( tCompAttr.btndone, Rect( 0.05f,0.81f,0.1f,0.05f ) );
		gui->SetElementText( tCompAttr.btndone, "Done" );

		// Gender
		tCompAttr.lbl_gender = gui->CreateText( tCompAttr.container, "Gender" );
		gui->SetElementRect( tCompAttr.lbl_gender, Rect( 0.04f, 0.15f, 0.2f, 0.027f ) );

		tCompAttr.ddl_gender_choice = gui->CreateDropdownList( tCompAttr.container );
		gui->SetElementRect( tCompAttr.ddl_gender_choice, Rect( 0.06f, 0.19f, 0.15f, 0.04f ) );
		gui->AddDropdownOption( tCompAttr.ddl_gender_choice, "Random", 0 );
		gui->AddDropdownOption( tCompAttr.ddl_gender_choice, "Same", 1 );
		gui->AddDropdownOption( tCompAttr.ddl_gender_choice, "Opposite", 2 );

		// Race
		tCompAttr.lbl_race = gui->CreateText( tCompAttr.container, "Race" );
		gui->SetElementRect( tCompAttr.lbl_race, Rect( 0.04f, 0.25f, 0.2f, 0.027f ) );

		tCompAttr.ddl_race_choice = gui->CreateDropdownList( tCompAttr.container );
		gui->SetElementRect( tCompAttr.ddl_race_choice, Rect( 0.06f, 0.29f, 0.15f, 0.04f ) );
		gui->AddDropdownOption( tCompAttr.ddl_race_choice, "Same", 0 );
		gui->AddDropdownOption( tCompAttr.ddl_race_choice, "Random", 1 );

		// Romantics
		tCompAttr.lbl_love = gui->CreateText( tCompAttr.container, "Romance Preference" );
		gui->SetElementRect( tCompAttr.lbl_love, Rect( 0.04f, 0.35f, 0.2f, 0.027f ) );

		tCompAttr.ddl_love_choice = gui->CreateDropdownList( tCompAttr.container );
		gui->SetElementRect( tCompAttr.ddl_love_choice, Rect( 0.06f, 0.39f, 0.22f, 0.04f ) );
		gui->AddDropdownOption( tCompAttr.ddl_love_choice, "Random", 0 );
		gui->AddDropdownOption( tCompAttr.ddl_love_choice, "Player, but Not Gay", 1 );
		gui->AddDropdownOption( tCompAttr.ddl_love_choice, "Player, Only Female", 2 );
		gui->AddDropdownOption( tCompAttr.ddl_love_choice, "Always Player", 3 );
		gui->AddDropdownOption( tCompAttr.ddl_love_choice, "Others, but Not Gay", 4 );
		gui->AddDropdownOption( tCompAttr.ddl_love_choice, "Others, Only Female", 5 );
		gui->AddDropdownOption( tCompAttr.ddl_love_choice, "Always Others", 6 );
		gui->AddDropdownOption( tCompAttr.ddl_love_choice, "Anyone, but Not Gay", 7 );
		gui->AddDropdownOption( tCompAttr.ddl_love_choice, "Anyone, Only Female", 8 );
		gui->AddDropdownOption( tCompAttr.ddl_love_choice, "Anyone, anywhere", 9 );

		tCompAttr.pgh_comp_desc = gui->CreateParagraph( tCompAttr.container );
		gui->SetElementRect( tCompAttr.pgh_comp_desc, Rect( 0.05f,0.45f,0.23f,0.4f ) );
		gui->SetElementText( tCompAttr.pgh_comp_desc,
			"The Companion is your number one fan. They'll follow you if you ask them, or leave you alone. When you're near death, they will come to give assistance."
			);
	}

	//sTattooElements_t
	{
		// Create container
		//tCharMarks.container = gui->CreateDraggablePanel();
		tCharMarks.container = gui->CreatePanel();
		gui->SetElementRect( tCharMarks.container, Rect( 0.03f,0.03f,0.27f,0.94f ) );
		gui->SetElementVisible( tCharMarks.container, false );
		gui->SetElementText( tCharMarks.container, "Marks, Scars and Tattoos" );
		
		tCharMarks.btndone = gui->CreateButton( tCharMarks.container );
		gui->SetElementRect( tCharMarks.btndone, Rect( 0.18f,0.91f,0.1f,0.05f ) );
		gui->SetElementText( tCharMarks.btndone, "Back" );

		tCharMarks.btn_next_tattoo = gui->CreateButton( tCharMarks.container );
		tCharMarks.btn_next_tattoo.SetRect( Rect(0.25f,0.10f,0.04f,0.04f) );
		tCharMarks.btn_next_tattoo.SetText( ">" );
		tCharMarks.btn_prev_tattoo = gui->CreateButton( tCharMarks.container );
		tCharMarks.btn_prev_tattoo.SetRect( Rect(0.04f,0.10f,0.04f,0.04f) );
		tCharMarks.btn_prev_tattoo.SetText( "<" );
		
		tCharMarks.lbl_current_tattoo = gui->CreateText( tCharMarks.container, "0/0" );
		tCharMarks.lbl_current_tattoo.SetRect( Rect(0.09f,0.10f,0.15f,0.04f) );

		tCharMarks.current_loaded_tattoo = 0;
		tCharMarks.need_list_refresh = false;

		tCharMarks.btn_add_tattoo = gui->CreateButton( tCharMarks.container );
		tCharMarks.btn_add_tattoo.SetRect( Rect( 0.04f,0.15f,0.12f,0.04f ) );
		tCharMarks.btn_add_tattoo.SetText( "Add Mark" );
		tCharMarks.btn_delete_tattoo = gui->CreateButton( tCharMarks.container );
		tCharMarks.btn_delete_tattoo.SetRect( Rect( 0.17f,0.15f,0.12f,0.04f ) );
		tCharMarks.btn_delete_tattoo.SetText( "Delete Mark" );



		tCharMarks.lbl_position = gui->CreateText( tCharMarks.container, "Source Position" );
		tCharMarks.lbl_position.SetRect( Rect( 0.06f, 0.21f, 0.2f, 0.027f ) );
		tCharMarks.fld_position = gui->CreateVector3dPicker( tCharMarks.container );
		tCharMarks.fld_position.SetRect( Rect( 0.04f,0.25f,0.25f,0.04f ) );
		tCharMarks.lbl_tattoo_facing = gui->CreateText( tCharMarks.container, "Paste Direction" );
		tCharMarks.lbl_tattoo_facing.SetRect( Rect( 0.06f, 0.29f, 0.2f, 0.027f ) );
		tCharMarks.ddl_tattoo_facing = gui->CreateDropdownList( tCharMarks.container );
		tCharMarks.ddl_tattoo_facing.SetRect( Rect( 0.06f, 0.33f, 0.21f, 0.04f ) );
		gui->AddDropdownOption( tCharMarks.ddl_tattoo_facing, "On Character", 0 );
		gui->AddDropdownOption( tCharMarks.ddl_tattoo_facing, "Align to front", 1 );
		gui->AddDropdownOption( tCharMarks.ddl_tattoo_facing, "Align to side", 2 );
		gui->AddDropdownOption( tCharMarks.ddl_tattoo_facing, "Align horizontally", 3 );
		
		tCharMarks.chk_mirrored = gui->CreateCheckbox( tCharMarks.container );
		tCharMarks.chk_mirrored.SetRect( Rect( 0.06f,0.39f,0.25f,0.04f ) );
		//tCharMarks.chk_mirrored.SetText( "Place Symmetrically" );
		tCharMarks.lbl_mirrored = gui->CreateText( tCharMarks.container, "Place Symmetrically" );
		tCharMarks.lbl_mirrored.SetRect( Rect( 0.09f,0.38f,0.2f,0.027f ) );

		tCharMarks.lbl_scale = gui->CreateText( tCharMarks.container, "Mark Size" );
		tCharMarks.lbl_scale.SetRect( Rect( 0.06f, 0.42f,0.2f, 0.027f ) );
		tCharMarks.sdr_scale = gui->CreateSlider( tCharMarks.container );
		tCharMarks.sdr_scale.SetRect( Rect( 0.06f, 0.46f,0.21f,0.04f ) );
		gui->SetSliderMinMax( tCharMarks.sdr_scale, 0.1f, 2.0f );

		tCharMarks.chk_flip = gui->CreateCheckbox( tCharMarks.container );
		tCharMarks.chk_flip.SetRect( Rect( 0.06f,0.52f,0.25f,0.04f ) );
		tCharMarks.lbl_flip = gui->CreateText( tCharMarks.container, "Flip image" );
		tCharMarks.lbl_flip.SetRect( Rect( 0.09f,0.51f,0.2f,0.027f ) );

		tCharMarks.lbl_curvature = gui->CreateText( tCharMarks.container, "Angle" );
		tCharMarks.lbl_curvature.SetRect( Rect( 0.06f, 0.55f,0.2f, 0.027f ) );
		tCharMarks.sdr_curvature = gui->CreateSlider( tCharMarks.container );
		tCharMarks.sdr_curvature.SetRect( Rect( 0.06f, 0.59f,0.21f,0.04f ) );
		gui->SetSliderMinMax( tCharMarks.sdr_curvature, -180.0f,180.0f );



		tCharMarks.lbl_tattoo_type = gui->CreateText( tCharMarks.container, "Type" );
		tCharMarks.lbl_tattoo_type.SetRect( Rect( 0.06f, 0.21f+0.44f, 0.2f ,0.027f ) );
		tCharMarks.ddl_tattoo_type = gui->CreateDropdownList( tCharMarks.container );
		tCharMarks.ddl_tattoo_type.SetRect( Rect( 0.06f, 0.25f+0.44f, 0.21f, 0.04f ) );
		gui->AddDropdownOption( tCharMarks.ddl_tattoo_type, "Clan Markings", 0 );
		gui->AddDropdownOption( tCharMarks.ddl_tattoo_type, "Tattoos", 1 );
		gui->AddDropdownOption( tCharMarks.ddl_tattoo_type, "Scars", 2 );
		gui->AddDropdownOption( tCharMarks.ddl_tattoo_type, "Fluxxmarks", 3 );
		tCharMarks.lbl_tattoo_index = gui->CreateText( tCharMarks.container, "Pattern" );
		tCharMarks.lbl_tattoo_index.SetRect( Rect( 0.06f, 0.29f+0.44f, 0.2f ,0.027f ) );
		tCharMarks.ddl_tattoo_index = gui->CreateDropdownList( tCharMarks.container );
		tCharMarks.ddl_tattoo_index.SetRect( Rect( 0.06f, 0.33f+0.44f, 0.21f, 0.04f ) );



		tCharMarks.lbl_blend = gui->CreateText( tCharMarks.container, "Blending Color" );
		tCharMarks.lbl_blend.SetRect( Rect( 0.04f, 0.85f, 0.1f, 0.027f ) );
		tCharMarks.clr_blend = gui->CreateColorPicker( tCharMarks.container );
		tCharMarks.clr_blend.SetRect( Rect( 0.21f,0.85f,0.07f,0.04f ) );

		tCharMarks.vTattooPosition = Vector3d( 0,0,24 );
		tCharMarks.fTattooScaling = 0.5f;

		tCharMarks.need_redraw_tattoos = true;

		tCharMarks.reinitialize = true;
	}
	//sVisualsElements_t
	{
		// Create container
		tCharLooks.container = gui->CreatePanel();
		gui->SetElementRect( tCharLooks.container, Rect( 0.03f,0.03f,0.27f,0.84f ) );
		gui->SetElementVisible( tCharLooks.container, false );
		gui->SetElementText( tCharLooks.container, "Facial and Body Features" );
		
		tCharLooks.btndone = gui->CreateButton( tCharLooks.container );
		gui->SetElementRect( tCharLooks.btndone, Rect( 0.18f,0.81f,0.1f,0.05f ) );
		gui->SetElementText( tCharLooks.btndone, "Back" );
	}
}


Color	GetFocusColor ( Color baseColor )
{
	ftype centerPoint = (baseColor.red*0.299f + baseColor.green*0.587f + baseColor.blue*0.114f) * 0.5f + 0.25f;
	baseColor.red += (baseColor.red - centerPoint)*3.0f;
	baseColor.green += (baseColor.green - centerPoint)*3.0f;
	baseColor.blue += (baseColor.blue - centerPoint)*3.0f;
	baseColor.alpha = (baseColor.alpha + 1.0f)*0.5f;
	return baseColor;
}
void C_RCharacterCreator::Update ( void )
{
	switch ( iMenuState )
	{
	case M_main:	stateMain(); break;
	case M_colors:	stateColors(); break;
	case M_stats:	stateStats(); break;
	case M_style:	stateStyle(); break;
	case M_friend:	stateCompanion(); break;
	case M_tattoos: stateTattoos(); break; //tattoos should be draggable or static panel?
							// tattoos should be able to drag character around.
	case M_visual:	stateVisual(); break;
	};

	/*if ( pl_model )
	{
		if ( particlesLeftHand ) {
			particlesLeftHand->enabled = false;
		}
		if ( particlesRightHand ) {
			particlesRightHand->enabled = false;
		}
		if ( iMenuState == M_colors && gui->ColorPickerInDialogue(tCharColors.cs_focuscolor) ) {
			pl_model->SetMoveAnimation( "idle_magic_showoff" );
			if ( !particlesLeftHand ) {
				particlesLeftHand = new CParticleSystem( ".res/particlesystems/spells/magic_showoff.pcf" );
			}
			if ( !particlesRightHand ) {
				particlesRightHand = new CParticleSystem( ".res/particlesystems/spells/magic_showoff.pcf" );
			}
			XTransform xtrans;
			pl_model->GetProp01Transform(xtrans);
			particlesLeftHand->transform.position = xtrans.position;
			particlesLeftHand->enabled = true;
			particlesLeftHand->GetRenderable(0)->GetMaterial()->m_diffuse = GetFocusColor( pl_stats->race_stats->cFocusColor );
			pl_model->GetProp02Transform(xtrans);
			particlesRightHand->transform.position = xtrans.position;
			particlesRightHand->enabled = true;
			particlesRightHand->GetRenderable(0)->GetMaterial()->m_diffuse = GetFocusColor( pl_stats->race_stats->cFocusColor );
		}
		else if ( iMenuState == M_stats ) {
			pl_model->SetMoveAnimation( "idle_02" );
		}
		else {
			static CAfterCharacterAnimator anim;
			anim.m_model = pl_model;
			anim.m_combat_info = NULL;
			anim.m_inventory = NULL;
			anim.m_race_stats = pl_stats->race_stats;
			anim.m_stats = pl_stats->stats;
			pl_stats->stats->fMana = pl_stats->stats->fManaMax;

			anim.SetMoveAnimation( NPC::MoveAnimWalkIdle );
			anim.DoMoveAnimation();
		}
		pl_model->SetVisualsFromStats( pl_stats->race_stats );

		pl_model->SetFaceAtRotation( rModelRotation );

		if ( tCharMarks.need_redraw_tattoos ) {
			pl_stats->UpdateStats();
			pl_model->UpdateTattoos();
			tCharMarks.need_redraw_tattoos = false;
		}
	}*/
	if ( m_playercharacter )
	{	// The following code mirrors CNpcBase::SetRaceStats
		//(*m_playercharacter->GetRacialStats()) = *pl_stats->race_stats;
		// Set stats
		m_playercharacter->GetRacialStats()->CopyFrom( pl_stats->race_stats );
		// Load new visuals
		((CMccCharacterModel*)m_playercharacter->GetCharModel())->SetVisualsFromStats( pl_stats->race_stats );

		if ( tCharMarks.need_redraw_tattoos ) {
			pl_stats->UpdateStats();
			((CMccCharacterModel*)m_playercharacter->GetCharModel())->UpdateTattoos();
			tCharMarks.need_redraw_tattoos = false; // Only update once
		}

		if ( iMenuState == M_tattoos || iMenuState == M_visual ) {
			m_playercharacter->GetAI()->RequestLookatOverride ( CCamera::activeCamera->transform.position );
		}
	}

	if ( pl_stats )
	{
		// Update properties
		gui->UpdateTextfield( tMainAttr.fld_name, tMainAttr.str_name );
		pl_stats->race_stats->sPlayerName = tMainAttr.str_name;
		gui->UpdateTextfield( tMainAttr.fld_soulname, tMainAttr.str_soulname );

		gui->UpdateColorPicker( tCharColors.cs_eyecolor, pl_stats->race_stats->cEyeColor );
		gui->UpdateColorPicker( tCharColors.cs_focuscolor, pl_stats->race_stats->cFocusColor );

		Color previousSkinColor = pl_stats->race_stats->cSkinColor;
		gui->UpdateColorPicker( tCharColors.cs_skincolor, pl_stats->race_stats->cSkinColor );
		gui->UpdateColorPicker( tCharColors.cs_haircolor, pl_stats->race_stats->cHairColor );
		if ( previousSkinColor != pl_stats->race_stats->cSkinColor )
		{	// If skin color is different, need to redraw tats to fix the fringes
			tCharMarks.need_redraw_tattoos = true;
		}

		eCharacterRace pRace = pl_stats->race_stats->iRace;
		pl_stats->race_stats->iRace = (eCharacterRace)gui->GetDropdownOption( tMainAttr.ddl_race_choice );
		pl_stats->race_stats->iGender = (eCharacterGender)gui->GetDropdownOption( tMainAttr.ddl_gender_choice );

		// Load race values
		pl_stats->stats->UpdateStats();
		pl_stats->race_stats->SetLuaDefaults();

		// Set hair type
		//pl_model->SetHair( gui->GetDropdownOption( tCharStyle.ddl_hair_choice ) );
		pl_stats->race_stats->iHairstyle = gui->GetDropdownOption( tCharStyle.ddl_hair_choice );

		// Set move type
		pl_stats->race_stats->iMovestyle = gui->GetDropdownOption( tCharStyle.ddl_movement_choice );
		if ( pRace != pl_stats->race_stats->iRace )
		{
			switch ( pl_stats->race_stats->iRace ) {
			case CRACE_ELF:		pl_stats->race_stats->iMovestyle = CMOVES_GLOOMY;	break;
			case CRACE_KITTEN:	pl_stats->race_stats->iMovestyle = CMOVES_JOLLY;	break;
			default:			pl_stats->race_stats->iMovestyle = CMOVES_DEFAULT;	break;
			}
			gui->SetDropdownValue( tCharStyle.ddl_movement_choice, pl_stats->race_stats->iMovestyle );
		}

		// Set companion info
		switch ( gui->GetDropdownOption( tCompAttr.ddl_love_choice ) ) {
			case 0:	pl_stats->companion_love_prefs = CLOVE_RANDOM;			break;
			case 1:
			case 2:
			case 3: pl_stats->companion_love_prefs = CLOVE_PLAYER;			break;
			case 4:
			case 5:
			case 6: pl_stats->companion_love_prefs = CLOVE_NOT_PLAYER;		break;
			case 7:
			case 8:
			case 9: pl_stats->companion_love_prefs = CLOVE_NO_PREFERENCE;	break;
		}
		switch ( gui->GetDropdownOption( tCompAttr.ddl_love_choice ) ) {
			case 0:	pl_stats->companion_sex_prefs = SEXPREF_RANDOM;		break;
			case 1:
			case 4:
			case 7: pl_stats->companion_sex_prefs = SEXPREF_STRAIGHT;	break;
			case 2:
			case 5:
			case 8: pl_stats->companion_sex_prefs = SEXPREF_ONLY_FEMALE;	break;
			case 3:
			case 6:
			case 9: pl_stats->companion_sex_prefs = SEXPREF_BISEXUAL;	break;
		}
		switch ( gui->GetDropdownOption( tCompAttr.ddl_gender_choice ) ) {
			case 0:	pl_stats->companion_gender	= CGEND_RANDOM;		break;
			case 1:	pl_stats->companion_gender	= CGEND_SAME;		break;
			case 2:	pl_stats->companion_gender	= CGEND_OPPOSITE;	break;
		}
		switch ( gui->GetDropdownOption( tCompAttr.ddl_race_choice ) ) {
			case 0:	pl_stats->companion_race	= CRACE_SAME;		break;
			case 1:	pl_stats->companion_race	= CRACE_RANDOM;		break;
		}


	}

	// Check ALL of the stats to make sure valid values are selected
	CheckColors();

	gui->SetColorPicker( tCharColors.cs_skincolor, pl_stats->race_stats->cSkinColor );
}

bool C_RCharacterCreator::DoCameraOverview ( void )
{
	return ( iMenuState == M_tattoos /*|| iMenuState == M_visual*/ );
}

void C_RCharacterCreator::stateMain ( void )
{
	gui->SetElementVisible( tMainAttr.container, true );

	// Based on current names, toggle availability of continue button
	{
		bool validCharacter = true;
		// Check character name
		if ( tMainAttr.str_name.size() <= 1 ) {
			validCharacter = false;
		}
		// Check soul name
		int c_count = 0;
		for ( unsigned int i = 0; i < tMainAttr.str_soulname.size(); ++i )
		{
			unsigned char c = tMainAttr.str_soulname[i];
			if ( tMainAttr.str_soulname[0] == ' ' ) {
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
		fs::path playerdir( CGameSettings::Active()->GetPlayerSaveDir(tMainAttr.str_soulname) );
		if ( (c_count < 2)||( fs::exists( playerdir ) ) ) {
			validCharacter = false;
		}
		// Set visibility of continue button
		gui->SetElementVisible( tMainAttr.btncontinue, validCharacter );
	}
	// Check for going to other states
	if ( gui->GetButtonClicked( tMainAttr.btn_colors ) )
	{
		gui->SetElementVisible( tMainAttr.container, false );
		iMenuState = M_colors;
	}
	else if ( gui->GetButtonClicked( tMainAttr.btn_stats ) )
	{
		gui->SetElementVisible( tMainAttr.container, false );
		iMenuState = M_stats;
	}
	else if ( gui->GetButtonClicked( tMainAttr.btn_style ) )
	{
		gui->SetElementVisible( tMainAttr.container, false );
		iMenuState = M_style;
	}
	else if ( gui->GetButtonClicked( tMainAttr.btn_companion ) )
	{
		gui->SetElementVisible( tMainAttr.container, false );
		iMenuState = M_friend;
	}
	else if ( gui->GetButtonClicked( tMainAttr.btn_markings ) )
	{
		gui->SetElementVisible( tMainAttr.container, false );
		iMenuState = M_tattoos;
	}
	else if ( gui->GetButtonClicked( tMainAttr.btn_appearance ) )
	{
		gui->SetElementVisible( tMainAttr.container, false );
		iMenuState = M_visual;
	}
	else if ( gui->GetButtonClicked( tMainAttr.btn_randomize ) )
	{
		TattooGUIRandomizeTattoos();
	}
	else if ( gui->GetButtonClicked( tMainAttr.btn_reroll ) )
	{
		pl_stats->race_stats->RerollColors();
	}
	// Check for finishing character creation
	else if ( gui->GetButtonClicked( tMainAttr.btncontinue ) )
	{
		CGameSettings::Active()->SetPlayerSaveFile( tMainAttr.str_soulname );
		pl_stats->SaveToFile();
		b_done_editing = true;
	}
}

void C_RCharacterCreator::stateColors ( void )
{
	gui->SetElementVisible( tCharColors.container, true );

	// Go to other states
	if ( gui->GetButtonClicked( tCharColors.btndone ) )
	{
		gui->SetElementVisible( tCharColors.container, false );
		iMenuState = M_main;
	}
}

void C_RCharacterCreator::stateStats ( void )
{
	gui->SetElementVisible( tCharStats.container, true );
	// Move race dialogue to here
	gui->SetElementParent( tMainAttr.lbl_race, tCharStats.container );
	gui->SetElementParent( tMainAttr.ddl_race_choice, tCharStats.container );

	// Set the stat text based on the player's stats
	char sTemp [256];
	sprintf( sTemp, "Str: %d", pl_stats->stats->iStrength );
	gui->SetElementText( tCharStats.lbl_txt_str, sTemp );
	sprintf( sTemp, "Agi: %d", pl_stats->stats->iAgility );
	gui->SetElementText( tCharStats.lbl_txt_agi, sTemp );
	sprintf( sTemp, "Int: %d", pl_stats->stats->iIntelligence );
	gui->SetElementText( tCharStats.lbl_txt_int, sTemp );

	sprintf( sTemp, "Max Health: %d", (int)pl_stats->stats->fHealthMax );
	gui->SetElementText( tCharStats.lbl_txt_hp, sTemp );
	sprintf( sTemp, "Max Stamina: %d", (int)pl_stats->stats->fStaminaMax );
	gui->SetElementText( tCharStats.lbl_txt_sp, sTemp );
	sprintf( sTemp, "Max Energy: %d", (int)pl_stats->stats->fManaMax );
	gui->SetElementText( tCharStats.lbl_txt_mp, sTemp );

	gui->SetElementText( tCharStats.pgh_race_buff, Races::GetRaceBuffDesc( pl_stats->race_stats->iRace ) );
	gui->SetElementText( tCharStats.pgh_race_desc, Races::GetRaceGeneralDesc( pl_stats->race_stats->iRace ) );

	// Go to other states
	if ( gui->GetButtonClicked( tCharStats.btndone ) )
	{
		// Move race dialogue back home
		gui->SetElementParent( tMainAttr.lbl_race, tMainAttr.container );
		gui->SetElementParent( tMainAttr.ddl_race_choice, tMainAttr.container );

		gui->SetElementVisible( tCharStats.container, false );
		iMenuState = M_main;
	}
}

void C_RCharacterCreator::stateStyle ( void )
{
	gui->SetElementVisible( tCharStyle.container, true );

	// Go to other states
	if ( gui->GetButtonClicked( tCharStyle.btndone ) )
	{
		gui->SetElementVisible( tCharStyle.container, false );
		iMenuState = M_main;
	}
}

void C_RCharacterCreator::stateCompanion ( void )
{
	gui->SetElementVisible( tCompAttr.container, true );

	// Go to other states
	if ( gui->GetButtonClicked( tCompAttr.btndone ) )
	{
		gui->SetElementVisible( tCompAttr.container, false );
		iMenuState = M_main;
	}
}

void C_RCharacterCreator::CheckColors ( void )
{
	pl_stats->race_stats->LimitColors();
	// Skin colors
	/*Color skinHue;
	pl_stats->race_stats->cSkinColor.GetHSLC(skinHue);

	switch ( pl_stats->race_stats->iRace ) {
	case CRACE_HUMAN:
	case CRACE_KITTEN:
	case CRACE_DWARF:
		if ( skinHue.red > 175 ) {
			skinHue.red = 30.0f;
			skinHue.green = 0.6f;
			skinHue.blue = 0.8f;
		}
		skinHue.red		= Math.Clamp( skinHue.red, 20.0f, 40.0f );
		skinHue.green	= Math.Clamp( skinHue.green, 0.1f,0.8f );
		skinHue.blue	= Math.Clamp( skinHue.blue, 0.3f, 0.9f );
		break;
	case CRACE_ELF:
		if ( skinHue.red < 45 ) {
			skinHue.red = 240.0f;
			skinHue.green = 0.1f;
			skinHue.blue = 0.4f;
		}
		skinHue.red		= Math.Clamp( skinHue.red, 180.0f, 280.0f );
		skinHue.green	= Math.Clamp( skinHue.green, 0.0f,0.5f );
		skinHue.blue	= Math.Clamp( skinHue.blue, 0.2f, 0.8f );
		break;
	case CRACE_FLUXXOR:
		if ( skinHue.red < 45 ) {
			skinHue.red = 165.0f;
			skinHue.green = 0.3f;
			skinHue.blue = 0.4f;
		}
		skinHue.red		= Math.Clamp( skinHue.red, 145.0f, 205.0f );
		skinHue.green	= Math.Clamp( skinHue.green, 0.0f, 0.4f );
		skinHue.blue	= Math.Clamp( skinHue.blue, 0.35f, 0.6f );
		break;
	}

	pl_stats->race_stats->cSkinColor.SetHSL( skinHue );


	// Set Color Alpha
	pl_stats->race_stats->cFocusColor.alpha	= 1;
	pl_stats->race_stats->cEyeColor.alpha	= 1;
	pl_stats->race_stats->cSkinColor.alpha	= 1;
	pl_stats->race_stats->cHairColor.alpha	= 1;*/
}

void C_RCharacterCreator::stateVisual ( void )
{
	gui->SetElementVisible( tCharLooks.container, true );

	// Go to other states
	if ( gui->GetButtonClicked( tCharLooks.btndone ) )
	{
		gui->SetElementVisible( tCharLooks.container, false );
		iMenuState = M_main;
		return;
	}
}

void C_RCharacterCreator::GetCameraOverview ( CCamera* cam )
{
	/*if ( CInput::mouse[ CInput::MBLeft ] )
	{
		Matrix4x4 rotMatx;
		rotMatx.setRotation( pCamera->transform.rotation );
		tCharModel.lookatCameraOffset -= rotMatx * Vector3d( 0, (ftype)CInput::deltaMouseX, (ftype)CInput::deltaMouseY ) * 0.02f;
	}*/
	if ( iMenuState == M_tattoos )
	{
		vCameraRotation.y = -13;
		if ( !gui->GetMouseInGUI() )
		{
			if ( CInput::Mouse( CInput::MBLeft ) )
			{
				//Matrix4x4 rotMatx;
				//rotMatx.setRotation( pCamera->transform.rotation );
				//tCharModel.lookatCameraOffset -= rotMatx * Vector3d( 0, (ftype)CInput::deltaMouseX, (ftype)CInput::deltaMouseY ) * 0.02f;
				//rCameraRotation = rCameraRotation * Rotator(0,0,(ftype)CInput::deltaMouseX);
				vCameraRotation.z += (ftype)CInput::DeltaMouseX();
			}
		}

		//Matrix4x4 rotMatx;
		//cam->transform.rotation = rCameraRotation;
		//cam->transform.rotation.Euler( vCameraRotation );
		/*cam->transform.rotation = Rotator(vCameraRotation).getQuaternion();
		cam->transform.rotation *= Rotator(0,0,-90) * rModelRotation;*/
		cam->transform.rotation = Rotator(0,0,-90) * rModelRotation;
		cam->transform.rotation *= Rotator(vCameraRotation);
		//rotMatx.setRotation( cam->transform.rotation );

		//XTransform eyeCam;
		//pl_model->GetEyecamTransform(eyeCam);
		//m_playercharacter->GetCharModel()->GetEyecamTransform(eyeCam);
		XTransform headCam;
		m_playercharacter->GetCharModel()->GetHeadTransform( headCam );
		cam->transform.position = m_playercharacter->transform.position.lerp(headCam.position,0.7f)+ cam->transform.rotation*Vector3d( -3.5f,1.2f,0 );
	}
	else
	{
		//vCamPosNext = Vector3d( -16,0,21.7f );
		//vCamRotNext = -Vector3d( 0,-3,-150 );
		if ( iMenuState == M_colors && gui->ColorPickerInDialogue(tCharColors.cs_focuscolor) ) {
			cam->transform.position = Vector3d( -16.3f,-0.5f,21.2f ) - Vector3d( -17.1f,0.9f,16.3f ) + m_playercharacter->transform.position;
			cam->transform.rotation = Rotator(Vector3d( 0,-3,-130 )).getQuaternion();
		}
		else {
			cam->transform.position = Vector3d( -16,0,21.7f ) - Vector3d( -17.1f,0.9f,16.3f ) + m_playercharacter->transform.position;
			cam->transform.rotation = Rotator(Vector3d( 0,-3,-150 )).getQuaternion();
		}
	}
}



//
// ==== TATTOO EDITOR ====
//
#include "renderer/debug/CDebugDrawer.h"
#include "core/debug/CDebugConsole.h"

void C_RCharacterCreator::stateTattoos ( void )
{
	gui->SetElementVisible( tCharMarks.container, true );

	// Go to other states
	if ( gui->GetButtonClicked( tCharMarks.btndone ) )
	{
		gui->SetElementVisible( tCharMarks.container, false );
		iMenuState = M_main;
		return;
	}

	// Now, GUI shit
	int prev_loaded_tattoo = tCharMarks.current_loaded_tattoo;
	if ( tCharMarks.btn_add_tattoo.GetButtonClicked() )
	{
		tCharMarks.current_loaded_tattoo = pl_stats->race_stats->tattooList.size();

		CRacialStats::tattoo_t newTat;
		newTat.color = Color( 0.5f,0.5f,0.5f,1.0f );
		newTat.pattern = "clan_laak";
		newTat.projection_dir = Vector3d( 0,1,0 );
		newTat.projection_pos = Vector3d( 0,-1.25f,0.5f );
		newTat.projection_angle = 0;
		newTat.projection_scale = Vector3d( 0.5f,0.5f,0.5f );
		newTat.mirror = false;
		newTat.type = 0;
		pl_stats->race_stats->tattooList.push_back( newTat );

		TattooGUILoadTattoo( tCharMarks.current_loaded_tattoo );

		tCharMarks.need_redraw_tattoos = true;
	}
	else if ( tCharMarks.btn_delete_tattoo.GetButtonClicked() )
	{
		pl_stats->race_stats->tattooList.erase( pl_stats->race_stats->tattooList.begin() + tCharMarks.current_loaded_tattoo );
		tCharMarks.current_loaded_tattoo -= 1;

		tCharMarks.need_redraw_tattoos = true;
	}
	if ( pl_stats->race_stats->tattooList.empty() )
	{
		tCharMarks.btn_next_tattoo.SetVisible( false );
		tCharMarks.btn_prev_tattoo.SetVisible( false );
	}
	else
	{
		tCharMarks.btn_next_tattoo.SetVisible( true );
		tCharMarks.btn_prev_tattoo.SetVisible( true );

		if ( tCharMarks.btn_next_tattoo.GetButtonClicked() ) {
			tCharMarks.current_loaded_tattoo += 1;
		}
		else if ( tCharMarks.btn_prev_tattoo.GetButtonClicked() ) {
			tCharMarks.current_loaded_tattoo -= 1;
		}
		if ( tCharMarks.current_loaded_tattoo < 0 ) {
			tCharMarks.current_loaded_tattoo = pl_stats->race_stats->tattooList.size()-1;
		}
		else if ( tCharMarks.current_loaded_tattoo == pl_stats->race_stats->tattooList.size() ) {
			tCharMarks.current_loaded_tattoo = 0;
		}

		if (( tCharMarks.reinitialize )||( prev_loaded_tattoo != tCharMarks.current_loaded_tattoo )) {
			TattooGUILoadTattoo( tCharMarks.current_loaded_tattoo );
			tCharMarks.reinitialize = false;
		}
	}
	
	if ( !pl_stats->race_stats->tattooList.empty() )
	{
		CRacialStats::tattoo_t prevTat = pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo];
		// place the tattoo
		gui->UpdateVector3dPicker( tCharMarks.fld_position, tCharMarks.vTattooPosition );
		pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_pos = tCharMarks.vTattooPosition / 12.0f;
		// scale the tattoo
		gui->UpdateSlider( tCharMarks.sdr_scale, tCharMarks.fTattooScaling );
		gui->UpdateCheckbox( tCharMarks.chk_flip, tCharMarks.cur_flipped );
		pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_scale = Vector3d((tCharMarks.cur_flipped)?-1.0f:1.0f,1,1) * tCharMarks.fTattooScaling;
		// align the tattoo
		int alignment = gui->GetDropdownOption( tCharMarks.ddl_tattoo_facing );
		if ( alignment == 0 ) {
			pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_dir = -pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_pos;
			pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_dir.z *= 0.2f;
			pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_dir.normalize();
		}
		else if ( alignment == 2 ) {
			pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_dir = Vector3d( (pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_pos.x > 0) ? -1.0f : 1.0f ,0,0 );
		}
		else if ( alignment == 1 ) {
			pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_dir = Vector3d( 0,(pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_pos.y > 0) ? -1.0f : 1.0f ,0 );
		}
		else if ( alignment == 3 ) {
			pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_dir = Vector3d( 0,0, (pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_pos.z > 0) ? -1.0f : 1.0f );
		}
		// angle the tattoo
		gui->UpdateSlider( tCharMarks.sdr_curvature, pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_angle );
		// color the tattoo
		gui->UpdateColorPicker( tCharMarks.clr_blend, pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].color );
		// mirror the tattoo
		gui->UpdateCheckbox( tCharMarks.chk_mirrored, pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].mirror );
		// set tattoo type
		pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].type = gui->GetDropdownOption( tCharMarks.ddl_tattoo_type );
		// set tattoo name
		pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].pattern = tCharMarks.vTattooNames[gui->GetDropdownOption( tCharMarks.ddl_tattoo_index )].c_str();
		// tattoo limiting!
		TattooGUILimitTattoo( pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo] );

		// check tat, update if modified
		if (( prevTat.color != pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].color )
			||( prevTat.mirror != pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].mirror )
			||( prevTat.pattern != pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].pattern )
			||( fabs(prevTat.projection_angle-pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_angle) > FTYPE_PRECISION )
			||( prevTat.projection_dir != pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_dir )
			||( prevTat.projection_pos != pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_pos )
			||( prevTat.projection_scale != pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_scale )
			||( prevTat.type != pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].type ))
		{
			tCharMarks.need_redraw_tattoos = true;
		}
		else {
			tCharMarks.need_redraw_tattoos = false;
		}

		// Draw projection pos and such too
		Debug::Drawer->DrawLine(
			m_playercharacter->transform.position + rModelRotation*pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_pos + Vector3d(0,0,3),
			m_playercharacter->transform.position + rModelRotation*pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_pos + Vector3d(0,0,3) + rModelRotation*pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_dir,
			Color( 1.0f,0.6f,0.6f,1.0f ) );
	}

	int old_tattoo_type = tCharMarks.cur_tattoo_type;
	tCharMarks.cur_tattoo_type = gui->GetDropdownOption( tCharMarks.ddl_tattoo_type );
	if ( tCharMarks.cur_tattoo_type != old_tattoo_type ) { 
		TattooGUIGenerateDropdown( tCharMarks.cur_tattoo_type );
	}
}
void C_RCharacterCreator::TattooGUIRefreshList ( void )
{

}
#include "core/utils/StringUtils.h"
void C_RCharacterCreator::TattooGUIGenerateDropdown ( int typeList )
{
	gui->SetDropdownOption( tCharMarks.ddl_tattoo_type, typeList );
	gui->ClearDropdownList( tCharMarks.ddl_tattoo_index );
	tCharMarks.vTattooNames.clear();

	string target_section;
	int selection = gui->GetDropdownOption( tCharMarks.ddl_tattoo_type );

	if ( selection == 0 ) {
		target_section = "clanmark";
	}
	else if ( selection == 1 ) {
		target_section = "tattoo";
	}
	else if ( selection == 2 ) {
		target_section = "scar";
	}
	else if ( selection == 3 ) {
		target_section = "fluxxglows";
	}

	// Open system/tattoo_manifest.txt and find target section
	std::ifstream tattoo_manifest ( Core::Resources::PathTo( "system/tattoo_manifest.txt" ) );
	if ( !tattoo_manifest.is_open() ) {
		Debug::Console->PrintError( "Could not read manifest!\n" );
	}
	string tempRdString;
	bool found_section = false;
	while ( !found_section && tattoo_manifest.good() ) {
		tattoo_manifest >> tempRdString;
		tempRdString = StringUtils::FullTrim( tempRdString );
		if ( tempRdString[0] == '$' ) {
			if ( tempRdString.find( target_section ) != string::npos ) {
				found_section = true;
			}
		}
	}

	// Now, read in line combos
	char	cmd [256];
	string	sName, sFn;
	int i, readamt;
	while ( tattoo_manifest.good() && found_section )
	{
		// Load in the next line
		memset( cmd, 0, 256 );
		tattoo_manifest.getline( cmd, 256 );
		if ( !tattoo_manifest.good() ) { continue; }
		readamt = strlen( cmd );
		
		// Look for quote and load in until quote hit
		sName.clear();
		i = 0;
		while ( cmd[i] != '"' && (i<readamt) ) {
			if ( cmd[i] == '$' ) {
				found_section = false;
			}
			++i;
		}
		++i;
		while ( cmd[i] != '"' && (i<readamt) ) {
			sName += cmd[i];
			++i;
		}
		++i;

		// Skip spaces
		while ( (cmd[i] == ' ' || cmd[i] == '\t') && (i<readamt) ) {
			++i;
		}

		// Now get the filename
		sFn.clear();
		while (( i < readamt )&&( cmd[i] != '\n' ))
		{
			sFn += cmd[i];
			++i;
		}

		// Now, open the textures/tats/ folder and find files with matching name
		// Or, you know, add it to a list. Because that's easier.
		if ( sFn.length() > 0 ) {
			gui->AddDropdownOption( tCharMarks.ddl_tattoo_index, sName, tCharMarks.vTattooNames.size() );
			tCharMarks.vTattooNames.push_back( sFn );
		}
	}

}


void C_RCharacterCreator::TattooGUILoadTattoo( int listIndex )
{
	tCharMarks.current_loaded_tattoo = listIndex;

	char temp_str [512];
	sprintf( temp_str, "%d/%d", listIndex+1, pl_stats->race_stats->tattooList.size() );
	tCharMarks.lbl_current_tattoo.SetText( temp_str );

	// load tat
	tCharMarks.vTattooPosition = pl_stats->race_stats->tattooList[listIndex].projection_pos * 12;
	tCharMarks.fTattooScaling = fabs(pl_stats->race_stats->tattooList[listIndex].projection_scale.x);

	// get tat type from name

	// get tat type from tat type
	int type = pl_stats->race_stats->tattooList[listIndex].type;

	// set type lists
	gui->SetDropdownOption( tCharMarks.ddl_tattoo_type, type );
	tCharMarks.cur_tattoo_type = gui->GetDropdownOption( tCharMarks.ddl_tattoo_type );
	TattooGUIGenerateDropdown( type );
	// find selection in list
	string tatName = string(pl_stats->race_stats->tattooList[listIndex].pattern.c_str());
	bool found = false;
	for ( uint i = 0; i < tCharMarks.vTattooNames.size(); ++i )
	{
		if ( tCharMarks.vTattooNames[i] == tatName ) {
			gui->SetDropdownOption( tCharMarks.ddl_tattoo_index, i );
			found = true;
			break;
		}
	}
	if( !found ) {
		std::cout << "Could not find \"" << tatName << "\" in list" << std::endl;
		for ( uint i = 0; i < tCharMarks.vTattooNames.size(); ++i )
		{
			std::cout << "--" << tCharMarks.vTattooNames[i] << std::endl;
		}
	}
	/*
	// set tattoo type
	pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].type = gui->GetDropdownOption( tCharMarks.ddl_tattoo_type );
	// set tattoo name
	pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].pattern = tCharMarks.vTattooNames[gui->GetDropdownOption( tCharMarks.ddl_tattoo_index )].c_str();
	*/

	// figure out alignement with dot products
	int projection_type = 0;
	if ( fabs(pl_stats->race_stats->tattooList[listIndex].projection_dir.dot( Vector3d(0,1,1) )) < 1e-6 ) {
		projection_type = 2;
	}
	else if ( fabs(pl_stats->race_stats->tattooList[listIndex].projection_dir.dot( Vector3d(1,0,1) )) < 1e-6 ) {
		projection_type = 1;
	}
	else if ( fabs(pl_stats->race_stats->tattooList[listIndex].projection_dir.dot( Vector3d(1,1,0) )) < 1e-6 ) {
		projection_type = 3;
	}
	gui->SetDropdownOption( tCharMarks.ddl_tattoo_facing, projection_type );

	// Set flipped
	if ( pl_stats->race_stats->tattooList[listIndex].projection_scale.x * pl_stats->race_stats->tattooList[listIndex].projection_scale.y >= 0 ) {
		tCharMarks.cur_flipped = false;
	}
	else {
		tCharMarks.cur_flipped = true;
	}


}

void C_RCharacterCreator::TattooGUILimitTattoo ( CRacialStats::tattoo_t& tattoo )
{
	//Color tattooHSL;
	//tattoo.color.GetHSLC( tattooHSL );
	switch ( tattoo.type )
	{
	case TATT_CLANMARK:
	case TATT_TATTOO:
		tattoo.color.red = tattoo.color.red<0.49f ? tattoo.color.red : 0.49f;
		tattoo.color.green = tattoo.color.green<0.49f ? tattoo.color.green : 0.49f;
		tattoo.color.blue = tattoo.color.blue<0.49f ? tattoo.color.blue : 0.49f;
		//tattooHSL.blue = tattooHSL.blue<0.5f ? tattooHSL.blue : 0.5f;
		break;
	case TATT_SCAR:
		//tattoo.color = Color( 0.5f,0.5f,0.5f,tattoo.color.alpha );
		tattoo.color.red = 0.5f;
		tattoo.color.green = 0.5f;
		tattoo.color.blue = 0.5f;
		//tattooHSL.green = 0;
		//tattooHSL.blue = 0.5f;
		break;
	case TATT_FLUXXGLOW:
		tattoo.color.red = tattoo.color.red>0.51f ? tattoo.color.red : 0.51f;
		tattoo.color.green = tattoo.color.green>0.51f ? tattoo.color.green : 0.51f;
		tattoo.color.blue = tattoo.color.blue>0.51f ? tattoo.color.blue : 0.51f;
		//tattooHSL.blue = tattooHSL.blue>0.5f ? tattooHSL.blue : 0.5f;
		break;
	};
	//tattoo.color.SetHSL( tattooHSL );
}

void C_RCharacterCreator::TattooGUIRandomizeTattoos ( void )
{
	// Clear old tattoo list
	pl_stats->race_stats->tattooList.clear();

	// Choose a random amount of tattoos for body and face.
	uint bodyTattooCount = Random.Next( 1, 8 );
	uint faceTattooCount = Random.Next( 0, 4 );

	// Choose a random amount of colors and select colors
	uint tattooColorCount = Random.Next( 1,3 );
	Color color0	= Color( Random.Range(0,0.5f), Random.Range(0,0.5f), Random.Range(0,0.5f), 1.0f );
	Color color1	= Color( Random.Range(0,0.5f), Random.Range(0,0.5f), Random.Range(0,0.5f), 1.0f );
	Color color2	= Color( Random.Range(0,0.5f), Random.Range(0,0.5f), Random.Range(0,0.5f), 1.0f );
	// And of course, mono looks better
	if ( bodyTattooCount+faceTattooCount < 4 ) {
		if ( tattooColorCount > 1 ) {
			if ( Random.Next()%2 == 0 ) {
				tattooColorCount -= 1;
			}
		}
	}
	if ( bodyTattooCount+faceTattooCount < 7 ) {
		if ( tattooColorCount > 1 ) {
			if ( Random.Next()%2 == 0 ) {
				tattooColorCount -= 1;
			}
		}
	}
	if ( tattooColorCount > 1 ) {
		if ( Random.Next()%2 == 0 ) {
			tattooColorCount -= 1;
		}
	}

	// Generate random tattoos
	for ( uint i = 0; i < bodyTattooCount; ++i )
	{
		CRacialStats::tattoo_t bodyTat;
		bodyTat.projection_pos = Vector3d(Random.PointOnUnitCircle()*1.6f,Random.Range( -2.6f, 2.5f ));
		bodyTat.projection_dir = -bodyTat.projection_pos;
		bodyTat.projection_dir.z *= 0.2f;
		bodyTat.projection_dir.normalize();
		bodyTat.projection_angle = Random.Range( -180.0f, 180.0f );
		bodyTat.projection_scale = Vector3d(1,1,1) * Random.Range( 0.4f, 1.5f );
		if ( Random.Next()%2 == 0 ) {
			bodyTat.projection_scale.x *= -1;
		}
		bodyTat.mirror = (Random.Next()%3 == 0) ? false : true;
		switch ( Random.Next()%3 ) {
			case 0:	bodyTat.color = color0; break;
			case 1:	bodyTat.color = color1; break;
			case 2:	bodyTat.color = color2; break;
		}
		switch ( Random.Next()%7 ) {
			case 0: bodyTat.type = TATT_SCAR; break;
			case 1:
			case 2: 
			case 3: 
			case 4:
			case 5:	bodyTat.type = TATT_CLANMARK; break;
			case 6: bodyTat.type = TATT_TATTOO;
					if ( Random.Next()%2 == 0 ) bodyTat.mirror = false;
					break;
		}
		if ( pl_stats->race_stats->iRace == CRACE_FLUXXOR ) {
			if ( bodyTat.type == TATT_CLANMARK ) {
				bodyTat.type = TATT_FLUXXGLOW;
				bodyTat.color = bodyTat.color + Color( 0.5f,0.5f,0.5f,0.0f );
			}
		}
		// Need to load pattern list and choose random pattern from list
		TattooGUIGenerateDropdown( bodyTat.type );
		bodyTat.pattern = tCharMarks.vTattooNames[Random.Next()%tCharMarks.vTattooNames.size()].c_str();
		// Limit tattoo
		TattooGUILimitTattoo( bodyTat );
		// Add tattoo to list
		pl_stats->race_stats->tattooList.push_back( bodyTat );
		// Load tattoo to gui
		TattooGUILoadTattoo( pl_stats->race_stats->tattooList.size()-1 );
	}

	// Generate face tattoos
	for ( uint i = 0; i < faceTattooCount; ++i )
	{
		CRacialStats::tattoo_t faceTat;
		//faceTat.projection_pos = Vector3d(Random.PointOnUnitCircle()*1.6f,Random.Range( -2.9f, 2.7f ));
		faceTat.projection_dir = Vector3d( 0,1,0 );
		faceTat.projection_pos = Vector3d( sqr(Random.Range(-0.7f,0.7f)),-1.25f,Random.Range(1.9f,2.9f) );
		faceTat.projection_angle = Random.Range( -180.0f, 180.0f );
		faceTat.projection_scale = Vector3d(1,1,1) * Random.Range( 0.2f, 1.1f ) * Random.Range( 0.3f,0.7f );
		if ( Random.Next()%2 == 0 ) {
			faceTat.projection_scale.x *= -1;
		}
		faceTat.mirror = (Random.Next()%3 == 0) ? false : true;
		switch ( Random.Next()%3 ) {
			case 0:	faceTat.color = color0; break;
			case 1:	faceTat.color = color1; break;
			case 2:	faceTat.color = color2; break;
		}
		switch ( Random.Next()%7 ) {
			case 0: faceTat.type = TATT_SCAR; break;
			case 1:
			case 2: 
			case 3: 
			case 4:
			case 5:	faceTat.type = TATT_CLANMARK; break;
			case 6: faceTat.type = TATT_TATTOO;
					if ( Random.Next()%2 == 0 ) faceTat.mirror = false;
					break;
		}
		if ( pl_stats->race_stats->iRace == CRACE_FLUXXOR ) {
			if ( faceTat.type == TATT_CLANMARK ) {
				faceTat.type = TATT_FLUXXGLOW;
				faceTat.color = faceTat.color + Color( 0.5f,0.5f,0.5f,0.0f );
				if ( Random.Next()%3 == 0 ) {
					faceTat.mirror = true;
				}
			}
		}
		// Need to load pattern list and choose random pattern from list
		TattooGUIGenerateDropdown( faceTat.type );
		faceTat.pattern = tCharMarks.vTattooNames[Random.Next()%tCharMarks.vTattooNames.size()].c_str();
		// Limit tattoo
		TattooGUILimitTattoo( faceTat );
		// Add tattoo to list
		pl_stats->race_stats->tattooList.push_back( faceTat );
		// Load tattoo to gui
		TattooGUILoadTattoo( pl_stats->race_stats->tattooList.size()-1 );
	}

	// Need to redraw tattoos now
	tCharMarks.need_redraw_tattoos = true;
}