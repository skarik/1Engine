
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
	matfntMenu->m_diffuse = Color( 1.0f,1,1 );
	matfntMenu->setTexture( TEX_MAIN, fntMenu );
	matfntMenu->passinfo.push_back( RrPassForward() );
	matfntMenu->passinfo[0].m_lighting_mode = renderer::LI_NONE;
	matfntMenu->passinfo[0].m_transparency_mode = renderer::ALPHAMODE_TRANSLUCENT;
	matfntMenu->passinfo[0].shader = new RrShader( "shaders/v2d/default.glsl" );

	matMenu = new RrMaterial;
	matMenu->m_diffuse = Color( 0.0f,0,0 );
	matMenu->setTexture( TEX_MAIN, new CTexture("null") );
	matMenu->passinfo.push_back( RrPassForward() );
	matMenu->passinfo[0].m_lighting_mode = renderer::LI_NONE;
	matMenu->passinfo[0].m_transparency_mode = renderer::ALPHAMODE_TRANSLUCENT;
	matMenu->passinfo[0].shader = new RrShader( "shaders/v2d/default.glsl" );

	SetMaterial( matfntMenu );

	transform.world.position.z = -35;
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

bool CDeveloperConsoleUI::PreRender ( void )
{
	//GL.Translate( Vector3d( 0,0,-35 ) );
	matMenu->prepareShaderConstants();
	matfntMenu->prepareShaderConstants();
	return true;
}
bool CDeveloperConsoleUI::Render ( const char pass )
{
	GL_ACCESS GLd_ACCESS
	//GL.beginOrtho();
	core::math::Cubic::FromPosition( Vector3d(0, 0, -45.0F), Vector3d((Real)Screen::Info.width, (Real)Screen::Info.height, +45.0F) );

	//GL.Translate( Vector3d( 0,0,-35 ) );
	GLd.DrawSet2DScaleMode();
	GLd.DrawSet2DMode( GLd.D2D_FLAT );

	if ( engine::Console->GetIsOpen() )
	{
		matMenu->m_diffuse = Color( 0.0f,0.0f,0.0f,0.6f );
		matMenu->bindPass(0);
		GLd.SetMaterial(matMenu);
		GLd.DrawRectangleA( 0.0f, 0.95f, 3.0f,0.05f );
		if ( !engine::Console->GetMatchingCommands().empty() ) {
			GLd.DrawRectangleA( 0.02f, 0.93f - engine::Console->GetMatchingCommands().size()*0.03f, 0.4f, engine::Console->GetMatchingCommands().size()*0.03f + 0.02f );
		}
		
		matfntMenu->m_diffuse = Color( 0.0f,0.5f,1.0f,1.0f );
		matfntMenu->bindPass(0);
		GLd.SetMaterial(matfntMenu);
		GLd.DrawAutoText( 0.03f, 0.98f, " >%s", engine::Console->GetCommandString().c_str() );	// Draw command
		for ( uint i = 0; i < engine::Console->GetMatchingCommands().size(); ++i ) {		// Draw command list
			GLd.DrawAutoText( 0.05f, 0.96f+(i-(Real)(engine::Console->GetMatchingCommands().size()))*0.03f, "%s", engine::Console->GetMatchingCommands()[i].c_str() );
		}
	}

	matfntMenu->m_diffuse = Color( 1.0f,1.0f,1.0f,0.5f );
	matfntMenu->bindPass(0);
	GLd.SetMaterial(matfntMenu);
	GLd.DrawAutoText( 0.005f, 0.023f, CGameSettings::Active()->sysprop_developerstring.c_str() );

	//GL.endOrtho();

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
	matCursor->passinfo[0].m_lighting_mode = renderer::LI_NONE;
	matCursor->passinfo[0].m_transparency_mode = renderer::ALPHAMODE_ALPHATEST;
	matCursor->passinfo[0].m_face_mode = renderer::FM_FRONTANDBACK;
	matCursor->passinfo[0].b_depthmask = false;
	matCursor->passinfo[0].b_depthtest = false;
	matCursor->passinfo[0].shader = new RrShader( "shaders/v2d/default.glsl" );
	//matCursor->passinfo[0].shader = new RrShader( "shaders/sys/copy_buffer.glsl" );
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
bool CDeveloperCursor::PreRender ( void )
{
	//GL.Translate( Vector3d( 0,0,-44 ) );
	matCursor->prepareShaderConstants();
	return true;
}
bool CDeveloperCursor::Render ( const char pass )
{
	GL_ACCESS GLd_ACCESS

	if ( pass != 0 )
		return false;

	////GL.pushModelMatrix( Matrix4x4() );
	////GL.beginOrtho();
	//core::math::Cubic::FromPosition( Vector3d(0, 0, -45.0F), Vector3d((Real)Screen::Info.width, (Real)Screen::Info.height, +45.0F) );
	//GLd.DrawSet2DScaleMode();
	//GLd.DrawSet2DMode( GLd.D2D_FLAT );
	//	matCursor->bindPass(0);
	//	GLd.SetMaterial(matCursor);
	//	//matCursor->setShaderConstants( this );
	//	GLd.P_PushColor( 1,1,1,1 );
	//	//	GLd.DrawScreenQuad();
	//	GLd.DrawRectangle( (Real)Input::MouseX(), (Real)Input::MouseY(), 32,32 );
	////GL.endOrtho();
	////GL.popModelMatrix();

	rrMeshBuilder2D builder(4);
	builder.addRect(
		Rect( (Real)Input::MouseX(), (Real)Input::MouseY(), 32,32 ),
		Color(1.0F, 1.0F, 1.0F, 1.0F),
		false);

	RrScopedMeshRenderer renderer;
	renderer.render(this, m_material, pass, builder);

	// Return success
	return true;
}