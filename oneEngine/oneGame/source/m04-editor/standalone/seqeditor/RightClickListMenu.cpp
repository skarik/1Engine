#include "RightClickListMenu.h"

#include "core/system/Screen.h"
#include "core/input/CInput.h"
#include "renderer/camera/RrCamera.h"
#include "m04/eventide/UserInterface.h"

#include "./SequenceEditor.h"
#include "./SequenceNodeViews.h"
#include "./noderenderer/NodeBoardRenderer.h"

m04::editor::sequence::RightClickListMenu::RightClickListMenu ( SequenceEditor* editor )
	: ListMenuHierarchical( editor->GetEventideUI() )
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

	std::vector<HeirarchicalChoice> l_choiceTree;

	if (nodeVisual != nullptr && !bHoveringOnConnection)
	{
		m_mode = Mode::kOnNode;
		m_targetNode = nodeVisual->GetBoardNode();

		l_choiceTree.push_back({"Cancel"});
		l_choiceTree.push_back({"Delete"});
		l_choiceTree.push_back({"Debug"});
		l_choiceTree.back().choices = std::make_shared<std::vector<HeirarchicalChoice>>();
		l_choiceTree.back().choices->push_back({"Dump OSF"});
	}
	else if (nodeVisual != nullptr && bHoveringOnConnection)
	{
		m_mode = Mode::kOnConnection;
		m_targetNode = nodeVisual->GetBoardNode();

		l_choiceTree.push_back({"Cancel"});
		l_choiceTree.push_back({"Unlink"});
	}
	else
	{
		m_mode = Mode::kEmptyBoard;

		l_choiceTree.push_back({"Cancel"});

		l_choiceTree.push_back({"Add System Node"});
		l_choiceTree.back().choices = std::make_shared<std::vector<HeirarchicalChoice>>();
		// Add all the hard-coded registered classes:
		for (auto& registryEntry : ISequenceNodeClassInfo::m_ordereredRegistry)
		{
			// Save classname listing locally
			m_classnameListing.push_back({
				/*.isExternal = */ false,
				/*.category = */ "",
				/*.name = */ registryEntry->m_classname
				});

			// Add option to the menu
			std::string choiceDisplayName = registryEntry->m_displayname;
			std::replace(choiceDisplayName.begin(), choiceDisplayName.end(), '_', '/');

			l_choiceTree.back().choices->push_back({choiceDisplayName.c_str(), (uint8_t)(m_classnameListing.size())});
		}
		// Add all the loaded ones from file:
		for (auto& categoryEntry : editor->GetTypes())
		{
			l_choiceTree.push_back({std::string("Add ") + categoryEntry.category.c_str() + " Node"});
			l_choiceTree.back().choices = std::make_shared<std::vector<HeirarchicalChoice>>();
			for (auto& definitionEntry : categoryEntry.node_definitions)
			{
				// Save classname listing locally
				m_classnameListing.push_back({
					/*.isExternal = */ true,
					/*.category = */ categoryEntry.category,
					/*.name = */ definitionEntry.first
					});
			
				// Add option to the menu
				std::string choiceDisplayName = std::string(definitionEntry.second->category) + "/" + definitionEntry.second->displayName.c_str();

				l_choiceTree.back().choices->push_back({choiceDisplayName.c_str(), (uint8_t)(m_classnameListing.size())});
			}
		}
	}

	// Build a leveled listview based on the prefixes:

	this->SetListChoices(l_choiceTree);
}

m04::editor::sequence::RightClickListMenu::~RightClickListMenu ( void )
{
}

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

			const auto& classnameInfo = m_classnameListing[choiceIndex - 1];
			BoardNode* board_node = m_editor->GetNodeBoardState()->CreateBoardNode(classnameInfo.name, classnameInfo.category);

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
			static constexpr int kChoiceId_DebugDump = 3;
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