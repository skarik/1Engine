#include "NodeBoardState.h"
#include "m04/eventide/elements/DefaultStyler.h"
#include "m04/eventide/elements/Button.h"

#include "./NodeBoardRenderer.h"
#include "./SequenceEditor.h"
#include "./SequenceSerialization.h"

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

void m04::editor::sequence::NodeBoardState::Save ( ISequenceSerializer* serializer )
{
	auto CollectFlowOutputs = [this](BoardNode* node, std::vector<BoardNode*>& out_list)
	{
		out_list.clear();
		for (uint outputIndex = 0; outputIndex < node->sequenceInfo->view->Flow().outputCount; ++outputIndex)
		{
			SequenceNode* outputNode = node->sequenceInfo->view->GetFlow(outputIndex);
			for (BoardNode* possibleBoardNode : nodes)
			{
				if (possibleBoardNode->sequenceInfo == outputNode)
				{
					out_list.push_back(possibleBoardNode);
					break;
				}
			}
		}
	};
	auto CollectFlowInputs = [this](BoardNode* node, std::vector<BoardNode*>& out_list)
	{
		out_list.clear();
		for (BoardNode* possibleBoardNode : nodes)
		{
			for (uint outputIndex = 0; outputIndex < possibleBoardNode->sequenceInfo->view->Flow().outputCount; ++outputIndex)
			{
				if (possibleBoardNode->sequenceInfo->view->GetFlow(outputIndex) == node->sequenceInfo)
				{
					out_list.push_back(possibleBoardNode);
				}
			}
		}
	};

	// PASS 1: DETERMINE WHICH NODES NEEDS JUMPS
	struct JumpInfo
	{
		bool needsGoto = false;
		bool isFlowStart = false;
	};
	std::unordered_map<BoardNode*, JumpInfo> l_nodeinfoTable; // Entry is true when jump
	// Initialize the table
	for (BoardNode* node : nodes)
	{
		l_nodeinfoTable[node] = {false, false};
	}

	// Start with all Task-based nodes (no flow inputs)
	for (BoardNode* possibleStartNode : nodes)
	{
		if (possibleStartNode->sequenceInfo->view->Flow().inputCount == 0)
		{
			l_nodeinfoTable[possibleStartNode].isFlowStart = true;

			// This is a flow node. Now we want to roll down the line and mark which nodes need goto's.
			std::vector<BoardNode*> l_nodeQueue;
			l_nodeQueue.push_back(possibleStartNode);

			std::vector<BoardNode*> l_nodeInputs;
			std::vector<BoardNode*> l_nodeOutputs;

			while (!l_nodeQueue.empty())
			{
				BoardNode* l_currentNode = l_nodeQueue.back();
				l_nodeQueue.pop_back();

				CollectFlowInputs(l_currentNode, l_nodeInputs);
				CollectFlowOutputs(l_currentNode, l_nodeOutputs);

				// If this current node has more than one output, its outputs cannot be linear. They must use jumps.
				if (l_nodeOutputs.size() > 1)
				{
					for (BoardNode* l_currentNodeOutput : l_nodeOutputs)
					{
						l_nodeinfoTable[l_currentNodeOutput].needsGoto = true;
					}
				}

				// If this current node has more than one input, it cnanot be linear. It must use a jump.
				if (l_nodeInputs.size() > 1)
				{
					l_nodeinfoTable[l_currentNode].needsGoto = true;
				}

				// Add in all the next flow values.
				for (BoardNode* l_currentNodeOutput : l_nodeOutputs)
				{
					l_nodeQueue.push_back(l_currentNodeOutput);
				}
			}
		}
	}

	// Now we build our output lists, beginning each list on any value in the l_nodeinfoTable that needs a label.
	std::vector<std::vector<BoardNode*>> l_outputLists;
	for (auto& nodeinfoEntry : l_nodeinfoTable)
	{
		if (nodeinfoEntry.second.needsGoto || nodeinfoEntry.second.isFlowStart)
		{
			std::vector<BoardNode*> l_nodePath;
			std::vector<BoardNode*> l_nodeQueue;
			l_nodeQueue.push_back(nodeinfoEntry.first);

			std::vector<BoardNode*> l_nodeOutputs;
			while (!l_nodeQueue.empty())
			{
				BoardNode* l_currentNode = l_nodeQueue.back();
				l_nodeQueue.pop_back();

				// Add current value to the path
				l_nodePath.push_back(l_currentNode);

				// Add outputs to consider next
				CollectFlowOutputs(l_currentNode, l_nodeOutputs);
				for (BoardNode* l_currentNodeOutput : l_nodeOutputs)
				{
					if (!l_nodeinfoTable[l_currentNodeOutput].isFlowStart && !l_nodeinfoTable[l_currentNodeOutput].needsGoto)
					{
						l_nodeQueue.push_back(l_currentNodeOutput);
					}
				}
			}

			// Add generated path to the output lists
			l_outputLists.push_back(l_nodePath);
		}
	}

	// Verify the paths we have are UNIQUE.
	for (auto currentListToCheckItr = l_outputLists.begin(); currentListToCheckItr != l_outputLists.end(); ++currentListToCheckItr)
	{
		for (auto listToCheckAgainstItr = (currentListToCheckItr + 1); listToCheckAgainstItr != l_outputLists.end(); ++listToCheckAgainstItr)
		{
			for (BoardNode* nodeInCurrentList : *currentListToCheckItr)
			{
				for (BoardNode* nodeInOtherList : *listToCheckAgainstItr)
				{
					ARCORE_ASSERT(nodeInCurrentList != nodeInOtherList);
				}
			}
		}
	}

	// Serialize each node. The view should have already handled the values.
	serializer->SerializeFileBegin();
	for (auto& outputList : l_outputLists)
	{
		serializer->SerializeListBegin();
		for (BoardNode* currentNode : outputList)
		{
			const auto& currentNodeInfo = l_nodeinfoTable[currentNode];

			if (currentNodeInfo.isFlowStart)
			{
				// Flow starts are special nodes and should be treated as such.
				serializer->SerializeStartpoint(currentNode->sequenceInfo);
			}
			else
			{
				if (currentNodeInfo.needsGoto)
				{
					// Serialize a label first.
					serializer->SerializeJumptarget(currentNode->sequenceInfo);
				}

				// Now serialize the node.
				serializer->SerializeNode(currentNode->sequenceInfo);
			}
		}
		// We're at the end of the list, serialize a "goto end"
		serializer->SerializeListEnd(outputList.back()->sequenceInfo);
	}
	// We're at the end of the file, serialize a "end"
	serializer->SerializeFileEnd();
}