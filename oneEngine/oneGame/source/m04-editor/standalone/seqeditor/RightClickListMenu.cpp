#include "RightClickListMenu.h"

#include "core/system/Screen.h"
#include "core/input/CInput.h"
#include "renderer/camera/RrCamera.h"
#include "m04/eventide/UserInterface.h"

#include "./SequenceEditor.h"
#include "./SequenceNodeViews.h"
#include "./noderenderer/NodeBoardRenderer.h"

m04::editor::sequence::RightClickListMenu::RightClickListMenu ( SequenceEditor* editor )
	: ListMenu( editor->GetEventideUI() )
	, m_editor(editor)
{
	m_mouseInteract = MouseInteract::kCatchAll;

	m04::editor::sequence::NodeRenderer* renderer = dynamic_cast<m04::editor::sequence::NodeRenderer*>(m_editor->GetEventideUI()->GetMouseHit());
	bool bHoveringOnConnection = false;

	const Vector3f mousePositionUI = m_editor->GetMousePosition3D();

	// Also check which node connection we're over
	for (const auto& boardNode : m_editor->GetNodeBoardState()->nodes)
	{
		NodeRenderer* outputNode = arFastCast<NodeRenderer*>(boardNode->display);
		NodeRenderer* inputNode = arFastCast<NodeRenderer*>(outputNode->GetNextNode());

		if (outputNode && inputNode)
		{
			// Only connection is the flow connection (for now)
			Vector3f flowOutput = outputNode->GetBboxFlowOutput(0).GetCenterPoint();
			Vector3f flowInput = inputNode->GetBboxFlowInput().GetCenterPoint();
			core::math::Cubic flowCenterBbox = core::math::Cubic::ConstructCenterExtents((flowOutput + flowInput) * 0.5F, Vector3f(10.0F, 10.0F, 20.0F));

			if (flowCenterBbox.PointIsInside(mousePositionUI))
			{
				bHoveringOnConnection = true;
				renderer = outputNode;
				m_targetConnection = 0;
			}
		}
	}

	// Set up own choices now:
	if (renderer != nullptr && !bHoveringOnConnection)
	{
		m_mode = Mode::kOnNode;
		m_targetNode = renderer->GetBoardNode();

		std::vector<std::string> l_choiceList;
		l_choiceList.push_back("Cancel");
		l_choiceList.push_back("Delete");
		l_choiceList.push_back("[Debug] Dump OSF");
		this->SetListChoices(l_choiceList);
	}
	else if (renderer != nullptr && bHoveringOnConnection)
	{
		m_mode = Mode::kOnConnection;
		m_targetNode = renderer->GetBoardNode();

		std::vector<std::string> l_choiceList;
		l_choiceList.push_back("Cancel");
		l_choiceList.push_back("Unlink");
		this->SetListChoices(l_choiceList);
	}
	else
	{
		m_mode = Mode::kEmptyBoard;

		std::vector<std::string> l_choiceList;
		l_choiceList.push_back("Cancel");
		for (auto& registryEntry : ISequenceNodeClassInfo::m_ordereredRegistry)
		{
			m_classnameListing.push_back(registryEntry->m_classname);

			std::string choiceDisplayName = registryEntry->m_displayname;
			std::replace(choiceDisplayName.begin(), choiceDisplayName.end(), '_', '/');

			l_choiceList.push_back(choiceDisplayName.c_str());
		}
		this->SetListChoices(l_choiceList);
	}
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
	Vector3f deltaToCamera = m_ui->GetCamera()->transform.position - quadParams.position;
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
		if (m_mode == Mode::kEmptyBoard)
		{
			// TODO: need a callback on somewhere else?
			// Need to signal to the board state that we want to add a new node at the given position.

			// TODO: move to a boardnode factory for the actual sequence info gen
			BoardNode* board_node = new BoardNode();
			board_node->SetPosition(GetBBox().GetCenterPoint());
			board_node->guid.generateDistinctTo(m_editor->GetNodeBoardState()->node_guids);

			// create a view for the board node
			//board_node->sequenceInfo.view = new m04::editor::sequence::BarebonesSequenceNodeView(&board_node->sequenceInfo);
			//board_node->sequenceInfo = m04::editor::SequenceNode::CreateWithEditorView("Generic");
			board_node->sequenceInfo = m04::editor::SequenceNode::CreateWithEditorView(m_classnameListing[choiceIndex - 1]);

			m_editor->GetNodeBoardState()->AddDisplayNode(board_node);
		}
		else if (m_mode == Mode::kOnNode)
		{
			if (choiceIndex == 1)
			{
				// get the node of the hovered
				BoardNode* board_node = m_targetNode;
				m_editor->GetNodeBoardState()->UnhookNode(board_node);
				m_editor->GetNodeBoardState()->RemoveDisplayNode(board_node);

				((NodeRenderer*)board_node->display)->Destroy();
				delete board_node;
			}
			else if (choiceIndex == 2)
			{
				BoardNode* board_node = m_targetNode;
				board_node->DebugDumpOSF();
			}
		}
		else if (m_mode == Mode::kOnConnection)
		{
			if (choiceIndex == 1)
			{
				// Unlink the given node
				BoardNode* board_node = m_targetNode;
				board_node->sequenceInfo->next = nullptr;
				NodeRenderer* node_display = arFastCast<NodeRenderer*>(board_node->display);
				node_display->UpdateNextNode();
			}
		}
	}
}