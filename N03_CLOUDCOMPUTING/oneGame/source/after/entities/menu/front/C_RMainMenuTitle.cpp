
#include "C_RMainMenuTitle.h"
#include "renderer/texture/CBitmapFont.h"
#include "core/math/random/Random.h"
#include "renderer/material/glMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"
#include "core/time/Time.h"

C_RMainMenuTitle::C_RMainMenuTitle ( void )
	: CRenderableObject ()
{
	GenerateTagline();

	renderType = Renderer::V2D;

	fntTitle	= new CBitmapFont ( "HVD_Comic_Serif_Pro.otf", 52, FW_BOLD );
	fntTagline	= new CBitmapFont ( "benegraphic.ttf", 40, FW_BOLD );

	matText = new glMaterial;
	matText->m_diffuse = Color( 1.0f,1,1 );
	matText->passinfo.push_back( glPass() );
	matText->passinfo[0].m_lighting_mode	= Renderer::LI_NONE;
	matText->setTexture( 0, fntTitle );
	//matText->setShader( new glShader( "shaders/v2d/outline.glsl" ) );
	matText->passinfo[0].shader = new glShader( "shaders/v2d/default.glsl" );

	SetMaterial( matText );
}

C_RMainMenuTitle::~C_RMainMenuTitle ( void )
{
	delete fntTitle;
	delete fntTagline;
	//matText->removeReference();
	//delete matText;
}


bool C_RMainMenuTitle::Render ( const char pass )
{
	GL_ACCESS;
	GLd_ACCESS;
	GL.beginOrtho();

	GLd.DrawSet2DScaleMode();

	matText->m_diffuse = Color( 0.0f, 0.0f, 0.0f, 1.0f );
	matText->setTexture( 0, fntTitle );
	matText->bindPass(0);
		ftype aof = Time::currentTime;
		for ( uint i = 0; i < 4; ++i ) {
			GLd.DrawAutoTextCentered( 0.5f + (ftype)sin(i*PI/2+aof)*0.007f,0.2f + (ftype)cos(i*PI/2+aof)*0.007f,"Project AFTER");
		}

	matText->m_diffuse = Color( 1.0f, 1.0f, 1.0f, 1.0f );
	matText->bindPass(0);
		GLd.DrawAutoTextCentered( 0.5f,0.2f,"Project AFTER");
		
	matText->m_diffuse = Color( 0.0f, 0.0f, 0.0f, 1.0f );
	matText->setTexture( 0, fntTagline );
	matText->bindPass(0);
		for ( uint i = 0; i < 8; ++i ) {
			GLd.DrawAutoTextCentered( 0.494f + (ftype)sin(i*PI/4+0.2f)*0.002f,0.3f + (ftype)cos(i*PI/4+0.2f)*0.002f,sTagline );
		}

	matText->m_diffuse = Color( 1.0f, 1.0f, 1.0f, 1.0f );
	matText->bindPass(0);
		GLd.DrawAutoTextCentered( 0.494f,0.3f, sTagline );

	GL.endOrtho();

	return true;
}


void C_RMainMenuTitle::GenerateTagline ( void )
{
	const int maxChoices = 22;
	const int impossible = maxChoices + 1;
	switch ( Random.Next()%maxChoices )
	{
	case 0: sTagline = "distilled high fantasy with methamphetamines";
		break;
	case 1:	sTagline = "adventure fighting terrain exploration rpg";
		break;
	case 2: sTagline = "50%% more dimensions than Terraria!";
		break;
	case 3: sTagline = "this text is not centered";
		break;
	case 4: sTagline = "komodoes will eat your face";
		break;
	case 5: sTagline = "the skilltrees are a lie";
		break;
	case 6: sTagline = "AXE ATTACKS!";
		break;
	//case 7: sTagline = "87.23%% less suck than Minecraft!";
	case 7: sTagline = "currently with 125 variations of swords";
		break;
	case 8: //sTagline = "Konoka x Setsuna FOR LIFE!!!"; // nobody knows what that is
			sTagline = "Ask Chiefmasamune for details.";
		break;
	case 9: //sTagline = "Makoto BEST GIRL"; // nobody knows what this is
			sTagline = "Idol (un)Approved!";
		break;
	case 10: sTagline = "Next update, this could be gone!";
		break;
	case 11: sTagline = "Now with more gameplay!";
		break;
	case 12: sTagline = "MORE MOE";
		break;
	case 13: sTagline = "help me i'm send this from tvtropes and i'm stuck help";
		break;
	case 14: sTagline = "Now with more chapters!";
		break;
	case 15: sTagline = "How did I get here?";
		break;
	case 16: sTagline = "With exactly 100 warnings in the Inventory class";
		break;
	case 17: sTagline = "Tell your wife!";
		break;
	case 18: sTagline = "Tell your kids!";
		break;
	case 19: sTagline = "The greatest in region-locking!";
		break;
	case 20: sTagline = "Tell your three stepsisters!";
		break;
	case 21: sTagline = "FMF100 still on Reddit";
		break;
	case impossible: sTagline = "BREAKING THROUGH THE IMPOSSIBLE!";
		break;
	default: sTagline = "xxxNoScopeTreePunch!hashtag!Swag2013-Yoloxxx!!";
		break;
	}
}