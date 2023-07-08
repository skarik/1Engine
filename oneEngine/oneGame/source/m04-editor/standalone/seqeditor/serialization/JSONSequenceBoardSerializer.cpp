#include "JSONSequenceBoardSerializer.h"
#include "../NodeBoardState.h"

#include <fstream>
#include <iomanip>

#include "nlohmann/json.hpp"

m04::editor::sequence::JSONSequenceBoardSerializer::JSONSequenceBoardSerializer ( const char* filename )
{
	mFilename = filename;
}

m04::editor::sequence::JSONSequenceBoardSerializer::~JSONSequenceBoardSerializer ( void )
{
}

static void SerializeOSFToJSON (nlohmann::json& data, const osf::ObjectValue* object);
static void SerializeOSFArrayToJSON (nlohmann::json& data, const osf::ArrayValue* array_object);

static void SerializeOSFToJSON (nlohmann::json& data, const osf::ObjectValue* object)
{
	for (osf::KeyValue* kv : object->values)
	{
		switch (kv->value->GetType())
		{
		case osf::ValueType::kObject:
			ARCORE_ASSERT(kv->value == kv->object);
			{
				nlohmann::json subobject;
				SerializeOSFToJSON(subobject, kv->object);
				data[kv->key.c_str()] = subobject;
			}
			break;
		case osf::ValueType::kArray:
			{
				nlohmann::json subobject;
				SerializeOSFArrayToJSON(subobject, kv->value->As<osf::ArrayValue>());
				data[kv->key.c_str()] = subobject;
			}
			break;

		case osf::ValueType::kString:
		case osf::ValueType::kMarker:
			data[kv->key.c_str()] = kv->value->As<osf::StringValue>()->value;
			break;

		case osf::ValueType::kBoolean:
			data[kv->key.c_str()] = kv->value->As<osf::BooleanValue>()->value;
			break;

		case osf::ValueType::kInteger:
			data[kv->key.c_str()] = kv->value->As<osf::IntegerValue>()->value;
			break;

		case osf::ValueType::kFloat:
			data[kv->key.c_str()] = kv->value->As<osf::FloatValue>()->value;
			break;

		default:
			ARCORE_ERROR("Not yet implemented.");
			break;
		}
	}
}

static void SerializeOSFArrayToJSON(nlohmann::json& data, const osf::ArrayValue* array_object)
{
	for (osf::BaseValue* value : array_object->values)
	{
		switch (value->GetType())
		{
		case osf::ValueType::kObject:
			{
				nlohmann::json subobject;
				SerializeOSFToJSON(subobject, value->As<osf::ObjectValue>());
				data.push_back(subobject);
			}
			break;
		case osf::ValueType::kArray:
			{
				nlohmann::json subobject;
				SerializeOSFArrayToJSON(subobject, value->As<osf::ArrayValue>());
				data.push_back(subobject);
			}
			break;

		case osf::ValueType::kString:
		case osf::ValueType::kMarker:
			data.push_back(value->As<osf::StringValue>()->value);
			break;

		case osf::ValueType::kBoolean:
			data.push_back(value->As<osf::BooleanValue>()->value);
			break;

		case osf::ValueType::kInteger:
			data.push_back(value->As<osf::IntegerValue>()->value);
			break;

		case osf::ValueType::kFloat:
			data.push_back(value->As<osf::FloatValue>()->value);
			break;

		default:
			ARCORE_ERROR("Not yet implemented.");
			break;
		}
	}
}

void m04::editor::sequence::JSONSequenceBoardSerializer::SerializeBoard ( const NodeBoardState* board )
{
	nlohmann::json data;
	
	// Resort the data nodes so that the nodes with no start are sorted to the top.
	auto board_copy = board->nodes;
	std::sort(board_copy.begin(), board_copy.end(), [](NodeBoardState::NodeEntry& a, NodeBoardState::NodeEntry& b)
		{
			return a.node->sequenceInfo->view->Flow().hasInput < b.node->sequenceInfo->view->Flow().hasInput;
		});

	// Push all nodes' editor data into their keyvalues
	for (auto& nodeEntry : board_copy)
	{
		nodeEntry.node->PushEditorData();
	}

	// Export the board as a node array
	nlohmann::json data_nodes;
	for (auto& nodeEntry : board_copy)
	{
		nlohmann::json data_node;

		// Build the structure of the node data
		nlohmann::json data_temp;
		// Save this into either the node directly, or the data structure
		nlohmann::json& data_target = false ? data_node : data_temp;
		{
			data_target["structType"] = nodeEntry.node->sequenceInfo->view->classname;
			SerializeOSFToJSON(data_target, &nodeEntry.node->sequenceInfo->data);
		}
		if (&data_target == &data_temp)
		{
			data_node["data"] = data_temp;
		}

		// Save editor state information into a separate data structure
		nlohmann::json editor_data;
		{
			// TODO: These are currently stored inside of the main OSF data.
			//		Do we need to split those into a separate structure for more reliable serialization state & error handling?
			// TODO: Need to make sure the Board State pushes editor data to the nodes before serialization.
			SerializeOSFToJSON(editor_data, &nodeEntry.node->editorData.data);
		}
		data_node["editor_data"] = editor_data;

		// Set up all UUIDs
		data_node["uuid"] = nodeEntry.node->editorData.guid.toString();

		// Save flow states
		{
			const auto& nodeFlow = nodeEntry.node->sequenceInfo->view->Flow();

			// Write out 'previousNode'
			if (nodeFlow.hasInput)
			{
				// Find the first node in our list that matches our node
				auto flowTargetItr = std::find_if(
					board_copy.begin(), board_copy.end(),
					[&nodeEntry](const NodeBoardState::NodeEntry& board_node)
					{
						auto& nextNodes = board_node.node->sequenceInfo->nextNodes;
						return std::find(nextNodes.begin(), nextNodes.end(), nodeEntry.node->sequenceInfo) != nextNodes.end();
					});

				if (flowTargetItr != board_copy.end())
				{
					auto flowTarget = flowTargetItr->node->sequenceInfo;
					if (flowTarget != nullptr)
						data_node["previousNode"] = flowTarget->editorData->guid.toString();
					else 
						data_node["previousNode"] = nullptr;
				}
				else
				{
					data_node["previousNode"] = nullptr;
				}
			}
			else
			{
				data_node["previousNode"] = nullptr;
			}

			// Write out 'nextNode'
			if (nodeFlow.outputCount <= 1)
			{
				auto flowTarget = nodeEntry.node->sequenceInfo->nextNodes.empty() ? nullptr : nodeEntry.node->sequenceInfo->nextNodes[0];

				if (flowTarget != nullptr)
					data_node["nextNode"] = flowTarget->editorData->guid.toString();
				else 
					data_node["nextNode"] = nullptr;
			}
			else
			{
				for (uint i = 0; i < nodeFlow.outputCount; ++i)
				{
					auto flowTarget = (i < nodeEntry.node->sequenceInfo->nextNodes.size()) ? nodeEntry.node->sequenceInfo->nextNodes[i] : nullptr;

					if (flowTarget != nullptr)
						data_node["nextNodes"].push_back(flowTarget->editorData->guid.toString());
					else 
						data_node["nextNodes"].push_back(nullptr);
				}
			}

			// Write out 'syncNode'
			if (nodeFlow.hasSync)
			{
				auto flowTarget = nodeEntry.node->sequenceInfo->syncNode;

				if (flowTarget != nullptr)
					data_node["syncNode"] = flowTarget->editorData->guid.toString();
				else
					data_node["syncNode"] = nullptr;
			}
			else
			{
				data_node["syncNode"] = nullptr;
			}
		}

		// Add the extra beautifiers
		data_node["structType"] = "SCTSNode";

		// Add to our node array
		data_nodes.push_back(data_node);
	}

	// Create the high level structure
	data["structType"] = "SCTSNodeTree";
	data["nodes"] = data_nodes;

	// Write the data out to file using C++ output
	{
		std::ofstream o (mFilename);
		o << std::setw(4) << data << std::endl;
	}
}

static void SerializeJSONToOSF (osf::ObjectValue* object, const nlohmann::json& data);
static void SerializeJSONArrayToOSFArray (osf::ArrayValue* array_object, const nlohmann::json& data);

static void SerializeJSONToOSF (osf::ObjectValue* object, const nlohmann::json& data)
{
	ARCORE_ASSERT(data.is_object());

	for (auto& entry : data.items())
	{
		switch (entry.value().type())
		{
		case nlohmann::json::value_t::string:
			object->GetConvertAdd<osf::StringValue>(entry.key().c_str())->value = entry.value();
			break;
		case nlohmann::json::value_t::boolean:
			object->GetConvertAdd<osf::BooleanValue>(entry.key().c_str())->value = entry.value();
			break;
		case nlohmann::json::value_t::number_float:
			object->GetConvertAdd<osf::FloatValue>(entry.key().c_str())->value = entry.value();
			break;
		case nlohmann::json::value_t::number_integer:
		case nlohmann::json::value_t::number_unsigned:
			object->GetConvertAdd<osf::IntegerValue>(entry.key().c_str())->value = entry.value();
			break;

		case nlohmann::json::value_t::object:
			{
				osf::ObjectValue* subobject = object->GetConvertAdd<osf::ObjectValue>(entry.key().c_str());
				SerializeJSONToOSF(subobject, entry.value());
			}
			break;

		case nlohmann::json::value_t::array:
			{
				osf::ArrayValue* subarray = object->GetConvertAdd<osf::ArrayValue>(entry.key().c_str());
				SerializeJSONArrayToOSFArray(subarray, entry.value());
			}
			break;

		default:
			ARCORE_ERROR("Not yet implemented.");
			break;
		}
	}
}

static void SerializeJSONArrayToOSFArray (osf::ArrayValue* array_object, const nlohmann::json& data)
{
	ARCORE_ASSERT(data.is_array());

	for (const nlohmann::json& entry : data)
	{
		switch (entry.type())
		{
		case nlohmann::json::value_t::string:
			array_object->values.push_back(new osf::StringValue);
			array_object->values.back()->As<osf::StringValue>()->value = entry;
			break;
		case nlohmann::json::value_t::boolean:
			array_object->values.push_back(new osf::BooleanValue);
			array_object->values.back()->As<osf::BooleanValue>()->value = entry;
			break;
		case nlohmann::json::value_t::number_float:
			array_object->values.push_back(new osf::FloatValue);
			array_object->values.back()->As<osf::FloatValue>()->value = entry;
			break;
		case nlohmann::json::value_t::number_integer:
		case nlohmann::json::value_t::number_unsigned:
			array_object->values.push_back(new osf::IntegerValue);
			array_object->values.back()->As<osf::IntegerValue>()->value = entry;
			break;

		case nlohmann::json::value_t::object:
			{
				osf::ObjectValue* subobject = new osf::ObjectValue;
				SerializeJSONToOSF(subobject, entry);
				array_object->values.push_back(subobject);
			}
			break;

		case nlohmann::json::value_t::array:
			{
				osf::ArrayValue* subarray = new osf::ArrayValue;
				SerializeJSONArrayToOSFArray(subarray, entry);
				array_object->values.push_back(subarray);
			}
			break;

		default:
			ARCORE_ERROR("Not yet implemented.");
			break;
		}
	}
}

void m04::editor::sequence::JSONSequenceBoardSerializer::DeserializeBoard ( NodeBoardState* board )
{
	nlohmann::json data;

	// Read the data in from file using C++ input
	{
		std::ifstream i (mFilename);
		i >> data;
	}

	struct LinkInfo
	{
		std::string nextNode;
		std::vector<std::string> nextNodes;
		std::string previousNode;
		std::string syncNode;
	};
	std::map<std::string, LinkInfo> l_linkInfoMap;

	// Start loading in the array of nodes:
	for (const nlohmann::json& node : data["nodes"])
	{
		// Load data from either the node directly, or the data structure
		const nlohmann::json& data_target = false ? node : node["data"];

		// Create new node we're going to fill:
		BoardNode* board_node = board->CreateBoardNode(((std::string)data_target["structType"]).c_str());

		SerializeJSONToOSF(&board_node->sequenceInfo->data, data_target);
		SerializeJSONToOSF(&board_node->sequenceInfo->editorData->data, node["editor_data"]);

		// Pop the editor data!
		board_node->PullEditorData();

		// Store nextNode/nextNodes, previousNode, syncNode
		LinkInfo l_linkInfo;
		if (node.find("nextNodes") != node.end() && node["nextNodes"].is_array())
		{
			for (const auto& entry : node["nextNodes"])
			{
				l_linkInfo.nextNodes.push_back(entry.is_string() ? entry : "");
			}
		}
		else if (node.find("nextNode") != node.end() && node["nextNode"].is_string())
		{
			l_linkInfo.nextNode = node["nextNode"];
		}

		if (node.find("previousNode") != node.end() && node["previousNode"].is_string())
		{
			l_linkInfo.previousNode = node["previousNode"];
		}

		if (node.find("syncNode") != node.end() && node["syncNode"].is_string())
		{
			l_linkInfo.syncNode = node["syncNode"];
		}

		l_linkInfoMap[board_node->editorData.guid.toString()] = l_linkInfo;

		// Save the node
		board->AddDisplayNode(board_node);
	}

	// With all data loaded, create the node links
	for (auto& entry : board->nodes)
	{
		BoardNode* board_node = entry.node;

		const LinkInfo& l_linkInfo = l_linkInfoMap[board_node->editorData.guid.toString()];

		// Define lambda for searching for the board nodes with matching GUID
		static auto FindSequenceNodeWithGUID = [&board](const std::string& guid_string, const BoardNodeGUIDType guid_type) -> SequenceNode*
		{
			BoardNodeGUID guid;
			guid.guidType = guid_type;
			guid.setFromString(guid_string);

			auto result = std::find_if(board->nodes.begin(), board->nodes.end(),
				[&guid](NodeBoardState::NodeEntry& entry)
				{
					return *entry.guid == guid;
				});
			
			BoardNode* board_node = (result != board->nodes.end()) ? result->node : nullptr;
			return (board_node != nullptr) ? board_node->sequenceInfo : nullptr;
		};

		const BoardNodeGUIDType l_guidType = board_node->editorData.guid.guidType;

		// Link next nodes
		if (l_linkInfo.nextNodes.empty())
		{
			if (!l_linkInfo.nextNode.empty())
				board_node->sequenceInfo->nextNodes[0] = FindSequenceNodeWithGUID(l_linkInfo.nextNode, l_guidType);
		}
		else
		{
			for (uint32_t i = 0; i < l_linkInfo.nextNodes.size(); ++i)
			{
				if (!l_linkInfo.nextNodes[i].empty())
					board_node->sequenceInfo->nextNodes[i] = FindSequenceNodeWithGUID(l_linkInfo.nextNodes[i], l_guidType);
			}
		}
	
		// Link sync node
		if (!l_linkInfo.syncNode.empty())
			board_node->sequenceInfo->syncNode = FindSequenceNodeWithGUID(l_linkInfo.syncNode, l_guidType);

		// Update the BoardNode's cached display info
		board_node->display->OnDoneLoading();
	}
}