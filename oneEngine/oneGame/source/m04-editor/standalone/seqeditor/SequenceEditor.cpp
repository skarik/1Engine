#include "SequenceEditor.h"
#include "core/debug/Console.h"

#include "renderer/camera/RrCamera.h"

#include "m04/eventide/UserInterface.h"
#include "m04/eventide_test/CubicLabel.h"

m04::editor::SequenceEditor::SequenceEditor ( void )
	: CGameBehavior()
{
	user_interface = new ui::eventide::UserInterface(NULL, NULL);

	test_element = new ETCubicLabel();
	test_element->SetBBox( core::math::BoundingBox( Matrix4x4(), Vector3f(100, 100, 0), Vector3f(64, 64, 4) ) );

	RrCamera* camera = new RrCamera();
	camera->SetActive();
	// Override certain camera aspects to get the right projection
	camera->transform.rotation = Rotator( 0.0, -90, -90 );
	camera->transform.position.z = 500;
	camera->zNear = 1;
	camera->zFar = 1000;
	camera->fieldOfView = 40;
}
m04::editor::SequenceEditor::~SequenceEditor ( void )
{
	delete_safe(test_element);
	delete_safe_decrement(user_interface);

	debug::Console->PrintMessage("SequenceEditor shutdown.\n");
}

void m04::editor::SequenceEditor::Update ( void )
{
	;
}
