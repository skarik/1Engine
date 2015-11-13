// class CPlayerHudStatus
//  Draws the health, stamina, and mana bars on screen.
//  Draws debuffs, hurt direction indicators, and controls damaged view effects.

// ==Includes==
// Main class
#include "CPlayerHudStatus.h"
// Player class definition
#include "engine-common/entities/CPlayer.h"
#include "after/entities/character/CAfterPlayer.h"
// Other classes needed
#include "core/time/time.h"
#include "core/math/Math.h"

// Include screen properties
#include "core/system/Screen.h"

#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

#include "engine/state/CGameState.h"

// ==Constructor==
CPlayerHudStatus::CPlayerHudStatus ( CAfterPlayer* p_player )
	: CGameBehavior(), CRenderableObject()
{
	pPlayer		= p_player;

	killTarget = NULL;
	killTimer = 0;

	Initialize();

	for ( uint i = 0; i < 3; ++i )
	{
		factorTimer[i] = 2.5f;
		factorAlpha[i] = 1.0f;
	}
}

// ==Init of Stuff==
// Initialize all materials
void CPlayerHudStatus::Initialize ( void )
{
	renderSettings.renderHints = RL_WORLD;
	renderType = Renderer::V2D;

	//fntPnts	= new CBitmapFont ( "Calibri", 13, FW_BOLD );
	//fntPnts	= new CBitmapFont ( "YanoneKaffeesatz-R.otf", 14, FW_BOLD );
	fntPnts	= new CBitmapFont ( "YanoneKaffeesatz-B.otf", 14, FW_HEAVY );
	matFontPnts = new glMaterial;
	matFontPnts->m_diffuse = Color( 1.0f,1,1 );
	matFontPnts->setTexture( 0, fntPnts );
	matFontPnts->passinfo.push_back( glPass() );
	matFontPnts->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	matFontPnts->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );
	SetMaterial( matFontPnts );

	matPointBars = new glMaterial;
	matPointBars->m_diffuse = Color( 1.0f,1,1 );
	matPointBars->setTexture( 0, new CTexture(".res/textures/hud/bar_gradient.png") );
	matPointBars->passinfo.push_back( glPass() );
	matPointBars->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	matPointBars->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );

	matPointBarsBG = new glMaterial;
	matPointBarsBG->m_diffuse = Color( 1.0f,1,1 );
	matPointBarsBG->setTexture( 0, new CTexture(".res/textures/hud/bar_bg.png" ) );
	matPointBarsBG->passinfo.push_back( glPass() );
	matPointBarsBG->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	matPointBarsBG->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );

	matPointBarsEnds = new glMaterial;
	matPointBarsEnds->m_diffuse = Color( 1.0f,1,1 );
	matPointBarsEnds->setTexture( 0, new CTexture(".res/textures/hud/barendcap-flat.png" ) );
	matPointBarsEnds->passinfo.push_back( glPass() );
	matPointBarsEnds->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	matPointBarsEnds->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );

	matBuffCircle = new glMaterial;
	matBuffCircle->m_diffuse = Color( 1,1,1 );
	matBuffCircle->setTexture( 0, new CTexture(".res/textures/hud/skillblob_circlet.png") );
	matBuffCircle->passinfo.push_back( glPass() );
	matBuffCircle->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	matBuffCircle->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );
}

// ==Destructor==
CPlayerHudStatus::~CPlayerHudStatus ( void )
{
	//matFontPnts->removeReference();
	//delete matFontPnts;
	delete fntPnts;

	matPointBars->removeReference();
	delete matPointBars;
	matPointBarsBG->removeReference();
	delete matPointBarsBG;
	matPointBarsEnds->removeReference();
	delete matPointBarsEnds;
	matBuffCircle->removeReference();
	delete matBuffCircle;
}

// == Inputs from External Sources ==
void CPlayerHudStatus::UpdateKillTarget ( CActor* new_target )
{
	killTarget = NULL;
	if ( new_target ) {
		if ( CGameState::Active()->ObjectExists(new_target) ) {
			killTarget = new_target;
			killTimer = 24.0f;
		}
	}
}

// == State Updates ==
// Update for hud values
void CPlayerHudStatus::Update ( void )
{
	killTimer -= Time::deltaTime;

	if ( ( pPlayer->GetCharStats()->fHealth < pPlayer->GetCharStats()->fHealthMax*0.3f )
		|| fabs(pPlayer->GetCharStats()->fHealthDelta) > 0.1f )
	{
		factorTimer[0] = 2.0f;
	}
	if ( ( pPlayer->GetCharStats()->fMana < pPlayer->GetCharStats()->fManaMax*0.5f )
		|| fabs(pPlayer->GetCharStats()->fManaDelta) > FTYPE_PRECISION )
	{
		factorTimer[2] = 2.0f;
	}
	if ( ( pPlayer->GetCharStats()->fStamina < pPlayer->GetCharStats()->fStaminaMax*0.5f )
		|| fabs(pPlayer->GetCharStats()->fStaminaDelta) > FTYPE_PRECISION )
	{
		factorTimer[1] = 2.0f;
	}

	for ( uint i = 0; i < 3; ++i )
	{
		factorTimer[i] -= Time::deltaTime;
		if ( factorTimer[i] > 0 )
		{
			factorAlpha[i] += Time::deltaTime * 3;
		}
		else
		{
			factorAlpha[i] -= Time::deltaTime;
		}
		factorAlpha[i] = Math.Clamp( factorAlpha[i], 0,1 );
	}
}

// Render for drawing
bool CPlayerHudStatus::Render ( const char pass )
{
	GL_ACCESS;
	GLd_ACCESS;
	// Set options
	GL.beginOrtho();
	GLd.DrawSet2DScaleMode();
	// Begin draw
	{
		CharacterStats & stats = *(pPlayer->GetCharStats());

		// First start with the health bars
		{

			// Create offsets
			Vector2d bar_size, bar_offset;
			bar_size.x = std::min( Screen::Info.width*0.19f, 210.0f );
			bar_size.y = std::min( Screen::Info.height*0.08f, 18.0f );
			bar_offset.x = std::min( Screen::Info.width*0.06f, 48.0f );
			bar_offset.y = std::min( Screen::Info.height*0.08f, 48.0f );

			GLd.DrawSet2DMode( GLd.D2D_FLAT );
			
			Vector2d bar_position;

			// First draw the health
			bar_position = Vector2d( bar_offset.x, Screen::Info.height - bar_offset.y - bar_size.y );
			matPointBarsBG->m_diffuse = Color( 0.2f, 0.2f, 0.2f, 0.8f*factorAlpha[0] ); // Draw bar background
			matPointBarsBG->bindPass(0);
			GLd.DrawRectangle(
				bar_position.x-1, bar_position.y-1,
				bar_size.x+2, bar_size.y+2 );
			Real bleedDamage, bleedThreshold, bleedTime;
			if ( ((CAfterPlayer*)pPlayer)->GetBleeding(bleedDamage,bleedThreshold,bleedTime) ) // Draw bleed damage
			{
				matPointBars->m_diffuse = Color( 0.5f, 0.05f, 0.05f, 1.0f*factorAlpha[0] );
				matPointBars->bindPass(0);
				GLd.DrawRectangle(
					bar_position.x, bar_position.y,
					bar_size.x*(1-(bleedDamage/bleedThreshold)),
					bar_size.y );
			}
			matPointBars->m_diffuse = Color( 0.8f*1.3f, 0.1f*1.3f, 0.14f*1.3f, 1.0f*factorAlpha[0] );
			matPointBars->bindPass(0);
			GLd.DrawRectangle(
				bar_position.x, bar_position.y,
				bar_size.x*(stats.fHealth/stats.fHealthMax),
				bar_size.y );
			matPointBarsEnds->m_diffuse = Color( 1.0f,1.0f, 1.0f, 1.0f*factorAlpha[0] );
			matPointBarsEnds->bindPass(0);
			GLd.DrawRectangle(
				bar_position.x-8, bar_position.y-2,
				8,bar_size.y+4 );
			GLd.DrawRectangle(
				bar_position.x+bar_size.x-1, bar_position.y-2,
				8,bar_size.y+4 );
			// Draw point value
			matFontPnts->m_diffuse = Color( 1.0f, 1.0f, 1.0f, factorAlpha[0] );
			matFontPnts->bindPass(0);
				GLd.DrawAutoText( (bar_position.x+4)/Screen::Info.width,(bar_position.y+bar_size.y-4)/Screen::Info.height, "%.0f", stats.fHealth );

			// Now draw the stamina
			//bar_position = Vector2d( bar_offset.x + bar_size.y, Screen::Info.height - bar_offset.y + 4 );
			bar_position = Vector2d( Screen::Info.width - bar_size.x - bar_offset.x, Screen::Info.height - bar_offset.y + 4 - bar_size.y/2 );
			matPointBarsBG->m_diffuse = Color( 0.2f, 0.2f, 0.2f, 0.8f*factorAlpha[1] );
			matPointBarsBG->bindPass(0);
			GLd.DrawRectangle(
				bar_position.x-1, bar_position.y-1,
				bar_size.x+2, bar_size.y+2 );
			matPointBars->m_diffuse = Color( 0.12f*1.4f, 0.6f*1.4f, 0.08f*1.4f, 1.0f*factorAlpha[1] );
			matPointBars->bindPass(0);
			GLd.DrawRectangle(
				bar_position.x, bar_position.y,
				bar_size.x*(stats.fStamina/stats.fStaminaMax),
				bar_size.y );
			matPointBarsEnds->m_diffuse = Color( 1.0f,1.0f, 1.0f, 1.0f*factorAlpha[1] );
			matPointBarsEnds->bindPass(0);
			GLd.DrawRectangle(
				bar_position.x -8, bar_position.y -2,
				8,bar_size.y+4 );
			GLd.DrawRectangle(
				bar_position.x+bar_size.x-1, bar_position.y -2,
				8,bar_size.y+4 );
			// Draw point value
			matFontPnts->m_diffuse = Color( 1.0f, 1.0f, 1.0f, factorAlpha[1] );
			matFontPnts->bindPass(0);
				GLd.DrawAutoText( (bar_position.x+4)/Screen::Info.width,(bar_position.y+bar_size.y-4)/Screen::Info.height, "%.0f", stats.fStamina );

			// Now draw the mana
			bar_position = Vector2d( Screen::Info.width - bar_size.x - bar_offset.x, Screen::Info.height - bar_offset.y - bar_size.y - bar_size.y/2 );
			matPointBarsBG->m_diffuse = Color( 0.2f, 0.2f, 0.2f, 0.8f*factorAlpha[2] );
			matPointBarsBG->bindPass(0);
			GLd.DrawRectangle(
				bar_position.x - 1, bar_position.y-1,
				bar_size.x+2, bar_size.y+2 );
			matPointBars->m_diffuse = Color( 0.19f*1.3f, 0.10f*1.3f, 0.64f*1.3f, 1.0f*factorAlpha[2] );
			matPointBars->bindPass(0);
			GLd.DrawRectangle(
				bar_position.x, bar_position.y,
				bar_size.x*(stats.fMana/stats.fManaMax),
				bar_size.y );
			matPointBarsEnds->m_diffuse = Color( 1.0f,1.0f, 1.0f, 1.0f*factorAlpha[2] );
			matPointBarsEnds->bindPass(0);
			GLd.DrawRectangle(
				bar_position.x -8, bar_position.y-2,
				8,bar_size.y+4 );
			GLd.DrawRectangle(
				bar_position.x +bar_size.x-1, bar_position.y -2,
				8,bar_size.y+4 );
			// Draw point value
			matFontPnts->m_diffuse = Color( 1.0f, 1.0f, 1.0f, factorAlpha[2] );
			matFontPnts->bindPass(0);
				GLd.DrawAutoText( (bar_position.x+4)/Screen::Info.width,(bar_position.y+bar_size.y-4)/Screen::Info.height, "%.0f", stats.fMana );

			if ( (killTimer > 0.0f) && killTarget && CGameState::Active()->ObjectExists(killTarget) ) {
				CharacterStats* killstats = killTarget->GetCharStats();
				if ( killstats )
				{
					// First draw the health
					matPointBarsBG->m_diffuse = Color( 0.2f, 0.2f, 0.2f, 0.8f );
					matPointBarsBG->bindPass(0);
					GLd.DrawRectangle(
						Screen::Info.width/2  - bar_size.x/2-1,
						Screen::Info.height/16 + bar_offset.y - bar_size.y-1,
						bar_size.x+2, bar_size.y+2 );
					matPointBars->m_diffuse = Color( 0.8f*1.3f, 0.1f*1.3f, 0.14f*1.3f, 1.0f );
					matPointBars->bindPass(0);
					GLd.DrawRectangle(
						Screen::Info.width/2 -bar_size.x/2,
						Screen::Info.height/16 + bar_offset.y - bar_size.y,
						bar_size.x*(killstats->fHealth/killstats->fHealthMax),
						bar_size.y );
					matPointBarsEnds->m_diffuse = Color( 1.0f,1.0f, 1.0f, 1.0f );
					matPointBarsEnds->bindPass(0);
					GLd.DrawRectangle(
						Screen::Info.width/2  -bar_size.x/2-8,
						Screen::Info.height/16 + bar_offset.y - bar_size.y-2,
						8,bar_size.y+4 );
					GLd.DrawRectangle(
						Screen::Info.width/2  -bar_size.x/2+bar_size.x-1,
						Screen::Info.height/16 + bar_offset.y - bar_size.y-2,
						8,bar_size.y+4 );
				}
			}
			else {
				killTarget = NULL;
			}
		}

		// Draw experience bar
		{
			Vector2d bar_size, bar_offset;
			bar_size.x = Screen::Info.width*0.29f;
			bar_size.y = Screen::Info.height*0.012f;
			bar_offset.x = Screen::Info.width*0.06f;
			bar_offset.y = Screen::Info.height*0.016f;

			matPointBarsBG->m_diffuse = Color( 0.2f, 0.2f, 0.2f, 0.8f );
			matPointBarsBG->bindPass(0);
			GLd.DrawRectangle(
				Screen::Info.width/2 - bar_size.x/2,
				Screen::Info.height - bar_offset.y,
				bar_size.x, bar_size.y );
			matPointBars->m_diffuse = Color( 0.2f, 0.2f, 0.9f, 1.0f );
			matPointBars->bindPass(0);
			GLd.DrawRectangle(
				Screen::Info.width/2 - bar_size.x/2,
				Screen::Info.height - bar_offset.y,
				bar_size.x * stats.fExperience/stats.fExperienceMax, bar_size.y ); //(stats.fExperience/100.0f)

		}

		// Draw level
		{
			matFontPnts->m_diffuse = Color( 0.4f, 0.4f, 1.0f, 1.0f );
			matFontPnts->bindPass(0);
				GLd.DrawAutoTextCentered( 0.5f,1.0f-0.006f, "%d", stats.iLevel );
		}

		// Draw buffs/debuffs
		{
			uint buffCount = 0;
			for ( auto buff = pPlayer->GetDebuffs().List()->begin(); buff != pPlayer->GetDebuffs().List()->end(); ++buff )
			{
				if ( (*buff)->positive == 0 ) {
					matBuffCircle->m_diffuse = Color( 1.1,0.3,0.3 );
				}
				else if ( (*buff)->positive == 1 ) {
					matBuffCircle->m_diffuse = Color( 0.5,0.6,1.1 );
				}
				else if ( (*buff)->positive == 2 ) {
					matBuffCircle->m_diffuse = Color( 0.9,0.9,0.9 );
				}
				matBuffCircle->bindPass(0);
				//GLd.DrawCircle( Screen::Info.width/2 - 200, Screen::Info.height - 
				GLd.DrawSet2DScaleMode( GLd.SCALE_HEIGHT );
				GLd.DrawRectangleA( 0.1f + buffCount*0.035f, 0.86f, 0.032f,0.032f );

				buffCount += 1;
			}
		}
	}
	// End ortho
	GL.endOrtho();
	// Return success
	return true;
}