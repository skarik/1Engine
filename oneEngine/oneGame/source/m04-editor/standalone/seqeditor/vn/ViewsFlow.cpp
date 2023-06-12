#include "ViewsFlow.h"

DECLARE_SEQUENCENODE_CLASS(VN_Wait, m04::editor::sequence::vn::WaitNodeView);

m04::editor::sequence::vn::WaitNodeView::WaitNodeView ( SequenceNode* in_node )
	: m04::editor::sequence::BarebonesSequenceNodeView(in_node)
{
	propertyViews.push_back({"Wait Time", "time", PropertyRenderStyle::kFloat});

	properties::SetProperty(&node->data, "time", 0.2F);
}

