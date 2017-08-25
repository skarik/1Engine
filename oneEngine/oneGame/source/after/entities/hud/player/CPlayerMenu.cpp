
#include "CPlayerMenu.h"

#include "core/input/CInput.h"
#include "core-ext/input/CInputControl.h"
#include "core/system/Screen.h"
#include "core/time/time.h"

#include "CPlayerInventoryGUI.h"
#include "CPlayerLogbook.h"
//#include "CPlayerQuestlog.h"
#include "after/entities/world/client/CQuestSystem.h"
#include "after/states/skilltree/CSkillTree.h"
#include "CCharacterScreen.h"
#include "after/entities/cutscene/CDialogueGUI.h"
#include "CInventoryGUI.h"

#include "engine-common/entities/CPlayer.h"
#include "engine-common/utils/CDeveloperConsoleUI.h"

#include "engine/utils/CDeveloperConsole.h"

#include "renderer/material/glMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

#include "after/scenes/gmsceneMenu.h"

CPlayerMenu::CPlayerMenu( CAfterPlayer*		p_player,
						  CPlayerInventoryGUI*	p_inventorygui,
						  CPlayerLogbook*	p_logbook,
						  CQuestSysRenderer*	p_questlog,
						  CSkillTreeGUI*	p_skilltreeGUI, 
						  CCharacterScreen*	p_charscreen,
						  CDialogueGUI*		p_dialogue_gui,
						  CInventoryGUI*	p_chest_gui)
	: CGameBehavior(), CRenderableObject(),
	pPlayer( p_player ), pInventoryGUI( p_inventorygui ), pLogbookGUI( p_logbook ), pQuestlogGUI( p_questlog ), pSkillGUI( p_skilltreeGUI ), pCharacterGUI( p_charscreen ), pDialogueGUI( p_dialogue_gui ), pChestGUI (p_chest_gui)
{
	InitGUInterface();

	renderSettings.renderHints = RL_WORLD;
	renderType = Renderer::V2D;

	//fntMenu	= new CBitmapFont ( "monofonto.ttf", 18, FW_BOLD );
	fntMenu	= new CBitmapFont ( "ComicNeue-Angular-Bold.ttf", 18 );
	matfntMenu = new glMaterial;
	matfntMenu->m_diffuse = Color( 1.0f,1,1 );
	matfntMenu->setTexture( 0, fntMenu );
	matfntMenu->passinfo.push_back( glPass() );
	matfntMenu->passinfo[0].m_lighting_mode	= Renderer::LI_NONE;
	matfntMenu->passinfo[0].m_transparency_mode	= Renderer::ALPHAMODE_TRANSLUCENT;
	matfntMenu->passinfo[0].shader = new glShader( "shaders/v2d/default.glsl" );
	SetMaterial( matfntMenu );

	matMenu = new glMaterial;
	matMenu->m_diffuse = Color( 0,0,0 );
	matMenu->setTexture( 0, new CTexture("null") );
	matMenu->passinfo.push_back( glPass() );
	matMenu->passinfo[0].m_lighting_mode	= Renderer::LI_NONE;
	matMenu->passinfo[0].m_transparency_mode= Renderer::ALPHAMODE_TRANSLUCENT;
	matMenu->passinfo[0].shader = new glShader( "shaders/v2d/default.glsl" );

	visible = false;

	for ( int i = 0; i < (int)(HS_NONE); ++i ) {	// Reset slide selection
		xMenuOffsets[i] = 0;
		bShowComponentState[i] = false;
	}

	CRenderableObject::transform.position.z = -34;
}

CPlayerMenu::~CPlayerMenu ( void )
{
	delete fntMenu;

	//matfntMenu->removeReference();
	//delete matfntMenu;

	matMenu->removeReference();
	delete matMenu;
}

// Update for hud values
void CPlayerMenu::Update ( void )
{
	UpdateGUInterface();
	UpdatePauseMenu();

	if ( pPlayer->IsAlive() ) {
		visible = bShowHudState || Engine::Console->GetIsOpen() || bShowPauseMenu;
		pPlayer->bHasInput = !bShowHudState && !Engine::Console->GetIsOpen() && !bShowPauseMenu;
	}
	else {
		bShowHudState = false;
		visible = Engine::Console->GetIsOpen() || bShowPauseMenu;
	}
	if ( ActiveCursor ) { 
		ActiveCursor->SetVisible( visible || pDialogueGUI->GetIsActive() || pInventoryGUI->GetIsVisible() );
	}
}

// Render for drawing
bool CPlayerMenu::Render ( const char pass )
{
	if ( pass != 0 )
		return false;
	
	GL_ACCESS;
	GLd_ACCESS;
	
	GL.beginOrtho();
	//GL.Translate( Vector3d( 0,0,34 ) );
	GLd.DrawSet2DScaleMode();
	//GL.Translate( Vector3d( 0,0,4 ) );

	if ( bShowHudState )
		DrawGUInterface();

	if ( bShowPauseMenu )
		DrawPauseMenu();

	GL.endOrtho();

	// Return success
	return true;
}

// === Main GUI Interface ===
// 
void CPlayerMenu::InitGUInterface ( void )
{
	iHudSelectState		= HS_INVENTORY;
	iHudSelectMouseover	= HS_NONE;
	bShowHudState		= false;
	bShowPauseMenu		= false;

	vMenuPos		= Vector2d( 0, 0.3f );
	vMenuButtonSize	= Vector2d( 0.1f, 0.07f ); 
}

void CPlayerMenu::UpdateGUInterface ( void )
{
	CInputControl* input = CInputControl::GetActive();//pPlayer->GetInputControl();
	//--if ( CInput::GetControl( this ) )
	if ( input->GetUser() == Engine::Console ) {
		return;
	}

	// Toggle pause menu
	if ( Engine::Console->GetIsOpen() )
		return;
	if ( Input::Keydown( Keys.Escape )||Input::Keydown( Keys.F11 ) )
	{
		bool bAnythingVisible = false;
		/*if ( bShowHudState ) {
			bShowHudState = false;
			bShowPauseMenu = false;
		}*/
		bAnythingVisible = HideAll();
		if ( !bAnythingVisible ) {
			bShowPauseMenu = !bShowPauseMenu;
		}
		else {
			bShowHudState = false;
		}
	}
	if ( bShowPauseMenu )
		return;

	// Use the controls to toggle hud visibility
	if ( input->axes.menuCharscreen.pressed() ) //if ( CInput::keydown['O'] )
	{
		if ( !bShowHudState )
		{
			bShowHudState = true;
			iHudSelectState = HS_CHARACTER;
		}
		else
		{
			if ( iHudSelectState == HS_CHARACTER )
				bShowHudState = false;
			else
				iHudSelectState = HS_CHARACTER;
		}
	}
	if ( input->axes.menuInventory.pressed() ) //if ( Input::Keydown('U') )	// Toggle inventory visibility
	{
		if ( !bShowHudState )
		{
			bShowHudState = true;
			iHudSelectState = HS_INVENTORY;
			bShowComponentState[HS_INVENTORY] = true;
		}
		else
		{
			/*if ( iHudSelectState == HS_INVENTORY ){
				bShowHudState = false;
				pInventoryGUI->EmptyCrafting();
				pInventoryGUI->DropSelected();
			}
			else
				iHudSelectState = HS_INVENTORY;*/
			iHudSelectState = HS_INVENTORY;
			bShowComponentState[HS_INVENTORY] = !bShowComponentState[HS_INVENTORY];
		}
	}
	if ( input->axes.menuLogbook.pressed() ) //if ( Input::Keydown('L') ) // Toggle logbook visibility
	{
		if ( !bShowHudState )
		{
			bShowHudState = true;
			iHudSelectState = HS_LOGBOOK;
			bShowComponentState[HS_LOGBOOK] = true;
		}
		else
		{
			/*if ( iHudSelectState == HS_LOGBOOK )
				bShowHudState = false;
			else
				iHudSelectState = HS_LOGBOOK;*/
			iHudSelectState = HS_LOGBOOK;
			bShowComponentState[HS_LOGBOOK] = !bShowComponentState[HS_LOGBOOK];
		}
	}
	if ( input->axes.menuCrafting.pressed() )
	{
		if ( !bShowHudState )
		{
			bShowHudState = true;
			iHudSelectState = HS_CRAFTING;
			bShowComponentState[HS_CRAFTING] = true;
		}
		else
		{
			iHudSelectState = HS_CRAFTING;
			bShowComponentState[HS_CRAFTING] = !bShowComponentState[HS_CRAFTING];
		}
	}
	if ( input->axes.menuSkills.pressed() ) //if ( Input::Keydown('K') ) // Toggle Skill Tree visibility
	{
		if ( !bShowHudState )
		{
			bShowHudState = true;
			iHudSelectState = HS_SKILLTREE;
		}
		else
		{
			if ( iHudSelectState == HS_SKILLTREE )
				bShowHudState = false;
			else
				iHudSelectState = HS_SKILLTREE;
		}
	}
	if ( input->axes.menuQuestlog.pressed() )
	{
		if ( !bShowHudState )
		{
			bShowHudState = true;
			iHudSelectState = HS_QUESTLOG;
		}
		else
		{
			if ( iHudSelectState == HS_QUESTLOG )
				bShowHudState = false;
			else
				iHudSelectState = HS_QUESTLOG;
		}
	}
	
	if ( input->axes.menuToggle.pressed() ) {//if (CInput::keydown[VK_TAB])
		bShowHudState = !bShowHudState;
		if ( bShowHudState == false ) {
			HideAll();
		}
	}

	if (pChestGUI->GetIsVisible())
	{
		bShowHudState = true;
		iHudSelectState = HS_CHEST;
	}

	// Update mouse controls
	if ( bShowHudState )
	{
		ftype mouseX = (ftype)CInput::MouseX() / Screen::Info.height;
		ftype mouseY = (ftype)CInput::MouseY() / Screen::Info.height;
		{
			int targetIndex = (int)(floor( (( mouseY - vMenuPos.y )/vMenuButtonSize.y) ));
		
			if (( targetIndex >= 0 )&&( targetIndex < 7 )&&( mouseX < vMenuButtonSize.y ))
			{
				iHudSelectMouseover = (eHUDSelection)targetIndex;
			}
			else
			{
				iHudSelectMouseover = HS_NONE;
			}
		}
		if ( iHudSelectMouseover != HS_NONE )
		{
			if ( CInput::MouseDown(CInput::MBLeft) ) // When mouse is clicked, change selection
			{
				iHudSelectState = iHudSelectMouseover;
				// TODO: Play sound.
				if ( iHudSelectState == HS_CRAFTING ) {
					//pInventoryGUI->SetCraftingVisibility( true );
					bShowComponentState[HS_CRAFTING] = !bShowComponentState[HS_CRAFTING];
				}
				else if ( iHudSelectState == HS_EQUIPMENT ) {
					//pInventoryGUI->SetEquipmentVisibility( true );
					bShowComponentState[HS_EQUIPMENT] = !bShowComponentState[HS_EQUIPMENT];
				}
				else if ( iHudSelectState == HS_INVENTORY ) {
					bShowComponentState[HS_INVENTORY] = !bShowComponentState[HS_INVENTORY];
				}
				else if ( iHudSelectState == HS_LOGBOOK ) {
					bShowComponentState[HS_LOGBOOK] = !bShowComponentState[HS_LOGBOOK];
				}
				else if ( iHudSelectState == HS_QUESTLOG ) {
					bShowComponentState[HS_QUESTLOG] = !bShowComponentState[HS_QUESTLOG];
				}
			}
		}
		for ( int i = 0; i < 7; ++i )	// Slide selection out
		{
			if ( i == (int)iHudSelectMouseover )
			{
				xMenuOffsets[i] += Time::deltaTime * 0.3f;
				xMenuOffsets[i] = std::min<ftype>( xMenuOffsets[i], vMenuButtonSize.x - vMenuButtonSize.y );
			}
			else
			{
				xMenuOffsets[i] -= Time::deltaTime * 0.3f;
				xMenuOffsets[i] = std::max<ftype>( xMenuOffsets[i], 0 );
			}
		}
	}

	// Control the visibility of all the HUDs
	pCharacterGUI->SetVisibility( bShowHudState && ( iHudSelectState == HS_CHARACTER ) );
//	pInventoryGUI->SetVisibility( bShowHudState && ( iHudSelectState == HS_INVENTORY || iHudSelectState == HS_CRAFTING || iHudSelectState == HS_EQUIPMENT ) );
//	pLogbookGUI->SetVisibility( bShowHudState && ( iHudSelectState == HS_LOGBOOK ) );
	pInventoryGUI->SetVisibility( (bShowHudState && ( iHudSelectState == HS_INVENTORY || iHudSelectState == HS_CRAFTING || iHudSelectState == HS_EQUIPMENT || iHudSelectState == HS_LOGBOOK ) ) ||  iHudSelectState == HS_CHEST);
	//pLogbookGUI->SetVisibility( bShowHudState && ( iHudSelectState == HS_INVENTORY || iHudSelectState == HS_CRAFTING || iHudSelectState == HS_EQUIPMENT || iHudSelectState == HS_LOGBOOK ) );
	pSkillGUI->SetVisibility( bShowHudState && ( iHudSelectState == HS_SKILLTREE ) );
	bool yes = bShowComponentState[HS_INVENTORY] || iHudSelectState == HS_CHEST;
	pInventoryGUI->SetInventoryVisibility( yes );//bShowComponentState[HS_INVENTORY] );//||  iHudSelectState == HS_CHEST);
	pInventoryGUI->SetCraftingVisibility( bShowComponentState[HS_CRAFTING] );
	pInventoryGUI->SetEquipmentVisibility( bShowComponentState[HS_EQUIPMENT] );
	pLogbookGUI->SetVisibility( bShowComponentState[HS_LOGBOOK] );
	pQuestlogGUI->SetVisible( bShowHudState && bShowComponentState[HS_QUESTLOG] );

	if ( bShowHudState && (iHudSelectState == HS_CHARACTER) ) {
		pPlayer->SetCameraMode( NPC::CAMERA_OVERVIEW );
	}
	else if ( bShowHudState ) {
		pPlayer->SetCameraMode( NPC::CAMERA_MONTAGE );
	}
	else {
		pPlayer->SetCameraMode();
	}
}

bool CPlayerMenu::HideAll ( void )
{
	bool swapped = bShowHudState;
	for ( int i = 0; i < (int)(HS_NONE); ++i ) {	// Reset slide selection
		if ( bShowComponentState[i] ) {
			swapped = true;
			bShowComponentState[i] = false;
		}
	}
	return swapped;
}

void CPlayerMenu::DrawGUInterface ( void )
{
	GLd_ACCESS;
	// Draw menu on the side
	GLd.DrawSet2DScaleMode( GLd.SCALE_HEIGHT );
	GLd.DrawSet2DMode( GLd.D2D_FLAT );

	for ( int i = 0; i < (int)(HS_NONE); ++i )
	{
		matMenu->m_diffuse = Color( 0.1f,0.1f,0.2f,0.3f+xMenuOffsets[i]*9 );
		matMenu->bindPass(0);
		GLd.DrawRectangleA( vMenuPos.x, vMenuPos.y + i*vMenuButtonSize.y, xMenuOffsets[i]+vMenuButtonSize.y, vMenuButtonSize.y );

		matfntMenu->setTexture( 0, fntMenu );
		matfntMenu->m_diffuse = Color( 1.0f,1.0f,1.0f,1.0f );
		matfntMenu->bindPass(0);
		Vector2d drawPos ( vMenuPos.x + 0.01f, vMenuPos.y + i*vMenuButtonSize.y + vMenuButtonSize.y*0.8f - xMenuOffsets[i] );
		if ( i == HS_CHARACTER )
			GLd.DrawAutoText( drawPos.x, drawPos.y, "Character" );
		else if ( i == HS_INVENTORY )
			GLd.DrawAutoText( drawPos.x, drawPos.y, "Inventory" );
		else if ( i == HS_QUESTLOG )
			GLd.DrawAutoText( drawPos.x, drawPos.y, "Questlog" );
		else if ( i == HS_LOGBOOK )
			GLd.DrawAutoText( drawPos.x, drawPos.y, "Logbook" );
		else if ( i == HS_SKILLTREE )
			GLd.DrawAutoText( drawPos.x, drawPos.y, "Skilltree" );
		else if ( i == HS_DEBUGSPAWNER )
			GLd.DrawAutoText( drawPos.x, drawPos.y, "Debug Spawner" );
		else if ( i == HS_EQUIPMENT )
			GLd.DrawAutoText( drawPos.x, drawPos.y, "Equipment" );
		else if ( i == HS_CRAFTING )
			GLd.DrawAutoText( drawPos.x, drawPos.y, "Crafting" );

	}
}

void CPlayerMenu::UpdatePauseMenu ( void )
{
	if ( Engine::Console->GetIsOpen() )
		return;
	
	if ( bShowPauseMenu )
	{
		ftype mouseX = (ftype)CInput::MouseX() / Screen::Info.height;
		ftype mouseY = (ftype)CInput::MouseY() / Screen::Info.height;

		if ( mouseX > 0.1f && mouseX < 0.22f && mouseY > 0.1f && mouseY < 0.17f )
		{
			iPazSelectMouseover = PZ_QUIT;
		}
		else
		{
			iPazSelectMouseover = PZ_NONE;
		}

		if ( CInput::MouseDown(CInput::MBLeft) )
		{
			switch ( iPazSelectMouseover )
			{
			case PZ_QUIT:
				// And now we go to the game place
				CGameScene* pNewScene;
				pNewScene = CGameScene::NewScene<gmsceneMenu> ();
				CGameScene::SceneGoto( pNewScene );
				break;
			}
		}
	}
}

void CPlayerMenu::DrawPauseMenu ( void )
{
	GLd_ACCESS
	// Draw menu on the side
	GLd.DrawSet2DScaleMode( GLd.SCALE_HEIGHT );
	GLd.DrawSet2DMode( GLd.D2D_FLAT );

	matMenu->m_diffuse = Color( 0.1f,0.1f,0.2f,0.4f );
	matMenu->bindPass(0);
	GLd.DrawRectangleA( 0.0f,0.0f,2.5f,1.0f );

	GLd.DrawRectangleA( 0.1f,0.1f,0.12f,0.07f );

	matfntMenu->m_diffuse = Color( 1.0f,1.0f,1.0f,(iPazSelectMouseover==PZ_QUIT) ? 1.0f : 0.6f );
	matfntMenu->setTexture( 0, fntMenu );
	matfntMenu->bindPass(0);
		GLd.DrawAutoText( 0.12f,0.13f, "Quit" );
}

void CPlayerMenu::TurnOffInventory (void)
{
	bShowHudState = false;
	iHudSelectState = HS_INVENTORY;
	bShowComponentState[HS_INVENTORY] = false;
}