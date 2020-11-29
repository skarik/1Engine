#include "SequenceEditor.h"
#include "m04/eventide/UserInterface.h"
#include "m04/eventide_test/CubicLabel.h"

m04::editor::SequenceEditor::SequenceEditor ( void )
	: CGameBehavior()
{
	user_interface = new ui::eventide::UserInterface(NULL, NULL);

	test_element = new ETCubicLabel();
	test_element->SetBBox( core::math::BoundingBox( Matrix4x4(), Vector3f(0, 0, 0), Vector3f(64, 64, 4) ) );
}
m04::editor::SequenceEditor::~SequenceEditor ( void )
{
	delete_safe(test_element);
	delete_safe(user_interface);
}

void m04::editor::SequenceEditor::Update ( void )
{
	;
}
