#include "RightClickListMenu.h"

#include "core/system/Screen.h"
#include "core/input/CInput.h"
#include "renderer/camera/RrCamera.h"
#include "m04/eventide/UserInterface.h"

#include "./SequenceEditor.h"

m04::editor::sequence::RightClickListMenu::RightClickListMenu ( SequenceEditor* editor )
	: ListMenu( editor->GetEventideUI() )
	, m_editor(editor)
{
	m_mouseInteract = MouseInteract::kCatchAll;

	// Set up own choices now:
	this->SetListChoices({
		"Cancel",
		"New Generic"
	});
}

m04::editor::sequence::RightClickListMenu::~RightClickListMenu ( void )
{
}
/*
void m04::editor::sequence::MouseGizmo::BuildMesh ( void )
{
	ParamsForQuad quadParams;
	quadParams.position = GetBBoxAbsolute().GetCenterPoint();
	quadParams.uvs = Rect(0.0F, 0.0F, 64.0F / 1024, 64.0F / 1024);
	quadParams.texture = &m_texture;

	// Move quad towards camera slightly
	Vector3f deltaToCamera = RrCamera::activeCamera->transform.position - quadParams.position;
	quadParams.position += deltaToCamera.normal();// * 2.0F;

	buildQuad(quadParams);
}

void m04::editor::sequence::MouseGizmo::OnGameFrameUpdate ( const GameFrameUpdateInput& input_frame )
{
	Vector3f centerPosition = m_ui->GetMousePosition();
	SetBBox(core::math::BoundingBox(Rotator(), centerPosition, Vector3f(10, 10, 1)));

	// Now that we're at a new position, update the menu
	RequestUpdateMesh();
}*/

void m04::editor::sequence::RightClickListMenu::OnEventMouse ( const EventMouse& mouse_event )
{
	if (GetMouseInside())
	{
		ListMenu::OnEventMouse(mouse_event);
	}
	else
	{
		if (mouse_event.type == EventMouse::Type::kClicked)
		//	|| mouse_event.type == EventMouse::Type::kReleased)
		{
			m_losingFocus = true;
		}
	}
}

void m04::editor::sequence::RightClickListMenu::OnActivated ( int choiceIndex )
{
	m_losingFocus = true; // Close on any activation

	if (choiceIndex == 0)
	{
		// Close
	}
	else
	{
		// TODO: need a callback on somewhere else?
		// Need to signal to the board state that we want to add a new node at the given position.

		// TODO: move to a boardnode factory for the actual sequence info gen
		BoardNode* board_node = new BoardNode();
		board_node->SetPosition(GetBBox().GetCenterPoint());
		board_node->guid.generate(); // TODO: make this better

		m_editor->GetNodeBoardState()->AddDisplayNode(board_node);
	}
}