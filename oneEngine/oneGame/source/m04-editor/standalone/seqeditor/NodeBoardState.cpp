#include "NodeBoardState.h"
#include "m04/eventide/elements/DefaultStyler.h"
#include "m04/eventide/elements/Button.h"

#include "./NodeBoardRenderer.h"
#include "./SequenceEditor.h"

m04::editor::sequence::NodeBoardState::NodeBoardState ( m04::editor::SequenceEditor* editor )
	: ui( editor->GetEventideUI() )
{
	;
}

void m04::editor::sequence::NodeBoardState::AddDisplayNode ( BoardNode* board_node )
{
	// Add new renderer for this board node
	board_node->display = new m04::editor::sequence::NodeRenderer(this, board_node, ui);

	// Add both node and node's display
	nodes.push_back(board_node);
	display.push_back(board_node->display);
	node_guids.push_back(&board_node->guid);
}

void m04::editor::sequence::NodeBoardState::RemoveDisplayNode ( BoardNode* board_node )
{
	auto nodeIter = std::find(nodes.begin(), nodes.end(), board_node);
	auto displayIter = std::find(display.begin(), display.end(), board_node->display);
	auto guidIter = std::find(node_guids.begin(), node_guids.end(), &board_node->guid);

	ARCORE_ASSERT(nodeIter != nodes.end());
	ARCORE_ASSERT(displayIter != display.end());
	ARCORE_ASSERT(guidIter != node_guids.end());

	nodes.erase(nodeIter);
	display.erase(displayIter);
	node_guids.erase(guidIter);
}