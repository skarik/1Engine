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

static void SerializeOSFToJSON(nlohmann::json& data, const osf::ObjectValue* object);
static void SerializeOSFArrayToJSON(nlohmann::json& data, const osf::ArrayValue* array_object);

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
			return a.node->sequenceInfo->view->Flow().inputCount < b.node->sequenceInfo->view->Flow().inputCount;
		});

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
		}
		data_node["editor_data"] = editor_data;

		// Set up all UUIDs
		data_node["uuid"] = nullptr; // TODO

		// Save flow states
		{
			const auto& nodeFlow = nodeEntry.node->sequenceInfo->view->Flow();

			//nodeEntry.node->sequenceInfo->view->GetOuptut(

			data_node["previousNode"] = nullptr; // TODO

			if (nodeFlow.outputCount <= 1)
			{
				//data_node["nextNode"] = nodeEntry.node->sequenceInfo->view->GetOuptut(0)->view.; // TODO
			}
			else
			{
				data_node["nextNodes"] = nullptr; // TODO
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

void m04::editor::sequence::JSONSequenceBoardSerializer::DeserializeBoard ( NodeBoardState* board )
{
	nlohmann::json data;

	// Read the data in from file using C++ input
	{
		std::ifstream i (mFilename);
		i >> data;
	}
}