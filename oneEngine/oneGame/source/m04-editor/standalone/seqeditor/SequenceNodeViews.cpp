#include "SequenceNodeViews.h"
#include "SequenceNodeExternalDefinition.h"

std::unordered_map<arstring128,m04::editor::ISequenceNodeClassInfo*> m04::editor::ISequenceNodeClassInfo::m_registry;
std::vector<m04::editor::ISequenceNodeClassInfo*> m04::editor::ISequenceNodeClassInfo::m_ordereredRegistry;

m04::editor::SequenceNode::~SequenceNode ( void )
{
	if (view != NULL)
	{
		delete view;
		view = NULL;
	}
}

m04::editor::SequenceNode* m04::editor::SequenceNode::CreateWithEditorView ( const char* className )
{
	auto classInfoItr = ISequenceNodeClassInfo::m_registry.find(className);
	if (classInfoItr == ISequenceNodeClassInfo::m_registry.end())
	{
		return NULL;
	}
	else
	{
		return classInfoItr->second->CreateNew();
	}
}

m04::editor::SequenceNode* m04::editor::SequenceNode::CreateWithEditorView ( const SequenceNodeDefinition* definition, const char* className )
{
	ARCORE_ASSERT((definition != nullptr) && (className != nullptr));
	m04::editor::SequenceNode* node = new m04::editor::SequenceNode;
	node->view = new m04::editor::sequence::ExternallyDefinedSeqNodeView(node, definition);
	node->view->classname = className;
	return node;
}


void m04::editor::ISequenceNodeView::SetFlow ( const int flowOutputIndex, SequenceNode* newNodeValue )
{
	node->next = newNodeValue;
}

m04::editor::SequenceNode* m04::editor::ISequenceNodeView::GetFlow ( const int flowOutputIndex )
{
	return node->next;
}

// Barebones sequence node view definition:

DECLARE_SEQUENCENODE_CLASS(Generic, m04::editor::sequence::BarebonesSequenceNodeView);

m04::editor::sequence::BarebonesSequenceNodeView::BarebonesSequenceNodeView ( SequenceNode* in_node )
	: ISequenceNodeView(in_node)
{
	flowView.inputCount = 1;
	flowView.outputCount = 1;
	propertyViews.resize(1);
	propertyViews[0] = {"Enabled?", "enable", PropertyRenderStyle::kBoolean};
	SetProperty(0, true); // Default enabled.
}

void m04::editor::sequence::BarebonesSequenceNodeView::SetFlow ( const int flowOutputIndex, SequenceNode* newNodeValue )
{
	ISequenceNodeView::SetFlow(flowOutputIndex, newNodeValue);
}

m04::editor::SequenceNode* m04::editor::sequence::BarebonesSequenceNodeView::GetFlow ( const int flowOutputIndex )
{
	return ISequenceNodeView::GetFlow(flowOutputIndex);
}

void m04::editor::sequence::BarebonesSequenceNodeView::SetOutput ( const int outputIndex, SequenceNode* newNodeValue )
{
}
m04::editor::SequenceNode* m04::editor::sequence::BarebonesSequenceNodeView::GetOuptut ( const int outputIndex )
{
	return NULL;
}

void m04::editor::sequence::BarebonesSequenceNodeView::SetProperty ( const int propertyIndex, const float newFloatValue )
{
}

void m04::editor::sequence::BarebonesSequenceNodeView::SetProperty ( const int propertyIndex, const int newIntValue )
{
}

void m04::editor::sequence::BarebonesSequenceNodeView::SetProperty ( const int propertyIndex, const bool newBooleanValue )
{
	if (propertyIndex == 0)
	{
		node->data.GetAdd<osf::BooleanValue>("enable")->value = newBooleanValue;
	}
}

void m04::editor::sequence::BarebonesSequenceNodeView::SetProperty ( const int propertyIndex, const char* newStringValue )
{
}

void m04::editor::sequence::BarebonesSequenceNodeView::SetProperty ( const char* stringIndex, const float newFloatValue )
{
	node->data.GetConvertAdd<osf::FloatValue>(stringIndex)->value = newFloatValue;
}
void m04::editor::sequence::BarebonesSequenceNodeView::SetProperty ( const char* stringIndex, const int newIntValue )
{
	node->data.GetConvertAdd<osf::IntegerValue>(stringIndex)->value = newIntValue;
}
void m04::editor::sequence::BarebonesSequenceNodeView::SetProperty ( const char* stringIndex, const bool newBooleanValue )
{
	node->data.GetConvertAdd<osf::BooleanValue>(stringIndex)->value = newBooleanValue;
}
void m04::editor::sequence::BarebonesSequenceNodeView::SetProperty ( const char* stringIndex, const char* newStringValue )
{
	node->data.GetConvertAdd<osf::StringValue>(stringIndex)->value = newStringValue;
}
void m04::editor::sequence::BarebonesSequenceNodeView::SetProperty ( const char* stringIndex, const Vector2f& newVectorValue )
{
	node->data.GetConvertAdd<osf::StringValue>(stringIndex)->value = std::to_string(newVectorValue.x) + "\t" + std::to_string(newVectorValue.y);
}
void m04::editor::sequence::BarebonesSequenceNodeView::SetProperty ( const char* stringIndex, const Vector3f& newVectorValue )
{
	node->data.GetConvertAdd<osf::StringValue>(stringIndex)->value = std::to_string(newVectorValue.x) + "\t" + std::to_string(newVectorValue.y) + "\t" + std::to_string(newVectorValue.z);
}
void m04::editor::sequence::BarebonesSequenceNodeView::SetProperty ( const char* stringIndex, const Color& newColorValue )
{
	ColorRGBA16 intConversion = newColorValue.ToRGBA16();
	node->data.GetConvertAdd<osf::StringValue>(stringIndex)->value =
		std::to_string(intConversion.r) + "\t"
		+ std::to_string(intConversion.g) + "\t"
		+ std::to_string(intConversion.b) + "\t"
		+ std::to_string(intConversion.a);
}
float m04::editor::sequence::BarebonesSequenceNodeView::GetPropertyAsFloat ( const char* stringIndex )
{
	return node->data.GetConvertAdd<osf::FloatValue>(stringIndex)->value;
}
int m04::editor::sequence::BarebonesSequenceNodeView::GetPropertyAsInt ( const char* stringIndex )
{
	return (int)node->data.GetConvertAdd<osf::IntegerValue>(stringIndex)->value;
}
bool m04::editor::sequence::BarebonesSequenceNodeView::GetPropertyAsBool ( const char* stringIndex )
{
	return node->data.GetConvertAdd<osf::BooleanValue>(stringIndex)->value;
}
const char* m04::editor::sequence::BarebonesSequenceNodeView::GetPropertyAsString ( const char* stringIndex )
{
	return node->data.GetConvertAdd<osf::StringValue>(stringIndex)->value.c_str();
}
Vector2f m04::editor::sequence::BarebonesSequenceNodeView::GetPropertyAsVector2f ( const char* stringIndex )
{
	// Need the string tuplet from the values as well.
	std::string positionTripletString = node->data.GetConvertAdd<osf::StringValue>(stringIndex)->value.c_str();
	// Split the poisition into three string values
	auto tripletValues = core::utils::string::Split(positionTripletString, core::utils::string::kWhitespace);
	ARCORE_ASSERT(tripletValues.size() == 2);
	// Set the node's position.
	return Vector2f(std::stof(tripletValues[0]), std::stof(tripletValues[1]));
}
Vector3f m04::editor::sequence::BarebonesSequenceNodeView::GetPropertyAsVector3f ( const char* stringIndex )
{
	// Need the string tuplet from the values as well.
	std::string positionTripletString = node->data.GetConvertAdd<osf::StringValue>(stringIndex)->value.c_str();
	// Split the poisition into three string values
	auto tripletValues = core::utils::string::Split(positionTripletString, core::utils::string::kWhitespace);
	ARCORE_ASSERT(tripletValues.size() == 3);
	// Set the node's position.
	return Vector3f(std::stof(tripletValues[0]), std::stof(tripletValues[1]), std::stof(tripletValues[2]));
}
Color m04::editor::sequence::BarebonesSequenceNodeView::GetPropertyAsColor ( const char* stringIndex )
{
	// Need the string tuplet from the values as well.
	std::string colorQuadString = node->data.GetConvertAdd<osf::StringValue>(stringIndex)->value.c_str();
	// Split the poisition into three string values
	auto quadValues = core::utils::string::Split(colorQuadString, core::utils::string::kWhitespace);
	ARCORE_ASSERT(quadValues.size() == 3 || quadValues.size() == 4);
	// Set the node's position.
	return ColorRGBA16(std::stoi(quadValues[0]), std::stoi(quadValues[0]), std::stoi(quadValues[0]), (quadValues.size() == 4) ? std::stoi(quadValues[0]) : 255).ToRGBAFloat();
}

//===============================================================================================//

DECLARE_SEQUENCENODE_CLASS(MainTask, m04::editor::sequence::MainTaskSeqNodeView);
DECLARE_SEQUENCENODE_CLASS(Sidetask, m04::editor::sequence::SidetaskSeqNodeView);

m04::editor::sequence::TaskSeqNodeView::TaskSeqNodeView ( SequenceNode* in_node )
	: BarebonesSequenceNodeView(in_node)
{
	flowView.inputCount = 0;
}

//===============================================================================================//

m04::editor::sequence::ExternallyDefinedSeqNodeView::ExternallyDefinedSeqNodeView ( SequenceNode* in_node, const SequenceNodeDefinition* definition )
	: BarebonesSequenceNodeView(in_node)
{
	flowView.inputCount = 1;
	isExternalClass = true;
	externalClass = definition;

	// Loop through all the inputs and set up properties
	propertyViews.resize(externalClass->properties.size() + 1);
	for (size_t propertyIndex = 0; propertyIndex < externalClass->properties.size(); ++propertyIndex)
	{
		auto& propertyView = propertyViews[propertyIndex + 1];
		const auto& sourceProperty = externalClass->properties[propertyIndex];

		propertyView.identifier = sourceProperty.name;
		propertyView.renderstyle = sourceProperty.type;
		propertyView.label = sourceProperty.displayName;
		
		if (propertyView.renderstyle != m04::editor::PropertyRenderStyle::kArray)
		{
			SetProperty(propertyView.identifier, sourceProperty.defaultValue);
		}
		else
		{
			node->data.GetAdd<osf::ArrayValue>(propertyView.identifier);
		}
		/*if (sourceProperty.defaultValue != "")
		{
			SetProperty((int)propertyIndex, sourceProperty.defaultValue);
		}
		else
		{
			// TODO: a correct default value for the type
			SetProperty((int)propertyIndex, "");
		}*/
	}
}