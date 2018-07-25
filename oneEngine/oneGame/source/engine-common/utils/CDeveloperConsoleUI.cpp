#include "engine/utils/CDeveloperConsole.h"
#include "CDeveloperConsoleUI.h"
#include "core/input/CInput.h"
#include "core/settings/CGameSettings.h"
#include "renderer/texture/RrFontTexture.h"
//#include "renderer/material/RrMaterial.h"
#include "renderer/material/RrPass.h"
#include "renderer/material/RrShaderProgram.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"
#include "renderer/camera/CCamera.h"
#include "renderer/object/immediate/immediate.h"

CDeveloperConsoleUI*	ActiveConsoleUI = NULL;
CDeveloperCursor*		ActiveCursor = NULL;

CDeveloperConsoleUI::CDeveloperConsoleUI ( void )
	: CRenderableObject()
{
	ActiveConsoleUI = this;

	renderType = renderer::kRLV2D;

	//fntMenu	= RrFontTexture::Load( "monofonto.ttf", 16, kFW_Bold );
	//matfntMenu = new RrMaterial;
	//matfntMenu->m_diffuse = Color( 1.0F,1,1 );
	//matfntMenu->setTexture( TEX_MAIN, fntMenu );
	//matfntMenu->enablePassForward<0>(true);
	//matfntMenu->getPassForward<0>().set2DCommon();
	//matfntMenu->getPassForward<0>().m_program = RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/v2d/default_vv.spv", "shaders/v2d/default_p.spv"});

	//matMenu = new RrMaterial;
	//matMenu->m_diffuse = Color( 1.0F,1,1 );
	//matMenu->setTexture( TEX_MAIN, core::Orphan(RrTexture::Load("null")) );
	//matMenu->enablePassForward<0>(true);
	//matMenu->getPassForward<0>().set2DCommon();
	//matMenu->getPassForward<0>().m_program = RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/v2d/default_vv.spv", "shaders/v2d/default_p.spv"});

	//SetMaterial( matfntMenu );

	fntMenu	= RrFontTexture::Load( "monofonto.ttf", 16, kFW_Bold );
	RrPass fontPass;
	fontPass.m_type = kPassTypeForward;
	fontPass.m_surface.diffuseColor = Color( 1.0F, 1, 1 );
	fontPass.setTexture( TEX_MAIN, fntMenu );
	fontPass.utilSetupAs2D();
	fontPass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/v2d/default_vv.spv", "shaders/v2d/default_p.spv"}) );
	PassInitWithInput(1, &fontPass);

	RrPass shapesPass;
	shapesPass.m_type = kPassTypeForward;
	shapesPass.m_surface.diffuseColor = Color( 1.0F, 1, 1 );
	shapesPass.setTexture( TEX_MAIN, RrTexture::Load("null") );
	shapesPass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/v2d/default_vv.spv", "shaders/v2d/default_p.spv"}) );
	shapesPass.utilSetupAs2D();
	PassInitWithInput(0, &shapesPass);

	transform.world.position.z = -35;
}

CDeveloperConsoleUI::~CDeveloperConsoleUI ( void )
{
	if ( ActiveConsoleUI == this ){
		ActiveConsoleUI = NULL;
	}

	//delete fntMenu;
	fntMenu->RemoveReference();
	//matMenu->removeReference();
	//matfntMenu->removeReference();
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
	Vector2d screenSize ((Real)Screen::Info.width, (Real)Screen::Info.height);
	const Real kLineHeight = (Real)fntMenu->GetFontInfo().height + 3.0F;
	const Real kBottomMargin = 5.0F;

	if ( CCamera::activeCamera )
	{	// Modify console size based on render scale so it is always legible!
		screenSize *= CCamera::activeCamera->render_scale;
	}
	core::math::Cubic screenMapping = core::math::Cubic::FromPosition(
		Vector3f(0, 0, -45.0F),
		Vector3f(screenSize, +45.0F)
	);

	rrMeshBuilder2D builder(screenMapping, 6);
	rrTextBuilder2D builder_text(fntMenu, screenMapping, 100);

	if ( engine::Console->GetIsOpen() )
	{
		// Console rect:
		builder.addRect(
			Rect( 0.0F, screenSize.y - kLineHeight - kBottomMargin * 2.0F, screenSize.x, kLineHeight + kBottomMargin * 2.0F),
			Color(0.0F, 0.0F, 0.3F, 0.6F),
			false);
		if ( !engine::Console->GetMatchingCommands().empty() )
		{
			// Autocomplete rect:
			Real boxHeight = engine::Console->GetMatchingCommands().size() * kLineHeight + kBottomMargin * 2.0F;
			builder.addRect(
				Rect( 0.0F, screenSize.y - kLineHeight - boxHeight - kBottomMargin * 2.0F, screenSize.x, boxHeight),
				Color(0.0F, 0.0F, 0.3F, 0.6F),
				false);
		}
		
		// Draw the current command:
		builder_text.addText(
			Vector2d(2.0F, screenSize.y - kBottomMargin),
			Color( 0.0F, 0.5F, 1.0F, 1.0F ),
			(">" + engine::Console->GetCommandString() + "_").c_str() );

		// Draw the autocomplete results:
		for ( uint i = 0; i < engine::Console->GetMatchingCommands().size(); ++i )
		{	// Draw command list
			builder_text.addText(
				Vector2d(18.0F, screenSize.y - kLineHeight - kBottomMargin * 3.0F - kLineHeight * i),
				Color( 0.0F, 0.5F, 1.0F, 1.0F ),
				engine::Console->GetMatchingCommands()[i].c_str() );
		}
	}

	// Draw the developer string in the upper-left corner.
	builder_text.addText(
		Vector2d(4.0F, (Real)fntMenu->GetFontInfo().height + 2.0F),
		Color( 1.0F, 1.0F, 1.0F, 0.5F ),
		CGameSettings::Active()->sysprop_developerstring.c_str() );

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

	renderType = renderer::kRLV2D;

	//texCursor = RrTexture::Load( "textures/system/cursor.png" );
	//matCursor = new RrMaterial;
	//matCursor->m_diffuse = Color( 1.0F,1,1 );
	//matCursor->setTexture( TEX_MAIN, texCursor );
	//matCursor->enablePassForward<0>(true);
	//matCursor->getPassForward<0>().set2DCommon();
	//matCursor->getPassForward<0>().m_transparency_mode = renderer::ALPHAMODE_ALPHATEST;
	//matCursor->getPassForward<0>().m_program = RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/v2d/default_vv.spv", "shaders/v2d/default_p.spv"});
	//matCursor->removeReference();
	//SetMaterial( matCursor );

	texCursor = RrTexture::Load( "textures/system/cursor.png" );
	texCursor->AddReference();
	RrPass cursorPass;
	cursorPass.m_type = kPassTypeForward;
	cursorPass.m_surface.diffuseColor = Color( 1.0F, 1, 1 );
	cursorPass.setTexture( TEX_MAIN, texCursor );
	cursorPass.utilSetupAs2D();
	cursorPass.m_alphaMode = renderer::kAlphaModeAlphatest;
	cursorPass.m_program = RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/v2d/default_vv.spv", "shaders/v2d/default_p.spv"});
	PassInitWithInput(0, &cursorPass);

}
CDeveloperCursor::~CDeveloperCursor ( void )
{
	if ( ActiveCursor == this ) {
		ActiveCursor = NULL;
	}
	texCursor->RemoveReference();
}
bool CDeveloperCursor::PreRender ( void )
{
	matCursor->prepareShaderConstants();
	return true;
}
bool CDeveloperCursor::Render ( const char pass )
{
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