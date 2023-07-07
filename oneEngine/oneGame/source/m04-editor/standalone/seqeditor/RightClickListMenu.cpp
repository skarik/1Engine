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

	m04::editor::sequence::NodeRenderer* nodeVisual = dynamic_cast<m04::editor::sequence::NodeRenderer*>(m_editor->GetEventideUI()->GetMouseHit());
	bool bHoveringOnConnection = false;

	const Vector3f mousePositionUI = m_editor->GetMousePosition3D();

	// Also check which node connection we're over
	for (const auto& boardNodeEntry : m_editor->GetNodeBoardState()->nodes)
	{
		NodeRenderer* currentNodeVisual = arFastCast<NodeRenderer*>(boardNodeEntry.node->display);
		auto currentFlow = boardNodeEntry.node->sequenceInfo->view->Flow();

		for (uint i = 0; i < currentFlow.outputCount; ++i)
		{ 
			NodeRenderer* inputNodeVisual = arFastCast<NodeRenderer*>(currentNodeVisual->GetNextNode(i));

			if (currentNodeVisual && inputNodeVisual)
			{
				// Only connection is the flow connection (for now)
				Vector3f flowOutput = currentNodeVisual->GetBboxFlowOutput(i).GetCenterPoint();
				Vector3f flowInput  =   inputNodeVisual->GetBboxFlowInput().GetCenterPoint();
				core::math::Cubic flowCenterBbox = core::math::Cubic::ConstructCenterExtents((flowOutput + flowInput) * 0.5F, Vector3f(10.0F, 10.0F, 20.0F));

				if (flowCenterBbox.PointIsInside(mousePositionUI))
				{
					bHoveringOnConnection = true;
					nodeVisual = currentNodeVisual;
					m_targetConnection = i;
				}
			}
		}
	}

	// Set up own choices now:
	if (nodeVisual != nullptr && !bHoveringOnConnection)
	{
		m_mode = Mode::kOnNode;
		m_targetNode = nodeVisual->GetBoardNode();

		std::vector<std::string> l_choiceList;
		l_choiceList.push_back("Cancel");
		l_choiceList.push_back("Delete");
		l_choiceList.push_back("[Debug] Dump OSF");
		this->SetListChoices(l_choiceList);
	}
	else if (nodeVisual != nullptr && bHoveringOnConnection)
	{
		m_mode = Mode::kOnConnection;
		m_targetNode = nodeVisual->GetBoardNode();

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
		// Add all the hard-coded registered classes:
		for (auto& registryEntry : ISequenceNodeClassInfo::m_ordereredRegistry)
		{
			m_classnameListing.push_back({
				/*.isExternal = */ false,
				/*.name = */ registryEntry->m_classname
				});

			std::string choiceDisplayName = registryEntry->m_displayname;
			std::replace(choiceDisplayName.begin(), choiceDisplayName.end(), '_', '/');

			l_choiceList.push_back(choiceDisplayName.c_str());
		}
		// Add all the loaded ones from file:
		for (auto& definitionEntry : editor->GetNodeTypes())
		{
			m_classnameListing.push_back({
				/*.isExternal = */ true,
				/*.name = */ definitionEntry.first
				});
			
			std::string choiceDisplayName = std::string(definitionEntry.second->category) + "/" + definitionEntry.second->displayName.c_str();

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
			/*BoardNode* board_node = new BoardNode();
			board_node->SetPosition(GetBBox().GetCenterPoint());
			std::vector<BoardNodeGUID*> all_guids;
			std::transform(
				m_editor->GetNodeBoardState()->nodes.begin(),
				m_editor->GetNodeBoardState()->nodes.end(),
				std::back_inserter(all_guids),
				[](const NodeBoardState::NodeEntry& entry){ return entry.guid; }
				);
			board_node->editorData.guid.guidType = (BoardNodeGUIDType)m_editor->GetSEL().guid_preference;
			board_node->editorData.guid.generateDistinctTo(all_guids);

			// create a view for the board node
			//board_node->sequenceInfo.view = new m04::editor::sequence::BarebonesSequenceNodeView(&board_node->sequenceInfo);
			//board_node->sequenceInfo = m04::editor::SequenceNode::CreateWithEditorView("Generic");
			const auto& classnameInfo = m_classnameListing[choiceIndex - 1];
			if (!classnameInfo.isExternal)
			{
				board_node->sequenceInfo = m04::editor::SequenceNode::CreateWithEditorView(classnameInfo.name);
			}
			else
			{
				board_node->sequenceInfo = m04::editor::SequenceNode::CreateWithEditorView(m_editor->GetNodeTypes().find(classnameInfo.name)->second, classnameInfo.name);
			}*/

			const auto& classnameInfo = m_classnameListing[choiceIndex - 1];
			BoardNode* board_node = m_editor->GetNodeBoardState()->CreateBoardNode(classnameInfo.name);

			//
			board_node->SetPosition(GetBBox().GetCenterPoint());

			// Make sure the sequence info has a reference to editor data it needs to be aware of
			//board_node->sequenceInfo->editorData = &board_node->editorData;

			// Add node to the board state!
			m_editor->GetNodeBoardState()->AddDisplayNode(board_node);
		}
		else if (m_mode == Mode::kOnNode)
		{
			static constexpr int kChoiceId_Cancel = 0;
			static constexpr int kChoiceId_Delete = 1;
			static constexpr int kChoiceId_DebugDump = 2;
			if (choiceIndex == kChoiceId_Delete)
			{
				// get the node of the hovered
				BoardNode* board_node = m_targetNode;
				m_editor->GetNodeBoardState()->UnhookNode(board_node);
				m_editor->GetNodeBoardState()->RemoveDisplayNode(board_node);

				((NodeRenderer*)board_node->display)->Destroy();
				delete board_node;
			}
			else if (choiceIndex == kChoiceId_DebugDump)
			{
				BoardNode* board_node = m_targetNode;
				board_node->DebugDumpOSF();
			}
		}
		else if (m_mode == Mode::kOnConnection)
		{
			static constexpr int kChoiceId_Cancel = 0;
			static constexpr int kChoiceId_Unlink = 1;
			if (choiceIndex == kChoiceId_Unlink)
			{
				// Unlink the given node
				BoardNode* board_node = m_targetNode;
				board_node->sequenceInfo->nextNodes[m_targetConnection] = nullptr;
				NodeRenderer* node_display = arFastCast<NodeRenderer*>(board_node->display);
				node_display->UpdateNextNode();
			}
		}
	}
}