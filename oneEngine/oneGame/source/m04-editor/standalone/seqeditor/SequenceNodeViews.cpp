#include "SequenceNodeViews.h"

std::unordered_map<arstring128,m04::editor::ISequenceNodeClassInfo*> m04::editor::ISequenceNodeClassInfo::m_registry;

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
	propertyViews[0] = {"Enabled?", PropertyRenderStyle::kBoolean};
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
}

void m04::editor::sequence::BarebonesSequenceNodeView::SetProperty ( const int propertyIndex, const char* newStringValue )
{
}

DECLARE_SEQUENCENODE_CLASS(MainTask, m04::editor::sequence::MainTaskSeqNodeView);
DECLARE_SEQUENCENODE_CLASS(Sidetask, m04::editor::sequence::SidetaskSeqNodeView);

m04::editor::sequence::TaskSeqNodeView::TaskSeqNodeView ( SequenceNode* in_node )
	: BarebonesSequenceNodeView(in_node)
{
	flowView.inputCount = 0;
}