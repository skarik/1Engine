
#include "CCharacterScreen.h"
#include "core/system/Screen.h"
#include "renderer/texture/CBitmapFont.h"
#include "after/states/CharacterStats.h"
#include "after/states/player/CPlayerStats.h"
#include "after/states/CRacialStats.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"
#include "renderer/material/glMaterial.h"


CCharacterScreen::CCharacterScreen ( CAfterPlayer* p_player, CPlayerStats* p_playerstats )
	: CGameBehavior(), CRenderableObject(),
	pl( p_player ), pl_stats( p_playerstats ), race_stats( p_playerstats->race_stats )
{
	renderSettings.renderHints = RL_WORLD;
	renderType = Renderer::V2D;

	fntDraw		= new CBitmapFont ( "HVD_Comic_Serif_Pro.otf", 14, FW_BOLD );
	matDrawFnt	= new glMaterial;
	matDrawFnt->m_diffuse = Color( 0.0f,0,0 );
	matDrawFnt->passinfo.push_back( glPass() );
	matDrawFnt->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	matDrawFnt->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );
	matDrawFnt->setTexture( 0, fntDraw );

	matDrawSys	= new glMaterial;
	matDrawSys->m_diffuse = Color( 0.0f,0,0 );
	matDrawSys->passinfo.push_back( glPass() );
	matDrawSys->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	matDrawSys->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );
	matDrawSys->setTexture( 0, new CTexture("null") );

	SetMaterial( matDrawSys );
}

CCharacterScreen::~CCharacterScreen ( void )
{
	// Free up the font and materials used
	delete fntDraw;
	matDrawFnt->removeReference();
	delete matDrawFnt;
	//matDrawSys->removeReference();
	//delete matDrawSys;
}

void CCharacterScreen::SetVisibility ( bool visibility )
{
	visible = visibility;
}

// Update for hud values
void CCharacterScreen::Update ( void )
{
	
}
// Render for drawing
bool CCharacterScreen::Render ( const char pass )
{
	GL_ACCESS;
	GL.beginOrtho();
		DrawStatsPage();
	GL.endOrtho();

	return true;
}


// Render out the stats page
void CCharacterScreen::DrawStatsPage ( void )
{
	GLd_ACCESS;
	GLd.DrawSet2DScaleMode( GLd.SCALE_HEIGHT );
	GLd.DrawSet2DMode( GLd.D2D_FLAT );
	matDrawSys->m_diffuse = Color ( .35f, .35f, .35f, 0.8f );
	matDrawSys->bindPass(0);
	{
		// Begin with the page background
		GLd.DrawRectangleA( 0.2f, 0.15f, 0.4f, 0.7f );
	} // End rectangles

	// Draw the character info
	matDrawFnt->m_diffuse = Color( 1.0f, 1.0f, 1.0f );
	matDrawFnt->bindPass(0);
	{
		// Draw player name at the top
		GLd.DrawAutoText( 0.21f, 0.18f, "%s %s", race_stats->sPlayerName.c_str(), race_stats->sLastName.c_str() );

		// Draw the player race and sex
		// Generate stat list (TODO, MAKE THIS A FUNCTION)
		string s_racetype;
		{
			if ( race_stats->iGender == CGEND_FEMALE )
				s_racetype = "Female ";
			else
				s_racetype = "Male ";
			if ( race_stats->iRace == CRACE_HUMAN )		s_racetype += "Human";
			else if ( race_stats->iRace == CRACE_ELF )	s_racetype += "Dark Elf";
			else if ( race_stats->iRace == CRACE_DWARF )	s_racetype += "Dwarf";
			else if ( race_stats->iRace == CRACE_KITTEN )	s_racetype += "Catperson";
			else if ( race_stats->iRace == CRACE_FLUXXOR )	s_racetype += "Fluxxor";
			else if ( race_stats->iRace == CRACE_MERCHANT )	s_racetype += "Merchant";
		}
		GLd.DrawAutoText( 0.22f, 0.21f, "%s", s_racetype.c_str() );

		// Draw point stats
		GLd.DrawAutoText( 0.22f, 0.24f, "Point Stats" );

		GLd.DrawAutoText( 0.23f, 0.3f, "Strength" );
		GLd.DrawAutoText( 0.23f, 0.4f, "Agility" );
		GLd.DrawAutoText( 0.23f, 0.5f, "Intelligence" );

		GLd.DrawAutoText( 0.38f, 0.3f, "Health" );
		GLd.DrawAutoText( 0.38f, 0.4f, "Stamina" );
		GLd.DrawAutoText( 0.38f, 0.5f, "Energy" );

		GLd.DrawAutoText( 0.24f, 0.34f, "%d", pl_stats->stats->iStrength );
		GLd.DrawAutoText( 0.24f, 0.44f, "%d", pl_stats->stats->iAgility );
		GLd.DrawAutoText( 0.24f, 0.54f, "%d", pl_stats->stats->iIntelligence );
		GLd.DrawAutoText( 0.31f, 0.34f, "+%d", pl_stats->stats->iOffsetStrength );
		GLd.DrawAutoText( 0.31f, 0.44f, "+%d", pl_stats->stats->iOffsetAgility );
		GLd.DrawAutoText( 0.31f, 0.54f, "+%d", pl_stats->stats->iOffsetIntelligence );

		GLd.DrawAutoText( 0.39f, 0.34f, "%4.0lf", pl_stats->stats->fHealthMax );
		GLd.DrawAutoText( 0.39f, 0.44f, "%4.0lf", pl_stats->stats->fStaminaMax );
		GLd.DrawAutoText( 0.39f, 0.54f, "%4.0lf", pl_stats->stats->fManaMax );

	} // End fonts
}