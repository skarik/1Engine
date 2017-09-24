
#include "engine/utils/CDeveloperConsole.h"
#include "CDeveloperConsoleUI.h"
#include "core/input/CInput.h"
#include "core/settings/CGameSettings.h"
#include "renderer/texture/CBitmapFont.h"
#include "renderer/material/RrMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"
#include "renderer/object/immediate/immediate.h"

CDeveloperConsoleUI*	ActiveConsoleUI = NULL;
CDeveloperCursor*		ActiveCursor = NULL;

CDeveloperConsoleUI::CDeveloperConsoleUI ( void )
	: CRenderableObject()
{
	ActiveConsoleUI = this;

	renderType = renderer::V2D;

	fntMenu	= new CBitmapFont ( "monofonto.ttf", 16, FW_BOLD );
	matfntMenu = new RrMaterial;
	matfntMenu->m_diffuse = Color( 1.0F,1,1 );
	matfntMenu->setTexture( TEX_MAIN, fntMenu );
	matfntMenu->passinfo.push_back( RrPassForward() );
	matfntMenu->passinfo[0].set2DCommon();
	matfntMenu->passinfo[0].shader = new RrShader( "shaders/v2d/default.glsl" );

	matMenu = new RrMaterial;
	matMenu->m_diffuse = Color( 1.0F,1,1 );
	matMenu->setTexture( TEX_MAIN, new CTexture("null") );
	matMenu->passinfo.push_back( RrPassForward() );
	matMenu->passinfo[0].set2DCommon();
	matMenu->passinfo[0].shader = new RrShader( "shaders/v2d/default.glsl" );

	SetMaterial( matfntMenu );

	transform.world.position.z = -35;
}

CDeveloperConsoleUI::~CDeveloperConsoleUI ( void )
{
	if ( ActiveConsoleUI == this ){
		ActiveConsoleUI = NULL;
	}

	delete fntMenu;
	matMenu->removeReference();
	matfntMenu->removeReference();
	// matfntMenu is the main material, and deallocated automagically.
}

bool CDeveloperConsoleUI::PreRender ( void )
{
	matMenu->prepareShaderConstants();
	matfntMenu->prepareShaderConstants();
	return true;
}
bool CDeveloperConsoleUI::Render ( const char pass )
{
	//GL_ACCESS GLd_ACCESS
	//GL.beginOrtho();
	//core::math::Cubic::FromPosition( Vector3d(0, 0, -45.0F), Vector3d((Real)Screen::Info.width, (Real)Screen::Info.height, +45.0F) );
	Vector2d screenSize ((Real)Screen::Info.width, (Real)Screen::Info.height);

	rrMeshBuilder2D builder(6);
	rrTextBuilder2D builder_text(fntMenu, 100);

	//GL.Translate( Vector3d( 0,0,-35 ) );
	//GLd.DrawSet2DScaleMode();
	//GLd.DrawSet2DMode( GLd.D2D_FLAT );

	if ( engine::Console->GetIsOpen() )
	{
		//matMenu->m_diffuse = Color( 0.0f,0.0f,0.0f,0.6f );
		//matMenu->bindPass(0);
		//GLd.SetMaterial(matMenu);

		builder.addRect(
			Rect( 0.0F, screenSize.y - 18.0F, screenSize.x, 18.0F),
			Color(0.0F, 0.0F, 0.1F, 0.6F),
			false);
		//GLd.DrawRectangleA( 0.0f, 0.95f, 3.0f,0.05f );
		if ( !engine::Console->GetMatchingCommands().empty() ) {
			//GLd.DrawRectangleA( 0.02f, 0.93f - engine::Console->GetMatchingCommands().size()*0.03f, 0.4f, engine::Console->GetMatchingCommands().size()*0.03f + 0.02f );
			Real boxHeight = engine::Console->GetMatchingCommands().size() * 17.0F + 8.0F;
			builder.addRect(
				Rect( 0.0F, screenSize.y - 18.0F - boxHeight, screenSize.x, boxHeight),
				Color(0.0F, 0.0F, 0.1F, 0.6F),
				false);
		}
		
		//matfntMenu->m_diffuse = Color( 0.0f,0.5f,1.0f,1.0f );
		//matfntMenu->bindPass(0);
		//GLd.SetMaterial(matfntMenu);
		//GLd.DrawAutoText( 0.03f, 0.98f, " >%s", engine::Console->GetCommandString().c_str() );	// Draw command
		builder_text.addText(
			Vector2d(2.0F, screenSize.y - 18.0F),
			Color( 0.0F, 0.5F, 1.0F, 1.0F ),
			(" >%s" + engine::Console->GetCommandString()).c_str() );
		for ( uint i = 0; i < engine::Console->GetMatchingCommands().size(); ++i )
		{	// Draw command list
		//	GLd.DrawAutoText( 0.05f, 0.96f+(i-(Real)(engine::Console->GetMatchingCommands().size()))*0.03f, "%s", engine::Console->GetMatchingCommands()[i].c_str() );
			builder_text.addText(
				Vector2d(2.0F, screenSize.y - 18.0F - 17.0F * (i+1)),
				Color( 0.0F, 0.5F, 1.0F, 1.0F ),
				engine::Console->GetMatchingCommands()[i].c_str() );
		}
	}

	/*matfntMenu->m_diffuse = Color( 1.0f,1.0f,1.0f,0.5f );
	matfntMenu->bindPass(0);
	GLd.SetMaterial(matfntMenu);
	GLd.DrawAutoText( 0.005f, 0.023f, CGameSettings::Active()->sysprop_developerstring.c_str() );*/

	//GL.endOrtho();

	RrScopedMeshRenderer renderer;
	renderer.render(this, matMenu, pass, builder);
	renderer.render(this, matfntMenu, pass, builder_text);

	// Return success
	return true;
}

CDeveloperCursor::CDeveloperCursor ( void )
	: CRenderableObject()
{
	ActiveCursor = this;

	transform.world.position.z = -44;

	renderType = renderer::V2D;

	texCursor = new CTexture( "textures/system/cursor.png" );
	matCursor = new RrMaterial;
	matCursor->m_diffuse = Color( 1.0F,1,1 );
	matCursor->setTexture( TEX_MAIN, texCursor );
	matCursor->passinfo.push_back( RrPassForward() );
	matCursor->passinfo[0].set2DCommon();
	matCursor->passinfo[0].m_transparency_mode = renderer::ALPHAMODE_ALPHATEST;
	matCursor->passinfo[0].shader = new RrShader( "shaders/v2d/default.glsl" );
	matCursor->removeReference();
	SetMaterial( matCursor );
}
CDeveloperCursor::~CDeveloperCursor ( void )
{
	if ( ActiveCursor == this ) {
		ActiveCursor = NULL;
	}
	delete texCursor;
}
bool CDeveloperCursor::PreRender ( void )
{
	matCursor->prepareShaderConstants();
	return true;
}
bool CDeveloperCursor::Render ( const char pass )
{
	GL_ACCESS GLd_ACCESS

	if ( pass != 0 )
		return false;

	rrMeshBuilder2D builder(6);
	builder.addRect(
		Rect( (Real)Input::MouseX(), (Real)Input::MouseY(), 32,32 ),
		Color(1.0F, 1.0F, 1.0F, 1.0F),
		false);

	RrScopedMeshRenderer renderer;
	renderer.render(this, m_material, pass, builder);

	// Return success
	return true;
}