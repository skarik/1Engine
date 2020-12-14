#include "SequenceNodeViews.h"

void m04::editor::ISequenceNodeView::SetFlow ( const int flowOutputIndex, SequenceNode* newNodeValue )
{
	node->next = newNodeValue;
}


// Barebones sequence node view definition:

m04::editor::sequence::BarebonesSequenceNodeView::BarebonesSequenceNodeView ( SequenceNode* in_node )
	: ISequenceNodeView(in_node)
{
	flowView.outputCount = 1;
	propertyViews.resize(1);
	propertyViews[0] = {"Enabled?", PropertyRenderStyle::kBoolean};
}

void m04::editor::sequence::BarebonesSequenceNodeView::SetFlow ( const int flowOutputIndex, SequenceNode* newNodeValue )
{
	ISequenceNodeView::SetFlow(flowOutputIndex, newNodeValue);
}

void m04::editor::sequence::BarebonesSequenceNodeView::SetOutput ( const int outputIndex, SequenceNode* newNodeValue )
{
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