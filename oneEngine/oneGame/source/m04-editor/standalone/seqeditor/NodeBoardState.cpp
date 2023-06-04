#include "NodeBoardState.h"
#include "m04/eventide/elements/DefaultStyler.h"
#include "m04/eventide/elements/Button.h"

#include "./noderenderer/NodeBoardRenderer.h"
#include "./SequenceEditor.h"
#include "./SequenceSerialization.h"

static const char* kKeyGuid = "guid";
static const char* kKeyEditorPosition = "__editor_position";

void m04::editor::sequence::BoardNode::PushEditorData ( void )
{
	osf::BaseValue* v;

	v = sequenceInfo->data[kKeyGuid];
	if (v == nullptr)
	{
		v = sequenceInfo->data.Add(new osf::KeyValue(kKeyGuid, new osf::StringValue()))->value;
	}
	v->As<osf::StringValue>()->value = guid.toString();

	v = sequenceInfo->data[kKeyEditorPosition];
	if (v == nullptr)
	{
		v = sequenceInfo->data.Add(new osf::KeyValue(kKeyEditorPosition, new osf::StringValue()))->value;
	}
	v->As<osf::StringValue>()->value = std::to_string(position.x) + "\t" + std::to_string(position.y) + "\t" + std::to_string(position.z);
}

void m04::editor::sequence::BoardNode::FreeData ( void )
{
	if (sequenceInfo)
	{
		delete sequenceInfo;
		sequenceInfo = NULL;
	}
	display = NULL;
}

void m04::editor::sequence::BoardNode::DebugDumpOSF ( void )
{
	PushEditorData();

	printf("Dump for %p\n", sequenceInfo);
	if (sequenceInfo != NULL)
	{
		for (auto kv : sequenceInfo->data.values)
		{
			printf("\"%s\": ", kv->key.c_str());
			switch (kv->value->GetType())
			{
			case osf::ValueType::kBoolean:
				printf("%s\n", kv->value->As<osf::BooleanValue>()->value ? "true" : "false");
				break;
			case osf::ValueType::kFloat:
				printf("%f\n", kv->value->As<osf::FloatValue>()->value);
				break;
			case osf::ValueType::kInteger:
				printf("%lld\n", kv->value->As<osf::IntegerValue>()->value);
				break;
			case osf::ValueType::kString:
				printf("%s\n", kv->value->As<osf::StringValue>()->value.c_str());
				break;
			case osf::ValueType::kObject:
				printf("OBJECT [TODO]\n");
				break;
			default:
				printf("??? (Unknown type)\n");
				break;
			}
		}
		printf("Next node: %p\n", sequenceInfo->next);
	}
	else
	{
		printf("Nothing to dump!\n");
	}
}

m04::editor::sequence::NodeBoardState::NodeBoardState ( m04::editor::SequenceEditor* editor )
	: ui( editor->GetEventideUI() )
	, parent_editor(editor)
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

void m04::editor::sequence::NodeBoardState::UnhookNode ( BoardNode* board_node )
{
	m04::editor::SequenceNode* sequence_node = board_node->sequenceInfo;
	for (BoardNode* node : nodes)
	{
		// Clear Sync
		if (node->sequenceInfo->task_sync_target == sequence_node)
		{
			node->sequenceInfo->task_sync_target = NULL;
		}
		// Clear Flow
		for (uint outputIndex = 0; outputIndex < node->sequenceInfo->view->Flow().outputCount; ++outputIndex)
		{
			if (node->sequenceInfo->view->GetFlow(outputIndex) == sequence_node)
			{
				node->sequenceInfo->view->SetFlow(outputIndex, NULL);
			}
		}
		// Clear Output
		auto nodeOutputs = node->sequenceInfo->view->OutputList();
		for (uint outputIndex = 0; outputIndex < nodeOutputs.size(); ++outputIndex)
		{
			if (node->sequenceInfo->view->GetOuptut(outputIndex) == sequence_node)
			{
				node->sequenceInfo->view->SetOutput(outputIndex, NULL);
			}
		}
	}
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

	// First push all nodes' editor data into their keyvalues
	for (BoardNode* node : nodes)
	{
		node->PushEditorData();
	}

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

void m04::editor::sequence::NodeBoardState::Load ( ISequenceDeserializer* deserializer )
{
	ClearAllNodes();

	// Loop through entire file, loading labels & nodes. 

	struct LabelToNode
	{
		arstring256 label;
		m04::editor::SequenceNode* node = NULL;
		arstring256* go_to = NULL;
	};
	std::vector<LabelToNode> l_labelToNodeListing;

	// Begin deserializing
	deserializer->DeserializeFileBegin();

	// Loop through all the items and deserialize them one-by-one
	DeserializedItem	l_nextItem;
	arstring256			l_currentLabel;
	do
	{
		l_nextItem = deserializer->DeserializeNext();

		if (l_nextItem.label != NULL)
		{
			l_currentLabel = *l_nextItem.label;
		}

		if (l_nextItem.go_to != NULL)
		{
			// Swap the go_to out of the item
			arstring256* l_currentGoto = l_nextItem.go_to;
			l_nextItem.go_to = NULL;

			// Add the goto onto the list
			l_labelToNodeListing.push_back(LabelToNode{l_currentLabel, NULL, l_currentGoto});
		}

		if (l_nextItem.node != NULL)
		{
			// Create the node based on the input
			m04::editor::SequenceNode* l_currentNode = m04::editor::SequenceNode::CreateWithEditorView(l_nextItem.node->c_str()); // TODO

			// Deserialize the rest of the node info
			deserializer->DeserializeNode(l_currentNode);

			// Add to the list with the current label
			l_labelToNodeListing.push_back(LabelToNode{l_currentLabel, l_currentNode, NULL});
		}

		l_nextItem.FreeItems();
	}
	while (!l_nextItem.last_item);

	// Create all the board nodes
	for (auto iter = l_labelToNodeListing.rbegin(); iter != l_labelToNodeListing.rend(); ++iter)
	{
		LabelToNode& nodeEntry = *iter;

		if (nodeEntry.node != NULL)
		{
			// TODO: move to a boardnode factory for the actual sequence info gen
			BoardNode* board_node = new BoardNode();

			// The seqinfo has the editor view already
			board_node->sequenceInfo = nodeEntry.node;

			// Add to the board
			AddDisplayNode(board_node);
		}
	}

	// With all the items on the board, we update all the Goto's on the board.
	for (size_t listingIndex = 0; listingIndex < l_labelToNodeListing.size(); ++listingIndex)
	{
		LabelToNode& nodeEntry = l_labelToNodeListing[listingIndex];
		if (nodeEntry.node != NULL)
		{
			// Check previous linking forward:
			if (listingIndex > 0)
			{
				LabelToNode& previousNodeEntry = l_labelToNodeListing[listingIndex - 1];
				if (previousNodeEntry.node != NULL)
				{
					previousNodeEntry.node->next = nodeEntry.node;
				}
			}
			// Check goto linking:
			if (listingIndex + 1 < l_labelToNodeListing.size())
			{
				LabelToNode& nextNodeEntry = l_labelToNodeListing[listingIndex + 1];
				if (nextNodeEntry.go_to != NULL)
				{
					// Find the matching node GUID
					auto possibleFindResult = std::find_if(
						l_labelToNodeListing.begin(),
						l_labelToNodeListing.end(),
						[&nextNodeEntry](LabelToNode& possibleEntry)
					{
						return (possibleEntry.node != NULL) && (possibleEntry.node->data[kKeyGuid]->As<osf::StringValue>()->value == nextNodeEntry.go_to->c_str());
					});

					// Redirect the current node to follow the goto
					if (possibleFindResult != l_labelToNodeListing.end())
					{
						nodeEntry.node->next = possibleFindResult->node;
					}
					else
					{
						nodeEntry.node->next = NULL;
					}
				}
			}
		}
	}

	// Free the goto info
	for (auto iter = l_labelToNodeListing.rbegin(); iter != l_labelToNodeListing.rend(); ++iter)
	{
		LabelToNode& nodeEntry = *iter;

		if (nodeEntry.go_to != NULL)
		{
			delete nodeEntry.go_to;
			nodeEntry.go_to = NULL;
		}
	}

	// Now, from all the loaded nodes, pull their GUIDs and positions.
	for (BoardNode* currentNode : nodes)
	{
		currentNode->guid.setFromString(currentNode->sequenceInfo->data[kKeyGuid]->As<osf::StringValue>()->value);

		// Need the string tuplet from the values as well.
		std::string positionTripletString = currentNode->sequenceInfo->data[kKeyEditorPosition]->As<osf::StringValue>()->value;
		// Split the poisition into three string values
		auto tripletValues = core::utils::string::Split(positionTripletString, core::utils::string::kWhitespace);
		ARCORE_ASSERT(tripletValues.size() == 3);
		// Set the node's position.
		currentNode->position = Vector3f(std::stof(tripletValues[0]), std::stof(tripletValues[1]), std::stof(tripletValues[2]));

		// Update the BoardNode's cached display info
		static_cast<NodeRenderer*>(currentNode->display)->UpdateNextNode();
		static_cast<NodeRenderer*>(currentNode->display)->UpdateCachedVisualInfo();
		static_cast<NodeRenderer*>(currentNode->display)->UpdatePropertyLayout();
		static_cast<NodeRenderer*>(currentNode->display)->UpdateBboxSize();
	}
}

void m04::editor::sequence::NodeBoardState::ClearAllNodes ( void )
{
	for (BoardNode* currentNode : nodes)
	{
		// get the node of the hovered
		BoardNode* board_node = currentNode;
		UnhookNode(board_node);
		RemoveDisplayNode(board_node);

		((NodeRenderer*)board_node->display)->Destroy();
		delete board_node;
	}

	nodes.clear();
	display.clear();
	node_guids.clear();
}