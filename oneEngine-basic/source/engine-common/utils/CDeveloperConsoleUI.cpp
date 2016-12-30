
#include "engine/utils/CDeveloperConsole.h"
#include "CDeveloperConsoleUI.h"
#include "core/input/CInput.h"
#include "core/settings/CGameSettings.h"
#include "renderer/texture/CBitmapFont.h"
#include "renderer/material/glMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

CDeveloperConsoleUI*	ActiveConsoleUI = NULL;
CDeveloperCursor*		ActiveCursor = NULL;

CDeveloperConsoleUI::CDeveloperConsoleUI ( void )
	: CRenderableObject()
{
	ActiveConsoleUI = this;

	renderType = Renderer::V2D;

	fntMenu	= new CBitmapFont ( "monofonto.ttf", 16, FW_BOLD );
	matfntMenu = new glMaterial;
	matfntMenu->m_diffuse = Color( 1.0f,1,1 );
	matfntMenu->setTexture( 0, fntMenu );
	matfntMenu->passinfo.push_back( glPass() );
	matfntMenu->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	matfntMenu->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	matfntMenu->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );

	matMenu = new glMaterial;
	matMenu->m_diffuse = Color( 0.0f,0,0 );
	matMenu->setTexture( 0, new CTexture("null") );
	matMenu->passinfo.push_back( glPass() );
	matMenu->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	matMenu->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	matMenu->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );

	SetMaterial( matfntMenu );

	transform.position.z = -35;
}

CDeveloperConsoleUI::~CDeveloperConsoleUI ( void )
{
	if ( ActiveConsoleUI == this ) {
		ActiveConsoleUI = NULL;
	}

	delete fntMenu;
	//matfntMenu->removeReference();
	matMenu->removeReference();
}

bool CDeveloperConsoleUI::Render ( const char pass )
{
	GL_ACCESS GLd_ACCESS
	GL.beginOrtho();
	//GL.Translate( Vector3d( 0,0,-35 ) );
	GLd.DrawSet2DScaleMode();
	GLd.DrawSet2DMode( GLd.D2D_FLAT );

	if ( Engine::Console->GetIsOpen() )
	{
		matMenu->m_diffuse = Color( 0.0f,0.0f,0.0f,0.6f );
		matMenu->bindPass(0);
		GLd.DrawRectangleA( 0.0f, 0.95f, 3.0f,0.05f );
		if ( !Engine::Console->GetMatchingCommands().empty() ) {
			GLd.DrawRectangleA( 0.02f, 0.93f - Engine::Console->GetMatchingCommands().size()*0.03f, 0.4f, Engine::Console->GetMatchingCommands().size()*0.03f + 0.02f );
		}
		
		matfntMenu->m_diffuse = Color( 0.0f,0.5f,1.0f,1.0f );
		matfntMenu->bindPass(0);
		GLd.DrawAutoText( 0.03f, 0.98f, " >%s", Engine::Console->GetCommandString().c_str() );	// Draw command
		for ( uint i = 0; i < Engine::Console->GetMatchingCommands().size(); ++i ) {		// Draw command list
			GLd.DrawAutoText( 0.05f, 0.96f+(i-(ftype)(Engine::Console->GetMatchingCommands().size()))*0.03f, "%s", Engine::Console->GetMatchingCommands()[i].c_str() );
		}
	}

	matfntMenu->m_diffuse = Color( 1.0f,1.0f,1.0f,0.5f );
	matfntMenu->bindPass(0);
	GLd.DrawAutoText( 0.005f, 0.023f, CGameSettings::Active()->sysprop_developerstring.c_str() );

	GL.endOrtho();

	// Return success
	return true;
}

CDeveloperCursor::CDeveloperCursor ( void )
	: CRenderableObject()
{
	ActiveCursor = this;

	transform.position.z = -44;

	renderType = Renderer::V2D;

	texCursor = new CTexture( "textures/system/cursor.png" );
	matCursor = new glMaterial;
	matCursor->m_diffuse = Color( 1.0f,1,1 );
	matCursor->setTexture( 0, texCursor );
	matCursor->passinfo.push_back( glPass() );
	matCursor->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	matCursor->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_ALPHATEST;
	matCursor->passinfo[0].shader = new glShader( "shaders/v2d/default.glsl" );
	//matCursor->passinfo[0].shader = new glShader( ".res/shaders/sys/copy_buffer.glsl" );
	matCursor->removeReference();
	SetMaterial( matCursor );
}
CDeveloperCursor::~CDeveloperCursor ( void )
{
	if ( ActiveCursor == this ) {
		ActiveCursor = NULL;
	}
	//delete matCursor;
	delete texCursor;
}
bool CDeveloperCursor::Render ( const char pass )
{
	GL_ACCESS GLd_ACCESS

	if ( pass != 0 )
		return false;

	GL.pushModelMatrix( Matrix4x4() );

	GL.beginOrtho();
	GL.Translate( Vector3d( 0,0,-44 ) );
	GLd.DrawSet2DScaleMode();
	GLd.DrawSet2DMode( GLd.D2D_FLAT );

	matCursor->bindPass(0);
	matCursor->setShaderConstants( this );
	GLd.P_PushColor( 1,1,1,1 );
	//	GLd.DrawScreenQuad();
	GLd.DrawRectangle( (Real)Input::MouseX(), (Real)Input::MouseY(), 32,32 );

	GL.endOrtho();

	GL.popModelMatrix();

	// Return success
	return true;
}