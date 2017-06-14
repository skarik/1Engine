
#include "Plhud_RadialMenu.h"
#include "after/entities/character/CAfterPlayer.h"
#include "core-ext/input/CInputControl.h"

#include "core/system/Screen.h"
#include "core/math/Math.h"
#include "engine/physics/raycast/Raycaster.h"

#include "renderer/material/glMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

CBitmapFont*	Plhud_RadialMenu::fntRadial = NULL;
glMaterial*		Plhud_RadialMenu::matRadialDraw = NULL;

Plhud_RadialMenu::Plhud_RadialMenu ( CAfterPlayer* nTargetPlayer )
		: CGameBehavior(), CRenderableObject(), targetPlayer(nTargetPlayer)
{
	// Initialize values
	hasKillSignal = false;
	fCurrentSize = 0;
	currentSelection = -1;
	selectionPosition = Vector2d(0,0);

	// Set up renderer
	renderSettings.renderHints = RL_WORLD;
	renderType = Renderer::V2D;
	if ( fntRadial == NULL ) {
		fntRadial	= new CBitmapFont ( "HVD_Comic_Serif_Pro.otf", 20, FW_BOLD );
	}
	if ( matRadialDraw == NULL ) {
		matRadialDraw = new glMaterial;
		matRadialDraw->m_diffuse = Color( 1.0f,1,1 );
		matRadialDraw->passinfo.push_back( glPass() );
		matRadialDraw->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
		matRadialDraw->setTexture( 0, new CTexture( "textures/white.jpg" ) );
		matRadialDraw->passinfo[0].shader = new glShader( "shaders/v2d/default.glsl" );
	}
	this->SetMaterial( matRadialDraw );

	// Generate command list
	commandList.push_back( rcl_MOVE_HERE );
	commandList.push_back( rcl_FOLLOW_ME );
	commandList.push_back( rcl_LOOK_HERE );
	commandList.push_back( rcl_ATTACK_THIS );
	commandList.push_back( rcl_NIX_THAT );
	commandList.push_back( rcl_BUFF_ME );
	commandList.push_back( rcl_STAND_GROUND );

	// Get spacing of commands
	spaceDiv =  360.0f / commandList.size();
	if ( spaceDiv < 45 ) {
		spaceDiv = 22.5f;
	}
	else if ( spaceDiv < 90 ) {
		spaceDiv = 45;
	}
	else {
		spaceDiv = 90;
	}
}

Plhud_RadialMenu::~Plhud_RadialMenu ( void )
{

}

//
void Plhud_RadialMenu::Update ( void )
{
	if ( !hasKillSignal ) {
		// Only disable input if still in active mode
		targetPlayer->bCanMouseMove = false;

		// Increase size
		fCurrentSize = std::min<ftype>( fCurrentSize + Time::deltaTime*5.0f, 1.0f );

		// Make a selection based on mouse angle
		CInputControl* input = CInputControl::GetActive();
		selectionPosition += Vector2d( input->vMouseInput.x, input->vMouseInput.y )*0.3f;
		if ( selectionPosition.magnitude() > 5 ) {
			selectionPosition = selectionPosition.normal() * 5;
		}

		// Calculate the selection
		if ( selectionPosition.magnitude() > 2 ) {
			ftype selectionAngle = (Real) radtodeg(atan2(selectionPosition.x,-selectionPosition.y));
			if ( selectionAngle < 0 ) {
				selectionAngle += 360;
			}
			currentSelection = (int)(0.5f + selectionAngle/spaceDiv);
			if ( currentSelection >= (int)(360/spaceDiv) ) {
				currentSelection = 0;
			}
		}
		else {
			currentSelection = -1;
		}
	}
	else {
		// Shrink and then die
		fCurrentSize -= Time::deltaTime*5.0f;
		if ( fCurrentSize <= 0 ) {
			fCurrentSize = 0;
			DeleteObject( this );
		}
	}
}
// Render routine
bool Plhud_RadialMenu::Render ( const char pass )
{
	if ( pass != 0 )
		return false;

	ftype sizePercent = Math.Smoothlerp( fCurrentSize, 0,1 );
	GL_ACCESS;
	GLd_ACCESS;

	GL.beginOrtho();
	GLd.DrawSet2DScaleMode();

	//matRadialDraw->bindPass(0);
	/*fntRadial->Set();
		ftype xoffset = mLerpTimer;
		if ( mLerpTimer < -1.0f ) {
			xoffset = -1.0f - Math.Smoothlerp( -1.0f-mLerpTimer, 0.0f, 18.0f );
		}
		else if ( mLerpTimer > 1.0f ) {
			xoffset = 1.0f + Math.Smoothlerp( mLerpTimer-1, 0.0f, 18.0f );
		}
		xoffset *= 0.5f/15.0f;
		// Draw the text
		glColor4f( 0.0f, 0.0f, 0.0f, 1.0f );
			GL.DrawAutoTextCentered( 0.49f+xoffset, 0.16f, mString.c_str() );
			GL.DrawAutoTextCentered( 0.52f-xoffset, 0.17f, mString.c_str() );

		glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
			GL.DrawAutoTextCentered( 0.48f+xoffset, 0.15f, mString.c_str() );

	fntRadial->Unbind();*/
		//glColor4f( 1.0f, 1.0f, 1.0f, 1.0f*sizePercent );
	matRadialDraw->m_diffuse = Color( 1.0f, 1.0f, 1.0f, 1.0f*sizePercent );
	matRadialDraw->bindPass(0);
		GLd.DrawCircleA( 0.5f,0.5f,0.2f*sizePercent );

		// Draw current selection movement
		GLd.DrawLineA( 0.5f,0.5f,0.5f+selectionPosition.x*0.04f,(Real)(0.5f+selectionPosition.y*0.04f*Screen::Info.aspect) );

		char* text[] = {
			"Move Here",
			"Follow Me",
			"Look Here",
			"Attack This",
			"Nix That",
			"Buff Me",
			"Stand Ground"
		};

		// Draw the commands as boxes.
		for ( uint i = 0; i < commandList.size(); ++i ) {
			// Draw selection box
			if ( currentSelection == i ) {
				//glColor4f( 1.0f, 1.0f, 1.0f, 1.0f*sizePercent );
				matRadialDraw->m_diffuse = Color( 1.0f, 1.0f, 1.0f, 1.0f*sizePercent );
				matRadialDraw->bindPass(0);
				GLd.DrawRectangleA(
					0.5f + sinf((Real)degtorad(spaceDiv*i))*0.2f*sizePercent/(Real)Screen::Info.aspect - 0.035f/(Real)Screen::Info.aspect,
					0.5f - cosf((Real)degtorad(spaceDiv*i))*0.2f*sizePercent - 0.035f,
					0.07f/(Real)Screen::Info.aspect,0.07f );
			}
			// Draw command box
			//glColor4f( 1.0f, 0.1f*i, 1.0f-0.1f*i, 1.0f*sizePercent );
			matRadialDraw->m_diffuse = Color( 1.0f, 0.1f*i, 1.0f-0.1f*i, 1.0f*sizePercent );
			matRadialDraw->bindPass(0);
			GLd.DrawRectangleA(
				0.5f + sinf((Real)degtorad(spaceDiv*i))*0.2f*sizePercent/(Real)Screen::Info.aspect - 0.03f/(Real)Screen::Info.aspect,
				0.5f - cosf((Real)degtorad(spaceDiv*i))*0.2f*sizePercent - 0.03f,
				0.06f/(Real)Screen::Info.aspect,0.06f );
		}

		// Draw the text of the selected one
		if ( currentSelection != -1 && (currentSelection < (int)commandList.size()) )
		{
			//glColor4f( 1.0f, 1.0f, 1.0f, 1.0f*sizePercent );
			matRadialDraw->m_diffuse = Color( 1.0f, 1.0f, 1.0f, 1.0f*sizePercent );
			matRadialDraw->bindPass(0);
			fntRadial->Set();
				GLd.DrawAutoTextCentered( 0.5f,0.5f,text[commandList[currentSelection]] );
			fntRadial->Unbind();
		}
	//matRadialDraw->unbind();

	GL.endOrtho();

	// Return success
	return true;
}

#include "after/entities/character/npc/CNpcBase.h"

// Notice for 'Kill'
// The command that is being hovered over is to be executed.
void Plhud_RadialMenu::NotifyKill ( void )
{
	// Do commands
	if ( !hasKillSignal ) {
		if ( currentSelection != -1 ) {
			switch ( commandList[currentSelection] )
			{
			case rcl_FOLLOW_ME:
				{
				// tell AI to follow
					// Loop through the player's companions
					std::vector<uint64_t> npcIds;
					NPC::Manager->GetCharactersInParty( npcIds, 1024 );
					for ( uint i = 0; i < npcIds.size(); ++i ) {
						NPC::CNpcBase* npc = (NPC::CNpcBase*) NPC::Manager->GetNPC( npcIds[i] );
						npc->GetAI()->PartyCmdFollowMe( targetPlayer );
					}
				} break;
			case rcl_MOVE_HERE:
				{
				// raycast to get position
					Ray eyeRay = targetPlayer->GetEyeRay();
					ftype max_go_distance = 1000;
					RaycastHit hitInfo;
					if ( Raycaster.Raycast( eyeRay, max_go_distance, &hitInfo, Physics::GetCollisionFilter(Layers::PHYS_BULLET_TRACE) ) )
					{
						// get owner's eye ray and cast
						Vector3d targetPos = hitInfo.hitPos;

						// Loop through the player's companions
						std::vector<uint64_t> npcIds;
						NPC::Manager->GetCharactersInParty( npcIds, 1024 );
						for ( uint i = 0; i < npcIds.size(); ++i ) {
							NPC::CNpcBase* npc = (NPC::CNpcBase*) NPC::Manager->GetNPC( npcIds[i] );
							npc->GetAI()->PartyCmdMoveTo( targetPos );
						}
					}
					else {
						// Say no, can't, or don't understand
					}
				} break;
			default:
				std::cout << "UNKNOWN/UNHANDLED COMPANION COMMAND" << std::endl;
				break;
			}
		}
	}

	// Switch
	targetPlayer->bCanMouseMove = true; // Reenable input
	hasKillSignal = true; // Set to kill mode
}