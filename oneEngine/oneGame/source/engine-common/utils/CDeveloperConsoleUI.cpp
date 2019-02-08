#include "engine/utils/CDeveloperConsole.h"
#include "CDeveloperConsoleUI.h"
#include "core/input/CInput.h"
#include "core/settings/CGameSettings.h"
#include "renderer/texture/RrFontTexture.h"
#include "renderer/material/RrPass.h"
#include "renderer/material/RrShaderProgram.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"
#include "renderer/camera/RrCamera.h"
#include "renderer/object/immediate/immediate.h"

CDeveloperConsoleUI*	ActiveConsoleUI = NULL;
CDeveloperCursor*		ActiveCursor = NULL;

CDeveloperConsoleUI::CDeveloperConsoleUI ( void )
	: CRenderableObject()
{
	ActiveConsoleUI = this;

	fntMenu	= RrFontTexture::Load( "monofonto.ttf", 16, kFW_Bold );
	RrPass fontPass;
	fontPass.m_layer = renderer::kRenderLayerV2D;
	fontPass.m_type = kPassTypeForward;
	fontPass.m_surface.diffuseColor = Color( 1.0F, 1, 1 );
	fontPass.setTexture( TEX_MAIN, fntMenu );
	fontPass.utilSetupAs2D();
	fontPass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/v2d/default_vv.spv", "shaders/v2d/default_p.spv"}) );
	PassInitWithInput(1, &fontPass);

	RrPass shapesPass;
	shapesPass.m_layer = renderer::kRenderLayerV2D;
	shapesPass.m_type = kPassTypeForward;
	shapesPass.m_surface.diffuseColor = Color( 1.0F, 1, 1 );
	shapesPass.setTexture( TEX_MAIN, RrTexture::Load(renderer::kTextureWhite) );
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

	fntMenu->RemoveReference();
}

bool CDeveloperConsoleUI::PreRender ( rrCameraPass* cameraPass )
{
	PushCbufferPerObject(XrTransform(), cameraPass);
	return true;
}
bool CDeveloperConsoleUI::Render ( const rrRenderParams* params )
{
	Vector2f screenSize ((Real)Screen::Info.width, (Real)Screen::Info.height);
	const Real kLineHeight = (Real)fntMenu->GetFontInfo()->height + 3.0F;
	const Real kBottomMargin = 5.0F;

	if ( RrCamera::activeCamera )
	{	// Modify console size based on render scale so it is always legible!
		screenSize *= RrCamera::activeCamera->renderScale;
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
			Vector2f(2.0F, screenSize.y - kBottomMargin),
			Color( 0.0F, 0.5F, 1.0F, 1.0F ),
			(">" + engine::Console->GetCommandString() + "_").c_str() );

		// Draw the autocomplete results:
		for ( uint i = 0; i < engine::Console->GetMatchingCommands().size(); ++i )
		{	// Draw command list
			builder_text.addText(
				Vector2f(18.0F, screenSize.y - kLineHeight - kBottomMargin * 3.0F - kLineHeight * i),
				Color( 0.0F, 0.5F, 1.0F, 1.0F ),
				engine::Console->GetMatchingCommands()[i].c_str() );
		}
	}

	// Draw the developer string in the upper-left corner.
	builder_text.addText(
		Vector2f(4.0F, (Real)fntMenu->GetFontInfo()->height + 2.0F),
		Color( 1.0F, 1.0F, 1.0F, 0.5F ),
		CGameSettings::Active()->sysprop_developerstring.c_str() );

	// TODO: build mesh in the pre-render (or similar place)
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
	
	texCursor = RrTexture::Load( "textures/system/cursor.png" );
	texCursor->AddReference();
	RrPass cursorPass;
	cursorPass.m_layer = renderer::kRenderLayerV2D;
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
bool CDeveloperCursor::PreRender ( rrCameraPass* cameraPass )
{
	PushCbufferPerObject(XrTransform(), cameraPass);
	return true;
}
bool CDeveloperCursor::Render ( const rrRenderParams* params )
{
	rrMeshBuilder2D builder(6);
	builder.addRect(
		Rect( (Real)Input::MouseX(), (Real)Input::MouseY(), 32,32 ),
		Color(1.0F, 1.0F, 1.0F, 1.0F),
		false);

	// TODO: build mesh in the pre-render (or similar place)
	RrScopedMeshRenderer renderer;
	renderer.render(this, m_material, pass, builder);

	// Return success
	return true;
}