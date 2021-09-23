#include "EditorWindowBase.h"

#include "core/debug/Console.h"

#include "renderer/state/RrRenderer.h"
#include "renderer/windowing/RrWindow.h"
#include "renderer/camera/RrCamera.h"

#include "m04/eventide/UserInterface.h"

m04::editor::WindowBase::WindowBase ( const Vector2i& position, const Vector2i& size, const char* outputName )
	: CGameBehavior()
{
	// Set up window and output for this
	window = new RrWindow(RrRenderer::Active, NIL);
	window->Resize(size.x, size.y);

	editor_world = new RrWorld();
	RrRenderer::Active->AddWorld(editor_world);

	RrOutputInfo output (editor_world, window);
	output.name = outputName;
	{
		editor_camera = new RrCamera(false);
		// Override certain camera aspects to get the right projection
		editor_camera->transform.rotation = Rotator( 0.0, -90, -90 );
		editor_camera->transform.position.z = 800;
		editor_camera->zNear = 1;
		editor_camera->zFar = 4000;
	}
	output.camera = editor_camera;
	output.update_interval_when_not_focused = 10;

	// Add the output to the renderer now that it's somewhat ready.
	RrRenderer::Active->AddOutput(output);

	// Create editor now that we have something
	dusk_interface = new dusk::UserInterface(window, editor_world);
	user_interface = new ui::eventide::UserInterface(window, dusk_interface, NULL, editor_world, editor_camera);

	// Update windowing options
	window->SetWantsClipCursor(false);
	window->SetWantsHideCursor(false);
	window->SetWantsSystemCursor(true);
	window->SetZeroInputOnLoseFocus(true);

	// Everything is set up, show the window now.
	window->Show();
}

m04::editor::WindowBase::~WindowBase ( void )
{
	user_interface->RemoveReference();
	DeleteObject(user_interface);
	dusk_interface->RemoveReference();
	DeleteObject(dusk_interface);

	RrRenderer::Active->RemoveOutput(RrRenderer::Active->FindOutputWithTarget(window));
	RrRenderer::Active->RemoveWorld(editor_world);
	window->Close();
	delete window;
}

void m04::editor::WindowBase::Update ( void )
{
	// Check if we want closing
	if (window->IsDone() || window->WantsClose())
	{
		DeleteObject(this);
	}
}

RrOutputInfo& m04::editor::WindowBase::GetRenderOutput ( void )
{
	return RrRenderer::Active->GetOutput( RrRenderer::Active->FindOutputWithTarget( window ) );
}

const RrOutputInfo& m04::editor::WindowBase::GetRenderOutput ( void ) const
{
	return RrRenderer::Active->GetOutput( RrRenderer::Active->FindOutputWithTarget( window ) );
}