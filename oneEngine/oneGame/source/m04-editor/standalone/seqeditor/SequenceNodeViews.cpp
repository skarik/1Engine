#include "SequenceNodeViews.h"

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
float m04::editor::sequence::BarebonesSequenceNodeView::GetPropertyAsFloat ( const char* stringIndex )
{
	return node->data.GetConvertAdd<osf::FloatValue>(stringIndex)->value;
}
int m04::editor::sequence::BarebonesSequenceNodeView::GetPropertyAsint ( const char* stringIndex )
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

DECLARE_SEQUENCENODE_CLASS(MainTask, m04::editor::sequence::MainTaskSeqNodeView);
DECLARE_SEQUENCENODE_CLASS(Sidetask, m04::editor::sequence::SidetaskSeqNodeView);

m04::editor::sequence::TaskSeqNodeView::TaskSeqNodeView ( SequenceNode* in_node )
	: BarebonesSequenceNodeView(in_node)
{
	flowView.inputCount = 0;
}